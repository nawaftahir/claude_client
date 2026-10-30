/*
 * guidpin.so: LD_PRELOAD shim for the release link, next to libfaketime.
 * The PDB GUID in the exe comes from rpcrt4 UuidCreate, which under Wine reads
 * 16 bytes from /dev/urandom. Every 16-byte read from /dev/urandom is answered
 * with the bytes in GUIDPIN (32 hex digits, exe byte order), so the linker
 * writes the retail GUID itself. All other I/O goes straight to the kernel.
 * Build (32-bit, the image is i386): gcc -m32 -shared -fPIC -O2 -o guidpin.so guidpin.c
 */
#define _GNU_SOURCE
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

static int urandom_fd = -1;

static int hexval(char c)
{
	if (c >= '0' && c <= '9') return c - '0';
	if (c >= 'a' && c <= 'f') return c - 'a' + 10;
	if (c >= 'A' && c <= 'F') return c - 'A' + 10;
	return -1;
}

static int pinned(unsigned char *out)
{
	const char *s = getenv("GUIDPIN");
	int i;

	if (!s || strlen(s) != 32)
		return 0;
	for (i = 0; i < 16; i++) {
		int hi = hexval(s[2 * i]), lo = hexval(s[2 * i + 1]);
		if (hi < 0 || lo < 0)
			return 0;
		out[i] = (unsigned char)(hi << 4 | lo);
	}
	return 1;
}

static int do_open(const char *path, int flags, mode_t mode)
{
	int fd = (int)syscall(SYS_open, path, flags, mode);

	if (fd >= 0 && strcmp(path, "/dev/urandom") == 0)
		urandom_fd = fd;
	return fd;
}

int open(const char *path, int flags, ...)
{
	mode_t mode = 0;

	if (flags & O_CREAT) {
		va_list ap;
		va_start(ap, flags);
		mode = va_arg(ap, mode_t);
		va_end(ap);
	}
	return do_open(path, flags, mode);
}

int open64(const char *path, int flags, ...)
{
	mode_t mode = 0;

	if (flags & O_CREAT) {
		va_list ap;
		va_start(ap, flags);
		mode = va_arg(ap, mode_t);
		va_end(ap);
	}
	return do_open(path, flags | O_LARGEFILE, mode);
}

ssize_t read(int fd, void *buf, size_t count)
{
	if (fd >= 0 && fd == urandom_fd && count == 16 && pinned((unsigned char *)buf))
		return 16;
	return (ssize_t)syscall(SYS_read, fd, buf, count);
}
