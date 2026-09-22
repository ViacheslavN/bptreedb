#pragma once
#include "SpatialKey.h"

namespace bptreedb {
	namespace spatiallib {

		template <class TPoint, class TZOrder, class TRect>
		bool IsZRectOrdertInRect(const TRect& rect, const TZOrder& zOrder)
		{
			TPoint xMin, xMax, yMin, yMax;
			zOrder.getXY(xMin, yMin,  xMax, yMax);
			TRect zRect(xMin, yMin, xMax, yMax);
			return rect.isIntersection(zRect) || rect.isInRect(zRect) || zRect.isInRect(rect);
		}

		struct ZOrderRect2DU16
		{
			typedef  int16_t TPointTypei;
			typedef  uint16_t TPointType;
			typedef  uint64_t ZValueType;
			typedef  TRect2D<uint16_t> TRect;
			static const TPointType coordMax = 0xFFFF;
			static const uint16_t SizeInByte = 8;
			ZOrderRect2DU16();
			ZOrderRect2DU16(ZValueType zValue);
			ZOrderRect2DU16(const TRect& rect);
			ZOrderRect2DU16(uint16_t xMin, uint16_t yMin, uint16_t xMax, uint16_t yMax);
			void setZOrder(uint16_t xMin, uint16_t yMin, uint16_t xMax, uint16_t yMax);
			void getXY(uint16_t& xMin, uint16_t& yMin, uint16_t& xMax, uint16_t& yMax) const;
			void getXY(TRect& rect) const;

			uint64_t getBit (int idx)
			{
				return uint64_t((m_nZValue >> (idx & 0x3f)) & 0x01);
			}

			void setLowBits(int idx);
			void clearLowBits(int idx);
			short getBits() const
			{
				return 63;
			}

			bool operator < (const ZOrderRect2DU16& zOrder) const
			{
				return m_nZValue < zOrder.m_nZValue;
			}
			bool operator <= (const ZOrderRect2DU16& zOrder) const
			{
				return m_nZValue <= zOrder.m_nZValue;
			}
			bool operator > (const ZOrderRect2DU16& zOrder) const
			{
				return m_nZValue > zOrder.m_nZValue;
			}
			bool operator == (const ZOrderRect2DU16&  Zorder) const
			{
				return m_nZValue == Zorder.m_nZValue;
			}


			ZOrderRect2DU16 operator - (const ZOrderRect2DU16&  Zorder) const
			{
				return ZOrderRect2DU16(m_nZValue - Zorder.m_nZValue);
			}
			ZOrderRect2DU16 operator + (const ZOrderRect2DU16&  Zorder) const
			{
				return ZOrderRect2DU16(m_nZValue + Zorder.m_nZValue);
			}

			const ZOrderRect2DU16& operator += (const ZOrderRect2DU16&  Zorder)
			{
				m_nZValue += Zorder.m_nZValue;
				return *this;
			}

			bool IsInRect(const TRect2Du16& rect) const
			{
				return IsZRectOrdertInRect<uint16_t, ZOrderRect2DU16, TRect2Du16>(rect, *this);
			}

			uint64_t m_nZValue;
		};

		struct ZRect16Comp
		{
			bool LE(const ZOrderRect2DU16& _Left, const ZOrderRect2DU16& _Right) const
			{

				return _Left.m_nZValue < _Right.m_nZValue;
			}
			bool EQ(const ZOrderRect2DU16& _Left, const ZOrderRect2DU16& _Right) const
			{
				return _Left.m_nZValue == _Right.m_nZValue;
			}

			bool operator()(const ZOrderRect2DU16& _Left, const ZOrderRect2DU16& _Right) const
			{
				return LE(_Left, _Right);
			}
		};

		struct ZOrderRect2DU32
		{
			typedef  int32_t TPointTypei;
			typedef  uint32_t TPointType;
			static const uint16_t SizeInByte = 16;
			static const TPointType coordMax = 0xFFFFFFFF;
			typedef TRect2D<uint32_t> TRect;
			ZOrderRect2DU32();
			ZOrderRect2DU32(const TRect& rect);
			ZOrderRect2DU32(uint32_t xMin, uint32_t yMin, uint32_t xMax, uint32_t yMax);
			void setZOrder(uint32_t xMin, uint32_t yMin, uint32_t xMax, uint32_t yMax) ;
			void getXY(uint32_t& xMin, uint32_t& yMin, uint32_t& xMax, uint32_t& yMax) const;
			void getXY(TRect& rect) const;
			short getBits() const
			{
				return 127;
			}

