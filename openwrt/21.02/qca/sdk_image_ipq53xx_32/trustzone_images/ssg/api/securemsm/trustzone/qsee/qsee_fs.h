#ifndef QSEE_FS_H
#define QSEE_FS_H
/**
@file qsee_fs.h
@brief Provide posix file operations
*/

/*===========================================================================
  Copyright (c) 2008 - 2015, 2017 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

#include <stddef.h>
#include <stdint.h>

/** @cond */
/* copied from linuxbuild/libc/kernel/common/asm-generic/errno-base.h */

#define EPERM 1
#define ENOENT 2
#define ESRCH 3
#define EINTR 4
#define EIO 5
#define ENXIO 6
#define E2BIG 7
#define ENOEXEC 8
#define EBADF 9
#define ECHILD 10
#define EAGAIN 11
#define ENOMEM 12
#define EACCES 13
#define EFAULT 14
#define ENOTBLK 15
#define EBUSY 16
#define EEXIST 17
#define EXDEV 18
#define ENODEV 19
#define ENOTDIR 20
#define EISDIR 21
#define EINVAL 22
#define ENFILE 23
#define EMFILE 24
#define ENOTTY 25
#define ETXTBSY 26
#define EFBIG 27
#define ENOSPC 28
#define ESPIPE 29
#define EROFS 30
#define EMLINK 31
#define EPIPE 32
#define EDOM 33
#define ERANGE 34

/* copied from linuxbuild\bionic\libc\kernel\common\linux\linux.h */

#define S_IFMT 00170000
#define S_IFSOCK 0140000
#define S_IFLNK 0120000
#define S_IFREG 0100000
#define S_IFBLK 0060000
#define S_IFDIR 0040000
#define S_IFCHR 0020000
#define S_IFIFO 0010000
#define S_ISUID 0004000
#define S_ISGID 0002000
#define S_ISVTX 0001000

#define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m) (((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m) (((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#define S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001

/* copied from linuxbuild\bionic\libc\kernel\common\asm-generic\fcntl.h */

#define O_ACCMODE 00000003
#define O_RDONLY 00000000
#define O_WRONLY 00000001
#define O_RDWR 00000002
#ifndef O_CREAT
#define O_CREAT 00000100
#endif
#ifndef O_EXCL
#define O_EXCL 00000200
#endif
#ifndef O_NOCTTY
#define O_NOCTTY 00000400
#endif
#ifndef O_TRUNC
#define O_TRUNC 00001000
#endif
#ifndef O_APPEND
#define O_APPEND 00002000
#endif
#ifndef O_NONBLOCK
#define O_NONBLOCK 00004000
#endif
#ifndef O_SYNC
#define O_SYNC 00010000
#endif
#ifndef FASYNC
#define FASYNC 00020000
#endif
#ifndef O_DIRECT
#define O_DIRECT 00040000
#endif
#ifndef O_LARGEFILE
#define O_LARGEFILE 00100000
#endif
#ifndef O_DIRECTORY
#define O_DIRECTORY 00200000
#endif
#ifndef O_NOFOLLOW
#define O_NOFOLLOW 00400000
#endif
#ifndef O_NOATIME
#define O_NOATIME 01000000
#endif
#ifndef O_NDELAY
#define O_NDELAY O_NONBLOCK
#endif

#ifndef F_GETFL
/* get file status flags */
#define F_GETFL 3
#endif

#ifndef AT_FDCWD
/* openat() relative to current working directory */
#define AT_FDCWD  -100
#endif

/* Values for the WHENCE argument to lseek.
   copied from linuxbuild\bionic\libc\include\unistd.h
*/
# define SEEK_SET 0 /* Seek from beginning of file.  */
# define SEEK_CUR 1 /* Seek from current position.  */
# define SEEK_END 2 /* Seek from end of file.  */

