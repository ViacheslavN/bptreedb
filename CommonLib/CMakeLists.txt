cmake_minimum_required(VERSION 3.17)
project(CommonLib)

find_library(PTHREAD_LIBRARY pthread)
find_package(OpenSSL REQUIRED)


set(CMAKE_CXX_STANDARD 17)
set(COMMON_SOURCE_FILES
        stdafx.cpp

		alloc/alloc.h
		alloc/simpleAlloc.cpp
		alloc/simpleAlloc.h
		alloc/stl_alloc.h

		CommonLib.h
		CommonLib_lib.h

		compress/EntropyCoders/ArithmeticCoder.h
		compress/EntropyCoders/BaseACCoder.h
		compress/EntropyCoders/RangeCoder.h

		compress/zlib/adler32.c
		compress/zlib/compress.c
		compress/zlib/crc32.c
		compress/zlib/crc32.h
		compress/zlib/deflate.c
		compress/zlib/deflate.h
		compress/zlib/gzclose.c
		compress/zlib/gzguts.h
		compress/zlib/gzlib.c
		compress/zlib/gzread.c
		compress/zlib/gzwrite.c
		compress/zlib/infback.c
		compress/zlib/inffast.c
		compress/zlib/inffast.h
		compress/zlib/inffixed.h
		compress/zlib/inflate.c
		compress/zlib/inflate.h
		compress/zlib/inftrees.c
		compress/zlib/inftrees.h
		compress/zlib/trees.c
		compress/zlib/trees.h
		compress/zlib/uncompr.c
		compress/zlib/ZCompressStream.cpp
		compress/zlib/ZCompressStream.h
		compress/zlib/zconf.h
		compress/zlib/ZDecompressStream.cpp
		compress/zlib/ZDecompressStream.h
		compress/zlib/zlib.h
		compress/zlib/ZlibUtil.cpp
		compress/zlib/ZlibUtil.h
		compress/zlib/ZStream.cpp
		compress/zlib/ZStream.h
		compress/zlib/zutil.c
		compress/zlib/zutil.h
#PNG
		ThirdParty/Png/png.c
		ThirdParty/Png/pngerror.c
		ThirdParty/Png/pngget.c
		ThirdParty/Png/pngmem.c
		ThirdParty/Png/pngpread.c
		ThirdParty/Png/pngread.c
		ThirdParty/Png/pngrio.c
		ThirdParty/Png/pngrtran.c
		ThirdParty/Png/pngrutil.c
		ThirdParty/Png/pngset.c
		#ThirdParty/Png/pngtest.c
		ThirdParty/Png/pngtrans.c
		ThirdParty/Png/pngwio.c
		ThirdParty/Png/pngwrite.c
		ThirdParty/Png/pngwtran.c
		ThirdParty/Png/pngwutil.c

		compress/zstd/ZstdCompressionStream.cpp
		compress/zstd/ZstdCompressionStream.h
		compress/zstd/ZstdDecompressionStream.cpp
		compress/zstd/ZstdDecompressionStream.h

		crypto/AESBase.cpp
		crypto/AESBase.h
		crypto/crypto.h
		crypto/CryptoData.h
		crypto/EmptyDataCipher.cpp
		crypto/EmptyDataCipher.h
		crypto/IDataCipher.h

		crypto/openssllib/AESCipherSSL.cpp
		crypto/openssllib/AESCipherSSL.h
		crypto/openssllib/CryptoFactoryOpenSSL.cpp
		crypto/openssllib/CryptoFactoryOpenSSL.h
		crypto/openssllib/EnvCipher.cpp
		crypto/openssllib/EnvCipher.h
		crypto/openssllib/EvpCipherCtx.cpp
		crypto/openssllib/EvpCipherCtx.h
		crypto/openssllib/KeyGeneratorSSL.cpp
		crypto/openssllib/KeyGeneratorSSL.h
		crypto/openssllib/OpensslExc.cpp
		crypto/openssllib/OpensslExc.h
		crypto/openssllib/OpenSSlLib.cpp
		crypto/openssllib/OpenSSlLib.h
		crypto/openssllib/RandomGeneratorSSL.cpp
		crypto/openssllib/RandomGeneratorSSL.h

		data/ByteArray.cpp
		data/ByteArray.h

		crypto/XTSBlockChiper.cpp
		crypto/XTSBlockChiper.h

		data/ByteArray.cpp
		data/ByteArray.h

		date/DateUtil.cpp
		date/DateUtil.h

		exception/exc_base.cpp
		exception/exc_base.h

		filesystem/File.cpp
		filesystem/File.h
		filesystem/filesystem.h
		filesystem/PathUtils.cpp
		filesystem/PathUtils.h

		GoogleLogs/Logging.h

		guid/guid.h

		log/ConsoleLogWriter.cpp
		log/ConsoleLogWriter.h
		log/FileAsyncLogger.cpp
		log/FileAsyncLogger.h
		log/log.h
		log/LogFileWriterBase.cpp
		log/LogFileWriterBase.h
		log/logger.cpp
		log/logger.h
		log/LogInfo.h
		log/LogLinePerformer.cpp
		log/LogLinePerformer.h

		network/AcceptedSocket.cpp
		network/AcceptedSocket.h
		network/AddrInfo.cpp
		network/AddrInfo.h
		network/BaseSocketIP.cpp
		network/BaseSocketIP.h
		network/ClientSocket.cpp
		network/ClientSocket.h
		network/CloseConnectionExc.h
		network/NetworkStreamIoDev.cpp
		network/NetworkStreamIoDev.h
		network/OperationTimeoutExc.h
		network/portable_endian.h
		network/ServerSocket.cpp
		network/ServerSocket.h
		network/SocketAddr.cpp
		network/SocketAddr.h
		network/SocketUtil.cpp
		network/SocketUtil.h

		perf/PerfCount.cpp
		perf/PerfCount.h

		stdafx.cpp
		stdafx.h

		compress/zlib/ZCompressStream.cpp
		compress/zlib/ZCompressStream.h
		compress/zlib/ZDecompressStream.cpp
		compress/zlib/ZDecompressStream.h
		compress/zlib/ZlibUtil.cpp
		compress/zlib/ZlibUtil.h
		compress/zlib/ZStream.cpp
		compress/zlib/ZStream.h

		compress/zstd/ZstdCompressionStream.cpp
		compress/zstd/ZstdCompressionStream.h
		compress/zstd/ZstdDecompressionStream.cpp
		compress/zstd/ZstdDecompressionStream.h

		crypto/AESBase.cpp
		crypto/AESBase.h

		crypto/openssllib/AESCipherSSL.cpp
		crypto/openssllib/AESCipherSSL.h
		crypto/openssllib/EnvCipher.cpp
		crypto/openssllib/EnvCipher.h
		crypto/openssllib/EvpCipherCtx.cpp
		crypto/openssllib/EvpCipherCtx.h
		crypto/openssllib/KeyGeneratorSSL.cpp
		crypto/openssllib/KeyGeneratorSSL.h
		crypto/openssllib/OpensslExc.cpp
		crypto/openssllib/OpensslExc.h
		crypto/openssllib/RandomGeneratorSSL.cpp
		crypto/openssllib/RandomGeneratorSSL.h

		crypto/XTSBlockChiper.cpp
		crypto/XTSBlockChiper.h

		exception/exc_base.cpp
		exception/exc_base.h

		filesystem/File.cpp
		filesystem/File.h

		log/ConsoleLogWriter.cpp
		log/ConsoleLogWriter.h
		log/FileAsyncLogger.cpp
		log/FileAsyncLogger.h
		log/log.h
		log/LogFileWriterBase.cpp
		log/LogFileWriterBase.h
		log/logger.cpp
		log/logger.h
		log/LogInfo.h
		log/LogLinePerformer.cpp
		log/LogLinePerformer.h

		network/NetworkStreamIoDev.cpp
		network/NetworkStreamIoDev.h
		network/SocketAddr.cpp
		network/SocketAddr.h

		str/convertToString.cpp
		str/convertToString.h
		str/multibyte.cpp
		str/multibyte.h
		str/str.cpp
		str/str.h
		str/StringEncoding.cpp
		str/StringEncoding.h
		str/StringTemplateArg.h
		str/StrUtils.cpp
		str/StrUtils.h

		stream/BitStream.cpp
		stream/BitStream.h
		stream/FileStream.cpp
		stream/FileStream.h
		stream/FixedBitStream.cpp
		stream/FixedBitStream.h
		stream/FixedMemoryStream.cpp
		stream/FixedMemoryStream.h
		stream/io.h
		stream/MemoryStream.cpp
		stream/MemoryStream.h
		stream/MemoryStreamBuffer.cpp
		stream/MemoryStreamBuffer.h
		stream/stream.cpp
		stream/stream.h
		stream/StreamBaseEmpty.h
		stream/StreamIO.cpp
		stream/StreamIO.h
		stream/WriteBitStream.cpp
		stream/WriteBitStream.h

		synch/SyncQueue.h
		synch/thread.cpp
		synch/thread.h
		synch/WaitForGraph
		synch/WaitForGraph/SyncObject.h
		synch/Semaphore.h
		synch/Event.h

		system/DynamicLib.h

		utils/CRC.cpp
		utils/CRC.h
		utils/ObjHolder.h

		system/Environment.cpp
		stream/StdLineInOut.cpp
		stream/StdLineInOut.h


		SpatialData/BoundaryBox.h
		SpatialData/BoundaryBox.cpp

		xml/XMLDoc.cpp
		xml/XMLNode.cpp
		xml/XMLUtils.cpp

		Serialize/SerializeObj.h
		Serialize/SerializeXML.h
		Serialize/SerializeXML.cpp
		data/PodVector.h
		SpatialData/GeoShape.h
		SpatialData/GeoShape.cpp
		SpatialData/IGeoShape.h
		SpatialData/IGeoShape.cpp
		data/blob.h
		data/blob.cpp
		SpatialData/Units.h
		SpatialData/Units.cpp
		utils/delegate.h
		utils/PropertySet.h
		utils/PropertySet.cpp
		variant/Variant.h
		variant/Variant.cpp
		variant/VariantTypeList.h
		variant/VariantVisitor.h
		variant/VariantVisitor.cpp
		filesystem/Directory.h
		filesystem/Directory.cpp
)

