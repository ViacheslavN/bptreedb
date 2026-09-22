#include "../../../CommonLib/CommonLib.h"
#include "../../../CommonLib/exception/exc_base.h"
#include "../../ZCurve/Rect/SpatialRectQuery.h"

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <string>

using namespace bptreedb::spatiallib;

// ---------------------------------------------------------------------------
// Test data: arrays of rects stored under their 4D Z-order key
// ---------------------------------------------------------------------------

// A grid of rects of `size`, laid out every `step` units over 0..worldMax. With
// size > step the rects overlap each other, which is what makes a window query
// interesting: many of them straddle the query border.
template<class ZOrder, class Type>
std::vector<ZOrder> CreateRectGrid(Type worldMax, Type step, Type size)
{
    std::vector<ZOrder> vecRect;
    for (Type x = 0; x + size <= worldMax; x = Type(x + step))
    {
        for (Type y = 0; y + size <= worldMax; y = Type(y + step))
            vecRect.push_back(ZOrder(x, y, Type(x + size), Type(y + size)));
    }

    std::sort(vecRect.begin(), vecRect.end());
    return vecRect;
}

// Randomly placed rects of random size, for a less regular distribution.
template<class ZOrder, class Type>
std::vector<ZOrder> CreateRandomRects(size_t count, Type worldMax, Type maxSize, uint32_t seed)
{
    std::mt19937_64 rng(seed);
    std::vector<ZOrder> vecRect;
    vecRect.reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
        const Type w = Type(rng() % maxSize) + 1;
        const Type h = Type(rng() % maxSize) + 1;
        const Type x = Type(rng() % Type(worldMax - w));
        const Type y = Type(rng() % Type(worldMax - h));
        vecRect.push_back(ZOrder(x, y, Type(x + w), Type(y + h)));
    }

    std::sort(vecRect.begin(), vecRect.end());
    return vecRect;
}

// ---------------------------------------------------------------------------
// Query key ranges
// ---------------------------------------------------------------------------

enum enKeyRange
{
    krMinCorner,  // rects whose min corner lies inside the window
    krIntersect,  // every rect that overlaps the window
};

// The 4D box that the scan walks: zKeyMin / zKeyMax are its corners.
template<class ZOrder, class TRect, class Type>
void MakeKeyRange(enKeyRange range, const TRect& extent, Type worldXMax, Type worldYMax,
    ZOrder& zKeyMin, ZOrder& zKeyMax)
{
    if (range == krMinCorner)
    {
        // xMin in [extent.minX, extent.maxX], yMin in [extent.minY, extent.maxY]
        zKeyMin = ZOrder(extent.m_minX, extent.m_minY, 0, 0);
        zKeyMax = ZOrder(extent.m_maxX, extent.m_maxY, worldXMax, worldYMax);
    }
    else
    {
        // overlap <=> xMin <= extent.maxX && yMin <= extent.maxY &&
        //             xMax >= extent.minX && yMax >= extent.minY
        zKeyMin = ZOrder(0, 0, extent.m_minX, extent.m_minY);
        zKeyMax = ZOrder(extent.m_maxX, extent.m_maxY, worldXMax, worldYMax);
    }
}

// ---------------------------------------------------------------------------
// Scans
// ---------------------------------------------------------------------------

struct SScanStats
{
    size_t nInRect = 0;   // keys reported as hits
    size_t nInOut = 0;    // keys read and thrown away
    size_t nTotal = 0;    // keys read
    size_t nSeeks = 0;    // FindRectMinZVal jumps
    double ms = 0;
};

// True when the rect stored in the key overlaps the window.
template<class ZOrder, class TRect, class Type>
bool IsOverlap(const ZOrder& zVal, TRect& extent)
{
    Type xMin, yMin, xMax, yMax;
    zVal.getXY(xMin, yMin, xMax, yMax);
    TRect rectFeature;
    rectFeature.set(xMin, yMin, xMax, yMax);
    return extent.isIntersection(rectFeature) || extent.isInRect(rectFeature) ||
        rectFeature.isInRect(extent);
}