/*
  Note: The kernel zero's the padded region because glibc might read them
  in the hope that the kernel has stretched to using larger sizes.
  copied from linuxbuild\bionic\libc\include\sys\stat.h
*/
#ifndef fs_stat
typedef struct _fs_stat {
    uint64_t             st_dev;          /* ID of device containing file */
    unsigned char       __pad0[4];
    uint32_t             __st_ino;
    unsigned int        st_mode;         /* protection */
    unsigned int        st_nlink;        /* number of hard links */
    uint32_t             st_uid;          /* user ID of owner */
    uint32_t             st_gid;          /* group ID of owner */
    uint64_t             st_rdev;         /* device ID (if special file) */
    unsigned char       __pad3[4];
    int64_t              st_size;         /* total size, in bytes */
    uint32_t             st_blksize;      /* blocksize for filesystem I/O */
    uint64_t             st_blocks;       /* number of blocks allocated */
    uint32_t             st_atime;        /* time of last access */
    uint32_t             st_atime_nsec;
    uint32_t             st_mtime;        /* time of last modification */
    uint32_t             st_mtime_nsec;
    uint32_t             st_ctime;        /* time of last status change */
    uint32_t             st_ctime_nsec;
    uint64_t             st_ino;          /* inode number */
}__attribute__ ((packed)) fs_stat;

#endif

#ifndef _DEFINED_DIR
#define _DEFINED_DIR
 typedef void DIR;
#endif

typedef struct tzDirent
{
  uint64_t d_ino;
  int64_t d_off;
  unsigned short d_reclen;
  unsigned char d_type;
  char d_name[256];
} __attribute__ ((packed)) tzDirent_t;


/* QSEE file operation interface prototypes and their system equivalent alias.
 * The system equivalent prototypes are derived or partially derived from
 * various system headers including:
 * linuxbuild\bionic\libc\include\fcntl.h
 * linuxbuild\bionic\libc\include\unistd.h
 * linuxbuild\bionic\libc\include\sys\stat.h
 * linuxbuild\bionic\libc\include\dirent.h
 */

/*
 * Protos copied or derived from linuxbuild\bionic\libc\include\fcntl.h
 */

/** @endcond */

/**
  @addtogroup qtee_fs
  @{
 */

/**
  Opens the specified file with the given mode.

  @param[in] path    Pointer to file path.
  @param[in] flags   File status flags.
  @param[in] mode    [Optional] File access mode.

  @note1
  Values for flags and mode are described in http://man7.org/linux/man-pages/man2/open.2.html

  @return
  SUCCESS -- Valid file descriptor \n
  FAILURE -- -1
 */
int open(const char* path, int flags, ...);

/**
  Opens a relative file.

  @param[in] fd      Directory file descriptor relative to which the file path is determined.
  @param[in] path    Pointer to file path, relative to directory described by fd.
  @param[in] flags   Access mode to open file.
  @param[in] mode    [Optional] File access mode.

  @note1
  Values for flags and mode are described in http://man7.org/linux/man-pages/man2/open.2.html

  @return
  SUCCESS -- Valid file descriptor \n
  FAILURE -- -1
 */
int openat(int fd, const char* path, int flags, ...);

/**
  Removes a relative directory.

  @param[in] dirfd     Directory file descriptor relative to which the file path is determined.
  @param[in] pathname  Pointer to the file path, relative to the directory described by fd.
  @param[in] flags     Bit mask flag.

  @note1
  Values for flags are described in http://man7.org/linux/man-pages/man2/unlinkat.2.html

  @return
  SUCCESS -- 0  \n
  FAILURE -- -1
 */
int unlinkat(int dirfd, const char *pathname, int flags);

/**
  Performs the command operation on file.

  @param[in] fd        File descriptor.
  @param[in] command   Operation to be performed.
  @param[in] params    [Optional] A variable number of optional parameters that are dependent
                                  on the command.

  @note1
  More information on optional arguments for fcntl() can be found at http://man7.org/linux/man-pages/man2/fcntl.2.html

  @return
  SUCCESS -- Return value depends on the operation \n
  FAILURE -- -1
 */
int fcntl(int fd, int command, ...);

/**
  Creates a file equivalent to open(path, O_WRONLY | O_CREAT | O_TRUNC, mode).

  @param[in] path    Pointer to file path.
  @param[in] mode    Mode to open a new file.

  @note1
  Values for this mode are described in http://man7.org/linux/man-pages/man2/creat.2.html

  @return
  SUCCESS -- 0 \n
  FAILURE -- -1
 */
int creat(const char* path, uint32_t mode);

/*
 * Protos copied or derived from linuxbuild\bionic\libc\include\unistd.h
 */

