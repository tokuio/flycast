.DEFAULT_GOAL := all
LOCAL_PATH := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
FOR_LINUX :=1
#NO_REC := 1
#NO_REND := 1
USE_OSS := 1
#USE_PULSEAUDIO := 1
#USE_SDLAUDIO := 1
#USE_LIBAO := 1
ifndef USE_SDL
USE_EVDEV := 1
USE_UDEV := 1
endif
USE_MODEM := 1
PLATFORM_EXT := elf

CXX ?= ${CC_PREFIX}g++
CC ?= ${CC_PREFIX}gcc
AS ?= ${CC_PREFIX}as
STRIP ?= ${CC_PREFIX}strip
LD ?= ${CC}
WINDRES=${CC_PREFIX}windres
CHD5_LZMA := 1
CHD5_FLAC := 1

MFLAGS :=
ASFLAGS :=
LDFLAGS ?=
INCS ?=
LIBS ?=
CFLAGS :=
CXXFLAGS :=

ifneq (, $(filter $(shell uname -s), FreeBSD OpenBSD NetBSD DragonFly))
    CFLAGS += -DTARGET_BSD
    undefine USE_UDEV
else
    USE_ALSA := 1
#    USE_JOYSTICK := 1
endif

# Platform auto-detection
# Can be overridden by using:
#   make platform=x64
ifeq (,$(platform))
    UNAME = $(shell uname -a)
	ifeq ($(UNAME),)
		platform = win32
	else ifneq ($(findstring MINGW,$(UNAME)),)
		platform = win32
	else ifneq ($(findstring win,$(UNAME)),)
		platform = win32
	endif
endif
ifeq (,$(platform))
    ARCH = $(shell uname -m)
    ifeq ($(ARCH), $(filter $(ARCH), i386 i686))
        platform = x86
    else ifeq ($(ARCH), $(filter $(ARCH), x86_64 AMD64 amd64))
        platform = x64
    else ifneq (,$(findstring aarch64,$(ARCH)))
        platform = arm64
        HARDWARE = $(shell grep Hardware /proc/cpuinfo)
        ifneq (,$(findstring Vero4K,$(HARDWARE)))
            platform = vero4k
            FEATURES = $(shell grep Features /proc/cpuinfo)
            ifneq (,$(findstring neon,$(FEATURES)))
                platform += neon
            endif
        endif
    else ifneq (,$(findstring arm,$(ARCH)))
        HARDWARE = $(shell grep Hardware /proc/cpuinfo)
        ifneq (,$(findstring BCM2709,$(HARDWARE)))
            platform = rpi2
        else ifneq (,$(findstring AM33XX,$(HARDWARE)))
            platform = beagle
        else ifneq (,$(findstring Pandora,$(HARDWARE)))
            platform = pandora
        else ifneq (,$(findstring ODROIDC,$(HARDWARE)))
            platform = odroidc1
        else ifneq (,$(findstring ODROID-XU3,$(HARDWARE)))
            platform = odroidxu3
	else ifneq (,$(findstring ODROID-XU4,$(HARDWARE)))
            platform = odroidxu3
        else ifneq (,$(findstring ODROIDXU,$(HARDWARE)))
            platform = odroidxu
        else ifneq (,$(findstring ODROIDX2,$(HARDWARE)))
            platform = odroidx2
        else ifneq (,$(findstring ODROIDX,$(HARDWARE)))
            platform = odroidx
        else ifneq (,$(findstring ODROID-U2/U3,$(HARDWARE)))
            platform = odroidu2
        else ifneq (,$(findstring ODROIDU2,$(HARDWARE)))
            platform = odroidu2
        else
            platform = armv7h
        endif
    else ifneq (,$(findstring mips,$(ARCH)))
        platform = gcwz
    else
        $(warning Unsupported CPU architecture, using lincpp)
        platform = lincpp
    endif

    FLAGS = $(shell grep flags /proc/cpuinfo)
    ifneq (,$(findstring sse4_1,$(FLAGS)))
        platform += sse4_1
    endif
endif

$(info Platform: $(platform))

# Generic 32 bit x86 (a.k.a. i386/i486/i686)
ifneq (,$(findstring x86,$(platform)))
    X86_REC := 1
    NOT_ARM := 1
ifndef USE_SDL
    USE_X11 := 1
