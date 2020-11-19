#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <windows.h>
#include <Shlwapi.h>

namespace Utils {
	inline std::wstring GetWorkingDirectory() {
		HMODULE hModule = GetModuleHandle(nullptr);
		if (hModule) {
			wchar_t path[256];
			GetModuleFileName(hModule, path, sizeof(path)*2);
			PathRemoveFileSpec(path);
			wcscat_s(path, L"\\");
			return std::wstring(path);
		}
		return L"";
	}

	inline void ReadQuotedString(std::stringstream& l_stream,
		std::string& l_string)
	{
		l_stream >> l_string;
		if (l_string.at(0) == '"') {
			while (l_string.at(l_string.length() - 1) != '"' || !l_stream.eof()) {
				std::string str;
				l_stream >> str;
				l_string.append(" " + str);
			}
		}
		l_string.erase(std::remove(l_string.begin(), l_string.end(), '"'), l_string.end());
	}
}