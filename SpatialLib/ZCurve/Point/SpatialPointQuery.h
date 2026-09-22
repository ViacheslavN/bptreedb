#pragma once
#include "SpatialKey.h"
namespace bptreedb
{
	namespace spatiallib {
		static const uint32_t MASKSPoint[] = {0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF};
		static const uint32_t SHIFTSPoint[] = {1, 2, 4, 8, 16, 32};


		//5 0xFFFF0000FFFF     <<16 //111111111111111100000000000000001111111111111111
		//4 0xFF00FF00FF00FF   <<8  //11111111000000001111111100000000111111110000000011111111
		//3 0xF0F0F0F0F0F0F0F  <<4  //111100001111000011110000111100001111000011110000111100001111
		//2 0x3333333333333333 <<2  //11001100110011001100110011001100110011001100110011001100110011
		//1 0x5555555555555555 <<1  //101010101010101010101010101010101010101010101010101010101010101
		static const uint64_t MASKSPoint64[] = {0x5555555555555555 , 0x3333333333333333 ,  0xF0F0F0F0F0F0F0F , 0xFF00FF00FF00FF ,0xFFFF0000FFFF};



		static uint32_t getShiftValue16(uint16_t nVal)
		{
			uint32_t  x = nVal;
			x = (x | (x << SHIFTSPoint[3])) & MASKSPoint[3];
			x = (x | (x << SHIFTSPoint[2])) & MASKSPoint[2];
			x = (x | (x << SHIFTSPoint[1])) & MASKSPoint[1];
			x = (x | (x << SHIFTSPoint[0])) & MASKSPoint[0];
			return x;
		}

		static uint64_t getShiftValue32(uint32_t nVal)
		{
			uint64_t  x = nVal;
			x = (x | (x << SHIFTSPoint[4])) & MASKSPoint64[4];
			x = (x | (x << SHIFTSPoint[3])) & MASKSPoint64[3];
			x = (x | (x << SHIFTSPoint[2])) & MASKSPoint64[2];
			x = (x | (x << SHIFTSPoint[1])) & MASKSPoint64[1];
			x = (x | (x << SHIFTSPoint[0])) & MASKSPoint64[0];
			return x;
		}

		template <class TPoint, class TZOrder, class TRect>
		bool IsZPointOrdertInRect(const TRect& rect, const TZOrder& zOrder)
		{
			TPoint x, y;
			zOrder.getXY(x, y);
			return rect.isPoinInRect(x, y);
		}


		struct ZOrderPoint2DU16
		{

			typedef int16_t  TPointTypei;
			typedef uint16_t TPointType;
			typedef uint32_t ZValueType;
			typedef TPoint2D<uint16_t> TPoint;
			static const uint16_t SizeInByte = 4;
			ZOrderPoint2DU16() : m_nZValue(0)
			{}
			ZOrderPoint2DU16(uint32_t nZorder) : m_nZValue(nZorder)
			{}
			ZOrderPoint2DU16(uint16_t nX, uint16_t nY) : m_nZValue(0)
			{
				setZOrder(nX, nY);
			}
			ZOrderPoint2DU16(const TPoint& point)
			{
				setZOrder(point.m_x, point.m_y);
			}
			void setZOrder(uint16_t nXPos, uint16_t nYPos)
			{
				uint32_t  x = nXPos;  // Interleave lower 16 bits of x and y, so the bits of x
				uint32_t  y = nYPos;  // are in the even positions and bits from y in the odd;

				x = getShiftValue16(nXPos);
				y = getShiftValue16(nYPos);
				m_nZValue = y | (x << 1);
			}

			bool operator < (const ZOrderPoint2DU16& zOrder) const
			{
				return m_nZValue < zOrder.m_nZValue;
			}
			bool operator <= (const ZOrderPoint2DU16& zOrder) const
			{
				return m_nZValue <= zOrder.m_nZValue;
			}
			bool operator > (const ZOrderPoint2DU16& zOrder) const
			{
				return m_nZValue > zOrder.m_nZValue;
			}
			ZOrderPoint2DU16& operator = (uint32_t nZorder)
			{
				m_nZValue = nZorder;
				return *this;
			}
			bool operator == (const ZOrderPoint2DU16&  Zorder) const
			{
				return m_nZValue == Zorder.m_nZValue;
			}
			short getBits() const
			{
				return 31;
			}
			uint32_t getBit (int idx)
			{
				return uint32_t(m_nZValue>>(idx&0x1f));
			}

