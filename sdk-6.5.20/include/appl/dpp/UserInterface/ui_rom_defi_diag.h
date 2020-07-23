/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UI_ROM_DEFI_DIAG_INCLUDED
/* { */
#define UI_ROM_DEFI_DIAG_INCLUDED
/*
 * General include file for reference design.
 */

#include <appl/diag/dpp/ref_sys.h>

#include <appl/diag/dpp/ui_defx.h>
#include <appl/diag/dpp/utils_defx.h>
#ifdef LINK_FAP20V_LIBRARIES
  #include <appl/diag/dpp/utils_fap20v_tests.h>

  #include <sweep/Fap20v/sweep_fap20v_app.h>
#endif /* LINK_FAP20V_LIBRARIES */

#include <ui_pure_defi_diag.h>
#include <appl/diag/dpp/utils_hw_diagnostics.h>


/*
 * UI definition{
 */

/********************************************************
 */
EXTERN CONST
   PARAM_VAL_RULES
     Diag_revert_data_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "true",
    {
      {
        UTILS_HW_DIAG_MEM_TEST_REVERT_DATA_YES,
        (long)"    Revert data bits.",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "false",
    {
      {
        UTILS_HW_DIAG_MEM_TEST_REVERT_DATA_NO,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Don't revert data bits.",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "both",
    {
      {
        UTILS_HW_DIAG_MEM_TEST_REVERT_DATA_BOTH,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Run test once without reverted data, and again with reverted data.",
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
     Diag_revert_addrs_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "true",
    {
      {
        UTILS_HW_DIAG_MEM_TEST_REVERT_ADDRS_YES,
        (long)"    Revert address offset bits.",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "false",
    {
      {
        UTILS_HW_DIAG_MEM_TEST_REVERT_ADDRS_NO,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Don't revert address offset bits.",
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
     Diag_addrs_alignment_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "byte",
    {
      {
        UTILS_HW_DIAG_MEM_TEST_ADDRESS_BYTE_ALIGNMENT,
        (long)"    Byte alignment.",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "long",
    {
      {
        UTILS_HW_DIAG_MEM_TEST_ADDRESS_LONG_ALIGNMENT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Long alignment.",
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
     Diag_data_walk_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "index_walk",
    {
      {
        UTILS_HW_DIAG_MEM_TEST_DATA_TYPE_RUNNING_INDEX,
        (long)"    Index walk: 0,1,2,3... (dec).",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "walking_one",
    {
      {
        UTILS_HW_DIAG_MEM_TEST_DATA_TYPE_WALKING_ONE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Walking one: 0, 1, 10, 100... (bin).",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "random",
    {
      {
        UTILS_HW_DIAG_MEM_TEST_DATA_TYPE_RANDOM,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Random data",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "sso",
    {
      {
        UTILS_HW_DIAG_MEM_TEST_DATA_TYPE_SSO,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    SSO: 1111.., 0000..., 1111..., 0000 ...(bin).",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "cross",
    {
      {
        UTILS_HW_DIAG_MEM_TEST_DATA_TYPE_CROSS_WALK,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Cross walk: 1010.., 0101..., 1010..., 0101...(bin).",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "pattern",
    {
      {
        UTILS_HW_DIAG_MEM_TEST_DATA_TYPE_PATTERN,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Pattern: constant value.",
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
     Diag_offset_walk_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "index_walk",
    {
      {
        UTILS_HW_DIAG_MEM_TEST_ADDR_WALK_TYPE_INC_ONE,
        (long)"    Index walk: 0,1,2,3... (dec).",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "walking_one",
    {
      {
        UTILS_HW_DIAG_MEM_TEST_ADDR_WALK_TYPE_WALKING_ONE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    Walking one: 0, 1, 10, 100... (bin).",
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
     Diag_revert_data_value_default
#ifdef INIT
   =
{
  VAL_NUMERIC,
  UTILS_HW_DIAG_MEM_TEST_REVERT_DATA_NO,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)UTILS_HW_DIAG_MEM_TEST_REVERT_DATA_NO
  }
}
#endif
;

/********************************************************
 */
EXTERN CONST
   PARAM_VAL_RULES
     Diag_empty_vals[]
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
     Diag_free_vals[]
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
   PARAM
     Diag_params[]
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
    PARAM_DIAG_HW_ID,
    "gen_hw",
    (PARAM_VAL_RULES *)&Diag_empty_vals[0],
    (sizeof(Diag_empty_vals) / sizeof(Diag_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Generic HW tests\r\n",
    "",
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
    PARAM_DIAG_HW_MEM_ID,
    "mem_test",
    (PARAM_VAL_RULES *)&Diag_empty_vals[0],
    (sizeof(Diag_empty_vals) / sizeof(Diag_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Test memory space.",
    "Examples:\r\n"
    "  tevb mem read 0x10\r\n"
    "NOTE: Do not run memory test on in use memory\r\n",
    "",
    {BIT(0) | BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DIAG_HW_MEM_SET_SPACE_ID,
    "set_space",
    (PARAM_VAL_RULES *)&Diag_empty_vals[0],
    (sizeof(Diag_empty_vals) / sizeof(Diag_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set tested memory space (default is CPU space)",
    "",
    "",
    {BIT(0)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

#if LINK_FAP20V_LIBRARIES
/*
 * {
 */
  {
    PARAM_DIAG_HW_MEM_SPACE_ZBT_ID,
    "zbt",
    (PARAM_VAL_RULES *)&Diag_free_vals[0],
    (sizeof(Diag_free_vals) / sizeof(Diag_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "ZBT memory space",
    "\r\n",
    "",
    {BIT(0)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DIAG_HW_MEM_SPACE_DRAM_ID,
    "dram",
    (PARAM_VAL_RULES *)&Diag_free_vals[0],
    (sizeof(Diag_free_vals) / sizeof(Diag_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "DRAM memory space",
    "\r\n",
    "",
    {BIT(0)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
/*
 * } LINK_FAP20V_LIBRARIES
 */
#endif

  {
    PARAM_DIAG_HW_MEM_SPACE_CPU_ID,
    "cpu",
    (PARAM_VAL_RULES *)&Diag_empty_vals[0],
    (sizeof(Diag_empty_vals) / sizeof(Diag_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "CPU memory space",
    "\r\n",
    "",
    {BIT(0)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DIAG_HW_MEM_BASE_ADDR_ID,
    "base_addr",
    (PARAM_VAL_RULES *)&Diag_free_vals[0],
    (sizeof(Diag_free_vals) / sizeof(Diag_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Base address of tested memory",
    "\r\n",
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
    PARAM_DIAG_HW_MEM_SIZE_ID,
    "size",
    (PARAM_VAL_RULES *)&Diag_free_vals[0],
    (sizeof(Diag_free_vals) / sizeof(Diag_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Size of tested space in bytes",
    "\r\n",
    "",
    {BIT(1)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DIAG_HW_MEM_REVERT_DATA_ID,
    "revert_data",
    (PARAM_VAL_RULES *)&Diag_revert_data_vals[0],
    (sizeof(Diag_revert_data_vals) / sizeof(Diag_revert_data_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Revertion of data bits",
    "\r\n",
    "",
    {BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DIAG_HW_MEM_REVERT_OFFSET_ID,
    "revert_offset",
    (PARAM_VAL_RULES *)&Diag_revert_addrs_vals[0],
    (sizeof(Diag_revert_addrs_vals) / sizeof(Diag_revert_addrs_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Revertion of address offset bits",
    "\r\n",
    "",
    {BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DIAG_HW_MEM_OFFSET_WALK_ID,
    "offset_walk",
    (PARAM_VAL_RULES *)&Diag_offset_walk_vals[0],
    (sizeof(Diag_offset_walk_vals) / sizeof(Diag_offset_walk_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Offset walk method (relation between address offset and index in test)",
    "\r\n",
    "",
    {BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DIAG_HW_MEM_DATA_WALK_ID,
    "data_walk",
    (PARAM_VAL_RULES *)&Diag_data_walk_vals[0],
    (sizeof(Diag_data_walk_vals) / sizeof(Diag_data_walk_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Writen data method (relation between data and index in test)",
    "\r\n",
    "",
    {BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DIAG_HW_MEM_ALIGNMENT_ID,
    "address_alignment",
    (PARAM_VAL_RULES *)&Diag_addrs_alignment_vals[0],
    (sizeof(Diag_addrs_alignment_vals) / sizeof(Diag_addrs_alignment_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Address alignment - bytes or longs",
    "\r\n",
    "",
    {BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DIAG_HW_MEM_PATTERN_ID,
    "pattern_value",
    (PARAM_VAL_RULES *)&Diag_free_vals[0],
    (sizeof(Diag_free_vals) / sizeof(Diag_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Pattern to use when data type is set to PATTERN",
    "\r\n",
    "",
    {BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DIAG_VERBOSE_ID,
    "verbose",
    (PARAM_VAL_RULES *)&Diag_empty_vals[0],
    (sizeof(Diag_empty_vals) / sizeof(Diag_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Do more printing.",
    "",
    "",
    {BIT(0) | BIT(1) | BIT(2) | BIT(3)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
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


