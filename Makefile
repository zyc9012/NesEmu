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
CINCLUDES = `pkg-config --cflags portaudio-2.0` `pkg-config --cflags glfw3`
LDFLAGS_GLEW = -lGLEW
LDFLAGS_GLFW = `pkg-config --static --libs glfw3`
LDFLAGS_PORTAUDIO = `pkg-config --static --libs portaudio-2.0` 
LDFLAGS = $(LDFLAGS_GLEW) $(LDFLAGS_GLFW) $(LDFLAGS_PORTAUDIO)

ifdef DEBUG
	COMMONFLAGS := $(COMMONFLAGS) -g
endif
CFLAGS = $(COMMONFLAGS) --std=c99 $(CINCLUDES)
CXXFLAGS = $(COMMONFLAGS) --std=c++11 $(CINCLUDES)
DEPDIR = deps

linux:
	$(MAKE) $(BINFILE)

mingw:
	$(MAKE) "BINFILE=NesEmu.exe" "LDFLAGS_GLEW= -static -lglew32" $(BINFILE).exe

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
	$(Q)rm -f bin/$(BINFILE) obj/* Makefile.dep
