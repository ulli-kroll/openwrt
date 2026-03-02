#ifndef _QSEE_SFS_H_
#define _QSEE_SFS_H_

/**
@file qsee_sfs.h
@brief Secure File System with Encryption and Integrity Protection.
This file contains the definitions of the constants, data
structures, and interfaces that provide methods to
encrypt/integrity-protect and decrypt/verify content to the EFS.
*/

/*=============================================================================
              Copyright (c) 2000-2018 QUALCOMM Technologies Incorporated.
                      All Rights Reserved.
              Qualcomm Confidential and Proprietary

=============================================================================*/

/*=============================================================================

                       EDIT HISTORY FOR MODULE

  $Header: //components/rel/ssg.tzw/1.2/api/securemsm/trustzone/qsee/qsee_sfs.h#1 $
  $DateTime: 2020/11/02 23:12:26 $
  $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/23/14   mr      Added new SFS for file enumeration
03/12/14   yh      Add qsee_sfs_error()
02/12/12   chm     Added support for SFS remove directory.
10/06/11    cz     corrected the checking of theqsee_sfs_open return
08/06/10   sd      {TechPubs] Edited Doxygen markup and comments.
08/28/09   ejt     removed #include "customer.h" and FEATURE flags for CMI compliance
12/17/08   ejt     removedqsee_sfs_SYSTEM_INTERFACE from SFS files
04/04/07   df      adding MINIMAL_DUAL_PD featurization
03/03/07   df      new functionqsee_sfs_ixfile_rm()
03/01/07   df      support for seek
01/09/06   df      local support for GetInfo
12/22/06   df      support for usfs
12/08/06   df      added cpp support
07/25/05   df      changedqsee_sfs_init prototype
07/22/05   df      added FEATURE_SEC_SFS
=============================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
/** @cond */

typedef enum
{
  SFS_NO_ERROR = 0,        /**< No error. */
  SFS_ERROR_GENERIC = 1,   /**< Generic failure error code. */
  SFS_NO_MEMORY = 2,       /**< Malloc failed. */
  SFS_FS_ERROR = 3,        /**< REE file system operation failed. */
  SFS_NO_FILE = 4,         /**< No such file. */
  SFS_VERI_HMAC_ERROR = 5, /**< SFS segment sanity check failed, data is tampered. */
}sfs_errno_enum_type;

/* Maximum file name size allowed by Secure File System */
#define SFS_MAX_FILENAME_SIZE 64