endif
    MFLAGS += -m32
    ASFLAGS += --32
    LDFLAGS += -m32
    CFLAGS += -m32 -D TARGET_LINUX_x86 -D TARGET_NO_AREC -fno-builtin-sqrtf
    CXXFLAGS += -fexceptions

# Generic 64 bit x86 (a.k.a. x64/AMD64/x86_64/Intel64/EM64T)
else ifneq (,$(findstring x64,$(platform)))
    X64_REC := 1
    NOT_ARM := 1
ifndef USE_SDL
    USE_X11 := 1
endif
	USE_VULKAN ?= 1
    CFLAGS += -D TARGET_LINUX_x64 -fno-builtin-sqrtf
    CXXFLAGS += -fexceptions
    LDFLAGS := -no-pie

# Generic 64 bit ARM (armv8) Linux
else ifneq (,$(findstring arm64,$(platform)))
    NOT_ARM := 1
ifndef USE_SDL
    USE_X11 := 1
endif
    ARM64_REC := 1
    ISARM64 := 1
    CFLAGS += -D TARGET_LINUX_ARMv8 -fno-builtin-sqrtf
    CXXFLAGS += -fexceptions

# Generic 32 bit ARMhf (a.k.a. ARMv7h)
else ifneq (,$(findstring armv7h,$(platform)))
    MFLAGS += -marm -mfloat-abi=hard -march=armv7-a -funroll-loops
    ASFLAGS += -mfloat-abi=hard -march=armv7-a
    ifneq (,$(findstring neon,$(platform)))
        MFLAGS += -mfpu=neon
        ASFLAGS += -mfpu=neon
    endif
    CFLAGS += -D TARGET_BEAGLE -D TARGET_LINUX_ARMELv7 -DARM_HARDFP -fsingle-precision-constant
    USE_GLES := 1

# LinCPP
else ifneq (,$(findstring lincpp,$(platform)))
    CPP_REC := 1
    NOT_ARM := 1
    USE_X11 := 1
    CFLAGS += -D TARGET_LINUX_x64 -D TARGET_NO_JIT
    CXXFLAGS += -fexceptions -std=gnu++11

# Raspberry Pi 2
else ifneq (,$(findstring rpi,$(platform)))
    CFLAGS += -D TARGET_BEAGLE -D TARGET_LINUX_ARMELv7 -DARM_HARDFP -fsingle-precision-constant
    ifneq (,$(findstring rpi2,$(platform)))
        MFLAGS += -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard
        ASFLAGS += -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard
    else ifneq (,$(findstring rpi3,$(platform)))
        MFLAGS += -march=armv8-a+crc -mtune=cortex-a53 -mfpu=neon-fp-armv8 -mfloat-abi=hard -marm
        ASFLAGS += -march=armv8-a+crc -mtune=cortex-a53 -mfpu=neon-fp-armv8 -mfloat-abi=hard -marm
    endif
    ifneq (,$(findstring mesa,$(platform)))
        USE_SDL := 1
        USE_GLES := 1
    else
        INCS += -I/opt/vc/include/ -I/opt/vc/include/interface/vmcs_host/linux -I/opt/vc/include/interface/vcos/pthreads
        LIBS += -L/opt/vc/lib/ -lbcm_host -ldl
        LIBS += -lbrcmEGL -lbrcmGLESv2
        CFLAGS += -D TARGET_VIDEOCORE -DGLES
        USE_OMX := 1
        USE_DISPMANX := 1
    endif
    undefine USE_X11

# BeagleBone Black
else ifneq (,$(findstring beagle,$(platform)))
    CC_PREFIX ?= arm-none-linux-gnueabi-
    MFLAGS += -marm -march=armv7-a -mtune=cortex-a9 -mfpu=neon -mfloat-abi=softfp -funroll-loops
    ASFLAGS := -march=armv7-a -mfpu=neon -mfloat-abi=softfp
    CFLAGS += -D TARGET_BEAGLE -fsingle-precision-constant
    USE_GLES := 1

# Pandora
else ifneq (,$(findstring pandora,$(platform)))
    FOR_PANDORA := 1
    USE_X11 := 1
    USE_SDL := 1
    PGO_USE := 1
    USE_GLES := 1
    MFLAGS +== -marm -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp -funroll-loops -fpermissive
    ASFLAGS += -march=armv7-a -mfpu=neon -mfloat-abi=softfp
    CFLAGS += -D TARGET_PANDORA  -D WEIRD_SLOWNESS -fsingle-precision-constant

