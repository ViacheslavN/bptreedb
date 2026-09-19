#pragma once

#include "BoundaryBox.h"

namespace CommonLib
{
    enum eShapeType
    {
        shape_type_null               = 0,
        shape_type_point              = 1,
        shape_type_point_m            = 21,
        shape_type_point_zm           = 11,
        shape_type_point_z            = 9,
        shape_type_multipoint         = 8,
        shape_type_multipoint_m       = 28,
        shape_type_multipoint_zm      = 18,
        shape_type_multipoint_z       = 20,
        shape_type_polyline           = 3,
        shape_type_polyline_m         = 23,
        shape_type_polyline_zm        = 13,
        shape_type_polyline_z         = 10,
        shape_type_polygon            = 5,
        shape_type_polygon_m          = 25,
        shape_type_polygon_zm         = 15,
        shape_type_polygon_z          = 19,
        shape_type_multipatch_m       = 31,
        shape_type_multipatch         = 32,
        shape_type_general_polyline   = 50,
        shape_type_general_polygon    = 51,
        shape_type_general_point      = 52,
        shape_type_general_multipoint = 53,
        shape_type_general_multipatch = 54,
        shape_type_last               = shape_type_general_multipatch + 1
    };


    enum eShapeMasks
    {
        shape_has_zs                    = 0x80000000,
        shape_has_ms                    = 0x40000000,
        shape_has_curves                = 0x20000000,
        shape_has_ids                   = 0x10000000,
        shape_has_normals               = 0x08000000,
        shape_has_textures              = 0x04000000,
        shape_has_partid                = 0x02000000,
        shape_has_materials             = 0x01000000,
        shape_is_compressed             = 0x00800000,
        shape_modifier_mask             = 0xFF000000,
        shape_multi_patch_modifier_mask = 0x00F00000,
        shape_basic_type_mask           = 0x000000FF,
        shape_basic_modifier_mask       = shape_has_zs | shape_has_ms,
        shape_non_basic_modifier_mask   = shape_modifier_mask & ~shape_basic_modifier_mask,
        shape_extended_modifier_mask    = shape_basic_modifier_mask | shape_has_ids | shape_has_normals | shape_has_textures | shape_has_materials
    };
    enum patch_type
    {
        patch_type_triangle_strip = 0,
        patch_type_triangle_fan   = 1,
        patch_type_outer_ring     = 2,
        patch_type_inner_ring     = 3,
        patch_type_first_ring     = 4,
        patch_type_ring           = 5,
        patch_type_triangles      = 6
    };


    enum segment_type
    {
        segment_type_circular_arc  = 1,
        segment_type_line          = 2,
        segment_type_spiral        = 3,
        segment_type_bezier3_curve = 4,
        segment_type_elliptic_arc  = 5
    };

    enum circular_arc_flags
    {
        circular_arc_flag_is_empty = 1,
        circular_arc_flag_is_ccw   = 8,
        circular_arc_flag_is_minor = 16,
        circular_arc_flag_is_line  = 32,
        circular_arc_flag_is_point = 64,
        circular_arc_flag_is_defined = 128
    };

    enum elliptic_arc_flags
    {
        elliptic_arc_flag_is_empty    = 1,
        elliptic_arc_flag_is_line     = 64,
        elliptic_arc_flag_is_point    = 128,
        elliptic_arc_flag_is_circular = 256,
        elliptic_arc_flag_center_to   = 512,
        elliptic_arc_flag_center_from = 1024,
        elliptic_arc_flag_is_ccw      = 2048,
        elliptic_arc_flag_is_minor    = 4096,
        elliptic_arc_flag_is_complete = 8192
    };
    struct GisXYPoint
    {
        double x;
        double y;
    };


    struct segment_circular_arc_t
    {
        union
        {
            GisXYPoint   centerPoint;
            double angles[2];
        };

        circular_arc_flags flags;
    };

    struct segment_bezier_curve_t
    {
        GisXYPoint controlPoints[2];
    };

    struct segment_elliptic_arc_t
    {
        union
        {
            GisXYPoint center;
            double vs[2];
        };

        union
        {
            double rotation;
            double fromV;
        };

        double semiMajor;

        union
        {
            double minorMajorRatio;
        };

        elliptic_arc_flags flags;
    };

    struct segment_modifier_t
    {
        long         fromPoint;
        segment_type segmentType;
        union
        {
            segment_circular_arc_t circularArc;
            segment_bezier_curve_t bezierCurve;
            segment_elliptic_arc_t ellipticArc;
        } segmentParams;
    };

    typedef std::shared_ptr<class IGeoShape> IGeoShapePtr;


    class IGeoShape
    {
    public:
        IGeoShape(){};
        virtual ~IGeoShape(){}
        virtual eShapeType Type() const = 0;


        virtual uint32_t  GetPartCount() const = 0;
        virtual uint32_t  GetPart(uint32_t idx) const = 0;
        virtual const uint32_t*  GetParts() const = 0;
        virtual uint32_t*  GetParts()= 0;

        virtual patch_type*       GetPartsTypes() = 0;
        virtual const patch_type* GetPartsTypes() const = 0;
        //virtual const patch_type partType(size_t idx) const = 0;

        virtual uint32_t NextPart(uint32_t nIdx) const = 0;
        virtual GisXYPoint NextPoint(uint32_t nIdx) const = 0;
        virtual bool NextPoint(uint32_t nIdx, GisXYPoint& pt) const = 0;


        virtual GisXYPoint* GetPoints() = 0;
        virtual const GisXYPoint* GetPoints() const = 0;
       // virtual void SetPoints(const double *pPoint) = 0; //xy
        //virtual double&       ptX(size_t idx) = 0;
        //virtual const double& ptX(size_t idx) const = 0;
        //virtual double&      ptY(size_t idx) = 0;
        //virtual const double& ptY(size_t idx) const = 0;


        virtual double* GetZs() = 0;
        //virtual const double* GetZs() const = 0;
        //virtual double&       ptZ(size_t idx)= 0;
       // virtual const double& ptZ(size_t idx) const= 0;
        //virtual void SetZs(const double *pPoint) = 0; //z

        virtual double* GetMs() = 0;
        //virtual const double* GetMs() const = 0;
        //virtual double&       ptM(size_t idx)= 0;
        //virtual const double& ptM(size_t idx) const = 0;
        virtual uint32_t GetPointCnt() const= 0;

        virtual void CalcBB() = 0;
        virtual bbox GetBB() const = 0;

        virtual eShapeType GeneralType() const = 0;

        virtual void Create(eShapeType shapeType) = 0;
        virtual void Create(eShapeType shapeType, uint32_t npoints, uint32_t nparts = 1, uint32_t ncurves = 0, uint32_t mpatchSpecificSize = 0) = 0;

        virtual void  Add(IGeoShapePtr ptrShap)  = 0;
        virtual void Import(const byte* extBuf, uint32_t extBufSize) = 0;
        virtual uint32_t  Size() const = 0;
        virtual  const byte_t * Data() const  = 0;
        virtual  IGeoShapePtr Clone() = 0;


        static bool IsTypeSimple(eShapeType shapeType);
        static void GetTypeParams(eShapeType shapeType, eShapeType* genType = 0, bool* has_z = 0, bool* has_m = 0, bool* has_curve = 0, bool* has_id = 0);
        static eShapeType GeneralType(eShapeType type);


    };
}