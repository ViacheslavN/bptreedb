#pragma once
#pragma once

namespace CommonLib
{
	namespace multibyte
	{
		long dest_mbsnrtowcs(wchar_t *dst, unsigned char **src, size_t nmc, size_t len);
		long dest_wcsnrtombs(unsigned char *dst, wchar_t **src, size_t nwc, size_t len);

		long dest_mbslen(const unsigned char *src, size_t nmc);
		long dest_mbslen(const unsigned char *src);
		long dest_wcsmblen(const wchar_t *src, size_t nwc);
		long dest_wcsmblen(const wchar_t *src);
	}
}