/**
  Reads a number of bytes from a given file.

  @param[in]  fd     File descriptor.
  @param[out] buf    Pointer to buffer to be read into.
  @param[in]  count  Number of bytes to read.

  @note1
  Further details are available at http://man7.org/linux/man-pages/man2/read.2.html

  @return
  SUCCESS -- Number of bytes read \n
  FAILURE -- -1
 */
int32_t read(int fd, void *buf, uint32_t count);

/**
  Writes a number of bytes to a given file.

  @param[in]  fd     File descriptor.
  @param[in]  buf    Pointer to buffer to write from.
  @param[in]  count  Number of bytes to write.

  @note1
  Further details are available at http://man7.org/linux/man-pages/man2/write.2.html

  @return
  SUCCESS -- Number of bytes written \n
  FAILURE -- -1
 */
int32_t write(int fd, const void *buf, uint32_t count);

/**
  Closes an open file.

  @param[in] fd      File descriptor.

  @note1
  Additional details are available at http://man7.org/linux/man-pages/man2/close.2.html

  @return
  SUCCESS -- 0 \n
  FAILURE -- -1
*/
int close(int fd);

/**
  Repositions a file offset.

  @param[in] fd      File descriptor.
  @param[in] offset  New offset.
  @param[in] whence  Offset reference which can be one of:
                     - SEEK_SET: Sets offset relative to beginning of file.
                     - SEEK_CUR: Sets offset relative to current offset.
                     - SEEK_END: Sets offset relative to end of file.

  @note1
  Further details are available at http://man7.org/linux/man-pages/man2/lseek.2.html

  @return
  SUCCESS -- Resulting offset as measured from the beginning of the file \n
  FAILURE -- -1
 */
int32_t lseek(int fd, int32_t offset, int whence);

/**
  Unlink a linked file.

  @param[in] path    Pointer to file pathname.

  @note1
  Further details are available at http://man7.org/linux/man-pages/man2/unlink.2.html

  @return
  SUCCESS -- 0  \n
  FAILURE -- -1
 */
int unlink(const char *path);

/**
  Removes a directory.

  @param[in] path    Pointer to pathname of directory.

  @note1
  Further details are available at http://man7.org/linux/man-pages/man2/rmdir.2.html

  @return
  SUCCESS -- 0  \n
  FAILURE -- -1
 */
int rmdir(const char *path);

/**
  Synchronizes a file with storage.

  @param[in] fd      File descriptor.

  @note1
  Further details are available at http://man7.org/linux/man-pages/man2/fsync.2.html

  @return
  SUCCESS -- 0 \n
  FAILURE -- -1

  @caution1hang
  fsync() is not supported in the QTEE off-target environment.
 */
int fsync(int fd);


/*
Protos copied or derived from linuxbuild\bionic\libc\include\sys\stat.h
*/

/**
  - Gets file status.
  - When file is a link, fstat() traverses the link and returns
  information on the linked file.
  - lstat() only returns information on the link itself.

  @param[in]  fd     File descriptor.
  @param[out] buf    Pointer to file status structure.

  @note1
  Further details are available at http://man7.org/linux/man-pages/man2/fstat.2.html

  @return
  SUCCESS -- 0 \n
  FAILURE -- -1
 */
int fstat(int fd, fs_stat *buf);

/**
  - Gets file status.
  - When the file is a link, lstat() returns information on the link,
  unlike fstat(), which traverses the link and returns information on the linked file.

  @param[in]  path    Pointer to file pathname.
  @param[out] buf     Pointer to status buffer.

  @note1
  Further details are available at http://man7.org/linux/man-pages/man2/lstat.2.html

  @return
  SUCCESS -- 0 \n
  FAILURE -- -1
 */
int lstat(const char *path, fs_stat *buf);

/**
  Creates a directory tree.

  @param[in] path      Pointer to directory pathname.
  @param[in] mode      Permissions mode.

  @note1
  Further details are available at http://man7.org/linux/man-pages/man2/mkdir.2.html

  @return
  SUCCESS -- 0 \n
  FAILURE -- -1
*/
int mkdir (const char *path, uint32_t mode);


/*
Protos copied or derived from linuxbuild\bionic\libc\include\dirent.h
*/

/**
  Opens a directory.

  @param[in] path    Pointer to the directory path to open.

  @note1
  Further details are available at http://man7.org/linux/man-pages/man3/opendir.3.html

  @return
  SUCCESS -- Pointer to directory stream \n
  FAILURE -- NULL
 */
DIR* opendir(const char *path);

