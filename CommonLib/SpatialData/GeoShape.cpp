#include "stdafx.h"
#include "GeoShape.h"

namespace CommonLib
{
    bool isTypeSimple(eShapeType shapeType)
    {
        return shapeType <= shape_type_multipatch  && shapeType >= shape_type_null;
    }

    eShapeType CGeoShape::GetGeneralType(eShapeType _general_type)
    {
        eShapeType shapeType = _general_type;

        if (isTypeSimple(shapeType))
        {
            switch (shapeType)
            {
                case shape_type_null:
                    return shape_type_null;

                case shape_type_point:
                case shape_type_point_z:
                case shape_type_point_m:
                case shape_type_point_zm:
                    return shape_type_general_point;

                case shape_type_multipoint:
                case shape_type_multipoint_z:
                case shape_type_multipoint_m:
                case shape_type_multipoint_zm:
                    return shape_type_general_multipoint;

                case shape_type_polyline:
                case shape_type_polyline_z:
                case shape_type_polyline_m:
                case shape_type_polyline_zm:
                    return shape_type_general_polyline;

                case shape_type_polygon:
                case shape_type_polygon_z:
                case shape_type_polygon_m:
                case shape_type_polygon_zm:
                    return shape_type_general_polygon;

                case shape_type_multipatch:
                case shape_type_multipatch_m:
                    return shape_type_general_multipatch;
            }
        }

        return (eShapeType)(shapeType & shape_basic_type_mask);
    }


   /* void CGeoShape::GetTypeParams(eShapeType shapeType, eShapeType* pGenType, bool* has_z, bool* has_m, bool* has_curve, bool* has_id)
    {
        if (isTypeSimple(shapeType))
        {
            if (has_z != NULL)
                *has_z = false;
            if (has_m != NULL)
                *has_m = false;
            if (has_curve != NULL)
                *has_curve = false;
            if (has_id != NULL)
                *has_id = false;

            eShapeType gType;
            switch (shapeType)
            {
                case shape_type_point:
                case shape_type_point_m:
                case shape_type_point_zm:
                case shape_type_point_z:
                    gType = shape_type_general_point;
                    break;
                case shape_type_multipoint:
                case shape_type_multipoint_m:
                case shape_type_multipoint_zm:
                case shape_type_multipoint_z:
                    gType = shape_type_general_multipoint;
                    break;
                case shape_type_polyline:
                case shape_type_polyline_m:
                case shape_type_polyline_zm:
                case shape_type_polyline_z:
                    gType = shape_type_general_polyline;
                    break;
                case shape_type_polygon:
                case shape_type_polygon_m:
                case shape_type_polygon_zm:
                case shape_type_polygon_z:
                    gType = shape_type_general_polygon;
                    break;
                case shape_type_multipatch_m:
                case shape_type_multipatch:
                    gType = shape_type_general_multipatch;
                    break;
                default:
                    gType = shape_type_null;
            }

            if (gType != shape_type_null)
            {
                if (has_m != NULL)
                {
                    if (shapeType == shape_type_point_m ||
                        shapeType == shape_type_multipoint_m ||
                        shapeType == shape_type_polyline_m ||
                        shapeType == shape_type_polygon_m ||
                        shapeType == shape_type_multipatch_m ||
                        shapeType == shape_type_point_zm ||
                        shapeType == shape_type_multipoint_zm ||
                        shapeType == shape_type_polyline_zm ||
                        shapeType == shape_type_polygon_zm)
                        *has_m = true;
                }

                if (has_z != NULL)
                {
                    if (shapeType == shape_type_point_z ||
                        shapeType == shape_type_multipoint_z ||
                        shapeType == shape_type_polyline_z ||
                        shapeType == shape_type_polygon_z ||
                        shapeType == shape_type_multipatch ||
                        shapeType == shape_type_multipatch_m ||
                        shapeType == shape_type_point_zm ||
                        shapeType == shape_type_multipoint_zm ||
                        shapeType == shape_type_polyline_zm ||
                        shapeType == shape_type_polygon_zm)
                        *has_z = true;
                }
            }

            if (pGenType != NULL)
                *pGenType = gType;
        }
        else
        {
            if (pGenType != NULL)
                *pGenType = (eShapeType)(shapeType & shape_basic_type_mask);
            if (has_z != NULL)
            {
                if ((eShapeType)(shapeType & shape_basic_type_mask) == shape_type_general_multipatch)
                    *has_z = true;
                else
                    *has_z = (shapeType & shape_has_zs) != 0;
            }
            if (has_m != NULL)
                *has_m = (shapeType & shape_has_ms) != 0;
            if (has_curve != NULL)
            {
                if ((shapeType & shape_non_basic_modifier_mask) == 0 &&
                    (shapeType == shape_type_general_polyline || shapeType == shape_type_general_polygon))
                    *has_curve = true;
                else
                    *has_curve = (shapeType & shape_has_curves) != 0;
            }
            if (has_id != NULL)
                *has_id = (shapeType & shape_has_ids) != 0;
        }

    }*/

