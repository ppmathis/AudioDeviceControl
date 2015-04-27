#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <locale>
#include <codecvt>

class Formatter {
public:
	Formatter() {}
	~Formatter() {}

	template <typename Type>
	Formatter & operator << (const Type & value) {
		mStream << value;
		return *this;
	}

	operator std::string() const {
		std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
		return converter.to_bytes(mStream.str());
	}

	operator std::wstring() const {
		return mStream.str();
	}

private:
	std::wstringstream mStream;

	Formatter(const Formatter &);
	Formatter & operator = (Formatter &);
};