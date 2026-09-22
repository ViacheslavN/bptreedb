#include "stdafx.h"
#include "SpatialRectQuery.h"
//#include "tableZRectt8Bit.h"
#include "tableZRectBack4Bit.h"
#define  ZRECT_TABLE_8_BITS

#ifdef ZRECT_TABLE_8_BITS
	#include "tableZRect8Bit.h"
#else
	#include "tableZRect16Bit.h"
#endif

#include "../../../UtilsLib/MathUtils.h"

namespace bptreedb {
	namespace spatiallib {


		static const uint64_t sBitsMasks[4] = {
			0x8888888888888888ULL,
			0x4444444444444444ULL,
			0x2222222222222222ULL,
			0x1111111111111111ULL
		};



		uint64_t getZCompu16(uint16_t nVal)
		{
#ifdef ZRECT_TABLE_8_BITS
			uint64_t nRet = 0;
			uint16_t nMinVal = nVal & 0xFF;
			uint16_t nMaxVal = nVal >> 8;

			nRet = TableZRect8Bit[nMinVal];
			nRet |=	TableZRect8Bit[nMaxVal] << 32;
			return nRet;
#else
			return TableZRect16Bit[nVal];
#endif

		}

		//0x1111111111111111
		uint64_t getZValue16(uint16_t xMin, uint16_t yMin, uint16_t xMax, uint16_t yMax)
		{
			uint64_t xZMin = getZCompu16(xMin);
			uint64_t yZMin = getZCompu16(yMin);
			uint64_t xZMax = getZCompu16(xMax);
			uint64_t yZMax = getZCompu16(yMax);
			uint64_t nVal = xZMin | (yZMin << 1) | (xZMax << 2) | (yZMax << 3);
			return nVal;
		}



		uint64_t CalcZValue16(uint16_t Cord1, uint16_t Cord2, uint16_t Cord3, uint16_t Cord4)
		{
			uint64_t ZCord1 = getZCompu16(Cord1);
			uint64_t ZCord2 = getZCompu16(Cord2);
			uint64_t ZCord3 = getZCompu16(Cord3);
			uint64_t ZCord4 = getZCompu16(Cord4);
			uint64_t nVal = ZCord1 | (ZCord2 << 1) | (ZCord3 << 2) | (ZCord4 << 3);
			return nVal;
		}



		uint16_t getFromZu16(uint64_t nZVal)
		{
			uint16_t nRet = BackTableZRect4Bit[nZVal & 0xf];
			for (uint16_t i = 1; i < 16; ++i)
			{
				nRet |= BackTableZRect4Bit[(nZVal >> 4 * i) & 0xf] << i;
			}
			return nRet;
		}
		void getXYFromZValue16(uint16_t& xMin, uint16_t& yMin, uint16_t& xMax, uint16_t& yMax, uint64_t nZValue)
		{
			xMin = getFromZu16(nZValue & 0x1111111111111111);
			yMin = getFromZu16((nZValue >> 1 ) & 0x1111111111111111);
			xMax = getFromZu16((nZValue >> 2 ) & 0x1111111111111111);
			yMax = getFromZu16((nZValue >> 3 ) & 0x1111111111111111);
		}


		void getCoordFromZValue16(uint16_t& Coord1, uint16_t& Coord2, uint16_t& Coord3, uint16_t& Coord4, uint64_t nZValue)
		{
			Coord1 = getFromZu16(nZValue & 0x1111111111111111);
			Coord2 = getFromZu16((nZValue >> 1 ) & 0x1111111111111111);
			Coord3 = getFromZu16((nZValue >> 2 ) & 0x1111111111111111);
			Coord4 = getFromZu16((nZValue >> 3 ) & 0x1111111111111111);
		}