    uint32_t CGeoShape::CalcSize(eShapeType shapeType, uint32_t npoints, uint32_t nparts, uint32_t ncurves, uint32_t mpatchSpecificSize)
    {

        uint32_t size = 1; //flag
        eShapeType genType;
        bool has_z;
        bool has_m;
        bool has_curve;
        bool has_id;
        GetTypeParams(shapeType, &genType, &has_z, &has_m, &has_curve, &has_id);
        uint32_t flag_z = has_z ? 1 : 0;
        uint32_t flag_m = has_m ? 1 : 0;
        uint32_t flag_curve = has_curve ? 1 : 0;
        uint32_t flag_id = has_id ? 1 : 0;

        size += 2; // type
        if (shapeType != shape_type_null)
        {
            switch (genType)
            {
                case shape_type_general_polyline:
                case shape_type_general_polygon:
                    size += 8 * (2 * (2 + flag_z + flag_m)) + 4 + 4 + nparts * 4 + npoints * (8 * (2 + flag_z + flag_m) + 4 * flag_id) + (4 + ncurves * sizeof(segment_modifier_t)) * flag_curve;
                    break;
                case shape_type_general_point:
                    size += 8 * (2 + flag_z + flag_m) + 4 * flag_id;
                    break;
                case shape_type_general_multipoint:
                    size += 8 * (2 * (2 + flag_z + flag_m)) + 4 + npoints * (8 * (2 + flag_z + flag_m) + 4 * flag_id);
                    break;
                case shape_type_general_multipatch:
                    size += 8 * (2 * (2 + 1 + flag_m)) + 4 + 4 + nparts * 4 + nparts * 4 + npoints * (8 * (2 + 1 + flag_m) + 4 * flag_id);
                    if (!isTypeSimple(shapeType))
                        size += 4 + 4;
                    size += mpatchSpecificSize;
                    break;
            }
        }

        return size;
    }
    void CGeoShape::InitShapeBufferBuffer(unsigned char* buf, eShapeType shapeType, uint32_t npoints, uint32_t nparts, uint32_t ncurves)
    {
        eShapeType genType;
        bool has_z;
        bool has_m;
        bool has_curve;
        bool has_id;
        double* dbuf;

        GetTypeParams(shapeType, &genType, &has_z, &has_m, &has_curve, &has_id);

        *buf = (byte)(0); //reserve
        buf += 1;

        // Type
        *reinterpret_cast<short*>(buf) = shapeType;
        buf += 2;
        if (shapeType == shape_type_null || genType == shape_type_null)
            return;

        if (genType == shape_type_general_point)
        {
            *reinterpret_cast<double*>(buf) = 0;
            return;
        }

        // Bounding box
        dbuf = reinterpret_cast<double*>(buf);
        *dbuf++ = 0.;
        *dbuf++ = 0.;
        *dbuf++ = 0.;
        *dbuf++ = 0.;

        buf += 8 * 4;

        // part count
        if (genType != shape_type_general_multipoint)
        {
            *reinterpret_cast<long*>(buf) = static_cast<long>(nparts);
            buf += 4;
        }
        // point count
        *reinterpret_cast<long*>(buf) = static_cast<long>(npoints);
        buf += 4;

        // parts starts
        if (nparts > 1)
        {
            if (genType != shape_type_general_multipoint)
                buf += 4 * nparts;

            // parts types
            if (genType == shape_type_general_multipatch)
                buf += 4 * nparts;
        }

        // x,y of points
        buf += 8 * 2 * npoints;

        if (has_z)
        {
            // range of z
            dbuf = reinterpret_cast<double*>(buf);
            *dbuf++ = 0.;
            *dbuf++ = 0.;
            buf += 8 * 2;

            // z of points
            buf += 8 * npoints;
        }
        if ((genType == shape_type_general_multipatch) && (!isTypeSimple(shapeType)))
        {
            if (has_m)
                *reinterpret_cast<long*>(buf) = static_cast<long>(npoints);
            else
                *reinterpret_cast<long*>(buf) = 0;
            buf += 4;
        }

        if (has_m)
        {
            dbuf = reinterpret_cast<double*>(buf);
            *dbuf++ = 0.;
            *dbuf++ = 0.;
            buf += 8 * 2; // range of m


            buf += 8 * npoints;
        }

        if (has_curve)
        {
            *reinterpret_cast<long*>(buf) = static_cast<long>(ncurves);
            buf += 4;
        }
        if ((genType == shape_type_general_multipatch) && (!isTypeSimple(shapeType))) //numIds
        {
            if (has_id)
                *reinterpret_cast<long*>(buf) = static_cast<long>(npoints);
            else
                *reinterpret_cast<long*>(buf) = 0;
            buf += 4;
        }
    }

