#pragma once

#include <chrono>
#include <initializer_list>

#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/uio.h>

#include "io.h"


namespace posix {


void access(const char *path, int amode);
void chdir(const char *path);
void chmod(const char *path, mode_t mode);
void chown(const char *path, uid_t owner, gid_t group);
void close(int fildes);
int creat(const char *path, mode_t mode = 0666);
int dup(int fildes);
int dup2(int fildes, int fildes2);
void faccessat(int fd, const char *path, int amode, int flag = 0);
void fadvise(int fd, off_t offset, off_t len, int advice);
void fallocate(int fd, off_t offset, off_t len);
void fchdir(int fildes);
void fchmod(int fildes, mode_t mode);
void fchmodat(int fd, const char *path, mode_t mode, int flag = 0);
void fchown(int fildes, uid_t owner, gid_t group);
void fchownat(int fd, const char *path, uid_t owner, gid_t group, int flag = 0);
int fcntl(int fildes, int cmd);
int fcntl(int fildes, int cmd, int arg);
int fcntl(int fildes, int cmd, void *arg);
void fdatasync(int fildes);
void fstat(int fildes, struct stat *buf);
void fstatat(int fd, const char * _restrict path, struct stat * _restrict buf, int flag = 0);
void fsync(int fildes);
void ftruncate(int fildes, off_t length = 0);
void futimens(int fd, const struct timespec times[2]);
int ioctl(int fildes, int request);
int ioctl(int fildes, int request, int arg);
int ioctl(int fildes, int request, void *arg);
bool isatty(int fildes);
void lchown(const char *path, uid_t owner, gid_t group);
void link(const char *path1, const char *path2);
void linkat(int fd1, const char *path1, int fd2, const char *path2, int flag = 0);
void lockf(int fildes, int function, off_t size);
off_t lseek(int fildes, off_t offset, int whence = SEEK_SET);
void lstat(const char * _restrict path, struct stat * _restrict buf);
void madvise(void *addr, size_t len, int advice);
void mkdir(const char *path, mode_t mode = 0777);
void mkdirat(int fd, const char *path, mode_t mode = 0777);
void mkfifo(const char *path, mode_t mode = 0666);
void mkfifoat(int fd, const char *path, mode_t mode = 0666);
void mknod(const char *path, mode_t mode, dev_t dev);
void mknodat(int fd, const char *path, mode_t mode, dev_t dev);
void * mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off);
void mprotect(void *addr, size_t len, int prot);
void msync(void *addr, size_t len, int flags);
void munmap(void *addr, size_t len);
int open(const char *path, int oflag, mode_t mode = 0666);
int openat(int fd, const char *path, int oflag, mode_t mode = 0666);
void pipe(int fildes[2]);
unsigned poll(struct pollfd fds[], nfds_t nfds, int timeout = -1);
ssize_t pread(int fildes, void *buf, size_t nbyte, off_t offset);
ssize_t preadv(int fd, const struct iovec iov[], int iovcnt, off_t offset);
size_t pwrite(int fildes, const void *buf, size_t nbyte, off_t offset);
size_t pwritev(int fd, const struct iovec iov[], int iovcnt, off_t offset);
ssize_t read(int fildes, void *buf, size_t nbyte);
ssize_t readv(int fildes, const struct iovec iov[], int iovcnt);
size_t readlink(const char * _restrict path, char * _restrict buf, size_t bufsize);
size_t readlinkat(int fd, const char * _restrict path, char * _restrict buf, size_t bufsize);
void rename(const char *oldpath, const char *newpath);
void renameat(int oldfd, const char *oldpath, int newfd, const char *newpath);
void rmdir(const char *path);
unsigned select(int nfds, fd_set * _restrict readfds, fd_set * _restrict writefds = nullptr, fd_set * _restrict errorfds = nullptr, struct timeval * _restrict timeout = nullptr);
void stat(const char * _restrict path, struct stat * _restrict buf);
void symlink(const char *path1, const char *path2);
void symlinkat(const char *path1, int fd, const char *path2);
void truncate(const char *path, off_t length = 0);
void unlink(const char *path);
void unlinkat(int fd, const char *path, int flag = 0);
void utimensat(int fd, const char *path, const struct timespec times[2], int flag = 0);
void utimes(const char *path, const struct timeval times[2]);
size_t write(int fildes, const void *buf, size_t nbyte);
size_t writev(int fildes, const struct iovec iov[], int iovcnt);

