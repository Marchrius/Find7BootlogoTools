UNAME := $(shell uname -s)
COMPILE := echo "OS Not supported!"
CLEAN := echo "OS Not supported!"
MACH := $(shell uname -m)

ifeq "$(MACH)" "x86_64"
		ARCH := -m64
else
	ifeq "$(MACH)" "i386"
		ARCH := -m32
	endif
endif

ifeq "$(UNAME)" "Linux"
	COMPILE := g++ -o build/linux/bin/Find7BootlogoTools Find7BootlogoTools/main.cpp lodepng/lodepng.cpp -D__LINUX__ $(ARCH)
	CLEAN := rm -rf build/linux/bin
	MKDIR := $(shell mkdir -p build/linux/bin/)
endif

ifeq "$(UNAME)" "Darwin"
	ARCH := -arch $(MACH)
	COMPILE := xcodebuild $(ARCH)
	CLEAN := xcodebuild clean
endif


all:
	$(MKDIR)
	$(COMPILE)

clean:
	$(CLEAN)
