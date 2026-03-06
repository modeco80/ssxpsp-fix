#include "log.hpp"
#include "types.hpp"

InternalModInfo mainModInfo;

void ModuleMain() {
	logc::printUnformatted(logc::Info, "Hello World!");
    // not really necessary
    //sceKernelDcacheWritebackAll();
}
