/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#if LINK_TIMNA_LIBRARIES

#ifndef UI_ROM_DEFI_LINE_TEVB_INCLUDED
/* { */
#define UI_ROM_DEFI_LINE_TEVB_INCLUDED
/*
 * General include file for reference design.
 */

#include <appl/diag/dpp/ref_sys.h>

#include <appl/diag/dpp/ui_defx.h>
#include <appl/diag/dpp/utils_defx.h>
#include <appl/diag/dpp/utils_line_TEVB.h>
#include <appl/diag/dpp/utils_fap20v_tests.h>

#include <sweep/Fap20v/sweep_fap20v_app.h>

#include <ui_pure_defi_line_tevb.h>


/*
 * UI definition{
 */

EXTERN CONST
   PARAM_VAL_RULES
     Tevb_dffs_dir_format_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "long",
    {
      {
        LONG_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Dffs dir - long version (more detailed).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "short",
    {
      {
        SHORT_EQUIVALENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Dffs dir - short version (less detailed).",
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the
           * compiler silent.
           */
        (long)NULL
      }
    }
  },

/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
 */
EXTERN CONST
   PARAM_VAL
     Tevb_dffs_dir_format_default
#ifdef INIT
   =
{
  VAL_SYMBOL,
  SHORT_EQUIVALENT,
  {
    /*
     * Casting required here only to keep the compiler silent.
     */
    (int)"short"
  }
}
#endif
;

/********************************************************
 */
EXTERN CONST
   PARAM_VAL_RULES
     Tevb_dffs_prnt_size_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
        0,
        0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)0
      }
    }
  },

/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
 */
EXTERN CONST
   PARAM_VAL
     Tevb_dffs_prnt_size_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)(128*1024)
  }
}
#endif
;
/********************************************************
 */
EXTERN CONST
   PARAM_VAL_RULES
     Tevb_on_off_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "on",
    {
      {
        1,
        (long)"    On. Enable. 1 value.",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "off",
    {
      {
        0,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Off. Disable. 0 value.",
        (long)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
 */
EXTERN CONST
   PARAM_VAL_RULES
     Tevb_dffs_date_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        DFFS_DATE_STR_SIZE-1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Dffs file creation date.",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
 */
EXTERN CONST
   PARAM_VAL
     Tevb_dffs_date_default
#ifdef INIT
   =
{
  VAL_TEXT,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)""
  }
}
#endif
;
/********************************************************
 */
EXTERN CONST
   PARAM_VAL
     Tevb_1_value_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  1,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)1
  }
}
#endif
;
/********************************************************
 */
EXTERN CONST
   PARAM_VAL_RULES
     Tevb_dffs_attr_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
        0,
        0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)0
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
 */
EXTERN CONST
   PARAM_VAL
     Tevb_0_value_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)0
  }
}
#endif
;

EXTERN CONST
   PARAM_VAL_RULES
     Tevb_dffs_ver_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
        0,
        0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)0
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
 */
EXTERN CONST
   PARAM_VAL
     Tevb_dffs_attr_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)0
  }
}
#endif
;

/********************************************************
 */
EXTERN CONST
   PARAM_VAL_RULES
     Tevb_empty_vals[]