static inline unsigned select(int nfds, fd_set * _restrict readfds, fd_set * _restrict writefds, fd_set * _restrict errorfds, std::chrono::microseconds timeout) {
	struct timeval tv;
	tv.tv_sec = static_cast<std::time_t>(std::chrono::duration_cast<std::chrono::seconds>(timeout).count());
	tv.tv_usec = static_cast<decltype(tv.tv_usec)>((timeout % std::chrono::seconds(1)).count());
	return posix::select(nfds, readfds, writefds, errorfds, &tv);
}


class FDSet {

private:
	fd_set set;

public:
	FDSet() { this->clear(); }
	FDSet(std::initializer_list<int> fds) { this->clear(); for (int fd : fds) *this += fd; }

public:
	operator const fd_set & () const { return set; }
	operator fd_set & () { return set; }
	operator const fd_set * () const { return &set; }
	operator fd_set * () { return &set; }

	void clear() { FD_ZERO(&set); }
	FDSet & operator += (int fd) { FD_SET(fd, &set); return *this; }
	FDSet & operator -= (int fd) { FD_CLR(fd, &set); return *this; }
	bool operator & (int fd) const { return FD_ISSET(fd, &set); }

};


} // namespace posix


class FileDescriptor : public Source, public Sink {

public:
	class MemoryMapping {
		friend FileDescriptor;
	private:
		void *addr;
		size_t len;
	public:
		MemoryMapping(MemoryMapping &&move) : addr(move.addr), len(move.len) { move.addr = MAP_FAILED; }
		MemoryMapping & operator = (MemoryMapping &&move) { std::swap(addr, move.addr); std::swap(len, move.len); return *this; }
		~MemoryMapping() { if (addr != MAP_FAILED) posix::munmap(addr, len); }
	private:
		MemoryMapping(void *addr, size_t len) : addr(addr), len(len) { }
		MemoryMapping(const MemoryMapping &) = delete;
		MemoryMapping & operator = (const MemoryMapping &) = delete;
	public:
		_pure operator void * () const { return addr; }
		void * _pure data() const { return addr; }
		size_t _pure size() const { return len; }
		void madvise(size_t offset, size_t len, int advice) { posix::madvise(static_cast<uint8_t *>(addr) + offset, len, advice); }
		void mprotect(size_t offset, size_t len, int prot) { posix::mprotect(static_cast<uint8_t *>(addr) + offset, len, prot); }
		void msync(size_t offset, size_t len, int flags = MS_ASYNC) { posix::msync(static_cast<uint8_t *>(addr) + offset, len, flags); }
	};

protected:
	int fd;

public:
	FileDescriptor() : fd(-1) { }
	explicit FileDescriptor(int fd) : fd(fd) { }
	explicit FileDescriptor(const char *path, int oflag = O_RDONLY | O_CLOEXEC, mode_t mode = 0666) : fd(posix::open(path, oflag, mode)) { }
	FileDescriptor(FileDescriptor &&move) : fd(move.fd) { move.fd = -1; }
	FileDescriptor & operator = (FileDescriptor &&move) { std::swap(fd, move.fd); return *this; }
	virtual ~FileDescriptor() { if (fd >= 0) posix::close(fd); }

private:
	FileDescriptor(const FileDescriptor &) = delete;
	FileDescriptor & operator = (const FileDescriptor &) = delete;

public:
	operator int () const { return fd; }