# ODROIDs
else ifneq (,$(findstring odroid,$(platform)))
    MFLAGS += -marm -mfpu=neon -mfloat-abi=hard -funroll-loops
    ASFLAGS += -mfpu=neon -mfloat-abi=hard
    CFLAGS += -D TARGET_BEAGLE -D TARGET_LINUX_ARMELv7 -DARM_HARDFP -fsingle-precision-constant
    USE_GLES := 1

    # ODROID-XU3, -XU3 Lite & -XU4
    ifneq (,$(findstring odroidxu3,$(platform)))
        MFLAGS += -march=armv7ve -mtune=cortex-a15.cortex-a7
        ASFLAGS += -march=armv7ve
		CFLAGS += -DGLES3
    # Other ODROIDs
    else
        MFLAGS += -march=armv7-a
        ASFLAGS += -march=armv7-a

        # ODROID-C1 & -C1+
        ifneq (,$(findstring odroidc1,$(platform)))
            MFLAGS += -mtune=cortex-a5

        # ODROID-U2, -U3, -X & -X2
        else
            MFLAGS += -mtune=cortex-a9

        endif
    endif

# GCW Zero
else ifneq (,$(findstring gcwz,$(platform)))
    NOT_ARM := 1
    NO_REC := 1
    CC_PREFIX ?= /opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-
    CFLAGS += -D TARGET_GCW0 -D TARGET_NO_REC -fsingle-precision-constant
    LIBS += -L./gcwz/enta_viv -lglapi
    GCWZ_PKG = reicast-gcwz.opk
    GCWZ_PKG_FILES = gcwz/default.gcw0.desktop gcwz/icon-32.png

# Vero4K
else ifneq (,$(findstring vero4k,$(platform)))
    MFLAGS += -marm -march=armv8-a+crc -mtune=cortex-a53 -mfloat-abi=hard -funsafe-math-optimizations -funroll-loops
    ASFLAGS += -mfloat-abi=hard
    ifneq (,$(findstring neon,$(platform)))
        MFLAGS += -mfpu=neon
        ASFLAGS += -mfpu=neon
    endif
    CFLAGS += -D TARGET_BEAGLE -D TARGET_LINUX_ARMELv7 -DARM_HARDFP -fsingle-precision-constant
    INCS += -I/opt/vero3/include/
    LIBS += -L/opt/vero3/lib/ -lEGL -lGLESv2
    USE_GLES := 1
    USE_SDL := 1

# Windows
else ifneq (,$(findstring win32,$(platform)))
    NOT_ARM := 1
    CFLAGS += -fno-builtin-sqrtf -funroll-loops -DHAVE_FSEEKO -I /mingw64/include
    LDFLAGS += -static-libgcc -static-libstdc++ -Wl,-subsystem,windows
    LIBS := -lopengl32 -lwinmm -lgdi32 -lwsock32 -lws2_32 -ldsound -lcomctl32 -lcomdlg32 -lxinput -liphlpapi -Wl,-Bstatic -lgomp
    PLATFORM_EXT := exe
    CC = gcc
    CXX = g++
    USE_SDL = 1
ifeq ($(WITH_DYNAREC), x86)
    X86_REC := 1
    LDFLAGS += -m32
    CFLAGS += -m32 -D TARGET_NO_AREC
else
    X64_REC := 1
endif
    undefine USE_X11
    undefine USE_ALSA
    undefine USE_OSS
    undefine USE_EVDEV
    undefine USE_UDEV
    undefine FOR_LINUX
    NO_NIXPROF := 1
    FOR_WINDOWS := 1
	USE_VULKAN ?= 1
else
    $(error Unknown platform)
endif

RZDCY_SRC_DIR = $(LOCAL_PATH)/../../core
include $(RZDCY_SRC_DIR)/core.mk

LDFLAGS += -g -Wl,-Map,$(notdir $@).map,--gc-sections -Wl,-O3 -Wl,--sort-common -fopenmp

CFLAGS += $(RZDCY_CFLAGS) -g -O3 -D NDEBUG -c -D USES_HOMEDIR -fopenmp #-D NO_REND
CFLAGS += -frename-registers -fno-strict-aliasing
CFLAGS += -ffast-math -ftree-vectorize  -Wno-unused-result

CXXFLAGS += $(RZDCY_CFLAGS) -fno-rtti -fno-operator-names -D_GLIBCXX_USE_CXX11_ABI=0 -std=gnu++11