#ifdef INIT
   =
{
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
 */
EXTERN CONST
   PARAM_VAL_RULES
     Tevb_free_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
        0,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
        (VAL_PROC_PTR)NULL
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
 */
EXTERN CONST
   PARAM_VAL_RULES
     Tevb_dffs_comment_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        DFFS_COMMENT_STR_SIZE-1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    A comment attached to a dffs file.",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
 */
EXTERN CONST
   PARAM_VAL_RULES
     Tevb_dffs_file_name_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        DFFS_FILENAME_SIZE-1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Dffs file name.",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;
/********************************************************
*NAME
*  Tevb_dffs_file_name_vals
*TYPE: BUFFER
*DATE: 24/JUL/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF VALUES
*  RELATED TO THE PARAMETER "DFFS" IN THE
*  CONTEXT OF SUBJECT 'FLASH DFFS'.
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM_VAL':
*    int val_type -
*      Type of value. Can be:
*        VAL_END_OF_LIST 0
*        VAL_NUMERIC     1
*        VAL_SYMBOL      2
*        VAL_TEXT        3
*        VAL_IP          4
*      The first one is only to indicate the end of
*      this array. The others indicate what kind of
*      value this is.
*    VAL_DESCRIPTOR val_descriptor -
*      Union of structures (of types VAL_NUM_DESCRIPTOR,
*      VAL_TEXT_DESCRIPTOR, VAL_IP_DESCRIPTOR and
*      VAL_SYMB_DESCRIPTOR). Descriptor of value.
*      See details in file of definition. Note: Symbols
*      must be small letters only!
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM_VAL_RULES
     Tevb_Tevb_dffs_file_name_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        DFFS_FILENAME_SIZE-1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Dffs file name.",
      }
    }
  },
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
 */
EXTERN CONST
   PARAM
     Tevb_params[]
#ifdef INIT
   =
/*
 ******
 * ENTRY -- 0
 *  BIT(0,1) - MEM
 *  BIT(0) - MEM read
 *  BIT(1) - MEM write
 *  BIT(2) - fpga download
 *  BIT(3,4,5) - flash
 ******
 * ENTRY -- 1
 *BIT() - .
 */
{
  {
    PARAM_TEVB_MEM_ID,
    "mem",
    (PARAM_VAL_RULES *)&Tevb_empty_vals[0],
    (sizeof(Tevb_empty_vals) / sizeof(Tevb_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Access to memory.",
    "Examples:\r\n"
    "  tevb mem read 0x10\r\n",
    "",
    {BIT(0) | BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_MEM_READ_ID,
    "read",
    (PARAM_VAL_RULES *)&Tevb_free_vals[0],
    (sizeof(Tevb_free_vals) / sizeof(Tevb_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Read from memory",
    "Examples:\r\n"
    "  tevb read 0x10\r\n"
    "Results in printing the contant of memory address (Timna CPU address space) 0x10.",
    "",
    {BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_MEM_WRITE_ID,
    "write",
    (PARAM_VAL_RULES *)&Tevb_free_vals[0],
    (sizeof(Tevb_free_vals) / sizeof(Tevb_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Write to memory",
    "Examples:\r\n"
    "  tevb write 0x10 data 15\r\n"
    "Results in writing 15 to memory address (Timna CPU address space) 0x10.",
    "",
    {BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_MEM_DATA_ID,
    "data",
    (PARAM_VAL_RULES *)&Tevb_free_vals[0],
    (sizeof(Tevb_free_vals) / sizeof(Tevb_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Data to be written to memoty",
    "Examples:\r\n"
    "  tevb write 0x10 data 15\r\n"
    "Results in writing 15 to memory address (Timna CPU address space) 0x10.",
    "",
    {BIT(1)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_VERBOSE_ID,
    "verbose",
    (PARAM_VAL_RULES *)&Tevb_empty_vals[0],
    (sizeof(Tevb_empty_vals) / sizeof(Tevb_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Do more printing.",
    "",
    "",
    {BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5) | BIT(6) | BIT(9) ,
     BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_COMPRESSED_ID,
    "compressed",
    (PARAM_VAL_RULES *)&Tevb_empty_vals[0],
    (sizeof(Tevb_empty_vals) / sizeof(Tevb_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Infalte the file before burning to fpga.",
    "",
    "",
    {BIT(2) | BIT(3) | BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FPGA_ID,
    "fpga",
    (PARAM_VAL_RULES *)&Tevb_empty_vals[0],
    (sizeof(Tevb_empty_vals) / sizeof(Tevb_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "TEVB FPGA BSP.",
    "Examples:\r\n"
    "  tevb fpga download\r\n",
    "",
    {BIT(2)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FPGA_DOWNLOAD_ID,
    "download",
    (PARAM_VAL_RULES *)&Tevb_empty_vals[0],
    (sizeof(Tevb_empty_vals) / sizeof(Tevb_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Download and burn TEVB FPGA.",
    "Examples:\r\n"
    "  tevb fpga download\r\n",
    "",
    {BIT(2)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FLASH_ID,
    "flash",
    (PARAM_VAL_RULES *)&Tevb_empty_vals[0],
    (sizeof(Tevb_empty_vals) / sizeof(Tevb_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "TEVB FLASH BSP. This flash holds the FPGAs executable.",
    "Examples:\r\n"
    "  tevb flash burn_fpga\r\n",
    "",
    {BIT(3) | BIT(4) | BIT(5) | BIT(6) | BIT(7),
    BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FLASH_BURN_ID,
    "burn_fpga",
    (PARAM_VAL_RULES *)&Tevb_empty_vals[0],
    (sizeof(Tevb_empty_vals) / sizeof(Tevb_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "burn TEVB FPGA. The exe is taken from the FLASH.",
    "\r\n",
    "",
    {BIT(6)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FLASH_DOWNLOAD_ID,
    "download",
    (PARAM_VAL_RULES *)&Tevb_empty_vals[0],
    (sizeof(Tevb_empty_vals) / sizeof(Tevb_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Download and burn TEVB FLASH.",
    "\r\n",
    "",
    {BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FLASH_DIAGNOSTICS_ID,
    "diagnostics",
    (PARAM_VAL_RULES *)&Tevb_empty_vals[0],
    (sizeof(Tevb_empty_vals) / sizeof(Tevb_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Diagnostics of TEVB FLASH.",
    "\r\n",
    "",
    {BIT(7)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FLASH_FILENAME_ID,
    "filename",
    (PARAM_VAL_RULES *)&Tevb_Tevb_dffs_file_name_vals[0],
    (sizeof(Tevb_Tevb_dffs_file_name_vals) / sizeof(Tevb_Tevb_dffs_file_name_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Name of file.",
    "\r\n",
    "",
    {BIT(3) | BIT(4)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FLASH_ERASE_ID,
    "erase",
    (PARAM_VAL_RULES *)&Tevb_empty_vals[0],
    (sizeof(Tevb_empty_vals) / sizeof(Tevb_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    " Erase entire TEVB FLASH. If filename is specified only one file is deleted.",
    "\r\n",
    "",
    {BIT(4)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FLASH_DFFS_ID,
    "dffs",
    (PARAM_VAL_RULES *)&Tevb_empty_vals[0],
    (sizeof(Tevb_empty_vals) / sizeof(Tevb_empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  a TEVB purpose file system located on the embedded flash,\r\n"
    "  dffs section is separate from the application section.",
    "Examples:\r\n"
    "  TEVB flash dffs download file_name file1.txt\r\n"
    "  results in downloading file1.txt and writing it to embedded flash\r\n",
    "",
    {
      0,
      BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5)
    },
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FLASH_DFFS_DIR_ID,
    "dir",
    (PARAM_VAL_RULES *)&Tevb_empty_vals[0],
    (sizeof(Tevb_empty_vals) / sizeof(Tevb_empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  list all files and their attributes.\r\n"
    "  long format adds comments listing",
    "Examples:\r\n"
    "  TEVB flash dffs dir format long\r\n",
    "",
    {
      0,
      BIT(1)
    },
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FLASH_DFFS_DIR_FORMAT_ID,
    "format",
    (PARAM_VAL_RULES *)&Tevb_dffs_dir_format_vals[0],
    (sizeof(Tevb_dffs_dir_format_vals) / sizeof(Tevb_dffs_dir_format_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&Tevb_dffs_dir_format_default,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  list all files and their attributes.\r\n"
    "  long format adds comments listing",
    "Examples:\r\n"
    "  TEVB flash dffs dir format long\r\n",
    "",
    {
      0,
      BIT(1)
    },
    4,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FLASH_DFFS_DIAGNOSTICS_ID,
    "diagnostics",
    (PARAM_VAL_RULES *)&Tevb_empty_vals[0],
    (sizeof(Tevb_empty_vals) / sizeof(Tevb_empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  print full dffs information.\r\n"
    "  can be used for diagnostics and debug",
    "Examples:\r\n"
    "  TEVB flash dffs diagnostics\r\n",
    "",
    {
      0,
      BIT(5)
    },
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FLASH_DFFS_DEL_ID,
    "delete",
    (PARAM_VAL_RULES *)&Tevb_empty_vals[0],
    (sizeof(Tevb_empty_vals) / sizeof(Tevb_empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  delete a file from the embedded flash",
    "Examples:\r\n"
    "  TEVB flash dffs delete file_name file1.txt\r\n"
    "  Results in erasing file1.txt from flash\r\n",
    "",
    {
      0,
      BIT(0)
    },
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FLASH_DFFS_DLD_ID,
    "download",
    (PARAM_VAL_RULES *)&Tevb_empty_vals[0],
    (sizeof(Tevb_empty_vals) / sizeof(Tevb_empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  download a file from host and burn it on the embedded flash",
    "Examples:\r\n"
    "  TEVB flash dffs download file_name file1.txt\r\n"
    "  Results in downloading file1.txt from host and writing it to flash \r\n",
    "",
    {
      0,
      BIT(0)
    },
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FLASH_DFFS_PRINT_ID,
    "print",
    (PARAM_VAL_RULES *)&Tevb_empty_vals[0],
    (sizeof(Tevb_empty_vals) / sizeof(Tevb_empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  print the binary contents of the specified file,\r\n"
    "  if that file exists\r\n"
    "  enough RAM memory must be available to load the file",
    "Examples:\r\n"
    "  TEVB flash dffs print file_name file1.txt size_in_bytes 300\r\n"
    "  Results in printing first 300 bytes (if exist) of file1.txt \r\n",
    "",
    {
      0,
      BIT(4)
    },
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FLASH_DFFS_PRINT_SIZE_ID,
    "size_in_bytes",
    (PARAM_VAL_RULES *)&Tevb_dffs_prnt_size_vals[0],
    (sizeof(Tevb_dffs_prnt_size_vals) / sizeof(Tevb_dffs_prnt_size_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  the number of bytes to print",
    "Examples:\r\n"
    "  TEVB flash dffs write file_name file1.txt\r\n"
    "  date 05/06/06 attr1 12 attr2 1 attr3 98 \r\n",
    "",
    {
      0,
      BIT(4)
    },
    5,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FLASH_DFFS_ADD_COMMENT_ID,
    "add_comment",
    (PARAM_VAL_RULES *)&Tevb_empty_vals[0],
    (sizeof(Tevb_empty_vals) / sizeof(Tevb_empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  add comment to dffs file\r\n"
    "  the comment should be one word - spaces should be avoided!",
    "Examples:\r\n"
    "  TEVB flash dffs add_comment file_name file1.txt comment this_is_my_comment \r\n"
    "  Results in adding the string 'this_is_my_comment' as a comment \r\n",
    "",
    {
      0,
      BIT (2)
    },
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FLASH_DFFS_SHOW_COMMENT_ID,
    "show_comment",
    (PARAM_VAL_RULES *)&Tevb_empty_vals[0],
    (sizeof(Tevb_empty_vals) / sizeof(Tevb_empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  show a comment of a dffs file",
    "Examples:\r\n"
    "  TEVB flash dffs show_comment file_name file1.txt\r\n"
    "  Results in printing the comment attached to file1.txt \r\n",
    "",
    {
      0,
      BIT (0)
    },
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FLASH_DFFS_NAME_ID,
    "file_name",
    (PARAM_VAL_RULES *)&Tevb_dffs_file_name_vals[0],
    (sizeof(Tevb_dffs_file_name_vals) / sizeof(Tevb_dffs_file_name_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  the name of the dffs file - maximum 24 characters long",
    "Examples:\r\n"
    "  TEVB flash dffs print file_name file1.txt\r\n",
    "",
    {
      0,
      BIT(0) | BIT (2) | BIT (3) | BIT(4)
    },
    4,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FLASH_DFFS_COMMENT_ID,
    "comment",
    (PARAM_VAL_RULES *)&Tevb_dffs_comment_vals[0],
    (sizeof(Tevb_dffs_comment_vals) / sizeof(Tevb_dffs_comment_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  comment attached to a dffs file"
    "  the comment should be one word - spaces should be avoided!",
    "Examples:\r\n"
    "  TEVB flash dffs add_comment file_name file1.txt comment this_is_my_comment \r\n"
    "  Results in adding the string 'this_is_my_comment' as a comment \r\n",
    "",
    {
      0,
      BIT (2)
    },
    5,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FLASH_DFFS_ADD_ATTR_ID,
    "add_attribute",
    (PARAM_VAL_RULES *)&Tevb_empty_vals[0],
    (sizeof(Tevb_empty_vals) / sizeof(Tevb_empty_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_NO_DEFAULT,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  add a free numeric attribute to dffs file\r\n"
    "  maximum 6 digits allowed",
    "Examples:\r\n"
    "  TEVB flash dffs add_attribute file_name file1.txt attr1_val 3\r\n"
    "  Results in adding 3 as the value of attr1 attribute for file1.txt\r\n",
    "",
    {
      0,
      BIT(0) | BIT (3)
    },
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FLASH_DFFS_DATE_ID,
    "date_val",
    (PARAM_VAL_RULES *)&Tevb_dffs_date_vals[0],
    (sizeof(Tevb_dffs_date_vals) / sizeof(Tevb_dffs_date_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&Tevb_dffs_date_default,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  creation date - in a dd/mm/yy format",
    "Examples:\r\n"
    "  TEVB flash dffs add_attribute file_name file1.txt date_val 22/10/77\r\n"
    "  Results in adding 22/10/77 as the creation date for file1.txt\r\n",

    "",
    {
      0,
      BIT (3)
    },
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FLASH_DFFS_VER_ID,
    "ver_val",
    (PARAM_VAL_RULES *)&Tevb_dffs_ver_vals[0],
    (sizeof(Tevb_dffs_ver_vals) / sizeof(Tevb_dffs_ver_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&Tevb_dffs_attr_default,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  version of dffs file",
    "Examples:\r\n"
     "  TEVB flash dffs add_attribute file_name file1.txt ver_val 2\r\n"
    "  Results in adding 2 as the version of file1.txt\r\n",
   "",
    {
      0,
      BIT (3)
    },
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FLASH_DFFS_ATTR1_ID,
    "attr1_val",
    (PARAM_VAL_RULES *)&Tevb_dffs_attr_vals[0],
    (sizeof(Tevb_dffs_attr_vals) / sizeof(Tevb_dffs_attr_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&Tevb_dffs_attr_default,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  free attribute value - numeric value, maximum 6 digits",
    "Examples:\r\n"
     "  TEVB flash dffs add_attribute file_name file1.txt attr1_val 3\r\n"
    "  Results in adding 3 as the value of attr1 attribute for file1.txt\r\n",
    "",
    {
      0,
      BIT (3)
    },
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FLASH_DFFS_ATTR2_ID,
    "attr2_val",
    (PARAM_VAL_RULES *)&Tevb_dffs_attr_vals[0],
    (sizeof(Tevb_dffs_attr_vals) / sizeof(Tevb_dffs_attr_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&Tevb_dffs_attr_default,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  free attribute value - numeric value, maximum 6 digits",
    "Examples:\r\n"
     "  TEVB flash dffs add_attribute file_name file1.txt attr2_val 3\r\n"
    "  Results in adding 3 as the value of attr2 attribute for file1.txt\r\n",
    "",
    {
      0,
      BIT (3)
    },
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_TEVB_FLASH_DFFS_ATTR3_ID,
    "attr3_val",
    (PARAM_VAL_RULES *)&Tevb_dffs_attr_vals[0],
    (sizeof(Tevb_dffs_attr_vals) / sizeof(Tevb_dffs_attr_vals[0])) - 1,
    MAY_NOT_APPEAR | HAS_DEFAULT,(PARAM_VAL *)&Tevb_dffs_attr_default,
    0,0,0,0,0,0,0,
    "Dune Flash File System:\r\n"
    "  free attribute value - numeric value, maximum 6 digits",
    "Examples:\r\n"
     "  TEVB flash dffs add_attribute file_name file1.txt attr3_val 3\r\n"
    "  Results in adding 3 as the value of attr3 attribute for file1.txt\r\n",
    "",
    {
      0,
      BIT (3)
    },
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
/*
 * Last element. Do not remove.
 */
  {
    PARAM_END_OF_LIST
  }
}
#endif
;

/*
 * }
 */


/* } */
#endif

#endif /*#if LINK_TIMNA_LIBRARIES*/