	void creat(const char *path, mode_t mode = 0666) { *this = FileDescriptor(posix::creat(path, mode)); }
	void open(const char *path, int oflag = O_RDONLY | O_CLOEXEC, mode_t mode = 0666) { *this = FileDescriptor(path, oflag, mode); }
	void close() { posix::close(fd), fd = -1; }
	ssize_t read(void *buf, size_t n) override { return posix::read(fd, buf, n); }
	size_t write(const void *buf, size_t n) override { return posix::write(fd, buf, n); }
	ssize_t readv(const struct iovec iov[], int iovcnt) { return posix::readv(fd, iov, iovcnt); }
	size_t writev(const struct iovec iov[], int iovcnt) { return posix::writev(fd, iov, iovcnt); }
	ssize_t pread(void *buf, size_t nbyte, off_t offset) const { return posix::pread(fd, buf, nbyte, offset); }
	size_t pwrite(const void *buf, size_t nbyte, off_t offset) { return posix::pwrite(fd, buf, nbyte, offset); }
	ssize_t preadv(const struct iovec iov[], int iovcnt, off_t offset) const { return posix::preadv(fd, iov, iovcnt, offset); }
	size_t pwritev(const struct iovec iov[], int iovcnt, off_t offset) { return posix::pwritev(fd, iov, iovcnt, offset); }
	void lockf(int function, off_t size) { posix::lockf(fd, function, size); }
	int fcntl(int cmd) { return posix::fcntl(fd, cmd); }
	int fcntl(int cmd, int arg) { return posix::fcntl(fd, cmd, arg); }
	int fcntl(int cmd, void *arg) { return posix::fcntl(fd, cmd, arg); }
	int ioctl(int request) { return posix::ioctl(fd, request); }
	int ioctl(int request, int arg) { return posix::ioctl(fd, request, arg); }
	int ioctl(int request, void *arg) { return posix::ioctl(fd, request, arg); }
	bool isatty() const { return posix::isatty(fd); }
	off_t lseek(off_t offset, int whence = SEEK_SET) { return posix::lseek(fd, offset, whence); }
	void fstat(struct stat *buf) const { posix::fstat(fd, buf); }
	void fchdir() const { posix::fchdir(fd); }
	void fchmod(mode_t mode) { posix::fchmod(fd, mode); }
	void fchown(uid_t owner, gid_t group) { posix::fchown(fd, owner, group); }
	void fallocate(off_t offset, off_t len) { posix::fallocate(fd, offset, len); }
	void ftruncate(off_t length = 0) { posix::ftruncate(fd, length); }
	void futimens(const struct timespec times[2]) { posix::futimens(fd, times); }
	void fsync() { posix::fsync(fd); }
	void fdatasync() { posix::fdatasync(fd); }
	void fadvise(off_t offset, off_t length, int advice) const { posix::fadvise(fd, offset, length, advice); }
	MemoryMapping mmap(off_t off, size_t len, int prot = PROT_READ, int flags = MAP_SHARED) { return { posix::mmap(nullptr, len, prot, flags, fd, off), len }; }

	void faccessat(const char *path, int amode, int flag = 0) const { posix::faccessat(fd, path, amode, flag); }
	void fchmodat(const char *path, mode_t mode, int flag = 0) const { posix::fchmodat(fd, path, mode, flag); }
	void fchownat(const char *path, uid_t owner, gid_t group, int flag = 0) const { posix::fchownat(fd, path, owner, group, flag); }
	void fstatat(const char * _restrict path, struct stat * _restrict buf, int flag = 0) const { posix::fstatat(fd, path, buf, flag); }
	void linkat(const char *path1, const char *path2, int flag = 0) const { posix::linkat(fd, path1, fd, path2, flag); }
	void mkdirat(const char *path, mode_t mode = 0777) const { posix::mkdirat(fd, path, mode); }
	void mkfifoat(const char *path, mode_t mode = 0666) const { posix::mkfifoat(fd, path, mode); }
	void mknodat(const char *path, mode_t mode, dev_t dev) const { posix::mknodat(fd, path, mode, dev); }
	FileDescriptor openat(const char *path, int oflag, mode_t mode = 0666) const { return FileDescriptor(posix::openat(fd, path, oflag, mode)); }
	size_t readlinkat(const char * _restrict path, char * _restrict buf, size_t bufsize) const { return posix::readlinkat(fd, path, buf, bufsize); }
	void renameat(const char *oldpath, const char *newpath) const { posix::renameat(fd, oldpath, fd, newpath); }
	void symlinkat(const char *path1, const char *path2) const { posix::symlinkat(path1, fd, path2); }
	void unlinkat(const char *path, int flag = 0) const { posix::unlinkat(fd, path, flag); }
	void utimensat(const char *path, const struct timespec times[2], int flag = 0) const { posix::utimensat(fd, path, times, flag); }

