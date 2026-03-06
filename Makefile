TARGET = ../otpspfix
OBJS = \
	abort.o \
	entry.o \
	heap.o \
	memory.o \
	log.o \
	main.o \
	exports.o

CFLAGS = -O3 -G0 -Wall -fshort-wchar -fno-pic -mno-check-zero-division -fpack-struct=16
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = exports.exp

LIBS = -lpspsystemctrl_user -lpspkubridge -lm

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak
