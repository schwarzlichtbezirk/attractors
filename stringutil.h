#pragma once
#ifndef __stringutil_h__
#define __stringutil_h__

#include <string>

//-----------------------------------------------------------------------------

std::string __cdecl format(const char* fmt, ...);
std::wstring __cdecl wformat(const wchar_t* fmt, ...);

// bring filename to unix-compatible format
template<typename T>
void correctfilename(std::basic_string<T>& str) {
	T c;
	for (T* dest = (T*)str.data(); *dest; dest++) {
		c = *dest;
		if (c >= 'A' && c <= 'Z') *dest += 32;
		else if (c == '\\') *dest = '/';
		else if (c == '*' || c == '?' || c == '|' || c == '<' || c == '>') *dest = '_';
	}
}

std::string wchar_to_utf8(const wchar_t* in, size_t len);
std::wstring utf8_to_wchar(const char* in, size_t len);
inline std::string wchar_to_utf8(const std::wstring& in) { return wchar_to_utf8(in.data(), in.size()); }
inline std::wstring utf8_to_wchar(const std::string in) { return utf8_to_wchar(in.data(), in.size()); }

//-----------------------------------------------------------------------------

#endif // __stringutil_h__
