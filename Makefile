# We want to build just enough to use other stuff
TIER_FILES := tier0/lib.cpp tier0/mem.cpp tier0/platform.cpp tier1/utlbuffer.cpp tier1/interface.cpp tier1/utlstring.cpp tier1/utlvector.cpp tier1/utlmap.cpp tier1/commandline.cpp tier2/filesystem.cpp tier2/filesystem_libc.cpp tier2/fileformats/ini.cpp

FPC_FILES := main.cpp library/runner.cpp library/helper.cpp library/c.cpp library/ld.cpp library/clang/c.cpp library/clang/ld.cpp library/target.cpp
CC = clang
OUTPUT_DIR = fpc 
CCFLAGS = -I../public -Ipublic -lc -lstdc++

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
CCFLAGS += -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk -std=c++11 -Wl,-export_dynamic
endif
ifeq ($(UNAME_S),Linux)
CCFLAGS += -rdynamic 
endif

recompile: ../build/tools
	./fpc build
	mv fpc_temp fpc

install: $(FPC_FILES) ../build/tools
	$(CC) -g $(TIER_FILES) $(FPC_FILES) $(CCFLAGS) -o $(OUTPUT_DIR)
	./fpc build
	mv fpc_temp fpc


../build/tools:
	mkdir -p ../build/tools

install_fpc:
	cp fpc ../build/tools
	cp -r public ../build/tools
	cp -r public/tier0 ../build/tools/public
	cp -r public/tier1 ../build/tools/public
	cp -r public/tier2 ../build/tools/public

auto: install install_fpc
