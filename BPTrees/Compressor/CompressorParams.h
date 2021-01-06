
#pragma once

#include "../../bptreedb.h"
#include "../../../CommonLib/stream/MemoryStream.h"
#include "../../../CommonLib/stream/FixedMemoryStream.h"

namespace bptreedb
{
	enum ECompressNodeType
	{
		eInnerNode,
		eLeafNode

	};

	enum ECompressParams
	{
		eInnerKey,
		eInnerValue,
		eLeafKey,
		eLeafValue

	};




	class CompressorParams
	{
	public:
		CompressorParams();
		virtual ~CompressorParams();
		virtual void Load(CommonLib::IReadStream *pStream);
		virtual void Save(CommonLib::IWriteStream *pStream);

		int32_t GetIntParam(const astr& name, int32_t defValue) const;
		double GetDoubleParam(const astr& name, double defValue) const;

		void SetIntParam(const astr& name, int32_t value);
		void SetDoubleParam(const astr& name, double value);


	protected:

		template <class Type>
		Type GetParam(const astr& name, const std::map<astr, Type>& mapParams, const Type& defValue) const
		{
			auto it = mapParams.find(name);
			if (it != mapParams.end())
				return it->second;

			return defValue;
		}

		template <class Type>
		void SaveParams(const std::map<astr, Type>& mapParams, CommonLib::IWriteStream *pStream)
		{
			pStream->Write(uint32_t(mapParams.size()));
			auto it = mapParams.begin();
			auto end = mapParams.end();
			for (; it != end; ++it)
			{
				pStream->Write(it->first);
				pStream->Write(it->second);
			}
		}

		template <class Type>
		void LoadParams(std::map<astr, Type>& mapParams, CommonLib::IReadStream *pStream)
		{
			uint32_t size = pStream->ReadIntu32();

			for (uint32_t i = 0; i < size; ++i)
			{
				astr name;
				Type value;

				pStream->Read(name);
				pStream->Read(value);

				mapParams.insert(std::make_pair(name, value));
			}

		}


	protected:
		//To do: XML probability is better choice
		typedef std::map<astr, int32_t> TMapIntParams;
		typedef std::map<astr, double> TMapDoubleParams;
		TMapIntParams m_IntParams;
		TMapDoubleParams m_DoubleParams;

	};

	typedef std::shared_ptr<CompressorParams> TCompressorParamsPtr;

	class CompressorParamsBase
	{
	public:
		CompressorParamsBase();
		virtual ~CompressorParamsBase();
		virtual void Load(CommonLib::IReadStream *pStream);
		virtual void Save(CommonLib::IWriteStream *pStream);

		TCompressorParamsPtr GetCompressParams(ECompressParams eId);
		void AddCompressParams(TCompressorParamsPtr ptrParams, ECompressParams eId);


	protected:

		typedef std::map<ECompressParams, TCompressorParamsPtr> TMapParams;
		TMapParams m_params;

	};

	typedef std::shared_ptr<CompressorParamsBase> TCompressorParamsBasePtr;
 

}

 

