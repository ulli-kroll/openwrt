/*=======================================================================
 * FILE:        bootconfig_tool.c 
 *
 * SERVICES:    Creates BOOTCONFIG and store in a file.
 *
 * DESCRIPTION
 *   This file has the main functions used for creating BOOTCONFIG
 *   file.
 *
 * Copyright (c) 2015-2019, 2021 Qualcomm Technologies Inc.
 * All Rights Reserved.
 *=======================================================================*/

/*===================================================================
*
*                       EDIT HISTORY FOR FILE
*
*   This section contains comments describing changes made to the
*   module. Notice that changes are listed in reverse chronological
*   order.
*
*   $Header: //components/rel/boot.xf/0.3.1.1/QcomPkg/IPQ9574Pkg/Tools/bootconfig_tool.c#1 $ $DateTime: 2022/02/24 22:30:26 $ $Author: pwbldsvc $
*
* when         who     what, where, why
* ----------   ---     ----------------------------------------------
* 03/15/19     smadhe  Initial version for failsafe boot support
                       brought in from Hawkeye
* 01/30/17     an      Initial version for failsafe boot support
*		       brought in from Dakota
* 05/13/16     gd      Initial version for failsafe boot support
*==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "mibib.h"
#include "flash.h"

/* Calculate no. of pages occupied by a table */
#define PAGE_COUNT(tbl_size, page_size) (((tbl_size) + (page_size) - 1) / (page_size))
/* Number of bytes required to pad the remaining pages in the nor flash */
#define PAGE_PADDING(tbl_size, page_size) \
	((page_size) - ((tbl_size) & (page_size-1)))


/* No. of bootconfig copies */
#define BOOTCONFIG_MAX_COPIES   1

static   uint32 pages_per_block;             /* Number of pages in a block */
static   uint32 page_size_bytes;             /* Logical page size */

/*
 * Display bootconfig_tool usage information.
 */
static void print_usage()
{
   printf("Usage: bootconfig_tool.exe [-s page_size -p num_of_pages_per_block] "
          "-i bootconfig_fname -o bootconfig_crc_fname\n");
}


/*
 * Display error string based on the error number before exit.
 */
static void error(int status, int myerrno, char *fmt, ...)
{
   va_list ap;

   fprintf(stderr, "bootconfig_tool: ");

   va_start(ap, fmt);
   vfprintf(stderr, fmt, ap);
   va_end(ap);

   if (myerrno != 0)
      fprintf(stderr, ": %s", strerror(myerrno));

   fprintf(stderr, "\n");

   if (status)
      exit(status);
}


/*
 * Reads user partition table bin file and updates the user partition
 * table structure.
 */
static void read_bootconfig_partition (const char *usrbin, struct flash_dual_boot_info * usr_parti)
{
   FILE *fd;

   fd = fopen(usrbin, "rb");

   if (fd == NULL)
      error(1, errno, "open usr partition table file failed");

   if (fread(usr_parti, sizeof(struct flash_dual_boot_info), 1, fd) != 1)
   {
      fclose(fd);
      error(1, errno, "reading user partition table file failed");
   }

   fclose(fd);
}

/*
 * Updates one table into the system partition table file, padding it
 * to the page size. Returns the no. of pages written.
 */
static unsigned int update_sys_bin_table(FILE *fd, void *tbl, size_t tbl_size,
                                         unsigned int pgsize, char *tbl_name)
{
   int pad_size;
   unsigned char *pad_buff = NULL;

   if (1 != fwrite(tbl, tbl_size, 1, fd))
      error(1, errno, "error writing %s info", tbl_name);

   pad_buff = malloc(sizeof(char) * pgsize);
   if (NULL == pad_buff)
      error(1, errno, "Memory allocation failed");

   pad_size = PAGE_PADDING(tbl_size, pgsize);
   memset(pad_buff, 0xFF, pad_size);

   if (1 != fwrite(pad_buff, pad_size, 1, fd))
   {
      free(pad_buff);
      error(1, errno, "error writing pad mibib info");
   }

   free(pad_buff);

   return PAGE_COUNT(tbl_size, pgsize);
}

/*
 * MIBIB tables only occupy 4 - 6 pages, depending on the page
 * size. Adds extra padding pages to the MIBIB block to round it off
 * to the block size.
 */
static void update_sys_bin_block_pad(FILE *fd, unsigned int nmibib_pages)
{
   int i;
   int pad_size;
   unsigned char *pad_buff = NULL;

   pad_buff = malloc(sizeof(char) * page_size_bytes);
   if (NULL == pad_buff)
      error(1, errno, "Memory allocation failed");

   memset(pad_buff, 0xFF, page_size_bytes);

   for (i = nmibib_pages; i < pages_per_block; i++)
   {
      if (1 != fwrite(pad_buff, page_size_bytes, 1, fd))
      {
         free(pad_buff);
         error(1, 0, "error writing block padding");
      }
   }

   free(pad_buff);
}

/*
 * Updates the partition table file with mibib, system partition table, user
 * partition table and CRC data structures.
 */
static void update_output_bin(FILE *fd, struct flash_dual_boot_info * dual_boot_info)
{
   unsigned int nmibib_pages = 0;

   nmibib_pages += update_sys_bin_table(fd, dual_boot_info, sizeof(struct flash_dual_boot_info),
                                        page_size_bytes, "dualboot config info");

   update_sys_bin_block_pad(fd, nmibib_pages);
}

/*
 * Console application main function to generate the mibib partition
 * table without flash
 */
int main(int argc, char *argv[])
{
   int option = 0;
   struct flash_dual_boot_info dual_boot_info_tbl;
   unsigned int crc;
   uint32 count;
   FILE *sysfp;
   char *inputbin = NULL;
   char *outbin = NULL;

   /* Number of pages in a block */
   pages_per_block = 256;
   /* Logical page size */
   page_size_bytes = 256;

   while ((option = getopt(argc, argv, "s:p:b:i:o:")) != -1)
   {
      switch (option)
      {
         case 's':
            page_size_bytes = atoi(optarg);
            break;
         case 'p':
            pages_per_block = atoi(optarg);
            break;
         case 'i':
            inputbin = optarg;
            break;
         case 'o':
            outbin = optarg;
            break;
         default:
            print_usage();
            return 1;
      }
   }

   if ((NULL == inputbin) || (NULL == outbin))
   {
     print_usage();
     return 1;
   }

   memset(&dual_boot_info_tbl, 0xFF, sizeof (struct flash_dual_boot_info));
   /* Retrieve the user partition table information from the user partition
    * file.
    */
   read_bootconfig_partition(inputbin, &dual_boot_info_tbl);

   dual_boot_info_tbl.end_magic = DUAL_BOOTINFO_END_MAGIC_V2;

   sysfp = fopen(outbin, "wb");
   if (sysfp == NULL)
      error(1, errno, "output bootconfig file open failed");

   for (count = 0; count < BOOTCONFIG_MAX_COPIES; count++, dual_boot_info_tbl.age--)
   {
      /* Update the system table binary file */
      update_output_bin(sysfp, &dual_boot_info_tbl);
   }

   fclose(sysfp);

   return 0;
}