set(STATIC_LIBS
        -lpthread
        )

set(POSIX_SOURCE_FILES
		exception/PosixExc.cpp
		exception/PosixExc.h
		filesystem/FilePosix.cpp
		filesystem/FilePosix.h
		filesystem/filesystemPosix.cpp
		synch/PosixThread.cpp
		synch/PosixThread.h
		guid/GuidLin.cpp
		guid/GuidLin.h
		perf/lin/PerformanceTimer.cpp
		perf/lin/PerformanceTimer.h
		system/linux/DynamicLibLin.cpp
		system/linux/DynamicLibLin.h
		system/linux/Process.cpp
		system/linux/Process.h
		system/linux/LinPipe.cpp
		system/linux/LinPipe.h
		system/linux/SignalHandler.cpp
		system/linux/SignalHandler.h
		system/linux/PipeWriteRead.cpp
		system/linux/PipeWriteRead.h
		system/linux/ProcessWaiter.cpp
		system/linux/ProcessWaiter.h

		guid/libuuid/clear.c
		guid/libuuid/compare.c
		guid/libuuid/copy.c
		guid/libuuid/gen_uuid.c
		guid/libuuid/isnull.c
		guid/libuuid/pack.c
		guid/libuuid/parse.c
		guid/libuuid/unpack.c
		guid/libuuid/unparse.c
		guid/libuuid/uuid_time.c



)