		void setZValue32(uint32_t xMin, uint32_t yMin, uint32_t xMax, uint32_t yMax, uint64_t& zValLow, uint64_t& zValHigh)
		{
			uint16_t xLowMin = xMin & 0xFFFF;
			uint16_t yLowMin = yMin & 0xFFFF;
			uint16_t xLowMax = xMax & 0xFFFF;
			uint16_t yLowMax = yMax & 0xFFFF;

			zValLow = CalcZValue16(yLowMax, yLowMin, xLowMax, xLowMin);

			uint16_t xHighMin = xMin >> 16;
			uint16_t yHighMin = yMin  >> 16;
			uint16_t xHighMax = xMax  >> 16;
			uint16_t yHighMax = yMax  >> 16;

			//zValHigh = getZValue16(xHighMin, yHighMin, xHighMax, yHighMax);
			zValHigh = CalcZValue16(yHighMax, yHighMin, xHighMax, xHighMin);
		}
		void getXYFromZValue32(uint32_t& xMin, uint32_t& yMin, uint32_t& xMax, uint32_t& yMax, uint64_t zValLow, uint64_t zValHigh)
		{
			uint16_t xLowMin = 0;
			uint16_t yLowMin = 0;
			uint16_t xLowMax = 0;
			uint16_t yLowMax = 0;

			getCoordFromZValue16(yLowMax, yLowMin, xLowMax, xLowMin, zValLow);

			uint16_t xHighMin = 0;
			uint16_t yHighMin = 0;
			uint16_t xHighMax = 0;
			uint16_t yHighMax = 0;


			getCoordFromZValue16(yHighMax, yHighMin, xHighMax, xHighMin, zValHigh);

			xMin = (uint32_t)xLowMin | ((uint32_t)xHighMin << 16);
			yMin = (uint32_t)yLowMin | ((uint32_t)yHighMin << 16);
			xMax = (uint32_t)xLowMax | ((uint32_t)xHighMax << 16);
			yMax = (uint32_t)yLowMax | ((uint32_t)yHighMax << 16);
		}
		ZOrderRect2DU16::ZOrderRect2DU16() : m_nZValue(0)
		{

		}
		ZOrderRect2DU16::ZOrderRect2DU16(ZValueType zValue) : m_nZValue(zValue)
		{

		}
		ZOrderRect2DU16::ZOrderRect2DU16(const TRect& rect)
		{
			setZOrder(rect.m_minX, rect.m_minY, rect.m_maxX, rect.m_maxY);
		}
		ZOrderRect2DU16::ZOrderRect2DU16(uint16_t xMin, uint16_t yMin, uint16_t xMax, uint16_t yMax)
		{
			setZOrder(xMin, yMin, xMax, yMax);
		}

		void ZOrderRect2DU16::setZOrder(uint16_t xMin, uint16_t yMin, uint16_t xMax, uint16_t yMax)
		{
			m_nZValue = CalcZValue16(yMax, yMin, xMax, xMin);
		}

		void ZOrderRect2DU16::getXY(uint16_t& xMin, uint16_t& yMin, uint16_t& xMax, uint16_t& yMax) const
		{
			getCoordFromZValue16(yMax, yMin, xMax, xMin, m_nZValue);
		}
		void ZOrderRect2DU16::getXY(TRect& rect) const
		{
			getXY(rect.m_minX, rect.m_minY, rect.m_maxX, rect.m_maxY);
		}

		void ZOrderRect2DU16::setLowBits(int idx)
		{
			uint64_t bitMask = sBitsMasks[0] >> (63 - idx);
			uint64_t bit = uint64_t (1) << uint64_t (idx & 0x3f);

			m_nZValue |= (bitMask);
			m_nZValue -= bit;
		}

		void ZOrderRect2DU16::clearLowBits(int idx)
		{
			uint64_t bitMask = sBitsMasks[0] >> (63 - idx);
			uint64_t bit = uint64_t (1) << uint64_t (idx & 0x3f);


			m_nZValue &= ~(bitMask);
			m_nZValue |= bit;
		}

