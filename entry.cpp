#include <pspsdk.h>
#include <pspuser.h>
#include <pspctrl.h>
#include <pspkernel.h>
#include <systemctrl.h>
#include <kubridge.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "log.hpp"
#include "types.hpp"

#define MODULE_NAME "pspfix"

PSP_MODULE_INFO("pspfix", 0, 1, 0);

extern InternalModInfo mainModInfo;
InternalModInfo ourModInfo;

// defined in main.cpp
void ModuleMain();

static void moduleScanSce() {
	SceUID modules[10]{};
	int count = 0;
	bool bFoundMainModule = false;
	bool bFoundInternalModule = false;

	SceKernelModuleInfo info {
		.size = sizeof(SceKernelModuleInfo)
	};

	if (sceKernelGetModuleIdList(modules, sizeof(modules), &count) >= 0) {
		for (auto i = 0; i < count; ++i) {
			if (sceKernelQueryModuleInfo(modules[i], &info) < 0)
				continue;

			if (!strcmp(info.name, "ssxpsp")) {
				bFoundMainModule = true;

				logc::print(logc::Info, "Found main module!");
				logc::print(logc::Info, "text_addr: 0x%X text_size: 0x%X", info.text_addr, info.text_size);

				mainModInfo.textBase = reinterpret_cast<u8*>(info.text_addr);
				mainModInfo.textSize = info.text_size;

			} else if (!strcmp(info.name, MODULE_NAME) == 0) {
				bFoundInternalModule = true;
				ourModInfo.textBase = reinterpret_cast<u8*>(info.text_addr);
				ourModInfo.textSize = info.text_size;
			}
		}
	}

	if (bFoundInternalModule && bFoundMainModule) {
		ModuleMain();
	}
}

static void moduleScanCfw() {
	SceModule mod = { 0 };
	SceModule this_module = { 0 };

	int kuErrCode = kuKernelFindModuleByName("ssxpsp", &mod);
	if (kuErrCode != 0)
		return;

	kuErrCode = kuKernelFindModuleByName(MODULE_NAME, &this_module);
	if (kuErrCode != 0)
		return;

	ModuleMain();
}

extern "C" int module_start(SceSize argc, void* argp) {
	logc::init();

	// If a kemulator: device exists, then we can be reasonably sure that we're in an emulator.
	// In that case, we need to scan the modules using normal/official syscalls.
	// See (https://github.com/hrydgard/ppsspp/pull/13335#issuecomment-689026242).
	if (sceIoDevctl("kemulator:", 0x00000003, NULL, 0, NULL, 0) == 0)
		moduleScanSce();
	else
		moduleScanCfw();

	return 0;
}
