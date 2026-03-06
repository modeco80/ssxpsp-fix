
#include <pspsdk.h>
#include <pspuser.h>
#include <pspctrl.h>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "log.hpp"

namespace logc {

		static SceUID logFd = -1;

		void init() {
			logFd = sceIoOpen("ms0:/ssxfix.log", PSP_O_WRONLY | PSP_O_CREAT | PSP_O_APPEND, 0777);
			if(logFd == -1) {
				printf("WARNING: Logs will only be available via sce printf\n");
			}
		}

		void putsImpl(const char* buffer, unsigned int length) {
			sceIoWrite(logFd, buffer, length);
			// Write to stdout as well.
			sceIoWrite(1, buffer, length);
		}

		void putsImpl(const char* buffer) {
			putsImpl(buffer, strlen(buffer));
		}

		void printUnformatted(Level level, const char* pszBuffer) {
			const char* pszPrefix;
			switch(level) {
				case Info: pszPrefix = "[i] "; break;
				case Warn: pszPrefix = "[W] "; break;
				case Error: pszPrefix = "[E] "; break;
				default: pszPrefix = "[? Fix code!!! ?] ";
			}

			putsImpl(pszPrefix);
			putsImpl(pszBuffer);
			putsImpl("\n");
		}

		void print(Level level, const char* text, ...) {
			va_list list{};
			char string[512]{};

			va_start(list, text);
			vsprintf(string, text, list);
			va_end(list);

			printUnformatted(level, string);
		}
}