/**
  Reads a directory.

  @param[in]  pdir     Pointer to directory stream.
  @param[out] entry    Pointer to directory entry struct to be populated.

  @note1
  Further details are available at http://man7.org/linux/man-pages/man3/readdir.3.html

  @return
  SUCCESS -- Pointer to entry \n
  FAILURE -- NULL
 */
struct tzDirent* readdir(DIR* pdir, struct tzDirent* entry);


/**
  Close a directory.

  @param[in]  pdir     Pointer to directory stream.

  @note1
  Further details are available at http://man7.org/linux/man-pages/man3/closedir.3.html

  @return
  SUCCESS -- 0 \n
  FAILURE -- -1
 */
int closedir(DIR* pdir);

/*
Protos copied or derived from linuxbuild\bionic\libc\include\stdio.h
*/

/**
  Removes a file or directory.

  @param[in] path      Pointer to pathname of file/directory.

  @note1
  Further details are available at http://man7.org/linux/man-pages/man3/remove.3.html

  @return
  SUCCESS -- 0 \n
  FAILURE -- -1
 */
int remove(const char *path);

/*
Custom prototypes
*/

/**
  Gets current directory location.

  @param[in] path      Pointer to directory pathname.

  @note1
  Further details are available at http://man7.org/linux/man-pages/man3/telldir.3.html

  @return
  SUCCESS -- Current location of directory stream \n
  FAILURE -- -1

  @caution1hang
  telldir() is not supported by the QTEE off-target environment.
 */
int32_t telldir(const char *path);

/**
  Tests if a directory exists.

  @param[in] path      Pointer to directory pathname.

  @return
  SUCCESS -- 0 if directory is found \n
  FAILURE -- -1 if directory is not found
 */
int testdir(const char *path);

/**
  Renames a file.

  @param[in] oldfilename   Pointer to the file name from which it is to be renamed.
  @param[in] newfilename   Pointer to the file name to which it is to be renamed.

  @return
  SUCCESS -- 0 \n
  FAILURE -- -1
 */
int frename(const char *oldfilename, const char *newfilename);

/**
  chown and chmod a file or directory tree.

  @param[in] path      Pointer to the parent directory to chmod and chown, e.g., "/persist/playready/drmms/".
  @param[in] path_len  Path string length.
  @param[in] word      Pointer to the start directory chown and chmod from.\n Must be included in path, e.g., "playready" .
  @param[in] word_len  Start directory string length.
  @param[in] owner     Pointer to owner string, e.g., "media.system".
  @param[in] owner_len Owner string length.
  @param[in] mod       Pointer to permission mode string, e.g., "700" or "777".
  @param[in] mod_len   Permission mode string length.
  @param[in] level:    Wildcarding level to apply to chown and chmod commands [0,1,2].

  @note1hang A level value greater than 0 causes an attempt to chown and chmod with the '*' wildcard level times, e.g.: \n
  - level = 0 :
  - chown media.system /persist/playready
  - chmod 700 /persist/playready
  \n
  - level = 1 :
    - chown media.system /persist/playready
    - chmod 700 /persist/playready
    - chown media.system /persist/playready/ *
    - chmod 700 /persist/playready/ *
  \n
  - level = 2 :
  - chown media.system /persist/playready
  - chmod 700 /persist/playready
  - chown media.system /persist/playready/ *
  - chmod 700 /persist/playready/ *
  - chown media.system /persist/playready/ * / *
  - chmod 700 /persist/playready/ * / *
  \n

@caution1hang file_dir_chown_chmod() is not supported by the QTEE off-target environment.

  @return
  SUCCESS -- 0 \n
  FAILURE -- -1
 */
int file_dir_chown_chmod(char *path, uint32_t path_len,
                         char *word, uint32_t word_len,
                         char *owner, uint32_t owner_len,
                         char *mod, uint32_t mod_len, uint32_t level);

/**
  Gets the last REE errno.

  @return
  The last errno value.
 */
int get_error_number(void);

/**
  Gets the free size of a specified partition.

  @param[in] path    Pointer to a file pathname.
  @param[out] size   Pointer to remaining bytes in partition.

  @return
  SUCCESS -- 0 \n
  FAILURE -- -1
 */
int file_get_partition_free_size(const char *path, uint64_t *size);

/** @} */

#endif /* QSEE_FS_H */
