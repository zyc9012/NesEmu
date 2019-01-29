ifdef VERBOSE
	Q =
	E = @true 
else
	Q = @
	E = @echo 
endif

CFILES := $(shell find NesEmu -mindepth 1 -maxdepth 4 -name "*.c")
CXXFILES := $(shell find NesEmu -mindepth 1 -maxdepth 4 -name "*.cpp")

INFILES := $(CFILES) $(CXXFILES)

OBJFILES := $(CXXFILES:NesEmu/%.cpp=%) $(CFILES:NesEmu/%.c=%)
DEPFILES := $(CXXFILES:NesEmu/%.cpp=%) $(CFILES:NesEmu/%.c=%)
OFILES := $(OBJFILES:%=obj/%.o)

BINFILE = NesEmu

COMMONFLAGS = -O3 -Wno-unused-result
CINCLUDES = `pkg-config --cflags glew glfw3 portaudio-2.0`
LDFLAGS = `pkg-config --static --libs glew glfw3 portaudio-2.0`

ifdef DEBUG
	COMMONFLAGS := $(COMMONFLAGS) -g
endif
CFLAGS = $(COMMONFLAGS) --std=c99 $(CINCLUDES)
CXXFLAGS = $(COMMONFLAGS) --std=c++11 $(CINCLUDES)
DEPDIR = deps

linux:
	$(MAKE) $(BINFILE)

mingw:
	$(MAKE) "BINFILE=NesEmu.exe" "LDFLAGS:= -static $(LDFLAGS)" $(BINFILE).exe

osx:
	$(MAKE) "STRIP=strip" "LDFLAGS:= $(LDFLAGS) -framework OpenGL" $(BINFILE)

ifeq ($(MAKECMDGOALS),)
-include Makefile.dep
endif
ifneq ($(filter-out clean, $(MAKECMDGOALS)),)
-include Makefile.dep
endif

CC = gcc
CXX = g++
STRIP = strip --strip-unneeded

-include Makefile.local

.PHONY: clean all depend
.SUFFIXES:
obj/%.o: NesEmu/%.c
	$(E)C-compiling $<
	$(Q)if [ ! -d `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(Q)$(CC) -o $@ -c $< $(CFLAGS)
obj/%.o: NesEmu/%.cpp
	$(E)C++-compiling $<
	$(Q)if [ ! -d `dirname $@` ]; then mkdir -p `dirname $@`; fi
	$(Q)$(CXX) -o $@ -c $< $(CXXFLAGS)
Makefile.dep: $(CFILES) $(CXXFILES)
	$(E)Depend
	$(Q)for i in $(^); do $(CXX) $(CXXFLAGS) -MM "$${i}" -MT obj/`basename $${i%.*}`.o; done > $@


$(BINFILE): $(OFILES)
	$(E)Linking $@
	$(Q)if [ ! -d `dirname bin/$@` ]; then mkdir -p `dirname bin/$@`; fi
	$(Q)$(CXX) -o bin/$@ $(OFILES) $(LDFLAGS)
	$(Q)$(STRIP) bin/$@
clean:
	$(E)Removing files
	$(Q)rm -r -f bin/$(BINFILE) obj Makefile.dep
