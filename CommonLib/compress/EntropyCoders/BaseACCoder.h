#pragma once

namespace CommonLib
{

	template<class TCodeValue>
	class BaseACEncoder
	{
	public:
		BaseACEncoder() {}
		virtual ~BaseACEncoder() {}

		virtual bool EncodeSymbol(TCodeValue nLowCount, TCodeValue nHightCount, TCodeValue nTotalCount) = 0;
		virtual bool EncodeFinish() = 0;
		virtual uint32_t GetAdditionalSize() const = 0 ;
	};

	template<class TCodeValue>
	class BaseACDecoder
	{
	public:
		BaseACDecoder() {}
		virtual ~BaseACDecoder() {}

		virtual void StartDecode() = 0;
		virtual void DecodeSymbol(TCodeValue SymbolLow, TCodeValue SymbolHigh, TCodeValue TotalRange) = 0;
		virtual TCodeValue GetFreq(TCodeValue nTotalCount) = 0;
	};

}
