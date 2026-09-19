#pragma once

#include "CommonLib.h"
#include "../data/ByteArray.h"
namespace CommonLib
{

		namespace xml
		{
			class CXMLUtils
			{
			public:
				static void Blob2String(const byte_t *data, size_t size, std::string& str);
				static void String2Blob(const std::string& str, Data::TVecBuffer& data);
			};
		}

}