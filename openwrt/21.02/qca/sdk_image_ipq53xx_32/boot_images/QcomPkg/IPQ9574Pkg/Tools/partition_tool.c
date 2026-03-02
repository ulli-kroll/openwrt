/*=======================================================================
 * FILE:        parition_tool.c
 *
 * SERVICES:    Creates flash partition table and stores in a file
 *
 * DESCRIPTION
 *   This file has the main functions used for creating flash
 *   partition table
 *
 * Copyright (c) 2012, 2015-2019, 2021 Qualcomm Technologies, Inc.
 * All Rights Reserved
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *=======================================================================*/

/*===================================================================
*
*                       EDIT HISTORY FOR FILE
*
*   This section contains comments describing changes made to the
*   module. Notice that changes are listed in reverse chronological
*   order.
*
*   $Header: //components/rel/boot.xf/0.3.1.1/QcomPkg/IPQ9574Pkg/Tools/partition_tool.c#1 $ $DateTime: 2022/02/24 22:30:26 $ $Author: pwbldsvc $
*
* when         who     what, where, why
* ----------   ---     ----------------------------------------------
* 03/15/19     smadhe  Version brought in from Hawkeye
* 05/25/18     gs      Support NAND grow partition in NORPLUSNAND
* 04/06/17     tv      Hawkeye header changes check-in.
* 03/30/17     an      Update the system partition table version to user partition table version
*		       instead of hardcoding to SYS_PARTI_VERSION. 
* 10/27/15     nm      Removed backup MIBIB for NOR
* 04/10/15     gs      Unified CRC checksum calcualtion for NAND and NOR.
* 01/27/15     an      nor_tool.c copied from Akronite and modified
*                      CRC checksum calculation.
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
#include "miparti.h"
#include "mibib.h"
#include "micrc.h"
#include "crc.h"

#include "flash.h"
#include "flash_dal.h"
#include "flash_nor_partition_tbl.h"

/* Maximum file name size */
#define NAME_MAX 		255
/* System partition table magic number 1 */
#define SYS_PARTI_MAGIC1 	0x55EE73AA;
/* System partition table magic number 2 */
#define SYS_PARTI_MAGIC2 	0xE35EBDDB;
/* System partion table version number */
#define SYS_PARTI_VERSION 	0x4;
/* No. of MIBIB copies */
#define MIBIB_MAX_COPIES 	2
/* Calculate no. of pages occupied by a table */
#define PAGE_COUNT(tbl_size, page_size) (((tbl_size) + (page_size) - 1) / (page_size))
/* Number of bytes required to pad the remaining pages in the flash */
#define PAGE_PADDING(tbl_size, page_size) \
	((page_size) - ((tbl_size) & (page_size-1)))

#define SECONDARY_FLASH		0x1

#define GROW_PARTITION		0xFFFFFFFF

#define qmemcpy memcpy
static struct nor_dev_data partition_data;
static struct nor_dev_data secondary_partition_data;

/*
 * Display partition_tool usage information.
 */
static void print_usage()
{
   printf("Usage: partition_tool.exe [-s page_size -p num_of_pages_per_block "
          "-b block_count -c mibib_copies] -u usr_tbl_fname -o sys_tbl_fname\n");
}


/*
 * Display error string based on the error number before exit.
 */
