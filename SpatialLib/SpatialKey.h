#pragma once
#include "pch.h"
#include <limits>
namespace bptreedb {
    namespace spatiallib
    {


        template <typename _TPointType>
        struct TPoint2D
        {
        public:
            typedef _TPointType TPointType;
            TPointType m_x;
            TPointType m_y;

            bool operator ==(const TPoint2D& point) const
            {
                return m_x == point.m_x && m_y == point.m_y;
            }
            bool operator !=(const TPoint2D& point) const
            {
                return m_x != point.m_x || m_y != point.m_y;
            }
            // Points have no natural order (spatial keys are ordered by their Z-order value),
            // so < and > are always false and <= / >= reduce to equality. That keeps the
            // operators consistent: p <= p holds and (a <= b) == (a < b || a == b).
            bool operator <(const TPoint2D& point) const
            {
                return false;
            }
            bool operator >(const TPoint2D& point) const
            {
                return false;
            }
            bool operator <=(const TPoint2D& point) const
            {
                return *this == point;
            }
            bool operator >=(const TPoint2D& point) const
            {
                return *this == point;
            }


        };



        template <class _TPointType >
        struct TRect2D
        {
        public:
            typedef _TPointType TPointType;

            TPointType m_minX;
            TPointType m_minY;
            TPointType m_maxX;
            TPointType m_maxY;


            // An empty rect: min above max on both axes. Using the numeric limits (rather
            // than 0 / -1) keeps it empty for unsigned coordinate types too, where -1 would
            // wrap to the maximum value and produce a rect covering the whole plane.
            // The extra parentheses stop the max() macro from windows.h expanding here.
            TRect2D() :
                m_minX((std::numeric_limits<TPointType>::max)()), m_minY((std::numeric_limits<TPointType>::max)()),
                m_maxX((std::numeric_limits<TPointType>::lowest)()), m_maxY((std::numeric_limits<TPointType>::lowest)())
            {}
            TRect2D(TPointType minX, TPointType minY, TPointType maxX, TPointType maxY) :
            m_minX(minX),m_minY(minY), m_maxX(maxX), m_maxY(maxY)
            {}

            // 0 for an empty rect (avoids overflow on the lowest - max sentinel values)
            TPointType width()  const
            {
                return IsEmpty() ? TPointType(0) : TPointType(m_maxX - m_minX);
            }
            TPointType height()  const
            {
                return IsEmpty() ? TPointType(0) : TPointType(m_maxY - m_minY);
            }

            bool IsEmpty()  const
            {
                return (m_maxX < m_minX || (m_maxY < m_minY));
            }

            bool isIntersection(TRect2D& rect) const
            {

                if(IsEmpty() || rect.IsEmpty())
                    return false;

                if(m_minX >= rect.m_maxX) return false;
                if(m_maxX <= rect.m_minX) return false;
                if(m_minY >= rect.m_maxY) return false;
                if(m_maxY <= rect.m_minY) return false;
                return true;
            }

            void set(TPointType minX, TPointType minY, TPointType maxX, TPointType maxY)
            {
                m_minX = minX;
                m_minY = minY;
                m_maxX = maxX;
                m_maxY = maxY;
            }
            void expand(TPointType X, TPointType Y)
            {
                if(IsEmpty())
                {
                    m_minX = X;
                    m_minY = Y;
                    m_maxX = X;
                    m_maxY = Y;
                    return;
                }
                if(X > m_maxX)
                    m_maxX = X;
                else if(X < m_minX)
                    m_minX = X;

                if(Y > m_maxY)
                    m_maxY = Y;
                else if(Y < m_minY)
                    m_minY = Y;
            }
            bool isInRectEQ (const TRect2D& rect) const
            {

                if(m_minX <= rect.m_minX) return false;
                if(m_maxX >= rect.m_maxX) return false;
                if(m_minY <= rect.m_minY) return false;
                if(m_maxY >= rect.m_maxY) return false;
                return true;
            }
            bool isInRect(const TRect2D& rect) const
            {

                if(m_minX < rect.m_minX) return false;
                if(m_maxX > rect.m_maxX) return false;
                if(m_minY < rect.m_minY) return false;
                if(m_maxY > rect.m_maxY) return false;
                return true;
            }

            bool isPoinInRectEQ (TPointType X, TPointType Y) const
            {

                if(m_minX >= X) return false;
                if(m_maxX <= X) return false;
                if(m_minY >= Y) return false;
                if(m_maxY <= Y) return false;
                return true;
            }
            bool isPoinInRect (TPointType X, TPointType Y) const
            {

                if(m_minX > X) return false;
                if(m_maxX < X) return false;
                if(m_minY > Y) return false;
                if(m_maxY < Y) return false;
                return true;
            }


            bool operator ==(const TRect2D& rect) const
            {
                return m_minX == rect.m_minX && m_maxX == rect.m_maxX &&
                    m_minY == rect.m_minY && m_maxY == rect.m_maxY;
            }
            bool operator !=(const TRect2D& rect) const
            {
                return m_minX != rect.m_minX || m_maxX != rect.m_maxX ||
                    m_minY != rect.m_minY || m_maxY != rect.m_maxY;
            }
            // Same convention as TPoint2D: no natural order, <= / >= reduce to equality.
            bool operator <(const TRect2D& rect) const
            {
                return false;
            }
            bool operator >(const TRect2D& rect) const
            {
                return false;
            }
            bool operator <=(const TRect2D& rect) const
            {
                return *this == rect;
            }
            bool operator >=(const TRect2D& rect) const
            {
                return *this == rect;
            }


        };


        typedef TPoint2D<int16_t> TPoint2D16;
        typedef TPoint2D<int32_t> TPoint2D32;
        typedef TPoint2D<int64_t> TPoint2D64;

        typedef TPoint2D<uint16_t> TPoint2Du16;
        typedef TPoint2D<uint32_t> TPoint2Du32;
        typedef TPoint2D<uint64_t> TPoint2Du64;

        typedef TRect2D<int16_t> TRect2D16;
        typedef TRect2D<int32_t> TRect2D32;
        typedef TRect2D<int64_t> TRect2D64;

        typedef TRect2D<uint16_t> TRect2Du16;
        typedef TRect2D<uint32_t> TRect2Du32;
        typedef TRect2D<uint64_t> TRect2Du64;

    }
}