			bool operator < (const ZOrderRect2DU32& zOrder) const
			{
				if(m_nZValue[1] != zOrder.m_nZValue[1] )
					return m_nZValue[1] < zOrder.m_nZValue[1];
				return m_nZValue[0] < zOrder.m_nZValue[0];
			}
			bool operator <= (const ZOrderRect2DU32& zOrder) const
			{
				// high word decides unless equal, exactly like operator<
				if(m_nZValue[1] != zOrder.m_nZValue[1] )
					return m_nZValue[1] < zOrder.m_nZValue[1];
				return m_nZValue[0] <= zOrder.m_nZValue[0];
			}
			bool operator > (const ZOrderRect2DU32& zOrder) const
			{
				if(m_nZValue[1] != zOrder.m_nZValue[1] )
					return m_nZValue[1] > zOrder.m_nZValue[1];
				return m_nZValue[0] > zOrder.m_nZValue[0];
			}
			bool operator == (const ZOrderRect2DU32&  zOrder) const
			{
				return m_nZValue[1] == zOrder.m_nZValue[1] && m_nZValue[0] == zOrder.m_nZValue[0];
			}

			uint64_t getBit (int idx)
			{
				int subIndex = idx > 63 ? idx - 64 : idx;
				return (m_nZValue[idx > 63 ? 1 : 0] >> (subIndex & 0x3f));
			}

			void setLowBits(int idx);
			void clearLowBits(int idx);

			bool IsInRect(const TRect2Du32& rect) const
			{
				return IsZRectOrdertInRect<uint32_t, ZOrderRect2DU32, TRect2Du32>(rect, *this);
			}


			ZOrderRect2DU32 operator - (const ZOrderRect2DU32&  Zorder) const;
			ZOrderRect2DU32 operator + (const ZOrderRect2DU32&  Zorder) const;
			ZOrderRect2DU32& operator += (const ZOrderRect2DU32&  Zorder);


			uint64_t m_nZValue[2];
		};




		struct ZRect32Comp
		{
			bool LE(const ZOrderRect2DU32& _Left, const ZOrderRect2DU32& _Right) const
			{
				if(_Left.m_nZValue[1] != _Right.m_nZValue[1] )
					return _Left.m_nZValue[1] < _Right.m_nZValue[1];
				return _Left.m_nZValue[0] < _Right.m_nZValue[0];
			}
			bool EQ(const ZOrderRect2DU32& _Left, const ZOrderRect2DU32& _Right) const
			{
				return _Left.m_nZValue[1] == _Right.m_nZValue[1] && _Left.m_nZValue[0] == _Right.m_nZValue[0];
			}

			bool operator()(const ZOrderRect2DU32& _Left, const ZOrderRect2DU32& _Right) const
			{
				return LE(_Left, _Right);
			}

		};

		struct ZOrderRect2DU64
		{
			typedef  int64_t TPointTypei;
			typedef  uint64_t TPointType;
			static const uint16_t SizeInByte = 32;
			static const TPointType coordMax = 0xFFFFFFFFFFFFFFFF;
			typedef TRect2D<uint64_t> TRect;
			ZOrderRect2DU64();
			ZOrderRect2DU64(const TRect& rect);
			ZOrderRect2DU64(uint64_t xMin, uint64_t yMin, uint64_t xMax, uint64_t yMax);
			void setZOrder(uint64_t xMin, uint64_t yMin, uint64_t xMax, uint64_t yMax);
			void getXY(uint64_t& xMin, uint64_t& yMin, uint64_t& xMax, uint64_t& yMax) const;
			void getXY(TRect& rect) const;

			short getBits() const
			{
				return 255;
			}

			bool less(const ZOrderRect2DU64& zOrder) const
			{
				for(int i = 3; i >= 0; i--)
				{
					if(m_nZValue[i]>zOrder.m_nZValue[i])
						return false;
					if(m_nZValue[i]<zOrder.m_nZValue[i])
						return true;
				}
				return false;
			}

