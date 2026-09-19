#pragma once

#include "../CommonLib.h"

namespace CommonLib
{
	namespace win
	{
		const GUID guid_null = { 0x00000000L, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
		const GUID guid_one = { 0x00000000L, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };

		class CGuid
		{
		public:
			CGuid();
			CGuid(const CGuid &gid);
			CGuid(const GUID gid);
			CGuid(const std::string &gid);
			CGuid(const std::wstring &gid);

			CGuid& operator =(const CGuid& gid);
			~CGuid();

			bool operator ==(const CGuid &gid) const;
			bool operator !=(const CGuid &gid) const;
			bool operator <(const CGuid &gid) const;
			bool operator >(const CGuid &gid) const;
			bool operator <=(const CGuid &gid) const;
			bool operator >=(const CGuid &gid) const;

            std::string ToAstr(bool withbrackets) const;
            std::wstring ToWstr(bool withbrackets) const;

			void FromAstr(const std::string& gid);
			void FromWstr(const std::wstring& gid);

			static CGuid CreateNew();
			static CGuid CreateNull();
            static CGuid CreateFromAstr(const std::string& gid);
		private:
			GUID m_guid;
		};
	}

	typedef win::CGuid CGuid;
}