CC = g++
CFLAGS = -march=native -pipe -O3 #-O#n for nondebug
CXXFLAGS = $(CFLAGS) -fPIC -Wall #-g << debugging
LIBS =
FLAGS =
MAKEFLAGS = -j5

module_name=OCommands

TOPDIR=../../../
SRCDIR=src/
LIBDIR=$(TOPDIR).libs/
objects=$(SRCDIR)$(module_name).o
output=$(module_name).so

default: $(output)
Debug: all
Release: all
all: default


$(output): $(objects)
	$(CC) -shared -o $(LIBDIR)$(@) $(objects) $(CXXFLAGS) $(FLAGS) $(LIBS)

cleanDebug: clean
cleanRelease: clean
clean:
	rm -f $(SRCDIR)$(objects) $(LIBDIR)$(output)

cleanall: clean


