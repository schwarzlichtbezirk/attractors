
#include "stdafx.h"

#include <cstdarg>

#include "stringutil.h"

template void correctfilename<char>(std::string&);
template void correctfilename<wchar_t>(std::wstring&);

std::string __cdecl format(const char* fmt, ...) {
	va_list argList;
	va_start(argList, fmt);

	std::string res(_vscprintf(fmt, argList), 0);
	vsprintf_s((char*)res.data(), res.size() + 1, fmt, argList);

	return res;
};

std::wstring __cdecl wformat(const wchar_t* fmt, ...) {
	va_list argList;
	va_start(argList, fmt);

	std::wstring res(_vscwprintf(fmt, argList), 0);
	vswprintf_s((wchar_t*)res.data(), res.size() + 1, fmt, argList);

	return res;
};

std::string wchar_to_utf8(const wchar_t* in, size_t len) {
	std::string out;
	unsigned int codepoint = 0;
	for (in; len--; ++in) {
		if (*in >= 0xd800 && *in <= 0xdbff)
			codepoint = ((*in - 0xd800) << 10) + 0x10000;
		else {
			if (*in >= 0xdc00 && *in <= 0xdfff)
				codepoint |= *in - 0xdc00;
			else
				codepoint = *in;

			if (codepoint <= 0x7f)
				out.push_back(static_cast<char>(codepoint));
			else if (codepoint <= 0x7ff) {
				out.push_back(static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
				out.push_back(static_cast<char>(0x80 | (codepoint & 0x3f)));
			}
			else if (codepoint <= 0xffff) {
				out.push_back(static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
				out.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
				out.push_back(static_cast<char>(0x80 | (codepoint & 0x3f)));
			}
			else {
				out.push_back(static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
				out.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
				out.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
				out.push_back(static_cast<char>(0x80 | (codepoint & 0x3f)));
			}
			codepoint = 0;
		}
	}
	return out;
}

std::wstring utf8_to_wchar(const char* in, size_t len) {
	std::wstring out;
	unsigned int codepoint;
	while (len--) {
		unsigned char ch = static_cast<unsigned char>(*in);
		if (ch <= 0x7f)
			codepoint = ch;
		else if (ch <= 0xbf)
			codepoint = (codepoint << 6) | (ch & 0x3f);
		else if (ch <= 0xdf)
			codepoint = ch & 0x1f;
		else if (ch <= 0xef)
			codepoint = ch & 0x0f;
		else
			codepoint = ch & 0x07;
		++in;
		if (((*in & 0xc0) != 0x80) && (codepoint <= 0x10ffff)) {
			if (sizeof(wchar_t) > 2)
				out.push_back(static_cast<wchar_t>(codepoint));
			else if (codepoint > 0xffff) {
				out.push_back(static_cast<wchar_t>(0xd800 + (codepoint >> 10)));
				out.push_back(static_cast<wchar_t>(0xdc00 + (codepoint & 0x03ff)));
			}
			else if (codepoint < 0xd800 || codepoint >= 0xe000)
				out.push_back(static_cast<wchar_t>(codepoint));
		}
	}
	return out;
}

// The End.
