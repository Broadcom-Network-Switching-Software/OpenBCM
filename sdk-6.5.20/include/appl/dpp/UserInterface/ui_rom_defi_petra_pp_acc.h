/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UI_ROM_DEFI_PETRA_PP_ACC_INCLUDED__
/* { */
#define __UI_ROM_DEFI_PETRA_PP_ACC_INCLUDED__

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <appl/dpp/UserInterface/ui_pure_defi_petra_pp_acc.h>
#define UI_PETRA_PP_IHP_API
#define UI_PETRA_PP_EGQ_API
#define UI_PETRA_PP_EPNI_API
 /*
  * ENUM RULES
  * {
  */
 /*
  * } ENUM RULES
  */
/********************************************************/
/********************************************************/
EXTERN CONST
   PARAM_VAL_RULES
     soc_petra_pp_acc_free_vals[]
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
     soc_petra_pp_acc_empty_vals[]
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
     soc_petra_pp_acc_members_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
        0xffffffff,
        0,
        5,
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
/********************************************************/
EXTERN CONST
   PARAM
     soc_petra_pp_acc_params[]
#ifdef INIT
   =
{
#ifdef UI_DEFAULT_SECTION/* { default_section*/
#endif /* } default_section*/
#ifdef UI_PETRA_PP_IHP_API/* { ihp*/
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_ID,
    "management_requests_fifo_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(0), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_SELF_ID,
    "self",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(0), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_REFRESHED_BY_DSP_ID,
    "refreshed_by_dsp",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(0), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_AGE_STATUS_ID,
    "age_status",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(0), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_PAYLOAD_ID,
    "payload",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(0), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_MAC_0_ID,
    "mac_0",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(0), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_CID_OR_FID_ID,
    "cid_or_fid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(0), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_IS_FID_ID,
    "is_fid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(0), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_STAMP_ID,
    "stamp",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(0), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_SET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_DATA_TYPE_ID,
    "type",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(0), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_GET_IHP_MANAGEMENT_REQUESTS_FIFO_TBL_GET_ID,
    "management_requests_fifo_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(1), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_PORT_INFO_TBL_SET_IHP_PORT_INFO_TBL_SET_ID,
    "port_info_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(2), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_PORT_INFO_TBL_SET_IHP_PORT_INFO_TBL_SET_IHP_PORT_INFO_TBL_DATA_PORT_MAP_ID,
    "port_map",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(2), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_PORT_INFO_TBL_SET_IHP_PORT_INFO_TBL_SET_IHP_PORT_INFO_TBL_DATA_SVLAN_TPID_VALID_ID,
    "svlan_tpid_valid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(2), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_PORT_INFO_TBL_SET_IHP_PORT_INFO_TBL_SET_IHP_PORT_INFO_TBL_DATA_CVLAN_TPID_VALID_ID,
    "cvlan_tpid_valid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(2), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_PORT_INFO_TBL_GET_IHP_PORT_INFO_TBL_GET_ID,
    "port_info_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(3), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_ID,
    "eth_port_info1_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(4), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_ENABLE_TUNNELED_LB_ID,
    "enable_tunneled_lb",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(4), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_SA_NOT_FOUND_FWD_ACTION_ID,
    "sa_not_found_fwd_action",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(4), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_DA_NOT_FOUND_FWD_ACTION_ID,
    "da_not_found_fwd_action",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(4), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_CVLAN_UP_ID,
    "cvlan_up",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(4), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_PVID_ID,
    "pvid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(4), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_CEP_PVID_ID,
    "cep_pvid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(4), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_CID_PROTOCOL_ENABLE_ID,
    "cid_protocol_enable",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(4), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_CID_SUBNET_ENABLE_ID,
    "cid_subnet_enable",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(4), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_SET_IHP_ETH_PORT_INFO1_TBL_DATA_PORT_TYPE_ID,
    "port_type",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(4), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO1_TBL_GET_IHP_ETH_PORT_INFO1_TBL_GET_ID,
    "eth_port_info1_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(5), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_ID,
    "eth_port_info2_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_ENABLE_IPV6_ROUTING_ID,
    "enable_ipv6_routing",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_ENABLE_IPV4_ROUTING_ID,
    "enable_ipv4_routing",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_NON_AUTHORIZED_MODE8021X_ID,
    "non_authorized_mode8021x",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_NO_INTERCEPT_OR_SNOOP_ID,
    "no_intercept_or_snoop",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_NO_CONTROL_ID,
    "no_control",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_ACCEPT_UNTAGGED_PRIORITY_ID,
    "accept_untagged_priority",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_ACCEPT_TAGGED_ID,
    "accept_tagged",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_ACL_PORT_ID,
    "acl_port",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PEP_DEI_ID,
    "pep_dei",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_DROP_PRECEDENCE_PORT_SELECT_ID,
    "drop_precedence_port_select",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_TC_L4_PROTOCOL_ENABLE_ID,
    "tc_l4_protocol_enable",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_TC_TOS_INDEX_ID,
    "tc_tos_index",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_TC_TOS_ENABLE_ID,
    "tc_tos_enable",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_TC_UP_OVERRIDE_ID,
    "tc_up_override",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_UP_OVERRIDE_ID,
    "up_override",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_UP_TO_TC_TABLE_INDEX_ID,
    "up_to_tc_table_index",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PCP_DECODING_TABLE_INDEX_ID,
    "pcp_decoding_table_index",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_USE_DEI_ID,
    "use_dei",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PEP_TX_TAGGED_ID,
    "pep_tx_tagged",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PEP_PVID_ID,
    "pep_pvid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PROG_PORT_DEFAULT_ID,
    "prog_port_default",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PEP_PORT_MEMBER_OF_OTHERS_ID,
    "pep_port_member_of_others",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PEP_PORT_MEMBER_OF_PEP_PVID_ID,
    "pep_port_member_of_pep_pvid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PEP_PORT_MEMBER_OF_CEP_PVID_ID,
    "pep_port_member_of_cep_pvid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_CEP_PORT_MEMBER_OF_OTHERS_ID,
    "cep_port_member_of_others",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_CEP_PORT_MEMBER_OF_PEP_PVID_ID,
    "cep_port_member_of_pep_pvid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_CEP_PORT_MEMBER_OF_CEP_PVID_ID,
    "cep_port_member_of_cep_pvid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PEP_GENERATE_PRIORITY_STAG_ID,
    "pep_generate_priority_stag",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_PEP_EGRESS_FILTER_ENABLE_ID,
    "pep_egress_filter_enable",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_CEP_ACCEPT_UNTAGGED_PRIORITY_ID,
    "cep_accept_untagged_priority",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_CEP_ACCEPT_TAGGED_ID,
    "cep_accept_tagged",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_SET_IHP_ETH_PORT_INFO2_TBL_DATA_CEP_INGRESS_FILTER_ENABLE_ID,
    "cep_ingress_filter_enable",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_ETH_PORT_INFO2_TBL_GET_IHP_ETH_PORT_INFO2_TBL_GET_ID,
    "eth_port_info2_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(7), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_ID,
    "cid_info_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(8), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_DATA_ENABLE_L3_LAG_HASH_ID,
    "enable_l3_lag_hash",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(8), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_DATA_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(8), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_DATA_DISABLE_MC_ROUTING_ID,
    "disable_mc_routing",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(8), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_DATA_DISABLE_UC_ROUTING_ID,
    "disable_uc_routing",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(8), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_DATA_INGRESS_FILTER_ENABLE_ID,
    "ingress_filter_enable",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(8), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_DATA_DA_NOT_FOUND_MC_FWD_ACTION_ID,
    "da_not_found_mc_fwd_action",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(8), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_DATA_DA_NOT_FOUND_UC_FWD_ACTION_ID,
    "da_not_found_uc_fwd_action",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(8), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_SET_IHP_CID_INFO_TBL_DATA_CID_DEFAULT_TC_ID,
    "cid_default_tc",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(8), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_INFO_TBL_GET_IHP_CID_INFO_TBL_GET_ID,
    "cid_info_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(9), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_TO_TOPOLOGY_TBL_SET_IHP_CID_TO_TOPOLOGY_TBL_SET_ID,
    "cid_to_topology_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(10), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_TO_TOPOLOGY_TBL_SET_IHP_CID_TO_TOPOLOGY_TBL_SET_IHP_CID_TO_TOPOLOGY_TBL_DATA_CID_TO_TOPOLOGY_ID,
    "cid_to_topology",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(10), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_TO_TOPOLOGY_TBL_GET_IHP_CID_TO_TOPOLOGY_TBL_GET_ID,
    "cid_to_topology_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(11), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_STP_TABLE_TBL_SET_IHP_STP_TABLE_TBL_SET_ID,
    "stp_table_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(12), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_STP_TABLE_TBL_SET_IHP_STP_TABLE_TBL_SET_IHP_STP_TABLE_TBL_DATA_STP_TABLE_ID,
    "stp_table",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(12), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_STP_TABLE_TBL_GET_IHP_STP_TABLE_TBL_GET_ID,
    "stp_table_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(13), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_SET_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_SET_ID,
    "port_and_protocol_to_cid_index_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(14), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_SET_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_SET_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_DATA_PORT_AND_PROTOCOL_TO_CID_INDEX_0_ID,
    "port_and_protocol_to_cid_index_0",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(14), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_GET_IHP_PORT_AND_PROTOCOL_TO_CID_INDEX_TBL_GET_ID,
    "port_and_protocol_to_cid_index_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(15), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_SET_IHP_CID_INDEX_TO_CID_TBL_SET_ID,
    "cid_index_to_cid_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(16), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_SET_IHP_CID_INDEX_TO_CID_TBL_SET_IHP_CID_INDEX_TO_CID_TBL_DATA_CVLAN_UP_ID,
    "cvlan_up",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(16), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_SET_IHP_CID_INDEX_TO_CID_TBL_SET_IHP_CID_INDEX_TO_CID_TBL_DATA_CID_ID,
    "cid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(16), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_SET_IHP_CID_INDEX_TO_CID_TBL_SET_IHP_CID_INDEX_TO_CID_TBL_DATA_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(16), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_INDEX_TO_CID_TBL_GET_IHP_CID_INDEX_TO_CID_TBL_GET_ID,
    "cid_index_to_cid_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(17), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_ID,
    "bridge_control_to_forwarding_params_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(18), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA_SNOOP_ID,
    "snoop",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(18), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA_POLICER_ID,
    "policer",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(18), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA_COS_ID,
    "cos",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(18), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA_COS_OVERWRITE_ID,
    "cos_overwrite",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(18), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA_DESTINATION_INDEX_ID,
    "destination_index",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(18), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_SET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_DATA_TYPE_ID,
    "type",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(18), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_GET_IHP_BRIDGE_CONTROL_TO_FORWARDING_PARAMS_TBL_GET_ID,
    "bridge_control_to_forwarding_params_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(19), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_ID,
    "cid_da_not_found_fwd_action_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(20), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_ADD_CID_TO_DEST_ID,
    "add_cid_to_dest",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(20), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_POLICER_ID,
    "policer",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(20), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_CPU_CODE_ID,
    "cpu_code",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(20), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_COS_OVERWRITE_ID,
    "cos_overwrite",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(20), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_COS_ID,
    "cos",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(20), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_DESTINATION_ID,
    "destination",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(20), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_DATA_TYPE_ID,
    "type",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(20), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_GET_IHP_CID_DA_NOT_FOUND_FWD_ACTION_TBL_GET_ID,
    "cid_da_not_found_fwd_action_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(21), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_SET_IHP_CID_PORT_MEMBERSHIP_TBL_SET_ID,
    "cid_port_membership_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(22), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_SET_IHP_CID_PORT_MEMBERSHIP_TBL_SET_IHP_CID_PORT_MEMBERSHIP_TBL_DATA_CID_PORT_MEMBERSHIP_2_ID,
    "cid_port_membership_2",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(22), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_SET_IHP_CID_PORT_MEMBERSHIP_TBL_SET_IHP_CID_PORT_MEMBERSHIP_TBL_DATA_CID_PORT_MEMBERSHIP_1_ID,
    "cid_port_membership_1",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(22), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_SET_IHP_CID_PORT_MEMBERSHIP_TBL_SET_IHP_CID_PORT_MEMBERSHIP_TBL_DATA_CID_PORT_MEMBERSHIP_0_ID,
    "cid_port_membership_0",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(22), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_PORT_MEMBERSHIP_TBL_GET_IHP_CID_PORT_MEMBERSHIP_TBL_GET_ID,
    "cid_port_membership_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(23), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_IS_CID_SHARED_TBL_SET_IHP_IS_CID_SHARED_TBL_SET_ID,
    "is_cid_shared_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(24), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_IS_CID_SHARED_TBL_SET_IHP_IS_CID_SHARED_TBL_SET_IHP_IS_CID_SHARED_TBL_DATA_IS_CID_SHARED_ID,
    "is_cid_shared",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(24), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_IS_CID_SHARED_TBL_GET_IHP_IS_CID_SHARED_TBL_GET_ID,
    "is_cid_shared_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(25), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TOS_TO_TC_TBL_SET_IHP_TOS_TO_TC_TBL_SET_ID,
    "tos_to_tc_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(26), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TOS_TO_TC_TBL_SET_IHP_TOS_TO_TC_TBL_SET_IHP_TOS_TO_TC_TBL_DATA_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(26), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TOS_TO_TC_TBL_SET_IHP_TOS_TO_TC_TBL_SET_IHP_TOS_TO_TC_TBL_DATA_TRAFFIC_CLASS_ID,
    "traffic_class",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(26), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TOS_TO_TC_TBL_GET_IHP_TOS_TO_TC_TBL_GET_ID,
    "tos_to_tc_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(27), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_ID,
    "event_fifo_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(28), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_FAP_PORT_ID,
    "fap_port",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(28), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_SA_DROP_ID,
    "sa_drop",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(28), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_DA_FWD_TYPE_ID,
    "da_fwd_type",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(28), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_DESTINATION_ID,
    "destination",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(28), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_IS_FID_ID,
    "is_fid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(28), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_CID_OR_FID_ID,
    "cid_or_fid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(28), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_MAC_0_ID,
    "mac_0",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(28), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_STAMP_ID,
    "stamp",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(28), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_SET_IHP_EVENT_FIFO_TBL_DATA_TYPE_ID,
    "type",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(28), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_EVENT_FIFO_TBL_GET_IHP_EVENT_FIFO_TBL_GET_ID,
    "event_fifo_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(29), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_TABLE_TBL_SET_IHP_FEC_TABLE_TBL_SET_ID,
    "fec_table_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(30), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_TABLE_TBL_SET_IHP_FEC_TABLE_TBL_SET_IHP_FEC_TABLE_TBL_DATA_FEC_TABLE_0_ID,
    "fec_table_0",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(30), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_TABLE_TBL_SET_IHP_FEC_TABLE_TBL_SET_IHP_FEC_TABLE_TBL_DATA_FEC_TABLE_1_ID,
    "fec_table_1",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(30), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_TABLE_TBL_GET_IHP_FEC_TABLE_TBL_GET_ID,
    "fec_table_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(31), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_ID,
    "fec_uc_table_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(0), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_DATA_RPF_ENABLE_ID,
    "rpf_enable",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(0), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_DATA_CPU_CODE_ID,
    "cpu_code",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(0), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_DATA_ARP_POINTER_ID,
    "arp_pointer",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(0), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_DATA_OUT_VID_ID,
    "out_vid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(0), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_DATA_COUNTER_POINTER_ID,
    "counter_pointer",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(0), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_DATA_TOS_BASED_COS_ID,
    "tos_based_cos",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(0), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_DATA_DESTINATION_ID,
    "destination",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(0), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_SET_IHP_FEC_UC_TABLE_TBL_DATA_COMMAND_ID,
    "command",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(0), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_UC_TABLE_TBL_GET_IHP_FEC_UC_TABLE_TBL_GET_ID,
    "fec_uc_table_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(1), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_SET_ID,
    "fec_mc_table_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(2), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_DATA_CPU_CODE_ID,
    "cpu_code",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(2), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_DATA_EXPECTED_IN_PORT_ID,
    "expected_in_port",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(2), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_DATA_EXPECTED_IN_VID_ID,
    "expected_in_vid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(2), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_DATA_COUNTER_POINTER_ID,
    "counter_pointer",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(2), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_DATA_TOS_BASED_COS_ID,
    "tos_based_cos",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(2), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_DATA_DESTINATION_ID,
    "destination",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(2), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_SET_IHP_FEC_MC_TABLE_TBL_DATA_COMMAND_ID,
    "command",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(2), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_MC_TABLE_TBL_GET_IHP_FEC_MC_TABLE_TBL_GET_ID,
    "fec_mc_table_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(3), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_SET_IHP_FEC_PROG_PORT_TABLE_TBL_SET_ID,
    "fec_prog_port_table_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(4), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_SET_IHP_FEC_PROG_PORT_TABLE_TBL_SET_IHP_FEC_PROG_PORT_TABLE_TBL_DATA_DESTINATION_ID,
    "destination",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(4), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_SET_IHP_FEC_PROG_PORT_TABLE_TBL_SET_IHP_FEC_PROG_PORT_TABLE_TBL_DATA_COS_KEY_ID,
    "cos_key",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(4), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_SET_IHP_FEC_PROG_PORT_TABLE_TBL_SET_IHP_FEC_PROG_PORT_TABLE_TBL_DATA_SNOOP_COMMAND_ID,
    "snoop_command",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(4), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_PROG_PORT_TABLE_TBL_GET_IHP_FEC_PROG_PORT_TABLE_TBL_GET_ID,
    "fec_prog_port_table_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(5), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_SMOOTH_DIVISION_TBL_SET_IHP_SMOOTH_DIVISION_TBL_SET_ID,
    "smooth_division_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(6), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_SMOOTH_DIVISION_TBL_SET_IHP_SMOOTH_DIVISION_TBL_SET_IHP_SMOOTH_DIVISION_TBL_DATA_SMOOTH_DIVISION_ID,
    "smooth_division",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(6), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_SMOOTH_DIVISION_TBL_GET_IHP_SMOOTH_DIVISION_TBL_GET_ID,
    "smooth_division_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(7), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_SET_IHP_LONGEST_PREFIX_MATCH_TBL_SET_ID,
    "longest_prefix_match_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(8), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_SET_IHP_LONGEST_PREFIX_MATCH_TBL_SET_TBL_NDX_ID,
    "tbl_ndx",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint8\r\n"
    "  tbl_ndx\r\n"
    "",
    "",
    "",
    {0, BIT(8), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_SET_IHP_LONGEST_PREFIX_MATCH_TBL_SET_IHP_LONGEST_PREFIX_MATCH_TBL_DATA_LONGEST_PREFIX_MATCH_ID,
    "longest_prefix_match",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(8), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_GET_IHP_LONGEST_PREFIX_MATCH_TBL_GET_ID,
    "longest_prefix_match_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(9), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH_TBL_GET_IHP_LONGEST_PREFIX_MATCH_TBL_GET_TBL_NDX_ID,
    "tbl_ndx",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint8\r\n"
    "  tbl_ndx\r\n"
    "",
    "",
    "",
    {0, BIT(9), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH0_TBL_SET_IHP_LONGEST_PREFIX_MATCH0_TBL_SET_ID,
    "longest_prefix_match0_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(10), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH0_TBL_SET_IHP_LONGEST_PREFIX_MATCH0_TBL_SET_IHP_LONGEST_PREFIX_MATCH0_TBL_DATA_LONGEST_PREFIX_MATCH0_ID,
    "longest_prefix_match0",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(10), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH0_TBL_GET_IHP_LONGEST_PREFIX_MATCH0_TBL_GET_ID,
    "longest_prefix_match0_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(11), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH1_TBL_SET_IHP_LONGEST_PREFIX_MATCH1_TBL_SET_ID,
    "longest_prefix_match1_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(12), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH1_TBL_SET_IHP_LONGEST_PREFIX_MATCH1_TBL_SET_IHP_LONGEST_PREFIX_MATCH1_TBL_DATA_LONGEST_PREFIX_MATCH1_ID,
    "longest_prefix_match1",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(12), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH1_TBL_GET_IHP_LONGEST_PREFIX_MATCH1_TBL_GET_ID,
    "longest_prefix_match1_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(13), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH2_TBL_SET_IHP_LONGEST_PREFIX_MATCH2_TBL_SET_ID,
    "longest_prefix_match2_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(14), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH2_TBL_SET_IHP_LONGEST_PREFIX_MATCH2_TBL_SET_IHP_LONGEST_PREFIX_MATCH2_TBL_DATA_LONGEST_PREFIX_MATCH2_ID,
    "longest_prefix_match2",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(14), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_LONGEST_PREFIX_MATCH2_TBL_GET_IHP_LONGEST_PREFIX_MATCH2_TBL_GET_ID,
    "longest_prefix_match2_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(15), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_A_TBL_SET_IHP_TCAM_ACTION_BANK_A_TBL_SET_ID,
    "tcam_action_bank_a_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(16), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_A_TBL_SET_IHP_TCAM_ACTION_BANK_A_TBL_SET_IHP_TCAM_ACTION_BANK_A_TBL_DATA_TCAM_ACTION_BANK_A_0_ID,
    "tcam_action_bank_a_0",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(16), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_A_TBL_GET_IHP_TCAM_ACTION_BANK_A_TBL_GET_ID,
    "tcam_action_bank_a_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(17), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_A_TBL_GET_IHP_TCAM_ACTION_BANK_A_TBL_GET_BANK_ID_ID,
    "bank_id",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(17), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_B_TBL_SET_IHP_TCAM_ACTION_BANK_B_TBL_SET_ID,
    "tcam_action_bank_b_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(18), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_B_TBL_SET_IHP_TCAM_ACTION_BANK_B_TBL_SET_IHP_TCAM_ACTION_BANK_B_TBL_DATA_TCAM_ACTION_BANK_B_0_ID,
    "tcam_action_bank_b_0",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(18), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_B_TBL_GET_IHP_TCAM_ACTION_BANK_B_TBL_GET_ID,
    "tcam_action_bank_b_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(19), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_C_TBL_SET_IHP_TCAM_ACTION_BANK_C_TBL_SET_ID,
    "tcam_action_bank_c_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(20), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_C_TBL_SET_IHP_TCAM_ACTION_BANK_C_TBL_SET_IHP_TCAM_ACTION_BANK_C_TBL_DATA_TCAM_ACTION_BANK_C_0_ID,
    "tcam_action_bank_c_0",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(20), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_C_TBL_GET_IHP_TCAM_ACTION_BANK_C_TBL_GET_ID,
    "tcam_action_bank_c_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(21), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_D_TBL_SET_IHP_TCAM_ACTION_BANK_D_TBL_SET_ID,
    "tcam_action_bank_d_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(22), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_D_TBL_SET_IHP_TCAM_ACTION_BANK_D_TBL_SET_IHP_TCAM_ACTION_BANK_D_TBL_DATA_TCAM_ACTION_BANK_D_0_ID,
    "tcam_action_bank_d_0",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(22), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_ACTION_BANK_D_TBL_GET_IHP_TCAM_ACTION_BANK_D_TBL_GET_ID,
    "tcam_action_bank_d_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(23), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TOS_TO_COS_TBL_SET_IHP_TOS_TO_COS_TBL_SET_ID,
    "tos_to_cos_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(24), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TOS_TO_COS_TBL_SET_IHP_TOS_TO_COS_TBL_SET_IHP_TOS_TO_COS_TBL_DATA_TOS_TO_COS_ID,
    "tos_to_cos",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(24), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TOS_TO_COS_TBL_GET_IHP_TOS_TO_COS_TBL_GET_ID,
    "tos_to_cos_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(25), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_ACCESSED_TABLE_TBL_SET_IHP_FEC_ACCESSED_TABLE_TBL_SET_ID,
    "fec_accessed_table_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(26), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_ACCESSED_TABLE_TBL_SET_IHP_FEC_ACCESSED_TABLE_TBL_SET_IHP_FEC_ACCESSED_TABLE_TBL_DATA_FEC_ACCESSED_TABLE_ID,
    "fec_accessed_table",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(26), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_FEC_ACCESSED_TABLE_TBL_GET_IHP_FEC_ACCESSED_TABLE_TBL_GET_ID,
    "fec_accessed_table_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(27), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_IPV4_STAT_TABLE_TBL_SET_IHP_IPV4_STAT_TABLE_TBL_SET_ID,
    "ipv4_stat_table_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(28), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_IPV4_STAT_TABLE_TBL_SET_IHP_IPV4_STAT_TABLE_TBL_SET_IHP_IPV4_STAT_TABLE_TBL_DATA_COUNTER_VAL_ID,
    "counter_val",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(28), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_IPV4_STAT_TABLE_TBL_GET_IHP_IPV4_STAT_TABLE_TBL_GET_ID,
    "ipv4_stat_table_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(29), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_BANK_A_TBL_SET_IHP_TCAM_BANK_A_TBL_SET_ID,
    "tcam_bank_a_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(30), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_BANK_A_TBL_SET_IHP_TCAM_BANK_A_TBL_SET_IHP_TCAM_BANK_A_TBL_DATA_TCAM_BANK_A_0_ID,
    "tcam_bank_a_0",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(30), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_BANK_A_TBL_GET_IHP_TCAM_BANK_A_TBL_GET_ID,
    "tcam_bank_a_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(31), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_TCAM_READ_ID,
    "tcam_read",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_empty_vals[0],
    (sizeof(soc_petra_pp_acc_empty_vals) / sizeof(soc_petra_pp_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(0), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_TCAM_READ_BANK_ID,
    "bank",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(0), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_TCAM_READ_UP_TO_ID,
    "up_to",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(0), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_TCAM_READ_PRINT_CONT_ID,
    "print_content",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(0), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_TCAM_READ_ENTRY_ID,
    "entry",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(0), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_TCAM_READ_NOF_ENTRIES_ID,
    "entries_in_line",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(0), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_TCAM_WRITE_ID,
    "tcam_write",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_empty_vals[0],
    (sizeof(soc_petra_pp_acc_empty_vals) / sizeof(soc_petra_pp_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(1), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_TCAM_WRITE_BANK_ID,
    "bank",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(1), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_TCAM_WRITE_ENTRY_ID,
    "entry",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(1), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_TCAM_WRITE_NOF_ENTRIES_ID,
    "entries_in_line",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(1), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_TCAM_WRITE_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(1), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_TCAM_WRITE_VALUE_ID,
    "value",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_members_vals[0],
    (sizeof(soc_petra_pp_acc_members_vals) / sizeof(soc_petra_pp_acc_members_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(1), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_TCAM_WRITE_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_members_vals[0],
    (sizeof(soc_petra_pp_acc_members_vals) / sizeof(soc_petra_pp_acc_members_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(1), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_TCAM_FLUSH_ID,
    "tcam_flush",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_empty_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(2), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_TCAM_FLUSH_BANK_ID,
    "bank",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(2), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_TCAM_COMPARE_ID,
    "tcam_compare",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_empty_vals[0],
    (sizeof(soc_petra_pp_acc_empty_vals) / sizeof(soc_petra_pp_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(3), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_TCAM_COMPARE_BANK_ID,
    "bank",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(3), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_TCAM_COMPARE_NOF_ENTRIES_ID,
    "entries_in_line",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(3), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_TCAM_COMPARE_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(3), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_TCAM_COMPARE_VALUE_ID,
    "value",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_members_vals[0],
    (sizeof(soc_petra_pp_acc_members_vals) / sizeof(soc_petra_pp_acc_members_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(3), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_TCAM_COMPARE_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_members_vals[0],
    (sizeof(soc_petra_pp_acc_members_vals) / sizeof(soc_petra_pp_acc_members_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(3), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_BANK_D_TBL_SET_IHP_TCAM_BANK_D_TBL_SET_ID,
    "tcam_bank_d_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(4), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_BANK_D_TBL_SET_IHP_TCAM_BANK_D_TBL_SET_IHP_TCAM_BANK_D_TBL_DATA_TCAM_BANK_D_0_ID,
    "tcam_bank_d_0",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(4), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_TCAM_BANK_D_TBL_GET_IHP_TCAM_BANK_D_TBL_GET_ID,
    "tcam_bank_d_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(5), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_IPV6_TC_TO_COS_TBL_SET_IHP_IPV6_TC_TO_COS_TBL_SET_ID,
    "ipv6_tc_to_cos_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(6), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_IPV6_TC_TO_COS_TBL_SET_IHP_IPV6_TC_TO_COS_TBL_SET_IHP_IPV6_TC_TO_COS_TBL_DATA_IPV6_TC_TO_COS_ID,
    "ipv6_tc_to_cos",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(6), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_IPV6_TC_TO_COS_TBL_GET_IHP_IPV6_TC_TO_COS_TBL_GET_ID,
    "ipv6_tc_to_cos_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(7), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_SET_ID,
    "cid_subnet_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(8), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_DATA_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(8), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_DATA_CVLAN_UP_ID,
    "cvlan_up",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(8), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_DATA_TRAFFIC_CLASS_ID,
    "traffic_class",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(8), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_DATA_CID_ID,
    "cid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(8), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_DATA_MASK_MINUS_ONE_ID,
    "mask_minus_one",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(8), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_SET_IHP_CID_SUBNET_TBL_DATA_IP_ID,
    "ip",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(8), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_CID_SUBNET_TBL_GET_IHP_CID_SUBNET_TBL_GET_ID,
    "cid_subnet_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(9), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_ID,
    "port_da_sa_not_found_fwd_action_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(10), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_ADD_CID_TO_DEST_ID,
    "add_cid_to_dest",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(10), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_POLICER_ID,
    "policer",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(10), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_CPU_CODE_ID,
    "cpu_code",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(10), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_COS_OVERWRITE_ID,
    "cos_overwrite",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(10), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_COS_ID,
    "cos",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(10), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_DESTINATION_ID,
    "destination",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(10), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_SET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_DATA_TYPE_ID,
    "type",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(10), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_GET_IHP_PORT_DA_SA_NOT_FOUND_FWD_ACTION_TBL_GET_ID,
    "port_da_sa_not_found_fwd_action_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(11), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_IHP_API_ID,
    "ihp",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_empty_vals[0],
    (sizeof(soc_petra_pp_acc_empty_vals) / sizeof(soc_petra_pp_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(8)|BIT(9)|BIT(10)|BIT(11)|BIT(12)|BIT(13)|BIT(14)|BIT(15)|BIT(16)|BIT(17)|BIT(18)|BIT(19)|BIT(20)|BIT(21)|BIT(22)|BIT(23)|BIT(24)|BIT(25)|BIT(26)|BIT(27)|BIT(28)|BIT(29)|BIT(30)|BIT(31), BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(8)|BIT(9)|BIT(10)|BIT(11)|BIT(12)|BIT(13)|BIT(14)|BIT(15)|BIT(16)|BIT(17)|BIT(18)|BIT(19)|BIT(20)|BIT(21)|BIT(22)|BIT(23)|BIT(24)|BIT(25)|BIT(26)|BIT(27)|BIT(28)|BIT(29)|BIT(30)|BIT(31), BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(8)|BIT(9)|BIT(10)|BIT(11), 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } ihp*/
#ifdef UI_PETRA_PP_EGQ_API/* { egq*/
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_ID,
    "pct_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(12), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_ETHERNET_BRIDGE_PORT_TYPE_ID,
    "ethernet_bridge_port_type",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(12), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_CEP_PVID_ID,
    "cep_pvid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(12), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_PEP_PVID_ID,
    "pep_pvid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(12), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_PEP_PORT_DEFAULT_PRIORITY_ID,
    "pep_port_default_priority",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(12), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_CEP_TX_TAGGED_CEP_PVID_ID,
    "cep_tx_tagged_cep_pvid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(12), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_CEP_TX_TAGGED_PEP_PVID_ID,
    "cep_tx_tagged_pep_pvid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(12), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_CEP_TX_TAGGED_OTHER_ID,
    "cep_tx_tagged_other",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(12), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_PORT_PCP_ENCODE_SELECT_ID,
    "port_pcp_encode_select",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(12), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_SET_EGQ_PCT_TBL_DATA_SA_LSB_ID,
    "sa_lsb",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(12), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PCT_TBL_GET_EGQ_PCT_TBL_GET_ID,
    "pct_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(13), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_ID,
    "ppct_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(14), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_PORT_TYPE_ID,
    "port_type",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(14), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_ETHERNET_BRIDGE_PORT_TYPE_ID,
    "ethernet_bridge_port_type",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(14), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_EGRESS_VLAN_FILTERING_ENABLE_ID,
    "egress_vlan_filtering_enable",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(14), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_CEP_PVID_ID,
    "cep_pvid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(14), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_CEP_ENABLE_EGRESS_FILTERING_ID,
    "cep_enable_egress_filtering",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(14), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_CEP_PORT_CEP_PVID_MEMBER_ID,
    "cep_port_cep_pvid_member",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(14), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_CEP_PORT_ALL_OTHERS_MEMBER_ID,
    "cep_port_all_others_member",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(14), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_PEP_PVID_ID,
    "pep_pvid",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(14), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_PEP_ACCEPTABLE_FRAME_TYPES_ID,
    "pep_acceptable_frame_types",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(14), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_PEP_ENABLE_INGRESS_FILTERING_ID,
    "pep_enable_ingress_filtering",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(14), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_PEP_PORT_PEP_PVID_MEMBER_ID,
    "pep_port_pep_pvid_member",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(14), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_PEP_PORT_ALL_OTHERS_MEMBER_ID,
    "pep_port_all_others_member",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(14), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_MTU_VIOLATION_ACTION_ID,
    "mtu_violation_action",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(14), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_MTU_ID,
    "mtu",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(14), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_PORT_IS_GLAG_MEMBER_ID,
    "port_is_glag_member",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(14), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_GLAG_PORT_ID_ID,
    "glag_port_id",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(14), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_GLAG_MEMBER_PORT_ID_ID,
    "glag_member_port_id",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(14), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_SYS_PORT_ID_ID,
    "sys_port_id",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(14), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_MULTICAST_LAG_LOAD_BALANCING_ENABLE_ID,
    "multicast_lag_load_balancing_enable",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(14), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_UNKNOWN_UC_DA_ACTION_FILTER_ID,
    "unknown_uc_da_action_filter",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(14), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_UNKNOWN_MC_DA_ACTION_FILTER_ID,
    "unknown_mc_da_action_filter",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(14), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_SET_EGQ_PPCT_TBL_DATA_UNKNOWN_BC_DA_ACTION_FILTER_ID,
    "unknown_bc_da_action_filter",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(14), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_PPCT_TBL_GET_EGQ_PPCT_TBL_GET_ID,
    "ppct_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(15), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_STP_TBL_SET_EGQ_STP_TBL_SET_ID,
    "stp_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(16), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_STP_TBL_SET_EGQ_STP_TBL_SET_EGQ_STP_TBL_DATA_EGRESS_STP_STATE_1_ID,
    "egress_stp_state_1",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(16), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_STP_TBL_SET_EGQ_STP_TBL_SET_EGQ_STP_TBL_DATA_EGRESS_STP_STATE_0_ID,
    "egress_stp_state_0",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(16), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EGQ_STP_TBL_GET_EGQ_STP_TBL_GET_ID,
    "stp_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(17), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_EGQ_API_ID,
    "egq",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_empty_vals[0],
    (sizeof(soc_petra_pp_acc_empty_vals) / sizeof(soc_petra_pp_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(12)|BIT(13)|BIT(14)|BIT(15)|BIT(16)|BIT(17), 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } egq*/
#ifdef UI_PETRA_PP_EPNI_API/* { epni*/
  {
    PARAM_PETRA_PP_PETRA_PP_EPNI_ARP_TBL_SET_EPNI_ARP_TBL_SET_ID,
    "arp_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(18), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EPNI_ARP_TBL_SET_EPNI_ARP_TBL_SET_EPNI_ARP_TBL_DATA_DMAC_0_ID,
    "dmac_0",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(18), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EPNI_ARP_TBL_GET_EPNI_ARP_TBL_GET_ID,
    "arp_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(19), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EPNI_PTT_TBL_SET_EPNI_PTT_TBL_SET_ID,
    "ptt_tbl_set",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(20), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EPNI_PTT_TBL_SET_EPNI_PTT_TBL_SET_EPNI_PTT_TBL_DATA_TRANSMIT_TAG_ENABLE_2_ID,
    "transmit_tag_enable_2",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(20), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EPNI_PTT_TBL_SET_EPNI_PTT_TBL_SET_EPNI_PTT_TBL_DATA_TRANSMIT_TAG_ENABLE_1_ID,
    "transmit_tag_enable_1",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(20), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EPNI_PTT_TBL_SET_EPNI_PTT_TBL_SET_EPNI_PTT_TBL_DATA_TRANSMIT_TAG_ENABLE_0_ID,
    "transmit_tag_enable_0",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(20), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_PETRA_PP_EPNI_PTT_TBL_GET_EPNI_PTT_TBL_GET_ID,
    "ptt_tbl_get",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_free_vals[0],
    (sizeof(soc_petra_pp_acc_free_vals) / sizeof(soc_petra_pp_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(21), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_PETRA_PP_EPNI_API_ID,
    "epni",
    (PARAM_VAL_RULES *)&soc_petra_pp_acc_empty_vals[0],
    (sizeof(soc_petra_pp_acc_empty_vals) / sizeof(soc_petra_pp_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(18)|BIT(19)|BIT(20)|BIT(21), 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } epni*/

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

/* } __UI_ROM_DEFI_PETRA_PP_ACC_INCLUDED__*/
#endif