    CGeoShape::CGeoShape(IAllocPtr  pAlloc) : m_blob(pAlloc)
    {
    }

    CGeoShape::CGeoShape(const CGeoShape& geoShp) : m_blob(geoShp.m_blob)
    {}

    CGeoShape& CGeoShape::operator=(const CGeoShape& shp)
    {
        if (this == &shp)
            return *this;

        m_blob = shp.m_blob;
        m_params.Reset();

        m_params.Set(m_blob.Buffer());


        return *this;
    }

    CGeoShape::~CGeoShape()
    {}

    void CGeoShape::Clear()
    {
        m_params.Reset();
        m_blob.Resize(0);
    }


    void CGeoShape::Write(IWriteStream *pStream) const
    {
        pStream->Write(m_blob.Size());
        pStream->Write(m_blob.Buffer(), m_blob.Size());
    }

    void CGeoShape::Read(IReadStream *pStream)
    {
        m_blob.Resize(pStream->ReadIntu32());
        if(m_blob.Size())
            pStream->Read(m_blob.Buffer(), m_blob.Size());

    }

    eShapeType CGeoShape::Type() const
    {
        if (m_params.m_bIsValid)
            return m_params.m_type;

        if (m_blob.Empty())
            return shape_type_null;

        return Type(m_blob.Buffer());
    }

    eShapeType CGeoShape::Type(const byte* buf)
    {
        if (buf == 0)
            return shape_type_null;

        return(eShapeType)(*reinterpret_cast<const short*>(buf + 1));
    }

    eShapeType CGeoShape::GeneralType() const
    {
        return GeneralType(m_blob.Buffer());
    }

    eShapeType CGeoShape::GeneralType(const byte* buf)
    {
        eShapeType shapeType = Type(buf);
        return GetGeneralType(shapeType);
    }
    uint32_t  CGeoShape::Size() const
    {
        return m_blob.Size();
    }

    const byte_t * CGeoShape::Data() const
    {
        return  m_blob.Buffer();
    }

    uint32_t  CGeoShape::GetPartCount() const
    {

        if (m_params.m_bIsValid)
            return m_params.m_nPartCount;

        return PartCount(m_blob.Buffer());
    }


    uint32_t CGeoShape::PartCount(const unsigned char* buf)
    {
        eShapeType genType = GeneralType(buf);
        return PartCount(buf, genType);
    }

    uint32_t CGeoShape::PartCount(const unsigned char* buf, eShapeType _general_type)
    {
        eShapeType genType = _general_type;

        switch (genType)
        {
            case shape_type_general_polyline:
            case shape_type_general_polygon:
            case shape_type_general_multipatch:
            {
                //const unsigned char* buf = cbuffer();
                //buf += 4 + 8 * 4;
                return *reinterpret_cast<const long*>(buf + 1 + 2 + 8 * 4); // flag type bbox
            }
        }

        return 0;
    }

    uint32_t  CGeoShape::GetPart(uint32_t idx) const
    {
        uint32_t nparts = GetPartCount();

        if (nparts == 0 || idx >= nparts)
            return 0;

        if (nparts == 1)
            return PointCount();

        const uint32_t* partStarts = GetParts();

        if (idx == nparts - 1)
            return PointCount() - (uint32_t)partStarts[idx];
        else
            return (uint32_t)partStarts[idx + 1] - (uint32_t)partStarts[idx];
    }

    const uint32_t*  CGeoShape::GetParts() const
    {
        return GetParts(m_blob.Buffer());
    }

    uint32_t*  CGeoShape::GetParts()
    {
        return GetParts(m_blob.Buffer());
    }