INCS += -I$(RZDCY_SRC_DIR) -I$(RZDCY_SRC_DIR)/deps -I$(RZDCY_SRC_DIR)/khronos

ifdef FOR_WINDOWS
	LIBS += -lm -static -lpthread
else
	LIBS += -lm -lpthread
endif
ifdef FOR_LINUX
LIBS += -lrt
endif

PREFIX ?= /usr/local
MAN_DIR ?= ${PREFIX}/share/man/man1
MENUENTRY_DIR ?= ${PREFIX}/share/applications
ICON_DIR ?= ${PREFIX}/share/pixmaps

ifdef USE_SDL
	ifdef FOR_WINDOWS
	    CXXFLAGS += -D USE_SDL
	    LIBS += -L$(RZDCY_SRC_DIR)/deps/SDL2-2.0.12/x86_64-w64-mingw32/lib -lsdl2 -ldinput8  -ldxguid -ldxerr8
	    LIBS += -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lsetupapi -lversion -luuid
	else
	    CXXFLAGS += `sdl2-config --cflags` -D USE_SDL
	    LIBS += `sdl2-config --libs`
	endif
endif

ifdef PGO_MAKE
    CFLAGS += -fprofile-generate -pg
    LDFLAGS += -fprofile-generate
else
    CFLAGS += -fomit-frame-pointer
endif

ifdef PGO_USE
    CFLAGS += -fprofile-use
endif

ifdef LTO_TEST
    CFLAGS += -flto -fwhole-program
    LDFLAGS +=-flto -fwhole-program
endif

ifdef USE_DISPMANX
    CFLAGS += -D SUPPORT_DISPMANX
    CXXFLAGS += -D SUPPORT_DISPMANX
endif

ifdef USE_X11
    CFLAGS += `pkg-config --cflags x11` -D SUPPORT_X11
    CXXFLAGS += `pkg-config --cflags x11` -D SUPPORT_X11
    LIBS += `pkg-config --libs x11`
endif

ifdef USE_EVDEV
    CXXFLAGS += -D USE_EVDEV
ifdef USE_UDEV
    CXXFLAGS += -D USE_UDEV
    LIBS += -ludev
endif
endif

ifdef USE_JOYSTICK
    CXXFLAGS += -D USE_JOYSTICK
endif

ifdef USE_OMX
    CXXFLAGS += -D USE_OMX
    LIBS += -lopenmaxil
endif

ifdef USE_ALSA
    CXXFLAGS += `pkg-config --cflags alsa` -D USE_ALSA
    LIBS += `pkg-config --libs alsa`
endif

ifdef USE_OSS
    CXXFLAGS += -D USE_OSS
endif

ifdef USE_PULSEAUDIO
    CXXFLAGS += `pkg-config --cflags libpulse-simple` -D USE_PULSEAUDIO
    LIBS += `pkg-config --libs libpulse-simple`
endif

ifdef USE_SDLAUDIO
    CXXFLAGS += `sdl2-config --cflags` -D USE_SDL_AUDIO
    LIBS += `sdl2-config --libs`
endif

ifdef USE_LIBAO
	CXXFLAGS += `pkg-config --cflags ao` -D USE_LIBAO
	LIBS += `pkg-config --libs ao`
endif

# these are also handled on core.mk, but ignored here

# GLES on x11?
ifdef USE_GLES
    CFLAGS += -DGLES
    LIBS += -lEGL -lGLESv2
else ifdef FOR_LINUX
    LIBS += -ldl -lGL #for desktop gl
endif

ifdef TEST_AUTOMATION
    CFLAGS += -DTEST_AUTOMATION
endif

ifdef DEBUGFAST
    CFLAGS += -DDEBUGFAST
endif

ifdef UNIT_TESTS
    CFLAGS += -DUNIT_TESTS
endif

ifdef ASAN
	CFLAGS += -fsanitize=address -static-libasan
	LDFLAGS += -fsanitize=address -static-libasan
endif 

ifdef LOG_TO_PTY
    CFLAGS += -D LOG_TO_PTY
endif


EXECUTABLE_STRIPPED=nosym-reicast.$(PLATFORM_EXT)
DC_PLATFORM=dreamcast
EXECUTABLE=reicast.$(PLATFORM_EXT)
EXECUTABLE_NAME=reicast

ifndef NOT_ARM
    AS=$(CC)
    ASFLAGS += $(CFLAGS)
endif