	void pread_fully(void *buf, size_t nbyte, off_t offset) const;
	void pwrite_fully(const void *buf, size_t nbyte, off_t offset);
	void readv_fully(struct iovec iov[], int iovcnt);
	void writev_fully(struct iovec iov[], int iovcnt);
	void preadv_fully(struct iovec iov[], int iovcnt, off_t offset) const;
	void pwritev_fully(struct iovec iov[], int iovcnt, off_t offset);

};


#if _POSIX_VERSION < 200809L
// polyfill file functions standardized in POSIX.1-2008
#ifndef AT_FDCWD
#	define AT_FDCWD (-100)
#endif
#ifndef AT_SYMLINK_NOFOLLOW
#	define AT_SYMLINK_NOFOLLOW 0x100
#endif
#ifndef AT_EACCESS
#	define AT_EACCESS 0x200
#endif
#ifndef AT_REMOVEDIR
#	define AT_REMOVEDIR 0x200
#endif
#ifndef AT_SYMLINK_FOLLOW
#	define AT_SYMLINK_FOLLOW 0x400
#endif
#ifndef UTIME_NOW
#	define UTIME_NOW ((1L << 30) - 1L)
#endif
#ifndef UTIME_OMIT
#	define UTIME_OMIT ((1L << 30) - 2L)
#endif
extern "C" {
	int faccessat(int fd, const char *path, int amode, int flag) _weak;
	int fchmodat(int fd, const char *path, mode_t mode, int flag) _weak;
	int fchownat(int fd, const char *path, uid_t owner, gid_t group, int flag) _weak;
	int fstatat(int fd, const char * _restrict path, struct stat * _restrict buf, int flag) _weak;
	int futimens(int fd, const struct timespec times[2]) _weak;
	int linkat(int fd1, const char *path1, int fd2, const char *path2, int flag) _weak;
	int mkdirat(int fd, const char *path, mode_t mode) _weak;
	int mkfifoat(int fd, const char *path, mode_t mode) _weak;
	int mknodat(int fd, const char *path, mode_t mode, dev_t dev) _weak;
	int openat(int fd, const char *path, int oflag, ...) _weak;
	ssize_t readlinkat(int fd, const char * _restrict path, char * _restrict buf, size_t bufsize) _weak;
	int renameat(int oldfd, const char *oldpath, int newfd, const char *newpath) _weak;
	int symlinkat(const char *path1, int fd, const char *path2) _weak;
	int unlinkat(int fd, const char *path, int flag) _weak;
	int utimensat(int fd, const char *path, const struct timespec times[2], int flag) _weak;
}
#endif // _POSIX_VERSION < 200809L

#if _POSIX_SYNCHRONIZED_IO <= 0
static int fdatasync(int fildes) _weakref("fsync");
#endif

#ifndef POSIX_FADV_NORMAL
#	define POSIX_FADV_NORMAL 0
#	define POSIX_FADV_RANDOM 1
#	define POSIX_FADV_SEQUENTIAL 2
#	define POSIX_FADV_WILLNEED 3
#	define POSIX_FADV_DONTNEED 4
#	define POSIX_FADV_NOREUSE 5
extern "C" int posix_fadvise(int fd, off_t offset, off_t len, int advice) _weak;
#endif // !defined(POSIX_FADV_NORMAL)

#ifdef __APPLE__
extern "C" {
	int posix_fallocate(int fd, off_t offset, off_t len) _weak;
	ssize_t preadv(int fd, const struct iovec iov[], int iovcnt, off_t offset) _weak;
	ssize_t pwritev(int fd, const struct iovec iov[], int iovcnt, off_t offset) _weak;
}
#endif // defined(__APPLE__)
