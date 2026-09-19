#pragma once

#include "CommonLib.h"
#include "xml.h"
#include "../str/convertToString.h"

namespace CommonLib
	{
		namespace xml
		{
			class CXMLNode : public IXMLNode, public std::enable_shared_from_this<CXMLNode>
			{
			public:
				CXMLNode(IXMLNodePtr ptrParent, const std::string& name = "");
				virtual ~CXMLNode();

				virtual IXMLNodePtr			   CreateChildNode(const std::string& name);
				virtual uint32_t			   GetChildCnt() const;
				virtual IXMLNodePtr			   GetChild(uint32_t nIndex) const;
				virtual IXMLNodePtr			   GetChild(const std::string& name) const;
				virtual std::vector<IXMLNodePtr> GetChilds(const std::string& name) const;
				virtual bool	IsChildExists(const std::string& name) const;
				virtual IXMLNodePtr			   GetParent() const;
				
				virtual const std::string& GetName() const;
				virtual void        SetName(const std::string& name);

				virtual const std::string&  GetText() const;
				virtual void  SetText(const std::string& textUtf8);
				virtual void  SetText(const std::wstring& unicode);

				virtual const std::string&  GetCDATA() const;
				virtual void  SetCDATA(const  std::string& cdata);

				virtual	void GetBlobCDATA(CommonLib::Data::TVecBuffer& data) const;
				virtual void SetBlobCDATA(const byte_t* data, size_t size);

				virtual void AddPropertyInt16(const  std::string& name, int16_t value);
				virtual void AddPropertyInt16U(const std::string& name, uint16_t value);
				virtual void AddPropertyInt32(const std::string& name, int32_t value);
				virtual void AddPropertyInt32U(const std::string& name, uint32_t value);
				virtual void AddPropertyInt64(const std::string& name, int64_t value);
				virtual void AddPropertyIntU64(const std::string& name, uint64_t value);
				virtual void AddPropertyDouble(const std::string& name, double value);
				virtual void AddPropertyBool(const std::string& name, bool value);
				virtual void AddPropertyString(const std::string& name, const std::string& valueUtf8);
				virtual void AddPropertyWString(const std::string& name, const std::wstring& value);


				virtual bool IsPropertyExists(const std::string& name) const;
				virtual const std::string& GetProperty(const std::string& name) const;
				virtual uint32_t GetPropertyCnt() const;
				virtual const std::string&  GetProperty(uint32_t nIndex) const;

				virtual int16_t	GetPropertyInt16(const std::string& name, int16_t defValue) const;
				virtual uint16_t GetPropertyInt16U(const std::string& name, uint16_t defValue) const;
				virtual int32_t	 GetPropertyInt32(const std::string& name, int32_t defValue) const;
				virtual uint32_t GetPropertyInt32U(const std::string& name, uint32_t defValue) const;
				virtual int64_t	GetPropertyInt64(const std::string& name, int64_t defValue) const;
				virtual uint64_t GetPropertyIntU64(const std::string& name, uint64_t defValue) const;
				virtual double  GetPropertyDouble(const std::string& name, double defValue) const;
				virtual bool  GetPropertyBool(const std::string& name, bool defValue) const;
				virtual std::string GetPropertyString(const std::string& name, const  std::string& defValue) const;
				virtual std::wstring GetPropertyWString(const std::string& name, const  std::wstring& defValue) const;


				virtual int16_t	GetPropertyInt16(const std::string& name) const;
				virtual uint16_t GetPropertyInt16U(const std::string& name) const;
				virtual int32_t	 GetPropertyInt32(const std::string& name) const;
				virtual uint32_t GetPropertyInt32U(const std::string& name) const;
				virtual int64_t	GetPropertyInt64(const std::string& name) const;
				virtual uint64_t GetPropertyIntU64(const std::string& name) const;
				virtual double  GetPropertyDouble(const std::string& name) const;
				virtual bool  GetPropertyBool(const std::string& name) const;
				virtual std::string GetPropertyString(const std::string& name) const;
				virtual std::wstring GetPropertyWString(const std::string& name) const;

				void Save(CommonLib::IWriteStream *pSteam);
			private:
				void SetParent(IXMLNodePtr ptrParent);

				template  <class T> 
				void AddProperyT(const std::string& name, const T& value)
				{
					AddPropertyString(name, CommonLib::str_utils::AStrFrom(value));
				}

				template  <class T>
				T GetProperyT(const std::string& name) const
				{
					const std::string& val = GetProperty(name);
					return CommonLib::str_utils::AStr2TVal<T>(val);

				}

				template  <class T>
				T GetProperyT(const std::string& name, const T& defValue) const
				{
					if (!IsPropertyExists(name))
						return defValue;

					return GetProperyT<T>(name);

				}			

			private:

                std::string m_name;
                std::string m_textUtf8;
                std::string m_caData;

				std::weak_ptr<IXMLNode> m_ptrParent;
				typedef std::vector<IXMLNodePtr> TNodes;
				typedef std::multimap<std::string, uint32_t> TNodesByName;

				typedef std::vector<std::pair<std::string, std::string> > TVecProp;
				typedef std::map<std::string, uint32_t> TPropByName;

				TNodes m_Nodes;
				TNodesByName m_NodeByName;

				TVecProp m_Props;
				TPropByName m_PropsByName;
			};
		}
	}