CHAR_EMPTY:=
CHAR_SPACE:=$(CHAR_EMPTY) $(CHAR_EMPTY)
BUILDDIR:=obj-$(DC_PLATFORM)-$(subst $(CHAR_SPACE),-,$(platform))

OBJECTS=$(RZDCY_FILES:.cpp=.build_obj)
OBJECTS:=$(OBJECTS:.cc=.build_obj)
OBJECTS:=$(OBJECTS:.c=.build_obj)
OBJECTS:=$(OBJECTS:.S=.build_obj)
OBJECTS:=$(patsubst $(RZDCY_SRC_DIR)/%,$(BUILDDIR)/%,$(OBJECTS))

ifdef FOR_WINDOWS
OBJECTS+=$(BUILDDIR)/reicastres.build_obj
$(BUILDDIR)/reicastres.build_obj: $(LOCAL_PATH)/../windows/reicast.rc $(LOCAL_PATH)/../windows/flycast.ico $(RZDCY_SRC_DIR)/version.h
	$(WINDRES) $< $@
endif

DEPDIR := .dep-$(BUILDDIR)
DEPFLAGS = -MT $@ -MD -MP -MF $(DEPDIR)/$*.Td
DEPS=$(RZDCY_FILES:.cpp=.d)
DEPS:=$(DEPS:.cc=.d)
DEPS:=$(DEPS:.c=.d)
DEPS:=$(DEPS:.S=.d)
DEPS:=$(patsubst $(RZDCY_SRC_DIR)/%,$(DEPDIR)/%,$(DEPS))
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

all: $(CPPFILES) $(EXECUTABLE) $(EXECUTABLE_STRIPPED)
ifneq (,$(findstring gcwz,$(platform)))
	mksquashfs $(EXECUTABLE_STRIPPED) $(GCWZ_PKG_FILES) $(GCWZ_PKG) -all-root
endif

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(MFLAGS) $(EXTRAFLAGS) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@

$(EXECUTABLE_STRIPPED): $(EXECUTABLE)
	cp $< $@ && $(STRIP) $@

$(BUILDDIR)/%.build_obj : $(RZDCY_SRC_DIR)/%.cpp
$(BUILDDIR)/%.build_obj: $(RZDCY_SRC_DIR)/%.cpp $(DEPDIR)/%.d $(VERSION_HEADER)
	mkdir -p $(dir $@)
	mkdir -p .dep-$(dir $@)
	$(CXX) $(EXTRAFLAGS) $(INCS) $(DEPFLAGS) $(CFLAGS) $(MFLAGS) $(CXXFLAGS) $< -o $@
	$(POSTCOMPILE)

$(BUILDDIR)/%.build_obj : $(RZDCY_SRC_DIR)/%.cc
$(BUILDDIR)/%.build_obj: $(RZDCY_SRC_DIR)/%.cc $(DEPDIR)/%.d $(VERSION_HEADER)
	mkdir -p $(dir $@)
	mkdir -p .dep-$(dir $@)
	$(CXX) $(EXTRAFLAGS) $(INCS) $(DEPFLAGS) $(CFLAGS) $(MFLAGS) $(CXXFLAGS) $< -o $@
	$(POSTCOMPILE)

$(BUILDDIR)/%.build_obj : $(RZDCY_SRC_DIR)/%.c
$(BUILDDIR)/%.build_obj: $(RZDCY_SRC_DIR)/%.c $(DEPDIR)/%.d $(VERSION_HEADER)
	mkdir -p $(dir $@)
	mkdir -p .dep-$(dir $@)
	$(CC) $(EXTRAFLAGS) $(INCS) $(DEPFLAGS) $(CFLAGS) $< -o $@
	$(POSTCOMPILE)

$(BUILDDIR)/%.build_obj : $(RZDCY_SRC_DIR)/%.S $(VERSION_HEADER)
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $(INCS) $< -o $@

