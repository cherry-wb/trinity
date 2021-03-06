VERSION=1.1pre

INSTALL_PREFIX ?= $(DESTDIR)
INSTALL_PREFIX ?= $(HOME)

CFLAGS = -Wall -W -g -O2 -I. -Wimplicit -D_FORTIFY_SOURCE=2 -DVERSION="$(VERSION)" -D_GNU_SOURCE
#CFLAGS += $(shell if $(CC) -m32 -S -o /dev/null -xc /dev/null >/dev/null 2>&1; then echo "-m32"; fi)
CFLAGS += -Wdeclaration-after-statement
CFLAGS += -Wformat-security
CFLAGS += -Wformat-y2k
CFLAGS += -Winit-self
CFLAGS += -Wnested-externs
CFLAGS += -Wpacked
CFLAGS += -Wshadow
CFLAGS += -Wstrict-aliasing=3
CFLAGS += -Wswitch-default
CFLAGS += -Wswitch-enum
CFLAGS += -Wundef
CFLAGS += -Wwrite-strings
CFLAGS += -Wformat

all: trinity

MACHINE		= $(shell $(CC) -dumpmachine)
SYSCALLS_ARCH	= $(patsubst %.c,%.o,$(shell case "$(MACHINE)" in \
		  (sh*) echo syscalls/sh/*.c ;; \
		  esac))

HEADERS		= $(patsubst %.h,%.h,$(wildcard *.h)) syscalls/syscalls.h $(patsubst %.h,%.h,$(wildcard ioctls/*.h))
SYSCALLS	= $(patsubst %.c,%.o,$(wildcard syscalls/*.c))
IOCTLS		= $(patsubst %.c,%.o,$(wildcard ioctls/*.c))
OBJS		= trinity.o \
			child.o \
			main.o \
			fds.o \
			files.o \
			generic-sanitise.o \
			log.o \
			maps.o \
			seed.o \
			sockets.o \
			sockaddr.o \
			syscall.o \
			params.o \
			pids.o \
			tables.o \
			watchdog.o \
			$(SYSCALLS) \
			$(SYSCALLS_ARCH) \
			$(SANITISE) \
			$(IOCTLS)
-include $(OBJS:.o=.d)

trinity: $(OBJS) $(HEADERS)
	$(CC) $(CFLAGS) -o trinity $(OBJS)
	@mkdir -p tmp

DEPDIR= .deps
df = $(DEPDIR)/$(*F)

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<
	@gcc -MM $(CFLAGS) $*.c > $(df).d
	@mv -f $(df).d $(df).d.tmp
	@sed -e 's|.*:|$*.o:|' <$(df).d.tmp > $(df).d
	@sed -e 's/.*://' -e 's/\\$$//' < $(df).d.tmp | fmt -1 | \
	  sed -e 's/^ *//' -e 's/$$/:/' >> $(df).d
	@rm -f $(df).d.tmp

clean:
	@rm -f *.o syscalls/*.o syscalls/ia64/*.o syscalls/powerpc/*.o ioctls/*.o
	@rm -f core.*
	@rm -f trinity
	@rm -f $(DEPDIR)/*.d

release:
	git repack -a -d
	git prune-packed
	git archive --format=tar.gz --prefix=trinity-$(VERSION)/ HEAD > trinity-$(VERSION).tgz

install:
	install -d -m 755 $(INSTALL_PREFIX)/bin
	install trinity $(INSTALL_PREFIX)/bin
