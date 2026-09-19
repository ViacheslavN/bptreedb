#pragma once

#include "../alloc/alloc.h"
#include "../alloc/simpleAlloc.h"
#include "../stream/stream.h"
#include "IGeoShape.h"
#include "../data/blob.h"

namespace CommonLib
{

    class CGeoShape : public IGeoShape
    {
    public:
        enum ePramsFlags
        {
            eExternParams = 0,
            eInnerParams = 1
        };

        CGeoShape(IAllocPtr pAlloc = nullptr);
        CGeoShape(const CGeoShape& geoShp);
        virtual ~CGeoShape();
        void Clear();

        CGeoShape&   operator=(const CGeoShape& shp);


        CGeoShape& operator += (const CGeoShape& shp)
        {
            return *this;
        }

        bool operator <(const CGeoShape& shp) const
        {
            return false;
        }
        bool operator >(const CGeoShape& shp) const
        {
            return false;
        }
        bool operator <=(const CGeoShape& shp) const
        {
            return false;
        }
        bool operator >=(const CGeoShape& shp) const
        {
            return false;
        }

        bool operator ==(const CGeoShape& shp) const
        {
            return false;
        }
        bool operator !=(const CGeoShape& shp) const
        {
            if (Type() != shp.Type())
                return true;

            if (GetPartCount() != shp.GetPartCount())
                return true;

            if (GetPointCnt() != shp.GetPointCnt())
                return true;

            return m_blob != shp.m_blob;

        }

        void Write(IWriteStream *pStream) const;
        void Read(IReadStream *pStream);

        virtual uint32_t NextPart(uint32_t nIdx) const;

        virtual GisXYPoint NextPoint(uint32_t nIdx) const;
        virtual bool NextPoint(uint32_t nIdx, GisXYPoint& pt) const;
        virtual eShapeType Type() const;


        virtual uint32_t  GetPartCount() const;
        virtual uint32_t  GetPart(uint32_t idx) const;


        virtual const uint32_t*  GetParts() const;
        virtual uint32_t*  GetParts();

        virtual GisXYPoint* GetPoints();
        virtual const GisXYPoint* GetPoints() const;
        virtual uint32_t GetPointCnt() const;


        virtual double* GetZs();
        virtual const double* GetZs() const;

        virtual double* GetMs();
        virtual const double* GetMs() const;



        virtual patch_type*       GetPartsTypes();
        virtual const patch_type* GetPartsTypes() const;
        virtual const patch_type  PartType(uint32_t idx) const;


        void Create(uint32_t nSize);
        virtual void Create(eShapeType shapeType);
        virtual void Create(eShapeType shapeType, uint32_t npoints, uint32_t nparts = 1, uint32_t ncurves = 0, uint32_t mpatchSpecificSize = 0);
        void Create(byte_t* pBuf, uint32_t nSize, eShapeType shapeType, uint32_t npoints, uint32_t nparts = 1, uint32_t ncurves = 0);
        virtual  IGeoShapePtr Clone();


        virtual void Import(const byte_t* extBuf, uint32_t extBufSize);
        void Attach(byte_t* extBuf, uint32_t extBufSize);
        byte_t* Detach();
        bool Validate();
        void SetNull(eShapeType shapeType = shape_type_null);


        //static void getTypeParams(eShapeType shapeType, eShapeType* pGenType, bool* has_z = nullptr, bool* has_m = nullptr, bool* has_curve = nullptr, bool* has_id = nullptr);
        static eShapeType GetGeneralType(eShapeType type);

        virtual uint32_t  Size() const;
        virtual  const byte_t * Data() const;
        virtual eShapeType GeneralType() const;
        bool       HasZs() const;
        bool       HasMs() const;
        bool       HasCurves() const;
        bool       HasIDs() const;

        uint32_t PointCount() const;

        static void InitShapeBufferBuffer(unsigned char* buf, eShapeType shapeType, uint32_t npoints, uint32_t nparts, uint32_t ncurves);
        static uint32_t CalcSize(eShapeType shapeType, uint32_t npoints = 1, uint32_t nparts = 1, uint32_t ncurves = 0, uint32_t mpatchSpecificSize = 0);

        static uint32_t   PointCount(const byte* buf);
        static uint32_t   PointCount(const byte* buf, eShapeType general_type);
        static uint32_t   PartCount(const byte* buf);
        static uint32_t    PartCount(const byte* buf, eShapeType general_type);
        static eShapeType  Type(const byte* buf);
        static eShapeType  GeneralType(const unsigned char* buf);



        static const GisXYPoint* GetXYs(const byte* buf);
        static const GisXYPoint* GetXYs(const byte* buf, eShapeType  general_type, uint32_t partCount);

        static GisXYPoint* GetXYs(byte* buf);
        static GisXYPoint* GetXYs(byte* buf, eShapeType  general_type, uint32_t partCount);


        virtual void CalcBB();

        double *GetBBoxVals();
        const double *GetBBoxVals() const;
        virtual bbox GetBB() const;

        byte_t* Buffer() { return m_blob.Buffer(); }
        const byte_t* Buffer() const { return m_blob.Buffer(); }


    private:

        const uint32_t*  GetParts(const byte *pBuf) const;
        uint32_t*  GetParts(byte *pBuf);


        double *GetBBoxVals(eShapeType shapeType);
        const double *GetBBoxVals(eShapeType shapeType) const;

        virtual void  Add(IGeoShapePtr ptrShap);

    private:
        static const uint32_t __minimum_point_ = 10;

        mutable Data::CBlob m_blob;


        struct sShapeParams
        {

            sShapeParams();

            void Set(const byte* pBuffer);
            void Reset();


            eShapeType m_type;
            eShapeType m_general_type;
            uint32_t m_nPointCount;
            uint32_t m_nPartCount;
            const GisXYPoint *m_pPoints;
            bool m_bIsValid;
        };

        sShapeParams m_params;

    };
}