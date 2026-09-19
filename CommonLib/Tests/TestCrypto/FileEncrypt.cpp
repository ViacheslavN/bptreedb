#include <openssl/evp.h>
#include <openssl/kdf.h>
#include <openssl/core_names.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/crypto.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <cstring>
#include <cassert>

constexpr size_t BUFFER_SIZE = 10 * 1024 * 1024; // 10 MB
constexpr size_t AES_KEY_SIZE = 32;
constexpr size_t NONCE_SIZE = 12;
constexpr size_t TAG_SIZE = 16;
constexpr size_t P384_PUB_SIZE = 97;

// -------------------------------------------------------------
// Secure Memory Wrapper
// -------------------------------------------------------------
class SecureBuffer {
public:
    explicit SecureBuffer(size_t size) : data_(size) {}
    ~SecureBuffer() {
        OPENSSL_cleanse(data_.data(), data_.size());
    }

    unsigned char* data() { return data_.data(); }
    size_t size() const { return data_.size(); }

private:
    std::vector<unsigned char> data_;
};

// -------------------------------------------------------------
// RAII OpenSSL Deleters
// -------------------------------------------------------------
struct EVP_PKEY_Deleter {
    void operator()(EVP_PKEY* p) const { EVP_PKEY_free(p); }
};

struct EVP_PKEY_CTX_Deleter {
    void operator()(EVP_PKEY_CTX* p) const { EVP_PKEY_CTX_free(p); }
};

struct EVP_CIPHER_CTX_Deleter {
    void operator()(EVP_CIPHER_CTX* p) const { EVP_CIPHER_CTX_free(p); }
};

using EVP_PKEY_ptr = std::unique_ptr<EVP_PKEY, EVP_PKEY_Deleter>;
using EVP_PKEY_CTX_ptr = std::unique_ptr<EVP_PKEY_CTX, EVP_PKEY_CTX_Deleter>;
using EVP_CIPHER_CTX_ptr = std::unique_ptr<EVP_CIPHER_CTX, EVP_CIPHER_CTX_Deleter>;

// -------------------------------------------------------------
// Error Handling
// -------------------------------------------------------------
[[noreturn]] void handleErrors() {
    ERR_print_errors_fp(stderr);
    std::abort();
}

// -------------------------------------------------------------
// Key Utilities
// -------------------------------------------------------------
class ECDHKey {
public:
    static EVP_PKEY_ptr generate() {
        EVP_PKEY_CTX_ptr ctx(EVP_PKEY_CTX_new_from_name(nullptr, "EC", nullptr));
        if (!ctx) handleErrors();

        if (EVP_PKEY_keygen_init(ctx.get()) <= 0)
            handleErrors();

        OSSL_PARAM params[2];
        params[0] = OSSL_PARAM_construct_utf8_string(
            OSSL_PKEY_PARAM_GROUP_NAME,
            (char*)"secp384r1",
            0);
        params[1] = OSSL_PARAM_construct_end();

        if (EVP_PKEY_CTX_set_params(ctx.get(), params) <= 0)
            handleErrors();

        EVP_PKEY* key = nullptr;
        if (EVP_PKEY_generate(ctx.get(), &key) <= 0)
            handleErrors();

        return EVP_PKEY_ptr(key);
    }

    static EVP_PKEY_ptr fromPublicSEC1(const std::string& raw) {
        if (raw.size() != P384_PUB_SIZE)
            throw std::runtime_error("Invalid public key size");

        EVP_PKEY_CTX_ptr ctx(EVP_PKEY_CTX_new_from_name(nullptr, "EC", nullptr));
        if (!ctx) handleErrors();

        if (EVP_PKEY_fromdata_init(ctx.get()) <= 0)
            handleErrors();

        OSSL_PARAM params[3];
        params[0] = OSSL_PARAM_construct_utf8_string(
            OSSL_PKEY_PARAM_GROUP_NAME,
            (char*)"secp384r1",
            0);

        params[1] = OSSL_PARAM_construct_octet_string(
            OSSL_PKEY_PARAM_PUB_KEY,
            (void*)raw.data(),
            raw.size());

        params[2] = OSSL_PARAM_construct_end();

        EVP_PKEY* key = nullptr;
        if (EVP_PKEY_fromdata(ctx.get(), &key,
                              EVP_PKEY_PUBLIC_KEY,
                              params) <= 0)
            handleErrors();

        return EVP_PKEY_ptr(key);
    }

