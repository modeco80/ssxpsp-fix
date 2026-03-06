#pragma once

namespace logc {
	enum Level {
		Info,
		Warn,
		Error
	};

	void init();

	void printUnformatted(Level level, const char* pszBuffer);

	void print(Level level, const char* pszFormat, ...);

}