/* File entry which makes up the file list returned by qsee_sfs_get_file_list() */
typedef struct
{
  char       file_name[SFS_MAX_FILENAME_SIZE + 1];  /* Name of file in app's file list */
  uint32_t   file_size;                             /* Size of the file with file_name is app's file list */
} sfs_file_entry;
/** @endcond */

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
  @addtogroup qtee_sfs
  @{
*/

/**
  Retrieves open SFS file size.

  @param[in]  fd    File descriptor.
  @param[out] size  Pointer to open file size.

  @return
  E_SUCCESS - File size stored in size parameter. \n
  E_FAILURE - Error occurred while getting file size.

*/
int qsee_sfs_getSize(int fd, uint32_t* size);


/**
  This is a deprecated function because SFS does not support folder hierarchy.
  Calls to this function result in a NOP.

  @param[in] path      Pointer to a fully qualified path.

  @return
  Always returns E_SUCCESS.

*/
int qsee_sfs_mkdir(const char* path);

/**
  This is a deprecated function because SFS does not support folder hierarchy.
  Calls to this function result in a NOP.

  @param[in] path      Pointer to a fully qualified path

  @return
  Always returns E_SUCCESS.

*/
int qsee_sfs_rmdir(const char* path);

/**
  Opens an SFS file. The options are specified by File mode in the flag
  parameter.

  @detdesc
  In the SFS, opening a file does not actually do anything in the file
  system. If the file (along with the associated file segments) already
  exists and the file is created with O_TRUNC mode, the associated
  subfiles are deleted. \n
  The first segment is created only when new bytes to be written begin
  arriving.

  @note1hang
  The base directory must exist; otherwise, returns NULL.

  @param[in] path      Pointer to a fully qualified path of filename to be
                       opened.
  @param[in] flags     Bitmask field used to specify file
                       modes:
                       - O_RDONLY - Open for read-only access.
                       - O_READWRITE - Open for read-write access.
                       - O_CREAT - Creates file if it does not
                         exist.
                       - O_TRUNC - Truncates file to zero size
                         after opening.
                       - O_APPEND - Write operations occur at
                         the end of the file.

  @return
  Non-zero -- A valid file descriptor. \n
  Zero - Error occurred while opening file.

*/
int qsee_sfs_open(const char* path, int flags);

/**
  Closes an open SFS file. Releases all resources used by the file.

  @param[in] fd File descriptor.

  @return
  E_SUCCESS - Closed file successfully. \n
  E_FAILURE - Error occurred while closing file.

*/
int qsee_sfs_close(int fd);

/**
  Reads bytes from an encrypted SFS file previously opened via
  call to qsee_sfs_open().

  @detdesc
  nbytes are read from the current file position, and the file position advances
  by the number of read bytes. SFS performs the necessary cipher and
  verification operations as bytes are read from the file.

  @param[in] fd     File descriptor.
  @param[in] buf    Pointer to buffer to hold read bytes.
  @param[in] nbytes Number of bytes to read from the file.

  @return
  Returns the number of bytes read from an SFS file. \n
  Returns -1 for the error case.

*/
int qsee_sfs_read(int fd, char *buf, int nbytes);

/**
  Writes bytes to a previously opened encrypted SFS file via call to
  qsee_sfs_open.

  @detdesc
  nbytes are written to the current file position. \n
  If O_APPEND flag was set in qsee_sfs_open(), bytes are written at the end of
  the file unless qsee_sfs_seek() was issued before qsee_sfs_write(). In this
  case, data is written to the file position that qsee_sfs_seek() set. The file
  position advances by the number of bytes written.

  @param[in] fd     File descriptor.
  @param[in] buf    Pointer to buffer to be written.
  @param[in] nbytes Number of bytes to write to the file.

  @return
  Returns the number of bytes written to an SFS file. \n
  Returns -1 for the error case.

*/
int qsee_sfs_write(int fd, const char *buf, int nbytes);

/**
  Removes an SFS file that was previously created through qsee_sfs_open().

  @param[in] path Pointer to a fully qualified path of the file to be deleted.

  @return
  E_SUCCESS - File removal successful. \n
  E_FAILURE - Error occurred while removing the file.

*/
int qsee_sfs_rm(const char *path);

/**
  Moves the read location to point to n bytes from the start, current,
  or end of the file.

  @detdesc
  Opens, verifies, and decrypts the subfile that corresponds to the current position. The segment
  position advances to the current position.

  @param[in] fd     File descriptor.
  @param[in] offset File offset to seek in bytes.
  @param[in] whence Indicates start, end, or current position.
                    - SEEK_SET - Start of file.
                    - SEEK_CUR - Current file position.
                    - SEEK_END - End of file.

  @return
  If successful, the current location. \n
  If an error occurred, -1.

*/
int qsee_sfs_seek(int fd, int32_t offset, int whence);

/**
  Checks error indicator.

  @detdesc
  Checks if the error indicator associated with fd is set; returns a value different
  from zero if it is.

  @note1hang
  The indicator is set by a previous operation on the SFS fd that failed.

  @param[in] fd     File descriptor. When qsee_sfs_open() returns zero, it
                    will try to return the failure reason of the last open file.


  @return
  Returns a non-zero value if the error indicator associated with fd is set. \n
  Otherwise, returns zero.

*/
int qsee_sfs_error(int fd);

/**
  Returns a list of files created by the calling application.

  @detdesc
  Returns an array of sfs_file_entry structs through the output parameter
  file_list, which contains the file name and file size for each file
  created by the calling applicaiton through qsee_sfs_open using the O_CREAT
  flag. The returned file_list size is returned through the output parameter
  file_list_len.
  @par
  If file_list_len > 0, the caller must call qsee_sfs_clean_file_list to
  recollect resources allocated by this function.
  @par
  If file_list_len == 0, there are currently no files belonging to the calling
  application and the caller should not call qsee_sfs_clean_file_list because the list
  will be set to NULL. \n
  After editing the files, it is recommended to obtain a new file list by
  calling qsee_sfs_clean_file_list on the current file list and calling this
  function once more.

  @param[out] file_list      Double pointer to sfs_file_entry list represents files created
                             by the calling application.
  @param[out] file_list_len  Pointer to file list length.

  @return
  0 -- Created the application file list successfully. \n
  Non-zero -- An error occured while creating the application file list. Use
             qsee_sfs_error() to get a more detailed error code.

*/
int qsee_sfs_get_file_list(sfs_file_entry** file_list, uint32_t* file_list_len);

/**
  Reclaims allocated resources to create the application's file list.

  @detdesc
  The memory resources allocated in qsee_sfs_get_file_list()
  are reclaimed and can be used for future memory allocation.

  @param[in] file_list      Pointer to file list returned by a successful call to
                            qsee_sfs_get_file_list().

*/
void qsee_sfs_clean_file_list(sfs_file_entry* file_list);

/**
  Reports if SFS supports hardware-backed rollback protection.

  @param[out] b             Pointer to boolean rollback protection supported status.

  @return
  E_SUCCESS -- On success. \n
  E_FAILURE -- On failure.

*/
int qsee_sfs_is_anti_rollback_enabled(bool* b);

/** @} */   /* end_addtogroup qtee_sfs*/

#ifdef __cplusplus
}
#endif

#endif //_qsee_sfs_API_H_
