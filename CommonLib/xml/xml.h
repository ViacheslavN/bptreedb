#pragma once

#include "CommonLib.h"
#include "../str/convertToString.h"
#include "../data/ByteArray.h"
#include "../stream/stream.h"

namespace CommonLib
	{
		namespace xml
		{

			typedef std::shared_ptr<class IXMLNode> IXMLNodePtr;
			typedef std::shared_ptr<class IXMLDoc> IXMLDocPtr;

			class IXMLNode
			{
			public:

				IXMLNode() {}
				virtual ~IXMLNode() {}

				virtual IXMLNodePtr	CreateChildNode(const std::string& name) = 0;
				virtual uint32_t	GetChildCnt() const = 0;
				virtual IXMLNodePtr	GetChild(uint32_t nIndex) const = 0;
				virtual IXMLNodePtr	GetChild(const std::string& name) const = 0;
				virtual std::vector<IXMLNodePtr> GetChilds(const std::string& name) const = 0;
				virtual bool	IsChildExists(const std::string& name) const = 0;



				virtual IXMLNodePtr	GetParent() const = 0;


				virtual const std::string& GetName() const = 0;
				virtual void        SetName(const std::string& name) = 0;
				virtual const std::string& GetText() const = 0;
				virtual void        SetText(const std::string& textUtf8) = 0;
				virtual void        SetText(const std::wstring&) = 0;


				virtual const std::string&   GetCDATA() const = 0;
				virtual void          SetCDATA(const std::string& cdata) = 0;

				virtual	void GetBlobCDATA(Data::TVecBuffer& data) const = 0;
				virtual void SetBlobCDATA(const byte_t* data, size_t size) = 0;

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


				virtual int16_t	GetPropertyInt16(const std::string& name) const = 0;
				virtual uint16_t GetPropertyInt16U(const std::string& name) const = 0;
				virtual int32_t	 GetPropertyInt32(const std::string& name) const = 0;
				virtual uint32_t GetPropertyInt32U(const std::string& name) const = 0;
				virtual int64_t	GetPropertyInt64(const std::string& name) const = 0;
				virtual uint64_t GetPropertyIntU64(const std::string& name) const = 0;
				virtual double  GetPropertyDouble(const std::string& name) const = 0;
				virtual bool  GetPropertyBool(const std::string& name) const = 0;
				virtual std::string GetPropertyString(const std::string& name) const = 0;
				virtual std::wstring GetPropertyWString(const std::string& name) const = 0;

			};

			class IXMLDoc
			{
			public:
				IXMLDoc() {}
				virtual ~IXMLDoc() {}

				virtual void  Open(const std::string& xml) = 0;
				virtual void  Open(CommonLib::IReadStream* pStream) = 0;

				virtual void  Save(const std::string& xml) = 0;
				virtual void  Save(CommonLib::IWriteStream* pStream) = 0;

				virtual IXMLNodePtr	 GetNodes() const = 0;
				virtual void Clear() = 0;

			};

		}
	}
