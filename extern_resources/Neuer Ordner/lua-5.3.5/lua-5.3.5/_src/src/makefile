.PHONY: all
.SILENT:

CC=D:\Qt\Tools\mingw730_64\bin\x86_64-w64-mingw32-gcc.exe -std=gnu99
STRIP=D:\Qt\Tools\mingw730_64\bin\strip.exe

CFILES=lapi.c lcode.c lctype.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c lmem.c lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c ltm.c lundump.c lvm.c lzio.c lauxlib.c lbaselib.c lbitlib.c lcorolib.c ldblib.c liolib.c lmathlib.c loslib.c lstrlib.c ltablib.c lutf8lib.c loadlib.c linit.c
CFLAGS=-O2 -Wall -Wextra -DLUA_COMPAT_5_2

lua-5.3.5.dll:
	$(CC) $(CFLAGS) -DLUA_BUILD_AS_DLL $(CFILES) -s -shared -o $@
	$(STRIP) --strip-unneeded $@

lua-5.3.5.lib:
	$(CC) $(CFLAGS) -DLUA_BUILD_AS_LIB $(CFILES) -s -shared -o $@
	$(STRIP) --strip-unneeded $@

lua-5.3.5.exe:
	echo $(CC) -s -lm -o $@ lua.c lua-5.3.5.dll

luac-5.3.5.exe:
	echo $(CC) -s -lm -o $@ luac.c lua-5.3.5.dll

all: lua-5.3.5.dll lua-5.3.5.lib lua-5.3.5.exe luac-5.3.5.exe
