#include <pspkernel.h>

__attribute__((weak)) extern "C" void abort() {
	sceKernelExitGame();

	// The above call will make the kernel always
	// exit, therefore, everything after is unreachable.
	__builtin_unreachable();
}
