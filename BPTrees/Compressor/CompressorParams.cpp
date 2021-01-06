#include "stdafx.h"
#include "CompressorParams.h"
 

namespace bptreedb
{

	CompressorParamsBase::CompressorParamsBase()
	{

	}

	CompressorParamsBase::~CompressorParamsBase()
	{

	}

	void CompressorParamsBase::Load(CommonLib::IReadStream *pStream)
	{
		try
		{
			uint32_t size = pStream->ReadIntu32();

			for (uint32_t i = 0; i < size; ++i)
			{
				uint32_t nID = pStream->ReadIntu32();

				TCompressorParamsPtr ptrParams(new CompressorParams());
				ptrParams->Load(pStream);

				AddCompressParams(ptrParams, (ECompressParams)nID);
			}
		}
		catch (std::exception& exc)
		{
			CommonLib::CExcBase::RegenExc("Failed to load params base", exc);
		}
	}

	void CompressorParamsBase::Save(CommonLib::IWriteStream *pStream)
	{
		try
		{
			pStream->Write((uint32_t)m_params.size());
			for (auto it = m_params.begin(); it != m_params.end(); ++it)
			{
				pStream->Write((uint32_t)it->first);
				it->second->Save(pStream);
			}
		}
		catch (std::exception& exc)
		{
			CommonLib::CExcBase::RegenExc("Failed to save params base", exc);
		}
	}

	TCompressorParamsPtr CompressorParamsBase::GetCompressParams(ECompressParams eId)
	{

		auto it = m_params.find(eId);
		if (it != m_params.end())
			return it->second;

		return TCompressorParamsPtr();

	}

	void CompressorParamsBase::AddCompressParams(TCompressorParamsPtr ptrParams, ECompressParams eId)
	{
		m_params[eId] = ptrParams;
	}

	CompressorParams::CompressorParams()
	{
		
	}

	CompressorParams::~CompressorParams()
	{

	}

	int32_t CompressorParams::GetIntParam(const astr& name, int32_t defValue) const
	{
		return GetParam<int32_t>(name, m_IntParams, defValue);
	}

	double CompressorParams::GetDoubleParam(const astr& name, double defValue) const
	{	
		return GetParam<double>(name, m_DoubleParams, defValue);
	}

	void  CompressorParams::SetIntParam(const astr& name, int32_t value)
	{
		m_IntParams[name] = value;
	}

	void  CompressorParams::SetDoubleParam(const astr& name, double value)
	{
		m_DoubleParams[name] = value;
	}

	void CompressorParams::Save(CommonLib::IWriteStream *pStream)
	{
		try
		{
			SaveParams<int32_t>(m_IntParams, pStream);
			SaveParams<double>(m_DoubleParams, pStream);
		}
		catch (std::exception& exc)
		{
			CommonLib::CExcBase::RegenExc("Failed to save params",exc);
		}
	}

	void CompressorParams::Load(CommonLib::IReadStream *pStream)
	{
		try
		{
			LoadParams<int32_t>(m_IntParams, pStream);
			LoadParams<double>(m_DoubleParams, pStream);
		}
		catch (std::exception& exc)
		{
			CommonLib::CExcBase::RegenExc("Failed to load params", exc);
		}
	}



}