    const uint32_t*  CGeoShape::GetParts(const byte *pBuf) const
    {

        eShapeType genType = GeneralType();

        switch (genType)
        {
            case shape_type_general_multipatch:
            case shape_type_general_polyline:
            case shape_type_general_polygon:
            {

                uint32_t nparts = GetPartCount();
                if (nparts < 2)
                    return nullptr;


                pBuf += 1 + 2 + 8 * 4 + 4 + 4; //flag, type, bbox, part cnt, point cnt
                return reinterpret_cast<const uint32_t*>(pBuf);
            }
        }

        return nullptr;
    }

    uint32_t*  CGeoShape::GetParts(byte *pBuf)
    {

        eShapeType genType = GeneralType();

        switch (genType)
        {
            case shape_type_general_multipatch:
            case shape_type_general_polyline:
            case shape_type_general_polygon:
            {
                uint32_t nparts = GetPartCount();
                if (nparts < 2)
                    return nullptr;


                pBuf += 1 + 2 + 8 * 4 + 4 + 4; //flag type bbox part cnt, point cnt
                return reinterpret_cast<uint32_t*>(pBuf);
            }
        }

        return nullptr;
    }


    patch_type*  CGeoShape::GetPartsTypes()
    {


        eShapeType genType = GeneralType();
        if (genType == shape_type_general_multipatch)
        {
            unsigned char* buf = m_blob.Buffer();
            buf += 1 + 2 + 8 * 4 + 4 + GetPointCnt() * 4; //flag, type, bbox part cnt, parts
            return reinterpret_cast<patch_type*>(buf);
        }

        return nullptr;
    }
    const patch_type* CGeoShape::GetPartsTypes() const
    {


        eShapeType genType = GeneralType();
        if (genType == shape_type_general_multipatch)
        {
            unsigned char* buf = m_blob.Buffer();
            buf += 1 + 2 + 8 * 4 + 4 + GetPointCnt() * 4; //flag, type, bbox part cnt, parts
            return reinterpret_cast<const patch_type*>(buf);
        }

        return nullptr;
    }

    const patch_type CGeoShape::PartType(uint32_t idx) const
    {
        return GetPartsTypes()[idx];
    }


    GisXYPoint* CGeoShape::GetPoints()
    {


        if (m_params.m_bIsValid)
            return const_cast<GisXYPoint*>(m_params.m_pPoints);
        return const_cast<GisXYPoint*>(GetXYs(m_blob.Buffer()));
    }
    const GisXYPoint* CGeoShape::GetPoints() const
    {


        if (m_params.m_bIsValid)
            return m_params.m_pPoints;
        return GetXYs(m_blob.Buffer());
    }

    uint32_t CGeoShape::GetPointCnt() const
    {

        if (m_params.m_bIsValid)
            return m_params.m_nPointCount;

        return PointCount(m_blob.Buffer());
    }

    void CGeoShape::Create(uint32_t nSize)
    {
        m_blob.Reserve(nSize);
        m_params.Reset();
    }

    void CGeoShape::Create(eShapeType shapeType)
    {
        Create(shapeType, 0);

    }

    void CGeoShape::Create(unsigned char* pBuf, uint32_t nSize, eShapeType shapeType, uint32_t npoints, uint32_t nparts, uint32_t ncurves)
    {
        InitShapeBufferBuffer(pBuf, shapeType, npoints, nparts, ncurves);
        m_params.Set(pBuf);
    }


    void CGeoShape::Create(eShapeType shapeType, uint32_t npoints, uint32_t nparts, uint32_t ncurves, uint32_t mpatchSpecificSize)
    {


        uint32_t  nBufSize = CalcSize(shapeType, npoints, nparts, ncurves, mpatchSpecificSize);
        if (nBufSize)
        {
            m_blob.Resize(nBufSize);
            InitShapeBufferBuffer(m_blob.Buffer(), shapeType, npoints, nparts, ncurves);
        }
        m_params.Set(m_blob.Buffer());
    }

    void CGeoShape::Import(const byte* extBuf, uint32_t extBufSize)
    {

        m_blob.Copy(extBuf, extBufSize);
        m_params.Set(m_blob.Buffer());

    }

    void CGeoShape::Attach(byte* extBuf, uint32_t extBufSize)
    {
        m_blob.Attach(extBuf, extBufSize);
        m_params.Set(m_blob.Buffer());
    }

    unsigned char* CGeoShape::Detach()
    {
        m_params.Reset();
        return m_blob.Deattach();
    }

    uint32_t CGeoShape::PointCount() const
    {
        if (m_params.m_bIsValid)
            return m_params.m_nPointCount;
        return PointCount(m_blob.Buffer());
    }