			bool operator < (const ZOrderRect2DU64& zOrder) const
			{
				for(int i = 3; i >= 0; i--)
				{
					if(m_nZValue[i]>zOrder.m_nZValue[i])
						return false;
					if(m_nZValue[i]<zOrder.m_nZValue[i])
						return true;
				}
				return false;
			}
			bool operator <= (const ZOrderRect2DU64& zOrder) const
			{
				if(m_nZValue[3] == zOrder.m_nZValue[3] && m_nZValue[2] == zOrder.m_nZValue[2] && m_nZValue[1] == zOrder.m_nZValue[1] && m_nZValue[0] == zOrder.m_nZValue[0])
					return true;

				if(m_nZValue[3] != zOrder.m_nZValue[3] )
					return m_nZValue[3] < zOrder.m_nZValue[3];
				if(m_nZValue[2] != zOrder.m_nZValue[2] )
					return m_nZValue[2] < zOrder.m_nZValue[2];
				if(m_nZValue[1] != zOrder.m_nZValue[1] )
					return m_nZValue[1] < zOrder.m_nZValue[1];
				return m_nZValue[0] < zOrder.m_nZValue[0];
			}
			bool operator > (const ZOrderRect2DU64& zOrder) const
			{
				return zOrder.less(*this); // !less() would be >=, making z > z true
			}

			bool operator == (const ZOrderRect2DU64&  zOrder) const
			{
				return m_nZValue[3] == zOrder.m_nZValue[3] && m_nZValue[2] == zOrder.m_nZValue[2] && m_nZValue[1] == zOrder.m_nZValue[1] && m_nZValue[0] == zOrder.m_nZValue[0];
			}

			uint64_t getBit (int idx);
			void setLowBits(int idx);
			void clearLowBits(int idx);


			bool IsInRect(const TRect2Du64& rect) const
			{
				return IsZRectOrdertInRect<uint64_t, ZOrderRect2DU64, TRect2Du64>(rect, *this);
			}


			ZOrderRect2DU64 operator - (const ZOrderRect2DU64&  zOrder) const;
			ZOrderRect2DU64 operator + (const ZOrderRect2DU64&  zOrder) const;
			ZOrderRect2DU64& operator += (const ZOrderRect2DU64&  zOrder);

			uint64_t m_nZValue[4];
		};

		struct ZRect64Comp
		{
			bool LE(const ZOrderRect2DU64& _Left, const ZOrderRect2DU64& _Right) const
			{

				for(int i = 3; i >= 0; i--)
				{
					if(_Left.m_nZValue[i]>_Right.m_nZValue[i])
						return false;
					if(_Left.m_nZValue[i]<_Right.m_nZValue[i])
						return true;
				}
				return false;


			}
			bool EQ(const ZOrderRect2DU64& _Left, const ZOrderRect2DU64& _Right) const
			{
				return _Left.m_nZValue[3] == _Right.m_nZValue[3] &&  _Left.m_nZValue[2] == _Right.m_nZValue[2] && _Left.m_nZValue[1] == _Right.m_nZValue[1] &&
					_Left.m_nZValue[1] == _Right.m_nZValue[1] && _Left.m_nZValue[0] == _Right.m_nZValue[0];
			}

			bool operator()(const ZOrderRect2DU64& _Left, const ZOrderRect2DU64& _Right) const
			{
				return LE(_Left, _Right);
			}
		};

template<class TZVal>
bool FindRectMinZVal(const TZVal& zVal,
	const TZVal& zMin, const TZVal& zMax, TZVal& zRes)
		{
			if(zVal < zMin || zVal > zMax)
			{
				return false;
			}


			short nBits = zRes.getBits();

			TZVal left = zMin;
			TZVal right = zMax;
			zRes = zMax;


			while(nBits >= 0)
			{


				TZVal qMin = left;
				TZVal qMax = right;

				while (qMin.getBit (nBits) == qMax.getBit (nBits))
				{

					nBits--;
					if(nBits < 0)
					{
						throw CommonLib::CExcBase("FindRectMinZVal: znBits < 0");
					}

				}
				qMin.clearLowBits(nBits);
				qMax.setLowBits(nBits);
				--nBits;
				if(qMin < qMax)
				{

					zRes = qMin;
					break;
				}

				if(zVal < qMax)
				{
					right = qMax;
					zRes = qMax;
				}
				else
				{
					left = qMin;
					zRes = qMin;
					if(qMin > zVal)
					{
						break;
					}
				}
			}

			return true;
		}



	}
}