template<class ZOrder, class TRect, class Type>
SScanStats TestIntersectRectFullScan(std::vector<ZOrder>& vecRect, TRect& extent,
    Type worldXMax, Type worldYMax, enKeyRange range, std::vector<ZOrder>* pFound = NULL)
{
    ZOrder zKeyMin, zKeyMax;
    MakeKeyRange<ZOrder, TRect, Type>(range, extent, worldXMax, worldYMax, zKeyMin, zKeyMax);

    SScanStats stats;
    const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    size_t nIndex = std::lower_bound(vecRect.begin(), vecRect.end(), zKeyMin) - vecRect.begin();
    for (size_t i = nIndex; i < vecRect.size(); ++i)
    {
        if (zKeyMax < vecRect[i])
            break;

        if (IsOverlap<ZOrder, TRect, Type>(vecRect[i], extent))
        {
            ++stats.nInRect;
            if (pFound)
                pFound->push_back(vecRect[i]);
        }
        else
            ++stats.nInOut;

        ++stats.nTotal;
    }

    stats.ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
    return stats;
}

template<class ZOrder, class TRect, class Type>
SScanStats TestIntersectRecttWithSubQueryScan(std::vector<ZOrder>& vecRect, TRect& extent,
    Type worldXMax, Type worldYMax, enKeyRange range, std::vector<ZOrder>* pFound = NULL)
{
    ZOrder zKeyMin, zKeyMax;
    MakeKeyRange<ZOrder, TRect, Type>(range, extent, worldXMax, worldYMax, zKeyMin, zKeyMax);

    SScanStats stats;
    const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    size_t i = std::lower_bound(vecRect.begin(), vecRect.end(), zKeyMin) - vecRect.begin();
    while (i < vecRect.size())
    {
        if (zKeyMax < vecRect[i])
            break;

        ++stats.nTotal;

        if (IsOverlap<ZOrder, TRect, Type>(vecRect[i], extent))
        {
            ++stats.nInRect;
            if (pFound)
                pFound->push_back(vecRect[i]);
            ++i;
            continue;
        }

        // Outside the window: ask for the next key inside the query box and seek there.
        ++stats.nInOut;

        // zKeyMax is the last key of the box, so nothing can follow it
        if (!(vecRect[i] < zKeyMax))
            break;

        ZOrder zQVal;
        if (!FindRectMinZVal(vecRect[i], zKeyMin, zKeyMax, zQVal))
            break;

        ++stats.nSeeks;

        const size_t index = std::lower_bound(vecRect.begin() + i, vecRect.end(), zQVal) - vecRect.begin();
        if (index <= i)
        {
            std::cout << "Error: index <= i" << std::endl;
            break;
        }

        i = index;
    }

    stats.ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
    return stats;
}

// Reference: every stored rect that overlaps the window, whatever its key.
template<class ZOrder, class TRect, class Type>
std::vector<ZOrder> BruteForce(std::vector<ZOrder>& vecRect, TRect& extent)
{
    std::vector<ZOrder> found;
    for (size_t i = 0; i < vecRect.size(); ++i)
        if (IsOverlap<ZOrder, TRect, Type>(vecRect[i], extent))
            found.push_back(vecRect[i]);
    return found;
}

// ---------------------------------------------------------------------------
// Reporting
// ---------------------------------------------------------------------------

namespace
{
    bool g_bFailed = false;

    void PrintHeader(const std::string& title)
    {
        std::cout << "\n=== " << title << " ===\n";
        std::cout << std::left << std::setw(16) << "scan"
            << std::right << std::setw(10) << "found"
            << std::setw(10) << "read"
            << std::setw(10) << "skipped"
            << std::setw(8) << "seeks"
            << std::setw(10) << "ms" << "\n";
    }

    void PrintStats(const std::string& name, const SScanStats& stats)
    {
        std::cout << std::left << std::setw(16) << name
            << std::right << std::setw(10) << stats.nInRect
            << std::setw(10) << stats.nTotal
            << std::setw(10) << stats.nInOut
            << std::setw(8) << stats.nSeeks
            << std::setw(10) << std::fixed << std::setprecision(2) << stats.ms << "\n";
    }

    void Check(const std::string& what, bool bOk)
    {
        std::cout << "  " << std::left << std::setw(58) << what << (bOk ? "OK" : "FAILED") << "\n";
        if (!bOk)
            g_bFailed = true;
    }
}

