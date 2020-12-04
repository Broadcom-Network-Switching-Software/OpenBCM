/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UI_ROM_DEFI_FAP21V_APP_INCLUDED__
/* { */
#define __UI_ROM_DEFI_FAP21V_APP_INCLUDED__

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

#include <appl/dpp/UserInterface/ui_pure_defi_fap21v_app.h>

#include <Sweep/SOC_SAND_FAP21V/sweep21v_info.h>
#include <Sweep/SOC_SAND_FAP21V/sweep21v_sch_schemes.h>

/********************************************************/

EXTERN CONST
   PARAM_VAL_RULES
     Fap21v_app_free_vals[]
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

/********************************************************/

EXTERN CONST
   PARAM_VAL_RULES
     Fap21v_app_empty_vals[]
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
     Fap21v_SWEEP21V_SCH_HR_CLASS_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "class_1",
    {
      {
        SWEEP21V_SCH_FLOW_HR_CLASS_1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "class_2",
    {
      {
        SWEEP21V_SCH_FLOW_HR_CLASS_2,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "class_3_wfq",
    {
      {
        SWEEP21V_SCH_FLOW_HR_CLASS_3_WFQ,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "class_4_wfq",
    {
      {
        SWEEP21V_SCH_FLOW_HR_CLASS_4_WFQ,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "class_5",
    {
      {
        SWEEP21V_SCH_FLOW_HR_CLASS_5,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "class_6",
    {
      {
        SWEEP21V_SCH_FLOW_HR_CLASS_6,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "class_7",
    {
      {
        SWEEP21V_SCH_FLOW_HR_CLASS_7,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "class_8_wfq",
    {
      {
        SWEEP21V_SCH_FLOW_HR_CLASS_8_WFQ,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "class_9_wfq",
    {
      {
        SWEEP21V_SCH_FLOW_HR_CLASS_9_WFQ,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "class_be",
    {
      {
        SWEEP21V_SCH_FLOW_HR_CLASS_BE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "",
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
   PARAM_VAL_RULES
     Fap21v_SWEEP21V_SCH_SCHEME_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "general",
    {
      {
        SWEEP21V_SCH_SCHEME_GENERAL,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWEEP21V_SCH_SCHEME.SWEEP21V_SCH_SCHEME_GENERAL\r\n"
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
    "wfq_per_flow",
    {
      {
        SWEEP21V_SCH_SCHEME_WFQ_PER_FLOW,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWEEP21V_SCH_SCHEME.SWEEP21V_SCH_SCHEME_WFQ_PER_FLOW\r\n"
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
     fap21v_app_params[]
#ifdef INIT
   =
{
  {
    PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_FAP_NDX_ID,
    "fap_ndx",
    (PARAM_VAL_RULES *)&Fap21v_app_free_vals[0],
    (sizeof(Fap21v_app_free_vals) / sizeof(Fap21v_app_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
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
    PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_NOMINAL_RATE_MBPS_ID,
    "nominal_rate_kbps",
    (PARAM_VAL_RULES *)&Fap21v_app_free_vals[0],
    (sizeof(Fap21v_app_free_vals) / sizeof(Fap21v_app_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
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
    PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_SCHEME_ID_ID,
    "scheme_id",
    (PARAM_VAL_RULES *)&Fap21v_SWEEP21V_SCH_SCHEME_rule[0],
    (sizeof(Fap21v_SWEEP21V_SCH_SCHEME_rule) / sizeof(Fap21v_SWEEP21V_SCH_SCHEME_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
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
    PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_MINIMUM_RATE_BY_PERCENTAGE_ID,
    "minimum_rate_by_percentage",
    (PARAM_VAL_RULES *)&Fap21v_app_free_vals[0],
    (sizeof(Fap21v_app_free_vals) / sizeof(Fap21v_app_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
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
    PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_MAXIMUM_RATE_BY_PERCENTAGE_ID,
    "maximum_rate_by_percentage",
    (PARAM_VAL_RULES *)&Fap21v_app_free_vals[0],
    (sizeof(Fap21v_app_free_vals) / sizeof(Fap21v_app_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
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
    PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_PORT_NDX_ID,
    "port_ndx",
    (PARAM_VAL_RULES *)&Fap21v_app_free_vals[0],
    (sizeof(Fap21v_app_free_vals) / sizeof(Fap21v_app_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
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
  {
    PARAM_FAP21V_APP_SCHEDULING_SCHEMES_ID,
    "scheme",
    (PARAM_VAL_RULES *)&Fap21v_app_empty_vals[0],
    (sizeof(Fap21v_app_empty_vals) / sizeof(Fap21v_app_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
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
    PARAM_FAP21V_APP_SCHEDULING_SCHEMES_UPDATE_ID,
    "update",
    (PARAM_VAL_RULES *)&Fap21v_app_empty_vals[0],
    (sizeof(Fap21v_app_empty_vals) / sizeof(Fap21v_app_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
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
    PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_SWEEP21V_SCH_HR_CLASS_ID,
    "sp_cls",
    (PARAM_VAL_RULES *)&Fap21v_SWEEP21V_SCH_HR_CLASS_rule[0],
    (sizeof(Fap21v_SWEEP21V_SCH_HR_CLASS_rule) / sizeof(Fap21v_SWEEP21V_SCH_HR_CLASS_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
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
    PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_WEIGHT_ID,
    "weight",
    (PARAM_VAL_RULES *)&Fap21v_app_free_vals[0],
    (sizeof(Fap21v_app_free_vals) / sizeof(Fap21v_app_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
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
    PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_MAX_ID,
    "max_rate_percent",
    (PARAM_VAL_RULES *)&Fap21v_app_free_vals[0],
    (sizeof(Fap21v_app_free_vals) / sizeof(Fap21v_app_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
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
    PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_MIN_ID,
    "min_rate_percent",
    (PARAM_VAL_RULES *)&Fap21v_app_free_vals[0],
    (sizeof(Fap21v_app_free_vals) / sizeof(Fap21v_app_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
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
    PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_CLASS_NDX_ID,
    "cls_ndx",
    (PARAM_VAL_RULES *)&Fap21v_app_free_vals[0],
    (sizeof(Fap21v_app_free_vals) / sizeof(Fap21v_app_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
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
    PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_PORT_NDX_ID,
    "port_ndx",
    (PARAM_VAL_RULES *)&Fap21v_app_free_vals[0],
    (sizeof(Fap21v_app_free_vals) / sizeof(Fap21v_app_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
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
    PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_FAP_NDX_ID,
    "fap_ndx",
    (PARAM_VAL_RULES *)&Fap21v_app_free_vals[0],
    (sizeof(Fap21v_app_free_vals) / sizeof(Fap21v_app_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
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
    PARAM_FAP21V_APP_SCHEDULING_SCHEMES_GENERAL_UPDATE_ID,
      "gen_sch_update",
      (PARAM_VAL_RULES *)&Fap21v_app_empty_vals[0],
      (sizeof(Fap21v_app_empty_vals) / sizeof(Fap21v_app_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "",
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
    PARAM_FAP21V_APP_SSR_SAVE_ID,
      "save",
      (PARAM_VAL_RULES *)&Fap21v_app_empty_vals[0],
      (sizeof(Fap21v_app_empty_vals) / sizeof(Fap21v_app_empty_vals[0])) - 1,
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
    PARAM_FAP21V_APP_SSR_RESTORE_ID,
      "restore",
      (PARAM_VAL_RULES *)&Fap21v_app_empty_vals[0],
      (sizeof(Fap21v_app_empty_vals) / sizeof(Fap21v_app_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "",
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
    PARAM_FAP21V_APP_SSR_ID,
      "ssr",
      (PARAM_VAL_RULES *)&Fap21v_app_empty_vals[0],
      (sizeof(Fap21v_app_empty_vals) / sizeof(Fap21v_app_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "",
      "",
      "",
    {BIT(2)|BIT(3)},
    1,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_APP_FE_GRACEFUL_RESTORE_FE_ID,
      "fe_ndx",
      (PARAM_VAL_RULES *)&Fap21v_app_free_vals[0],
      (sizeof(Fap21v_app_free_vals) / sizeof(Fap21v_app_free_vals[0])) - 1,
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
    PARAM_FAP21V_APP_FE_GRACEFUL_RESTORE_ID,
      "graceful_restore",
      (PARAM_VAL_RULES *)&Fap21v_app_empty_vals[0],
      (sizeof(Fap21v_app_empty_vals) / sizeof(Fap21v_app_empty_vals[0])) - 1,
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
    PARAM_FAP21V_APP_FE_GRACEFUL_SHUT_DOWN_FE_ID,
      "fe_ndx",
      (PARAM_VAL_RULES *)&Fap21v_app_free_vals[0],
      (sizeof(Fap21v_app_free_vals) / sizeof(Fap21v_app_free_vals[0])) - 1,
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
    PARAM_FAP21V_APP_FE_GRACEFUL_SHUT_DOWN_ID,
      "graceful_shut_down",
      (PARAM_VAL_RULES *)&Fap21v_app_empty_vals[0],
      (sizeof(Fap21v_app_empty_vals) / sizeof(Fap21v_app_empty_vals[0])) - 1,
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
    PARAM_FAP21V_APP_FE_ID,
      "fe",
      (PARAM_VAL_RULES *)&Fap21v_app_empty_vals[0],
      (sizeof(Fap21v_app_empty_vals) / sizeof(Fap21v_app_empty_vals[0])) - 1,
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
  {
    PARAM_FAP21V_APP_LINKS_ID,
      "links",
      (PARAM_VAL_RULES *)&Fap21v_app_empty_vals[0],
      (sizeof(Fap21v_app_empty_vals) / sizeof(Fap21v_app_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "",
      "",
      "",
    {BIT(6)|BIT(7)|BIT(8)},
    1,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_APP_CONNECTIVITY_MAP_GET_ID,
      "connectivity_map_get",
      (PARAM_VAL_RULES *)&Fap21v_app_empty_vals[0],
      (sizeof(Fap21v_app_empty_vals) / sizeof(Fap21v_app_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "",
      "",
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
    PARAM_FAP21V_APP_CONNECTIVITY_MAP_GET_FAP_NDX_ID,
      "fap_ndx",
      (PARAM_VAL_RULES *)&Fap21v_app_free_vals[0],
      (sizeof(Fap21v_app_free_vals) / sizeof(Fap21v_app_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "",
      "",
      "",
    {BIT(6)},
    3,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_APP_SERDES_POWER_ID,
      "serdes_power",
      (PARAM_VAL_RULES *)&Fap21v_app_empty_vals[0],
      (sizeof(Fap21v_app_empty_vals) / sizeof(Fap21v_app_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "",
      "",
      "",
    {BIT(7)|BIT(8)},
    2,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_APP_SERDES_POWER_DOWN_ID,
      "down",
      (PARAM_VAL_RULES *)&Fap21v_app_empty_vals[0],
      (sizeof(Fap21v_app_empty_vals) / sizeof(Fap21v_app_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "",
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
    PARAM_FAP21V_APP_SERDES_POWER_DOWN_FE_NDX_ID,
      "fap_ndx",
      (PARAM_VAL_RULES *)&Fap21v_app_free_vals[0],
      (sizeof(Fap21v_app_free_vals) / sizeof(Fap21v_app_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "",
      "",
      "",
    {BIT(7)},
    4,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_APP_SERDES_POWER_DOWN_SERDES_NDX_ID,
      "serdes_ndx",
      (PARAM_VAL_RULES *)&Fap21v_app_free_vals[0],
      (sizeof(Fap21v_app_free_vals) / sizeof(Fap21v_app_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "",
      "",
      "",
    {BIT(7)},
    5,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_APP_SERDES_POWER_UP_ID,
      "up",
      (PARAM_VAL_RULES *)&Fap21v_app_empty_vals[0],
      (sizeof(Fap21v_app_empty_vals) / sizeof(Fap21v_app_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "",
      "",
      "",
    {BIT(8)},
    3,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_APP_SERDES_POWER_UP_FE_NDX_ID,
      "fap_ndx",
      (PARAM_VAL_RULES *)&Fap21v_app_free_vals[0],
      (sizeof(Fap21v_app_free_vals) / sizeof(Fap21v_app_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "",
      "",
      "",
    {BIT(8)},
    4,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP21V_APP_SERDES_POWER_UP_SERDES_NDX_ID,
      "serdes_ndx",
      (PARAM_VAL_RULES *)&Fap21v_app_free_vals[0],
      (sizeof(Fap21v_app_free_vals) / sizeof(Fap21v_app_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "",
      "",
      "",
    {BIT(8)},
    5,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
  },
  /*
   * Last element. Do not remove. Last {BIT(5)},
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

#endif    /* } __UI_ROM_DEFI_FAP21V_APP_INCLUDED__*/
