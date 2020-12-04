/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UI_ROM_DEFI_SWEEP_APP_B_H_INCLUDED__
/* { */
#define __UI_ROM_DEFI_SWEEP_APP_B_H_INCLUDED__

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

/*************
 * INCLUDES  *
 */
/* { */

#include <ui_pure_defi_sweep_app.h>
#include <appl/dpp/UserInterface/ui_cli_files.h>

#include <sweep/Fap20v/sweep_fap20v_app.h>
#include <sweep/Fap20v/sweep_fap20v_qos.h>

#include <CSystem/csystem_consts.h>

#include <soc/dpp/SOC_SAND_FAP20V/fap20v_api_scheduler_flows.h>

/* } */

/*************
 * DEFINES   *
 */
/* { */

/* } */

/*************
 *  MACROS   *
 */
/* { */

/* } */

/*************
 * TYPE DEFS *
 */
/* { */

/* } */

/*************
 * GLOBALS   *
 */
/* { */


/********************************************************
*NAME
*  Sweep_app_port_vals
 */
EXTERN CONST
   PARAM_VAL_RULES
     Sweep_app_port_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        63,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
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
*NAME
*  Sweep_app_fap_vals
 */
EXTERN CONST
   PARAM_VAL_RULES
     Sweep_app_fap_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        CSYS_MAX_NOF_FAP_IN_SYSTEM-1,0,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        1,
          /*
           * Pointer to a function to call after the full numeric value
           * has been accepted.
           */
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
*NAME
*  Sweep_scheme_vals
 */
