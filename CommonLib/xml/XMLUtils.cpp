#include "stdafx.h"
#include "XMLUtils.h"

namespace CommonLib
	{
		namespace xml
		{
			void CXMLUtils::Blob2String(const byte_t *pData, size_t size, std::string& result)
			{
	 
				if (!pData || size == 0)
					return;

				result.resize(size * 2 + 1, 0);
				for (size_t i = 0; i < size; ++i)
				{
					unsigned char sym = pData[i];
					unsigned char left = sym >> 4;
					unsigned char right = sym & 0xF;
					result[(int)i * 2 + 0] = left <= 9 ? (left + '0') : ((left - 10) + 'A');
					result[(int)i * 2 + 1] = right <= 9 ? (right + '0') : ((right - 10) + 'A');
				}
			}

			void CXMLUtils::String2Blob(const std::string& str, CommonLib::Data::TVecBuffer& blob)
			{
				int len = (int)str.length() / 2;
				if (len == 0)
					return;
				blob.resize(len);

				for (int i = 0; i < len; ++i)
				{
					wchar_t left = str[i * 2 + 0];
					wchar_t right = str[i * 2 + 1];
					int leftVal;
					int rightVal;

					if (left >= '0' && left <= '9')
						leftVal = left - '0';
					else if (left >= 'A' && left <= 'F')
						leftVal = left - 'A' + 10;
					else
						throw CommonLib::CExcBase("CXMLUtils::String2Blob invalid symbol");
			

					if (right >= '0' && right <= '9')
						rightVal = right - '0';
					else if (right >= 'A' && right <= 'F')
						rightVal = right - 'A' + 10;
					else
						throw CommonLib::CExcBase("CXMLUtils::String2Blob invalid symbol");
	
					blob[i] = (unsigned char)((leftVal & 0xF) << 4 | (rightVal & 0xF));
				}
			}
		}

}