set(WINDOWS_SOURCE_FILES
		crypto/winapi/aes/AESCipher.cpp
		crypto/winapi/aes/AESCipher.h
		crypto/winapi/CryptoAlgoritmProvider.cpp
		crypto/winapi/CryptoAlgoritmProvider.h
		crypto/winapi/CryptoFactory.cpp
		crypto/winapi/CryptoFactory.h
		crypto/winapi/KeyGenerator.cpp
		crypto/winapi/KeyGenerator.h

		crypto/winapi/random/RandomGenerator.cpp
		crypto/winapi/random/RandomGenerator.h
		crypto/winapi/SymmetricKey.cpp
		crypto/winapi/SymmetricKey.h

		exception/ComExc.cpp
		exception/ComExc.h
		exception/WinExc.cpp
		exception/WinExc.h
		exception/WinNtExc.cpp
		exception/WinNtExc.h
		exception/WinSockExc.cpp
		exception/WinSockExc.h

		filesystem/filesystemWin.cpp
		filesystem/FileWin.cpp
		filesystem/FileWin.h

		guid/GuidWin.cpp
		guid/GuidWin.h

		perf/win/PerformanceTimer.cpp
		perf/win/PerformanceTimer.h

		sqlitelib/CryptoContext.cpp
		sqlitelib/CryptoContext.h
		sqlitelib/Database.h
		sqlitelib/sqlite
		sqlitelib/sqlite/CryptoContextHolder.cpp
		sqlitelib/sqlite/CryptoContextHolder.h
		sqlitelib/sqlite/Database.cpp
		sqlitelib/sqlite/Database.h
		sqlitelib/sqlite/db
		sqlitelib/sqlite/db/sqlite3.c
		sqlitelib/sqlite/db/sqlite3.h
		sqlitelib/sqlite/db/sqlite3ext.h
		sqlitelib/sqlite/filesys
		sqlitelib/sqlite/filesys/SqliteVfs.cpp
		sqlitelib/sqlite/filesys/SqliteVfs.h
		sqlitelib/sqlite/filesys/VfsIO.cpp
		sqlitelib/sqlite/filesys/VfsIO.h
		sqlitelib/sqlite/filesys/VfsProxyIo.cpp
		sqlitelib/sqlite/filesys/VfsProxyIo.h
		sqlitelib/sqlite/SqliteApi.cpp
		sqlitelib/sqlite/SqliteApi.h
		sqlitelib/sqlite/SqliteExc.cpp
		sqlitelib/sqlite/SqliteExc.h
		sqlitelib/sqlite/Statement.cpp
		sqlitelib/sqlite/Statement.h
		sqlitelib/sqlite/Transaction.cpp
		sqlitelib/sqlite/Transaction.h

		synch/WinThread.cpp
		synch/WinThread.h

		system/win/DynamicLibWin.cpp
		system/win/DynamicLibWin.h
		system/win/Handle.cpp
		system/win/Handle.h
		system/win/Process.cpp
		system/win/Process.h
		targetver.h
		system/win/WinPipe.cpp
		system/win/WinPipe.h
		system/win/PipeWriteRead.cpp
		system/win/PipeWriteRead.h
		filesystem/DirectoryWin.h
		filesystem/DirectoryWin.cpp

)