    static std::vector<unsigned char>
    deriveSharedSecret(EVP_PKEY* priv, EVP_PKEY* peer) {

        EVP_PKEY_CTX_ptr ctx(EVP_PKEY_CTX_new(priv, nullptr));
        if (!ctx) handleErrors();

        if (EVP_PKEY_derive_init(ctx.get()) <= 0)
            handleErrors();

        if (EVP_PKEY_derive_set_peer(ctx.get(), peer) <= 0)
            handleErrors();

        size_t secret_len = 0;
        if (EVP_PKEY_derive(ctx.get(), nullptr, &secret_len) <= 0)
            handleErrors();

        std::vector<unsigned char> secret(secret_len);

        if (EVP_PKEY_derive(ctx.get(),
                            secret.data(),
                            &secret_len) <= 0)
            handleErrors();

        secret.resize(secret_len);
        return secret;
    }
};

// -------------------------------------------------------------
// HKDF
// -------------------------------------------------------------
std::vector<unsigned char>
hkdf_sha256(const unsigned char* ikm, size_t ikm_len) {

    EVP_PKEY_CTX_ptr ctx(
        EVP_PKEY_CTX_new_from_name(nullptr, "HKDF", nullptr));

    if (!ctx) handleErrors();

    if (EVP_PKEY_derive_init(ctx.get()) <= 0)
        handleErrors();

    if (EVP_PKEY_CTX_set_hkdf_md(ctx.get(), EVP_sha256()) <= 0)
        handleErrors();

    if (EVP_PKEY_CTX_set1_hkdf_key(ctx.get(), ikm, ikm_len) <= 0)
        handleErrors();

    const char* info = "testing-encription-alg";
    if (EVP_PKEY_CTX_add1_hkdf_info(ctx.get(),
                                    (unsigned char*)info,
                                    strlen(info)) <= 0)
        handleErrors();

    std::vector<unsigned char> key(AES_KEY_SIZE);
    size_t len = key.size();

    if (EVP_PKEY_derive(ctx.get(), key.data(), &len) <= 0)
        handleErrors();

    return key;
}

// -------------------------------------------------------------
// File Encryption
// -------------------------------------------------------------
class FileEncryptor {
public:
    static void encrypt(const std::string& recipient_pub,
                        const std::string& inFile,
                        const std::string& outFile) {

        auto recipient = ECDHKey::fromPublicSEC1(recipient_pub);
        auto ephemeral = ECDHKey::generate();

        auto shared = ECDHKey::deriveSharedSecret(
            ephemeral.get(), recipient.get());

        auto aes_key = hkdf_sha256(shared.data(), shared.size());

        SecureBuffer nonce(NONCE_SIZE);
        RAND_bytes(nonce.data(), NONCE_SIZE);

        EVP_CIPHER_CTX_ptr ctx(EVP_CIPHER_CTX_new());
        EVP_EncryptInit_ex(ctx.get(),
                           EVP_aes_256_gcm(),
                           nullptr,
                           nullptr,
                           nullptr);

        EVP_CIPHER_CTX_ctrl(ctx.get(),
                            EVP_CTRL_GCM_SET_IVLEN,
                            NONCE_SIZE,
                            nullptr);

        EVP_EncryptInit_ex(ctx.get(),
                           nullptr,
                           nullptr,
                           aes_key.data(),
                           nonce.data());

        std::ifstream in(inFile, std::ios::binary);
        std::ofstream out(outFile, std::ios::binary);

        writePublicKey(ephemeral.get(), out);
        out.write((char*)nonce.data(), NONCE_SIZE);

        std::vector<unsigned char> buffer(BUFFER_SIZE);
        std::vector<unsigned char> outbuf(BUFFER_SIZE);

        while (in.good()) {
            in.read((char*)buffer.data(), buffer.size());
            std::streamsize read = in.gcount();
            if (read <= 0) break;

            int outlen = 0;
            EVP_EncryptUpdate(ctx.get(),
                              outbuf.data(),
                              &outlen,
                              buffer.data(),
                              static_cast<int>(read));

            out.write((char*)outbuf.data(), outlen);
        }

        int final_len = 0;
        EVP_EncryptFinal_ex(ctx.get(),
                            outbuf.data(),
                            &final_len);
        out.write((char*)outbuf.data(), final_len);

        unsigned char tag[TAG_SIZE];
        EVP_CIPHER_CTX_ctrl(ctx.get(),
                            EVP_CTRL_GCM_GET_TAG,
                            TAG_SIZE,
                            tag);

        out.write((char*)tag, TAG_SIZE);
    }

private:
    static void writePublicKey(EVP_PKEY* key,
                               std::ofstream& out) {

        std::vector<unsigned char> pub(P384_PUB_SIZE);
        size_t len = pub.size();

        EVP_PKEY_get_octet_string_param(
            key,
            OSSL_PKEY_PARAM_PUB_KEY,
            pub.data(),
            pub.size(),
            &len);

        out.write((char*)pub.data(), len);
    }
};