EXTERN CONST
   PARAM_VAL_RULES
     Sweep_scheme_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "strict",
    {
      {
        FAP20V_B_SCH_SCHEME_STRICT,
        (long)"    Strict QoS Scheme.",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "enhanced_strict",
    {
      {
        FAP20V_B_SCH_SCHEME_ENHANCED_STRICT,
        (long)"    Enhanced Strict QoS Scheme.",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "wfq_per_flow",
    {
      {
        FAP20V_B_SCH_SCHEME_WFQ_PER_FLOW,
        (long)"    WFQ per Flow.",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "wfq_per_class",
    {
      {
        FAP20V_B_SCH_SCHEME_WFQ_PER_CLASS,
        (long)"    WFQ per Class.",
        (long)NULL
      }
    }
  },
  {
    VAL_END_OF_LIST
  }
}
#endif
;

/********************************************************
*NAME
*  Sweep_percentage_vals
 */
EXTERN CONST
   PARAM_VAL_RULES
     Sweep_percentage_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        100,0,
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
*NAME
*  Sweep_app_b_params
 */
EXTERN CONST
   PARAM
     Sweep_app_b_params[]
#ifdef INIT
   =
{
/*
 ******
 * ENTRY -- 0
 *SOC_SAND_BIT(0) -
 *SOC_SAND_BIT(1) - get_connectivity_map
 *SOC_SAND_BIT(2) - power_down_serdes/power_up_serdes
 *SOC_SAND_BIT(3) - get_qos_params
 *SOC_SAND_BIT(4) - set_qos_params
 *SOC_SAND_BIT(5) - graceful_shutdown_fe
 *SOC_SAND_BIT(6) -
 *SOC_SAND_BIT(7) -
 *SOC_SAND_BIT(8) - QoS Get
 *SOC_SAND_BIT(9) - QoS Set
 *SOC_SAND_BIT(10) - FAST RESET Status
 *SOC_SAND_BIT(11) - FAST RESET load from local to hw
 ******
 * ENTRY -- 1
 *SOC_SAND_BIT() - .
*/


  {
    PARAM_SWEEP_QOS_ID,
    "qos",
    (PARAM_VAL_RULES *)&Sweep_app_empty_vals[0],
    (sizeof(Sweep_app_empty_vals) / sizeof(Sweep_app_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "S/Gets QoS Scheme.",
    "Examples:\r\n"
    "  sweep qos get fap# 0\r\n"
    " Gets FAP-0 QoS scheme.",
    "",
    { SOC_SAND_BIT(8) | SOC_SAND_BIT(9)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_QOS_GET_ID,
    "get",
    (PARAM_VAL_RULES *)&Sweep_app_empty_vals[0],
    (sizeof(Sweep_app_empty_vals) / sizeof(Sweep_app_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gets QoS Scheme.",
    "Examples:\r\n"
    "  sweep qos get fap# 0\r\n"
    " Gets FAP-0 QoS scheme.",
    "",
    { SOC_SAND_BIT(8) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_QOS_SET_ID,
    "set",
    (PARAM_VAL_RULES *)&Sweep_app_empty_vals[0],
    (sizeof(Sweep_app_empty_vals) / sizeof(Sweep_app_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gets QoS Scheme.",
    "Examples:\r\n"
    "  sweep qos set fap# 0\r\n"
    " Sets FAP-0 QoS scheme.",
    "",
    { SOC_SAND_BIT(9)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_QOS_SCH_ID,
    "scheme",
    (PARAM_VAL_RULES *)&Sweep_scheme_vals[0],
    (sizeof(Sweep_scheme_vals) / sizeof(Sweep_scheme_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Scheme to set.",
    "Examples:\r\n"
    "  sweep qos set scheme default\r\n"
    " Sets default QoS scheme to all FAPs all Ports.",
    "",
    { SOC_SAND_BIT(9) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_QOS_MIN_RATE_ID,
    "min_rate_per_class",
    (PARAM_VAL_RULES *)&Sweep_percentage_vals[0],
    (sizeof(Sweep_percentage_vals) / sizeof(Sweep_percentage_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Sweep_ZERO_default,
    0,0,0,0,0,0,0,
    "Each class will have minimum_rate_by_percentage percentage of\n\r"
    "  the port rate reserved to it.  \n\r"
    "  0 - Stands for no assured rate.\n\r"
    "  Valid values: 0-100.",
    "",
    "",
    {SOC_SAND_BIT(9)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_QOS_MAX_RATE_ID,
    "max_rate_per_class",
    (PARAM_VAL_RULES *)&Sweep_percentage_vals[0],
    (sizeof(Sweep_percentage_vals) / sizeof(Sweep_percentage_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "No class will have more than maximum_rate_by_percentage\n\r"
    "  percentage of the port rate.\n\r"
    "  0 - Stands for not limitation on the class rate.\n\r"
    "  Valid values: 0-100.",
    "",
    "",
    {SOC_SAND_BIT(9)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_QOS_LOW_LATENCY_ID,
    "low_latency",
    (PARAM_VAL_RULES *)&Sweep_app_on_off_vals[0],
    (sizeof(Sweep_app_on_off_vals) / sizeof(Sweep_app_on_off_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Add a second sub-flow that will decrease the latency.\n\r"
    "  When this is enabled, the maximum_rate_by_percentage,\n\r"
    "  will behave differently!\n\r"
    "  Off - Disable, On - Enable.",
    "",
    "",
    {SOC_SAND_BIT(9)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_VERBOSE_ID,
    "verbose",
    (PARAM_VAL_RULES *)&Sweep_app_on_off_vals[0],
    (sizeof(Sweep_app_on_off_vals) / sizeof(Sweep_app_on_off_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Sweep_app_off_default,
    0,0,0,0,0,0,0,
    "Verbose",
    "Examples:\r\n"
    "  sweep qos get fap# 0 verbose off\r\n"
    " Verbose make more prints.",
    "",
    { SOC_SAND_BIT(8) | SOC_SAND_BIT(9)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_DESTINATION_PORT_ID,
    "port#",
    (PARAM_VAL_RULES *)&Sweep_app_port_vals[0],
    (sizeof(Sweep_app_port_vals) / sizeof(Sweep_app_port_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)NULL,
    0,0,0,0,0,0,0,
    "Port to set/get. May be All ports.",
    "Examples:\r\n"
    "  sweep qos get fap# 0 verbose off\r\n"
    "Results in getting from FAP#0 port 1",
    "",
    { SOC_SAND_BIT(8) | SOC_SAND_BIT(9) },
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_FAP_DEVICE_ID,
    "fap#",
    (PARAM_VAL_RULES *)&Sweep_app_fap_vals[0],
    (sizeof(Sweep_app_fap_vals) / sizeof(Sweep_app_fap_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)NULL,
    0,0,0,0,0,0,0,
    "FAP to set/get. May be All faps.",
    "Examples:\r\n"
    "  sweep qos get fap# 0\r\n"
    "Results in getting from FAP#0 all ports",
    "",
    { SOC_SAND_BIT(8) | SOC_SAND_BIT(9) },
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_GRACEFUL_SHUTDOWN_FE_ID,
    "graceful_shutdown_fe",
    (PARAM_VAL_RULES *)&Sweep_app_empty_vals[0],
    (sizeof(Sweep_app_empty_vals) / sizeof(Sweep_app_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gracefully shuts down a single FE (by stopping reachability messages).",
    "Examples:\r\n"
    "  sweep graceful_shutdown_fe fe# 2\r\n"
    "Results in shutting down the second FE on the first fabric card\n\r"
    " (the lower card).",
    "",
    { SOC_SAND_BIT(5)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
    {
    PARAM_SWEEP_APP_GRACEFUL_RESTORE_FE_ID,
    "graceful_restore_fe",
    (PARAM_VAL_RULES *)&Sweep_app_empty_vals[0],
    (sizeof(Sweep_app_empty_vals) / sizeof(Sweep_app_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "restores a single registered FE (setting out of reset + restarting reachability messages).",
    "Examples:\r\n"
    "  sweep graceful_restore_fe fe# 1\r\n"
    "Results in restoring the second FE on the first fabric card\n\r"
    " (the lower card).",
    "",
    { SOC_SAND_BIT(5)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_GET_CONNECTIVITY_MAP_ID,
    "get_connectivity_map",
    (PARAM_VAL_RULES *)&Sweep_app_empty_vals[0],
    (sizeof(Sweep_app_empty_vals) / sizeof(Sweep_app_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gets the connectivity map of one of the FAPs.",
    "Examples:\r\n"
    "  sweep get_connectivity_map fap# 0\r\n"
    "Results in printing the connectivity map of the fap at the line card\n\r"
    " in the first slot (the upper card).",
    "",
    { SOC_SAND_BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_POWER_DOWN_SERDES_ID,
    "power_down_serdes",
    (PARAM_VAL_RULES *)&Sweep_app_empty_vals[0],
    (sizeof(Sweep_app_empty_vals) / sizeof(Sweep_app_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Powers down a single serdes of one of the FEs / FAPs.",
    "Examples:\r\n"
    "  sweep power_down_serdes fap# 1 serdes# 7\r\n"
    "Results in reseting the power of serdes #7 of the fap that is placed\n\r"
    " on the line card in slot #2 (the lower card).",
    "",
    { SOC_SAND_BIT(2)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_POWER_UP_SERDES_ID,
    "power_up_serdes",
    (PARAM_VAL_RULES *)&Sweep_app_empty_vals[0],
    (sizeof(Sweep_app_empty_vals) / sizeof(Sweep_app_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Powers up a single serdes of one of the FEs / FAPs.",
    "Examples:\r\n"
    "  sweep power_up_serdes fap# 1 serdes# 7\r\n"
    "Results in setting the power of serdes #7 of the fap that is placed\n\r"
    " on the line card in slot #2 (the lower card).",
    "",
    { SOC_SAND_BIT(2)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_FAP_DEVICE_ID,
    "fap#",
    (PARAM_VAL_RULES *)&Sweep_app_fap_no_vals[0],
    (sizeof(Sweep_app_fap_no_vals) / sizeof(Sweep_app_fap_no_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "identify the fap at get connectivity map command.",
    "Examples:\r\n"
    "  sweep get_connectivity_map fap# 0\r\n"
    "Results in printing the connectivity map of the fap at the line card\n\r"
    "in the first slot (upper card in Negev, leftmost card in Gobi).",
    "",
    { SOC_SAND_BIT(1) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_FE_DEVICE_ID,
    "fe#",
    (PARAM_VAL_RULES *)&Sweep_app_fe_no_vals[0],
    (sizeof(Sweep_app_fe_no_vals) / sizeof(Sweep_app_fe_no_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "identify the fe for operation. E.g., at power up/down serdes commands.",
    "Examples:\r\n"
    "  sweep power_up_serdes fe# 1 serdes# 7\r\n"
    "Results in setting the power of serdes #7 of the second fe in the fabric card.",
    "",
    { SOC_SAND_BIT(2) | SOC_SAND_BIT(5) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEP_APP_SERDES_ID,
    "serdes#",
    (PARAM_VAL_RULES *)&Sweep_app_serdes_no_vals[0],
    (sizeof(Sweep_app_serdes_no_vals) / sizeof(Sweep_app_serdes_no_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "identify the serdes at power up/down serdes commands.",
    "Examples:\r\n"
    "  sweep power_up_serdes fe# 1 serdes# 7\r\n"
    "Results in setting the power of serdes #7 of the second fe in the fabric card.",
    "",
    { SOC_SAND_BIT(2) },
    3,
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



/* } */


#ifdef _MSC_VER
  #pragma pack(pop)
#endif

#ifdef  __cplusplus
}
#endif


/* } __UI_ROM_DEFI_SWEEP_APP_B_H_INCLUDED__*/
#endif
