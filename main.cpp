#include <kubridge.h>
#include <pspctrl.h>
#include <pspkernel.h>
#include <pspsdk.h>
#include <pspuser.h>
#include <systemctrl.h>

#include "log.hpp"
#include "memory.hpp"
#include "mips.hpp"
#include "types.hpp"

InternalModInfo mainModInfo {};

u32* pHookMemory;

void doLoadRiderPatch() {
	// Offset to a problematic instruction in cRider::loadRider(), which breaks loading rider data.
	constexpr static auto kOffset_cRider_loadRider_problematic = 0x6f88;
	constexpr static auto kOffset_cRider_loadRider_problematic2 = 0x6f80;

	// Partially rewrite a piece of cRider::loadRider() so that its switch/case state machine
	// actually functions. (thank god for the compiler keeping it in)
	*(reinterpret_cast<volatile u32*>(mainModInfo.textBase + kOffset_cRider_loadRider_problematic)) = mipse::lw(mipse::a0, mipse::a1, 0x1420);
	*(reinterpret_cast<volatile u32*>(mainModInfo.textBase + kOffset_cRider_loadRider_problematic2)) = mipse::nop();
	logc::print(logc::Info, "did cRider::loadRider() patch");
}

void doAsyncfilePatch() {
	constexpr static auto kOffset_initFileSys_li = 0x93c5c;
	*(reinterpret_cast<volatile u32*>(mainModInfo.textBase + kOffset_initFileSys_li)) = mipse::li(mipse::a0, 0x80);
	logc::print(logc::Info, "Did ASYNCFILE_init patch");
}

void doInitPlayersHook() {
	constexpr static auto kHookMemorySize = 64;

	// In module offsets:

	// Offsets in cAI::InitPlayers which we first start the hook to, and then
	// return into to continue normal game execution.
	constexpr static auto kOffset_cAI_InitPlayers_TrampolineHook = 0x55e84;
	constexpr static auto kOffset_cAI_InitPlayers_TrampolineRetk = 0x55e8c;

	// offset to cRider::initOnce() function
	constexpr static auto kOffset_cRider_initOnce_Func = 0x6b2c;

	// Allocate hook memory.
	pHookMemory = reinterpret_cast<u32*>(mem::allocate(kHookMemorySize));
	if(pHookMemory == nullptr) {
		logc::print(logc::Error, "Hook memory was not allocated successfully :(");
		return;
	}

	volatile auto* pHookMemoryCursor = pHookMemory;
	volatile auto* pModuleHookTarget = reinterpret_cast<u32*>(mainModInfo.textBase + kOffset_cAI_InitPlayers_TrampolineHook);

	// Perform the initial trampoline hook which gets us to our code.
	*pModuleHookTarget++ = mipse::j(unsafeTransmute<i32>(pHookMemory));
	*pModuleHookTarget++ = mipse::nop();

	// Now, in the hook memory, assemble the calls we need.

	// Call cRider::initOnce() with the rider pointer.
	*pHookMemoryCursor++ = mipse::jal(unsafeTransmute<i32>(mainModInfo.textBase + kOffset_cRider_initOnce_Func)); // jal cRider__initOne
	*pHookMemoryCursor++ = mipse::addiu(mipse::a0, mipse::s1, 0x0);												  // move a0,s1

	// Once that's done, do the things we stepped over in preparation for stepping back into the game code.
	*pHookMemoryCursor++ = mipse::lw(mipse::a0, mipse::sp, 0x58); // lw a0, 0x58(sp)
	*pHookMemoryCursor++ = mipse::lw(mipse::s1, mipse::sp, 0x4c); // lw s1, 0x4c(sp)

	// Jump back into the game code, like nothing ever happened.
	*pHookMemoryCursor++ = mipse::j(unsafeTransmute<i32>(mainModInfo.textBase + kOffset_cAI_InitPlayers_TrampolineRetk));
	*pHookMemoryCursor++ = mipse::nop();

#ifdef DEBUG
	// Validate to make sure we don't buffer overflow hook memory.
	auto usedHookMemory = (reinterpret_cast<volatile u8*>(pHookMemoryCursor) - reinterpret_cast<u8*>(pHookMemory));
	if(usedHookMemory > kHookMemorySize) {
		logc::print(logc::Error, "Hook memory buffer overflow of %d bytes!", (usedHookMemory - kHookMemorySize));
		return;
	}

	logc::print(logc::Info, "Hooked! Used %d bytes of hook memory.", usedHookMemory);
#endif
}

void ModuleMain() {
	logc::printUnformatted(logc::Info, "SSX PSP Viewer Rider Fix loaded successfully");

	doLoadRiderPatch();
	doAsyncfilePatch();
	doInitPlayersHook();

	// not really necessary
	sceKernelDcacheWritebackAll();
}