		ZOrderRect2DU32::ZOrderRect2DU32()
		{
			m_nZValue[0] = 0;
			m_nZValue[1] = 0;
		}
		ZOrderRect2DU32::ZOrderRect2DU32(const TRect& rect)
		{
			setZOrder(rect.m_minX, rect.m_minY, rect.m_maxX, rect.m_maxY);
		}
		ZOrderRect2DU32::ZOrderRect2DU32(uint32_t xMin, uint32_t yMin, uint32_t xMax, uint32_t yMax)
		{
			setZOrder(xMin, yMin, xMax, yMax);
		}


		void ZOrderRect2DU32::setZOrder(uint32_t xMin, uint32_t yMin, uint32_t xMax, uint32_t yMax)
		{
			setZValue32(xMin, yMin, xMax, yMax, m_nZValue[0], m_nZValue[1]);
		}

		void ZOrderRect2DU32::getXY(uint32_t& xMin, uint32_t& yMin, uint32_t& xMax, uint32_t& yMax) const
		{
			getXYFromZValue32(xMin, yMin, xMax, yMax, m_nZValue[0], m_nZValue[1]);
		}
		void ZOrderRect2DU32::getXY(TRect& rect) const
		{
			getXY(rect.m_minX, rect.m_minY, rect.m_maxX, rect.m_maxY);
		}


		void ZOrderRect2DU32::setLowBits(int idx)
		{

			if (idx > 128 || idx < 0)
				throw CommonLib::CExcBase("FindRectMinZVal: idx > 128 || idx < 0");

			if(idx > 63)
			{
				int subIndex = (idx - 64);
				uint64_t bitMask = 0x8888888888888888 >> (63 - subIndex);
				uint64_t bit = uint64_t (1) << uint64_t (subIndex & 0x3f);

				// clear bit idx: subtracting it would borrow when the bit is already 0
				m_nZValue[1] &= ~bit;
				m_nZValue[1] |= (bitMask & (bit - 1));

				bitMask = 0x8888888888888888 >> (3 - subIndex % 4);
				m_nZValue[0] |= bitMask;
			}
			else
			{
				uint64_t bitMask = 0x8888888888888888 >> (63 - idx);
				uint64_t bit = uint64_t (1) << uint64_t (idx & 0x3f);

				m_nZValue[0] |= bitMask;
				m_nZValue[0] &= ~bit;

			}

		}





		void ZOrderRect2DU32::clearLowBits(int idx)
		{
	 		if (idx >128 || idx < 0)
				throw CommonLib::CExcBase("FindRectMinZVal: idx > 128 || idx < 0");

			if(idx > 63)
			{
				int subIndex =  (idx - 64);
				uint64_t bitMask = 0x8888888888888888 >> (63 - subIndex);
				uint64_t bit = uint64_t (1) << uint64_t (subIndex & 0x3f);
				m_nZValue[1] |=  bit;
				m_nZValue[1] &= ~(bitMask& (bit - 1));

				bitMask = 0x8888888888888888 >> (3 - subIndex % 4);
				m_nZValue[0] &= ~(bitMask /*& (bit - 1)*/);


			}
			else
			{
				uint64_t bitMask = 0x8888888888888888 >> (63 - idx);
				uint64_t bit = uint64_t (1) << uint64_t (idx & 0x3f);
				m_nZValue[0] &= ~(bitMask & (bit - 1));
				m_nZValue[0] |= bit;
			}
		}
		ZOrderRect2DU32 ZOrderRect2DU32::operator - (const ZOrderRect2DU32&  zOrder) const
		{


			ZOrderRect2DU32 result;
			bool bCarry = true;

			result.m_nZValue[0] = utils::UI64_Add(m_nZValue[0], ~zOrder.m_nZValue[0], bCarry, &bCarry);
			result.m_nZValue[1] = utils::UI64_Add(m_nZValue[1], ~zOrder.m_nZValue[1], bCarry, &bCarry);

			return result;
		}
		ZOrderRect2DU32 ZOrderRect2DU32::operator + (const ZOrderRect2DU32&  zOrder) const
		{


			ZOrderRect2DU32 result;
			bool bCarry = false;

			result.m_nZValue[0] = utils::UI64_Add(m_nZValue[0], zOrder.m_nZValue[0], bCarry, &bCarry);
			result.m_nZValue[1] = utils::UI64_Add(m_nZValue[1], zOrder.m_nZValue[1], bCarry, &bCarry);

			return result;
		}

