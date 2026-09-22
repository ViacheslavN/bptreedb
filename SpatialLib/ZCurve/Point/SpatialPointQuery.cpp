#include "pch.h"

#include "SpatialPointQuery.h"
#include "BackTableZPoint16Bit.h"
#include "../../../UtilsLib/MathUtils.h"
namespace bptreedb {
	namespace spatiallib
	{

		void ZOrderPoint2DU16::getXY(uint16_t& x, uint16_t& y)  const
		{
			uint32_t zY = m_nZValue & MASKSPoint[0];
			uint32_t zX = (m_nZValue >> 1) & MASKSPoint[0];

			x = uint16_t(uint32_t(BackTableZpoint16[zX & 0xFFFF]) | (uint32_t(BackTableZpoint16[(zX >> 16) & 0xFFFF]) << 8));
			y = uint16_t(uint32_t(BackTableZpoint16[zY & 0xFFFF]) | (uint32_t(BackTableZpoint16[(zY >> 16) & 0xFFFF]) << 8));

		}
		void ZOrderPoint2DU16::getXY(TPoint& point) const
		{
			getXY(point.m_x, point.m_y);
		}

		uint32_t getX32FromZ(uint64_t zX)
		{
			// widen each byte to uint32_t before shifting: byte << 24 on a promoted (signed) int
			// overflows for values >= 0x80
			return uint32_t(BackTableZpoint16[zX & 0xFFFF])
				| (uint32_t(BackTableZpoint16[(zX >> 16) & 0xFFFF]) << 8)
				| (uint32_t(BackTableZpoint16[(zX >> 32) & 0xFFFF]) << 16)
				| (uint32_t(BackTableZpoint16[(zX >> 48) & 0xFFFF]) << 24);
		}
		void ZOrderPoint2DU32::getXY(uint32_t& x, uint32_t& y) const
		{
			uint64_t zY = m_nZValue & MASKSPoint64[0];
			uint64_t zX = (m_nZValue >> 1) & MASKSPoint64[0];
			x = getX32FromZ(zX);
			y = getX32FromZ(zY);
		}
		void ZOrderPoint2DU32::getXY(TPoint& point) const
		{
			getXY(point.m_x, point.m_y);
		}

		void ZOrderPoint2DU64::getXY(uint64_t& x,  uint64_t& y)  const
		{
			uint64_t zYMin = m_nZValue[0] & MASKSPoint64[0];
			uint64_t zXMin = (m_nZValue[0] >> 1) & MASKSPoint64[0];

			uint64_t zYMax = m_nZValue[1] & MASKSPoint64[0];
			uint64_t zXMax = (m_nZValue[1] >> 1) & MASKSPoint64[0];

			uint64_t nXmax = getX32FromZ(zXMax);
			uint64_t nXmin = getX32FromZ(zXMin);

			uint64_t nYmax = getX32FromZ(zYMax);
			uint64_t nYmin = getX32FromZ(zYMin);

			x = nXmin | (nXmax << 32);
			y = nYmin | (nYmax << 32);
		}
		void ZOrderPoint2DU64::getXY(TPoint& point) const
		{
			getXY(point.m_x, point.m_y);
		}
		void ZOrderPoint2DU64::setLowBits(int idx)
		{

			if(idx > 63)
			{
				int subIndex = idx - 64;
				uint64_t bitMask = 0xAAAAAAAAAAAAAAAA >> (63 - subIndex);
				m_nZValue[1] |= bitMask;
				uint64_t bit = uint64_t (1) << uint64_t (subIndex & 0x3f);
				m_nZValue[1] -= bit;
				bitMask = 0xAAAAAAAAAAAAAAAA >> (idx % 2 ? 0 : 1);
				m_nZValue[0] |= bitMask;
			}
			else
			{
				uint64_t bitMask = 0xAAAAAAAAAAAAAAAA >> (63 - idx);
				m_nZValue[0] |= bitMask;
				uint64_t bit = uint64_t (1) << uint64_t (idx & 0x3f);
				m_nZValue[0] -= bit;
			}

		}

		void ZOrderPoint2DU64::clearLowBits(int idx)
		{
			if(idx > 63)
			{
				int subIndex =  idx - 64;
				uint64_t bitMask = 0xAAAAAAAAAAAAAAAA >> (63 - subIndex);
				m_nZValue[1] &= ~bitMask;
				uint64_t bit = uint64_t (1) << uint64_t (subIndex & 0x3f);
				m_nZValue[1] |=  bit;
				bitMask = 0xAAAAAAAAAAAAAAAA >> (idx % 2 ? 0 : 1);
				m_nZValue[0] &= ~bitMask;
			}
			else
			{
				uint64_t bitMask = 0xAAAAAAAAAAAAAAAA >> (63 - idx);
				m_nZValue[0] &= ~bitMask;
				uint64_t bit = uint64_t (1) << uint64_t (idx & 0x3f);
				m_nZValue[0] |=  bit;
			}
		}


		ZOrderPoint2DU64 ZOrderPoint2DU64::operator - (const ZOrderPoint2DU64&  zOrder) const
		{
			ZOrderPoint2DU64 result;
			bool bCarry = true;

			result.m_nZValue[0] = utils::UI64_Add(m_nZValue[0], ~zOrder.m_nZValue[0], bCarry, &bCarry);
			result.m_nZValue[1] = utils::UI64_Add(m_nZValue[1], ~zOrder.m_nZValue[1], bCarry, &bCarry);

			return result;
		}
		ZOrderPoint2DU64 ZOrderPoint2DU64::operator + (const ZOrderPoint2DU64&  zOrder) const
		{
			ZOrderPoint2DU64 result;
			bool bCarry = false;

			result.m_nZValue[0] = utils::UI64_Add(m_nZValue[0], zOrder.m_nZValue[0], bCarry, &bCarry);
			result.m_nZValue[1] = utils::UI64_Add(m_nZValue[1], zOrder.m_nZValue[1], bCarry, &bCarry);

			return result;
		}
		ZOrderPoint2DU64& ZOrderPoint2DU64::operator += (const ZOrderPoint2DU64&  zOrder)
		{
			bool bCarry = false;

			m_nZValue[0] = utils::UI64_Add(m_nZValue[0], zOrder.m_nZValue[0], bCarry, &bCarry);
			m_nZValue[1] = utils::UI64_Add(m_nZValue[1], zOrder.m_nZValue[1], bCarry, &bCarry);
			return *this;
		}
	}
}