IF (WIN32)
        set(SOURCE_FILES ${COMMON_SOURCE_FILES} ${WINDOWS_SOURCE_FILES})
		add_definitions(-DUNICODE -D_UNICODE -DSQLITE_ENABLE_RTREE)
ELSE()
        set(SOURCE_FILES ${COMMON_SOURCE_FILES} ${POSIX_SOURCE_FILES}
				)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc -fPIC -fvisibility=hidden")
		add_definitions(-DSQLITE_ENABLE_RTREE)
ENDIF()

if (WIN32)

	set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -MTd")
	set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -MTd")
	set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -MT")
	set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -MT")

endif()



add_library(CommonLib ${SOURCE_FILES})
target_link_libraries(CommonLib PRIVATE ${STATIC_LIBS})
target_include_directories(CommonLib PUBLIC .)
target_include_directories(CommonLib PUBLIC ./compress/zlib)
target_link_libraries(CommonLib PRIVATE OpenSSL::SSL OpenSSL::Crypto)




#set(SOURCE_FILES
#       Tests/TestLog/TestLog.cpp
#		Tests/TestLog/TestZlib.cpp
#        )

#set(CMAKE_CXX_STANDARD 14)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")

#add_compile_options(-pthread)
#add_executable(TestLog ${SOURCE_FILES})
#target_link_libraries(TestLog PRIVATE ${STATIC_LIBS})
#target_link_libraries(TestLog PRIVATE  CommonLib)
#target_include_directories(TestLog PRIVATE ../../../)

add_subdirectory(Tests)