			void setLowBits(int idx)
			{
				uint32_t bitMask = 0xAAAAAAAA >> (31 - idx);
				m_nZValue |= bitMask;
				uint32_t bit = uint32_t (1) << uint32_t (idx & 0x1f);
				m_nZValue -= bit;
			}

			void clearLowBits(int idx)
			{
				uint32_t bitMask = 0xAAAAAAAA >> (31 - idx);
				m_nZValue &= ~bitMask;
				uint32_t bit = uint32_t (1) << uint32_t (idx & 0x3f);
				m_nZValue |= bit;
			}
			bool IsInRect(const TRect2Du16& rect) const
			{
				return IsZPointOrdertInRect<uint16_t, ZOrderPoint2DU16, TRect2Du16>(rect, *this);
			}

			ZOrderPoint2DU16 operator - (const ZOrderPoint2DU16&  zOrder) const
			{
				return ZOrderPoint2DU16(m_nZValue - zOrder.m_nZValue);
			}
			ZOrderPoint2DU16 operator + (const ZOrderPoint2DU16&  zOrder) const
			{
				return ZOrderPoint2DU16(m_nZValue + zOrder.m_nZValue);
			}
			ZOrderPoint2DU16& operator += (const ZOrderPoint2DU16&  zOrder)
			{
				m_nZValue += zOrder.m_nZValue;
				return *this;
			}

			void getXY(uint16_t& x,  uint16_t& y) const;
			void getXY(TPoint& point) const;
			uint32_t m_nZValue;


		};
		//typename ZOrderPoint2D<uint16> PointZOrderU16;


		static void SetZOrderInt32(uint64_t& nZValue, uint32_t nXPos, uint32_t nYPos)
		{
			uint64_t  x = nXPos;  // Interleave lower 16 bits of x and y, so the bits of x
			uint64_t  y = nYPos;  // are in the even positions and bits from y in the odd;

			x = getShiftValue32(nXPos);
			y = getShiftValue32(nYPos);


			nZValue = y | (x << 1);
		}

		struct ZOrderPoint2DU32
		{
			typedef int32_t  TPointTypei;
			typedef uint32_t TPointType;
			typedef uint64_t ZValueType;
			typedef TPoint2D<uint32_t> TPoint;
			static const uint16_t SizeInByte = 8;
			ZOrderPoint2DU32() : m_nZValue(0)
			{}
			ZOrderPoint2DU32(uint64_t nZorder) : m_nZValue(nZorder)
			{}
			ZOrderPoint2DU32(uint32_t nX, uint32_t nY) : m_nZValue(0)
			{
				setZOrder(nX, nY);
			}
			ZOrderPoint2DU32(const TPoint& point)
			{
				setZOrder(point.m_x, point.m_y);
			}
			void setZOrder(uint32_t nXPos, uint32_t nYPos)
			{
				SetZOrderInt32(m_nZValue, nXPos, nYPos);
			}
			uint64_t getBit (int idx) const
			{
				return uint64_t(m_nZValue >> (idx & 0x3f));
			}
			void getXY(uint32_t& x,  uint32_t& y) const;
			void getXY(TPoint& point) const;
			void setLowBits(int idx)
			{
				uint64_t bitMask = 0xAAAAAAAAAAAAAAAA >> (63 - idx);
				m_nZValue |= bitMask;
				uint64_t bit = uint64_t (1) << uint64_t (idx & 0x3f);
				m_nZValue -= bit;
			}
			void clearLowBits(int idx)
			{
				uint64_t bitMask = 0xAAAAAAAAAAAAAAAA >> (63 - idx);
				m_nZValue &= ~bitMask;
				uint64_t bit = uint64_t (1) << uint64_t (idx & 0x3f);
				m_nZValue |= bit;
			}
			bool operator < (const ZOrderPoint2DU32& zOrder) const
			{
				return m_nZValue < zOrder.m_nZValue;
			}
			bool operator <= (const ZOrderPoint2DU32& zOrder) const
			{
				return m_nZValue <= zOrder.m_nZValue;
			}
			bool operator > (const ZOrderPoint2DU32& zOrder) const
			{
				return m_nZValue > zOrder.m_nZValue;
			}
			ZOrderPoint2DU32& operator = (uint64_t nZorder)
			{
				m_nZValue = nZorder;
				return *this;
			}
			bool operator == (const ZOrderPoint2DU32&  Zorder) const
			{
				return m_nZValue == Zorder.m_nZValue;
			}
			short getBits() const
			{
				return 63;
			}

