#pragma once

#pragma once

#include "../include/bptreedb.h"
#include "../include/Variant.h"

namespace bptreedb
{
	typedef std::shared_ptr<class IFieldIterator> IFieldIteratorPtr;
	typedef std::shared_ptr<class IValueField> IValueFieldPtr;

	struct IFieldIterator  
	{
	public:
		IFieldIterator() {};
		virtual ~IFieldIterator() {}
		virtual bool IsValid() = 0;
		virtual bool Next() = 0;
		virtual bool Back() = 0;
		virtual bool IsNull() = 0;
		virtual void GetVal(CVariant& val) = 0;
		virtual int64_t GetRowID() = 0;
		virtual int64_t GetAddr() const = 0;
		virtual int32_t GetPos() const = 0;

		virtual bool Copy(IFieldIteratorPtr ptrIter) = 0;
		virtual IValueFieldPtr GetField() = 0;
	};

	class IValueField 
	{
	public:
		IValueField() {}
		virtual ~IValueField(){}

		virtual void Insert(int64_t nOID, const CVariant& val, IFieldIteratorPtr ptrFromIter, IFieldIteratorPtr* ptrRetIter = nullptr) = 0;
		virtual int64_t Insert(const CVariant& val, IFieldIteratorPtr ptrFromIter, IFieldIteratorPtr* ptrRetIter = nullptr) = 0;

		virtual void Update(int64_t nRowID, const CVariant& val) = 0;
		virtual void Remove(int64_t nRowID, IFieldIteratorPtr* ptrRetIter = nullptr) = 0;
		virtual bool Find(int64_t nRowID, const CVariant& val) = 0;
		virtual void Find(int64_t nRowID, IFieldIteratorPtr retPtr, IFieldIteratorPtr* ptrFrom = nullptr) = 0;

		virtual IFieldIteratorPtr Find(int64_t nRowID, IFieldIteratorPtr pFrom) = 0;
		virtual IFieldIteratorPtr Upper_bound(int64_t nRowID, IFieldIteratorPtr pFrom) = 0;
		virtual IFieldIteratorPtr Lower_bound(int64_t nRowID, IFieldIteratorPtr pFrom) = 0;
		virtual IFieldIteratorPtr Begin() = 0;
		virtual IFieldIteratorPtr Last() = 0;
		virtual void Commit() = 0;
	};


}