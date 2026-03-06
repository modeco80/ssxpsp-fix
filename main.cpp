#include "log.hpp"


void ModuleMain() {
	logc::printUnformatted(logc::Info, "Hello World!");
    // not really necessary
    //sceKernelDcacheWritebackAll();
}