			bool IsInRect(const   TRect2Du32& rect) const
			{
				return IsZPointOrdertInRect<uint32_t, ZOrderPoint2DU32,  TRect2Du32>(rect, *this);
			}

			ZOrderPoint2DU32 operator - (const ZOrderPoint2DU32&  zOrder) const
			{
				return ZOrderPoint2DU32(m_nZValue - zOrder.m_nZValue);
			}
			ZOrderPoint2DU32 operator + (const ZOrderPoint2DU32&  zOrder) const
			{
				return ZOrderPoint2DU32(m_nZValue + zOrder.m_nZValue);
			}
			ZOrderPoint2DU32& operator += (const ZOrderPoint2DU32&  zOrder)
			{
				m_nZValue += zOrder.m_nZValue;
				return *this;
			}

			uint64_t m_nZValue;
		};

		template <class PontType>
		struct ZPointComp
		{
			bool LE(const PontType& _Left, const PontType& _Right) const
			{
				return (_Left.m_nZValue < _Right.m_nZValue);
			}
			bool EQ(const PontType& _Left, const PontType& _Right) const
			{
				return (_Left.m_nZValue == _Right.m_nZValue);
			}

			bool operator()(const PontType& _Left, const PontType& _Right)
			{
				return (_Left.m_nZValue < _Right.m_nZValue);
			}
		};


		struct ZOrderPoint2DU64
		{
			typedef uint64_t TPointType;
			typedef int64_t  TPointTypei;
			typedef  TPoint2D<uint64_t> TPoint;
			static const uint16_t SizeInByte = 16;
			ZOrderPoint2DU64()
			{
				m_nZValue[0] = 0;
				m_nZValue[1] = 0;
			}
			ZOrderPoint2DU64(uint64_t nXPos, uint64_t nYPos)
			{
				//m_nZValue[0] = nZorderMax;
				//m_nZValue[1] = nZorderMin;
				setZOrder(nXPos, nYPos);
			}
			ZOrderPoint2DU64(const TPoint& point)
			{
				setZOrder(point.m_x, point.m_y);
			}
			void setZOrder(uint64_t nXPos, uint64_t nYPos)
			{
				uint64_t nXmax = nXPos >> 32;
				uint64_t nXmin = nXPos & 0xFFFFFFFF;

				uint64_t nYmax = nYPos >> 32;
				uint64_t nYmin = nYPos & 0xFFFFFFFF;

				SetZOrderInt32(m_nZValue[1], (uint32_t)nXmax, (uint32_t)nYmax);
				SetZOrderInt32(m_nZValue[0], (uint32_t)nXmin, (uint32_t)nYmin);
			}
			void getXY(uint64_t& x, uint64_t& y) const;
			void getXY(TPoint& point) const;
			uint64_t m_nZValue[2];

			uint64_t getBit (int idx) const
			{
				int subIndex = idx > 63 ? idx - 64 : idx;
				return (m_nZValue[idx > 63 ? 1 : 0] >> (subIndex & 0x3f));
			}

			void setLowBits(int idx);
			void clearLowBits(int idx);