    uint32_t CGeoShape::PointCount(const byte* buf)
    {
        eShapeType genType = GeneralType(buf);
        return PointCount(buf, genType);
    }

    uint32_t CGeoShape::PointCount(const byte* buf, eShapeType general_type)
    {
        eShapeType genType = general_type;
        long flag_has_parts = 1;

        switch (genType)
        {
            case shape_type_general_point:
                return 1;
            case shape_type_general_multipoint:
                flag_has_parts = 0;
            case shape_type_general_polyline:
            case shape_type_general_polygon:
            case shape_type_general_multipatch:
            {
                return *reinterpret_cast<const uint32_t*>(buf + 1 + 2 + 8 * 4 + 4 * flag_has_parts); // flag type bbox npart_cnt,
            }
        }

        return 0;
    }

    const GisXYPoint* CGeoShape::GetXYs(const unsigned char* buf)
    {
        eShapeType genType = GeneralType(buf);
        uint32_t nparts = PartCount(buf);
        return GetXYs(buf, genType, nparts);
    }

    const GisXYPoint* CGeoShape::GetXYs(const unsigned char* buf, eShapeType general_type, uint32_t partCount)
    {
        eShapeType genType = general_type;
        uint32_t nparts = partCount;

        buf += 1 + 2; //flag, type
        switch (genType)
        {
            case shape_type_general_point:
                buf += 4; //cnt
                break;
            case shape_type_general_multipoint:
                buf += 8 * 4 + 4; //bbox, cnt
                break;
            case shape_type_general_polyline:
            case shape_type_general_polygon:
                buf += 8 * 4 + 4 + 4 + 4 * nparts; //bbox, part cnt, point cnt, parts,
                break;
            case shape_type_general_multipatch:
                buf += 8 * 4 + 4 + 4 + (4 + 4) * nparts; //bbox, part cnt, point cnt, parts, type parts,
                break;
            default:
                return nullptr;
        }

        return reinterpret_cast<const GisXYPoint*>(buf);
    }

    GisXYPoint* CGeoShape::GetXYs(byte* buf)
    {
        eShapeType genType = GeneralType(buf);
        uint32_t nparts = PartCount(buf);
        return GetXYs(buf, genType, nparts);
    }
    GisXYPoint* CGeoShape::GetXYs(byte* buf, eShapeType  general_type, uint32_t partCount)
    {
        eShapeType genType = general_type;
        uint32_t nparts = partCount;

        buf += 1 + 2; //flag, type
        switch (genType)
        {
            case shape_type_general_point:
                buf += 4; //cnt
                break;
            case shape_type_general_multipoint:
                buf += 8 * 4 + 4; //bbox, cnt
                break;
            case shape_type_general_polyline:
            case shape_type_general_polygon:
                buf += 8 * 4 + 4 + 4 + 4 * nparts; //bbox, part cnt, point cnt, parts,
                break;
            case shape_type_general_multipatch:
                buf += 8 * 4 + 4 + 4 + (4 + 4) * nparts; //bbox, part cnt, point cnt, parts, type parts,
                break;
            default:
                return nullptr;
        }

        return reinterpret_cast<GisXYPoint*>(buf);
    }

    double* CGeoShape::GetZs()
    {
        return NULL;
    }
    const double* CGeoShape::GetZs() const
    {
        return NULL;
    }

    double* CGeoShape::GetMs()
    {
        return NULL;
    }
    const double* CGeoShape::GetMs() const
    {
        return NULL;
    }

    void  CGeoShape::Add(IGeoShapePtr ptrShap)
    {

    }

    ////////////////////////////////////////////////////////

    CGeoShape::sShapeParams::sShapeParams()
            : m_bIsValid(false), m_type(shape_type_null), m_general_type(shape_type_null), m_nPointCount(0), m_nPartCount(0), m_pPoints(0)
    {}

    void CGeoShape::sShapeParams::Reset()
    {

        m_bIsValid = false;
        m_type = shape_type_null;
        m_general_type = shape_type_null;
        m_nPointCount = 0;
        m_nPartCount = 0;
        m_pPoints = NULL;
    }

    void CGeoShape::sShapeParams::Set(const byte* buf)
    {
        m_bIsValid = true;

        // shape type
        m_type = CGeoShape::Type(buf);
        m_general_type = GetGeneralType(m_type);
        m_nPointCount = CGeoShape::PointCount(buf, m_general_type);
        m_nPartCount = CGeoShape::PartCount(buf, m_general_type);
        m_pPoints = CGeoShape::GetXYs(buf, m_general_type, m_nPartCount);
    }

