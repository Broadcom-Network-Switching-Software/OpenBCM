/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UI_ROM_DEFI_PETRA_APP_INCLUDED__
/* { */
#define __UI_ROM_DEFI_PETRA_APP_INCLUDED__

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <appl/dpp/UserInterface/ui_pure_defi_swp_petra.h>
#include <appl/dpp/UserInterface/ui_rom_defi_petra_api.h>


#include <appl/dpp/sweep/Petra/swp_p_tm_info.h>
#include <appl/dpp/sweep/Petra/swp_p_tm_sch_schemes.h>
#include <appl/dpp/sweep/Petra/swp_p_api_multicast.h>

#define UI_SWP_P_DIAGNOSTICS
#define UI_SWP_P_MULTICAST
#define UI_SWP_P_TM_VSQ         
#define UI_SWP_P_TM_QOS_INGR

/********************************************************/

EXTERN CONST
   PARAM_VAL_RULES
     swp_p_api_free_vals[]
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
;

/********************************************************/

EXTERN CONST
   PARAM_VAL_RULES
     swp_p_api_empty_vals[]
   =
{
/*
 * Last element. Do not remove.
 */
  {
    VAL_END_OF_LIST
  }
}
;

/********************************************************/

/********************************************************/
EXTERN CONST
   PARAM_VAL_RULES
     SWP_P_MC_GRP_MEMBERS_rule[]
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
        UI_MC_GRP_MEMBERS_MAX,
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
     SWP_P_PER_MEMBER_CUD_rule[]
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
        UI_MC_GRP_MEMBERS_MAX,
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
     SWP_P_MC_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "all_auto",
    {
      {
        SWP_P_MC_TYPE_ALL_AUTO,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_MC_TYPE.SWP_P_MC_TYPE_ALL_AUTO:\r\n"
        "  Egress + Ingress multicast, based on pre-defined policy. \r\n"
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
    "egr_auto",
    {
      {
        SWP_P_MC_TYPE_EGR_AUTO,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_MC_TYPE.SWP_P_MC_TYPE_EGR_AUTO:\r\n"
        "  Egress multicast, based on pre-defined policy. \r\n"
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
    "egr_vlan",
    {
      {
        SWP_P_MC_TYPE_EGR_VLAN,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_MC_TYPE.SWP_P_MC_TYPE_EGR_VLAN:\r\n"
        "  Egress multicast, VLAN membership replication (set of system ports). \r\n"
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
    "egr_gen",
    {
      {
        SWP_P_MC_TYPE_EGR_GEN,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_MC_TYPE.SWP_P_MC_TYPE_EGR_GEN:\r\n"
        "  Egress multicast, General replication (set of system ports +\r\n"
        "  Copy-Unique-Data).\r\n"
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
    "ingr_auto",
    {
      {
        SWP_P_MC_TYPE_INGR_AUTO,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_MC_TYPE.SWP_P_MC_TYPE_INGR_AUTO:\r\n"
        "  Ingress multicast, based on pre-defined policy.\r\n"
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
    "ingr_dev_sched",
    {
      {
        SWP_P_MC_TYPE_INGR_DEV_SCHED,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_MC_TYPE.SWP_P_MC_TYPE_INGR_DEV_SCHED:\r\n"
        "  Ingress multicast, Device-Level-Scheduled (set of destination FAP-s). \r\n"
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
    "ingr_port_sched",
    {
      {
        SWP_P_MC_TYPE_INGR_PORT_SCHED,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_MC_TYPE.SWP_P_MC_TYPE_INGR_PORT_SCHED:\r\n"
        "  Ingress multicast, Port-Level-Scheduled (set of destination system ports).\r\n"
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
    "ingr_fabric",
    {
      {
        SWP_P_MC_TYPE_INGR_FABRIC,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_MC_TYPE.SWP_P_MC_TYPE_INGR_FABRIC:\r\n"
        "  Fabric multicast, not scheduled by the egress.\r\n"
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

EXTERN CONST
   PARAM_VAL_RULES
      SWP_PETRA_SRD_CNT_SRC_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "MAC",
    {
      {
        SOC_PETRA_SRD_CNT_SRC_CRC_MAC,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_PETRA_SRD_CNT_SRC.SOC_PETRA_SRD_CNT_SRC_MAC:\r\n"
        "  Counting source as MAC, no FEC enable.\r\n"
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
    "PRBS",
    {
      {
        SOC_PETRA_SRD_CNT_SRC_PRBS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_PETRA_SRD_CNT_SRC.SOC_PETRA_SRD_CNT_SRC_PRBS:\r\n"
        "  Counting source as PRBS, the prbs_ parameters must be set.\r\n"
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
    "BER",
    {
      {
        SOC_PETRA_SRD_CNT_SRC_FEC_BER,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_PETRA_SRD_CNT_SRC.SOC_PETRA_SRD_CNT_SRC_BER:\r\n"
        "  BER - FEC enable.\r\n"
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
    "CER",
    {
      {
        SOC_PETRA_SRD_CNT_SRC_FEC_CER,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_PETRA_SRD_CNT_SRC.SOC_PETRA_SRD_CNT_SRC_CER:\r\n"
        "  CER - FEC enable.\r\n"
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
   PARAM_VAL_RULES
      SWP_PETRA_SRD_TRAFFIC_SRC_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "prbs",
    {
      {
        SOC_PETRA_SRD_TRAFFIC_SRC_PRBS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_PETRA_SRD_TRAFFIC_SRC.SOC_PETRA_SRD_TRAFFIC_SRC_PRBS:\r\n"
        "  PRBS enable.\r\n"
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
    "ext",
    {
      {
        SOC_PETRA_SRD_TRAFFIC_SRC_EXT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_PETRA_SRD_TRAFFIC_SRC.SOC_PETRA_SRD_TRAFFIC_SRC_ExT:\r\n"
        "  External traffic (no PRBS, nor snake) is already generated.\r\n"
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
  PARAM_VAL_RULES
    soc_petra_api_serdes_vals[];

EXTERN CONST
  PARAM_VAL_RULES
    SOC_PETRA_SRD_PRBS_MODE_rule[];

/*
  * } ENUM RULES
  */
/********************************************************/
EXTERN CONST
   PARAM
     swp_p_api_params[]
   =
{
#ifdef INIT
  {
    PARAM_SWP_P_SSR_SAVE_ID,
      "reset_restore_and_test",
      (PARAM_VAL_RULES *)&swp_p_api_empty_vals[0],
      (sizeof(swp_p_api_empty_vals) / sizeof(swp_p_api_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "",
      "",
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
    PARAM_SWP_P_SSR_ID,
      "ssr",
      (PARAM_VAL_RULES *)&swp_p_api_empty_vals[0],
      (sizeof(swp_p_api_empty_vals) / sizeof(swp_p_api_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "",
      "",
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
    PARAM_SWP_P_FE_GRACEFUL_RESTORE_FE_ID,
      "fe_ndx",
      (PARAM_VAL_RULES *)&swp_p_api_free_vals[0],
      (sizeof(swp_p_api_free_vals) / sizeof(swp_p_api_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "",
      "",
      "",
    {BIT(5)},
    3,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_FE_GRACEFUL_RESTORE_ID,
      "graceful_restore",
      (PARAM_VAL_RULES *)&swp_p_api_empty_vals[0],
      (sizeof(swp_p_api_empty_vals) / sizeof(swp_p_api_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "",
      "",
      "",
    {BIT(5)},
    2,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
    {
    PARAM_SWP_P_FE_GRACEFUL_SHUT_DOWN_FE_ID,
      "fe_ndx",
      (PARAM_VAL_RULES *)&swp_p_api_free_vals[0],
      (sizeof(swp_p_api_free_vals) / sizeof(swp_p_api_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "",
      "",
      "",
    {BIT(4)},
    3,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_FE_GRACEFUL_SHUT_DOWN_ID,
      "graceful_shut_down",
      (PARAM_VAL_RULES *)&swp_p_api_empty_vals[0],
      (sizeof(swp_p_api_empty_vals) / sizeof(swp_p_api_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "",
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
    PARAM_SWP_P_FE_ID,
      "fe",
      (PARAM_VAL_RULES *)&swp_p_api_empty_vals[0],
      (sizeof(swp_p_api_empty_vals) / sizeof(swp_p_api_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "",
      "",
      "",
    {BIT(4)|BIT(5)},
    1,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
#endif    /* } INIT*/

#ifdef UI_SWP_P_DIAGNOSTICS/* { swp_p_diagnostics*/
  {
    PARAM_SWP_P_DIAG_NIF_COUNTERS_GET_NIF_COUNTERS_GET_ID,
    "nif_counters_get",
    (PARAM_VAL_RULES *)&swp_p_api_empty_vals[0],
    (sizeof(swp_p_api_empty_vals) / sizeof(swp_p_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Gets Statistics Counters for all the NIF-s in the device.\r\n"
    "",
    "",
#if UI_SWP_P_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_api swp_p_diagnostics nif_counters_get\r\n"
    "",
#endif
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
    PARAM_SWP_P_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_ID,
      "eye_scan_run",
      (PARAM_VAL_RULES *)&swp_p_api_empty_vals[0],
      (sizeof(swp_p_api_empty_vals) / sizeof(swp_p_api_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "  Runs the eye scan, in order to map between serdes rx parameters\r\n"
      "  and amount of crc errors.\r\n"
      "  Eye scan sequence is as following:\r\n"
      "  For each tlth * dfelth pair, run prbs for a given time, and collect\r\n"
      "  results to a matrix, per lane. The matrix is used to find optimal\r\n"
      "  rx parameters. \r\n"
      "",
#if UI_PETRA_API_ADD_EXAMPLE
      "Examples:\r\n"
        "  swp_p_api swp_p_diagnostics srd_eye_scan_run resolution 0 duration_min_sec 1\r\n"
      "  lane_ndx 0"
#endif
      "",
      "",
    {BIT(9)},
    2,
      /*
      * Pointer to a function to call after symbolic parameter
      * has been accepted.
      */
      (VAL_PROC_PTR)NULL
    },
    {
     PARAM_SWP_P_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_TRAFFIC_SRC_ID,
      "traffic_src",
      (PARAM_VAL_RULES *)&SWP_PETRA_SRD_TRAFFIC_SRC_rule[0],
    (sizeof(SWP_PETRA_SRD_TRAFFIC_SRC_rule) / sizeof(SWP_PETRA_SRD_TRAFFIC_SRC_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.traffic source:\r\n"
    "  Traffic source is optionally prbs or snake.\r\n"
    "",
#if UI_PETRA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  soc_petra_api serdes_utils srd_eye_scan_run resolution 0 duration_min_sec 0\r\n"
    "  lane_ndx 0"
#endif
    "",
    "",
      {BIT(9)},
    3,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_RESOLUTION_ID,
      "resolution",
      (PARAM_VAL_RULES *)&swp_p_api_free_vals[0],
      (sizeof(swp_p_api_free_vals) / sizeof(swp_p_api_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "  info.resolution:\r\n"
      "  Scan density (e.g value of 2 scans every 2nd point in matrix). Range: 1 - 10.\r\n"
      "",
#if UI_PETRA_API_ADD_EXAMPLE
      "Examples:\r\n"
        "  swp_p_api swp_p_diagnostics srd_eye_scan_run resolution 4 duration_min_sec 1\r\n"
        "  lane_ndx 0"
#endif
        "",
        "",
      {BIT(9)},
      LAST_ORDINAL,
      /*
      * Pointer to a function to call after symbolic parameter
      * has been accepted.
      */
      (VAL_PROC_PTR)NULL
      },
      {
     PARAM_SWP_P_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_CRC_ID,
      "counting_src",
      (PARAM_VAL_RULES *)&SWP_PETRA_SRD_CNT_SRC_rule[0],
    (sizeof(SWP_PETRA_SRD_CNT_SRC_rule) / sizeof(SWP_PETRA_SRD_CNT_SRC_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.counting source:\r\n"
    "  Counting source if not PRBS.\r\n"
    "",
#if UI_PETRA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  soc_petra_api serdes_utils srd_eye_scan_run resolution 0 duration_min_sec 0\r\n"
      "  lane_ndx 0"
#endif
      "",
      "",
    {BIT(9)},
    LAST_ORDINAL,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
    },
    {
      PARAM_SWP_P_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_PRBS_DURATION_MIN_SEC_ID,
          "duration_min_sec",
        (PARAM_VAL_RULES *)&swp_p_api_free_vals[0],
        (sizeof(swp_p_api_free_vals) / sizeof(swp_p_api_free_vals[0])) - 1,
        HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
        0,0,0,0,0,0,0,
          "  info.duration_min_sec:\r\n"
        "  Minimal duration to run PRBS on each point of scan. Range: 100 - 60000.\r\n"
        "",
#if UI_PETRA_API_ADD_EXAMPLE
        "Examples:\r\n"
          "  swp_p_api swp_p_diagnostics srd_eye_scan_run resolution 4 duration_min_sec 1\r\n"
        "  lane_ndx 0"
#endif
        "",
        "",
      {BIT(9)},
      LAST_ORDINAL,
      /*
      * Pointer to a function to call after symbolic parameter
      * has been accepted.
      */
      (VAL_PROC_PTR)NULL
    },
    {
        PARAM_SWP_P_SRD_EYE_SCAN_RUN_SRD_PRBS_MODE_SET_MODE_ID,
            "prbs_mode",
          (PARAM_VAL_RULES *)&SOC_PETRA_SRD_PRBS_MODE_rule[0],
          (sizeof(SOC_PETRA_SRD_PRBS_MODE_rule) / sizeof(SOC_PETRA_SRD_PRBS_MODE_rule[0])) - 1,
          HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
          0,0,0,0,0,0,0,
          "  mode:\r\n"
          "  SerDes PRBS pattern mode.\r\n"
          "",
#ifdef UI_PETRA_API_ADD_EXAMPLE
          "Examples:\r\n"
          "  soc_petra_api serdes_utils srd_prbs_mode_set lane_ndx 0 direction_ndx 0 mode 0\r\n"
#endif
          "",
          "",
        {BIT(9)},
        LAST_ORDINAL,
        /*
        * Pointer to a function to call after symbolic parameter
        * has been accepted.
        */
        (VAL_PROC_PTR)NULL
      },
      {
        PARAM_SWP_P_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_LANE_NDX_ID,
          "lane_ndx",
          (PARAM_VAL_RULES *)&soc_petra_api_serdes_vals[0],
          (sizeof(soc_petra_api_serdes_vals) / sizeof(soc_petra_api_serdes_vals[0])) - 1,
          HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
          0,0,0,0,0,0,0,
          "  info.lane_ndx[ prm_lane_ndx_index]:\r\n"
          "  Array of lane indices to run eye scan on.\r\n"
          "",
#if UI_PETRA_API_ADD_EXAMPLE
          "Examples:\r\n"
            "  swp_p_api swp_p_diagnostics srd_eye_scan_run resolution 4 duration_min_sec 1\r\n"
          "  lane_ndx 0"
#endif
          "",
          "",
        {BIT(9)},
        LAST_ORDINAL,
        /*
        * Pointer to a function to call after symbolic parameter
        * has been accepted.
        */
        (VAL_PROC_PTR)NULL
      },
      {
    PARAM_SWP_P_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_TX_PRBS_ID,
      "prbs_is_tx_enabled",
    (PARAM_VAL_RULES *)&swp_p_api_free_vals[0],
    (sizeof(swp_p_api_free_vals) / sizeof(swp_p_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.is_tx_prbs_enabled:\r\n"
    "  Range: 0 - 1.\r\n"
    "",
#if UI_PETRA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  soc_petra_api serdes_utils srd_eye_scan_run resolution 0 duration_min_sec 0\r\n"
    "  lane_ndx 0"
  #endif
        "",
        "",
      {BIT(9)},
      LAST_ORDINAL,
      /*
      * Pointer to a function to call after symbolic parameter
      * has been accepted.
      */
      (VAL_PROC_PTR)NULL
      },

      {
        PARAM_SWP_P_DIAGNOSTICS_ID,
        "diagnostics",
        (PARAM_VAL_RULES *)&swp_p_api_empty_vals[0],
        (sizeof(swp_p_api_empty_vals) / sizeof(swp_p_api_empty_vals[0])) - 1,
        HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
        0,0,0,0,0,0,0,
        "",
        "",
        "",
        {BIT(0)|BIT(9)},
        1,
          /*
           * Pointer to a function to call after symbolic parameter
           * has been accepted.
           */
        (VAL_PROC_PTR)NULL
      },

#endif /* } swp_p_diagnostics*/
#ifdef UI_SWP_P_MULTICAST/* { swp_p_multicast*/
  {
    PARAM_SWP_P_API_MULTICAST_ID,
    "multicast",
    (PARAM_VAL_RULES *)&swp_p_api_empty_vals[0],
    (sizeof(swp_p_api_empty_vals) / sizeof(swp_p_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(10)|BIT(11)|BIT(12)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_MC_GRP_OPEN_MC_GRP_OPEN_ID,
    "mc_grp_open",
    (PARAM_VAL_RULES *)&swp_p_api_empty_vals[0],
    (sizeof(swp_p_api_empty_vals) / sizeof(swp_p_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Open Multicast Group.\r\n"
    "",
    "",
#if UI_SWP_P_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_api multicast mc_grp_open mc_id 0 mc_type 0 mc_grp_size 0\r\n"
    "  mc_grp_members 0 per_member_cud 0\r\n"
    "",
#endif
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
    PARAM_SWP_P_MC_GRP_OPEN_MC_GRP_OPEN_MC_ID_ID,
    "mc_id",
    (PARAM_VAL_RULES *)&swp_p_api_free_vals[0],
    (sizeof(swp_p_api_free_vals) / sizeof(swp_p_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_id:\r\n"
    "  The index of the multicast group to open. Range: 0 - 16K-1.\r\n"
    "",
    "",
#if UI_SWP_P_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_api multicast mc_grp_open mc_id 0 mc_type 0 mc_grp_size 0\r\n"
    "  mc_grp_members 0 per_member_cud 0\r\n"
    "",
#endif
    "",
    {BIT(10)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_MC_GRP_OPEN_MC_GRP_OPEN_MC_TYPE_ID,
    "mc_type",
    (PARAM_VAL_RULES *)&SWP_P_MC_TYPE_rule[0],
    (sizeof(SWP_P_MC_TYPE_rule) / sizeof(SWP_P_MC_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_type:\r\n"
    "  The type of the multicast to open.\r\n"
    "",
    "",
#if UI_SWP_P_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_api multicast mc_grp_open mc_id 0 mc_type 0 mc_grp_size 0\r\n"
    "  mc_grp_members 0 per_member_cud 0\r\n"
    "",
#endif
    "",
    {BIT(10)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_MC_GRP_OPEN_MC_GRP_OPEN_MC_GRP_MEMBERS_ID,
    "mc_grp_members",
    (PARAM_VAL_RULES *)&SWP_P_MC_GRP_MEMBERS_rule[0],
    (sizeof(SWP_P_MC_GRP_MEMBERS_rule) / sizeof(SWP_P_MC_GRP_MEMBERS_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_grp_members:\r\n"
    "  A list of multicast group members;\r\n"
    "  Destination Physical System Ports.\r\n"
    "  The size of the array is mc_grp_size.\r\n"
    "  Note: for SWP_P_MC_TYPE_INGR_FABRIC, this field is ignored\r\n"
    "  and may be set to NULL.\r\n"
    "",
    "",
#if UI_SWP_P_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_api multicast mc_grp_open mc_id 0 mc_type 0 mc_grp_size 0\r\n"
    "  mc_grp_members 0 per_member_cud 0\r\n"
    "",
#endif
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
    PARAM_SWP_P_MC_GRP_OPEN_MC_GRP_OPEN_PER_MEMBER_CUD_ID,
    "per_member_cud",
    (PARAM_VAL_RULES *)&SWP_P_PER_MEMBER_CUD_rule[0],
    (sizeof(SWP_P_PER_MEMBER_CUD_rule) / sizeof(SWP_P_PER_MEMBER_CUD_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  per_member_cud:\r\n"
    "  If applicable (e.g. for SWP_P_MC_TYPE_EGR_GEN multicast type) - the\r\n"
    "  Copy-Unique-Data to embed into the OTMH outlif field. Note: for Ethernet\r\n"
    "  Ports, this field is interpreted as the ARP pointer.\r\n"
    "  For MC types that don't need the CUD information, this field is ignored, and\r\n"
    "  may be set to NULL. \r\n"
    "",
    "",
#if UI_SWP_P_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_api multicast mc_grp_open mc_id 0 mc_type 0 mc_grp_size 0\r\n"
    "  mc_grp_members 0 per_member_cud 0\r\n"
    "",
#endif
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
    PARAM_SWP_P_MC_GRP_CLOSE_MC_GRP_CLOSE_ID,
    "mc_grp_close",
    (PARAM_VAL_RULES *)&swp_p_api_empty_vals[0],
    (sizeof(swp_p_api_empty_vals) / sizeof(swp_p_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Close a Multicast Group if exists.\r\n"
    "",
    "",
#if UI_SWP_P_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_api multicast mc_grp_close mc_type 0 mc_id 0\r\n"
    "",
#endif
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
    PARAM_SWP_P_MC_GRP_CLOSE_MC_GRP_CLOSE_MC_TYPE_ID,
    "mc_type",
    (PARAM_VAL_RULES *)&SWP_P_MC_TYPE_rule[0],
    (sizeof(SWP_P_MC_TYPE_rule) / sizeof(SWP_P_MC_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_type:\r\n"
    "  SOC_SAND_IN SWP_P_MC_TYPE\r\n"
    "  mc_type\r\n"
    "",
    "",
#if UI_SWP_P_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_api multicast mc_grp_close mc_type 0 mc_id 0\r\n"
    "",
#endif
    "",
    {BIT(11)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_MC_GRP_CLOSE_MC_GRP_CLOSE_MC_ID_ID,
    "mc_id",
    (PARAM_VAL_RULES *)&swp_p_api_free_vals[0],
    (sizeof(swp_p_api_free_vals) / sizeof(swp_p_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_id:\r\n"
    "  The index of the multicast group to close. Range: 0 - 16K-1.\r\n"
    "  SOC_SAND_IN SWP_P_MC_TYPE\r\n"
    "  mc_type -\r\n"
    "  The type of the multicast to close.\r\n"
    "",
    "",
#if UI_SWP_P_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_api multicast mc_grp_close mc_type 0 mc_id 0\r\n"
    "",
#endif
    "",
    {BIT(11)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_MC_GRP_GET_MC_GRP_GET_ID,
    "mc_grp_get",
    (PARAM_VAL_RULES *)&swp_p_api_empty_vals[0],
    (sizeof(swp_p_api_empty_vals) / sizeof(swp_p_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Open Multicast Group.\r\n"
    "",
    "",
#if UI_SWP_P_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_api multicast mc_grp_get mc_id 0 mc_type 0 mc_grp_size 0\r\n"
    "",
#endif
    "",
    {BIT(12)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_MC_GRP_GET_MC_GRP_GET_MC_ID_ID,
    "mc_id",
    (PARAM_VAL_RULES *)&swp_p_api_free_vals[0],
    (sizeof(swp_p_api_free_vals) / sizeof(swp_p_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_id:\r\n"
    "  The index of the multicast group to open. Range: 0 - 16K-1.\r\n"
    "",
    "",
#if UI_SWP_P_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_api multicast mc_grp_get mc_id 0 mc_type 0 mc_grp_size 0\r\n"
    "",
#endif
    "",
    {BIT(12)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_MC_GRP_GET_MC_GRP_GET_MC_TYPE_ID,
    "mc_type",
    (PARAM_VAL_RULES *)&SWP_P_MC_TYPE_rule[0],
    (sizeof(SWP_P_MC_TYPE_rule) / sizeof(SWP_P_MC_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_type:\r\n"
    "  The type of the multicast to open.\r\n"
    "",
    "",
#if UI_SWP_P_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_api multicast mc_grp_get mc_id 0 mc_type 0 mc_grp_size 0\r\n"
    "",
#endif
    "",
    {BIT(12)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } swp_p_multicast*/
#ifdef UI_SWP_P_TM_VSQ/* { swp_p_tm_vsq*/
  { 
    PARAM_SWP_P_TM_VSQ_TAIL_DROP_APP_TM_VSQ_TAIL_DROP_APP_ID, 
    "tail_drop_app", 
    (PARAM_VAL_RULES *)&swp_p_api_empty_vals[0], 
    (sizeof(swp_p_api_empty_vals) / sizeof(swp_p_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  VSQ tail drop per Traffic class example. Different Max-Queue-Size thresholds\n\r"
    "  are set for the 32 CTTC-VSQs and a packet count is configured for the first\n\r"
    "  CTTC-VSQ.\n\r"
    "",
#if UI_SWP_P_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_api swp_p_tm_vsq tm_vsq_tail_drop_app silent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(13), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_TM_VSQ_TAIL_DROP_APP_TM_VSQ_TAIL_DROP_APP_SILENT_ID, 
    "silent", 
    (PARAM_VAL_RULES *)&swp_p_api_free_vals[0], 
    (sizeof(swp_p_api_free_vals) / sizeof(swp_p_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  silent:\n\r"
    "  If TRUE, printing is suppressed. \n\r"
    "",
#if UI_SWP_P_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_api swp_p_tm_vsq tm_vsq_tail_drop_app silent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(13), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_TM_VSQ_WRED_PER_STAG_APP_TM_VSQ_WRED_PER_STAG_APP_ID, 
    "wred_per_stag_app", 
    (PARAM_VAL_RULES *)&swp_p_api_empty_vals[0], 
    (sizeof(swp_p_api_empty_vals) / sizeof(swp_p_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  VSQ WRED test example according to an S-Tag. The packets are tagged with a\n\r"
    "  statistic tag and gathered in the ST-VSQs according to their Traffic Class\n\r"
    "  field in the S-Tag. WRED tests are performed per (Traffic Class, Drop\n\r"
    "  Precedence).\n\r"
    "",
#if UI_SWP_P_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_api swp_p_tm_vsq tm_vsq_wred_per_stag_app silent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(14), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_TM_VSQ_WRED_PER_STAG_APP_TM_VSQ_WRED_PER_STAG_APP_SILENT_ID, 
    "silent", 
    (PARAM_VAL_RULES *)&swp_p_api_free_vals[0], 
    (sizeof(swp_p_api_free_vals) / sizeof(swp_p_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  silent:\n\r"
    "  If TRUE, printing is suppressed. \n\r"
    "",
#if UI_SWP_P_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_api swp_p_tm_vsq tm_vsq_wred_per_stag_app silent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(14), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_TM_VSQ_APP_TM_VSQ_APP_ID, 
    "vsq_app", 
    (PARAM_VAL_RULES *)&swp_p_api_empty_vals[0], 
    (sizeof(swp_p_api_empty_vals) / sizeof(swp_p_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Run the Tail Drop and the WRED per S-Tag field applications, and set a packet\n\r"
    "  counter to the first CTTC-VSQ.\n\r"
    "",
#if UI_SWP_P_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_api swp_p_tm_vsq tm_vsq_app silent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(15), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_TM_VSQ_APP_TM_VSQ_APP_SILENT_ID, 
    "silent", 
    (PARAM_VAL_RULES *)&swp_p_api_free_vals[0], 
    (sizeof(swp_p_api_free_vals) / sizeof(swp_p_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  silent:\n\r"
    "  If TRUE, printing is suppressed. \n\r"
    "",
#if UI_SWP_P_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_api swp_p_tm_vsq tm_vsq_app silent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(15), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_TM_VSQ_ID, 
    "vsq", 
    (PARAM_VAL_RULES *)&swp_p_api_empty_vals[0], 
    (sizeof(swp_p_api_empty_vals) / sizeof(swp_p_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    BIT_UI_16_RNG(12,15), 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
#endif /* } swp_p_tm_vsq*/
#ifdef UI_SWP_P_TM_QOS_INGR/* { swp_p_tm_qos_ingr*/
  { 
    PARAM_SWP_P_TM_ISP_SCHED_SET_TM_ISP_SCHED_SET_ID, 
    "isp_sched_set", 
    (PARAM_VAL_RULES *)&swp_p_api_empty_vals[0], 
    (sizeof(swp_p_api_empty_vals) / sizeof(swp_p_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Example ingress shaping configuration. Each queue in the ingress shaping\n\r"
    "  queues range gets a portion of an overall rate, so that the next queue gets\n\r"
    "  double the previous queue rate.\n\r"
    "",
#if UI_SWP_P_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_api swp_p_tm_qos_ingr tm_isp_sched_set port_ndx 0 overall_rate_mbps 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(16), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_TM_ISP_SCHED_SET_TM_ISP_SCHED_SET_PORT_NDX_ID, 
    "port_ndx", 
    (PARAM_VAL_RULES *)&swp_p_api_free_vals[0], 
    (sizeof(swp_p_api_free_vals) / sizeof(swp_p_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  port_ndx:\n\r"
    "  OFP-Port. Range: 0 - 79.	\n\r"
    "",
#if UI_SWP_P_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_api swp_p_tm_qos_ingr tm_isp_sched_set port_ndx 0 overall_rate_mbps 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(16), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_TM_ISP_SCHED_SET_TM_ISP_SCHED_SET_OVERALL_RATE_MBPS_ID, 
    "overall_rate_mbps", 
    (PARAM_VAL_RULES *)&swp_p_api_free_vals[0], 
    (sizeof(swp_p_api_free_vals) / sizeof(swp_p_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  overall_rate_mbps:\n\r"
    "  Overall rate. Units: Mbps. 	\n\r"
    "",
#if UI_SWP_P_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_api swp_p_tm_qos_ingr tm_isp_sched_set port_ndx 0 overall_rate_mbps 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(16), 
    4, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_TM_ISP_SCHED_SET_TM_ISP_SCHED_SET_SILENT_ID, 
    "silent", 
    (PARAM_VAL_RULES *)&swp_p_api_free_vals[0], 
    (sizeof(swp_p_api_free_vals) / sizeof(swp_p_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  silent:\n\r"
    "  If TRUE, printing is suppressed. \n\r"
    "",
#if UI_SWP_P_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_api swp_p_tm_qos_ingr tm_isp_sched_set port_ndx 0 overall_rate_mbps 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(16), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_TM_SCHEDULING_SCHEMES_OPEN_TM_SCHEDULING_SCHEMES_OPEN_ID, 
    "scheduling_schemes_open", 
    (PARAM_VAL_RULES *)&swp_p_api_empty_vals[0], 
    (sizeof(swp_p_api_empty_vals) / sizeof(swp_p_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Change scheduling scheme.\n\r"
    "",
#if UI_SWP_P_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_api externals tm_scheduling_schemes_open vsq_app_enable 0 bdb_wa_enable\n\r"
    "  0 is_t20e_in_system 0 srd_power_down_index 0 srd_power_down 0\n\r"
    "  screening_app_id 0 srd_phys_param_preset_id 0 serdes 0 is_trfc_tst_on_start 0\n\r"
    "  is_fabric_always 0 vlan_mc_id 0 raw_port_enable 0 is_variable_cell_size 0\n\r"
    "  use_is_variable_cell_size 0 nof_faps_in_system 0 fatp_port_rate 0\n\r"
    "  olp_port_rate 0 erp_port_rate 0 rcy_port_rate 0 cpu_port_rate 0 nif_port_rate\n\r"
    "  0 sch_scheme 0 rcy_ports_id_first 0 nof_rcy_ports 0 nof_cpu_ports 0\n\r"
    "  is_srd_double_rate 0 is_sgmii 0 nof_ports_per_nif 0 nof_nifs 0 bct_enable 0\n\r"
    "  fat_pipe_enable 0 is_mesh 0 coexist_system 0 stag 0 dram_buffs 0 dram_freq 0\n\r"
    "  dram_type 0 credit_worth 0 op_mode 0 fap_id 0 max_nof_faps_in_system 0 silent\n\r"
    "  0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(17), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_TM_QOS_INGR_ID, 
    "qos", 
    (PARAM_VAL_RULES *)&swp_p_api_empty_vals[0], 
    (sizeof(swp_p_api_empty_vals) / sizeof(swp_p_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    BIT_UI_16_RNG(15,17), 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
#endif /* } swp_p_tm_qos_ingr*/

/*
 * Last element. Do not remove.
 */
  {
    PARAM_END_OF_LIST
  }
}
;

/* } */
#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif    /* } __UI_ROM_DEFI_PETRA_APP_INCLUDED__*/
