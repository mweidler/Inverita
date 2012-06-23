COMMIT_VERSION = $(shell git describe HEAD 2>/dev/null | sed 's/-/./;s/\([^-]*\).*/\1/')
TAG_VERSION = $(shell git describe HEAD 2>/dev/null | sed 's/\([^-]*\).*/\1/')

COPT = -W -Wall -Werror -O2
AOPT = $(COPT) $(shell getconf LFS_CFLAGS)
VOPT = $(AOPT) -DCOMMIT_VERSION='"$(COMMIT_VERSION)"'
LOPT = -s $(shell getconf LFS_LDFLAGS) $(shell getconf LFS_LIBS)

#getconf LFS_CFLAGS: RHEL 5.7 64Bit: empty
# Ubuntu 10.04 32Bit: -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
# Linux Mint 12 64Bit: empty

all: alvara

alvara: alvara_main.cpp version.h sha1.o ContentList.o StreamPersistence.o Alvara.o
@if [ "$(TAG_VERSION)" != "" ]; then echo "#define ALVARA_VERSION \"$(TAG_VERSION)\"" >version.h; fi
g++ $(VOPT) $(LOPT) alvara_main.cpp sha1.o ContentList.o StreamPersistence.o Alvara.o -o alvara

