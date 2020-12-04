/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __UI_ROM_DEFI_OAM_ACC_INCLUDED__
/* { */
#define __UI_ROM_DEFI_OAM_ACC_INCLUDED__

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <appl/dpp/UserInterface/ui_pure_defi_oam_acc.h>
#include <soc/dpp/oam/oam_api_general.h>
#include <soc/dpp/oam/oam_api_mgmt.h>

#define UI_OAM_ACC_OAM
#define UI_OAM_REG_ACCESS

EXTERN CONST
   PARAM_VAL_RULES
     oam_acc_mac_addr_vals[]
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
        12,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    MAC address parameter. 12 hex digits. No 0 no 0x Prefix",
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
     OAM_ACC_IWRITE_VAL_rule[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
        0 ,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        3,
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
    OAM_ACC_SIZE_ID_rule[]
#ifdef INIT
=
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
          3, 1,
          1,
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
EXTERN CONST
   PARAM_VAL_RULES
     oam_addr_and_data[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
        0 ,0,
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
     oam_acc_free_vals[]
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
     oam_acc_empty_vals[]
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
/********************************************************/
EXTERN CONST
PARAM_VAL_RULES
OAM_ACC_MODULE_ID_rule[]
#ifdef INIT
=
{
  {
    VAL_SYMBOL,
      "oam",
    {
      {
        OAM_INT_ID,
          /*
          * Casting added here just to keep the compiler silent.
          */
          (long)""
          "  OAM_MODULE_ID.OAM_IDR_ID \r\n"
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

/*
* } ENUM RULES
*/
/********************************************************/
/********************************************************/
EXTERN CONST
   PARAM
     Oam_acc_params[]
#ifdef INIT
   =
{
#ifdef UI_OAM_ACC_OAM/* { oam*/
  {
    PARAM_OAM_SET_REPS_FOR_TBL_UNSAFE_OAMP_SET_REPS_FOR_TBL_UNSAFE_ID,
    "set_reps_for_tbl_unsafe",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_set_reps_for_tbl_unsafe nof_reps 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_SET_REPS_FOR_TBL_UNSAFE_OAMP_SET_REPS_FOR_TBL_UNSAFE_NOF_REPS_ID,
    "nof_reps",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  nof_reps:\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_set_reps_for_tbl_unsafe nof_reps 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(3)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_ID,
    "mos_db_tbl_set",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mos_db_tbl_set entry 0 start 0 ffd_rate\r\n"
    "  0 fdi 0 bdi 0 is_ffd 0 cv_ffd_enable 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(4)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mos_db_tbl_set entry 0 start 0 ffd_rate\r\n"
    "  0 fdi 0 bdi 0 is_ffd 0 cv_ffd_enable 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(4)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_TBL_DATA_START_ID,
    "start",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.start:\r\n"
    "  uint32 start\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mos_db_tbl_set entry 0 start 0 ffd_rate\r\n"
    "  0 fdi 0 bdi 0 is_ffd 0 cv_ffd_enable 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_TBL_DATA_FFD_RATE_ID,
    "ffd_rate",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ffd_rate:\r\n"
    "  uint32 ffd_rate\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mos_db_tbl_set entry 0 start 0 ffd_rate\r\n"
    "  0 fdi 0 bdi 0 is_ffd 0 cv_ffd_enable 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_TBL_DATA_FDI_ID,
    "fdi",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.fdi:\r\n"
    "  uint32 fdi\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mos_db_tbl_set entry 0 start 0 ffd_rate\r\n"
    "  0 fdi 0 bdi 0 is_ffd 0 cv_ffd_enable 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_TBL_DATA_BDI_ID,
    "bdi",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.bdi:\r\n"
    "  uint32 bdi\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mos_db_tbl_set entry 0 start 0 ffd_rate\r\n"
    "  0 fdi 0 bdi 0 is_ffd 0 cv_ffd_enable 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_TBL_DATA_CV0_FFD1_ID,
    "is_ffd",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.is_ffd:\r\n"
    "  uint32 is_ffd\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mos_db_tbl_set entry 0 start 0 ffd_rate\r\n"
    "  0 fdi 0 bdi 0 is_ffd 0 cv_ffd_enable 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_TBL_DATA_CV_FFD_ENABLE_ID,
    "cv_ffd_enable",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.cv_ffd_enable:\r\n"
    "  uint32 cv_ffd_enable\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mos_db_tbl_set entry 0 start 0 ffd_rate\r\n"
    "  0 fdi 0 bdi 0 is_ffd 0 cv_ffd_enable 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOS_DB_TBL_GET_OAMP_MOS_DB_TBL_GET_ID,
    "mos_db_tbl_get",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mos_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(5)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOS_DB_TBL_GET_OAMP_MOS_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mos_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(5)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_ID,
    "mor_db_tbl_set",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(6)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_MOT_DB_INDEX_ID,
    "mot_db_index",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mot_db_index:\r\n"
    "  uint32 mot_db_index\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOR_DB_CPU_TBL_SET_OAMP_MOR_DB_CPU_TBL_SET_TBL_DATA_IS_FFD_ID,
    "is_ffd",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mot_db_index:\r\n"
    "  uint32 mot_db_index\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(30)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_CNT_TM_1_ID,
    "cnt_tm_1",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_CNT_TM_2_ID,
    "cnt_tm_2",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_CNT_TM_3_ID,
    "cnt_tm_3",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_ENABLE_ID,
    "enable",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.bdi_state:\r\n"
    "  uint32 bdi_state\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_D_LOCV_STATE_ID,
    "d_locv_state",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.d_locv_state:\r\n"
    "  uint32 d_locv_state\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_D_EXCESS_STATE_ID,
    "d_excess_state",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.d_locv_state:\r\n"
    "  uint32 d_locv_state\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_FFD_RX_RATE_ID,
    "ffd_rx_rate",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ffd_rx_rate:\r\n"
    "  uint32 ffd_rx_rate\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOR_DB_CPU_TBL_GET_OAMP_MOR_DB_CPU_TBL_GET_ID,
    "mor_db_cpu_tbl_get",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_sys_port_tbl_set entry 0 sys_port 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOR_DB_CPU_TBL_GET_OAMP_MOR_DB_CPU_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(31)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOR_DB_CPU_TBL_SET_OAMP_MOR_DB_CPU_TBL_SET_ID,
    "mor_db_cpu_tbl_set",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_sys_port_tbl_set entry 0 sys_port 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(30)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOR_DB_CPU_TBL_SET_OAMP_MOR_DB_CPU_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(30)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOR_DB_CPU_TBL_SET_OAMP_MOR_DB_CPU_TBL_SET_TBL_DATA_LAST_FDI_DEFECT_TYPE_ID,
    "fdi_defect_type",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.is_ffd:\r\n"
    "  uint32 is_ffd\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(30)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  { 
    PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_LAST_FDI_RECEIVE_TIME_ID, 
    "last_fdi_receive_time", 
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0], 
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.last_fdi_receive_time:\r\n"
    "  uint32 last_fdi_receive_time\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE 
    "Examples:\r\n" 
    "  oam_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 cv0_ffd1 0\r\n"
#endif 
    "",
    "",
    {0, 0, 0, 0, 0, BIT(6)}, 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  {
    PARAM_OAM_MOR_DB_CPU_TBL_SET_OAMP_MOR_DB_CPU_TBL_SET_TBL_DATA_LAST_BDI_DEFECT_TYPE_ID,
    "bdi_defect_type",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.is_ffd:\r\n"
    "  uint32 is_ffd\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(30)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOR_DB_CPU_TBL_SET_OAMP_MOR_DB_CPU_TBL_SET_TBL_DATA_ENABLE_ID,
    "enable",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.is_ffd:\r\n"
    "  uint32 is_ffd\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(30)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  { 
    PARAM_OAM_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_LAST_BDI_RECEIVE_TIME_ID, 
    "last_bdi_receive_time", 
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0], 
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tbl_data.last_bdi_receive_time:\r\n"
    "  uint32 last_bdi_receive_time\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE 
    "Examples:\r\n" 
    "  oam_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 cv0_ffd1 0\r\n"
#endif 
    "",
    "",
    {0, 0, 0, 0, 0, BIT(6)}, 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  {
    PARAM_OAM_MOR_DB_TBL_GET_OAMP_MOR_DB_TBL_GET_ID,
    "mor_db_tbl_get",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mor_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(7)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOR_DB_TBL_GET_OAMP_MOR_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mor_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_SYS_PORT_TBL_SET_OAMP_SYS_PORT_TBL_SET_ID,
    "sys_port_tbl_set",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_sys_port_tbl_set entry 0 sys_port 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(8)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_SYS_PORT_TBL_SET_OAMP_SYS_PORT_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_sys_port_tbl_set entry 0 sys_port 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(8)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_SYS_PORT_TBL_SET_OAMP_SYS_PORT_TBL_SET_TBL_DATA_SYS_PORT_ID,
    "sys_port",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.sys_port:\r\n"
    "  uint32 sys_port\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_sys_port_tbl_set entry 0 sys_port 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_SYS_PORT_TBL_GET_OAMP_SYS_PORT_TBL_GET_ID,
    "sys_port_tbl_get",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_sys_port_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(9)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_SYS_PORT_TBL_GET_OAMP_SYS_PORT_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_sys_port_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(9)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_LSR_ID_TBL_SET_OAMP_LSR_ID_TBL_SET_ID,
    "lsr_id_tbl_set",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_lsr_id_tbl_set entry 0 lsr_id_index 0\r\n"
    "  lsr_id 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(10)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_LSR_ID_TBL_SET_OAMP_LSR_ID_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_lsr_id_tbl_set entry 0 lsr_id_index 0\r\n"
    "  lsr_id 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(10)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_LSR_ID_TBL_SET_OAMP_LSR_ID_TBL_SET_TBL_DATA_LSR_ID_INDEX_ID,
    "lsr_id_index",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lsr_id_index:\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_lsr_id_tbl_set entry 0 lsr_id_index 0\r\n"
    "  lsr_id 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_LSR_ID_TBL_SET_OAMP_LSR_ID_TBL_SET_TBL_DATA_LSR_ID_ID,
    "lsr_id",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.lsr_id[ prm_lsr_id_index]:\r\n"
    "  uint32 lsr_id[4]\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_lsr_id_tbl_set entry 0 lsr_id_index 0\r\n"
    "  lsr_id 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_LSR_ID_TBL_GET_OAMP_LSR_ID_TBL_GET_ID,
    "lsr_id_tbl_get",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_lsr_id_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(11)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_LSR_ID_TBL_GET_OAMP_LSR_ID_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_lsr_id_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(11)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_DEFECT_TYPE_TBL_SET_OAMP_DEFECT_TYPE_TBL_SET_ID,
    "defect_type_tbl_set",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_defect_type_tbl_set entry 0 defect_type\r\n"
    "  0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(12)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_DEFECT_TYPE_TBL_SET_OAMP_DEFECT_TYPE_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_defect_type_tbl_set entry 0 defect_type\r\n"
    "  0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(12)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_DEFECT_TYPE_TBL_SET_OAMP_DEFECT_TYPE_TBL_SET_TBL_DATA_DEFECT_TYPE_ID,
    "defect_type",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.defect_type:\r\n"
    "  uint32 defect_type\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_defect_type_tbl_set entry 0 defect_type\r\n"
    "  0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(12)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_DEFECT_TYPE_TBL_GET_OAMP_DEFECT_TYPE_TBL_GET_ID,
    "defect_type_tbl_get",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_defect_type_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(13)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_DEFECT_TYPE_TBL_GET_OAMP_DEFECT_TYPE_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_defect_type_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(13)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_ID,
    "mep_db_tbl_set",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mep_db_tbl_set entry 0 up1down0 0 port\r\n"
    "  0 dm_db_index 0 dm_enable 0 ccm_tx_start 0 maid 0 ccm_interval 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(14)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mep_db_tbl_set entry 0 up1down0 0 port\r\n"
    "  0 dm_db_index 0 dm_enable 0 ccm_tx_start 0 maid 0 ccm_interval 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(14)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_UP1DOWN0_ID,
    "up1down0",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.up1down0:\r\n"
    "  uint32 up1down0\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mep_db_tbl_set entry 0 up1down0 0 port\r\n"
    "  0 dm_db_index 0 dm_enable 0 ccm_tx_start 0 maid 0 ccm_interval 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_PORT_ID,
    "port",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.port:\r\n"
    "  uint32 port\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mep_db_tbl_set entry 0 up1down0 0 port\r\n"
    "  0 dm_db_index 0 dm_enable 0 ccm_tx_start 0 maid 0 ccm_interval 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_DM_DB_INDEX_ID,
    "dm_db_index",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.dm_db_index:\r\n"
    "  uint32 dm_db_index\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mep_db_tbl_set entry 0 up1down0 0 port\r\n"
    "  0 dm_db_index 0 dm_enable 0 ccm_tx_start 0 maid 0 ccm_interval 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_DM_ENABLE_ID,
    "dm_enable",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.dm_enable:\r\n"
    "  uint32 dm_enable\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mep_db_tbl_set entry 0 up1down0 0 port\r\n"
    "  0 dm_db_index 0 dm_enable 0 ccm_tx_start 0 maid 0 ccm_interval 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_CCM_TX_START_ID,
    "ccm_tx_start",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ccm_tx_start:\r\n"
    "  uint32 ccm_tx_start\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mep_db_tbl_set entry 0 up1down0 0 port\r\n"
    "  0 dm_db_index 0 dm_enable 0 ccm_tx_start 0 maid 0 ccm_interval 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_MAID_ID,
    "maid",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.maid:\r\n"
    "  uint32 maid\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mep_db_tbl_set entry 0 up1down0 0 port\r\n"
    "  0 dm_db_index 0 dm_enable 0 ccm_tx_start 0 maid 0 ccm_interval 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_CCM_INTERVAL_ID,
    "ccm_interval",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ccm_interval:\r\n"
    "  uint32 ccm_interval\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mep_db_tbl_set entry 0 up1down0 0 port\r\n"
    "  0 dm_db_index 0 dm_enable 0 ccm_tx_start 0 maid 0 ccm_interval 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MEP_DB_TBL_GET_OAMP_MEP_DB_TBL_GET_ID,
    "mep_db_tbl_get",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mep_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(15)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MEP_DB_TBL_GET_OAMP_MEP_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mep_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(15)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MEP_INDEX_DB_TBL_SET_OAMP_MEP_INDEX_DB_TBL_SET_ID,
    "mep_index_db_tbl_set",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mep_index_db_tbl_set entry 0\r\n"
    "  accelerated 0 mep_db_ptr 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(16)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MEP_INDEX_DB_TBL_SET_OAMP_MEP_INDEX_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mep_index_db_tbl_set entry 0\r\n"
    "  accelerated 0 mep_db_ptr 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(16)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MEP_INDEX_DB_TBL_SET_OAMP_MEP_INDEX_DB_TBL_SET_TBL_DATA_ACCELERATED_ID,
    "accelerated",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.accelerated:\r\n"
    "  uint32 accelerated\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mep_index_db_tbl_set entry 0\r\n"
    "  accelerated 0 mep_db_ptr 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MEP_INDEX_DB_TBL_SET_OAMP_MEP_INDEX_DB_TBL_SET_TBL_DATA_MEP_DB_PTR_ID,
    "mep_db_ptr",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mep_db_ptr:\r\n"
    "  uint32 mep_db_ptr\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mep_index_db_tbl_set entry 0\r\n"
    "  accelerated 0 mep_db_ptr 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MEP_INDEX_DB_TBL_GET_OAMP_MEP_INDEX_DB_TBL_GET_ID,
    "mep_index_db_tbl_get",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mep_index_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(17)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MEP_INDEX_DB_TBL_GET_OAMP_MEP_INDEX_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mep_index_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_ID,
    "rmep_db_tbl_set",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_rmep_db_tbl_set entry 0 rcvd_rdi 0\r\n"
    "  ccmdefect 0 ccminterval 0 last_ccmtime_valid 0 last_ccmtime 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(18)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_rmep_db_tbl_set entry 0 rcvd_rdi 0\r\n"
    "  ccmdefect 0 ccminterval 0 last_ccmtime_valid 0 last_ccmtime 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(18)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_TBL_DATA_RCVD_RDI_ID,
    "rcvd_rdi",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.rcvd_rdi:\r\n"
    "  uint32 rcvd_rdi\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_rmep_db_tbl_set entry 0 rcvd_rdi 0\r\n"
    "  ccmdefect 0 ccminterval 0 last_ccmtime_valid 0 last_ccmtime 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_TBL_DATA_CCMDEFECT_ID,
    "ccmdefect",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ccmdefect:\r\n"
    "  uint32 ccmdefect\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_rmep_db_tbl_set entry 0 rcvd_rdi 0\r\n"
    "  ccmdefect 0 ccminterval 0 last_ccmtime_valid 0 last_ccmtime 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_TBL_DATA_CCMINTERVAL_ID,
    "ccminterval",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ccminterval:\r\n"
    "  uint32 ccminterval\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_rmep_db_tbl_set entry 0 rcvd_rdi 0\r\n"
    "  ccmdefect 0 ccminterval 0 last_ccmtime_valid 0 last_ccmtime 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_TBL_DATA_LAST_CCMTIME_VALID_ID,
    "last_ccmtime_valid",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.last_ccmtime_valid:\r\n"
    "  uint32 last_ccmtime_valid\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_rmep_db_tbl_set entry 0 rcvd_rdi 0\r\n"
    "  ccmdefect 0 ccminterval 0 last_ccmtime_valid 0 last_ccmtime 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_TBL_DATA_LAST_CCMTIME_ID,
    "last_ccmtime",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.last_ccmtime:\r\n"
    "  uint32 last_ccmtime\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_rmep_db_tbl_set entry 0 rcvd_rdi 0\r\n"
    "  ccmdefect 0 ccminterval 0 last_ccmtime_valid 0 last_ccmtime 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_RMEP_DB_TBL_GET_OAMP_RMEP_DB_TBL_GET_ID,
    "rmep_db_tbl_get",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_rmep_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(19)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_RMEP_DB_TBL_GET_OAMP_RMEP_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_rmep_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(19)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_RMEP_HASH_0_DB_TBL_SET_OAMP_RMEP_HASH_0_DB_TBL_SET_ID,
    "rmep_hash_0_db_tbl_set",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_rmep_hash_0_db_tbl_set entry 0\r\n"
    "  valid_hash0 0 verifier0 0 rmep_db_ptr0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(20)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_RMEP_HASH_0_DB_TBL_SET_OAMP_RMEP_HASH_0_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_rmep_hash_0_db_tbl_set entry 0\r\n"
    "  valid_hash0 0 verifier0 0 rmep_db_ptr0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(20)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_RMEP_HASH_0_DB_TBL_SET_OAMP_RMEP_HASH_0_DB_TBL_SET_TBL_DATA_VALID_HASH0_ID,
    "valid_hash0",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.valid_hash0:\r\n"
    "  uint32 valid_hash0\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_rmep_hash_0_db_tbl_set entry 0\r\n"
    "  valid_hash0 0 verifier0 0 rmep_db_ptr0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_RMEP_HASH_0_DB_TBL_SET_OAMP_RMEP_HASH_0_DB_TBL_SET_TBL_DATA_VERIFIER0_ID,
    "verifier0",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.verifier0:\r\n"
    "  uint32 verifier0\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_rmep_hash_0_db_tbl_set entry 0\r\n"
    "  valid_hash0 0 verifier0 0 rmep_db_ptr0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_RMEP_HASH_0_DB_TBL_SET_OAMP_RMEP_HASH_0_DB_TBL_SET_TBL_DATA_RMEP_DB_PTR0_ID,
    "rmep_db_ptr0",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.rmep_db_ptr0:\r\n"
    "  uint32 rmep_db_ptr0\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_rmep_hash_0_db_tbl_set entry 0\r\n"
    "  valid_hash0 0 verifier0 0 rmep_db_ptr0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_RMEP_HASH_0_DB_TBL_GET_OAMP_RMEP_HASH_0_DB_TBL_GET_ID,
    "rmep_hash_0_db_tbl_get",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_rmep_hash_0_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(21)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_RMEP_HASH_0_DB_TBL_GET_OAMP_RMEP_HASH_0_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_rmep_hash_0_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(21)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_RMEP_HASH_1_DB_TBL_SET_OAMP_RMEP_HASH_1_DB_TBL_SET_ID,
    "rmep_hash_1_db_tbl_set",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_rmep_hash_1_db_tbl_set entry 0\r\n"
    "  valid_hash1 0 verifier1 0 rmep_db_ptr1 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(22)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_RMEP_HASH_1_DB_TBL_SET_OAMP_RMEP_HASH_1_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_rmep_hash_1_db_tbl_set entry 0\r\n"
    "  valid_hash1 0 verifier1 0 rmep_db_ptr1 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(22)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_RMEP_HASH_1_DB_TBL_SET_OAMP_RMEP_HASH_1_DB_TBL_SET_TBL_DATA_VALID_HASH1_ID,
    "valid_hash1",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.valid_hash1:\r\n"
    "  uint32 valid_hash1\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_rmep_hash_1_db_tbl_set entry 0\r\n"
    "  valid_hash1 0 verifier1 0 rmep_db_ptr1 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(22)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_RMEP_HASH_1_DB_TBL_SET_OAMP_RMEP_HASH_1_DB_TBL_SET_TBL_DATA_VERIFIER1_ID,
    "verifier1",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.verifier1:\r\n"
    "  uint32 verifier1\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_rmep_hash_1_db_tbl_set entry 0\r\n"
    "  valid_hash1 0 verifier1 0 rmep_db_ptr1 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(22)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_RMEP_HASH_1_DB_TBL_SET_OAMP_RMEP_HASH_1_DB_TBL_SET_TBL_DATA_RMEP_DB_PTR1_ID,
    "rmep_db_ptr1",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.rmep_db_ptr1:\r\n"
    "  uint32 rmep_db_ptr1\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_rmep_hash_1_db_tbl_set entry 0\r\n"
    "  valid_hash1 0 verifier1 0 rmep_db_ptr1 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(22)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_RMEP_HASH_1_DB_TBL_GET_OAMP_RMEP_HASH_1_DB_TBL_GET_ID,
    "rmep_hash_1_db_tbl_get",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_rmep_hash_1_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(23)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_RMEP_HASH_1_DB_TBL_GET_OAMP_RMEP_HASH_1_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_rmep_hash_1_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(23)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_ID,
    "lmdb_tbl_set",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_lmdb_tbl_set entry 0 tx_fcf_peer_p 0\r\n"
    "  tx_fcf_peer_c 0 rx_fcb_p 0 rx_fcb_c 0 rx_fcb_peer_p 0 rx_fcb_peer_c 0\r\n"
    "  tx_fcf_p 0 tx_fcf_c 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(24)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_lmdb_tbl_set entry 0 tx_fcf_peer_p 0\r\n"
    "  tx_fcf_peer_c 0 rx_fcb_p 0 rx_fcb_c 0 rx_fcb_peer_p 0 rx_fcb_peer_c 0\r\n"
    "  tx_fcf_p 0 tx_fcf_c 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(24)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_TX_FCF_PEER_P_ID,
    "tx_fcf_peer_p",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.tx_fcf_peer_p:\r\n"
    "  uint32 tx_fcf_peer_p\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_lmdb_tbl_set entry 0 tx_fcf_peer_p 0\r\n"
    "  tx_fcf_peer_c 0 rx_fcb_p 0 rx_fcb_c 0 rx_fcb_peer_p 0 rx_fcb_peer_c 0\r\n"
    "  tx_fcf_p 0 tx_fcf_c 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(24)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_TX_FCF_PEER_C_ID,
    "tx_fcf_peer_c",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.tx_fcf_peer_c:\r\n"
    "  uint32 tx_fcf_peer_c\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_lmdb_tbl_set entry 0 tx_fcf_peer_p 0\r\n"
    "  tx_fcf_peer_c 0 rx_fcb_p 0 rx_fcb_c 0 rx_fcb_peer_p 0 rx_fcb_peer_c 0\r\n"
    "  tx_fcf_p 0 tx_fcf_c 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(24)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_RX_FCB_P_ID,
    "rx_fcb_p",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.rx_fcb_p:\r\n"
    "  uint32 rx_fcb_p\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_lmdb_tbl_set entry 0 tx_fcf_peer_p 0\r\n"
    "  tx_fcf_peer_c 0 rx_fcb_p 0 rx_fcb_c 0 rx_fcb_peer_p 0 rx_fcb_peer_c 0\r\n"
    "  tx_fcf_p 0 tx_fcf_c 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(24)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_RX_FCB_C_ID,
    "rx_fcb_c",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.rx_fcb_c:\r\n"
    "  uint32 rx_fcb_c\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_lmdb_tbl_set entry 0 tx_fcf_peer_p 0\r\n"
    "  tx_fcf_peer_c 0 rx_fcb_p 0 rx_fcb_c 0 rx_fcb_peer_p 0 rx_fcb_peer_c 0\r\n"
    "  tx_fcf_p 0 tx_fcf_c 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(24)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_RX_FCB_PEER_P_ID,
    "rx_fcb_peer_p",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.rx_fcb_peer_p:\r\n"
    "  uint32 rx_fcb_peer_p\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_lmdb_tbl_set entry 0 tx_fcf_peer_p 0\r\n"
    "  tx_fcf_peer_c 0 rx_fcb_p 0 rx_fcb_c 0 rx_fcb_peer_p 0 rx_fcb_peer_c 0\r\n"
    "  tx_fcf_p 0 tx_fcf_c 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(24)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_RX_FCB_PEER_C_ID,
    "rx_fcb_peer_c",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.rx_fcb_peer_c:\r\n"
    "  uint32 rx_fcb_peer_c\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_lmdb_tbl_set entry 0 tx_fcf_peer_p 0\r\n"
    "  tx_fcf_peer_c 0 rx_fcb_p 0 rx_fcb_c 0 rx_fcb_peer_p 0 rx_fcb_peer_c 0\r\n"
    "  tx_fcf_p 0 tx_fcf_c 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(24)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_TX_FCF_P_ID,
    "tx_fcf_p",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.tx_fcf_p:\r\n"
    "  uint32 tx_fcf_p\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_lmdb_tbl_set entry 0 tx_fcf_peer_p 0\r\n"
    "  tx_fcf_peer_c 0 rx_fcb_p 0 rx_fcb_c 0 rx_fcb_peer_p 0 rx_fcb_peer_c 0\r\n"
    "  tx_fcf_p 0 tx_fcf_c 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(24)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_TX_FCF_C_ID,
    "tx_fcf_c",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.tx_fcf_c:\r\n"
    "  uint32 tx_fcf_c\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_lmdb_tbl_set entry 0 tx_fcf_peer_p 0\r\n"
    "  tx_fcf_peer_c 0 rx_fcb_p 0 rx_fcb_c 0 rx_fcb_peer_p 0 rx_fcb_peer_c 0\r\n"
    "  tx_fcf_p 0 tx_fcf_c 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(24)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_LMDB_TBL_GET_OAMP_LMDB_TBL_GET_ID,
    "lmdb_tbl_get",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_lmdb_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(25)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_LMDB_TBL_GET_OAMP_LMDB_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_lmdb_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(25)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_ID,
    "mot_db_tbl_set",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mot_db_tbl_set entry 0 defect_location\r\n"
    "  0 defect_type_ptr 0 ttl_profile 0 lsp_id_lsb 0 lsr_id_ptr 0 eep 0 cos 0\r\n"
    "  sysport 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(26)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mot_db_tbl_set entry 0 defect_location\r\n"
    "  0 defect_type_ptr 0 ttl_profile 0 lsp_id_lsb 0 lsr_id_ptr 0 eep 0 cos 0\r\n"
    "  sysport 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(26)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_DEFECT_LOCATION_ID,
    "defect_location",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.defect_location:\r\n"
    "  uint32 defect_location\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mot_db_tbl_set entry 0 defect_location\r\n"
    "  0 defect_type_ptr 0 ttl_profile 0 lsp_id_lsb 0 lsr_id_ptr 0 eep 0 cos 0\r\n"
    "  sysport 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_DEFECT_TYPE_PTR_ID,
    "defect_type_ptr",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.defect_type_ptr:\r\n"
    "  uint32 defect_type_ptr\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mot_db_tbl_set entry 0 defect_location\r\n"
    "  0 defect_type_ptr 0 ttl_profile 0 lsp_id_lsb 0 lsr_id_ptr 0 eep 0 cos 0\r\n"
    "  sysport 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_TTL_PROFILE_ID,
    "ttl_profile",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ttl_profile:\r\n"
    "  uint32 ttl_profile\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mot_db_tbl_set entry 0 defect_location\r\n"
    "  0 defect_type_ptr 0 ttl_profile 0 lsp_id_lsb 0 lsr_id_ptr 0 eep 0 cos 0\r\n"
    "  sysport 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_LSP_ID_LSB_ID,
    "lsp_id_lsb",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.lsp_id_lsb:\r\n"
    "  uint32 lsp_id_lsb\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mot_db_tbl_set entry 0 defect_location\r\n"
    "  0 defect_type_ptr 0 ttl_profile 0 lsp_id_lsb 0 lsr_id_ptr 0 eep 0 cos 0\r\n"
    "  sysport 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_LSR_ID_PTR_ID,
    "lsr_id_ptr",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.lsr_id_ptr:\r\n"
    "  uint32 lsr_id_ptr\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mot_db_tbl_set entry 0 defect_location\r\n"
    "  0 defect_type_ptr 0 ttl_profile 0 lsp_id_lsb 0 lsr_id_ptr 0 eep 0 cos 0\r\n"
    "  sysport 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_EEP_ID,
    "eep",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.eep:\r\n"
    "  uint32 eep\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mot_db_tbl_set entry 0 defect_location\r\n"
    "  0 defect_type_ptr 0 ttl_profile 0 lsp_id_lsb 0 lsr_id_ptr 0 eep 0 cos 0\r\n"
    "  sysport 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_CO_S_ID,
    "cos",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.cos:\r\n"
    "  uint32 cos\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mot_db_tbl_set entry 0 defect_location\r\n"
    "  0 defect_type_ptr 0 ttl_profile 0 lsp_id_lsb 0 lsr_id_ptr 0 eep 0 cos 0\r\n"
    "  sysport 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_SYSPORT_ID,
    "sysport",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.sysport:\r\n"
    "  uint32 sysport\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mot_db_tbl_set entry 0 defect_location\r\n"
    "  0 defect_type_ptr 0 ttl_profile 0 lsp_id_lsb 0 lsr_id_ptr 0 eep 0 cos 0\r\n"
    "  sysport 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOT_DB_TBL_GET_OAMP_MOT_DB_TBL_GET_ID,
    "mot_db_tbl_get",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mot_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(27)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MOT_DB_TBL_GET_OAMP_MOT_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_mot_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(27)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_ID,
    "ext_mep_db_tbl_set",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_ext_mep_db_tbl_set entry 0 lmenable 0\r\n"
    "  priority 0 vid 0 md_level 0 mepid 0 rdi_indicator 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(28)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_ext_mep_db_tbl_set entry 0 lmenable 0\r\n"
    "  priority 0 vid 0 md_level 0 mepid 0 rdi_indicator 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(28)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_TBL_DATA_LMENABLE_ID,
    "lmenable",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.lmenable:\r\n"
    "  uint32 lmenable\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_ext_mep_db_tbl_set entry 0 lmenable 0\r\n"
    "  priority 0 vid 0 md_level 0 mepid 0 rdi_indicator 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_TBL_DATA_PRIORITY_ID,
    "priority",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.priority:\r\n"
    "  uint32 priority\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_ext_mep_db_tbl_set entry 0 lmenable 0\r\n"
    "  priority 0 vid 0 md_level 0 mepid 0 rdi_indicator 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_TBL_DATA_VID_ID,
    "vid",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.vid:\r\n"
    "  uint32 vid\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_ext_mep_db_tbl_set entry 0 lmenable 0\r\n"
    "  priority 0 vid 0 md_level 0 mepid 0 rdi_indicator 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_TBL_DATA_MD_LEVEL_ID,
    "md_level",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.md_level:\r\n"
    "  uint32 md_level\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_ext_mep_db_tbl_set entry 0 lmenable 0\r\n"
    "  priority 0 vid 0 md_level 0 mepid 0 rdi_indicator 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_TBL_DATA_MEPID_ID,
    "mepid",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mepid:\r\n"
    "  uint32 mepid\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_ext_mep_db_tbl_set entry 0 lmenable 0\r\n"
    "  priority 0 vid 0 md_level 0 mepid 0 rdi_indicator 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_TBL_DATA_RDI_INDICATOR_ID,
    "rdi_indicator",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.rdi_indicator:\r\n"
    "  uint32 rdi_indicator\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_ext_mep_db_tbl_set entry 0 lmenable 0\r\n"
    "  priority 0 vid 0 md_level 0 mepid 0 rdi_indicator 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_EXT_MEP_DB_TBL_GET_OAMP_EXT_MEP_DB_TBL_GET_ID,
    "ext_mep_db_tbl_get",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_ext_mep_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(29)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_EXT_MEP_DB_TBL_GET_OAMP_EXT_MEP_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc oam oamp_ext_mep_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(29)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_ACC_OAM_ID,
    "oam",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, 0, 0, 0, BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(8)|BIT(9)|BIT(10)|BIT(11)|BIT(12)|BIT(13)|BIT(14)|BIT(15)|BIT(16)|BIT(17)|BIT(18)|BIT(19)|BIT(20)|BIT(21)|BIT(22)|BIT(23)|BIT(24)|BIT(25)|BIT(26)|BIT(27)|BIT(28)|BIT(29)|BIT(30)|BIT(31)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } oam*/

#ifdef UI_OAM_REG_ACCESS/* { reg_access*/
  {
    PARAM_OAM_READ_FLD_READ_FLD_ID,
    "read_fld",
    (PARAM_VAL_RULES *)&oam_addr_and_data[0],
    (sizeof(oam_addr_and_data) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Read a field from oam register\r\n"
    "",
    "Examples:\r\n"
    "  oam_acc reg_access read_fld 0x100 lsb 0 msb 0 \r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_READ_FLD_READ_FLD_FIELD_LSB_ID,
    "lsb",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  field.lsb:\r\n"
    "  Field Least Significant Bit in the register.\r\n"
    "",
#ifdef UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc reg_access read_fld lsb 0 msb 0 step 0 base 0 instance_idx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_READ_FLD_READ_FLD_FIELD_MSB_ID,
    "msb",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  field.msb:\r\n"
    "  Field Most Significant Bit in the register.\r\n"
    "",
#ifdef UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc reg_access read_fld lsb 0 msb 0 step 0 base 0 instance_idx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_WRITE_FLD_WRITE_FLD_ID,
    "write_fld",
    (PARAM_VAL_RULES *)&oam_addr_and_data[0],
    (sizeof(oam_addr_and_data) / sizeof(oam_addr_and_data[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Write a field from oam register\r\n"
    "",
    "Examples:\r\n"
    "  oam_acc reg_access write_fld  0x100 lsb 0 msb 0 val 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_WRITE_FLD_WRITE_FLD_FIELD_LSB_ID,
    "lsb",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  field.lsb:\r\n"
    "  Field Least Significant Bit in the register.\r\n"
    "",
#ifdef UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc reg_access write_fld lsb 0 msb 0 step 0 base 0 instance_idx 0 val 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_WRITE_FLD_WRITE_FLD_FIELD_MSB_ID,
    "msb",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  field.msb:\r\n"
    "  Field Most Significant Bit in the register.\r\n"
    "",
#ifdef UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc reg_access write_fld lsb 0 msb 0 step 0 base 0 instance_idx 0 val 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_WRITE_FLD_WRITE_FLD_VAL_ID,
    "val",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  val:\r\n"
    "  field value to write.\r\n"
    "",
#ifdef UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc reg_access write_fld lsb 0 msb 0 step 0 base 0 instance_idx 0 val 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_READ_REG_READ_REG_ID,
    "read_reg",
    (PARAM_VAL_RULES *)&oam_addr_and_data[0],
    (sizeof(oam_addr_and_data) / sizeof(oam_addr_and_data[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Read a register from oam register\r\n"
    "",
    "Examples:\r\n"
    "  oam_acc reg_access 0x3400\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(2)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_WRITE_REG_WRITE_REG_ID,
    "write_reg",
    (PARAM_VAL_RULES *)&oam_addr_and_data[0],
    (sizeof(oam_addr_and_data) / sizeof(oam_addr_and_data[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Write a register from oam register\r\n"
    "",
    "Examples:\r\n"
    "  oam_acc reg_access write_reg 0x3400 val 0x100\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_WRITE_REG_WRITE_REG_VAL_ID,
    "val",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  val:\r\n"
    "  register value to write.\r\n"
    "",
#ifdef UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc reg_access write_reg step 0 base 0 instance_idx 0 val 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(3)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_IWRITE_ID,
    "iwrite",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Write Data (MSB) to indirect Addr\r\n"
    "",
    "Examples:\r\n"
    "  oam_acc general iwrite 0x10045 0x4 0x4 0x2\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(4)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_IREAD_ID,
    "iread",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Read data (MSB) from indirect addr\r\n"
    "",
    "Examples:\r\n"
    "  oam_acc general iread 0x10045\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(5)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_IRW_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&oam_addr_and_data[0],
    (sizeof(oam_addr_and_data) / sizeof(oam_addr_and_data[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Read data (MSB) from indirect addr\r\n"
    "",
    "Examples:\r\n"
    "  oam_acc general iread 0x10045\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(4)|BIT(5)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_IACC_MODULE_ID_ID,
    "module_id",
    (PARAM_VAL_RULES *)&OAM_ACC_MODULE_ID_rule[0],
    (sizeof(OAM_ACC_MODULE_ID_rule) / sizeof(OAM_ACC_MODULE_ID_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  oam block (module) to access. Range: 0 - 20\r\n"
    "OLP:   0   \r\n"
    "IRE:   1   \r\n"
    "IDR:   2   \r\n"
    "IRR:   3   \r\n"
    "IHP:   4   \r\n"
    "QDR:   5   \r\n"
    "IPS:   6   \r\n"
    "IPT:   7   \r\n"
    "DPI_A: 8   \r\n"
    "DPI_B: 9   \r\n"
    "DPI_C: 10  \r\n"
    "DPI_D: 11  \r\n"
    "DPI_E: 12  \r\n"
    "DPI_F: 13  \r\n"
    "RTP:   14  \r\n"
    "EGQ:   15  \r\n"
    "SCH:   16  \r\n"
    "CFC:   17  \r\n"
    "EPNI:  18  \r\n"
    "IQM:   19  \r\n"
    "MMU:   20  \r\n"
    "",
    "Examples:\r\n"
    "  oam_acc general iread 0x10045 size 1 module_id 4\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(4)|BIT(5)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_SIZE_ID,
    "size",
    (PARAM_VAL_RULES *)&OAM_ACC_SIZE_ID_rule[0],
    (sizeof(OAM_ACC_SIZE_ID_rule) / sizeof(OAM_ACC_SIZE_ID_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  table size 1-3\r\n"
    "",
    "Examples:\r\n"
    "  oam_acc general iread 0x10045 size 1\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(4)|BIT(5)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_ACC_IWRITE_VAL_ID,
    "val",
    (PARAM_VAL_RULES *)&OAM_ACC_IWRITE_VAL_rule[0],
    (sizeof(OAM_ACC_IWRITE_VAL_rule) / sizeof(OAM_ACC_IWRITE_VAL_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  val:\r\n"
    "  register value to write.\r\n"
    "",
#ifdef UI_OAM_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_acc reg_access write_reg step 0 base 0 instance_idx 0 val 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(4)},
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_DO_TWICE_ID,
    "do_twice",
    (PARAM_VAL_RULES *)&oam_acc_free_vals[0],
    (sizeof(oam_acc_free_vals) / sizeof(oam_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Perform twice and wait in milliseconds\r\n"
    "",
    "Examples:\r\n"
    "  oam_acc general iread 0x10045 size 1 do_twice 1000\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(0)|BIT(4)|BIT(5)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_ACC_REG_ACCESS_ID,
    "access",
    (PARAM_VAL_RULES *)&oam_acc_empty_vals[0],
    (sizeof(oam_acc_empty_vals) / sizeof(oam_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } reg_access*/
/*
 * Last element. Do not remove.
 */
  {
    PARAM_END_OF_LIST
  }
}
#endif
;
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __UI_ROM_DEFI_OAM_ACC_INCLUDED__*/
#endif