			bool operator < (const ZOrderPoint2DU64& zOrder) const
			{
				if(m_nZValue[1] != zOrder.m_nZValue[1] )
					return m_nZValue[1] < zOrder.m_nZValue[1];
				return m_nZValue[0] < zOrder.m_nZValue[0];
			}
			bool operator <= (const ZOrderPoint2DU64& zOrder) const
			{
				// high word decides unless equal, exactly like operator<
				if(m_nZValue[1] != zOrder.m_nZValue[1] )
					return m_nZValue[1] < zOrder.m_nZValue[1];
				return m_nZValue[0] <= zOrder.m_nZValue[0];
			}
			bool operator > (const ZOrderPoint2DU64& zOrder) const
			{
				if(m_nZValue[1] != zOrder.m_nZValue[1] )
					return m_nZValue[1] > zOrder.m_nZValue[1];
				return m_nZValue[0] > zOrder.m_nZValue[0];
			}
			bool operator == (const ZOrderPoint2DU64&  zOrder) const
			{
				return m_nZValue[1] == zOrder.m_nZValue[1] && m_nZValue[0] == zOrder.m_nZValue[0];
			}
			short getBits() const
			{
				return 127;
			}

			bool IsInRect(const   TRect2Du64& rect) const
			{
				return IsZPointOrdertInRect<uint64_t, ZOrderPoint2DU64,  TRect2Du64>(rect, *this);
			}

			ZOrderPoint2DU64 operator - (const ZOrderPoint2DU64&  zOrder) const;
			ZOrderPoint2DU64 operator + (const ZOrderPoint2DU64&  zOrder) const;
			ZOrderPoint2DU64& operator += (const ZOrderPoint2DU64&  zOrder);
		};


		struct ZPointComp64
		{
			bool LE(const ZOrderPoint2DU64& _Left, const ZOrderPoint2DU64& _Right) const
			{
				if(_Left.m_nZValue[1] != _Right.m_nZValue[1])
					return _Left.m_nZValue[1] < _Right.m_nZValue[1];
				return _Left.m_nZValue[0] < _Right.m_nZValue[0];
			}
			bool EQ(const ZOrderPoint2DU64& _Left, const ZOrderPoint2DU64& _Right) const
			{
				return (_Left.m_nZValue[0] == _Right.m_nZValue[0] && _Left.m_nZValue[1] == _Right.m_nZValue[1]);
			}
			bool operator()(const ZOrderPoint2DU64& _Left, const ZOrderPoint2DU64& _Right) const
			{
				return LE(_Left, _Right);
			}
		};


		// Finds the next Z value that lies inside the query rect, for a zVal that has
		// left the rect (BIGMIN).
		//   zMin / zMax : Z values of the rect's (minX, minY) and (maxX, maxY) corners
		//   zVal        : current Z value, must lie OUTSIDE the rect, zMin < zVal < zMax
		//   zRes        : receives the smallest Z value > zVal whose point is inside the rect
		// Returns false when zVal is not below zMax (nothing can follow it) or below zMin.
		template<class TZVal>
		bool FindMinZVal(const TZVal& zVal,
		const TZVal& zMin, const TZVal& zMax, TZVal& zRes)
		{
			// zMax is the last Z value inside the rect, so there is no "next" from it onwards
			if(zVal < zMin || !(zVal < zMax))
				return false;

			short nBits = zMin.getBits();

			TZVal left = zMin;
			TZVal right = zMax;
			zRes = zMax;

			while(nBits >= 0)
			{


				TZVal qMin = left;
				TZVal qMax = right;

				// highest bit where the two corners differ: that is where the rect splits
				while ((qMin.getBit (nBits) & 1) == (qMax.getBit (nBits) & 1))
				{

					nBits--;
					if (nBits < 0) // corners converged: only happens for a zVal inside the rect
						throw CommonLib::CExcBase("FindMinZVal: zVal must be outside the query rect");

				}

				qMin.clearLowBits(nBits);
				qMax.setLowBits(nBits);


				if(zVal < qMax)
				{
					right = qMax;
					zRes = qMax;
				}
				else
				{
					if(qMin > zVal)
					{
						zRes = qMin;
						break;
					}
					left = qMin;
					zRes = qMin;

				}
				nBits--;
			}

			return true;
		}


	}

}