		ZOrderRect2DU32& ZOrderRect2DU32::operator += (const ZOrderRect2DU32&  zOrder)
		{

			bool bCarry = false;
			m_nZValue[0] = utils::UI64_Add(m_nZValue[0], zOrder.m_nZValue[0], bCarry, &bCarry);
			m_nZValue[1] = utils::UI64_Add(m_nZValue[1], zOrder.m_nZValue[1], bCarry, &bCarry);

			return *this;
		}

		ZOrderRect2DU64::ZOrderRect2DU64()
		{
			m_nZValue[0] = m_nZValue[1] = m_nZValue[2] = m_nZValue[3] = 0;
		}
		ZOrderRect2DU64::ZOrderRect2DU64(const TRect& rect)
		{
			setZOrder(rect.m_minX, rect.m_minY, rect.m_maxX, rect.m_maxY);
		}
		ZOrderRect2DU64::ZOrderRect2DU64(uint64_t xMin, uint64_t yMin, uint64_t xMax, uint64_t yMax)
		{
			setZOrder(xMin, yMin, xMax, yMax);
		}
		void ZOrderRect2DU64::setZOrder(uint64_t xMin, uint64_t yMin, uint64_t xMax, uint64_t yMax)
		{
			uint32_t xLowMin = xMin & 0xFFFFFFFF;
			uint32_t yLowMin = yMin & 0xFFFFFFFF;
			uint32_t xLowMax = xMax & 0xFFFFFFFF;
			uint32_t yLowMax = yMax & 0xFFFFFFFF;

			setZValue32(xLowMin, yLowMin, xLowMax, yLowMax, m_nZValue[0], m_nZValue[1]);

			uint32_t xHighMin = xMin >> 32;
			uint32_t yHighMin = yMin >> 32;
			uint32_t xHighMax = xMax >> 32;
			uint32_t yHighMax = yMax >> 32;

			setZValue32(xHighMin, yHighMin, xHighMax, yHighMax, m_nZValue[2], m_nZValue[3]);
		}
		void ZOrderRect2DU64::getXY(uint64_t& xMin, uint64_t& yMin, uint64_t& xMax, uint64_t& yMax) const
		{
			uint32_t xLowMin = 0;
			uint32_t yLowMin = 0;
			uint32_t xLowMax = 0;
			uint32_t yLowMax = 0;

			getXYFromZValue32(xLowMin, yLowMin, xLowMax, yLowMax, m_nZValue[0], m_nZValue[1]);


			uint32_t xHighMin = 0;
			uint32_t yHighMin = 0;
			uint32_t xHighMax = 0;
			uint32_t yHighMax = 0;
			getXYFromZValue32(xHighMin, yHighMin, xHighMax, yHighMax, m_nZValue[2], m_nZValue[3]);


			xMin = (uint64_t)xLowMin | ((uint64_t)xHighMin << 32);
			yMin = (uint64_t)yLowMin | ((uint64_t)yHighMin << 32);
			xMax = (uint64_t)xLowMax | ((uint64_t)xHighMax << 32);
			yMax = (uint64_t)yLowMax | ((uint64_t)yHighMax << 32);
		}

		void ZOrderRect2DU64::getXY(TRect& rect) const
		{
			getXY(rect.m_minX, rect.m_minY, rect.m_maxX, rect.m_maxY);
		}
		uint64_t ZOrderRect2DU64::getBit (int idx)
		{
			int nBlock = (idx & 0xff) >> 6;
			int nSubIndex = idx - (64 * nBlock);
			return (m_nZValue[nBlock] >> (nSubIndex & 0x3f));
		}