template<class ZOrder, class TRect, class Type>
void RunQuery(const std::string& title, std::vector<ZOrder>& vecRect, TRect& extent,
    Type worldXMax, Type worldYMax)
{
    std::vector<ZOrder> brute = BruteForce<ZOrder, TRect, Type>(vecRect, extent);

    PrintHeader(title);

    std::vector<ZOrder> foundFull, foundSkip;
    SScanStats full = TestIntersectRectFullScan<ZOrder, TRect, Type>(
        vecRect, extent, worldXMax, worldYMax, krIntersect, &foundFull);
    SScanStats skip = TestIntersectRecttWithSubQueryScan<ZOrder, TRect, Type>(
        vecRect, extent, worldXMax, worldYMax, krIntersect, &foundSkip);
    PrintStats("full", full);
    PrintStats("with sub-query", skip);

    Check("full scan finds every overlapping rect", foundFull.size() == brute.size());
    Check("sub-query scan finds the same rects", foundSkip == foundFull);
    Check("sub-query scan reads no more keys", skip.nTotal <= full.nTotal);

    // the narrower key range: only rects whose min corner is inside the window
    std::vector<ZOrder> foundCornerFull, foundCornerSkip;
    SScanStats cornerFull = TestIntersectRectFullScan<ZOrder, TRect, Type>(
        vecRect, extent, worldXMax, worldYMax, krMinCorner, &foundCornerFull);
    SScanStats cornerSkip = TestIntersectRecttWithSubQueryScan<ZOrder, TRect, Type>(
        vecRect, extent, worldXMax, worldYMax, krMinCorner, &foundCornerSkip);
    PrintStats("full (corner)", cornerFull);
    PrintStats("sub-q (corner)", cornerSkip);

    // The full scan applies the geometric filter over the whole z-range, so it still sees
    // everything; the sub-query scan jumps between keys of the 4D box, so it reports only
    // the rects whose min corner is inside the window. That is the range's real answer.
    Check("corner range returns a subset of the overlapping rects",
        foundCornerSkip.size() <= brute.size() &&
        std::includes(foundFull.begin(), foundFull.end(), foundCornerSkip.begin(), foundCornerSkip.end()));

    bool bMissesOnlyOutsideStarts = true;
    for (size_t i = 0; i < brute.size(); ++i)
    {
        if (std::binary_search(foundCornerSkip.begin(), foundCornerSkip.end(), brute[i]))
            continue;

        Type xMin, yMin, xMax, yMax;
        brute[i].getXY(xMin, yMin, xMax, yMax);
        if (!(xMin < extent.m_minX || yMin < extent.m_minY))
            bMissesOnlyOutsideStarts = false;
    }
    Check("every rect it misses starts left of or below the window", bMissesOnlyOutsideStarts);

    std::cout << "  (corner range found " << foundCornerSkip.size()
        << " of " << brute.size() << " overlapping rects)\n";
}

// ---------------------------------------------------------------------------

template<class ZOrder, class TRect, class Type>
void RunForType(const std::string& name, Type worldMax, Type step, Type size, Type queryLo, Type queryHi)
{
    std::cout << "\n\n##### " << name << " #####\n";

    std::vector<ZOrder> grid = CreateRectGrid<ZOrder, Type>(worldMax, step, size);
    std::cout << "grid rects: " << grid.size() << "  (size " << uint64_t(size)
        << " every " << uint64_t(step) << " over 0.." << uint64_t(worldMax) << ")\n";

    TRect extent(queryLo, queryLo, queryHi, queryHi);
    RunQuery<ZOrder, TRect, Type>(name + " grid, centre window", grid, extent, worldMax, worldMax);

    TRect corner(0, 0, Type(queryHi - queryLo), Type(queryHi - queryLo));
    RunQuery<ZOrder, TRect, Type>(name + " grid, corner window", grid, corner, worldMax, worldMax);

    std::vector<ZOrder> random = CreateRandomRects<ZOrder, Type>(grid.size(), worldMax, size, 1234);
    std::cout << "\nrandom rects: " << random.size() << "\n";
    RunQuery<ZOrder, TRect, Type>(name + " random, centre window", random, extent, worldMax, worldMax);
}

int main()
{
    try
    {
        // query window bounds are odd while the grid sits on even coordinates, so no rect
        // merely touches the window border
        RunForType<ZOrderRect2DU16, TRect2Du16, uint16_t>("ZOrderRect2DU16",
            60000, 200, 600, 13001, 41001);

        RunForType<ZOrderRect2DU32, TRect2Du32, uint32_t>("ZOrderRect2DU32",
            2000000, 4000, 12000, 300001, 900001);

        RunForType<ZOrderRect2DU64, TRect2Du64, uint64_t>("ZOrderRect2DU64",
            4000000000000ull, 8000000000ull, 24000000000ull, 600000000001ull, 1800000000001ull);
    }
    catch (std::exception& exc)
    {
        std::cout << "\nUnexpected exception: " << exc.what() << std::endl;
        return 2;
    }

    std::cout << "\n\n" << (g_bFailed ? "SOME CHECKS FAILED" : "All checks passed") << std::endl;
    return g_bFailed ? 1 : 0;
}
