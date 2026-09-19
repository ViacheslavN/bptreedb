#pragma once
#include "../CommonLib.h"
#include "../data/ByteArray.h"
#include "../guid/guid.h"

 namespace CommonLib
 {
     typedef std::shared_ptr<class ISerializeObj> ISerializeObjPtr;

     class ISerializeObj
     {
     public:
         ISerializeObj() {}
         virtual ~ISerializeObj() {}

         virtual const std::string& GetName() const = 0;
         virtual void       SetName(const std::string&  name) = 0;
         virtual ISerializeObjPtr   CreateChildNode(const std::string& name) const = 0;
         virtual uint32_t	GetChildCnt() const = 0;
         virtual ISerializeObjPtr	GetChild(uint32_t nIndex) const = 0;
         virtual ISerializeObjPtr	GetChild(const std::string& name) const = 0;
         virtual std::vector<ISerializeObjPtr> GetChilds(const std::string& name) const = 0;
         virtual bool	IsChildExists(const std::string& name) const = 0;

         virtual void GetBlob(Data::TVecBuffer& data) const = 0;
         virtual void SetBlob(const byte_t* data, size_t size) = 0;

         virtual void AddPropertyInt16(const  std::string& name, int16_t value) = 0;
         virtual void AddPropertyInt16U(const std::string& name, uint16_t value) = 0;
         virtual void AddPropertyInt32(const std::string& name, int32_t value) = 0;
         virtual void AddPropertyInt32U(const std::string& name, uint32_t value) = 0;
         virtual void AddPropertyInt64(const std::string& name, int64_t value) = 0;
         virtual void AddPropertyIntU64(const std::string& name, uint64_t value) = 0;
         virtual void AddPropertyDouble(const std::string& name, double value) = 0;
         virtual void AddPropertyBool(const std::string& name, bool value) = 0;
         virtual void AddPropertyString(const std::string& name, const std::string& valueUtf8) = 0;
         virtual void AddPropertyWString(const std::string& name, const std::wstring& value) = 0;
         virtual void AddPropertyGuid(const std::string& name, const CommonLib::CGuid& value) = 0;


         virtual bool IsPropertyExists(const std::string& name) const = 0;
         virtual const std::string& GetProperty(const std::string& name) const = 0;
         virtual uint32_t GetPropertyCnt() const = 0;
         virtual const std::string&  GetProperty(uint32_t nIndex) const = 0;

         virtual int16_t	GetPropertyInt16(const std::string& name, int16_t defValue) const = 0;
         virtual uint16_t GetPropertyInt16U(const std::string& name, uint16_t defValue) const = 0;
         virtual int32_t	 GetPropertyInt32(const std::string& name, int32_t defValue) const = 0;
         virtual uint32_t GetPropertyInt32U(const std::string& name, uint32_t defValue) const = 0;
         virtual int64_t	GetPropertyInt64(const std::string& name, int64_t defValue) const = 0;
         virtual uint64_t GetPropertyIntU64(const std::string& name, uint64_t defValue) const = 0;
         virtual double  GetPropertyDouble(const std::string& name, double defValue) const = 0;
         virtual bool  GetPropertyBool(const std::string& name, bool defValue) const = 0;
         virtual std::string GetPropertyString(const std::string& name, const  std::string& defValueUtf8) const = 0;
         virtual std::wstring GetPropertyWString(const std::string& name, const  std::wstring& defValue) const = 0;
         virtual CommonLib::CGuid GetPropertyGuid(const std::string& name, const CommonLib::CGuid& value) const = 0;


         virtual int16_t	GetPropertyInt16(const std::string& name) const = 0;
         virtual uint16_t GetPropertyInt16U(const std::string& name) const = 0;
         virtual int32_t	 GetPropertyInt32(const std::string& name) const = 0;
         virtual uint32_t GetPropertyInt32U(const std::string& name) const = 0;
         virtual int64_t	GetPropertyInt64(const std::string& name) const = 0;
         virtual uint64_t GetPropertyIntU64(const std::string& name) const = 0;
         virtual double  GetPropertyDouble(const std::string& name) const = 0;
         virtual bool  GetPropertyBool(const std::string& name) const = 0;
         virtual std::string GetPropertyString(const std::string& name) const = 0;
         virtual CommonLib::CGuid GetPropertyGuid(const std::string& name) const = 0;
     };


     class ISerialize
     {
     public:
         ISerialize() {}
         virtual ~ISerialize() {}

         virtual void Save(CommonLib::ISerializeObjPtr pObj) const = 0;
         virtual void Load(CommonLib::ISerializeObjPtr pObj) = 0;

     };
 }