// -------------------------------------------------------------
// File Decryption
// -------------------------------------------------------------
class FileDecryptor {
public:
    static void decrypt(EVP_PKEY* recipient_priv,
                        const std::string& inFile,
                        const std::string& outFile) {

        std::ifstream in(inFile, std::ios::binary);

        std::vector<unsigned char> eph_pub(P384_PUB_SIZE);
        in.read((char*)eph_pub.data(), eph_pub.size());

        std::vector<unsigned char> nonce(NONCE_SIZE);
        in.read((char*)nonce.data(), nonce.size());

        EVP_PKEY_ptr ephemeral =
            ECDHKey::fromPublicSEC1(
                std::string((char*)eph_pub.data(),
                            eph_pub.size()));

        auto shared =
            ECDHKey::deriveSharedSecret(
                recipient_priv,
                ephemeral.get());

        auto aes_key =
            hkdf_sha256(shared.data(), shared.size());

        in.seekg(0, std::ios::end);
        size_t file_size = in.tellg();

        size_t ciphertext_size =
            file_size - P384_PUB_SIZE -
            NONCE_SIZE - TAG_SIZE;

        in.seekg(P384_PUB_SIZE + NONCE_SIZE,
                 std::ios::beg);

        std::ofstream out(outFile,
                          std::ios::binary);

        EVP_CIPHER_CTX_ptr ctx(
            EVP_CIPHER_CTX_new());

        EVP_DecryptInit_ex(ctx.get(),
                           EVP_aes_256_gcm(),
                           nullptr,
                           aes_key.data(),
                           nonce.data());

        std::vector<unsigned char> buffer(BUFFER_SIZE);
        std::vector<unsigned char> outbuf(BUFFER_SIZE);

        size_t processed = 0;

        while (processed < ciphertext_size) {
            size_t to_read =
                std::min(BUFFER_SIZE,
                         ciphertext_size - processed);

            in.read((char*)buffer.data(), to_read);
            int outlen = 0;

            EVP_DecryptUpdate(ctx.get(),
                              outbuf.data(),
                              &outlen,
                              buffer.data(),
                              to_read);

            out.write((char*)outbuf.data(), outlen);

            processed += to_read;
        }

        unsigned char tag[TAG_SIZE];
        in.read((char*)tag, TAG_SIZE);

        EVP_CIPHER_CTX_ctrl(ctx.get(),
                            EVP_CTRL_GCM_SET_TAG,
                            TAG_SIZE,
                            tag);

        if (EVP_DecryptFinal_ex(ctx.get(),
                                outbuf.data(),
                                nullptr) <= 0)
            throw std::runtime_error("Auth failed");
    }
};

// -------------------------------------------------------------
// Unit Test Harness
// -------------------------------------------------------------
int main111() {

    std::cout << "Generating recipient key...\n";
    auto recipient = ECDHKey::generate();

    // Export public key
    std::vector<unsigned char> pub(P384_PUB_SIZE);
    size_t len = pub.size();
    EVP_PKEY_get_octet_string_param(
        recipient.get(),
        OSSL_PKEY_PARAM_PUB_KEY,
        pub.data(),
        pub.size(),
        &len);

    std::string recipient_pub(
        (char*)pub.data(),
        pub.size());

    std::cout << "Encrypting...\n";
    FileEncryptor::encrypt(
        recipient_pub,
        "input.txt",
        "encrypted.bin");

    std::cout << "Decrypting...\n";
    FileDecryptor::decrypt(
        recipient.get(),
        "encrypted.bin",
        "decrypted.txt");

    std::cout << "Done.\n";

    return 0;
}