install: $(EXECUTABLE)
	mkdir -p $(DESTDIR)$(PREFIX)/bin 2>/dev/null || true
	mkdir -p $(DESTDIR)$(PREFIX)/share/reicast/mappings 2>/dev/null || true
	mkdir -p $(DESTDIR)$(MAN_DIR) 2>/dev/null || true
	mkdir -p $(DESTDIR)$(MENUENTRY_DIR) 2>/dev/null || true
	mkdir -p $(DESTDIR)$(ICON_DIR) 2>/dev/null || true
	install -m755 $(EXECUTABLE) $(DESTDIR)$(PREFIX)/bin/$(EXECUTABLE_NAME)
	install -m755 tools/reicast-joyconfig.py $(DESTDIR)$(PREFIX)/bin/reicast-joyconfig
	install -m644 mappings/controller_gcwz.cfg $(DESTDIR)$(PREFIX)/share/reicast/mappings
	install -m644 mappings/controller_generic.cfg $(DESTDIR)$(PREFIX)/share/reicast/mappings
	install -m644 mappings/controller_pandora.cfg $(DESTDIR)$(PREFIX)/share/reicast/mappings
	install -m644 mappings/controller_xboxdrv.cfg $(DESTDIR)$(PREFIX)/share/reicast/mappings
	install -m644 mappings/controller_xpad.cfg $(DESTDIR)$(PREFIX)/share/reicast/mappings
	install -m644 mappings/keyboard.cfg $(DESTDIR)$(PREFIX)/share/reicast/mappings
	install -m644 man/reicast.1 $(DESTDIR)$(MAN_DIR)
	install -m644 man/reicast-joyconfig.1 $(DESTDIR)$(MAN_DIR)
	install -m644 reicast.desktop $(DESTDIR)$(MENUENTRY_DIR)
	install -m644 flycast.png $(DESTDIR)$(ICON_DIR)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(EXECUTABLE_NAME)
	rm -f $(DESTDIR)$(PREFIX)/bin/reicast-joyconfig
	rm -f $(DESTDIR)$(PREFIX)/share/reicast/mappings/controller_gcwz.cfg
	rm -f $(DESTDIR)$(PREFIX)/share/reicast/mappings/controller_generic.cfg
	rm -f $(DESTDIR)$(PREFIX)/share/reicast/mappings/controller_pandora.cfg
	rm -f $(DESTDIR)$(PREFIX)/share/reicast/mappings/controller_xboxdrv.cfg
	rm -f $(DESTDIR)$(PREFIX)/share/reicast/mappings/controller_xpad.cfg
	rm -f $(DESTDIR)$(PREFIX)/share/reicast/mappings/keyboard.cfg
	rm -f $(DESTDIR)$(MAN_DIR)/reicast.1
	rm -f $(DESTDIR)$(MAN_DIR)/reicast-joyconfig.1
	rm -f $(DESTDIR)$(MENUENTRY_DIR)/reicast.desktop
	rm -f $(DESTDIR)$(ICON_DIR)/flycast.png

clean:
	rm -f $(OBJECTS) $(EXECUTABLE) $(EXECUTABLE_STRIPPED) .map $(VERSION_HEADER) $(TEST_OBJ) alltests

TEST_SRC_DIR:=$(LOCAL_PATH)/../../tests/src
TEST_SRC:=$(wildcard $(TEST_SRC_DIR)/*.cpp) $(RZDCY_SRC_DIR)/deps/gtest/src/gtest_main.cc $(RZDCY_SRC_DIR)/deps/gtest/src/gtest-all.cc
TEST_OBJ:=$(TEST_SRC:.cpp=.build_obj)
TEST_OBJ:=$(TEST_OBJ:.cc=.build_obj)
TEST_OBJ:=$(patsubst $(TEST_SRC_DIR)/%,$(BUILDDIR)/%,$(TEST_OBJ))
TEST_OBJ:=$(patsubst $(RZDCY_SRC_DIR)/%,$(BUILDDIR)/%,$(TEST_OBJ))

$(BUILDDIR)/%.build_obj : $(TEST_SRC_DIR)/%.cpp
$(BUILDDIR)/%.build_obj: $(TEST_SRC_DIR)/%.cpp $(DEPDIR)/%.d
	mkdir -p $(dir $@)
	mkdir -p .dep-$(dir $@)
	$(CXX) $(EXTRAFLAGS) $(INCS) $(DEPFLAGS) $(CFLAGS) $(MFLAGS) $(CXXFLAGS) $< -o $@
	$(POSTCOMPILE)

tests: alltests
	./alltests

alltests: $(TEST_OBJ) $(OBJECTS)
	$(CXX) $(MFLAGS) $(EXTRAFLAGS) $(LDFLAGS) $(TEST_OBJ) $(OBJECTS) $(LIBS) -o $@

.PRECIOUS = $(DEPDIR)/%.d
$(DEPDIR)/%.d: ;

-include $(DEPS)
