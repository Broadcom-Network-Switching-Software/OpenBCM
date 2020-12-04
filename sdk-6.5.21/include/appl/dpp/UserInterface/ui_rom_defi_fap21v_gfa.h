/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UI_ROM_DEFI_FAP21V_GFA_INCLUDED__
/* { */
#define __UI_ROM_DEFI_FAP21V_GFA_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
  /*
   * On MS-Windows platform this attribute is not defined.
   */
  #define __ATTRIBUTE_PACKED__
  #pragma pack(push)
  #pragma pack(1)

#elif __GNUC__
    /*
     * GNUC packing attribute
     */
     #define __ATTRIBUTE_PACKED__  __attribute__ ((packed))
#else
     #error  "Add your system support for packed attribute."
#endif

#include <appl/dpp/UserInterface/ui_pure_defi_fap21v_gfa.h>

/********************************************************/

EXTERN CONST
   PARAM_VAL_RULES
     Fap21v_gfa_ddr_offset_vals[]
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
        8,
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
EXTERN CONST
   PARAM_VAL_RULES
     Fap21v_gfa_free_vals[]
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
EXTERN CONST
   PARAM_VAL_RULES
     Fap21v_gfa_empty_vals[]
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
EXTERN CONST
   PARAM_VAL_RULES
     Fap21v_gfa_iddr_data_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "ddr_x_2",
    {
      {
        2,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        ""
        ""
        "",
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
    "ddr_x_4",
    {
      {
        4,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        ""
        ""
        "",
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

/********************************************************/

EXTERN CONST
   PARAM
     fap21v_gfa_params[]
#ifdef INIT
   =
{
  {
    PARAM_FAP21V_GFA_MB_FGPA_DOWNLOAD_ID,
    "download",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_MB_FPGA_ID,
    "fpga",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
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
    PARAM_FAP21V_GFA_MB_FLASH_DOWNLOAD_ID,
    "download",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
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
    PARAM_FAP21V_GFA_MB_FLASH_ERASE_ID,
    "erase",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
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
    PARAM_FAP21V_GFA_MB_FLASH_STATUS_ID,
    "status",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
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
    PARAM_FAP21V_GFA_MB_FLASH_BURN_FPGA_ID,
    "burn_fpga",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
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
    PARAM_FAP21V_GFA_MB_FLASH_ID,
    "flash",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
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
    PARAM_FAP21V_GFA_MB_ID,
    "mb",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(0)|BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_RESET_DOWN_MILISEC_ID,
    "reset_down_milisec",
    (PARAM_VAL_RULES *)&Fap21v_gfa_free_vals[0],
    (sizeof(Fap21v_gfa_free_vals) / sizeof(Fap21v_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_RESET_STAY_IN_RESET_ID,
    "stay_in_reset",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_RESET_ID,
    "reset",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(10)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_INQ_TEST_RW_ID,
    "inq_test_rw",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
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
    PARAM_FAP21V_GFA_GENERAL_IDDR_TEST_RW_DATA_TO_TEST_ID,
    "data_to_test",
    (PARAM_VAL_RULES *)&Fap21v_gfa_ddr_offset_vals[0],
    (sizeof(Fap21v_gfa_ddr_offset_vals) / sizeof(Fap21v_gfa_ddr_offset_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(3)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_IDDR_TEST_RW_NOF_ITERATION_ID,
    "nof_iteration",
    (PARAM_VAL_RULES *)&Fap21v_gfa_free_vals[0],
    (sizeof(Fap21v_gfa_free_vals) / sizeof(Fap21v_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(3)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_IDDR_TEST_RW_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&Fap21v_gfa_free_vals[0],
    (sizeof(Fap21v_gfa_free_vals) / sizeof(Fap21v_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(3)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_IDDR_TEST_RW_ID,
    "iddr_test_rw",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
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
    PARAM_FAP21V_GFA_GENERAL_CPU_TEST_RW_ID,
    "cpu_test_rw",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
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
    PARAM_FAP21V_GFA_GENERAL_CALIBRATE_BIST_FULL_ALL_OFFSET_ID,
    "offset_correct",
    (PARAM_VAL_RULES *)&Fap21v_gfa_free_vals[0],
    (sizeof(Fap21v_gfa_free_vals) / sizeof(Fap21v_gfa_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(7)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_CALIBRATE_BIST_ID,
    "bist",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_CALIBRATE_LBG_ID,
    "lbg",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_CALIBRATE_FULL_ALL_ID,
    "calibration",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
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
    PARAM_FAP21V_GFA_GENERAL_CALIBRATE_DELETE_FILE_ID,
    "delete_calibration_file",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(11)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_GENERAL_ID,
    "general",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(10)|BIT(11)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_SCREENING_FTG_TEST_ID,
    "ftg_test",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(8)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_SCREENING_INTERFACES_ID,
    "interfaces",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(8)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_SCREENING_PRINT_MODE_ID,
    "print_mode",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(8)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_SCREENING_ID,
    "screening",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(8)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_GFA_STATUS_ID,
    "status",
    (PARAM_VAL_RULES *)&Fap21v_gfa_empty_vals[0],
    (sizeof(Fap21v_gfa_empty_vals) / sizeof(Fap21v_gfa_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    ""
    "",
    ""
    ""
    "",
    "",
    {BIT(9)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /*
   * Last element. Do not remove.  BIT(11)
   */
  {
    PARAM_END_OF_LIST
  }
};

#endif    /* } INIT*/

#ifdef _MSC_VER
  #pragma pack(pop)
#endif

#ifdef  __cplusplus
}
#endif

#endif    /* } __UI_ROM_DEFI_FAP21V_GFA_INCLUDED__*/
