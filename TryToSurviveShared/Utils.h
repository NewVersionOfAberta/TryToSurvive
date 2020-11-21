#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <windows.h>

#define SIZE 256

namespace Utils {
	inline std::string GetWorkingDirectory() {
		

		HMODULE hModule = GetModuleHandle(nullptr);
		if (hModule) {
			char path[SIZE];
			char newPath[SIZE] = "\0";

			char drive[_MAX_DRIVE];
			char dir[_MAX_DIR];
			char fname[_MAX_FNAME];
			char ext[_MAX_EXT];

			GetModuleFileNameA(hModule, path, SIZE);
			_splitpath_s(path, drive, dir, fname, ext);
			strcat_s(newPath, SIZE,  drive);
			strcat_s(newPath, SIZE, dir);
			//strcat_s(newPath, SIZE, "\\");
			return std::string(newPath);
		}
		return "";
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