static void error(int status, int myerrno, char *fmt, ...)
{
   va_list ap;

   fprintf(stderr, "partition_tool: ");

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
 * Updates the mibib information.
 */
static void update_mibib_info(mi_boot_info_t mi_boot_info, uint32 age)
{
   mi_boot_info->magic1 = MIBIB_MAGIC1;
   mi_boot_info->magic2 = MIBIB_MAGIC2;
   mi_boot_info->version = MIBIB_VERSION;
   mi_boot_info->age = age;
}


/*
 * Reads user partition table bin file and updates the user partition
 * table structure.
 */
static void read_usr_partition (const char *usrbin, flash_usr_partable_t usr_parti)
{
   FILE *fd;
   int ret = 0;

   fd = fopen(usrbin, "rb");

   if (fd == NULL)
      error(1, errno, "open usr partition table file failed");

   if (fread(usr_parti, sizeof(struct flash_usr_partition_table), 1, fd) != 1)
   {
      fclose(fd);
      error(1, errno, "reading user partition table file failed");
   }

   fclose(fd);
}

/*
 * Parses user partition table structure and updates the system partition
 * table structure.
 */
static int convert_usr_parti_tbl_to_sys_parti_tbl(
             flash_usr_partable_t parti1, flash_partable_t parti2)
{
   unsigned int i, user_parti_size = 0;
   unsigned int user_parti_pad;
   unsigned int sys_parti_start = 0;
   unsigned int sys_parti_size = 0;
   unsigned int secondary_sys_parti_start = 0;
   unsigned int secondary_sys_parti_size = 0;
   unsigned int block_size = 0;
   unsigned int secondary_block_size = 0;
   unsigned char *parti_buffer = (unsigned char *) parti2;

   /* Check device parameters information is non-zero. */
   if ((partition_data.pages_per_block == 0) || (partition_data.block_count == 0))
   {
      error(0, 0, "Invalid device geometry infomation");
      return FLASH_DEVICE_FAIL;
   }

   block_size = (partition_data.pages_per_block * partition_data.page_size_bytes)  / 1024;
   secondary_block_size = (secondary_partition_data.pages_per_block * secondary_partition_data.page_size_bytes)  / 1024;

   /* Start partition buffer with all 0xFFs. */
   memset(parti_buffer, 0x0, sizeof(struct flash_partition_table));

   parti2->magic1 = SYS_PARTI_MAGIC1;
   parti2->magic2 = SYS_PARTI_MAGIC2;
   parti2->version = parti1->version;

   /* Set number of partition parts even if not used. */
   parti2->numparts = parti1->numparts;

   /* Use number of partitions from user partition table*/
   for (i = 0; i < parti1->numparts; i++)
   {
      /* check for 0:All partitions */
      if (strncmp(parti1->part_entry[i].name, FLASH_DEVICE_ALL_PARTI_NAME,
          sizeof(FLASH_DEVICE_ALL_PARTI_NAME)) == 0)
      {
         error(0, 0, "Reserved partition name found in user partition table!");
         return FLASH_DEVICE_FAIL;
      }
      else
      {
        (void) qmemcpy(parti2->part_entry[i].name, parti1->part_entry[i].name,
                    FLASH_NOR_PARTI_NAME_LENGTH);

         parti2->part_entry[i].attrib1 =
         parti1->part_entry[i].reserved_flag1;
         parti2->part_entry[i].attrib2 =
         parti1->part_entry[i].reserved_flag2;
         parti2->part_entry[i].attrib3 =
         parti1->part_entry[i].reserved_flag3;
         parti2->part_entry[i].which_flash =
         parti1->part_entry[i].which_flash;
     }

      user_parti_size = parti1->part_entry[i].img_size;
      user_parti_pad = parti1->part_entry[i].padding;

      if (user_parti_size == 0)
      {
         printf("Partition %s is defined with zero size\n",
         parti1->part_entry[i].name);
         return FLASH_DEVICE_FAIL;
      }

      if (user_parti_size == GROW_PARTITION)
      {
         if (i != (parti1->numparts - 1))
         {
            error(0, 0, "Non-terminal partition defined with GROW");
            return FLASH_DEVICE_FAIL;
         }

         if (parti1->part_entry[i].which_flash == SECONDARY_FLASH)
         {
            parti2->part_entry[i].offset = secondary_sys_parti_start;
            parti2->part_entry[i].length = GROW_PARTITION;
	 }
         else
         {
            parti2->part_entry[i].offset = sys_parti_start;
            parti2->part_entry[i].length = GROW_PARTITION;
	 }

         return FLASH_DEVICE_DONE;
      }

      sys_parti_size = 0;
      secondary_sys_parti_size = 0;

      /*
       * System partition table requires the partition size in
       * blocks. If user partition table entry flag 4 is 0xFE,
       * partition size is specified in blocks, else it is specified
       * in KB. Apply the required conversion.
       */
      if (parti1->part_entry[i].which_flash == SECONDARY_FLASH)
      {
         if (parti1->part_entry[i].reserved_flag4 == 0xFE)
            secondary_sys_parti_size = user_parti_size + user_parti_pad;
         else
            secondary_sys_parti_size = (user_parti_size + user_parti_pad + secondary_block_size - 1) / secondary_block_size;

         if ((secondary_sys_parti_start + secondary_sys_parti_size) > (secondary_partition_data.block_count * secondary_block_size))
         {
            error(0, 0, "system partition size exceeds the total flash size");
            return FLASH_DEVICE_FAIL;
         }

         parti2->part_entry[i].offset = secondary_sys_parti_start;
         parti2->part_entry[i].length = secondary_sys_parti_size;
         secondary_sys_parti_start += secondary_sys_parti_size;
      }
      else
      {
         if (parti1->part_entry[i].reserved_flag4 == 0xFE)
            sys_parti_size = user_parti_size + user_parti_pad;
         else
            sys_parti_size = (user_parti_size + user_parti_pad + block_size - 1) / block_size;

         if ((sys_parti_start + sys_parti_size) > (partition_data.block_count * block_size))
         {
            error(0, 0, "system partition size exceeds the total flash size");
            return FLASH_DEVICE_FAIL;
         }

         parti2->part_entry[i].offset = sys_parti_start;
         parti2->part_entry[i].length = sys_parti_size;
         sys_parti_start += sys_parti_size;
      }

   }

   return FLASH_DEVICE_DONE;
}

/*
 * Calculates the crc for the mibib, user partition table and system partition
 * table structures.
 */
static unsigned int calculate_parti_crc(mi_boot_info_t mi_boot_block,
              flash_usr_partable_t usr_parti, flash_partable_t sys_parti)
{
   unsigned char *crc_buff, *temp;
   unsigned int buff_sz;
   unsigned int block_size;
   unsigned int crc = 0;

   block_size = 0;
   block_size = sizeof(struct mi_boot_info_block)/partition_data.page_size_bytes;

   if (sizeof(struct mi_boot_info_block)%partition_data.page_size_bytes)
   {
      block_size++;
   }

   crc_buff = calloc (1, (block_size * partition_data.page_size_bytes));

   if (crc_buff == NULL)
   {
     return 0;
   }

   memset (crc_buff, 0xFF, (block_size * partition_data.page_size_bytes));
   qmemcpy (crc_buff, mi_boot_block, sizeof (struct mi_boot_info_block));

   /* Calculate CRC for mibib info */
   crc = crc_32_calc(crc_buff,((block_size * partition_data.page_size_bytes)*8),
                     crc);

   block_size = 0;
   block_size = sizeof(struct flash_partition_table)/partition_data.page_size_bytes;

   if (sizeof(struct flash_partition_table)%partition_data.page_size_bytes)
   {
      block_size++;
   }

   crc_buff = realloc (crc_buff, (block_size * partition_data.page_size_bytes));

   if (crc_buff == NULL)
   {
      return 0;
   }

   memset (crc_buff, 0xFF, (block_size * partition_data.page_size_bytes));
   qmemcpy (crc_buff, sys_parti, sizeof (struct flash_partition_table));

   /* Calculate CRC for system partition information */
   crc = crc_32_calc(crc_buff,((block_size * partition_data.page_size_bytes)*8), crc);

   block_size = 0;
   block_size = sizeof(struct flash_usr_partition_table)/partition_data.page_size_bytes;

   if (sizeof(struct flash_usr_partition_table)%partition_data.page_size_bytes)
   {
      block_size++;
   }

   crc_buff = realloc (crc_buff, (block_size * partition_data.page_size_bytes));

   if (crc_buff == NULL)
   {
     return 0;
   }

   memset (crc_buff, 0xFF, (block_size * partition_data.page_size_bytes));
   qmemcpy (crc_buff, usr_parti, sizeof (struct flash_usr_partition_table));

   crc = crc_32_calc(crc_buff,((block_size * partition_data.page_size_bytes)*8), crc);

   free(crc_buff);
   return crc;
}

/*
 * Updates the CRC data structure.
 */
static void update_crc_info(flash_micrc_data_t flash_micrc_info, unsigned int crc)
{
   flash_micrc_info->magic1 = FLASH_MIBIB_CRC_MAGIC1;
   flash_micrc_info->magic2 = FLASH_MIBIB_CRC_MAGIC2;
   flash_micrc_info->version = FLASH_MIBIB_CRC_VERSION;
   flash_micrc_info->crc = crc;
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

   pad_buff = malloc(sizeof(char) * partition_data.page_size_bytes);
   if (NULL == pad_buff)
      error(1, errno, "Memory allocation failed");

   memset(pad_buff, 0xFF, partition_data.page_size_bytes);

   for (i = nmibib_pages; i < partition_data.pages_per_block; i++)
   {
      if (1 != fwrite(pad_buff, partition_data.page_size_bytes, 1, fd))
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
static void update_sys_bin(FILE *fd, mi_boot_info_t mi_boot_block,
                flash_usr_partable_t usr_parti, flash_partable_t sys_parti,
                flash_micrc_data_t crc_parti)
{
   unsigned int nmibib_pages = 0;

   nmibib_pages += update_sys_bin_table(fd, mi_boot_block, sizeof(struct mi_boot_info_block),
                                        partition_data.page_size_bytes, "mibib info");

   nmibib_pages += update_sys_bin_table(fd, sys_parti, sizeof(struct flash_partition_table),
                                        partition_data.page_size_bytes, "sys partition");

   nmibib_pages += update_sys_bin_table(fd, usr_parti, sizeof(struct flash_usr_partition_table),
                                        partition_data.page_size_bytes, "usr partition");

   nmibib_pages += update_sys_bin_table(fd, crc_parti, sizeof(struct flash_mibib_crc_data),
                                        partition_data.page_size_bytes, "crc info");

   update_sys_bin_block_pad(fd, nmibib_pages);
}

/*
 * Console application main function to generate the mibib partition
 * table without flash
 */
int main(int argc, char *argv[])
{
   int option = 0;
   int usr_parti_fname_known = FALSE;
   int sys_parti_fname_known = FALSE;
   int mibib_copies = MIBIB_MAX_COPIES;
   struct mi_boot_info_block mi_boot_info_tbl;
   struct flash_usr_partition_table flash_usr_parti_tbl;
   struct flash_partition_table flash_sys_parti_tbl;
   struct flash_mibib_crc_data flash_mibib_crc_tbl;
   unsigned int crc;
   uint32 age;
   FILE *sysfp;
   char *usrbin = NULL;
   char *sysbin = NULL;

   /* Number of total blocks in device */
   partition_data.block_count = 16;
   /* Number of pages in a block */
   partition_data.pages_per_block = 256;
   /* Logical page size */
   partition_data.page_size_bytes = 256;

   /* Number of total blocks in device */
   secondary_partition_data.block_count = 4096;
   /* Number of pages in a block */
   secondary_partition_data.pages_per_block = 64;
   /* Logical page size */
   secondary_partition_data.page_size_bytes = 2048;



   while ((option = getopt(argc, argv, "s:p:b:x:y:z:c:u:o:")) != -1)
   {
      switch (option)
      {
         case 's':
            partition_data.page_size_bytes = atoi(optarg);
            break;
         case 'p':
            partition_data.pages_per_block = atoi(optarg);
            break;
         case 'b':
            partition_data.block_count = atoi(optarg);
            break;
         case 'x':
            secondary_partition_data.page_size_bytes = atoi(optarg);
            break;
         case 'y':
            secondary_partition_data.pages_per_block = atoi(optarg);
            break;
         case 'z':
            secondary_partition_data.block_count = atoi(optarg);
            break;
         case 'c':
            mibib_copies  = atoi(optarg);
            break;
         case 'u':
            usrbin = optarg;
            break;
         case 'o':
            sysbin = optarg;
            break;
         default:
            print_usage();
            return 1;
      }
   }

   if ((NULL == usrbin) || (NULL == sysbin))
   {
     print_usage();
     return 1;
   }

   memset(&flash_usr_parti_tbl, 0xFF, sizeof (struct flash_usr_partition_table));
   /* Retrieve the user partition table information from the user partition
    * file.
    */
   read_usr_partition(usrbin, &flash_usr_parti_tbl);

   /* Update the system  partition table structure by the information provided
    * in the user partition file.
    */
   if (convert_usr_parti_tbl_to_sys_parti_tbl(&flash_usr_parti_tbl,
       &flash_sys_parti_tbl))
   {
      error(1, 0, "Problem in usr to sys parti conversion");
   }

   sysfp = fopen(sysbin, "wb");
   if (sysfp == NULL)
      error(1, errno, "system partition bin file open failed");

   for (age = 0; age < mibib_copies; age++)
   {
      /* Retrieve mibib structure information */
      update_mibib_info(&mi_boot_info_tbl, age);

      /* Calculate the crc for mibib, system partition and user partition table
       * structures
       */
      crc = calculate_parti_crc(&mi_boot_info_tbl, &flash_usr_parti_tbl,
                                &flash_sys_parti_tbl);

      /* Update the crc structure */
      update_crc_info(&flash_mibib_crc_tbl, crc);

      /* Update the system table binary file */
      update_sys_bin(sysfp, &mi_boot_info_tbl, &flash_usr_parti_tbl,
                     &flash_sys_parti_tbl, &flash_mibib_crc_tbl);
   }

   fclose(sysfp);

   return 0;
}
