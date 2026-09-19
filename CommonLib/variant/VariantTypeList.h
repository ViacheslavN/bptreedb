START_TYPE_LIST()
DECLARE_TYPE(CEmptyVariant)
DECLARE_SIMPLE_TYPE(bool)
DECLARE_SIMPLE_TYPE(int8_t)
DECLARE_SIMPLE_TYPE(uint8_t)
DECLARE_SIMPLE_TYPE(int16_t)
DECLARE_SIMPLE_TYPE(uint16_t)
DECLARE_SIMPLE_TYPE(int32_t)
DECLARE_SIMPLE_TYPE(uint32_t)
DECLARE_SIMPLE_TYPE(int64_t)
DECLARE_SIMPLE_TYPE(uint64_t)
DECLARE_SIMPLE_TYPE(float)
DECLARE_SIMPLE_TYPE(double)
DECLARE_TYPE(CGuid)
DECLARE_TYPE(astr_t)
DECLARE_TYPE(wstr_t)
DECLARE_TYPE(IGeoShapePtr)
DECLARE_TYPE(CBlobPtr_t)

FINISH_TYPE_LIST()

#ifndef TYPE_COUNT
#define TYPE_COUNT _type_list_count_
#define MAX_GV_SIZE maxSize<TYPE_COUNT-2>::mxSize
#endif


#ifdef DECLARE_FUNC_TABLES
#ifndef CREATE_FUNC_TABLES

extern constructorVariant		var_constructors_[TYPE_COUNT];
extern destructorVariant		var_destructors_[TYPE_COUNT];
extern compareVariantFunc		var_compare_[TYPE_COUNT];
extern copyVariantFunc			var_copy_[TYPE_COUNT];
extern assignVariantFunc		var_assign_[TYPE_COUNT];
extern acceptVariantFunc		var_accept_[TYPE_COUNT];

#else

constructorVariant		var_constructors_[TYPE_COUNT];
destructorVariant		var_destructors_[TYPE_COUNT];
compareVariantFunc		var_compare_[TYPE_COUNT];
copyVariantFunc			var_copy_[TYPE_COUNT];
assignVariantFunc		var_assign_[TYPE_COUNT];
acceptVariantFunc		var_accept_[TYPE_COUNT];

#endif
#endif