		void ZOrderRect2DU64::setLowBits(int idx)
		{

			int nBlock = (idx & 0xff) >> 6;
			int nSubIndex = idx - (64 * nBlock);
			uint64_t bitMask = 0x8888888888888888 >> (63 - nSubIndex);
			uint64_t bit = uint64_t (1) << uint64_t (nSubIndex & 0x3f);
			// clear bit idx: subtracting it would borrow when the bit is already 0
			m_nZValue[nBlock] &= ~bit;
			m_nZValue[nBlock] |= (bitMask & (bit - 1));


			bitMask = 0x8888888888888888 >> (3 - nSubIndex % 4);
			for (int i = nBlock - 1; i >= 0; --i)
			{
				m_nZValue[i] |= (bitMask /*& (bit - 1)*/);
			}
		}

		void ZOrderRect2DU64::clearLowBits(int idx)
		{

			int nBlock = (idx & 0xff) >> 6;
			int nSubIndex = idx - (64 * nBlock);
			uint64_t bitMask = 0x8888888888888888 >> (63 - nSubIndex);
			uint64_t bit = uint64_t (1) << uint64_t (nSubIndex & 0x3f);
			m_nZValue[nBlock] |=  bit;
			m_nZValue[nBlock] &= ~(bitMask& (bit - 1));

			bitMask = 0x8888888888888888 >> (3 - nSubIndex % 4);
			for (int i = nBlock - 1; i >= 0; --i)
			{
				m_nZValue[i] &= ~(bitMask /*& (bit - 1)*/);
			}
		}


		ZOrderRect2DU64 ZOrderRect2DU64::operator - (const ZOrderRect2DU64&  zOrder) const
		{
			ZOrderRect2DU64 result;
			bool bCarry = true;

			result.m_nZValue[0] = utils::UI64_Add(m_nZValue[0], ~zOrder.m_nZValue[0], bCarry, &bCarry);
			result.m_nZValue[1] = utils::UI64_Add(m_nZValue[1], ~zOrder.m_nZValue[1], bCarry, &bCarry);
			result.m_nZValue[2] = utils::UI64_Add(m_nZValue[2], ~zOrder.m_nZValue[2], bCarry, &bCarry);
			result.m_nZValue[3] = utils::UI64_Add(m_nZValue[3], ~zOrder.m_nZValue[3], bCarry, &bCarry);

			return result;
		}

		ZOrderRect2DU64 ZOrderRect2DU64::operator + (const ZOrderRect2DU64&  zOrder) const
		{
			ZOrderRect2DU64 result;

			bool bCarry = false;

			result.m_nZValue[0] = utils::UI64_Add(m_nZValue[0], zOrder.m_nZValue[0], bCarry, &bCarry);
			result.m_nZValue[1] = utils::UI64_Add(m_nZValue[1], zOrder.m_nZValue[1], bCarry, &bCarry);
			result.m_nZValue[2] = utils::UI64_Add(m_nZValue[2], zOrder.m_nZValue[2], bCarry, &bCarry);
			result.m_nZValue[3] = utils::UI64_Add(m_nZValue[3], zOrder.m_nZValue[3], bCarry, &bCarry);

			return result;
		}
		ZOrderRect2DU64& ZOrderRect2DU64::operator += (const ZOrderRect2DU64&  zOrder)
		{

			bool bCarry = false;
			m_nZValue[0] = utils::UI64_Add(m_nZValue[0], zOrder.m_nZValue[0], bCarry, &bCarry);
			m_nZValue[1] = utils::UI64_Add(m_nZValue[1], zOrder.m_nZValue[1], bCarry, &bCarry);
			m_nZValue[2] = utils::UI64_Add(m_nZValue[2], zOrder.m_nZValue[2], bCarry, &bCarry);
			m_nZValue[3] = utils::UI64_Add(m_nZValue[3], zOrder.m_nZValue[3], bCarry, &bCarry);

			return *this;
		}
	}
}