    void CGeoShape::CalcBB()
    {

        eShapeType genType;
        eShapeType shType = Type();
        bool has_z;
        bool has_m;
        bool has_curve;
        bool has_id;
        GetTypeParams(shType, &genType, &has_z, &has_m, &has_curve, &has_id);

        if (genType == shape_type_null || shType == shape_type_null || genType == shape_type_general_point)
            return;

        size_t npoints = PointCount();
        if (npoints == 0)
            return;

        double* xmin = NULL;
        double* xmax = NULL;
        double* ymin = NULL;
        double* ymax = NULL;
        double* zmin = NULL;
        double* zmax = NULL;
        double* mmin = NULL;
        double* mmax = NULL;

        GisXYPoint* xys = GetPoints();
        double* xy_bounds = GetBBoxVals(genType);
        xmin = xy_bounds + 0;
        ymin = xy_bounds + 1;
        xmax = xy_bounds + 2;
        ymax = xy_bounds + 3;

        *xmin = DBL_MAX;
        *ymin = DBL_MAX;
        *xmax = -DBL_MAX;
        *ymax = -DBL_MAX;

        for (size_t pt = 0; pt < npoints; ++pt)
        {
            *xmin = min(*xmin, xys[pt].x);
            *ymin = min(*ymin, xys[pt].y);
            *xmax = max(*xmax, xys[pt].x);
            *ymax = max(*ymax, xys[pt].y);
        }

        if (has_z)
        {
            double* zs = GetZs();
            double* z_bounds = zs - 2;
            zmin = z_bounds + 0;
            zmax = z_bounds + 1;
            *zmin = DBL_MAX;
            *zmax = -DBL_MAX;
            for (size_t pt = 0; pt < npoints; ++pt)
            {
                *zmin = min(*zmin, zs[pt]);
                *zmax = max(*zmax, zs[pt]);
            }
        }

        if (has_m)
        {
            double* ms = GetMs();
            double* m_bounds = ms - 2;
            mmin = m_bounds + 0;
            mmax = m_bounds + 1;
            *mmin = DBL_MAX;
            *mmax = -DBL_MAX;
            for (size_t pt = 0; pt < npoints; ++pt)
            {
                *mmin = min(*mmin, ms[pt]);
                *mmax = max(*mmax, ms[pt]);
            }
        }
        if (has_curve)
        {
            //TO DO
        }
    }



    bbox CGeoShape::GetBB() const
    {
        bbox bb;

        auto xy_bounds = GetBBoxVals(GeneralType());
        if (!xy_bounds)
            return bb;

        bb.type = CommonLib::bbox_type_normal;
        bb.xMin = *xy_bounds;
        bb.yMin = *(xy_bounds + 1);
        bb.xMax = *(xy_bounds + 2);
        bb.yMax = *(xy_bounds + 3);

        return bb;

    }

    double *CGeoShape::GetBBoxVals()
    {
        eShapeType genType;
        GetTypeParams(Type(), &genType);
        return GetBBoxVals(genType);
    }
    const double *CGeoShape::GetBBoxVals() const
    {
        eShapeType genType;
        GetTypeParams(Type(), &genType);
        return GetBBoxVals(genType);
    }

    double *CGeoShape::GetBBoxVals(eShapeType genType)
    {
        if (genType == shape_type_null || genType == shape_type_general_point)
            return nullptr;


        return  reinterpret_cast<double*>(m_blob.Buffer() + 1 + 2);

    }
    const double *CGeoShape::GetBBoxVals(eShapeType genType) const
    {
        if (genType == shape_type_null || genType == shape_type_general_point)
            return nullptr;

        return  reinterpret_cast<const double*>(m_blob.Buffer() + 1 + 2);
    }


    uint32_t CGeoShape::NextPart(uint32_t nIdx) const
    {
         return GetPart(nIdx);
    }

    GisXYPoint CGeoShape::NextPoint(uint32_t nIdx) const
    {
         return GetPoints()[nIdx];
    }

    bool CGeoShape::NextPoint(uint32_t nIdx, GisXYPoint& pt) const
    {
        pt = GetPoints()[nIdx];
        return true;
    }

    IGeoShapePtr CGeoShape::Clone()
    {
        IGeoShapePtr ptrShape = std::make_shared<CGeoShape>();
        ptrShape->Import(Data(), Size());

        return  ptrShape;
    }

}