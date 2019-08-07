// clang-format off

/* This file is part of ooFatFs, a customised version of FatFs
 * See https://github.com/micropython/oofatfs for details
 */

/*----------------------------------------------------------------------------/
/  FatFs - Generic FAT Filesystem module  R0.13c                              /
/-----------------------------------------------------------------------------/
/
/ Copyright (C) 2018, ChaN, all right reserved.
/
/ FatFs module is an open source software. Redistribution and use of FatFs in
/ source and binary forms, with or without modification, are permitted provided
/ that the following condition is met:

/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/
/----------------------------------------------------------------------------*/


#ifndef FF_DEFINED
#define FF_DEFINED  86604   /* Revision ID */

#ifdef __cplusplus
extern "C" {
#endif

#include "ffconf.h"       /* FatFs configuration options */

#if FF_DEFINED != FFCONF_DEF
#error Wrong configuration file (ffconf.h).
#endif


/* Integer types used for FatFs API */

#if defined(_WIN32) /* Main development platform */
#define FF_INTDEF 2
#include <windows.h>
typedef unsigned __int64 QWORD;
#elif (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || defined(__cplusplus)    /* C99 or later */
#define FF_INTDEF 2
#include <stdint.h>
typedef unsigned int    UINT;   /* int must be 16-bit or 32-bit */
typedef unsigned char   BYTE;   /* char must be 8-bit */
typedef uint16_t        WORD;   /* 16-bit unsigned integer */
typedef uint16_t        WCHAR;  /* 16-bit unsigned integer */
typedef uint32_t        DWORD;  /* 32-bit unsigned integer */
typedef uint64_t        QWORD;  /* 64-bit unsigned integer */
#else   /* Earlier than C99 */
#define FF_INTDEF 1
typedef unsigned int    UINT;   /* int must be 16-bit or 32-bit */
typedef unsigned char   BYTE;   /* char must be 8-bit */
typedef unsigned short  WORD;   /* 16-bit unsigned integer */
typedef unsigned short  WCHAR;  /* 16-bit unsigned integer */
typedef unsigned long   DWORD;  /* 32-bit unsigned integer */
#endif

/* Type of path name strings on FatFs API */

#ifndef _INC_TCHAR
#define _INC_TCHAR

/* Unicode in UTF-8 encoding */
typedef char TCHAR;
#define _T(x) u8 ## x
#define _TEXT(x) u8 ## x

#endif


/* Type of file size variables */

typedef DWORD FSIZE_t;


/* Filesystem object structure (FATFS) */

typedef struct {
    void    *drv;           // block device underlying this filesystem
    BYTE    fs_type;        /* Filesystem type (0:not mounted) */
    BYTE    n_fats;         /* Number of FATs (1 or 2) */
    BYTE    wflag;          /* win[] flag (b0:dirty) */
    BYTE    fsi_flag;       /* FSINFO flags (b7:disabled, b0:dirty) */
    WORD    id;             /* Volume mount ID */
    WORD    n_rootdir;      /* Number of root directory entries (FAT12/16) */
    WORD    csize;          /* Cluster size [sectors] */
#if FF_MAX_SS != FF_MIN_SS
    WORD    ssize;          /* Sector size (512, 1024, 2048 or 4096) */
#endif
#if FF_USE_LFN
    WCHAR*  lfnbuf;         /* LFN working buffer */
#endif
#if !FF_FS_READONLY
    DWORD   last_clst;      /* Last allocated cluster */
    DWORD   free_clst;      /* Number of free clusters */
#endif
    DWORD   n_fatent;       /* Number of FAT entries (number of clusters + 2) */
    DWORD   fsize;          /* Size of an FAT [sectors] */
    DWORD   volbase;        /* Volume base sector */
    DWORD   fatbase;        /* FAT base sector */
    DWORD   dirbase;        /* Root directory base sector/cluster */
    DWORD   database;       /* Data base sector */
    DWORD   winsect;        /* Current sector appearing in the win[] */
    BYTE    win[FF_MAX_SS]; /* Disk access window for Directory, FAT (and file data at tiny cfg) */
} FATFS;



/* Object ID and allocation information (FFOBJID) */

typedef struct {
    FATFS*  fs;             /* Pointer to the hosting volume of this object */
    WORD    id;             /* Hosting volume mount ID */
    BYTE    attr;           /* Object attribute */
    BYTE    stat;           /* Object chain status (b1-0: =0:not contiguous, =2:contiguous, =3:fragmented in this session, b2:sub-directory stretched) */
    DWORD   sclust;         /* Object data start cluster (0:no cluster or root directory) */
    FSIZE_t objsize;        /* Object size (valid when sclust != 0) */
} FFOBJID;



/* File object structure (FIL) */

typedef struct {
    FFOBJID obj;            /* Object identifier (must be the 1st member to detect invalid object pointer) */
    BYTE    flag;           /* File status flags */
    BYTE    err;            /* Abort flag (error code) */
    FSIZE_t fptr;           /* File read/write pointer (Zeroed on file open) */
    DWORD   clust;          /* Current cluster of fpter (invalid when fptr is 0) */
    DWORD   sect;           /* Sector number appearing in buf[] (0:invalid) */
#if !FF_FS_READONLY
    DWORD   dir_sect;       /* Sector number containing the directory entry (not used at exFAT) */
    BYTE*   dir_ptr;        /* Pointer to the directory entry in the win[] (not used at exFAT) */
#endif
#if !FF_FS_TINY
    BYTE    buf[FF_MAX_SS]; /* File private data read/write window */
#endif
} FIL;



/* Directory object structure (FF_DIR) */

typedef struct {
    FFOBJID obj;            /* Object identifier */
    DWORD   dptr;           /* Current read/write offset */
    DWORD   clust;          /* Current cluster */
    DWORD   sect;           /* Current sector (0:Read operation has terminated) */
    BYTE*   dir;            /* Pointer to the directory item in the win[] */
    BYTE    fn[12];         /* SFN (in/out) {body[8],ext[3],status[1]} */
#if FF_USE_LFN
    DWORD   blk_ofs;        /* Offset of current entry block being processed (0xFFFFFFFF:Invalid) */
#endif
} FF_DIR;



/* File information structure (FILINFO) */

typedef struct {
    FSIZE_t fsize;          /* File size */
    WORD    fdate;          /* Modified date */
    WORD    ftime;          /* Modified time */
    BYTE    fattrib;        /* File attribute */
#if FF_USE_LFN
    TCHAR   altname[FF_SFN_BUF + 1];/* Altenative file name */
    TCHAR   fname[FF_LFN_BUF + 1];  /* Primary file name */
#else
    TCHAR   fname[12 + 1];  /* File name */
#endif
} FILINFO;



/* File function return code (FRESULT) */

typedef enum {
    FR_OK = 0,              /* (0) Succeeded */
    FR_DISK_ERR,            /* (1) A hard error occurred in the low level disk I/O layer */
    FR_INT_ERR,             /* (2) Assertion failed */
    FR_NOT_READY,           /* (3) The physical drive cannot work */
    FR_NO_FILE,             /* (4) Could not find the file */
    FR_NO_PATH,             /* (5) Could not find the path */
    FR_INVALID_NAME,        /* (6) The path name format is invalid */
    FR_DENIED,              /* (7) Access denied due to prohibited access or directory full */
    FR_EXIST,               /* (8) Access denied due to prohibited access */
    FR_INVALID_OBJECT,      /* (9) The file/directory object is invalid */
    FR_WRITE_PROTECTED,     /* (10) The physical drive is write protected */
    FR_INVALID_DRIVE,       /* (11) The logical drive number is invalid */
    FR_NOT_ENABLED,         /* (12) The volume has no work area */
    FR_NO_FILESYSTEM,       /* (13) There is no valid FAT volume */
    FR_MKFS_ABORTED,        /* (14) The f_mkfs() aborted due to any problem */
    FR_TIMEOUT,             /* (15) Could not get a grant to access the volume within defined period */
    FR_LOCKED,              /* (16) The operation is rejected according to the file sharing policy */
    FR_NOT_ENOUGH_CORE,     /* (17) LFN working buffer could not be allocated */
    FR_TOO_MANY_OPEN_FILES, /* (18) Number of open files > FF_FS_LOCK */
    FR_INVALID_PARAMETER    /* (19) Given parameter is invalid */
} FRESULT;



/*--------------------------------------------------------------*/
/* FatFs module application interface                           */

FRESULT f_open (FATFS *fs, FIL* fp, const TCHAR* path, BYTE mode);  /* Open or create a file */
FRESULT f_close (FIL* fp);                                          /* Close an open file object */
FRESULT f_read (FIL* fp, void* buff, UINT btr, UINT* br);           /* Read data from the file */
FRESULT f_write (FIL* fp, const void* buff, UINT btw, UINT* bw);    /* Write data to the file */
FRESULT f_lseek (FIL* fp, FSIZE_t ofs);                             /* Move file pointer of the file object */
FRESULT f_truncate (FIL* fp);                                       /* Truncate the file */
FRESULT f_sync (FIL* fp);                                           /* Flush cached data of the writing file */
FRESULT f_opendir (FATFS *fs, FF_DIR* dp, const TCHAR* path);       /* Open a directory */
FRESULT f_closedir (FF_DIR* dp);                                    /* Close an open directory */
FRESULT f_readdir (FF_DIR* dp, FILINFO* fno);                       /* Read a directory item */
FRESULT f_findfirst (FF_DIR* dp, FILINFO* fno, const TCHAR* path, const TCHAR* pattern); /* Find first file */
FRESULT f_findnext (FF_DIR* dp, FILINFO* fno);                      /* Find next file */
FRESULT f_mkdir (FATFS *fs, const TCHAR* path);                     /* Create a sub directory */
FRESULT f_unlink (FATFS *fs, const TCHAR* path);                    /* Delete an existing file or directory */
FRESULT f_rename (FATFS *fs, const TCHAR* path_old, const TCHAR* path_new); /* Rename/Move a file or directory */
FRESULT f_stat (FATFS *fs, const TCHAR* path, FILINFO* fno);        /* Get file status */
FRESULT f_chmod (FATFS *fs, const TCHAR* path, BYTE attr, BYTE mask); /* Change attribute of a file/dir */
FRESULT f_utime (FATFS *fs, const TCHAR* path, const FILINFO* fno); /* Change timestamp of a file/dir */
FRESULT f_chdir (FATFS *fs, const TCHAR* path);                     /* Change current directory */
FRESULT f_getcwd (FATFS *fs, TCHAR* buff, UINT len);                /* Get current directory */
FRESULT f_getfree (FATFS *fs, DWORD* nclst);                        /* Get number of free clusters on the drive */
FRESULT f_getlabel (FATFS *fs, TCHAR* label, DWORD* vsn);           /* Get volume label */
FRESULT f_setlabel (FATFS *fs, const TCHAR* label);                 /* Set volume label */
FRESULT f_forward (FIL* fp, UINT(*func)(const BYTE*,UINT), UINT btf, UINT* bf); /* Forward data to the stream */
FRESULT f_expand (FIL* fp, FSIZE_t szf, BYTE opt);                  /* Allocate a contiguous block to the file */
FRESULT f_mount (FATFS* fs);                                        /* Mount/Unmount a logical drive */
FRESULT f_umount (FATFS* fs);                                       /* Unmount a logical drive */
FRESULT f_mkfs (FATFS *fs, BYTE opt, DWORD au, void* work, UINT len); /* Create a FAT volume */
FRESULT f_fdisk (void *pdrv, const DWORD* szt, void* work);         /* Divide a physical drive into some partitions */
FRESULT f_setcp (WORD cp);                                          /* Set current code page */
FRESULT f_repair (FATFS* fs, void* work, UINT len);                 /* Free unreferenced clusters from the FAT */

#define f_eof(fp) ((int)((fp)->fptr == (fp)->obj.objsize))
#define f_error(fp) ((fp)->err)
#define f_tell(fp) ((fp)->fptr)
#define f_size(fp) ((fp)->obj.objsize)
#define f_rewind(fp) f_lseek((fp), 0)
#define f_rewinddir(dp) f_readdir((dp), 0)
#define f_rmdir(path) f_unlink(path)
#define f_unmount(path) f_mount(0, path, 0)

#ifndef EOF
#define EOF (-1)
#endif




/*--------------------------------------------------------------*/
/* Additional user defined functions                            */

/* RTC function */
#if !FF_FS_READONLY && !FF_FS_NORTC
DWORD get_fattime (void);
#endif

/* LFN support functions */
WCHAR ff_oem2uni (WCHAR oem, WORD cp);  /* OEM code to Unicode conversion */
WCHAR ff_uni2oem (DWORD uni, WORD cp);  /* Unicode to OEM code conversion */
DWORD ff_wtoupper (DWORD uni);          /* Unicode upper-case conversion */


/*--------------------------------------------------------------*/
/* Flags and offset address                                     */


/* File access mode and open method flags (3rd argument of f_open) */
#define FA_READ             0x01
#define FA_WRITE            0x02
#define FA_OPEN_EXISTING    0x00
#define FA_CREATE_NEW       0x04
#define FA_CREATE_ALWAYS    0x08
#define FA_OPEN_ALWAYS      0x10
#define FA_OPEN_APPEND      0x30

/* Fast seek controls (2nd argument of f_lseek) */
#define CREATE_LINKMAP  ((FSIZE_t)0 - 1)

/* Format options (2nd argument of f_mkfs) */
#define FM_FAT      0x01
#define FM_FAT32    0x02
#define FM_EXFAT    0x04
#define FM_ANY      0x07
#define FM_SFD      0x08

/* Filesystem type (FATFS.fs_type) */
#define FS_FAT12    1
#define FS_FAT16    2
#define FS_FAT32    3
#define FS_EXFAT    4

/* File attribute bits for directory entry (FILINFO.fattrib) */
#define AM_RDO  0x01    /* Read only */
#define AM_HID  0x02    /* Hidden */
#define AM_SYS  0x04    /* System */
#define AM_DIR  0x10    /* Directory */
#define AM_ARC  0x20    /* Archive */


#ifdef __cplusplus
}
#endif

#endif /* FF_DEFINED */
