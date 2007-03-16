##############################################################################
# USER CONFIGURATION
# You MUST set all of the following variables for the compilation to succeed.
##############################################################################

##
# Paths to includes and libraries.
# BOTAN_INC: botan include directory (botan/botan.h must exist)
# BOTAN_LIB: directory that contains the libbotan.so (or .a)
# BOOST_INC: boost include directory (boost/program_options.hpp must exist)  
##
BOTAN_INC = .
BOTAN_LIB = .
BOOST_INC = .

##
# Change PREFIX to install to different directories. The binary is installed
# in $PREFIX/bin, and the man in $PREFIX/man/man1
##
PREFIX = /usr/local

##
# Sometimes you have only dynamic libraries available. In that case COMMENT
# the following line.
##
#LINK_STATIC = -static

##############################################################################
# NO USER MODIFIABLE PARTS AFTER THIS POINT
##############################################################################
CXXFLAGS = -g -Wall -W -I$(BOTAN_INC) -I$(BOOST_INC)
LDFLAGS  = -L$(BOTAN_LIB) $(LINK_STATIC) -lbotan -lm

.PHONY : all install-strip install clean 

OBJS = main.o random_item.o filters.o diceware8k.o skeylist.o

all: secpwgen

secpwgen: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

install-strip: secpwgen
	strip secpwgen
	$(MAKE) install

install: secpwgen
	cp -i secpwgen $(PREFIX)/bin
	cp -i secpwgen.1 $(PREFIX)/man/man1

clean:
	rm -f *.o secpwgen

main.o: main.cc filters.h
random_item.o: random_item.cc random_item.h
filters.o: filters.cc filters.h
diceware8k.o: diceware8k.cc random_item.h
skeylist.o: skeylist.cc random_item.h
