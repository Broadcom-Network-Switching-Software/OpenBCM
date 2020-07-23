/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifndef UI_ROM_DEFI_FAP20V_B_INCLUDED
/* { */
#define UI_ROM_DEFI_FAP20V_B_INCLUDED
/*
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>
/*
 * User Interface include file.
 */
#include <appl/diag/dpp/dune_chips.h>
#include <appl/diag/dpp/ui_defx.h>
#include <appl/diag/dpp/utils_defx.h>

#include <soc/dpp/SAND/Management/sand_chip_consts.h>

#ifndef __DUNE_HRP__
/* { */
  #include <ChipSim/chip_sim_fe200_chip_defines_indirect.h>
  #include <ChipSim/chip_sim.h>
  #include <soc/dpp/SOC_SAND_FAP10M/fap10m_chip_defines.h>
  #include <appl/diag/dpp/dune_rpc.h>
/* } */
#endif

#include <soc/dpp/SOC_SAND_FAP20V/fap20v_api_general.h>
#include <soc/dpp/SOC_SAND_FAP20V/fap20v_api_statistics.h>
#include <soc/dpp/SOC_SAND_FAP20V/fap20v_api_spatial_multicast.h>
#include <soc/dpp/SOC_SAND_FAP20V/fap20v_api_configuration.h>
#include <soc/dpp/SOC_SAND_FAP20V/fap20v_api_scheduler_flows.h>
#include <soc/dpp/SOC_SAND_FAP20V/fap20v_b_api_scheduler.h>
#include <soc/dpp/SOC_SAND_FAP20V/fap20v_b_api_egress_ports.h>

#include <soc/dpp/SOC_SAND_FAP20V/fap20v_chip_defines.h>
#include <soc/dpp/SOC_SAND_FAP20V/fap20v_configuration.h>
#include <soc/dpp/SOC_SAND_FAP20V/fap20v_api_egress_tm.h>

#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>

#include <ui_pure_defi.h>
#include <ui_pure_defi_fap20v.h>
/*
#define CONST const
*/
/*
 * UI definition{
 */

#define UI_FAP20V_B_PORT_MAX_CREDIT 17000

EXTERN CONST
   PARAM_VAL_RULES
     Fap20v_b_queue_types[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP20V_B_NOF_QUEUE_TYPES-1,FAP20V_QUEUE_A,
          /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        FAP20V_FABRIC_UNICAST_CLASSES,
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
     Fap20v_percentage_vals[]
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
EXTERN CONST
   PARAM_VAL_RULES
     Fap20v_b_port_credit_rate[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        UI_FAP20V_B_PORT_MAX_CREDIT,0,
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
     Fap20v_burst_size_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "size_64",
    {
      {
        FAP20V_B_SPI_BURST_SIZE_64,
        (long)"    64 Bytes.",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "size_128",
    {
      {
        FAP20V_B_SPI_BURST_SIZE_128,
        (long)"    128 Bytes.",
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

EXTERN CONST
   PARAM_VAL_RULES
     Fap20v_b_queue_prio[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "low",
    {
      {
        0,
        (long)"    Low.",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "high",
    {
      {
        1,
        (long)"    High.",
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

EXTERN CONST
   PARAM_VAL_RULES
     Fap20v_b_port_prio[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "low",
    {
      {
        FAP20V_B_EGRESS_PORT_PRIO_LOW,
        (long)"    Lowest.",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "medium",
    {
      {
        FAP20V_B_EGRESS_PORT_PRIO_MEDIUM,
        (long)"    Medium.",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "high",
    {
      {
        FAP20V_B_EGRESS_PORT_PRIO_HIGH,
        (long)"    Highest.",
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


EXTERN CONST
   PARAM_VAL_RULES
     Fap20v_b_port_sch_weight[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP20V_MAX_EGRESS_QUEUE_WEIGHT,0,
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
     Fap20v_device_if_vals_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP20V_B_SCH_SPIB_WEIGHT_MAX,0,
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
     Fap20v_b_discrete_weight_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP20V_B_SCH_SCT_AF1_WEIGHT_MAX,0,
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
     Fap20v_b_cl_type_id_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP20V_B_SCH_NOF_CLASS_TYPES,0,
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
     Fap20v_b_type_mode_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "mode_1",
    {
      {
        FAP20V_B_CL_SCHED_CLASS_MODE_1,
        (long)"    4 levels SP : FQ > FQ > FQ > FQ.",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "mode_2",
    {
      {
        FAP20V_B_CL_SCHED_CLASS_MODE_2,
        (long)"    3 levels SP : FQ > FQ > WFQ with n = 2.",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "mode_3",
    {
      {
        FAP20V_B_CL_SCHED_CLASS_MODE_3,
        (long)"    2 levels SP : WFQ with n = 3 or x = 64 > FQ.",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "mode_4",
    {
      {
        FAP20V_B_CL_SCHED_CLASS_MODE_4,
        (long)"    2 levels SP : FQ > WFQ with n = 3 or x = 64.",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "mode_5",
    {
      {
        FAP20V_B_CL_SCHED_CLASS_MODE_5,
        (long)"    1 level  SP : WFQ with n = 4 or x = 254.",
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

EXTERN CONST
   PARAM_VAL_RULES
     Fap20v_b_weight_mode_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "independent_per_flow",
    {
      {
        FAP20V_B_CLASS_WEIGHTS_MODE_INDEPENDENT_PER_FLOW,
        (long)"    Regular.",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "discrete_per_flow",
    {
      {
        FAP20V_B_CLASS_WEIGHTS_MODE_DISCRETE_PER_FLOW,
        (long)"    discrete per flow.",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "discrete_per_class",
    {
      {
        FAP20V_B_CLASS_WEIGHTS_MODE_DISCRETE_PER_CLASS,
        (long)"    discrete per class.",
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


EXTERN CONST
   PARAM_VAL_RULES
     Fap20v_b_hr_mode[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "single",
    {
      {
        FAP20V_B_HR_SCHED_MODE_SINGLE_WFQ,
        (long)"    HR scheduler single mode.",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "dual",
    {
      {
        FAP20V_B_HR_SCHED_MODE_DUAL_WFQ,
        (long)"    HR scheduler dual mode.",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "enhanced",
    {
      {
        FAP20V_B_HR_SCHED_MODE_ENHANCED_PRIO_WFQ,
        (long)"    HR scheduler enhanced mode.",
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

EXTERN CONST
   PARAM_VAL_RULES
     Fap20v_b_supported_mode_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "dual",
    {
      {
        FAP20V_B_HR_SCHED_MODE_DUAL_WFQ,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    HR scheduler dual mode.",
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
    "enhanced",
    {
      {
        FAP20V_B_HR_SCHED_MODE_ENHANCED_PRIO_WFQ,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    HR scheduler enhanced mode.",
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
     Fap20v_b_elementary_scheduler_state[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "enabled",
    {
      {
        FAP20V_B_ELEMENTARY_SCHEDULER_STATE_ENABLE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    enabled.",
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
    "disabled",
    {
      {
        FAP20V_B_ELEMENTARY_SCHEDULER_STATE_DISABLE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    disabled.",
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
     Fap20v_b_sch_cl_type_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP20V_B_SCH_NOF_CLASS_TYPES-1,0,
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
     Fap20v_rci_level_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP20V_B_NOF_RCI_LEVELS-1,0,
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
     Fap20v_nof_active_links_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP20V_NUMBER_OF_LINKS,0,
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
     Fap20v_sch_agg_id_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP20V_B_MAX_FQ_SCHEDULER_ID,0,
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
     Fap20v_b_sub_flow_weight_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP20V_B_SUB_FLOW_HR_MAX_WEIGHT,0,
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
     Fap20v_b_sch_agg_type_vals[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "hr_class_ef1",
    {
      {
        FAP20V_B_SUB_FLOW_HR_CLASS_EF1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    HR .",
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
    "hr_class_ef2",
    {
      {
        FAP20V_B_SUB_FLOW_HR_CLASS_EF2,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    HR .",
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
    "hr_class_ef3",
    {
      {
        FAP20V_B_SUB_FLOW_HR_CLASS_EF3,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    HR .",
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
    "hr_single_class_af1_wfq",
    {
      {
        FAP20V_B_SUB_FLOW_HR_SINGLE_CLASS_AF1_WFQ,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    HR .",
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
    "hr_single_class_be1",
    {
      {
        FAP20V_B_SUB_FLOW_HR_SINGLE_CLASS_BE1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    HR .",
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
    "hr_dual_class_af1_wfq",
    {
      {
        FAP20V_B_SUB_FLOW_HR_DUAL_CLASS_AF1_WFQ,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    HR .",
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
    "hr_dual_class_be1_wfq",
    {
      {
        FAP20V_B_SUB_FLOW_HR_DUAL_CLASS_BE1_WFQ,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    HR .",
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
    "hr_dual_class_be2",
    {
      {
        FAP20V_B_SUB_FLOW_HR_DUAL_CLASS_BE2,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    HR .",
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
    "hr_enhanced_class_af1",
    {
      {
        FAP20V_B_SUB_FLOW_HR_ENHANCED_CLASS_AF1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    HR .",
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
    "hr_enhanced_class_af2",
    {
      {
        FAP20V_B_SUB_FLOW_HR_ENHANCED_CLASS_AF2,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    HR .",
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
    "hr_enhanced_class_af3",
    {
      {
        FAP20V_B_SUB_FLOW_HR_ENHANCED_CLASS_AF3,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    HR .",
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
    "hr_enhanced_class_af4",
    {
      {
        FAP20V_B_SUB_FLOW_HR_ENHANCED_CLASS_AF4,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    HR .",
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
    "hr_enhanced_class_af5",
    {
      {
        FAP20V_B_SUB_FLOW_HR_ENHANCED_CLASS_AF5,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    HR .",
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
    "hr_enhanced_class_af6",
    {
      {
        FAP20V_B_SUB_FLOW_HR_ENHANCED_CLASS_AF6,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    HR .",
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
    "hr_enhanced_class_be1_wfq",
    {
      {
        FAP20V_B_SUB_FLOW_HR_ENHANCED_CLASS_BE1_WFQ,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    HR .",
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
    "hr_enhanced_class_be2",
    {
      {
        FAP20V_B_SUB_FLOW_HR_ENHANCED_CLASS_BE2,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    HR .",
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
    "cl_class_sp1",
    {
      {
        FAP20V_B_SUB_FLOW_CL_CLASS_SP1 + 100,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    CL .",
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
    "cl_class_sp2",
    {
      {
        FAP20V_B_SUB_FLOW_CL_CLASS_SP2 + 100,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    CL .",
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
    "cl_class_sp3",
    {
      {
        FAP20V_B_SUB_FLOW_CL_CLASS_SP3 + 100,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    CL .",
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
    "cl_class_sp4",
    {
      {
        FAP20V_B_SUB_FLOW_CL_CLASS_SP4 + 100,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    CL .",
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
    "cl_class_sp1_wfq",
    {
      {
        FAP20V_B_SUB_FLOW_CL_CLASS_SP1_WFQ + 100,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    CL .",
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
    "cl_class_sp1_wfq1",
    {
      {
        FAP20V_B_SUB_FLOW_CL_CLASS_SP1_WFQ1 + 100,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    CL .",
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
    "cl_class_sp1_wfq2",
    {
      {
        FAP20V_B_SUB_FLOW_CL_CLASS_SP1_WFQ2 + 100,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    CL .",
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
    "cl_class_sp1_wfq3",
    {
      {
        FAP20V_B_SUB_FLOW_CL_CLASS_SP1_WFQ3 + 100,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    CL .",
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
    "cl_class_sp1_wfq4",
    {
      {
        FAP20V_B_SUB_FLOW_CL_CLASS_SP1_WFQ4 + 100,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    CL .",
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
    "cl_class_sp2_wfq",
    {
      {
        FAP20V_B_SUB_FLOW_CL_CLASS_SP2_WFQ + 100,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    CL .",
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
    "cl_class_sp2_wfq1",
    {
      {
        FAP20V_B_SUB_FLOW_CL_CLASS_SP2_WFQ1 + 100,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    CL .",
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
    "cl_class_sp2_wfq2",
    {
      {
        FAP20V_B_SUB_FLOW_CL_CLASS_SP2_WFQ2 + 100,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    CL .",
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
    "cl_class_sp2_wfq3",
    {
      {
        FAP20V_B_SUB_FLOW_CL_CLASS_SP2_WFQ3 + 100,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    CL .",
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
    "cl_class_sp3_wfq1",
    {
      {
        FAP20V_B_SUB_FLOW_CL_CLASS_SP3_WFQ1 + 100,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    CL .",
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
    "cl_class_sp3_wfq2",
    {
      {
        FAP20V_B_SUB_FLOW_CL_CLASS_SP3_WFQ2 + 100,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    CL .",
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
    "fq",
    {
      {
        200,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    FQ .",
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
     Fap20v_b_egq_type[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP20V_EG_Q_3,FAP20V_EG_Q_0,
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
     Fap20v_b_egs_max_port_credit[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FAP20V_EGQ_PORT_MAX_CREDIT_MAX,0,
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
     Fap20v_b_true_false_table[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "true",
    {
      {
        TRUE,
        (long)"    true.",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "false",
    {
      {
        FALSE,
        (long)"    false.",
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

EXTERN CONST
   PARAM_VAL_RULES
     Fap20v_b_egs_mci_type[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        1,0,
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
*  Fap20v_b_params
*TYPE: BUFFER
*DATE: 29/Dec/2002
*FUNCTION:
*  ARRAY CONTAINING THE INITIAL LIST OF PARAMETERS
*  RELATED TO THE SUBJECT "SOC_SAND_FAP20V".
*ORGANIZATION:
*  ARRAY OF STRUCTURES OF TYPE 'PARAM':
*    int   subject_id -
*      Identifier of this parameter.
*    char *par_name -
*      Ascii presentation of parameter. Must be small
*      letters only! Must contain at least one character.
*    PARAM_VAL *allowed_vals -
*      Array of structures of type PARAM_VAL. This is
*      the list of values allowed for that
*      parameter.
*    int default_care -
*      Flags bitmap.
*      Bit(0) indicates whether there is a default
*      value for this parameter (i.e. 'default_val'
*      is meaningful). '0' indicates 'no default val'.
*      Bit(1) indicates whether this parameter must
*      or may not appear on command line (within its
*      context). '0' indicates 'may not appear'.
*    PARAM_VAL *default_val -
*      Default value for this parameter.
*    unsigned int num_numeric_vals ;
*      Number of numeric values in array 'allowed_vals'.
*    unsigned int num_symbolic_vals ;
*      Number of symbolic values in array 'allowed_vals'.
*    unsigned int num_text_vals ;
*      Number of free text values in array 'allowed_vals'.
*    unsigned int num_ip_vals ;
*      Number of IP address values in array 'allowed_vals'.
*    unsigned int numeric_index ;
*      Index of numeric value (if there is any) in
*      array 'allowed_vals'.
*    unsigned int text_index ;
*      Index of free text value (if there is any) in
*      array 'allowed_vals'.
*    unsigned int ip_index ;
*      Index of IP address value (if there is any) in
*      array 'allowed_vals'.
*    char  *param_short_help -
*      Short help for this parameter.
*    char  *param_detailed_help -
*      Detailed help for this parameter.
*    char  *vals_detailed_help ;
*      Detailed help for all values attached to this parameter.
*      THIS FIELD IS AUTOMATICALLY FILLED IN BY THE SYSTEM
*      and does NOT need to be filled in by the user.
*    unsigned long mutex_control ;
*      Mutual exclusion syntax control. Up to 32 groups
*      specified by bits. If a parameter has specific
*      bits set then other parameters, with any of
*      these bits set, may not be on the same line.
*USAGE:
*  SEE ORGANIZATION.
*REMARKS:
*  NONE.
*SEE ALSO:
 */
EXTERN CONST
   PARAM
     Fap20v_b_params[]
#ifdef INIT
   =
{
  {
    PARAM_FAP20V_GENRAL_API_ID,
    "general",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver general services.",
    "Examples:\r\n"
    "  fap20v general start_driver\r\n"
    "Results in starting the fap20v driver.\r\n",
    "",
    {BIT(0) | BIT(1) | BIT(2) | BIT(4) | BIT(5) | BIT(26) | BIT(28) | BIT(29) | BIT(30) },
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_DEFEREED_API_ID,
    "deferred",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver deferred services.",
    "Examples:\r\n"
    "  fap20v deferred physical_read 0x1 offset 0x0 size 0x8 polling_rate 50 \r\n"
    "Results in reading data of 64 bit at offset 0x0 of device 0x1 every 50 system ticks.\r\n",
    "",
    {BIT(3) | BIT(6) | BIT(9) | BIT(10) | BIT(13) | BIT(15) | BIT(18) | BIT(24)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_PRINT_DRIVER_INFORMATION_ID,
    "print_driver_information",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Prints driver specific information.\r\n",
    "Examples:\r\n"
    "  fap20v general print_driver_information \r\n"
    "Results in printing SOC_SAND_FAP20V driver information.\r\n",
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
    PARAM_FAP20V_START_DRIVER_ID,
    "driver_start",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts the Fap20v driver.\r\n",
    "Examples:\r\n"
    "  fap20v general start_driver\r\n"
    "Results in starting the fap20v driver.\r\n",
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
    PARAM_FAP20V_SHUTDOWN_DRIVER_ID,
    "driver_shutdown",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Shuts down Fap20v driver completly.\r\n",
    "Examples:\r\n"
    "  fap20v general shutdown_driver\r\n"
    "Results in shutting down the fap20v driver.\r\n",
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
    PARAM_FAP20V_REGISTER_DEVICE_ID,
    "register_device",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "registers an Fap20v device with the driver.\r\n",
    "Examples:\r\n"
    "  fap20v register_device 0x30043745 \r\n"
    "Results in registering an fap20v device at address 0x30043745 with the driver.\r\n",
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
    PARAM_FAP20V_UNREGISTER_DEVICE_ID,
    "unregister_device",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "unregisters an Fap20v device from the driver.\r\n",
    "Examples:\r\n"
    "  fap20v unregister_device 0x1 \r\n"
    "Results in unregistering an fap20v device that was registered under the handle 0x1.\r\n",
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
    PARAM_FAP20V_RESET_DEVICE_ID,
    "reset_device",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "resets a SOC_SAND_FAP20V device.\r\n",
    "Examples:\r\n"
    "  fap20v general reset_device 0x1\r\n"
    "Results in resetting device 0x1 (off-on).\r\n",
    "",
    {BIT(26) | BIT(28)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_FAP_ID_ID,
    "new_fap_id",
    (PARAM_VAL_RULES *)&Fap20v_fap_id[0],
    (sizeof(Fap20v_fap_id) / sizeof(Fap20v_fap_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR, (PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "New FAP-ID to set in the init sequnce.\r\n",
    "Examples:\r\n"
    "  fe200 general reset_device 0x1 new_fap_id 3\r\n"
    "Results in shutting of device 0x1 and setting the FAP-ID to be 3.\r\n",
    "",
    {BIT(28)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_REMAIN_OFF_ID,
    "remain_off",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR, (PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "If appears the device will stay in reset.\r\n",
    "Examples:\r\n"
    "  fap20v general reset_device 0x1 remain_off\r\n"
    "Results in shutting of device 0x1.\r\n",
    "",
    {BIT(26)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_START_AND_REGISTER_ID,
    "start_driver_and_register_devices",
    (PARAM_VAL_RULES *)&Fap20v_Interrupt_mockup_vals[0],
    (sizeof(Fap20v_Interrupt_mockup_vals) / sizeof(Fap20v_Interrupt_mockup_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Fap20v_Interrupt_mockup_default,
    0,0,0,0,0,0,0,
    "starts the fap20v driver, simulator and register 2 devices (with or without ISR).\r\n",
    "Examples:\r\n"
    "  fap20v start_driver_and_register_devices mockup_inetrrupts\r\n"
    "Results in activating the driver, the simulator and registering 2 devices,\n\r"
    "  and simulating interrupts by polling.\r\n",
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
    PARAM_FAP20V_UNREGISTER_AND_SHUTDOWN_ID,
    "unregister_devices_and_shutdown_driver",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "unregister 2 fap20v devices, ends the simulator and shutdown the driver.\r\n",
    "Examples:\r\n"
    "  fap20v unregister_devices_and_shutdown_driver \r\n"
    "Results in unregistering the devices and shutting down the simulator and the driver.\r\n",
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
    PARAM_FAP20V_CLEAR_DEVICE_PENDING_SERVICES_ID,
    "clear_device_pending_services",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "clears all pending services of an Fap20v device.\r\n",
    "Examples:\r\n"
    "  fap20v clear_device_pending_services 0x1 \r\n"
    "Results in clearing all services registered for an fap20v device with the handle 0x1.\r\n",
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
    PARAM_FAP20V_STOP_POLLING_TRANSACTON_ID,
    "stop_polling_transaction",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "unregister one deferred service accroding to its handle.\r\n",
    "Examples:\r\n"
    "  fap20v stop_polling_transaction 0x28342359 \r\n"
    "Results in unregistered the service with handle 0x28342359.\r\n",
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
    PARAM_FAP20V_SILENT_ID,
    "silent",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "If appears - less printouts as possible.\r\n",
    "Examples:\r\n"
    "  fap20v general physical_write 0x0 offset 0x4 data 0x2233 silent\r\n"
    "Results in writing data of 32 bit at offset 0x4 of device 0 - with no printing.\r\n",
    "",
    {BIT(1) | BIT(2)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_PHYSICAL_WRITE_ID,
    "physical_write",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "writes data to the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fap20v general physical_write 0x1 offset 0x8 size 0x4 data 0x2233 \r\n"
    "Results in writing data of 32 bit at offset 0x8 of device 0x1.\r\n",
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
    PARAM_FAP20V_PHYSICAL_READ_ID,
    "physical_read",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "reads data from the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fap20v general physical_read 0x1 offset 0x8 size 0x4 \r\n"
    "Results in reading data of 32 bit at offset 0x8 of device 0x1.\r\n",
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
    PARAM_FAP20V_DEFERRED_PHYSICAL_READ_ID,
    "physical_read",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "deferred read of data from the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fap20v deferred physical_read 0x1 offset 0x0 size 0x8 polling_rate 50 \r\n"
    "Results in reading data of 64 bit at offset 0x0 of device 0x1 every 50 system ticks.\r\n",
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
    PARAM_FAP20V_LOGICAL_WRITE_ID,
    "logical_write",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "writes data to the specified device at the register / field.\r\n",
    "Examples:\r\n"
    "  fap20v general logical_write 0x1 field chip_id data 0x3 \r\n"
    "Results in writing the value 0x3 to the chip_id field of fe 0x1.\r\n",
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
    PARAM_FAP20V_LOGICAL_READ_ID,
    "logical_read",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "reads specified register / field from the device.\r\n",
    "Examples:\r\n"
    "  fap20v general logical_read 0x1 field chip_id \r\n"
    "Results in reading the value of the chip_id field from fe 0x1.\r\n",
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
    PARAM_FAP20V_DEFERRED_LOGICAL_READ_ID,
    "logical_read",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "deferred read of the specified register / field from the device.\r\n",
    "Examples:\r\n"
    "  fap20v deferred logical_read 0x1 field chip_id polling_rate 50\r\n"
    "Results in polling of the field chip_id field from fe 0x1, every 50 time ticks.\r\n",
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
    PARAM_FAP20V_PHYSICAL_DIRECT_ID,
    "direct",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "a part of read / write data to the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fap20v general physical_write direct 0x1 offset 0x8 size 0x4 data 0x2233 \r\n"
    "Results in writing data of 32 bit at offset 0x8 of device 0x1.\r\n",
    "",
    {BIT(1) | BIT(2) | BIT(3)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_PHYSICAL_INDIRECT_ID,
    "indirect",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "a part of read / write data to the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fap20v general physical_write indirect 0x1 offset 0x8 size 0x4 data 0x2233 \r\n"
    "Results in indirect writing data of 32 bit at offset 0x8 of device 0x1.\r\n",
    "",
    {BIT(1) | BIT(2) | BIT(3)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_PHYSICAL_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "a part of read / write data to the specified device at the specified offset.\r\n"
    "The first byte (bits 30:27) of the offset indicate the module:\n\r"
    "  0x00:spi4_0, 0x08:spi4_1, 0x10:egq, 0x18:lbp, 0x20:qdp, 0x28:rtp, \r\n"
    "  0x30:fdt,    0x40:sch,    0x48:sla, 0x50:mmu, 0x58:inq.           \r\n",
    "Examples:\r\n"
    "  fap20v general physical_write direct 0x1 offset 0x8 size 0x4 data 0x2233 \r\n"
    "Results in writing data of 32 bit at offset 0x8 of device 0x1.\r\n",
    "",
    {BIT(1) | BIT(2) | BIT(3) | BIT(29) | BIT(30)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_PHYSICAL_SIZE_ID,
    "size",
    (PARAM_VAL_RULES *)&Fap20v_data_size_vals[0],
    (sizeof(Fap20v_data_size_vals) / sizeof(Fap20v_data_size_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Fap20v_read_size_default,
    0,0,0,0,0,0,0,
    "a part of read / write data to the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fap20v general physical_write 0x1 direct offset 0x8 size 0x4 data 0x2233 \r\n"
    "Results in writing data of 32 bit at offset 0x8 of device 0x1.\r\n",
    "",
    { BIT(2) | BIT(3)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_NUM_OF_TIMES_ID,
    "nof_times",
    (PARAM_VAL_RULES *)&Fap20v_nof_times_mem_read_vals[0],
    (sizeof(Fap20v_nof_times_mem_read_vals) / sizeof(Fap20v_nof_times_mem_read_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Fap20v_nof_times_mem_read_default,
    0,0,0,0,0,0,0,
    "Re-current read. 'nof_times' times.",
    "Examples:\r\n"
    "  fap20v general physical_read 0x1 direct offset 0x8 size 0x4 nof_times 12\r\n"
    "Results in reading, 12 times, starting at offset 0x8.\r\n",
    "",
    {BIT(2)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_MEM_PAUSE_AFTER_READ_ID,
    "pause_after_read",
    (PARAM_VAL_RULES *)&Fap20v_pause_after_read_mem_read_vals[0],
    (sizeof(Fap20v_pause_after_read_mem_read_vals) / sizeof(Fap20v_pause_after_read_mem_read_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Fap20v_pause_after_read_mem_read_default,
    0,0,0,0,0,0,0,
    "Pause for amount of time after read (micro-sec). (Useful with 'nof_times' parameter.)",
    "Examples:\r\n"
    "  fap20v general physical_read 0x1 direct offset 0x8 size 0x4 nof_times 12 pause_after_read 10\r\n"
    "Results in reading, 12 times, starting at offset 0x8.\r\n"
    "After every read the CPU will halt for 10 micro-sec \n\r",
    "",
    {BIT(2)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_PHYSICAL_DATA_ID,
    "data",
    (PARAM_VAL_RULES *)&Fap20v_data_values_vals[0],
    (sizeof(Fap20v_data_values_vals) / sizeof(Fap20v_data_values_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "a part of read / write data to the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fap20v general physical_write 0x1 direct offset 0x8 size 0x4 data 0x2233 \r\n"
    "Results in writing data of 32 bit at offset 0x8 of device 0x1.\r\n",
    "",
    {BIT(1) | BIT(4) | BIT(29)},
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_POLLING_RATE_ID,
    "polling_rate",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "a part of a deferred read of data to the specified device at the specified offset.\r\n",
    "Examples:\r\n"
    "  fap20v deferred physical_read 0x1 offset 0x0 size 0x8 polling_rate 50 \r\n"
    "Results in reading data of 64 bit at offset 0x0 of device 0x1 every 50 system ticks.\r\n",
    "",
    {BIT(3) | BIT(6) | BIT(9) | BIT(13) | BIT(15) | BIT(18) | BIT(24)},
    7,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_FAP20V_FIELD_ID,
    "interrupt",
    (PARAM_VAL_RULES *)&Fap20v_logical_interrupt_mask_field[0],
    (sizeof(Fap20v_logical_interrupt_mask_field) / sizeof(Fap20v_logical_interrupt_mask_field[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interrupt group of registers/fields.\r\n",
    "Examples:\r\n"
    "  fap20v general logical_write 0x1 interrupt src_dv_cng_link_mask 0x1\r\n"
    "Results in writing the value 0x1 to the src_dv_cng_link_mask field of device 0x1.\r\n",
    "",
    {BIT(4) | BIT(5) | BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_FIELD_ID,
    "init_trig",
    (PARAM_VAL_RULES *)&Fap20v_logical_initialization_triggers_field[0],
    (sizeof(Fap20v_logical_initialization_triggers_field) / sizeof(Fap20v_logical_initialization_triggers_field[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Initialization triggers group of registers/fields.\r\n",
    "Examples:\r\n"
    "  fap20v general logical_write 0x1 init_trig queue_controller_init_trig 0x1\r\n"
    "Results in writing the value 0x1 to the queue_controller_init_trig field of device 0x1.\r\n",
    "",
    {BIT(4) | BIT(5) | BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_FIELD_ID,
    "sms",
    (PARAM_VAL_RULES *)&Fap20v_logical_spatial_multicast_scheduler_field[0],
    (sizeof(Fap20v_logical_spatial_multicast_scheduler_field) / sizeof(Fap20v_logical_spatial_multicast_scheduler_field[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Spatial Multicast Scheduler group of registers/fields.\r\n",
    "Examples:\r\n"
    "  fap20v general logical_write 0x1 misc unicast_weight 0x3\r\n"
    "Results in writing the value 0x3 to the unicast_weight field of device 0x1.\r\n",
    "",
    {BIT(4) | BIT(5) | BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_FIELD_ID,
    "label_processor",
    (PARAM_VAL_RULES *)&Fap20v_label_processor_field[0],
    (sizeof(Fap20v_label_processor_field) / sizeof(Fap20v_label_processor_field[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "label-processor group of registers/fields.\r\n",
    "Examples:\r\n"
    "  fap20v general logical_write 0x1 label_processor sig_class_bits 0x3\r\n"
    "Results in writing the value 0x3 to the 'sig_class_bits' field of device 0x1.\r\n",
    "",
    {BIT(4) | BIT(5) | BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_FIELD_ID,
    "misc",
    (PARAM_VAL_RULES *)&Fap20v_logical_misc_field[0],
    (sizeof(Fap20v_logical_misc_field) / sizeof(Fap20v_logical_misc_field[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "miscellaneous group of registers/fields.\r\n",
    "Examples:\r\n"
    "  fap20v general logical_write 0x1 misc chip_id 0x3\r\n"
    "Results in writing the value 0x3 to the chip_id field of device 0x1.\r\n",
    "",
    {BIT(4) | BIT(5) | BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_FIELD_ID,
    "queue_controller",
    (PARAM_VAL_RULES *)&Fap20v_logical_external_dram_field[0],
    (sizeof(Fap20v_logical_external_dram_field) / sizeof(Fap20v_logical_external_dram_field[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "queueing group of registers/fields.\r\n",
    "Examples:\r\n"
    "  fap20v general logical_write 0x1 queue_controller top_que_blk_1 0x333\r\n"
    "Results in writing the value 0x333 to the top_que_blk_1 field of device 0x1.\r\n",
    "",
    {BIT(4) | BIT(5) | BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_FIELD_ID,
    "sch",
    (PARAM_VAL_RULES *)&Fap20v_logical_sch_field[0],
    (sizeof(Fap20v_logical_sch_field) / sizeof(Fap20v_logical_sch_field[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "scheduler group of registers/fields.\r\n",
    "Examples:\r\n"
    "  fap20v general logical_write 0x1 sch port_cfg 0x3\r\n"
    "Results in writing the value 0x3 to the port_cfg field of device 0x1.\r\n",
    "",
    {BIT(4) | BIT(5) | BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* miscellaneous interface { */
  {
    PARAM_FAP20V_MISC_API_ID,
    "misc",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver miscellaneous services.",
    "Examples:\r\n"
    "  fap20v traffic send_sr_data_cell 0 initial_output_link 3 fe2_link 16 destination_entity_type fop body 0x88888877 \r\n"
    "Results in sending a source routed data cell through device 0 link 3, and the data: 0x88888877 to a fop.\r\n",
    "",
    {0,
     0,
     0,
     BIT(2)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  /* END miscellaneous interface } */

  /* fap20v_api_cell.h interface { */
  {
    PARAM_FAP20V_CELL_API_ID,
    "traffic",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver traffic (cell/packet) services.",
    "Examples:\r\n"
    "  fap20v traffic send_sr_data_cell 0 initial_output_link 3 fe2_link 16 destination_entity_type fop body 0x88888877 \r\n"
    "Results in sending a source routed data cell through device 0 link 3, and the data: 0x88888877 to a fop.\r\n",
    "",
    {BIT(7) | BIT(8),
     BIT(22) | BIT(23),
     BIT(30)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SEND_CELL_ID,
    "send_sr_data_cell",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sends a source routed data cell.\r\n",
    "Examples:\r\n"
    "  fap20v traffic send_sr_data_cell 0 initial_output_link 3 fe2_link 16 destination_entity_type fop body 0x88888877 \r\n"
    "Results in sending a source routed data cell through device 0 link 3, and the data: 0x88888877 to a fop.\r\n",
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
    PARAM_FAP20V_SEND_CELL_INIT_OUT_LINK_ID,
    "initial_output_link",
    (PARAM_VAL_RULES *)&FAP20V_initial_output_link_num_vals[0],
    (sizeof(FAP20V_initial_output_link_num_vals) / sizeof(FAP20V_initial_output_link_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR, (PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "the link to choose when leave this fap.\r\n",
    "Examples:\r\n"
      "  fap20v traffic send_sr_data_cell 0 initial_output_link 3 fe2_link 16 destination_entity_type fop body 0x88888877 \r\n"
      "Results in sending a source routed data cell through device 0 link 3, and the data: 0x88888877 to a fop.\r\n",
    "",
    {BIT(7)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SEND_CELL_FE1_LINK_ID,
    "fe1_link",
    (PARAM_VAL_RULES *)&FAP20V_fe1_link_num_vals[0],
    (sizeof(FAP20V_fe1_link_num_vals) / sizeof(FAP20V_fe1_link_num_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR, (PARAM_VAL *)&Fap20v_fe1_link_default,
    0,0,0,0,0,0,0,
    "the link to choose when passing through fe1.\r\n",
    "Examples:\r\n"
      "  fap20v traffic send_sr_data_cell 0 initial_output_link 3 fe2_link 16 destination_entity_type fop body 0x88888877 \r\n"
      "Results in sending a source routed data cell through device 0 link 3, and the data: 0x88888877 to a fop.\r\n",
    "",
    {BIT(7)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SEND_CELL_FE2_LINK_ID,
    "fe2_link",
    (PARAM_VAL_RULES *)&FAP20V_fe2_link_num_vals[0],
    (sizeof(FAP20V_fe2_link_num_vals) / sizeof(FAP20V_fe2_link_num_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR, (PARAM_VAL *)&Fap20v_fe2_link_default,
    0,0,0,0,0,0,0,
    "the link to choose when passing through fe2.\r\n",
    "Examples:\r\n"
      "  fap20v traffic send_sr_data_cell 0 initial_output_link 3 fe2_link 16 destination_entity_type fop body 0x88888877 \r\n"
      "Results in sending a source routed data cell through device 0 link 3, and the data: 0x88888877 to a fop.\r\n",
    "",
    {BIT(7)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SEND_CELL_FE3_LINK_ID,
    "fe3_link",
    (PARAM_VAL_RULES *)&Fap20v_fe3_link_num_vals[0],
    (sizeof(Fap20v_fe3_link_num_vals) / sizeof(Fap20v_fe3_link_num_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR, (PARAM_VAL *)&Fap20v_fe3_link_default,
    0,0,0,0,0,0,0,
    "the link to choose when passing through fe3.\r\n",
    "Examples:\r\n"
      "  fap20v traffic send_sr_data_cell 0 initial_output_link 3 fe2_link 16 destination_entity_type fop body 0x88888877 \r\n"
      "Results in sending a source routed data cell through device 0 link 3, and the data: 0x88888877 to a fop.\r\n",
    "",
    {BIT(7)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SEND_CELL_DEST_ENTITY_TYPE_ID,
    "destination_entity_type",
    (PARAM_VAL_RULES *)&Fap20v_entity_type[0],
    (sizeof(Fap20v_entity_type) / sizeof(Fap20v_entity_type[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR, (PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "the entity this cell is destined to.\r\n",
    "Examples:\r\n"
      "  fap20v traffic send_sr_data_cell 0 initial_output_link 3 fe2_link 16 destination_entity_type fop body 0x88888877 \r\n"
      "Results in sending a source routed data cell through device 0 link 3, and the data: 0x88888877 to a fop.\r\n",
    "",
    {BIT(7)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SEND_CELL_SRC_CHIP_ID,
    "source_chip_id",
    (PARAM_VAL_RULES *)&Fap20v_source_chip_id_num_val[0],
    (sizeof(Fap20v_source_chip_id_num_val) / sizeof(Fap20v_source_chip_id_num_val[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR, (PARAM_VAL *)&Fap20v_source_chip_id_default,
    0,0,0,0,0,0,0,
    "the entity this cell is originated in.\r\n",
    "Examples:\r\n"
      "  fap20v traffic send_sr_data_cell 0 initial_output_link 3 fe2_link 16 destination_entity_type fop body 0x88888877 \r\n"
      "Results in sending a source routed data cell through device 0 link 3, and the data: 0x88888877 to a fop.\r\n",
    "",
    {BIT(7)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SEND_CELL_BODY_ID,
    "body",
    (PARAM_VAL_RULES *)&Fap20v_cell_body_num_vals[0],
    (sizeof(Fap20v_cell_body_num_vals) / sizeof(Fap20v_cell_body_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR, (PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "the body of the cell.\r\n",
    "Examples:\r\n"
      "  fap20v traffic send_sr_data_cell 0 initial_output_link 3 fe2_link 16 destination_entity_type fop body 0x88888877 \r\n"
      "Results in sending a source routed data cell through device 0 link 3, and the data: 0x88888877 to a fop.\r\n",
    "",
    {BIT(7)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_RECV_CELL_ID,
    "recv_sr_data_cell",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "recieves a source routed data cell.\r\n",
    "Examples:\r\n"
    "  fap20v traffic recv_sr_data_cell 0x1 \r\n"
    "Results in reading a source routed data cell from device 0x1 recieve cell buffer.\r\n",
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
    PARAM_FAP20V_DEFRRED_RECV_CELL_ID,
    "recv_sr_data_cell",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "deferred receive (interrupt or polling) of a source routed data cell.\r\n",
    "Examples:\r\n"
    "  fap20v deferred recv_sr_data_cell 0x1 interrupt_mode \r\n"
    "Results in deferred reading a source routed data cell from device 0x1 \n\r"
    " recieve cell buffer, which is triggered by an interrupt.\r\n",
    "",
    {BIT(9) | BIT(10)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DEFERRED_FAP20V_RECV_CELL_POLL_MODE_ID,
    "polling_mode",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Deferred call is triggered by simple polling of the device.\r\n",
    "Examples:\r\n"
    "  fap20v deferred recv_sr_data_cell 0x1 polling_mode polling_rate 50 \r\n"
    "Results in deferred reading a source routed data cell from device 0x1 'low'"
    " recieve cell buffer, which is triggered by a polling action every 50 ticks.\r\n",
    "",
    {BIT(9)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_DEFERRED_FAP20V_INT_MODE_ID,
    "interrupt_mode",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Deferred call is triggered by an interrupt of the device.\r\n",
    "Examples:\r\n"
    "  fap20v deferred recv_sr_data_cell 0x1 links range_0-31 interrupt_mode\r\n"
    "Results in deferred reading a source routed data cell from device 0x1 'low'"
    " recieve cell buffer, which is triggered by an interrupt.\r\n",
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
    PARAM_FAP20V_SEND_FLOW_STATUS_CTRL_CELL_ID,
    "send_flow_status_control_cell",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sends a flow status control cell.\r\n",
    "Examples:\r\n"
    "  fap20v traffic send_flow_status_control_cell 0 target_fap_id 2 flow_id 11444 queue_id 2345 flow_status on\r\n"
    "Results in sending a 'ON' flow status control cell through device 0 to scheduler flow 11444 on fap 2.\r\n"
    " (assuming that Flow 11444 is mapped to ingress queue 2345 on device 0)\r\n",
    "",
    {0,
     0,
     BIT(30)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TARGET_FAP_ID_ID,
    "target_fap_id",
    (PARAM_VAL_RULES *)&Fap20v_fap_id[0],
    (sizeof(Fap20v_fap_id) / sizeof(Fap20v_fap_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Target FAP identifier.",
    "Examples:\r\n"
    "  fap20v traffic send_flow_status_control_cell 0 target_fap_id 2 flow_id 11444 queue_id 2345 flow_status on\r\n"
    "Results in sending a 'ON' flow status control cell through device 0 to scheduler flow 11444 on fap 2.\r\n"
    " (assuming that Flow 11444 is mapped to ingress queue 2345 on device 0)\r\n",
    "",
    {0,
     0,
     BIT(30)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_FLOW_ID_ID,
    "flow_id",
    (PARAM_VAL_RULES *)&Fap20v_flow_id[0],
    (sizeof(Fap20v_flow_id) / sizeof(Fap20v_flow_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Flow, in the scheduler.",
    "Examples:\r\n"
    "  fap20v traffic send_flow_status_control_cell 0 target_fap_id 2 flow_id 11444 queue_id 2345 flow_status on\r\n"
    "Results in sending a 'ON' flow status control cell through device 0 to scheduler flow 11444 on fap 2.\r\n"
    " (assuming that Flow 11444 is mapped to ingress queue 2345 on device 0)\r\n",
    "",
    {0,
     0,
     BIT(30)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_QUEUE_ID_ID,
    "queue_id",
    (PARAM_VAL_RULES *)&Fap20v_queue_id[0],
    (sizeof(Fap20v_queue_id) / sizeof(Fap20v_queue_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Queue identifier.\r\n",
    "Examples:\r\n"
    "  fap20v traffic send_flow_status_control_cell 0 target_fap_id 2 flow_id 11444 queue_id 2345 flow_status on\r\n"
    "Results in sending a 'ON' flow status control cell through device 0 to scheduler flow 11444 on fap 2.\r\n"
    " (assuming that Flow 11444 is mapped to ingress queue 2345 on device 0)\r\n",
    "",
    {0,
     0,
     BIT(30)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_FLOW_STATUS_ID,
    "flow_status",
    (PARAM_VAL_RULES *)&Fap20v_flow_status_id[0],
    (sizeof(Fap20v_flow_status_id) / sizeof(Fap20v_flow_status_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Flow status.\r\n",
    "Examples:\r\n"
    "  fap20v traffic send_flow_status_control_cell 0 target_fap_id 2 flow_id 11444 queue_id 2345 flow_status on\r\n"
    "Results in sending a 'ON' flow status control cell through device 0 to scheduler flow 11444 on fap 2.\r\n"
    " (assuming that Flow 11444 is mapped to ingress queue 2345 on device 0)\r\n",
    "",
    {0,
     0,
     BIT(30)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  /* END fap20v_api_cell.h interface } */
  /* fap20v_api_links.h interface { */
  {
    PARAM_FAP20V_LINKS_API_ID,
    "links",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver links services.",
    "Examples:\r\n"
    "  fap20v cell_services send_sr_data_cell 0x1 initial_output_link 40 fe2_link 16 fe3_link 22 destination_entity_type fop \r\n"
    "Results in sending a source routed data cell through device 0 link 3, and the data: 0x88888877 to a fop.\r\n",
    "",
    {BIT(11) | BIT(12) | BIT(14) | BIT(27),
     0,
     BIT(29) },
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_TEST_SSR_RESET_ID,
    "ssr_reset",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "performs SSR save-restore (reset) cycle on fap20v platform. ",
    "Examples:\r\n"
    "  fap20v tests ssr_reset \r\n"
    "Results in perfoming SSR reset to the appropriate card\r\n",
    "",
     {
      0,
      BIT(0),
      0,
      0
     },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_TEST_SSR_XOR_CHECK_ID,
    "ssr_xor_check",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "tests fap20v SSR mechanism by perfoming SSR reset twice, \r\n"
    " and confirming that SSR data is identical after each reset (XOR check) ",
    "Examples:\r\n"
    "  fap20v tests ssr_xor_check \r\n"
    "Results in perfoming two SSR resets and conforming (mem XOR) SSR data identity after each reset\r\n",
    "",
     {
      0,
      BIT(0),
      0,
      0
     },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_TEST_SSR_TEST_DATA_ID,
    "ssr_test_data",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The test gets sample global data from the various modules \r\n"
    " and confirms that it is preserved during SSR reset ",
    "Examples:\r\n"
    "  fap20v tests ssr_test_data \r\n"
    "Results in getting sample data, performing SSR reset, getting the data again and comparing the two results \r\n",
    "",
     {
      0,
      BIT(0),
      0,
      0
     },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_RESET_SERDES_PARAMS_ID,
    "reset_serdes",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Resets specified SerDes. In reset is 1 - out of reset is 0.\r\n",
    "Examples:\r\n"
    "  fap20v links reset_serdes 0x0 serdes_number 3 indication 1\r\n"
    "Set serdes 2 and 3 to be in reset mode.\r\n"
    "Note: The links are coupled, i.e., links 0-1, 2-3, , 22-23 are\n\r"
    "powered up-down together.",
    "",
    {0,
     0,
     BIT(29)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_RESET_SERDES_PLL_PARAMS_ID,
    "reset_serdes_pll",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Resets specified SerDes PLL. In reset is 1 - out of reset is 0.\r\n",
    "Examples:\r\n"
    "  fap20v links reset_serdes_pll 0x0 serdes_number 3 indication 1\r\n"
    "Set serdes 2 and 3 to be in reset mode.\r\n"
    "Note: The links are coupled, i.e., links 0-1, 2-3, , 22-23 are\n\r"
    "powered up-down together.",
    "",
    {0,
     0,
     BIT(29)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_INDICATION_ID,
    "indication",
    (PARAM_VAL_RULES *)&Fap20v_zero_one_num_vals[0],
    (sizeof(Fap20v_zero_one_num_vals) / sizeof(Fap20v_zero_one_num_vals[0])) - 1,
    HAS_DEFAULT,(PARAM_VAL *)&Fap20v_zero_default,
    0,0,0,0,0,0,0,
    "Indication: 0/1 value.\r\n",
    "Examples:\r\n"
    "  fap20v links reset_serdes 0x0 serdes_number 3 indication 1\r\n"
    "Set serdes 2 and 3 to be in reset mode.\r\n",
    "",
    {0,
     0,
     BIT(29)},
   (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_GET_SERDES_PARAMS_ID,
    "get_serdes_params",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets the status report of all the serdeses.\r\n",
    "Examples:\r\n"
    "  fap20v links get_serdes_params 0x1 \r\n"
    "Results in printing the ststus of all serdeses.\r\n",
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
    PARAM_FAP20V_SET_SERDES_PD_PARAMS_ID,
    "set_serdes_power_down",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the serdes power down/up.\r\n",
    "Examples:\r\n"
    "  fap20v links set_serdes_power_down 0x1 serdes_number 12\r\n"
    "Results in setting link#12 to power down.\r\n",
    "",
    {BIT(27)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SET_SERDES_PARAMS_ID,
    "set_serdes_params",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the params of a specific serdes (may reset the link).\r\n",
    "Examples:\r\n"
    "  fap20v links set_serdes_params 0x1 serdes_number 12 amplitude 3 common_mode 2\r\n"
    "   main_control 0 rx_tx_power_down 0 tbg_power_down 0 pre_emphasis 3\r\n"
    "Results in setting link#12 to the above values.\r\n",
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
    PARAM_FAP20V_SET_SERDES_PARAMS_SERDES_NUMBER_ID,
    "serdes_number",
    (PARAM_VAL_RULES *)&Fap20v_serdes_number_num_vals[0],
    (sizeof(Fap20v_serdes_number_num_vals) / sizeof(Fap20v_serdes_number_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the params of a specific serdes (may reset the link).\r\n",
    "Examples:\r\n"
    "  fap20v links set_serdes_params 0x1 serdes_number 12 amplitude 3 common_mode 2\r\n"
    "   main_control 0 rx_tx_power_down 0 tbg_power_down 0 pre_emphasis 3\r\n"
    "Results in setting link#12 to the above values.\r\n",
    "",
    {BIT(12) | BIT(23) | BIT(27),
     0,
     BIT(29)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SERDES_POWER_UP_ID,
    "power_up",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Power-up indication.\r\n",
    "Examples:\r\n"
    "  fap20v links set_serdes_power_down 0x1 serdes_number 12 power_up\r\n"
    "Results in setting link#12 to power up.\r\n",
    "",
    {BIT(27)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SET_SERDES_DRIVER_TX_ID,
    "driver_tx",
    (PARAM_VAL_RULES *)&Fap20v_serdes_driver_tx_num_vals[0],
    (sizeof(Fap20v_serdes_driver_tx_num_vals) / sizeof(Fap20v_serdes_driver_tx_num_vals[0])) - 1,
    HAS_DEFAULT,(PARAM_VAL *)&Fap20v_serdes_driver_tx_default,
    0,0,0,0,0,0,0,
    "Driver TX value of specific SerDes.\r\n",
    "Examples:\r\n"
    "  fap20v links set_serdes_params 0x1 serdes_number 12 amplitude 3 common_mode 2\r\n"
    "   main_control 0 rx_tx_power_down 0 tbg_power_down 0 pre_emphasis 3\r\n"
    "Results in setting link#12 to the above values.\r\n",
    "",
    {BIT(12)},
   (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SET_SERDES_DRIVER_EQ_MODE_ID,
    "driver_eq",
    (PARAM_VAL_RULES *)&Fap20v_serdes_driver_eq_num_vals[0],
    (sizeof(Fap20v_serdes_driver_eq_num_vals) / sizeof(Fap20v_serdes_driver_eq_num_vals[0])) - 1,
    HAS_DEFAULT,(PARAM_VAL *)&Fap20v_serdes_driver_eq_default,
    0,0,0,0,0,0,0,
    "Driver EQ value of specific SerDes.\r\n",
    "Examples:\r\n"
    "  fap20v links set_serdes_params 0x1 serdes_number 12 amplitude 3 common_mode 2\r\n"
    "   main_control 0 rx_tx_power_down 0 tbg_power_down 0 pre_emphasis 3\r\n"
    "Results in setting link#12 to the above values.\r\n",
    "",
    {BIT(12)},
   (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SET_SERDES_HI_DRV_ID,
    "hi_drv",
    (PARAM_VAL_RULES *)&Fap20v_zero_one_num_vals[0],
    (sizeof(Fap20v_zero_one_num_vals) / sizeof(Fap20v_zero_one_num_vals[0])) - 1,
    HAS_DEFAULT,(PARAM_VAL *)&Fap20v_zero_default,
    0,0,0,0,0,0,0,
    "Hi-Drv on link.\r\n",
    "Examples:\r\n"
    "  fap20v links set_serdes_params 0x1 serdes_number 12 amplitude 3 common_mode 2\r\n"
    "   main_control 0 rx_tx_power_down 0 tbg_power_down 0 pre_emphasis 3\r\n"
    "Results in setting link#12 to the above values.\r\n",
    "",
    {BIT(12)},
   (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SET_SERDES_LOW_DRV_ID,
    "low_drv",
    (PARAM_VAL_RULES *)&Fap20v_zero_one_num_vals[0],
    (sizeof(Fap20v_zero_one_num_vals) / sizeof(Fap20v_zero_one_num_vals[0])) - 1,
    HAS_DEFAULT,(PARAM_VAL *)&Fap20v_zero_default,
    0,0,0,0,0,0,0,
    "Low-Drv on link.\r\n",
    "Examples:\r\n"
    "  fap20v links set_serdes_params 0x1 serdes_number 12 amplitude 3 common_mode 2\r\n"
    "   main_control 0 rx_tx_power_down 0 tbg_power_down 0 pre_emphasis 3\r\n"
    "Results in setting link#12 to the above values.\r\n",
    "",
    {BIT(12)},
   (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SET_SERDES_LOCAL_LOOPBACK_ID,
    "local_loopback",
    (PARAM_VAL_RULES *)&Fap20v_zero_one_num_vals[0],
    (sizeof(Fap20v_zero_one_num_vals) / sizeof(Fap20v_zero_one_num_vals[0])) - 1,
    HAS_DEFAULT,(PARAM_VAL *)&Fap20v_zero_default,
    0,0,0,0,0,0,0,
    "Low-Drv on link.\r\n",
    "Examples:\r\n"
    "  fap20v links set_serdes_params 0x1 serdes_number 12 amplitude 3 common_mode 2\r\n"
    "   main_control 0 rx_tx_power_down 0 tbg_power_down 0 pre_emphasis 3\r\n"
    "Results in setting link#12 to the above values.\r\n",
    "",
    {BIT(12)},
   (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_GET_CONNECTIVITY_MAP_ID,
    "get_connectivity_map",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "present connectivity map the the chip sees\r\n",
    "Examples:\r\n"
    "  fap20v links get_connectivity_map 0x1 \r\n"
    "Results in printing the connectivity map this chip sees.\r\n",
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
    PARAM_FAP20V_DEFERRED_GET_CONNECTIVITY_MAP_ID,
    "get_connectivity_map",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets the status report of all the serdeses.\r\n",
    "Examples:\r\n"
    "  fap20v deferred get_connectivity_map 0x1 \r\n"
    "Results in printing the connectivity map this chip sees, and every time it changes.\r\n",
    "",
    {BIT(9) | BIT(10)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_GET_LINKS_STATUS_ID,
    "get_links_status",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets status report of all links.\r\n",
    "Examples:\r\n"
    "  fap20v links get_links_status 0x1 list_of_links 0x0 0x1\r\n"
    "Results in printing the ststus of link #0.\r\n",
    "",
    {BIT(14)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_LIST_OF_LINKS_ID,
    "list_of_links",
    (PARAM_VAL_RULES *)&Fap20v_list_of_links_num_vals[0],
    (sizeof(Fap20v_list_of_links_num_vals) / sizeof(Fap20v_list_of_links_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets status report of all links.\r\n",
    "Examples:\r\n"
    "  fap20v links get_links_status 0x1 list_of_links 0x0 0x1\r\n"
    "Results in printing the ststus of link #1.\r\n",
    "",
    {BIT(14) | BIT(15) | BIT(19)},
    8,/* just so it would be after polling rate (==6) */
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_DEFERRED_GET_LINKS_STATUS_ID,
    "get_links_status",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets status report of all links.\r\n",
    "Examples:\r\n"
    "  fap20v deferred get_links_status 0x1 list_of_links 0x0 0x1 interrupt_mode \r\n"
    "Results in printing the ststus of link #0 when changed.\r\n",
    "",
    {BIT(9) | BIT(10)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* END fap20v_api_links.h interface } */
  /* fap20v_api_routing.h interface { */
  {
    PARAM_FAP20V_ROUTING_API_ID,
    "routing",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver routing services.",
    "Examples:\r\n"
    "  fap20v routing get_unicast_table 0x1 starting_row 125 nof_rows 6\r\n"
    "Results in printing the distribution unicast table of\n\r"
    "  device #1 rows (target FAPs) 125-130.\r\n",
    "",
    {BIT(16) | BIT(17) | BIT (19) | BIT (20)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_GET_UNICAST_TABLE_ID,
    "get_unicast_table",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets (max of 100 entires of) the unicast distribution table of the device.\r\n",
    "Examples:\r\n"
    "  fap20v routing get_unicast_table 0 starting_row 0 nof_rows 2\r\n"
    "Results in printing the distribution unicast table of device #0 rows (target FAPs) 0-1.\r\n",
    "",
    {BIT(17)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_DEFERRED_GET_UNICAST_TABLE_ID,
    "get_unicast_table",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "peridically gets (max of 100 entires of) the unicast distribution table of the device.\r\n",
    "Examples:\r\n"
    "  fap20v deferred get_unicast_table 0x1 starting_row 125 ... \r\n"
    "Results in peridically printing the distribution table of device #1 starting at row 125.\r\n",
    "",
    {BIT(18)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_STARTING_ROW_ID,
    "starting_row",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets the unicast distribution table of the device.\r\n",
    "Examples:\r\n"
    "  fap20v deferred get_unicast_table 0x1 ... \r\n"
    "Results in peridically printing the distribution table of device #1.\r\n",
    "",
    {BIT(17) | BIT(18) | BIT(19)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_NOF_ROWS_ID,
    "nof_rows",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets the unicast distribution table of the device.\r\n",
    "Examples:\r\n"
    "  fap20v deferred get_unicast_table 0x1 ... \r\n"
    "Results in peridically printing the distribution table of device #1.\r\n",
    "",
    {BIT(17) | BIT(18) | BIT(19)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* END fap20v_api_routing.h interface } */
  /* fap20v_api_diagnostics.h interface { */
  {
    PARAM_FAP20V_DIAGNOSTICS_API_ID,
    "diagnostics",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver diagnostics services.",
    "Examples:\r\n"
    "  fap20v diagnostics get_loopback_type 0x1\r\n"
    "Results in printing loopback mode device #1 is in.\r\n",
    "",
    {BIT(21) | BIT(22) | BIT(23), 0, BIT(9), 0, 0, 0, 0, 0, BIT(1)|BIT(2)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_QDP_INIT_SEQ_ID,
    "qdp_init_seq",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set QDP init sequence.\r\n",
    "Examples:\r\n"
    "  fap20v diagnostics qdp_init_seq 0x0 wait_time_in_nano_sec 1000000\r\n"
    "Sets QDP init sequnce at device #0, wait for 1 mili-sec (1000000 nano).\r\n",
    "",
    {0,
     0,
     BIT(9),
     0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_DO_QDP_WORK_AROUND_ID,
    "do_qdp_work_around",
    (PARAM_VAL_RULES *)&Fap20v_on_off_vals[0],
    (sizeof(Fap20v_on_off_vals) / sizeof(Fap20v_on_off_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Time in nano to wait before setting the QDO-init trigger.\r\n",
    "Examples:\r\n"
    "  fap20v diagnostics qdp_init_seq 0x0 wait_time_in_nano_sec 1000000\r\n"
    "Sets QDP init sequnce at device #0, wait for 1 mili-sec (1000000 nano).\r\n",
    "",
    {0,
     0,
     BIT(9),
     0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TIME_IN_NANO_SEC_ID,
    "wait_time_in_nano_sec",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Time in nano to wait before setting the QDO-init trigger.\r\n",
    "Examples:\r\n"
    "  fap20v diagnostics qdp_init_seq 0x0 wait_time_in_nano_sec 1000000\r\n"
    "Sets QDP init sequnce at device #0, wait for 1 mili-sec (1000000 nano).\r\n",
    "",
    {0,
     0,
     BIT(9),
     0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_LOOPBACK_TYPE_ID,
    "local_loop",
    (PARAM_VAL_RULES *)&Fap20v_local_loopback_on_vals[0],
    (sizeof(Fap20v_local_loopback_on_vals) / sizeof(Fap20v_local_loopback_on_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the loopback mode the device is in.\r\n",
    "Examples:\r\n"
    "  fap20v diagnostics set_loopback_type 0x1 local_loop on\r\n"
    "Results in setting the loopback mode of device #1 to local loopback.\r\n",
    "",
    {BIT(22)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_GET_PRBS_MODE_ID,
    "get_prbs_mode",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets the prbs mode the device is in.\r\n",
    "Examples:\r\n"
    "  fap20v diagnostics get_prbs_mode 0x1\r\n"
    "Results in printing the prbs mode of device #1.\r\n",
    "",
    {BIT(21)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SET_PRBS_MODE_ID,
    "set_prbs_mode",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the prbs mode of the device's MAC module.\n\r",
    "Examples:\r\n"
    "  fap20v diagnostics set_prbs_mode 0 serdes_number 0 checker_active on generator_active on loopback on\r\n"
    "Results in setting the prbs params of the MAC.\r\n",
    "",
    {BIT(23)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_PRBS_CHECKER_LINK_ID,
    "checker_active",
    (PARAM_VAL_RULES *)&Fap20v_on_off_vals[0],
    (sizeof(Fap20v_on_off_vals) / sizeof(Fap20v_on_off_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)NULL,
    0,0,0,0,0,0,0,
    "Make the checker (rx) active.\r\n",
    "Examples:\r\n"
    "  fap20v diagnostics set_prbs_mode 0 serdes_number 0 checker_active on generator_active on loopback on\r\n"
    "Results in setting the prbs params of MAC.\r\n",
    "",
    {BIT(23)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_PRBS_GENERATOR_LINK_ID,
    "generator_active",
    (PARAM_VAL_RULES *)&Fap20v_on_off_vals[0],
    (sizeof(Fap20v_on_off_vals) / sizeof(Fap20v_on_off_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)NULL,
    0,0,0,0,0,0,0,
    "Make the generator (tx) active.\r\n",
    "Examples:\r\n"
    "  fap20v diagnostics set_prbs_mode 0 serdes_number 0 checker_active on generator_active on loopback on\r\n"
    "Results in setting the prbs params of MAC.\r\n",
    "",
    {BIT(23)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_PRBS_LOOPBACK_ID,
    "loopback",
    (PARAM_VAL_RULES *)&Fap20v_on_off_vals[0],
    (sizeof(Fap20v_on_off_vals) / sizeof(Fap20v_on_off_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)NULL,
    0,0,0,0,0,0,0,
    "Make loopback in the MAC From FAP core to the MACs and back to the core.\r\n",
    "Examples:\r\n"
    "  fap20v diagnostics set_prbs_mode 0 serdes_number 0 checker_active on generator_active on loopback on\r\n"
    "Results in setting the prbs params of MAC.\r\n",
    "",
    {BIT(23)},
    (unsigned int)LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_START_PRBS_TEST_ID,
    "start_prbs_test",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "starts background collection of ber counters.\r\n",
    "Examples:\r\n"
    "  fap20v diagnostics start_prbs_test 0x1\r\n"
    "Results in the background collection of ber counters of device 1.\r\n",
    "",
    {BIT(21)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_STOP_PRBS_TEST_ID,
    "stop_prbs_test",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "stops background collection of ber counters.\r\n",
    "Examples:\r\n"
    "  fap20v diagnostics stop_prbs_test 0x1\r\n"
    "Results in stopping the background collection of ber counters of device 1.\r\n",
    "",
    {BIT(21)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_GET_BER_COUNTERS_ID,
    "get_ber_counters",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets counter report of prbs units.\r\n",
    "Examples:\r\n"
    "  fap20v diagnostics get_ber_counters 0x1\r\n"
    "Results in printing the counter report of the 4 prbs units of device 1.\r\n",
    "",
    {BIT(21)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_DIAGNOSTICS_DUMP_REGS_ID,
    "dump_regs",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Print all of the registers of the device.\r\n",
    "Examples:\r\n"
    "  fap20v diagnostics dump_regs 0x0\r\n"
    "Results in printing all of the registers of the device device 0.\r\n",
    "",
    {BIT(21)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_PRINT_ALL_INFO_ID,
    "print_all",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Print all the configuration information from the device.\r\n",
    "Examples:\r\n"
    "  fap20v diagnostics print_all 0x0\r\n"
    "Results in printing all the logical configuration of device 0.\r\n",
    "",
    {0,0,0,0,0,0,0,0,BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_PRINT_AND_CLEAR_ALL_COUNTERS_AND_INTERRUPTS_ID,
    "clear_all_counters_and_interrupts",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    ""
    ""
    "",
    "",
    {0,0,0,0,0,0,0,0,BIT(2)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_CLEAR_ALL_COUNTERS_AND_INTERRUPTS_ID,
    "print_all_counters_and_interrupts",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    ""
    ""
    "",
    "",
    {0,0,0,0,0,0,0,0,BIT(2)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_DIAGNOSTICS_DUMP_REGS_IND_ID,
    "dont_print_indirects",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "When appears, dont print indirects.\r\n",
    "Examples:\r\n"
    "  fap20v diagnostics dump_regs 0x0 dont_print_indirects\r\n"
    "Results in printing all of the registers of the device device 0.\r\n"
    " without the indirect tables.",
    "",
    {BIT(21)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_DEFERRED_GET_BER_COUNTERS_ID,
    "get_ber_counters",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets counter report of prbs units.\r\n",
    "Examples:\r\n"
    "  fap20v deferred get_ber_counters 0x1 polling_rate 60 \r\n"
    "Results in printing the counter report of the 4 prbs units of device 1 every second\r\n",
    "",
    {BIT(24)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* END fap20v_api_diagnostics.h interface } */
  /* fap20v_api_statistics.h interface { */
  {
    PARAM_FAP20V_STATISTICS_API_ID,
    "statistics",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver statistics services.",
    "Examples:\r\n"
    "  fap20v statistics_services get_mac_counter_type 0x1\r\n"
    "Results in printing mac rx & tx counter type device #1 is in.\r\n",
    "",
    {BIT(25),
     0,
     BIT(26),
     BIT(7) | BIT(8) | BIT(9)
    },
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SET_PRG_COUNTERS_ID,
    "set_programmable_counters",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set programable counters\r\n"
    "   The counters result will be displayed by the CLI \n\r"
    "   \'fap20v statistices get_all_counters 0\'\r\n",
    "",
    "",
    {0,0,0,BIT(7) | BIT(8) | BIT(9)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SET_PRG_PORT_ID,
    "port",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set programable Port ID.\r\n"
    "   The counters result will be displayed by the CLI \n\r"
    "   \'fap20v statistices get_all_counters 0\'\r\n",
    "",
    "",
    {0,0,0,BIT(7)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SET_Q_ID,
    "queue",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set programable Queue ID.\r\n"
    "   The counters result will be displayed by the CLI \n\r"
    "   \'fap20v statistices get_all_counters 0\'\r\n",
    "",
    "",
    {0,0,0,BIT(8)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SET_Q_BY_CLS_ID,
    "class_of_traffic",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set all the queue of class of service. Range 0-7\r\n"
    "   The counters result will be displayed by the CLI \n\r"
    "   \'fap20v statistices get_all_counters 0\'\r\n",
    "",
    "",
    {0,0,0,BIT(9)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_GET_ALL_INTERRUPTS_ID,
    "print_all_interrups",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Print all the interrupts in the system\r\n"
    "   Most of importent errors in the system result in interrupts\r\n",
    "Examples:\r\n"
    "  fap20v statistics print_all_interrups 0\r\n"
    "Results in printing all the interrupts in the system for device 0\r\n",
    "",
    {BIT(25)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_ALL_COUNTERS_ID,
    "get_all_counters",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gets set of counters that is defined in the statistics section.\n\r"
    "  Refer to other command to see specific counter meanning.\r\n",
    "Examples:\r\n"
    "  fap20v statistics get_all_counters 0x0\r\n"
    "Results in printing all statistics counters of device 0.\r\n",
    "",
    {BIT(25)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_LONG_FORMAT_ID,
    "long_display_format",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Makes the print in long format.\r\n",
    "Examples:\r\n"
    "  fap20v statistics get_all_counters 0x0 long_display_format\r\n"
    "Results in printing all statistics counters of device 0,\r\n"
    "  and print in long format.\r\n",
    "",
    {BIT(25),
     0,
     BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_ONE_COUNTER_ID,
    "get_cnt",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get one counter result.\r\n",
    "Examples:\r\n"
    "  fap20v statistics get_cnt 0x0 cnt_type 1\r\n"
    "Results in printing the device 0 counter 1.\r\n",
    "",
    {0,
     0,
     BIT(26)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_COUNTER_TYPE_ID,
    "type",
    (PARAM_VAL_RULES *)&Fap20v_counter_type[0],
    (sizeof(Fap20v_counter_type) / sizeof(Fap20v_counter_type[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Type of counter to get.\r\n",
    "Examples:\r\n"
    "  fap20v statistics get_cnt 0x0 cnt_type tdm_a_pkt\r\n"
    "Results in printing the device 0 counter tdm_a_pkt.\r\n",
    "",
    {BIT(24),
     0,
     BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_DEFERRED_ONE_COUNTER_ID,
    "get_cnt",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets counter report from the FIP about the number of packets\n\r"
    "  were tail-discarded from memory due to the 'drop from front' scheme.\r\n",
    "Examples:\r\n"
    "  fap20v statistics get_tail_discard_pkt_cnt 0x1\r\n"
    "Results in printing the 'Tail-Discarded Packet Counter' of device 1.\r\n",
    "",
    {BIT(24)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* } END fap20v_api_statistics.h interface */
  /*
   * Test API {
   */
  {
    PARAM_FAP20V_TEST_API_ID,
    "tests",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver tests services.\n\r"
    "  The system need to be configured in a specific configuration.\n\r"
    "  *NOTE*: All tests may change your FAP configuration - without\n\r"
    "          Restore it to your original values.",
    "Examples:\r\n"
    "  fap20v tests cells\r\n"
    "Results in testing fap20v driver cell module.\r\n"
    "In all the tests the system need to be configured as follows:\n\r"
    "  1. The fabric is considered working (FE/loopbacks)\n\r"
    "  2. The PP is considered UP and initialized with PP-Id is 0\n\r"
    "  3. The is considered after its reset sequence (all HW is functional)\n\r"
    "     FAP-Id is 0.\n\r"
    "  4. No traffic should be injected from out side ports.\n\r"
    "     Unless otherwise is stated.\n\r"
    "  5. One ethernet cable should connect from port 4 to 5.\n\r"
    "     Port 1 and 2 should not be connected.\n\r"
    "  6. PP MAC-table: All port on VLAN 1.\n\r"
    "         MAC address 000000AAAA05 - port 5\n\r"
    "         MAC address 000000AAAA04 - port 4\n\r",
    "",
    {BIT(31),
     BIT(30) | BIT(31) | BIT(0), /**/
     BIT(16) | BIT(17) | BIT(18), /*16: ,17, 18:app*/
     BIT(3) | BIT(4) | BIT(5)

    },
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_APP_ID,
    "app",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap20v test application.\r\n",
    "Examples:\r\n"
    "  fap20v tests app snake_1024\r\n"
    "Results in activating 'snake_1024' application.\r\n",
    "",
    {0,
     0,
     BIT(17) | BIT(18)
    },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_MEM_READ_DELTA_VALUE_STOP_ID,
    "read_addr_stop",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Stops the running test that checks that a certain field does not pass a threshold on several faps .\r\n",
    "Examples:\r\n"
    "  fap20v tests read_addr_stop\r\n"
    "Stops the currently running test.\n\r",
    "",
    {0,
     0,
     0,
     BIT(3)
    },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_MEM_READ_DELTA_VALUE_START_ID,
    "read_addr_start",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts a test that checks that a certain field does not pass a threshold on several faps .\r\n",
    "Examples:\r\n"
    "  fap20v read_addr_start offset 0xbe00 ms_bit 28 ls_bit 16 fap_ids 0 1 2 3 \r\n"
    "   delta_test requires_time_normaliztion higher_bound 50 lower_bound 10 \r\n"
    "   polling_rate_in_system_ticks 3 routine_printout_every 1_hour\r\n"
    "Results in registering a polling serivce on the 4 faps above. The polling rate will be\n\r"
    " 3 system ticks (48 ms). The test will check that the delta value between 2 conswqutive calls\r\n"
    " is not greater then 50 and not lower then 10. If both thresholds are not passed in one hour\r\n"
    " the display will show a routine print to the screen\r\n",
    "",
    {0,
     0,
     0,
     BIT(4) | BIT(5)
    },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_MEM_READ_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&Fap20v_offset_vals[0],
    (sizeof(Fap20v_offset_vals) / sizeof(Fap20v_offset_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts a test that checks that a certain field does not pass a threshold on several faps .\r\n",
    "Examples:\r\n"
    "  fap20v read_addr_start offset 0xbe00 ms_bit 28 ls_bit 16 fap_ids 0 1 2 3 \r\n"
    "   delta_test requires_time_normaliztion higher_bound 50 lower_bound 10 \r\n"
    "   polling_rate_in_system_ticks 3 routine_printout_every 1_hour\r\n"
    "Results in registering a polling serivce on the 4 faps above. The polling rate will be\n\r"
    " 3 system ticks (48 ms). The test will check that the delta value between 2 conswqutive calls\r\n"
    " is not greater then 50 and not lower then 10. If both thresholds are not passed in one hour\r\n"
    " the display will show a routine print to the screen\r\n",
    "",
    {0,
     0,
     0,
     BIT(4) | BIT(5)
    },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_MEM_READ_LS_BIT_ID,
    "ls_bit",
    (PARAM_VAL_RULES *)&Fap20v_bit_vals[0],
    (sizeof(Fap20v_bit_vals) / sizeof(Fap20v_bit_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts a test that checks that a certain field does not pass a threshold on several faps .\r\n",
    "Examples:\r\n"
    "  fap20v read_addr_start offset 0xbe00 ms_bit 28 ls_bit 16 fap_ids 0 1 2 3 \r\n"
    "   delta_test requires_time_normaliztion higher_bound 50 lower_bound 10 \r\n"
    "   polling_rate_in_system_ticks 3 routine_printout_every 1_hour\r\n"
    "Results in registering a polling serivce on the 4 faps above. The polling rate will be\n\r"
    " 3 system ticks (48 ms). The test will check that the delta value between 2 conswqutive calls\r\n"
    " is not greater then 50 and not lower then 10. If both thresholds are not passed in one hour\r\n"
    " the display will show a routine print to the screen\r\n",
    "",
    {0,
     0,
     0,
     BIT(4) | BIT(5)
    },
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_MEM_READ_MS_BIT_ID,
    "ms_bit",
    (PARAM_VAL_RULES *)&Fap20v_bit_vals[0],
    (sizeof(Fap20v_bit_vals) / sizeof(Fap20v_bit_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts a test that checks that a certain field does not pass a threshold on several faps .\r\n",
    "Examples:\r\n"
    "  fap20v read_addr_start offset 0xbe00 ms_bit 28 ls_bit 16 fap_ids 0 1 2 3 \r\n"
    "   delta_test requires_time_normaliztion higher_bound 50 lower_bound 10 \r\n"
    "   polling_rate_in_system_ticks 3 routine_printout_every 1_hour\r\n"
    "Results in registering a polling serivce on the 4 faps above. The polling rate will be\n\r"
    " 3 system ticks (48 ms). The test will check that the delta value between 2 conswqutive calls\r\n"
    " is not greater then 50 and not lower then 10. If both thresholds are not passed in one hour\r\n"
    " the display will show a routine print to the screen\r\n",
    "",
    {0,
     0,
     0,
     BIT(4) | BIT(5)
    },
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_MEM_READ_FAP_IDS_ID,
    "fap_ids",
    (PARAM_VAL_RULES *)&Fap20v_fap_ids_vals[0],
    (sizeof(Fap20v_fap_ids_vals) / sizeof(Fap20v_fap_ids_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts a test that checks that a certain field does not pass a threshold on several faps .\r\n",
    "Examples:\r\n"
    "  fap20v read_addr_start offset 0xbe00 ms_bit 28 ls_bit 16 fap_ids 0 1 2 3 \r\n"
    "   delta_test requires_time_normaliztion higher_bound 50 lower_bound 10 \r\n"
    "   polling_rate_in_system_ticks 3 routine_printout_every 1_hour\r\n"
    "Results in registering a polling serivce on the 4 faps above. The polling rate will be\n\r"
    " 3 system ticks (48 ms). The test will check that the delta value between 2 conswqutive calls\r\n"
    " is not greater then 50 and not lower then 10. If both thresholds are not passed in one hour\r\n"
    " the display will show a routine print to the screen\r\n",
    "",
    {0,
     0,
     0,
     BIT(4) | BIT(5)
    },
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_MEM_READ_DELTA_TEST_ID,
    "delta_test",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts a test that checks that the delta of 2 consequtive reads of certain field\r\n"
    " does not pass a threshold on several faps .\r\n",
    "Examples:\r\n"
    "  fap20v read_addr_start offset 0xbe00 ms_bit 28 ls_bit 16 fap_ids 0 1 2 3 \r\n"
    "   delta_test requires_time_normaliztion higher_bound 50 lower_bound 10 \r\n"
    "   polling_rate_in_system_ticks 3 routine_printout_every 1_hour\r\n"
    "Results in registering a polling serivce on the 4 faps above. The polling rate will be\n\r"
    " 3 system ticks (48 ms). The test will check that the delta value between 2 conswqutive calls\r\n"
    " is not greater then 50 and not lower then 10. If both thresholds are not passed in one hour\r\n"
    " the display will show a routine print to the screen\r\n",
    "",
    {0,
     0,
     0,
     BIT(4)
    },
    7,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_MEM_READ_NORMALIZATION_ID,
    "requires_time_normalization",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts a test that checks that the delta of 2 consequtive reads of certain field normalized\r\n"
    " to the (later passed) exact time interval does not pass a threshold on several faps .\r\n",
    "Examples:\r\n"
    "  fap20v read_addr_start offset 0xbe00 ms_bit 28 ls_bit 16 fap_ids 0 1 2 3 \r\n"
    "   delta_test requires_time_normaliztion higher_bound 50 lower_bound 10 \r\n"
    "   polling_rate_in_system_ticks 3 routine_printout_every 1_hour\r\n"
    "Results in registering a polling serivce on the 4 faps above. The polling rate will be\n\r"
    " 3 system ticks (48 ms). The test will check that the delta value between 2 conswqutive calls\r\n"
    " is not greater then 50 and not lower then 10. If both thresholds are not passed in one hour\r\n"
    " the display will show a routine print to the screen\r\n",
    "",
    {0,
     0,
     0,
     BIT(4)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_MEM_READ_VALUE_TEST_ID,
    "value_test",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts a test that checks that a certain field does not pass a threshold on several faps .\r\n",
    "Examples:\r\n"
    "  fap20v read_addr_start offset 0xbe00 ms_bit 28 ls_bit 16 fap_ids 0 1 2 3 \r\n"
    "   value_test requires_time_normaliztion higher_bound 50 lower_bound 10 \r\n"
    "   polling_rate_in_system_ticks 3 routine_printout_every 1_hour\r\n"
    "Results in registering a polling serivce on the 4 faps above. The polling rate will be\n\r"
    " 3 system ticks (48 ms). The test will check that the value read from the device\r\n"
    " is not greater then 50 and not lower then 10. If both thresholds are not passed in one hour\r\n"
    " the display will show a routine print to the screen.\r\n",
    "",
    {0,
     0,
     0,
     BIT(5)
    },
    7,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_MEM_READ_HIGH_BOUND_ID,
    "higher_bound",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts a test that checks that a certain field does not pass a threshold on several faps .\r\n",
    "Examples:\r\n"
    "  fap20v read_addr_start offset 0xbe00 ms_bit 28 ls_bit 16 fap_ids 0 1 2 3 \r\n"
    "   delta_test requires_time_normaliztion higher_bound 50 lower_bound 10 \r\n"
    "   polling_rate_in_system_ticks 3 routine_printout_every 1_hour\r\n"
    "Results in registering a polling serivce on the 4 faps above. The polling rate will be\n\r"
    " 3 system ticks (48 ms). The test will check that the delta value between 2 conswqutive calls\r\n"
    " is not greater then 50 and not lower then 10. If both thresholds are not passed in one hour\r\n"
    " the display will show a routine print to the screen\r\n",
    "",
    {0,
     0,
     0,
     BIT(4) | BIT(5)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_MEM_READ_LOW_BOUND_ID,
    "lower_bound",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts a test that checks that a certain field does not pass a threshold on several faps .\r\n",
    "Examples:\r\n"
    "  fap20v read_addr_start offset 0xbe00 ms_bit 28 ls_bit 16 fap_ids 0 1 2 3 \r\n"
    "   delta_test requires_time_normaliztion higher_bound 50 lower_bound 10 \r\n"
    "   polling_rate_in_system_ticks 3 routine_printout_every 1_hour\r\n"
    "Results in registering a polling serivce on the 4 faps above. The polling rate will be\n\r"
    " 3 system ticks (48 ms). The test will check that the delta value between 2 conswqutive calls\r\n"
    " is not greater then 50 and not lower then 10. If both thresholds are not passed in one hour\r\n"
    " the display will show a routine print to the screen\r\n",
    "",
    {0,
     0,
     0,
     BIT(4) | BIT(5)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_MEM_READ_SYS_TICKS_ID,
    "polling_rate_in_system_ticks",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "A system tick is approx 16 ms (60 ticks in a second).\r\n",
    "Examples:\r\n"
    "  fap20v read_addr_start offset 0xbe00 ms_bit 28 ls_bit 16 fap_ids 0 1 2 3 \r\n"
    "   delta_test requires_time_normaliztion higher_bound 50 lower_bound 10 \r\n"
    "   polling_rate_in_system_ticks 3 routine_printout_every 1_hour\r\n"
    "Results in registering a polling serivce on the 4 faps above. The polling rate will be\n\r"
    " 3 system ticks (48 ms). The test will check that the delta value between 2 conswqutive calls\r\n"
    " is not greater then 50 and not lower then 10. If both thresholds are not passed in one hour\r\n"
    " the display will show a routine print to the screen\r\n",
    "",
    {0,
     0,
     0,
     BIT(4) | BIT(5)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_MEM_READ_PRINT_INTERVAL_ID,
    "routine_printout_every",
    (PARAM_VAL_RULES *)&Fap20v_print_interval_vals[0],
    (sizeof(Fap20v_print_interval_vals) / sizeof(Fap20v_print_interval_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Starts a test that checks that a certain field does not pass a threshold on several faps .\r\n",
    "Examples:\r\n"
    "  fap20v read_addr_start offset 0xbe00 ms_bit 28 ls_bit 16 fap_ids 0 1 2 3 \r\n"
    "   delta_test requires_time_normaliztion higher_bound 50 lower_bound 10 \r\n"
    "   polling_rate_in_system_ticks 3 routine_printout_every 1_hour\r\n"
    "Results in registering a polling serivce on the 4 faps above. The polling rate will be\n\r"
    " 3 system ticks (48 ms). The test will check that the delta value between 2 conswqutive calls\r\n"
    " is not greater then 50 and not lower then 10. If both thresholds are not passed in one hour\r\n"
    " the display will show a routine print to the screen\r\n",
    "",
    {0,
     0,
     0,
     BIT(4) | BIT(5)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_NOF_FAP_IDS_ID,
    "nof_fap_ids",
    (PARAM_VAL_RULES *)&Fap20v_fap_id[0],
    (sizeof(Fap20v_fap_id) / sizeof(Fap20v_fap_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Number of target faps to use\r\n",
    "Examples:\r\n"
    "  fap20v tests app set_queue_label_group 0 start_fap_id 0 nof_fap_ids 128 start_queue_id 8188 queue_block_size 64\r\n"
    "Open on FAP 0 (driver id not FAP-id), at the LBP unicast table 128 tatget faps starting at fap 0.\n\r"
    "  The queue number to use are 8188, 60, 124, ... (all module 8K).\r\n",
    "",
    {0,
     0,
     BIT(18)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_START_QUEUE_ID_ID,
    "start_queue_id",
    (PARAM_VAL_RULES *)&Fap20v_queue_id[0],
    (sizeof(Fap20v_queue_id) / sizeof(Fap20v_queue_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Start queue id to set\r\n",
    "Examples:\r\n"
    "  fap20v tests app set_queue_label_group 0 start_fap_id 0 nof_fap_ids 128 start_queue_id 8188 queue_block_size 64\r\n"
    "Open on FAP 0 (driver id not FAP-id), at the LBP unicast table 128 tatget faps starting at fap 0.\n\r"
    "  The queue number to use are 8188, 60, 124, ... (all module 8K).\r\n",
    "",
    {0,
     0,
     BIT(18)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TARGET_FAP_ID_ID,
    "start_fap_id",
    (PARAM_VAL_RULES *)&Fap20v_fap_id[0],
    (sizeof(Fap20v_fap_id) / sizeof(Fap20v_fap_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Start fap id to set\r\n",
    "Examples:\r\n"
    "  fap20v tests app set_queue_label_group 0 start_fap_id 0 nof_fap_ids 128 start_queue_id 8188 queue_block_size 64\r\n"
    "Open on FAP 0 (driver id not FAP-id), at the LBP unicast table 128 tatget faps starting at fap 0.\n\r"
    "  The queue number to use are 8188, 60, 124, ... (all module 8K).\r\n",
    "",
    {0,
     0,
     BIT(18)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_QUEUE_BLOCK_SIZE_ID,
    "queue_block_size",
    (PARAM_VAL_RULES *)&Fap20v_nof_queues[0],
    (sizeof(Fap20v_nof_queues) / sizeof(Fap20v_nof_queues[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "How many queues to add in every table entry.\r\n",
    "Examples:\r\n"
    "  fap20v tests app set_queue_label_group 0 start_fap_id 0 nof_fap_ids 128 start_queue_id 8188 queue_block_size 64\r\n"
    "Open on FAP 0 (driver id not FAP-id), at the LBP unicast table 128 tatget faps starting at fap 0.\n\r"
    "  The queue number to use are 8188, 60, 124, ... (all module 8K).\r\n",
    "",
    {0,
     0,
     BIT(18)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_APP_OPEN_FLOW_QUEUE_GROUP_ID,
    "open_queue_flow_group",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Rapid flow/queue app.\r\n",
    "Examples:\r\n"
    "  fap20v tests app open_queue_flow_group 0 target_fap_id 0 start_queue_id 4 start_flow_id 4 nof_queue_flow 800 nof_flows_per_port 64 start_port_id 0 nof_ports 8 qos_scheme 0 qos_free_param 10000\r\n"
    "Open on FAP 0 (driver id), 800 flows/queues, both use target fap id - 0,\n\r"
    "  the start queue and flow ids are 4. Number of flow before port\n\r"
    "  is incrementing is 64, port number will run from 0 and will wrap at 8\n\r"
    "  (8 will not be used).\r\n",

    "",
    {0,
     0,
     BIT(17)
    },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TARGET_FAP_ID_ID,
    "target_fap_id",
    (PARAM_VAL_RULES *)&Fap20v_fap_id[0],
    (sizeof(Fap20v_fap_id) / sizeof(Fap20v_fap_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Target fap to use\r\n",
    "Examples:\r\n"
    "  fap20v tests app open_queue_flow_group 0 target_fap_id 0 start_queue_id 4 start_flow_id 4 nof_queue_flow 800 nof_flows_per_port 64 start_port_id 0 nof_ports 8 qos_scheme 0 qos_free_param 10000\r\n"
    "Open on FAP 0 (driver id), 800 flows/queues, both use target fap id - 0,\n\r"
    "  the start queue and flow ids are 4. Number of flow before port\n\r"
    "  is incrementing is 64, port number will run from 0 and will wrap at 8\n\r"
    "  (8 will not be used).\r\n",
    "",
    {0,
     0,
     BIT(17)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_QUEUE_ID_ID,
    "start_queue_id",
    (PARAM_VAL_RULES *)&Fap20v_queue_id[0],
    (sizeof(Fap20v_queue_id) / sizeof(Fap20v_queue_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The first queue id to use.\r\n",
    "Examples:\r\n"
    "  fap20v tests app open_queue_flow_group 0 target_fap_id 0 start_queue_id 4 start_flow_id 4 nof_queue_flow 800 nof_flows_per_port 64 start_port_id 0 nof_ports 8 qos_scheme 0 qos_free_param 10000\r\n"
    "Open on FAP 0 (driver id), 800 flows/queues, both use target fap id - 0,\n\r"
    "  the start queue and flow ids are 4. Number of flow before port\n\r"
    "  is incrementing is 64, port number will run from 0 and will wrap at 8\n\r"
    "  (8 will not be used).\r\n",
    "",
    {0,
     0,
     BIT(17)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_FLOW_ID_ID,
    "start_flow_id",
    (PARAM_VAL_RULES *)&Fap20v_flow_id[0],
    (sizeof(Fap20v_flow_id) / sizeof(Fap20v_flow_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The first flow id to use.\r\n",
    "Examples:\r\n"
    "  fap20v tests app open_queue_flow_group 0 target_fap_id 0 start_queue_id 4 start_flow_id 4 nof_queue_flow 800 nof_flows_per_port 64 start_port_id 0 nof_ports 8 qos_scheme 0 qos_free_param 10000\r\n"
    "Open on FAP 0 (driver id), 800 flows/queues, both use target fap id - 0,\n\r"
    "  the start queue and flow ids are 4. Number of flow before port\n\r"
    "  is incrementing is 64, port number will run from 0 and will wrap at 8\n\r"
    "  (8 will not be used).\r\n",
    "",
    {0,
     0,
     BIT(17)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_NOF_QUEUES_ID,
    "nof_queue_flow",
    (PARAM_VAL_RULES *)&Fap20v_nof_queues[0],
    (sizeof(Fap20v_nof_queues) / sizeof(Fap20v_nof_queues[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Total number of queues/flows to open.\r\n",
    "Examples:\r\n"
    "  fap20v tests app open_queue_flow_group 0 target_fap_id 0 start_queue_id 4 start_flow_id 4 nof_queue_flow 800 nof_flows_per_port 64 start_port_id 0 nof_ports 8 qos_scheme 0 qos_free_param 10000\r\n"
    "Open on FAP 0 (driver id), 800 flows/queues, both use target fap id - 0,\n\r"
    "  the start queue and flow ids are 4. Number of flow before port\n\r"
    "  is incrementing is 64, port number will run from 0 and will wrap at 8\n\r"
    "  (8 will not be used).\r\n",
    "",
    {0,
     0,
     BIT(17)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_NOF_FLOWS_PER_PORT_ID,
    "nof_flows_per_port",
    (PARAM_VAL_RULES *)&Fap20v_nof_queues[0],
    (sizeof(Fap20v_nof_queues) / sizeof(Fap20v_nof_queues[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Number of flows before the port number get incremented.\r\n",
    "Examples:\r\n"
    "  fap20v tests app open_queue_flow_group 0 target_fap_id 0 start_queue_id 4 start_flow_id 4 nof_queue_flow 800 nof_flows_per_port 64 start_port_id 0 nof_ports 8 qos_scheme 0 qos_free_param 10000\r\n"
    "Open on FAP 0 (driver id), 800 flows/queues, both use target fap id - 0,\n\r"
    "  the start queue and flow ids are 4. Number of flow before port\n\r"
    "  is incrementing is 64, port number will run from 0 and will wrap at 8\n\r"
    "  (8 will not be used).\r\n",
    "",
    {0,
     0,
     BIT(17)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_QOS_SCHEME_ID,
    "qos_scheme",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Quality of service scheme.\r\n"
    "0 - edf0 for all flows, free param the Kilo Bit Sec.\n\r"
    "1 - edf1 for all flows, free param the Kilo Bit Sec\n\r"
    "2 - wfq for all the wheels, free param the weight .\n\r"
    "3 - edf0 for all the wheels, Kilo Bit Sec start at 0 and increments by free param.\n\r"
    "4 - edf1 for all the wheels, Kilo Bit Sec start at 0 and increments by free param.\n\r"
    "5 - edf0 for one flow, edf1 for the second flow, free param the Kilo Bit Sec.\n\r"
    "6 - 6 - edf1 for one flow free param the Kilo Bit Sec, wfq for the other flow weight - 30.\n\r"
    "7 - rcy flow for each wheel free param the weight\n\r"
    "8 - edf0,edf1 for even flows free param the Kilo Bit Sec,\n\r"
    "    wfq for odd flows weight - 30.\n\r"
    "    FSF 8 ==> 3 sub flows on one flows\n\r"
    "9 - edf0,edf1 for even flows free param the Kilo Bit Sec,\n\r"
    "    wfq for odd flows weight - 30, edf0 for odd flows minimum BW.\n\r"
    "    FSF 4 ==> 4 sub flows on one flows\n\r",
    "Examples:\r\n"
    "  fap20v tests app open_queue_flow_group 0 target_fap_id 0 start_queue_id 4 start_flow_id 4 nof_queue_flow 800 nof_flows_per_port 64 start_port_id 0 nof_ports 8 qos_scheme 0 qos_free_param 10000\r\n"
    "Open on FAP 0 (driver id), 800 flows/queues, both use target fap id - 0,\n\r"
    "  the start queue and flow ids are 4. Number of flow before port\n\r"
    "  is incrementing is 64, port number will run from 0 and will wrap at 8\n\r"
    "  (8 will not be used).\r\n",
    "",
    {0,
     0,
     BIT(17)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_QOS_FREE_PARAM_ID,
    "qos_free_param",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Quality of service parameter, changes with respect to the QOS scheme.\r\n"
    "0 - edf0 for all flows, free param the Kilo Bit Sec.\n\r"
    "1 - edf1 for all flows, free param the Kilo Bit Sec\n\r"
    "2 - wfq for all the wheels, free param the weight .\n\r"
    "3 - edf0 for all the wheels, Kilo Bit Sec start at 0 and increments by free param.\n\r"
    "4 - edf1 for all the wheels, Kilo Bit Sec start at 0 and increments by free param.\n\r"
    "5 - edf0 for one flow, edf1 for the second flow, free param the Kilo Bit Sec.\n\r"
    "6 - 6 - edf1 for one flow free param the Kilo Bit Sec, wfq for the other flow weight - 30.\n\r"
    "7 - rcy flow for each wheel free param the weight\n\r"
    "8 - edf0,edf1 for even flows free param the Kilo Bit Sec,\n\r"
    "    wfq for odd flows weight - 30.\n\r"
    "    FSF 8 ==> 3 sub flows on one flows\n\r"
    "9 - edf0,edf1 for even flows free param the Kilo Bit Sec,\n\r"
    "    wfq for odd flows weight - 30, edf0 for odd flows minimum BW.\n\r"
    "    FSF 4 ==> 4 sub flows on one flows\n\r",
    "Examples:\r\n"
    "  fap20v tests app open_queue_flow_group 0 target_fap_id 0 start_queue_id 4 start_flow_id 4 nof_queue_flow 800 nof_flows_per_port 64 start_port_id 0 nof_ports 8 qos_scheme 0 qos_free_param 10000\r\n"
    "Open on FAP 0 (driver id), 800 flows/queues, both use target fap id - 0,\n\r"
    "  the start queue and flow ids are 4. Number of flow before port\n\r"
    "  is incrementing is 64, port number will run from 0 and will wrap at 8\n\r"
    "  (8 will not be used).\r\n",
    "",
    {0,
     0,
     BIT(17)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_START_PORT_ID_ID,
    "start_port_id",
    (PARAM_VAL_RULES *)&Fap20v_scheduler_port_id_vals[0],
    (sizeof(Fap20v_scheduler_port_id_vals) / sizeof(Fap20v_scheduler_port_id_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The first port number.\r\n",
    "Examples:\r\n"
    "  fap20v tests app open_queue_flow_group 0 target_fap_id 0 start_queue_id 4 start_flow_id 4 nof_queue_flow 800 nof_flows_per_port 64 start_port_id 0 nof_ports 8 qos_scheme 0 qos_free_param 10000\r\n"
    "Open on FAP 0 (driver id), 800 flows/queues, both use target fap id - 0,\n\r"
    "  the start queue and flow ids are 4. Number of flow before port\n\r"
    "  is incrementing is 64, port number will run from 0 and will wrap at 8\n\r"
    "  (8 will not be used).\r\n",
    "",
    {0,
     0,
     BIT(17)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_NOF_PORTS_ID,
    "nof_ports",
    (PARAM_VAL_RULES *)&Fap20v_scheduler_port_id_vals[0],
    (sizeof(Fap20v_scheduler_port_id_vals) / sizeof(Fap20v_scheduler_port_id_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Number of ports.\r\n",
    "Examples:\r\n"
    "  fap20v tests app open_queue_flow_group 0 target_fap_id 0 start_queue_id 4 start_flow_id 4 nof_queue_flow 800 nof_flows_per_port 64 start_port_id 0 nof_ports 8 qos_scheme 0 qos_free_param 10000\r\n"
    "Open on FAP 0 (driver id), 800 flows/queues, both use target fap id - 0,\n\r"
    "  the start queue and flow ids are 4. Number of flow before port\n\r"
    "  is incrementing is 64, port number will run from 0 and will wrap at 8\n\r"
    "  (8 will not be used).\r\n",
    "",
    {0,
     0,
     BIT(17)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_SCENARIO_PKT_ID,
    "packet_utils",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap20v cells test.\r\n",
    "Examples:\r\n"
    "  fap20v tests packet_utils send_packet_test_fdt 0 target_fap_id 0 pattern 0xFF0022FF pkt_byte_size 1024 nof_pkts 1000\r\n"
    "Results in activating packet tests.\r\n",
    "",
    {0,
     BIT(30) | BIT(31),
     BIT(16)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_SCENARIO_1_ID,
    "cells",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap20v cell test.\r\n",
    "Examples:\r\n"
    "  fap20v test cells\r\n"
    "Results in activating cells tests using direct/deferred.\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_SCENARIO_2_ID,
    "sch_flows",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap20v scheduler-flows test.\r\n",
    "Examples:\r\n"
    "  fap20v test sch_flows\r\n"
    "Results in activating scheduler-flows tests.\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_SCENARIO_3_ID,
    "q_diag",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap20v queue diagnostics (QDP Profiler) test.\r\n",
    "Examples:\r\n"
    "  fap20v test q_diag\r\n"
    "Results in stopping the traffic, testing queue DB coherency, restarting traffic.\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_SCENARIO_4_ID,
    "packets",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap20v traffic test.\r\n",
    "Examples:\r\n"
    "  fap20v test packets\r\n"
    "Results in activating pakcets tests using direct/deferred.\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_SCENARIO_5_ID,
    "links",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap20v links test.\r\n",
    "Examples:\r\n"
    "  fap20v test links\r\n"
    "Results in activating links tests using direct and deferred calls.\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_SCENARIO_6_ID,
    "routing",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap20v routing test.\r\n",
    "Examples:\r\n"
    "  fap20v test_scenario_routing direct \r\n"
    "Results in activating routing tests using direct (and not deferred calls).\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_SCENARIO_7_ID,
    "statistics",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap20v statistics test.\r\n",
    "Examples:\r\n"
    "  fap20v test_scenario_statistics direct \r\n"
    "Results in activating statistics tests using direct (and not deferred calls).\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_SCENARIO_8_ID,
    "diagnostics",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap20v diagnostics test.\r\n",
    "Examples:\r\n"
    "  fap20v test_scenario_diagnostics direct \r\n"
    "Results in activating diagnostics tests using direct (and not deferred calls).\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_SCENARIO_9_ID,
    "general",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap20v agent general module test.\r\n",
    "Examples:\r\n"
    "  fap20v test general\r\n"
    "Results in activating test on the general module through the FMC.\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_SCENARIO_10_ID,
    "spatial_multicast",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap20v agent spatial-multicast module test.\r\n",
    "Examples:\r\n"
    "  fap20v test spatial_multicast\r\n"
    "Results in activating test on the spatial-multicast module through the FMC.\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_SCENARIO_11_ID,
    "recycle_multicast",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap20v agent recycle-multicast module test.\r\n",
    "Examples:\r\n"
    "  fap20v test recycle_multicast\r\n"
    "Results in activating test on the recycle-multicast module through the FMC.\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_SCENARIO_12_ID,
    "configuration",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap20v agent configuration module test.\r\n",
    "Examples:\r\n"
    "  fap20v test configuration\r\n"
    "Results in activating test on the configuration module through the FMC.\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_ZBT_INTERRUPT_ID,
    "zbt_interrupt",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap20v agent zbt interrupt test.\r\n",
    "Examples:\r\n"
    "  fap20v test zbt_interrupt\r\n"
    "Results in activating test on the zbt interrupt through the FMC.\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_ALL_ID,
    "all",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates all fap20v tests.\r\n",
    "Examples:\r\n"
    "  fap20v test all\r\n"
    "Results in activating all fap20v tests through the ATM and FMC.\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TEST_ALL_SHOTER_TEST_ID,
    "shorter_test",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Make the test shorter.\r\n",
    "Examples:\r\n"
    "  fap20v test all shorter_test\r\n"
    "Results in activating all fap20v tests in shorter mode.\r\n",
    "",
    {BIT(31)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_FAP20V_TEST_CALLBACKS_ID,
    "callbacks",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "activates fap20v agent callbacks tests.\r\n",
    "Examples:\r\n"
    "  fap20v test all\r\n"
    "Results in activating fap20v agent callbacks tests through the FMC.\r\n",
    "",
    {BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SEND_PACKET_TEST_FDT_ID,
    "send_packet_test_fdt",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This procedure transmits packets through the FDT\n\r"
    "  and expect to recive them through the CPU interface.\r\n"
    "  It them check that the packet payload was unchanged.",
    "Examples:\r\n"
    "  fap20v tests packets send_packet_test_fdt 0 nof_pkts 10 target_fap_id 0\r\n"
    "Results in sending to 10 packets to FAP with id of 0, from device 0\r\n"
    "  and than check there payload.\r\n",
    "",
    {0,
     BIT(30) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_PORT_ID,
    "pp_port",
    (PARAM_VAL_RULES *)&Fap20v_scheduler_port_id_no_recyle_vals[0],
    (sizeof(Fap20v_scheduler_port_id_no_recyle_vals) / sizeof(Fap20v_scheduler_port_id_no_recyle_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Fap20v_pp_port_default,
    0,0,0,0,0,0,0,
    "Prestera port to send packet to. Range 0-63 (63 is the CPU port)",
    "Examples:\r\n"
    "  fap20v tests packets send_packet_to_pp 0 nof_pkts 10 packet_type 0 port 5\r\n"
    "Results in sending packet (64  bytes), through the FDT,\n\r"
    "  via the fabric (or loop-backs), to the FAP.\n\r"
    "  Next to the PP and to PORT 5, VLAN 1.\r\n"
    "  Note: We've reached 700 Meg-bit/sec through this method\r\n",
    "",
    {0,
     BIT(31)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_VLAN_ID_ID,
    "vlan_id",
    (PARAM_VAL_RULES *)&Fap20v_vlan_id_vals[0],
    (sizeof(Fap20v_vlan_id_vals) / sizeof(Fap20v_vlan_id_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Fap20v_vlan_id_default,
    0,0,0,0,0,0,0,
    "Prestera VLAN id. Range 0-4095",
    "Examples:\r\n"
    "  fap20v tests packets send_packet_to_pp 0 nof_pkts 10 packet_type 0 port 5 vlan_id 1\r\n"
    "Results in sending packet (64  bytes), through the FDT,\n\r"
    "  via the fabric (or loop-backs), to the FAP.\n\r"
    "  Next to the PP and to PORT 5, VLAN 1.\r\n"
    "  Note: We've reached 700 Meg-bit/sec through this method\r\n",
    "",
    {0,
     BIT(31)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_NOF_PACKETS_ID,
    "nof_pkts",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This procedure transmits packets through the FDT\n\r"
    "  and expect to recive them through the CPU interface.\r\n"
    "  It them check that the packet payload was unchanged.\n\r",
    "Examples:\r\n"
    "  fap20v tests packets send_packet_test_fdt 0 nof_pkts 10 target_fap_id 0\r\n"
    "Results in sending to 10 packets to FAP with id of 0, from device 0\r\n"
    "  and than check there payload.\r\n",
    "",
    {0,
     BIT(30) | BIT(31) },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_PACKET_BYTE_SIZE_ID,
    "pkt_byte_size",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Packet payload byte size.\n\r",
    "Examples:\r\n"
    "  fap20v tests packets send_packet_test_fdt 0 nof_pkts 10 pkt_byte_size 60 target_fap_id 0\r\n"
    "Results in sending to 10 packets to FAP with id of 0, from device 0\r\n"
    "  and than check there payload.\r\n",
    "",
    {0,
     BIT(30) },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_PACKET_PATTERN_ID,
    "pattern",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Packet payload byte size.\n\r",
    "Examples:\r\n"
    "  fap20v tests packets send_packet_test_fdt 0 nof_pkts 10 pkt_byte_size 60 target_fap_id 0 pattren 0xFE\r\n"
    "Results in sending to 10 packets to FAP with id of 0, from device 0\r\n"
    "  and than check there payload.\r\n",
    "",
    {0,
     BIT(30) },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TARGET_FAP_ID_ID,
    "target_fap_id",
    (PARAM_VAL_RULES *)&Fap20v_fap_id[0],
    (sizeof(Fap20v_fap_id) / sizeof(Fap20v_fap_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Target FAP identifier.",
    "Examples:\r\n"
    "  fap20v tests packets send_packet_test_fdt 0 nof_pkts 10 target_fap_id 0\r\n"
    "Results in sending to 10 packets to FAP with id of 0, from device 0\r\n"
    "  and than check there payload.\r\n",
    "",
    {0,
     BIT(30) | BIT(31) },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /*
   * END Test API }
   */

  /* { fap20v_api_configuration.h interface */
  {
    PARAM_FAP20V_CONFIGURATION_API_ID,
    "configuration",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver configuration services.",
    "Examples:\r\n"
    "  fap20v configuration set_fap_id 1 fap_id 123 \r\n"
    "Results in setting on device 1, fap identifier of 123.\n\r"
    "This fap identifier is the identifier that the device is seen\n\r"
    "in the fabric system. while the 'unit' is the identifier\n\r"
    "of that fap in the context of the local driver\r\n",
    "",
    {0,
     0,
     BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5) | BIT(6) | BIT(7) | BIT(8)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_GET_MIRROR_ID,
    "mirror_get",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get mirroring information. \n\r",
    "For example\r\n"
    "fap20v mirror_get 0 \n\r",
    "  Result in getting all the mirroring information.",
    {0,0, BIT(8) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_GET_SNOOP_ID,
    "snooping_get",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get snooping information. \n\r",
    "For example\r\n"
    "fap20v snooping_get 0 \n\r",
    "  Result in getting all the snooping information.",
    {0,0, BIT(8) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SET_WRED_PARAMS_ID,
    "set_wred_params",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Sets the WRED parameters of the specific 'queue_type' and 'drop_precedence'.",
    "Examples:\r\n"
    "  fap20v configuration set_wred_params 0 queue_type 2 drop_precedence 3 max_packet_size 1024 max_probability 54 min_avrg_th 10000 max_avrg_th 20000\r\n"
    "Results in setting (on device 0) for queue type 2 \n\r"
    " and drop precedence of 3 its WRED parameters:\r\n"
    " The WRED alg. max packet size is 1024,\r\n"
    " maximum probability of discarding a packet is 54%%,\r\n"
    " if queue average size is below 10000 all packet are admitted\r\n"
    " and if queue average size is above 20000 all packet are discarded.\n\r",
    "",
    {0,
     0,
     BIT(6)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_MAX_PACKET_SIZE_ID,
    "max_packet_size",
    (PARAM_VAL_RULES *)&Fap20v_max_packet_size[0],
    (sizeof(Fap20v_max_packet_size) / sizeof(Fap20v_max_packet_size[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Max packet size for the WRED algorithm.\n\r"
    "  Size of packet in bytes.",
    "Examples:\r\n"
    "  fap20v configuration set_wred_params 0 queue_type 2 drop_precedence 3 max_packet_size 1024 max_probability 54 min_avrg_th 10000 max_avrg_th 20000\r\n"
    "Results in setting (on device 0) for queue type 2 \n\r"
    " and drop precedence of 3 its WRED parameters:\r\n"
    " The WRED alg. max packet size is 1024,\r\n"
    " maximum probability of discarding a packet is 54%%,\r\n"
    " if queue average size is below 10000 all packet are admitted\r\n"
    " and if queue average size is above 20000 all packet are discarded.\n\r",
    "",
    {0,
     0,
     BIT(6)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_MAX_PROBABILITY_ID,
    "max_probability",
    (PARAM_VAL_RULES *)&Fap20v_percent[0],
    (sizeof(Fap20v_percent) / sizeof(Fap20v_percent[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The maximum probability of discarding a packet (when the queue reaches\n\r"
    "  the maximum average size ('max_avrg_th') and the packet size is the\n\r"
    "  maximum size ('max_packet_size')).",
    "Examples:\r\n"
    "  fap20v configuration set_wred_params 0 queue_type 2 drop_precedence 3 max_packet_size 1024 max_probability 54 min_avrg_th 10000 max_avrg_th 20000\r\n"
    "Results in setting (on device 0) for queue type 2 \n\r"
    " and drop precedence of 3 its WRED parameters:\r\n"
    " The WRED alg. max packet size is 1024,\r\n"
    " maximum probability of discarding a packet is 54%%,\r\n"
    " if queue average size is below 10000 all packet are admitted\r\n"
    " and if queue average size is above 20000 all packet are discarded.\n\r",
    "",
    {0,
     0,
     BIT(6)},
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_MIN_AVRG_TH_ID,
    "min_avrg_th",
    (PARAM_VAL_RULES *)&Fap20v_min_avrg_th[0],
    (sizeof(Fap20v_min_avrg_th) / sizeof(Fap20v_min_avrg_th[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Relating to queue average size.\n\r"
    "  Maximum Average Threshold - Above this threshold,\n\r"
    "  packet is discarded from the queue.\n\r"
    "  Resolution of 1-Kbyte (64 is 64K bytes)\n\r"
    "  The SOC_SAND_FAP20V resolution is of 64K bytes (1-63 KB will be round up to 64 KB)",
    "Examples:\r\n"
    "  fap20v configuration set_wred_params 0 queue_type 2 drop_precedence 3 max_packet_size 1024 max_probability 54 min_avrg_th 10000 max_avrg_th 20000\r\n"
    "Results in setting (on device 0) for queue type 2 \n\r"
    " and drop precedence of 3 its WRED parameters:\r\n"
    " The WRED alg. max packet size is 1024,\r\n"
    " maximum probability of discarding a packet is 54%%,\r\n"
    " if queue average size is below 10000 all packet are admitted\r\n"
    " and if queue average size is above 20000 all packet are discarded.\n\r",
    "",
    {0,
     0,
     BIT(6)},
    7,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_MAX_AVRG_TH_ID,
    "max_avrg_th",
    (PARAM_VAL_RULES *)&Fap20v_max_avrg_th[0],
    (sizeof(Fap20v_max_avrg_th) / sizeof(Fap20v_max_avrg_th[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Relating to queue average size.\n\r"
    "  Maximum Average Threshold - Above this threshold,\n\r"
    "  packet is discarded from the queue.\n\r"
    "  Resolution of 1-Kbyte (64 is 64K bytes)\n\r"
    "  The SOC_SAND_FAP20V resolution is of 64K bytes (1-63 KB will be round up to 64 KB)",
    "Examples:\r\n"
    "  fap20v configuration set_wred_params 0 queue_type 2 drop_precedence 3 max_packet_size 1024 max_probability 54 min_avrg_th 10000 max_avrg_th 20000\r\n"
    "Results in setting (on device 0) for queue type 2 \n\r"
    " and drop precedence of 3 its WRED parameters:\r\n"
    " The WRED alg. max packet size is 1024,\r\n"
    " maximum probability of discarding a packet is 54%%,\r\n"
    " if queue average size is below 10000 all packet are admitted\r\n"
    " and if queue average size is above 20000 all packet are discarded.\n\r",
    "",
    {0,
     0,
     BIT(6)},
    8,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_FAP20V_GET_WRED_PARAMS_ID,
    "get_wred_params",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gets the WRED parameters of the specific 'queue_type' and 'drop_precedence'.",
    "Examples:\r\n"
    "  fap20v configuration get_wred_params 0 queue_type 2 drop_precedence 3\r\n"
    "Results in getting (on device 0) for queue type 2 \n\r"
    "  and drop precedence of 3 its WRED parameters.\r\n",
    "",
    {0,
     0,
     BIT(5)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_DROP_PRECEDENCE_ID,
    "drop_precedence",
    (PARAM_VAL_RULES *)&Fap20v_drop_precedence[0],
    (sizeof(Fap20v_drop_precedence) / sizeof(Fap20v_drop_precedence[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Drop precedence identifier.",
    "Examples:\r\n"
    "  fap20v configuration get_wred_params 0 queue_type 2 drop_precedence 3\r\n"
    "Results in getting (on device 0) for queue type 2 \n\r"
    "  and drop precedence of 3 its WRED parameters.\r\n",
    "",
    {0,
     0,
     BIT(5) | BIT(6)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL  },

  {
    PARAM_FAP20V_SET_QUEUE_TYPE_PARAMS_ID,
    "set_queue_type_params",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Sets the required queue type parameters.",
    "Examples:\r\n"
    "  fap20v configuration set_queue_type_params 1 queue_type 2 max_queue_size 1200 flow_stat_msg 500 flow_slow_th 300 RED_exp_weight 5 \r\n"
    "Results in setting (on device 1) queue type 2 parameters to:\r\n"
    " Maximum queue size 1200 bytes,\n\r"
    " every time the queue size cross over multiple of 500 the queue \r\n"
    " is to sent status message to its related flow,\n\r"
    " if the queue size is beneath 300 the queue is considered in 'Slow' state,\n\r"
    " RED_exp_weight appear - indication that RED algorithm is applying this queue\n\r"
    " type with the specified exponent.\n\r",
    "",
    {0,
     0,
     BIT(4)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_MAX_QUEUE_SIZE_ID,
    "max_queue_size",
    (PARAM_VAL_RULES *)&Fap20v_max_queue_size[0],
    (sizeof(Fap20v_max_queue_size) / sizeof(Fap20v_max_queue_size[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Max queue size. When a queue reaches this max size,\n\r"
    "  all packets reaches this queue are droped.\n\r"
    "  Byte resolution.",
    "Examples:\r\n"
    "  fap20v configuration set_queue_type_params 1 queue_type 2 max_queue_size 1200 flow_stat_msg 500 flow_slow_th 300 RED_exp_weight 5 \r\n"
    "Results in setting (on device 1) queue type 2 parameters to:\r\n"
    " Maximum queue size 1200 bytes,\n\r"
    " every time the queue size cross over multiple of 500 the queue \r\n"
    " is to sent status message to its related flow,\n\r"
    " if the queue size is beneath 300 the queue is considered in 'Slow' state,\n\r"
    " RED_exp_weight appear - indication that RED algorithm is applying this queue\n\r"
    " type with the specified exponent.\n\r",
    "",
    {0,
     0,
     BIT(4)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_FLOW_STAT_MSG_ID,
    "flow_stat_msg",
    (PARAM_VAL_RULES *)&Fap20v_flow_stat_msg[0],
    (sizeof(Fap20v_flow_stat_msg) / sizeof(Fap20v_flow_stat_msg[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Every integer multiple of this parameter, a status message is sent to\n\r"
    "  the flow related to that queue.\n\r"
    "  Byte resolution.",
    "Examples:\r\n"
    "  fap20v configuration set_queue_type_params 1 queue_type 2 max_queue_size 1200 flow_stat_msg 500 flow_slow_th 300 RED_exp_weight 5 \r\n"
    "Results in setting (on device 1) queue type 2 parameters to:\r\n"
    " Maximum queue size 1200 bytes,\n\r"
    " every time the queue size cross over multiple of 500 the queue \r\n"
    " is to sent status message to its related flow,\n\r"
    " if the queue size is beneath 300 the queue is considered in 'Slow' state,\n\r"
    " RED_exp_weight appear - indication that RED algorithm is applying this queue\n\r"
    " type with the specified exponent.\n\r",
    "",
    {0,
     0,
     BIT(4)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_FLOW_SLOW_TH_ID,
    "flow_slow_th_up",
    (PARAM_VAL_RULES *)&Fap20v_flow_slow_th[0],
    (sizeof(Fap20v_flow_slow_th) / sizeof(Fap20v_flow_slow_th[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Beneath this threshold, the queue is in 'Slow' state.\n\r"
    "  Byte resolution.",
    "Examples:\r\n"
    "  fap20v configuration set_queue_type_params 1 queue_type 2 max_queue_size 1200 flow_stat_msg 500 flow_slow_th 300 RED_exp_weight 5 \r\n"
    "Results in setting (on device 1) queue type 2 parameters to:\r\n"
    " Maximum queue size 1200 bytes,\n\r"
    " every time the queue size cross over multiple of 500 the queue \r\n"
    " is to sent status message to its related flow,\n\r"
    " if the queue size is beneath 300 the queue is considered in 'Slow' state,\n\r"
    " RED_exp_weight appear - indication that RED algorithm is applying this queue\n\r"
    " type with the specified exponent.\n\r",
    "",
    {0,
     0,
     BIT(4)},
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_FLOW_SLOW_HYS_ID,
    "slow_hystersis_size",
    (PARAM_VAL_RULES *)&Fap20v_flow_slow_th[0],
    (sizeof(Fap20v_flow_slow_th) / sizeof(Fap20v_flow_slow_th[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The flow status would be changed from FAST to SLOW.\n\r"
    "  when the queue size is less \n\r"
    "  than flow_slow_th - slow_hystersis_size \n\r"
    "  When slow_hystersis_size == 0, the slow hysteresis is disabled.\n\r"
    "  Byte resolution.",
    "Examples:\r\n"
    "  fap20v configuration set_queue_type_params 1 queue_type 2 max_queue_size 1200 flow_stat_msg 500 flow_slow_th 300 RED_exp_weight 5 \r\n"
    "Results in setting (on device 1) queue type 2 parameters to:\r\n"
    " Maximum queue size 1200 bytes,\n\r"
    " every time the queue size cross over multiple of 500 the queue \r\n"
    " is to sent status message to its related flow,\n\r"
    " if the queue size is beneath 300 the queue is considered in 'Slow' state,\n\r"
    " RED_exp_weight appear - indication that RED algorithm is applying this queue\n\r"
    " type with the specified exponent.\n\r",
    "",
    {0,
     0,
     BIT(4)},
    7,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_HDR_CMP_ID,
    "header_compensation",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Header Compensation.\n\r"
    "  Byte resolution.",
    "Examples:\r\n"
    "  fap20v configuration set_queue_type_params 1 queue_type 2 max_queue_size 1200 flow_stat_msg 500 flow_slow_th 300 RED_exp_weight 5 \r\n"
    "Results in setting (on device 1) queue type 2 parameters to:\r\n"
    " Maximum queue size 1200 bytes,\n\r"
    " every time the queue size cross over multiple of 500 the queue \r\n"
    " is to sent status message to its related flow,\n\r"
    " if the queue size is beneath 300 the queue is considered in 'Slow' state,\n\r"
    " RED_exp_weight appear - indication that RED algorithm is applying this queue\n\r"
    " type with the specified exponent.\n\r",
    "",
    {0,
     0,
     BIT(4)},
    8,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_RED_EXP_WEIGHT_ID,
    "red_exp_weight",
    (PARAM_VAL_RULES *)&Fap20v_red_exp_weight[0],
    (sizeof(Fap20v_red_exp_weight) / sizeof(Fap20v_red_exp_weight[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Weight for average queue size calculation.\n\r"
    "  If not appears disable WRED algorithm to this queue type.\n\r"
    "  The average calculation:\n\r"
    "    Average_constant = 2^(-RED_exp_weight)\n\r"
    "    if (Instantaneous-Queue-size < Average-queue-size)\n\r"
    "      Average-queue-size = Instantaneous-Queue-size\n\r"
    "    else\n\r"
    "      Average-queue-size = \n\r"
    "          (1-Average_constant)*Average-queue-size +\n\r"
    "          Average_constant*Instantaneous-Queue-size\n\r"
    "  Range  0-31, I.e., make the average factor from 1 to 1^(-31).",
    "Examples:\r\n"
    "  fap20v configuration set_queue_type_params 1 queue_type 2 max_queue_size 1200 flow_stat_msg 500 flow_slow_th 300 RED_exp_weight 5 \r\n"
    "Results in setting (on device 1) queue type 2 parameters to:\r\n"
    " Maximum queue size 1200 bytes,\n\r"
    " every time the queue size cross over multiple of 500 the queue \r\n"
    " is to sent status message to its related flow,\n\r"
    " if the queue size is beneath 300 the queue is considered in 'Slow' state,\n\r"
    " RED_exp_weight appear - indication that RED algorithm is applying this queue\n\r"
    " type with the specified exponent.\n\r",
    "",
    {0,
     0,
     BIT(4)},
    9,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_FAP20V_GET_QUEUE_TYPE_PARAMS_ID,
    "get_queue_type_params",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gets the queue parameters of 'queue_type'.",
    "Examples:\r\n"
    "  fap20v configuration get_queue_type_params 1 queue_type 2\r\n"
    "Results in getting (on device 1) queue type 2 parameters.\r\n",
    "",
    {0,
     0,
     BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_QUEUE_TYPE_ID,
    "queue_type",
    (PARAM_VAL_RULES *)&Fap20v_b_queue_types[0],
    (sizeof(Fap20v_b_queue_types) / sizeof(Fap20v_b_queue_types[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Queue type number.",
    "Examples:\r\n"
    "  fap20v configuration get_queue_type_params 1 queue_type 2\r\n"
    "Results in getting (on device 1) queue type 2 parameters.\r\n",
    "",
    {0,
     0,
     BIT(3) | BIT(4) | BIT(5) | BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_FAP20V_GET_FAP_ID_ID,
    "get_fap_id",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get fap identifier (as seen in the system).",
    "Examples:\r\n"
    "  fap20v configuration get_fap_id 1\r\n"
    "Results in getting the (on device 1) its system identifier.\r\n",
    "",
    {0,
     0,
     BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SET_FAP_ID_ID,
    "set_fap_id",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Sets the device with its identifier (as seen in the system).",
    "Examples:\r\n"
    "  fap20v configuration set_fap_id 1 fap_id 123\r\n"
    "Results in setting on device 1, fap identifier of 123.\n\r"
    "This fap identifier is the identifier that the device is seen\n\r"
    "in the fabric system. while the 'unit' is the identifier\n\r"
    "of that fap in the context of the local driver\r\n",
    "",
    {0,
     0,
     BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_FAP_ID_ID,
    "fap_id",
    (PARAM_VAL_RULES *)&Fap20v_fap_id[0],
    (sizeof(Fap20v_fap_id) / sizeof(Fap20v_fap_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The designated physical FAP id.",
    "Examples:\r\n"
    "  fap20v configuration set_fap_id 1 fap_id 123\r\n"
    "Results in setting on device 1, fap identifier of 123.\n\r"
    "This fap identifier is the identifier that the device is seen\n\r"
    "in the fabric system. while the 'unit' is the identifier\n\r"
    "of that fap in the context of the local driver\r\n",
    "",
    {0,
     0,
     BIT(1)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_GET_PROFILE_ID,
    "get_profile",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gets the identifier of current loaded 'profile' \n\r"
    "  (the profile identifier loaded with 'set_profile').",
    "Examples:\r\n"
    "  fap20v configuration get_profile 1\r\n"
    "Results in getting the (on device 1) its loaded profile identifier.\r\n",
    "",
    {0,
     0,
     BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SET_PROFILE_ID,
    "set_profile",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Sets sets device with pre-defined profile.\n\r"
    "  A large set of registers is written to the device.",
    "Examples:\r\n"
    "  fap20v configuration set_profile 1 profile_id 0\r\n"
    "Results in setting the (on device 1) its profile number 1.\r\n",
    "",
    {0,
     0,
     BIT(2)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_PROFILE_ID_ID,
    "profile_id",
    (PARAM_VAL_RULES *)&Fap20v_profile_id[0],
    (sizeof(Fap20v_profile_id) / sizeof(Fap20v_profile_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The profile identifier, which the driver load the device.",
    "Examples:\r\n"
    "  fap20v configuration set_profile_id 1 profile_id 0\r\n"
    "Results in setting the (on device 1) its profile number 1.\r\n",
    "",
    {0,
     0,
     BIT(2)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_GET_PROFILE_VALUES_ID,
    "get_profile_values",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gets the profile values from the device.\n\r"
    "  A large set of register is read from the device.",
    "Examples:\r\n"
    "  fap20v configuration get_profile_values 1\r\n"
    "Results in getting the (on device 1) loaded profile values from the device.\r\n",
    "",
    {0,
     0,
     BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  /* END fap20v_api_configuration.h interface }*/


  /* { fap20v_api_scheduler_flows.h interface */
  {
    PARAM_FAP20V_SCHEDULER_FLOW_API_ID,
    "flow",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver scheduler flow services.",
    "Examples:\r\n"
    "  fap20v scheduler flow open_flow 0 flow_id 16111 sch_port_id 0 source_fap 2047 ingress_queue 8191 low_delay off flow_type regular sub_0_flow_type edf1 sub_0_flow_value 1000\r\n"
    "Resulting in opening (on device 0) flow 16111, \n\r"
    " which to be installed on scheduler port 0,\r\n"
    " send it credits to fap 2047 queue 8191 (on that fap).\n\r"
    " The flow will NOT be 'low jitter'. It will be regular (appose to virtual or aggregate)\n\r"
    " One subflow will be installed on that flow.\n\r",
    "",
    {0,
     BIT(24) | BIT(25) | BIT(26) | BIT(29),
     0,
     BIT(0) | BIT(1) | BIT(19) | BIT(20) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_CLOSE_SCHEDULER_FLOW_ID,
    "close_flow",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get scheduler flow parameters.",
    "Examples:\r\n"
    "  fap20v scheduler flow close_flow 1 flow_id 54\r\n"
    "Results in closing on device 1 scheduler flow 54.\r\n",
    "",
    {0,
     BIT(26)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_GET_SCHEDULER_FLOW_PARAMS_ID,
    "get_flow",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get scheduler flow parameters.",
    "Examples:\r\n"
    "  fap20v scheduler flow get_flow 1 flow_id 54\r\n"
    "Results in getting on device 1 the parameters of scheduler flow 54.\r\n",
    "",
    {0,
     BIT(26)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_UPDATE_SCHEDULER_RECYCLE_FLOW_ID,
    "update_recycle_flow",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Updates recycle scheduler flow, set its parameters and return the\n\r"
    "  exact parameters were loaded to the device after rounding.",
    "Examples:\r\n"
    "  fap20v scheduler flow update_recycle_flow 1 flow_id 122 source_fap 90 ingress_queue 2305 low_delay weight 12\r\n"
    "Resulting in updating (on device 1) flow 122 to be recycle flow, \n\r"
    " which to be installed on scheduler port 64,\r\n"
    " send it credits to fap 90 queue 2035 (on the fap).\n\r"
    " The flow weight on the recycle wheel is 12.\n\r",
    "",
    {0,
     BIT(29)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_OPEN_SCHEDULER_RECYCLE_FLOW_ID,
    "open_recycle_flow",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Opens recycle scheduler flow, set its parameters and return the\n\r"
    "  exact parameters were loaded to the device after rounding.",
    "Examples:\r\n"
    "  fap20v scheduler flow open_recycle_flow 1 flow_id 122 source_fap 90 ingress_queue 2305 low_delay weight 12\r\n"
    "Resulting in opening (on device 1) flow 122 to be recycle flow, \n\r"
    " which to be installed on scheduler port 64,\r\n"
    " send it credits to fap 90 queue 2035 (on the fap).\n\r"
    " The flow weight on the recycle wheel is 12.\n\r",
    "",
    {0,
     BIT(29)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SCHEDULER_RECYCLE_FLOW_WEIGHT_ID,
    "weight",
    (PARAM_VAL_RULES *)&fap20v_scheduler_recycle_flow_weight_vals[0],
    (sizeof(fap20v_scheduler_recycle_flow_weight_vals) / sizeof(fap20v_scheduler_recycle_flow_weight_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "OWeight of that flow on the recycle wheel.",
    "Examples:\r\n"
    "  fap20v scheduler flow open_recycle_flow 1 flow_id 122 source_fap 90 ingress_queue 2305 weight 12 low_delay \r\n"
    "Resulting in opening (on device 1) flow 122 to be recycle flow, \n\r"
    " which to be installed on scheduler port 64,\r\n"
    " send it credits to fap 90 queue 2035 (on the fap).\n\r"
    " The flow weight on the recycle wheel is 12.\n\r",
    "",
    {0,
     BIT(29)},
    10,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_UPDATE_SCHEDULER_FLOW_ID,
    "update_flow",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Updates scheduler flow, set its parameters and return the\n\r"
    "  exact parameters were loaded to the device after rounding.",
    "Examples:\r\n"
    "  fap20v scheduler flow update_flow 1 flow_id 122 sch_port_id 54 source_fap 90 ingress_queue 2305 low_delay  sub_0_flow_type wfq1 sub_0_flow_value 323 sub_1_flow_type edf1 sub_1_flow_value 12  \r\n"
    "Resulting in updating (on device 1) flow 122, \n\r"
    " which to be installed on scheduler port 54,\r\n"
    " send it credits to fap 90 queue 2035 (on tha fap).\n\r"
    " The flow wiil be 'low jitter' flow with 2 subflows.\n\r",
    "",
    {0,
     BIT(24) | BIT(25),
     0,
     BIT(19) | BIT(20)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_OPEN_SCHEDULER_FLOW_ID,
    "open_flow",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Open scheduler flow, set its parameters and return the\n\r"
    "  exact parameters were loaded to the device after rounding.",
    "Examples:\r\n"
    "  fap20v scheduler flow open_flow 0 flow_id 16111 sch_port_id 0 source_fap 2047 ingress_queue 8191 low_delay off flow_type regular sub_0_flow_type edf1 sub_0_flow_value 1000\r\n"
    "Resulting in opening (on device 0) flow 16111, \n\r"
    " which to be installed on scheduler port 0,\r\n"
    " send it credits to fap 2047 queue 8191 (on that fap).\n\r"
    " The flow will NOT be 'low jitter'. It will be regular (appose to virtual or aggregate)\n\r"
    " One subflow will be installed on that flow.\n\r",
    "",
    {0,
     BIT(24) | BIT(25),
     0,
     BIT(19) | BIT(20) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_FLOW_ID_ID,
    "flow_id",
    (PARAM_VAL_RULES *)&Fap20v_flow_id[0],
    (sizeof(Fap20v_flow_id) / sizeof(Fap20v_flow_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Flow, in the scheduler.",
    "Examples:\r\n"
    "  fap20v scheduler flow open_flow 0 flow_id 16111 sch_port_id 0 source_fap 2047 ingress_queue 8191 low_delay off flow_type regular sub_0_flow_type edf1 sub_0_flow_value 1000\r\n"
    "Resulting in opening (on device 0) flow 16111, \n\r"
    " which to be installed on scheduler port 0,\r\n"
    " send it credits to fap 2047 queue 8191 (on that fap).\n\r"
    " The flow will NOT be 'low jitter'. It will be regular (appose to virtual or aggregate)\n\r"
    " One subflow will be installed on that flow.\n\r",
    "",
    {0,
     BIT(24) | BIT(25) | BIT(26) | BIT(29),
     0,
     BIT(0) | BIT(1) | BIT(19) | BIT(20) },
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SCHEDULER_PORT_ID_ID,
    "sch_port_id",
    (PARAM_VAL_RULES *)&Fap20v_scheduler_port_id_no_recyle_vals[0],
    (sizeof(Fap20v_scheduler_port_id_no_recyle_vals) / sizeof(Fap20v_scheduler_port_id_no_recyle_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Scheduler port, the will is belonged to.",
    "Examples:\r\n"
    "  fap20v scheduler flow open_flow 0 flow_id 16111 sch_port_id 0 source_fap 2047 ingress_queue 8191 low_delay off flow_type regular sub_0_flow_type edf1 sub_0_flow_value 1000\r\n"
    "Resulting in opening (on device 0) flow 16111, \n\r"
    " which to be installed on scheduler port 0,\r\n"
    " send it credits to fap 2047 queue 8191 (on that fap).\n\r"
    " The flow will NOT be 'low jitter'. It will be regular (appose to virtual or aggregate)\n\r"
    " One subflow will be installed on that flow.\n\r",
    "",
    {0,
     BIT(24) | BIT(25),
     0,
     BIT(19) | BIT(20) },
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SOURCE_FAP_ID_ID,
    "source_fap",
    (PARAM_VAL_RULES *)&Fap20v_fap_id[0],
    (sizeof(Fap20v_fap_id) / sizeof(Fap20v_fap_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "FAP identifier, this flow is supposed to sent credits to.",
    "Examples:\r\n"
    "  fap20v scheduler flow open_flow 0 flow_id 16111 sch_port_id 0 source_fap 2047 ingress_queue 8191 low_delay off flow_type regular sub_0_flow_type edf1 sub_0_flow_value 1000\r\n"
    "Resulting in opening (on device 0) flow 16111, \n\r"
    " which to be installed on scheduler port 0,\r\n"
    " send it credits to fap 2047 queue 8191 (on that fap).\n\r"
    " The flow will NOT be 'low jitter'. It will be regular (appose to virtual or aggregate)\n\r"
    " One subflow will be installed on that flow.\n\r",
    "",
    {0,
     BIT(24) | BIT(25) | BIT(29),
     0,
     BIT(19) | BIT(20) },
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_INGRESS_QUEUE_ID_ID,
    "ingress_queue",
    (PARAM_VAL_RULES *)&Fap20v_queue_id[0],
    (sizeof(Fap20v_queue_id) / sizeof(Fap20v_queue_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Ingress queue identifier, this flow is supposed to sent credits to.",
    "Examples:\r\n"
    "  fap20v scheduler flow open_flow 0 flow_id 16111 sch_port_id 0 source_fap 2047 ingress_queue 8191 low_delay off flow_type regular sub_0_flow_type edf1 sub_0_flow_value 1000\r\n"
    "Resulting in opening (on device 0) flow 16111, \n\r"
    " which to be installed on scheduler port 0,\r\n"
    " send it credits to fap 2047 queue 8191 (on that fap).\n\r"
    " The flow will NOT be 'low jitter'. It will be regular (appose to virtual or aggregate)\n\r"
    " One subflow will be installed on that flow.\n\r",
    "",
    {0,
     BIT(24) | BIT(25) | BIT(29),
     0,
     BIT(19) | BIT(20) },
    7,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_LOW_JITTER_ENABLE_ID,
    "low_delay",
    (PARAM_VAL_RULES *)&Fap20v_on_off_vals[0],
    (sizeof(Fap20v_on_off_vals) / sizeof(Fap20v_on_off_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "If appears, indicates that this flow will sent its credit\r\n"
    " regardless if its ingress queue has data.",
    "  fap20v scheduler flow open_flow 0 flow_id 16111 sch_port_id 0 source_fap 2047 ingress_queue 8191 low_delay off flow_type regular sub_0_flow_type edf1 sub_0_flow_value 1000\r\n"
    "Resulting in opening (on device 0) flow 16111, \n\r"
    " which to be installed on scheduler port 0,\r\n"
    " send it credits to fap 2047 queue 8191 (on that fap).\n\r"
    " The flow will NOT be 'low jitter'. It will be regular (appose to virtual or aggregate)\n\r"
    " One subflow will be installed on that flow.\n\r",
    "",
    {0,
     BIT(24) | BIT(25) | BIT(29),
     BIT(13) | BIT(14),
     BIT(19) | BIT(20) },
     8,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SCH_FLOW_TYPE_ID,
    "flow_type",
    (PARAM_VAL_RULES *)&Fap20v_sch_flow_type_vals[0],
    (sizeof(Fap20v_sch_flow_type_vals) / sizeof(Fap20v_sch_flow_type_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "If appears, indicates the scheduler flow type (regular, virtual, aggregate).",
    "  fap20v scheduler flow open_flow 1 flow_id 122 sch_port_id 54 source_fap 90 ingress_queue 2305 low_delay  sub_0_flow_type wfq1 sub_0_flow_value 323 sub_1_flow_type edf1 sub_1_flow_value 12  \r\n"
    "Resulting in opening (on device 1) flow 122, \n\r"
    " which to be installed on scheduler port 54,\r\n"
    " send it credits to fap 90 queue 2035 (on tha fap).\n\r"
    " The flow wiil be 'low jitter' flow with 2 subflows.\n\r",
    "",
    {0,
     BIT(24) | BIT(25) | BIT(29),
     BIT(13) | BIT(14),
     BIT(19) | BIT(20) },
     9,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SUB_FLOW_0_TYPE_ID,
    "sub_0_flow_type",
    (PARAM_VAL_RULES *)&Fap20v_sub_flow_types[0],
    (sizeof(Fap20v_sub_flow_types) / sizeof(Fap20v_sub_flow_types[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Ingress queue identifier, this flow is supposed to sent credits to.",
    "Examples:\r\n"
    "  fap20v scheduler flow open_flow 0 flow_id 16111 sch_port_id 0 source_fap 2047 ingress_queue 8191 low_delay off flow_type regular sub_0_flow_type edf1 sub_0_flow_value 1000\r\n"
    "Resulting in opening (on device 0) flow 16111, \n\r"
    " which to be installed on scheduler port 0,\r\n"
    " send it credits to fap 2047 queue 8191 (on that fap).\n\r"
    " The flow will NOT be 'low jitter'. It will be regular (appose to virtual or aggregate)\n\r"
    " One subflow will be installed on that flow.\n\r",
    "",
    {0,
     BIT(24) | BIT(25),
     BIT(13) | BIT(14),
     BIT(0) | BIT(1) | BIT(19) | BIT(20) },
    10,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SUB_FLOW_0_VALUE_ID,
    "sub_0_flow_value",
    (PARAM_VAL_RULES *)&Fap20v_sub_flow_values[0],
    (sizeof(Fap20v_sub_flow_values) / sizeof(Fap20v_sub_flow_values[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Ingress queue identifier, this flow is supposed to sent credits to.\n\r"
    " 'value' semantics depends on the 'type' value:\n\r"
    "\n\r"
    " FAP20V_SUB_FLOW_TYPE    | 'value' semantics\n\r"
    "----------------------------------------------\n\r"
    " FAP20V_EDF0_FLOW        | Kilo-bits - guaranteed rate\n\r"
    " FAP20V_EDF1_FLOW        | Kilo-bits -Maximal rate\n\r"
    " FAP20V_WFQ1_FLOW        | Weight. Range from 1 to 126*8.\n\r"
    "",
    "Examples:\r\n"
    "  fap20v scheduler flow open_flow 0 flow_id 16111 sch_port_id 0 source_fap 2047 ingress_queue 8191 low_delay off flow_type regular sub_0_flow_type edf1 sub_0_flow_value 1000\r\n"
    "Resulting in opening (on device 0) flow 16111, \n\r"
    " which to be installed on scheduler port 0,\r\n"
    " send it credits to fap 2047 queue 8191 (on that fap).\n\r"
    " The flow will NOT be 'low jitter'. It will be regular (appose to virtual or aggregate)\n\r"
    " One subflow will be installed on that flow.\n\r",
    "",
    {0,
     BIT(24) | BIT(25),
     BIT(13) | BIT(14),
     BIT(0) | BIT(1) | BIT(19) | BIT(20) },
    11,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SUB_FLOW_1_TYPE_ID,
    "sub_1_flow_type",
    (PARAM_VAL_RULES *)&Fap20v_sub_flow_types[0],
    (sizeof(Fap20v_sub_flow_types) / sizeof(Fap20v_sub_flow_types[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Ingress queue identifier, this flow is supposed to sent credits to.",
    "Examples:\r\n"
    "  fap20v scheduler flow open_flow 0 flow_id 16111 sch_port_id 0 source_fap 2047 ingress_queue 8191 low_delay off flow_type regular sub_0_flow_type edf1 sub_0_flow_value 1000 sub_1_flow_type edf0 sub_1_flow_value 9500\r\n"
    "Resulting in opening (on device 0) flow 16111, \n\r"
    " which to be installed on scheduler port 0,\r\n"
    " send it credits to fap 2047 queue 8191 (on that fap).\n\r"
    " The flow will NOT be 'low jitter'. It will be regular (appose to virtual or aggregate)\n\r"
    " 2 subflows will be installed on that flow.\n\r",
    "",
    {0,
     BIT(25),
     BIT(13),
     BIT(1) | BIT(19) | BIT(20)
    },
    12,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SUB_FLOW_1_VALUE_ID,
    "sub_1_flow_value",
    (PARAM_VAL_RULES *)&Fap20v_sub_flow_values[0],
    (sizeof(Fap20v_sub_flow_values) / sizeof(Fap20v_sub_flow_values[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Ingress queue identifier, this flow is supposed to sent credits to.",
    "Examples:\r\n"
    "  fap20v scheduler flow open_flow 0 flow_id 16111 sch_port_id 0 source_fap 2047 ingress_queue 8191 low_delay off flow_type regular sub_0_flow_type edf1 sub_0_flow_value 1000 sub_1_flow_type edf0 sub_1_flow_value 9500\r\n"
    "Resulting in opening (on device 0) flow 16111, \n\r"
    " which to be installed on scheduler port 0,\r\n"
    " send it credits to fap 2047 queue 8191 (on that fap).\n\r"
    " The flow will NOT be 'low jitter'. It will be regular (appose to virtual or aggregate)\n\r"
    " 2 subflows will be installed on that flow.\n\r",
    "",
    {0,
     BIT(25) ,
     BIT(13),
     BIT(1) | BIT(19) | BIT(20)
    },
    13,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SUB_FLOW_2_TYPE_ID,
    "sub_2_flow_type",
    (PARAM_VAL_RULES *)&Fap20v_sub_flow_types[0],
    (sizeof(Fap20v_sub_flow_types) / sizeof(Fap20v_sub_flow_types[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Ingress queue identifier, this flow is supposed to sent credits to.",
    "Examples:\r\n"
    "  fap20v scheduler flow open_flow 0 flow_id 16111 sch_port_id 0 source_fap 2047 ingress_queue 8191 low_delay off flow_type regular sub_0_flow_type edf1 sub_0_flow_value 1000 sub_1_flow_type edf0 sub_1_flow_value 9500 sub_2_flow_type wfq1 sub_2_flow_value 54\r\n"
    "Resulting in opening (on device 0) flow 16111, \n\r"
    " which to be installed on scheduler port 0,\r\n"
    " send it credits to fap 2047 queue 8191 (on that fap).\n\r"
    " The flow will NOT be 'low jitter'. It will be regular (appose to virtual or aggregate)\n\r"
    " Three subflows will be installed on that flow.\n\r",
    "",
    {0,
     0,
     0,
     BIT(1) | BIT(19) | BIT(20)
    },
    14,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SUB_FLOW_2_VALUE_ID,
    "sub_2_flow_value",
    (PARAM_VAL_RULES *)&Fap20v_sub_flow_values[0],
    (sizeof(Fap20v_sub_flow_values) / sizeof(Fap20v_sub_flow_values[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Ingress queue identifier, this flow is supposed to sent credits to.",
    "Examples:\r\n"
    "  fap20v scheduler flow open_flow 0 flow_id 16111 sch_port_id 0 source_fap 2047 ingress_queue 8191 low_delay off flow_type regular sub_0_flow_type edf1 sub_0_flow_value 1000 sub_1_flow_type edf0 sub_1_flow_value 9500 sub_2_flow_type wfq1 sub_2_flow_value 54\r\n"
    "Resulting in opening (on device 0) flow 16111, \n\r"
    " which to be installed on scheduler port 0,\r\n"
    " send it credits to fap 2047 queue 8191 (on that fap).\n\r"
    " The flow will NOT be 'low jitter'. It will be regular (appose to virtual or aggregate)\n\r"
    " Three subflows will be installed on that flow.\n\r",
    "",
    {0,
     0,
     0,
     BIT(1) | BIT(19) | BIT(20)
    },
    15,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SUB_FLOW_3_TYPE_ID,
    "sub_3_flow_type",
    (PARAM_VAL_RULES *)&Fap20v_sub_flow_types[0],
    (sizeof(Fap20v_sub_flow_types) / sizeof(Fap20v_sub_flow_types[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Ingress queue identifier, this flow is supposed to sent credits to.",
    "Examples:\r\n"
    "  fap20v scheduler flow open_flow 0 flow_id 16111 sch_port_id 0 source_fap 2047 ingress_queue 8191 low_delay off flow_type regular sub_0_flow_type edf1 sub_0_flow_value 1000 sub_1_flow_type edf0 sub_1_flow_value 9500 sub_2_flow_type wfq1 sub_2_flow_value 54 sub_3_flow_type edf0 sub_3_flow_value 54\r\n"
    "Resulting in opening (on device 0) flow 16111, \n\r"
    " which to be installed on scheduler port 0,\r\n"
    " send it credits to fap 2047 queue 8191 (on that fap).\n\r"
    " The flow will NOT be 'low jitter'. It will be regular (appose to virtual or aggregate)\n\r"
    " Four subflows will be installed on that flow.\n\r",
    "",
    {0,
     BIT(31),
     0 ,
     BIT(1) | BIT(20)
    },
    16,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SUB_FLOW_3_VALUE_ID,
    "sub_3_flow_value",
    (PARAM_VAL_RULES *)&Fap20v_sub_flow_values[0],
    (sizeof(Fap20v_sub_flow_values) / sizeof(Fap20v_sub_flow_values[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Ingress queue identifier, this flow is supposed to sent credits to.",
    "  fap20v scheduler flow open_flow 0 flow_id 16111 sch_port_id 0 source_fap 2047 ingress_queue 8191 low_delay off flow_type regular sub_0_flow_type edf1 sub_0_flow_value 1000 sub_1_flow_type edf0 sub_1_flow_value 9500 sub_2_flow_type wfq1 sub_2_flow_value 54 sub_3_flow_type edf0 sub_3_flow_value 54\r\n"
    "Resulting in opening (on device 0) flow 16111, \n\r"
    " which to be installed on scheduler port 0,\r\n"
    " send it credits to fap 2047 queue 8191 (on that fap).\n\r"
    " The flow will NOT be 'low jitter'. It will be regular (appose to virtual or aggregate)\n\r"
    " Four subflows will be installed on that flow.\n\r",
    "",
    {0,
     BIT(31),
     0 ,
     BIT(1) | BIT(20)
    },
    17,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* END fap20v_api_scheduler_flows.h interface }*/
  /* { fap20v_api_packet.h interface */
  {
    PARAM_FAP20V_RECV_PACKET_ID,
    "recv_packet",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This procedure receives packet.\r\n",
    "Examples:\r\n"
    "  fap20v traffic recv_packet 0 \r\n"
    "Results in checking if there is packet in the device.\n\r"
    "Case there is, read and print it.\r\n",
    "",
    {0,
     BIT(22) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_DEFRRED_RECV_PACKET_ID,
    "recv_packet",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "deferred receive (polling) packet.\r\n",
    "Examples:\r\n"
    "  fap20v deferred recv_packet 0x1 polling_rate 160\r\n"
    "Results in deferred reading a packets (were designated to the CPU)\n\r"
    " from device 0x1.\r\n",
    "",
    {BIT(9) | BIT(10)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SEND_PACKET_ID,
    "send_packet",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This procedure transmits one packet.\r\n",
    "Examples:\r\n"
    "  fap20v traffic send_packet 0 target_fap 120 data 0x12 0x775487 0x76767 \r\n"
    "Results in sending to FAP with id of 120, from device 0, one packet\r\n"
    "  with 12 bytes with the above data.\r\n",
    "",
    {0,
     BIT(23) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TARGET_FAP_ID_ID,
    "target_fap",
    (PARAM_VAL_RULES *)&Fap20v_fap_id[0],
    (sizeof(Fap20v_fap_id) / sizeof(Fap20v_fap_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "One of FAPs identifier in the system.\r\n",
    "Examples:\r\n"
    "  fap20v traffic send_packet 0 target_fap 120 data 0x12 0x775487 0x76767 \r\n"
    "Results in sending to FAP with id of 120, from device 0, one packet\r\n"
    "  with 12 bytes with the above data.\r\n",
    "",
    {0,
     BIT(23) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_PACKET_DATA_ID,
    "data",
    (PARAM_VAL_RULES *)&Fap20v_packet_data[0],
    (sizeof(Fap20v_packet_data) / sizeof(Fap20v_packet_data[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Data to carry on packet payload. In case that data length is less \r\n"
    "then 64 bytes (16 long) it will be completed to 16 long with zero padding.\r\n",
    "Examples:\r\n"
    "  fap20v traffic send_packet 0 target_fap 120 data 0x12 0x775487 0x76767 \r\n"
    "Results in sending to FAP with id of 120, from device 0, one packet\r\n"
    "  with 12 bytes with the above data and 52 bytes of zero.\r\n",
    "",
    {0,
     BIT(23) },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SEND_PACKET_NOF_PACKETS_ID,
    "nof_packets",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Number of packets to be sent.\r\n",
    "Examples:\r\n"
    "  fap20v traffic send_packet 0 target_fap 120 nof_packets 3 data 0x12 0x775487 0x76767\r\n"
    "Results in sending 3 packets to FAP with id of 120, from device 0, one packet\r\n"
    "  with 12 bytes with the above data.\r\n",
    "",
    {0,
     BIT(23) },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* END fap20v_api_packet.h interface }*/
  /* { fap20v_api_recycle_multicast.h interface */
  {
    PARAM_FAP20V_RECYCLE_MULTICAST_API_ID,
    "recycle",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver recycle multicast module services.",
    "Examples:\r\n"
    "  fap20v multicast recycle open_internal_node 0 tree_id 12000 next_tree_id 900 recycle_q_index_0 100 recycle_q_index_1 29 \r\n"
    "Results in setting on device 0 for tree id 12000 to be:\n\r"
    "Internal node, which point to next tree id 900 and to recycling queue indices of 100 and 29.\r\n",
    "",
    {0,
     BIT(17) | BIT(18) | BIT(19) | BIT(20) | BIT(21) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_GET_RECYCLING_QUEUE_ID,
    "get_recycling_queue",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This procedure gets recycling queue parameters.\r\n",
    "Examples:\r\n"
    "  fap20v multicast recycle get_recycling_queue 0 recycle_queue_index 12\r\n"
    "Results in get and display recycle queue index 12.\r\n",
    "",
    {0,
     BIT(21) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_CLOSE_RECYCLING_QUEUE_ID,
    "close_recycling_queue",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This procedure deletes a recycling queue by setting the FAP - \n\r"
    "  'recycle_queue_index' to be invalid.\r\n",
    "Examples:\r\n"
    "  fap20v multicast recycle close_recycling_queue 0 recycle_queue_index 12\r\n"
    "Results in recycle queue index 12 to be invalid.\r\n",
    "",
    {0,
     BIT(21) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_OPEN_RECYCLING_QUEUE_ID,
    "open_recycling_queue",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This procedure adds a recycling queue by setting the\r\n"
    "  'recycle_queue_index' to point to a the new 'queue_id'.\r\n",
    "Examples:\r\n"
    "  fap20v multicast recycle open_recycling_queue 0 recycle_queue_index 127 queue_id 8191\r\n"
    "Results in recycle queue index 127 points to queue 8191 on device 0.\r\n",
    "",
    {0,
     BIT(20) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_RECYCLE_Q_INDEX_ID,
    "recycle_queue_index",
    (PARAM_VAL_RULES *)&Fap20v_recycle_queue_index[0],
    (sizeof(Fap20v_recycle_queue_index) / sizeof(Fap20v_recycle_queue_index[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Index in the Recycling-Queue table.\r\n",
    "Examples:\r\n"
    "  fap20v multicast recycle open_recycling_queue 0 recycle_queue_index 127 queue_id 8191\r\n"
    "Results in recycle queue index 127 points to queue 8191 on device 0.\r\n",
    "",
    {0,
     BIT(20) | BIT(21)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_QUEUE_ID_ID,
    "queue_id",
    (PARAM_VAL_RULES *)&Fap20v_queue_id[0],
    (sizeof(Fap20v_queue_id) / sizeof(Fap20v_queue_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Queue identifier.\r\n",
    "Examples:\r\n"
    "  fap20v multicast recycle open_recycling_queue 0 recycle_queue_index 127 queue_id 8191\r\n"
    "Results in recycle queue index 127 points to queue 8191 on device 0.\r\n",
    "",
    {0,
     BIT(20) },
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_CLOSE_RECYCLE_NODE_ID,
    "close_node",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This procedure deletes one node form the recycle tree.\r\n",
    "Examples:\r\n"
    "  fap20v multicast recycle close_node 0 tree_id 1221 \r\n"
    "Results in closing on device 0 for tree id 1221.\r\n",
    "",
    {0,
     BIT(19) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_GET_RECYCLE_NODE_ID,
    "get_node",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This procedure get and display one node form the recycle tree.\r\n",
    "Examples:\r\n"
    "  fap20v multicast recycle get_node 0 tree_id 1221 \r\n"
    "Results in getting and displaying on device 0 node on tree id 1221.\r\n",
    "",
    {0,
     BIT(19) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_OPEN_RECYCLE_LAST_NODE_ID,
    "open_last_node",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This procedure adds one last node to the recycle tree.\r\n",
    "Examples:\r\n"
    "  fap20v multicast recycle open_last_node 0 tree_id 1221 queues 125 8191 \r\n"
    "Results in setting on device 0 for tree id 1221 to be:\n\r"
    "Last node, which point to 2 queues: 125 and 8191.\r\n",
    "",
    {0,
     BIT(18) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_QUEUES_ID_ID,
    "queues",
    (PARAM_VAL_RULES *)&Fap20v_recycle_multicast_queues[0],
    (sizeof(Fap20v_recycle_multicast_queues) / sizeof(Fap20v_recycle_multicast_queues[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "1 or 2 queues identifiers.\r\n",
    "Examples:\r\n"
    "  fap20v multicast recycle open_last_node 0 tree_id 1221 queues 125 8191 \r\n"
    "Results in setting on device 0 for tree id 1221 to be:\n\r"
    "Last node, which point to 2 queues: 125 and 8191.\r\n",
    "",
    {0,
     BIT(18) },
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_OPEN_RECYCLE_INTERNAL_NODE_ID,
    "open_internal_node",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "This procedure adds one last node to the recycle tree.\r\n",
    "Examples:\r\n"
    "  fap20v multicast recycle open_internal_node 0 tree_id 12000 next_tree_id 900 recycle_q_index_0 100 recycle_q_index_1 29 \r\n"
    "Results in setting on device 0 for tree id 12000 to be:\n\r"
    "Internal node, which point to nect tree id 900 and to recycling queue indices of 100 and 29.\r\n",
    "",
    {0,
     BIT(17) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TREE_ID_ID,
    "tree_id",
    (PARAM_VAL_RULES *)&Fap20v_recycle_multicast_tree_id[0],
    (sizeof(Fap20v_recycle_multicast_tree_id) / sizeof(Fap20v_recycle_multicast_tree_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Tree id in recycle tree.\r\n",
    "Examples:\r\n"
    "  fap20v multicast recycle open_internal_node 0 tree_id 12000 next_tree_id 900 recycle_q_index_0 100 recycle_q_index_1 29 \r\n"
    "Results in setting on device 0 for tree id 12000 to be:\n\r"
    "Internal node, which point to nect tree id 900 and to recycling queue indices of 100 and 29.\r\n",
    "",
    {0,
     BIT(17) | BIT(18) | BIT(19)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_NEXT_TREE_ID_ID,
    "next_tree_id",
    (PARAM_VAL_RULES *)&Fap20v_recycle_multicast_tree_id[0],
    (sizeof(Fap20v_recycle_multicast_tree_id) / sizeof(Fap20v_recycle_multicast_tree_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Next level tree id.\r\n",
    "Examples:\r\n"
    "  fap20v multicast recycle open_internal_node 0 tree_id 12000 next_tree_id 900 recycle_q_indices 100 29 \r\n"
    "Results in setting on device 0 for tree id 12000 to be:\n\r"
    "Internal node, which point to next tree id 900 and to recycling queue indices of 100 and 29.\r\n",
    "",
    {0,
     BIT(17) },
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_RECYCLE_Q_INDEX_0_ID,
    "recycle_q_index_0",
    (PARAM_VAL_RULES *)&Fap20v_recycle_queue_index[0],
    (sizeof(Fap20v_recycle_queue_index) / sizeof(Fap20v_recycle_queue_index[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "index to \'Multicast Recycling-Queue\' table.\r\n",
    "Examples:\r\n"
    "  fap20v multicast recycle open_internal_node 0 tree_id 12000 next_tree_id 900 recycle_q_index_0 100 recycle_q_index_1 29 \r\n"
    "Results in setting on device 0 for tree id 12000 to be:\n\r"
    "Internal node, which point to next tree id 900 and to recycling queue indices of 100 and 29.\r\n",
    "",
    {0,
     BIT(17) },
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_RECYCLE_Q_INDEX_1_ID,
    "recycle_q_index_1",
    (PARAM_VAL_RULES *)&Fap20v_recycle_queue_index[0],
    (sizeof(Fap20v_recycle_queue_index) / sizeof(Fap20v_recycle_queue_index[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "index to \'Multicast Recycling-Queue\' table.\r\n",
    "Examples:\r\n"
    "  fap20v multicast recycle open_internal_node 0 tree_id 12000 next_tree_id 900 recycle_q_index_0 100 recycle_q_index_1 29 \r\n"
    "Results in setting on device 0 for tree id 12000 to be:\n\r"
    "Internal node, which point to next tree id 900 and to recycling queue indices of 100 and 29.\r\n",
    "",
    {0,
     BIT(17) },
    7,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* END fap20v_api_recycle_multicast.h interface }*/

  /*******************/
  /* { EGQ interface */
  /*******************/
  /*******************/
  /* } EGQ interface */
  /*******************/
  /* { fap20v_api_ingress_queue.h interface */
  {
    PARAM_FAP20V_INGRESS_QUEUE_API_ID,
    "in_queue",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver ingress queue module services.",
    "Examples:\r\n"
    "  fap20v in_queue set_queue_labeling 0 target_fap 10 base_queue 1001 label_type destination_mapping \r\n"
    "Results in setting on device 0 for packet destained to fap 10,\n\r"
    "the base queue 1001, with destination_mapping labeling\r\n",
    "",
    {0,
     BIT(13) | BIT(14) | BIT(15) | BIT(16),
     BIT(25) | BIT(27) | BIT(28),
     0,
     0,
     0,
     BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5) | BIT(6)
    },
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /*
  * Queue priority {
  */
  {
    PARAM_FAP20V_B_IN_Q_Q_PRIO_SET_ID,
    "queues_priority_set",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set ingress queues priority.\r\n"
    "The queues are divided to blocks. Each block contain 64 queues.\r\n",
    "Examples:\r\n"
    "  fap20v in_queue queues_priority_set 0 queue_id 65 up_to_queue_id 128 priority high\r\n"
    "Results in setting queues 65-128 to be high priority.\r\n",
    "",
    {0,0,0,0,0,0,
     BIT(1)
    },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_IN_Q_Q_PRIO_GET_ID,
    "queues_priority_get",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get ingress queues priority.\r\n"
    "The queues are divided to blocks. Each block contain 64 queues.\r\n",
    "Examples:\r\n"
    "  fap20v in_queue queues_priority_set 0 queue_id 65 up_to_queue_id 128 priority high\r\n"
    "Results in setting queues 65-128 to be high priority.\r\n",
    "",
    {0,0,0,0,0,0,
     BIT(2)
    },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_IN_Q_Q_ID_ID,
    "queue_id",
    (PARAM_VAL_RULES *)&Fap20v_queue_id[0],
    (sizeof(Fap20v_queue_id) / sizeof(Fap20v_queue_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The first queue ID.\r\n"
    "The queues are divided to blocks. Each block contain 64 queues.\r\n",
    "Examples:\r\n"
    "  fap20v in_queue queues_priority_set 0 queue_id 65 up_to_queue_id 128 priority high\r\n"
    "Results in setting queues 65-128 to be high priority.\r\n",
    "",
    {0,0,0,0,0,0,
     BIT(1)
    },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_IN_Q_Q_END_ID_ID,
    "up_to_queue_id",
    (PARAM_VAL_RULES *)&Fap20v_queue_id[0],
    (sizeof(Fap20v_queue_id) / sizeof(Fap20v_queue_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The last queue ID.\r\n"
    "The queues are divided to blocks. Each block contain 64 queues.\r\n",
    "Examples:\r\n"
    "  fap20v in_queue queues_priority_set 0 queue_id 65 up_to_queue_id 128 priority high\r\n"
    "Results in setting queues 65-128 to be high priority.\r\n",
    "",
    {0,0,0,0,0,0,
     BIT(1)
    },
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_IN_Q_Q_PRIO_ID,
    "priority",
    (PARAM_VAL_RULES *)&Fap20v_b_queue_prio[0],
    (sizeof(Fap20v_b_queue_prio) / sizeof(Fap20v_b_queue_prio[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The queue priorirt high or low.\r\n"
    "The queues are divided to blocks. Each block contain 64 queues.\r\n",
    "Examples:\r\n"
    "  fap20v in_queue queues_priority_set 0 queue_id 65 up_to_queue_id 128 priority high\r\n"
    "Results in setting queues 65-128 to be high priority.\r\n",
    "",
    {0,0,0,0,0,0,
     BIT(1)
    },
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /*
  * Queue priority }
  */
  /*
  * Back-off {
  */
    {
      PARAM_FAP20V_B_BACK_OFF_SET_ID,
      "back_off_set",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Back off information. \n\r"
      "When the queue is in back-off mode, the queue will not ask for \n\r"
      "credits.\n\r ",
      "For example\r\n"
      "fap20v back_off_set 0 high_th 6000 low_th 4000.\n\r"
      "  Result in setting the back of to be activated when the credit\n\r"
      "  Account is higher then 6000Byte, and deactivated when the \n\r"
      "  the credit account is lower than 4000 Bytes.\n\r",
      "",
      {0,0,0,0,0,0, BIT(5) },
      2,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_BACK_OFF_GET_ID,
      "back_off_get",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Back off information. \n\r"
      "When the queue is in back-off mode, the queue will not ask for \n\r"
      "credits.\n\r ",
      "For example\r\n"
      "fap20v back_off_set 0 high_th 6000 low_th 4000.\n\r"
      "  Result in setting the back of to be activated when the credit\n\r"
      "  Account is higher then 6000Byte, and deactivated when the \n\r"
      "  the credit account is lower than 4000 Bytes.\n\r",
      "",
      {0,0,0,0,0,0, BIT(6) },
      2,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_BACK_OFF_UP_TH_ID,
      "high_th",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Credit account in bytes to enable back-off. \n\r"
      "When the queue is in back-off mode, the queue will not ask for \n\r"
      "credits.\n\r ",
      "For example\r\n"
      "fap20v back_off_set 0 high_th 6000 low_th 4000.\n\r"
      "  Result in setting the back of to be activated when the credit\n\r"
      "  Account is higher then 6000Byte, and deactivated when the \n\r"
      "  the credit account is lower than 4000 Bytes.\n\r",
      "",
      {0,0,0,0,0,0, BIT(5) },
      3,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_BACK_OFF_DOWN_TH_ID,
      "low_th",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Credit account in bytes to disable back-off. \n\r"
      "When the queue is in back-off mode, the queue will not ask for \n\r"
      "credits.\n\r ",
      "For example\r\n"
      "fap20v back_off_set 0 high_th 6000 low_th 4000.\n\r"
      "  Result in setting the back of to be activated when the credit\n\r"
      "  Account is higher then 6000Byte, and deactivated when the \n\r"
      "  the credit account is lower than 4000 Bytes.\n\r",
      "",
      {0,0,0,0,0,0, BIT(5) },
      4,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
  /*
  * Back-off }
  */

  /*
  * Scrubber {
  */
    {
      PARAM_FAP20V_B_SCRUBBER_SET_ID,
      "scrubber_set",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Set scrubber information. \n\r",
      "For example\r\n"
      "fap20v scrubber_set 0 first_queue 55 last_queue 200 rate 20.\n\r",
      "  Result in setting the scruuber to work at queues 55 to 200\n\r"
      "  in a rate of 20 KHZ.",
      {0,0,0,0,0,0, BIT(3) },
      2,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCRUBBER_GET_ID,
      "scrubber_get",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Get scrubber information. \n\r",
      "For example\r\n"
      "fap20v scrubber_set 0 first_queue 55 last_queue 200 rate 20.\n\r",
      "  Result in setting the scruuber to work at queues 55 to 200\n\r"
      "  in a rate of 20 KHZ.",
      {0,0,0,0,0,0, BIT(4) },
      2,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCRUBBER_FIRST_Q_ID,
      "first_queue",
      (PARAM_VAL_RULES *)&Fap20v_queue_id[0],
      (sizeof(Fap20v_queue_id) / sizeof(Fap20v_queue_id[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "The first queue in the scrubber. \n\r",
      "For example\r\n"
      "fap20v scrubber_set 0 first_queue 55 last_queue 200 rate 20.\n\r",
      "  Result in setting the scruuber to work at queues 55 to 200\n\r"
      "  in a rate of 20 KHZ.",
      {0,0,0,0,0,0, BIT(3) },
      3,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCRUBBER_LAST_Q_ID,
      "last_queue",
      (PARAM_VAL_RULES *)&Fap20v_queue_id[0],
      (sizeof(Fap20v_queue_id) / sizeof(Fap20v_queue_id[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "The last queue in the scrubber. \n\r",
      "For example\r\n"
      "fap20v scrubber_set 0 first_queue 55 last_queue 200 rate 20.\n\r",
      "  Result in setting the scruuber to work at queues 55 to 200\n\r"
      "  in a rate of 20 KHZ.",
      {0,0,0,0,0,0, BIT(3) },
      4,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCRUBBER_RATE_ID,
      "rate",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "The frequency for the scrubber to be activated. \n\r",
      "Scrubber is disabled when set to 0\n\r"
      "The scrubber scan rate in KHZ.    \n\r"
      "If the scan rate is 10 KHZ,               \n\r"
      "there are 5 queues in the scrubber,                    \n\r"
      "the scrubber will scan a queue 10K times in a second,  \n\r"
      "and since there are only 5 queues, each queue would be \n\r"
      "scanned 2K times in a second.                          \n\r"
      "For example\r\n"
      "fap20v scrubber_set 0 first_queue 55 last_queue 200 rate 20.\n\r"
      "  Result in setting the scruuber to work at queues 55 to 200\n\r"
      "  in a rate of 20 KHZ.",
      "",
      {0,0,0,0,0,0, BIT(3) },
      5,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
  /*
  * Scrubber }
  */
   {
    PARAM_FAP20V_GET_UNICAST_DATA_QUEUES_ID,
    "get_unicast_data_queues",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Gets 8 consecutive queues. These ingress queues belong to Unicast-Data packets.\r\n",
    "Examples:\r\n"
    "  fap20v in_queue get_unicast_data_queues 0 destination_id 23\r\n"
    "Results in getting on device 0 destination 11 8 queue-parameters.\r\n",
    "",
    {0,
     0,
     BIT(25),
     0 },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_GET_QUEUE_PARAMS_ID,
    "get_queue_params",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get from the device the ingress FAP queue configured parameters\r\n",
    "Examples:\r\n"
    "  fap20v in_queue get_queue_params 0 queue_id 11\r\n"
    "Results in getting on device 0 queue 11 parameters.\r\n",
    "",
    {0,
     BIT(16) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_GET_QUEUE_SIZE_ID,
    "get_queue_size",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get from the device the ingress FAP queue byte size\r\n",
    "Examples:\r\n"
    "  fap20v in_queue get_queue_size 0 queue_id 11\r\n"
    "Results in getting on device 0 queue 11 byte size.\r\n",
    "",
    {0,
     BIT(16) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_GET_NON_EMPTY_QUEUES_ID,
    "get_non_empty_queues",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get from the device the ingress FAP non empty queues\r\n",
    "Examples:\r\n"
    "  fap20v in_queue get_non_empty_queues 0\r\n"
    "Results in getting on device 0 non-empty queues. 8K queues are getting scanned\r\n",
    "",
    {0,
     0,
     BIT(27) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_GET_NON_EMPTY_Q_FLV_ID,
    "flow_info",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Print the scheduler information of the first local flow\r\n",
    "Examples:\r\n"
    "  fap20v in_queue get_non_empty_queues 0 flow_info\r\n"
    "Results in getting on device 0 non-empty queues. 8K queues are getting scanned\r\n",
    "",
    {0,
     0,
     BIT(27) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_GET_LBP_LAST_HEADER_ID,
    "get_last_pkt_hdr",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get the last packet header from the LBP\r\n",
    "Examples:\r\n"
    "  fap20v in_queue get_last_pkt_hdr 0\r\n"
    "Results in getting the last packet header from the LBP at device 0\r\n",
    "",
    {0,
     0,
     BIT(27) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_GET_FLOW_DATA_BASEQ_ID,
    "get_flow_data_baseq",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get from the device the ingress FAP Flow-Data Base Queue Id\r\n",
    "Examples:\r\n"
    "  fap20v in_queue get_flow_data_baseq 0\r\n"
    "Results in getting on device 0 the ingress FAP Flow-Data Base Queue Id\r\n",
    "",
    {0,
     0,
     BIT(27) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SET_FLOW_DATA_BASEQ_ID,
    "set_flow_data_baseq",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set from the device the ingress FAP Flow-Data Base Queue Id\r\n",
    "Examples:\r\n"
    "  fap20v in_queue set_flow_data_baseq 0 base_q 1000 increment true\r\n"
    "Results in setting on device 0 the ingress FAP to Flow-Data Base Queue Id 1000\r\n"
    " and with increment indicator\n\r",
    "",
    {0,
     0,
     BIT(28) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_BASE_QUEUE_ID_ID,
    "base_q",
    (PARAM_VAL_RULES *)&Fap20v_queue_id[0],
    (sizeof(Fap20v_queue_id) / sizeof(Fap20v_queue_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Flow-Data Base Queue Id\r\n",
    "Examples:\r\n"
    "  fap20v in_queue set_flow_data_baseq 0 base_q 1000 increment true\r\n"
    "Results in setting on device 0 the ingress FAP to Flow-Data Base Queue Id 1000\r\n"
    " and with increment indicator\n\r",
    "",
    {0,
     0,
     BIT(28) },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_INCREMENT_INDICATOR_ID,
    "increment",
    (PARAM_VAL_RULES *)&Fap20v_true_false_vals[0],
    (sizeof(Fap20v_true_false_vals) / sizeof(Fap20v_true_false_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Flow-Data Base Queue Id\r\n",
    "Examples:\r\n"
    "  fap20v in_queue set_flow_data_baseq 0 base_q 1000 increment true\r\n"
    "Results in setting on device 0 the ingress FAP to Flow-Data Base Queue Id 1000\r\n"
    " and with increment indicator\n\r",
    "",
    {0,
     0,
     BIT(28) },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_CLOSE_QUEUE_ID,
    "close_queue",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Close an ingress FAP queue.\r\n"
    "Note the device is set with some default parameters.\n\r"
    "These parameters do NOT cause the device not to send packets.\n\r",
    "Examples:\r\n"
    "  fap20v in_queue close_queue 0 queue_id 11 \r\n"
    "Results in closing on device 0 queue 11.\r\n",
    "",
    {0,
     BIT(16) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_UPDATE_QUEUE_ID,
    "update_queue",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Update an ingress FAP queue parameters.\r\n",
    "Examples:\r\n"
    "  fap20v in_queue update_queue 0 queue_id 11 target_fap 90 target_flow 1255 queue_type 0\r\n"
    "Results in updating on device 0 queue 11 to point:\n\r"
    " Target fap - 11, Target data port - 2, Target fap flow id - 1255 and queue type of 0.\r\n",
    "",
    {0,
     BIT(15) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_OPEN_QUEUE_ID,
    "open_queue",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Open at the device an ingress FAP queue to a target FAP scheduler flow.\r\n",
    "Examples:\r\n"
    "  fap20v in_queue open_queue 0 queue_id 11 target_fap 90 target_data_port 2 target_flow 1255 queue_type 0\r\n"
    "Results in openning on device 0 queue 11 to point:\n\r"
    " Target fap - 11, Target data port - 2, Target fap flow id - 1255 and queue type of 0.\r\n",
    "",
    {0,
     BIT(15) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_DESTINATION_ID_ID,
    "destination_id",
    (PARAM_VAL_RULES *)&Fap20v_destination_id[0],
    (sizeof(Fap20v_destination_id) / sizeof(Fap20v_destination_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Target identifier. Range from 0 to 4095 (4K).\r\n",
    "Examples:\r\n"
    "  fap20v in_queue get_queue_params 0 queue_id 11\r\n"
    "Results in getting on device 0 queue 11 parameters.\r\n",
    "",
    {0,
     0,
     BIT(25),
     0 },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_QUEUE_ID_ID,
    "queue_id",
    (PARAM_VAL_RULES *)&Fap20v_queue_id[0],
    (sizeof(Fap20v_queue_id) / sizeof(Fap20v_queue_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Queue identifier.\r\n",
    "Examples:\r\n"
    "  fap20v in_queue open_queue 0 queue_id 11 target_fap 90 target_data_port 2 target_flow 1255 queue_type 0\r\n"
    "Results in openning on device 0 queue 11 to point:\n\r"
    " Target fap - 11, Target data port - 2, Target fap flow id - 1255 and queue type of 0.\r\n",
    "",
    {0,
     BIT(15) | BIT(16)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TARGET_FAP_ID_ID,
    "target_fap",
    (PARAM_VAL_RULES *)&Fap20v_fap_id[0],
    (sizeof(Fap20v_fap_id) / sizeof(Fap20v_fap_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The target FAP for the queue.\r\n",
    "Examples:\r\n"
    "  fap20v in_queue open_queue 0 queue_id 11 target_fap 90 target_data_port 2 target_flow 1255 queue_type 0\r\n"
    "Results in openning on device 0 queue 11 to point:\n\r"
    " Target fap - 11, Target data port - 2, Target fap flow id - 1255 and queue type of 0.\r\n",
    "",
    {0,
     BIT(15) },
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TARGET_DATA_PORT_ID,
    "target_data_port",
    (PARAM_VAL_RULES *)&Fap20v_data_port_id[0],
    (sizeof(Fap20v_data_port_id) / sizeof(Fap20v_data_port_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The target Data-Port for the queue.\r\n",
    "Examples:\r\n"
    "  fap20v in_queue open_queue 0 queue_id 11 target_fap 90 target_data_port 2 target_flow 1255 queue_type 0\r\n"
    "Results in openning on device 0 queue 11 to point:\n\r"
    " Target fap - 11, Target data port - 2, Target fap flow id - 1255 and queue type of 0.\r\n",
    "",
    {0,
     BIT(15) },
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TARGET_FLOW_ID_ID,
    "target_flow",
    (PARAM_VAL_RULES *)&Fap20v_flow_id[0],
    (sizeof(Fap20v_flow_id) / sizeof(Fap20v_flow_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Flow, in egress scheduler, assigned for that queue.\r\n",
    "Examples:\r\n"
    "  fap20v in_queue open_queue 0 queue_id 11 target_fap 90 target_data_port 2 target_flow 1255 queue_type 0\r\n"
    "Results in openning on device 0 queue 11 to point:\n\r"
    " Target fap - 11, Target data port - 2, Target fap flow id - 1255 and queue type of 0.\r\n",
    "",
    {0,
     BIT(15) },
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_QUEUE_TYPE_ID,
    "queue_type",
    (PARAM_VAL_RULES *)&Fap20v_b_queue_types[0],
    (sizeof(Fap20v_b_queue_types) / sizeof(Fap20v_b_queue_types[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "The queue type.\r\n",
    "Examples:\r\n"
    "  fap20v in_queue open_queue 0 queue_id 11 target_fap 90 target_data_port 2 target_flow 1255 queue_type 0\r\n"
    "Results in openning on device 0 queue 11 to point:\n\r"
    " Target fap - 11, Target data port - 2, Target fap flow id - 1255 and queue type of 0.\r\n",
    "",
    {0,
     BIT(15) },
    7,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_TARGET_FAP_ID_ID,
    "target_fap",
    (PARAM_VAL_RULES *)&Fap20v_fap_id[0],
    (sizeof(Fap20v_fap_id) / sizeof(Fap20v_fap_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Target FAP identifier.\r\n",
    "Examples:\r\n"
    "  fap20v in_queue set_queue_labeling 0 target_fap 10 base_queue 1001 label_type destination_mapping \r\n"
    "Results in setting on device 0 for packet destained to fap 10,\n\r"
    "the base queue 1001, with destination_mapping labeling\r\n",
    "",
    {0,
     BIT(13) | BIT(14)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_BASE_QUEUE_ID_ID,
    "base_queue",
    (PARAM_VAL_RULES *)&Fap20v_queue_id[0],
    (sizeof(Fap20v_queue_id) / sizeof(Fap20v_queue_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Base queue identifier to use with the target FAP.\r\n",
    "Examples:\r\n"
    "  fap20v in_queue set_queue_labeling 0 target_fap 10 base_queue 1001 label_type destination_mapping \r\n"
    "Results in setting on device 0 for packet destained to fap 10,\n\r"
    "the base queue 1001, with destination_mapping labeling\r\n",
    "",
    {0,
     BIT(13)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* END fap20v_api_ingress_queue.h interface }*/
  /* { fap20v_api_queue_id_allocator.h interface */
  {
    PARAM_FAP20V_ALLOCATOR_API_ID,
    "allocator",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver ingress-queue-id/scheduler-flow-id allocator\r\n"
    "  module services. This module is a management aid of queue identifiers.\r\n"
    "  All of this services DO NOT read/write the device but rather\r\n"
    "  effect driver private structure.",
    "Examples:\r\n"
    "  fap20v allocator queue set_range 1 min 12 max 1000\r\n"
    "Results in setting on device 1 queue idetifier range to look from to [12 - 1000]\r\n",
    "",
    {0,
     BIT(10) | BIT(11) | BIT(12)| BIT(27) | BIT(7) | BIT(8) | BIT(9) },
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_QUEUE_ID_ALLOCATOR_API_ID,
    "queue",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver ingress queue id allocator module services.\r\n"
    "  This module is a management aid of queue identifiers. All of this\r\n"
    "  services DO NOT read/write the device but rather effect driver\r\n"
    "  private structure.",
    "Examples:\r\n"
    "  fap20v allocator queue set_range 1 min 12 max 1000\r\n"
    "Results in setting on device 1 queue idetifier range to look from to [12 - 1000]\r\n",
    "",
    {0,
     BIT(10) | BIT(11) | BIT(12)| BIT(27) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_PRINT_ALLOC_QUEUE_ID,
    "display_allocator",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display the driver queue-id allocator.\r\n",
    "Examples:\r\n"
    "  fap20v allocator queue display_allocator 1\r\n"
    "Results in displaying, on device 1, queue identifier allocator\r\n",
    "",
    {0,
     BIT(27)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SET_QUEUE_ID_RANGE_ID,
    "set_range",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set the range, out of [0-8191], to search queue identifier from.\r\n",
    "Examples:\r\n"
    "  fap20v allocator queue set_range 1 min 12 max 1000\r\n"
    "Results in setting on device 1 queue idetifier range to look from to [12 - 1000]\r\n",
    "",
    {0,
     BIT(10)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_MIN_QUEUE_ID_ID,
    "min",
    (PARAM_VAL_RULES *)&Fap20v_queue_id[0],
    (sizeof(Fap20v_queue_id) / sizeof(Fap20v_queue_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Minimum queue id to search from.\r\n",
    "Examples:\r\n"
    "  fap20v allocator queue set_range 1 min 12 max 1000\r\n"
    "Results in setting on device 1 queue idetifier range to look from to [12 - 1000]\r\n",
    "",
    {0,
     BIT(10)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_MAX_QUEUE_ID_ID,
    "max",
    (PARAM_VAL_RULES *)&Fap20v_queue_id[0],
    (sizeof(Fap20v_queue_id) / sizeof(Fap20v_queue_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Maximal queue id to search from (this number id is included in the search).\r\n",
    "Examples:\r\n"
    "  fap20v allocator queue set_range 1 min 12 max 1000\r\n"
    "Results in setting on device 1 queue idetifier range to look from to [12 - 1000]\r\n",
    "",
    {0,
     BIT(10)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_ALLOC_QUEUE_ID_BLOCK_ID,
    "alloc",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get free queue identifier within the specified range.\r\n"
    "  Mark that queues identifiers as 'taken'.\r\n",
    "Examples:\r\n"
    "  fap20v allocator queue alloc 1\r\n"
    "Results in allocating on device 1, block of consecutive queue idetifiers.\r\n"
    "The base queue is given back. The block size is taken from the device class.\n\r"
    "class_a: 1 queue block, class_b: 2, class_c: 4 and class_a: 8 queues block.\n\r",
    "",
    {0,
     BIT(11)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_FREE_QUEUE_ID_ID,
    "free",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Free the queue identifiers block of identifiers.\r\n",
    "Examples:\r\n"
    "  fap20v allocator queue free 0 base_queue 11\r\n"
    "Results in freeing on device 0, block of consecutive queue idetifiers,\r\n"
    "starting from base queue 11. The block size is taken from the device class.\n\r"
    "class_a: 1 queue block, class_b: 2, class_c: 4 and class_a: 8 queues block.\n\r",
    "",
    {0,
     BIT(12)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_QUEUE_BASE_ID_ID,
    "base_queue",
    (PARAM_VAL_RULES *)&Fap20v_flow_id[0],
    (sizeof(Fap20v_flow_id) / sizeof(Fap20v_flow_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "queue base identifier to free.\r\n",
    "Examples:\r\n"
    "  fap20v allocator queue free 0 base_queue 11 nof_queues 8\r\n"
    "Results in freeing on device 0, block of 8 consecutive queue idetifiers,\r\n"
    "starting from base queue 11. \n\r",
    "",
    {0,
     BIT(12)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_NOF_QUEUES_ID,
    "nof_queues",
    (PARAM_VAL_RULES *)&Fap20v_nof_queues[0],
    (sizeof(Fap20v_nof_queues) / sizeof(Fap20v_nof_queues[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Number of queues.\r\n",
    "Examples:\r\n"
    "  fap20v allocator queue free 0 base_queue 11 nof_queues 8\r\n"
    "Results in freeing on device 0, block of 8 consecutive queue idetifiers,\r\n"
    "starting from base queue 11. \n\r",
    "",
    {0,
     BIT(11) | BIT(12)
    },
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  /* END fap20v_api_queue_id_allocator.h interface }*/
  /* { fap20v_api_flow_id_allocator.h interface */
  {
    PARAM_FAP20V_FLOW_ID_ALLOCATOR_API_ID,
    "flow",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver scheduler-flow/queue id allocator module services.\r\n"
    "  This module is a management aid of flow identifiers. All of this\r\n"
    "  services DO NOT read/write the device but rather effect driver\r\n"
    "  private structure.",
    "Examples:\r\n"
    "  fap20v allocator flow set_range 0 min 12 max 1000\r\n"
    "Results in setting on device 1 flow idetifier range to look from to: [12 - 1000]\r\n",
    "",
    {0,
     BIT(7) | BIT(8) | BIT(9) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_PRINT_ALLOC_FLOW_ID,
    "display_allocator",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Display the driver flow-id allocator.\r\n",
    "Examples:\r\n"
    "  fap20v allocator flow display_allocator 1\r\n"
    "Results in displaying, on device 1, flow identifier allocator\r\n",
    "",
    {0,
     BIT(8)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SET_SCHEDULER_FLOW_ID_RANGE_ID,
    "set_range",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set the range, out of [0-8191], to search flow identifier from.\r\n",
    "Examples:\r\n"
    "  fap20v allocator flow set_range 0 min 12 max 1000\r\n"
    "Results in setting on device 1 flow idetifier range to look from to: [12 - 1000]\r\n",
    "",
    {0,
     BIT(7)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_MIN_FLOW_ID_ID,
    "min",
    (PARAM_VAL_RULES *)&Fap20v_flow_id[0],
    (sizeof(Fap20v_flow_id) / sizeof(Fap20v_flow_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Minimum flow id to search from.\r\n",
    "Examples:\r\n"
    "  fap20v allocator flow set_range 0 min 12 max 1000\r\n"
    "Results in setting on device 1 flow idetifier range to look from to: [12 - 1000]\r\n",
    "",
    {0,
     BIT(7)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_MAX_FLOW_ID_ID,
    "max",
    (PARAM_VAL_RULES *)&Fap20v_flow_id[0],
    (sizeof(Fap20v_flow_id) / sizeof(Fap20v_flow_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Maximal flow id to search from (this number id is.\r\n"
    "  included in the search).\r\n",
    "Examples:\r\n"
    "  fap20v allocator flow set_range 0 min 12 max 1000\r\n"
    "Results in setting on device 1 flow idetifier range to look from to: [12 - 1000]\r\n",
    "",
    {0,
     BIT(7)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_ALLOC_SCHEDULER_FLOW_ID_ID,
    "alloc",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get free flow identifier within the specified range.\r\n"
    "  Mark that flows identifiers as 'taken'\r\n",
    "Examples:\r\n"
    "  fap20v allocator flow set_range 0 min 12 max 1000\r\n"
    "Results in setting on device 1 flow idetifier range to look from to: [12 - 1000]\r\n",
    "",
    {0,
     BIT(8)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_FREE_SCHEDULER_FLOW_ID_ID,
    "free",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Free the flow identifier id.\r\n",
    "Examples:\r\n"
    "  fap20v allocator flow free 0 flow_id 11\r\n"
   "Results in setting on device 0, that flow id 11 is free for allocation again.\r\n",
    "",
    {0,
     BIT(9)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_FLOW_ID_ID,
    "flow_id",
    (PARAM_VAL_RULES *)&Fap20v_flow_id[0],
    (sizeof(Fap20v_flow_id) / sizeof(Fap20v_flow_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "flow identifier to free.\r\n",
    "Examples:\r\n"
    "  fap20v allocator flow free 0 flow_id 11\r\n"
    "Results in setting on device 0, that flow id 11 is free for allocation again.\r\n",
    "",
    {0,
     BIT(9)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* END fap20v_api_flow_id_allocator.h interface }*/
  /* { Egress */
  {
    PARAM_FAP20V_B_EGS_API_ID,
    "egress",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver egress configuration.",
    "Examples:\r\n"
    "  fap20v egress spi set_burst_size 0 burst size 64\r\n"
    "Results in setting on device 0 egress burst size to be 64 Bytes.\r\n",
    "",
    {0,0,0,0,0,0,0,0xFFFFFFFF},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_Q_API_ID,
    "queue",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver egress Queues configuration.",
    "Examples:\r\n"
    "  fap20v egress queue type_set 0 queue_id 64 queue_type 3\r\n"
    "Results in setting on device 0 egress queue 64 to queue type 3.\r\n",
    "",
    {0,0,0,0,0,0,0, BIT(10) | BIT(11) | BIT(12) | BIT(13) | BIT(14) \
                  | BIT(15) | BIT(16) | BIT(17) | BIT(18) | BIT(19) \
                  | BIT(20) | BIT(21) | BIT(22) | BIT(23) | BIT(24) \
                  | BIT(25) | BIT(26) },
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_GET_Q_TYPE_ID,
    "type_get",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get queue type. Please specify unit.",
    "Examples:\r\n"
    "  fap20v egress queue type_get 0 queue_id 64\r\n"
    "Results in getting the queue type of queue 64.\r\n",
    "",
    {0,0,0,0,0,0,0, BIT(10)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_STATUS_ID,
    "status",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get queues status. Please specify unit.",
    "Examples:\r\n"
    "  fap20v egress queue status 0 \r\n"
    "Results in getting the buffers current status.\r\n",
    "",
    {0,0,0,0,0,0,0, BIT(26)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_FAP20V_B_EGS_SET_Q_TYPE_ID,
    "type_set",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set queue type. Please specify unit.",
    "Examples:\r\n"
    "  fap20v egress queue type_set 0 queue_id 64 queue_type 3\r\n"
    "Results in setting on device 0 egress queue 64 to queue type 3.\r\n",
    "",
    {0,0,0,0,0,0,0, BIT(11)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_Q_ID_ID,
    "queue_id",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "queue type. Please specify queue_id.",
    "Examples:\r\n"
    "  fap20v egress queue type_get 0 queue_id 64\r\n"
    "Results in getting the queue type of queue 64.\r\n",
    "",
    {0,0,0,0,0,0,0, BIT(10) | BIT(11)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_Q_TYPE_ID,
    "queue_type",
    (PARAM_VAL_RULES *)&Fap20v_b_egq_type[0],
    (sizeof(Fap20v_b_egq_type) / sizeof(Fap20v_b_egq_type[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set queue type. Please specify queue_type.",
    "Examples:\r\n"
    "  fap20v egress queue type_set 0 queue_id 64 queue_type 3\r\n"
    "Results in setting on device 0 egress queue 64 to queue type 3.\r\n",
    "",
    {0,0,0,0,0,0,0, BIT(11)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_GET_Q_SETTINGS_ID,
    "settings_get",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get queue settings. Please specify unit.",
    "Examples:\r\n"
    "  fap20v egress queue settings_get 0 all\r\n"
    "Results in getting all the settings of device 0.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(12) | BIT(25)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_GET_Q_SETTINGS_ALL_ID,
    "all",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(&Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get all the settings.",
    "Examples:\r\n"
    "  fap20v egress queue settings_get 0 all\r\n"
    "Results in getting all the settings of device 0.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(12)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_GET_Q_SETTINGS_SHAPER_ID,
    "shaper",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(&Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get shaper settings.",
    "Examples:\r\n"
    "  fap20v egress queue settings_get 0 shaper\r\n"
    "Results in getting the shaper settings of device 0 EGQ.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(12)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_GET_Q_TH_ID,
    "queue_threshold",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(&Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get all queue threshold settings.",
    "Examples:\r\n"
    "  fap20v egress queue settings_get 0 queue_threshold all\r\n"
    "Results in getting the queue threshold settings of device 0 EGQ.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(25)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_GET_Q_TH_SETTINGS_WORD_FC_ID,
    "word_flow_control",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(&Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get queue threshold settings.",
    "Examples:\r\n"
    "  fap20v egress queue settings_get 0 queue_threshold word_flow_control\r\n"
    "Results in getting the word flow control parameters in the queue threshold settings.\r\n",    "",
    {0,0,0,0,0,0,0,BIT(25)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_GET_Q_TH_SETTINGS_PKT_FC_ID,
    "packet_flow_control",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(&Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get queue threshold settings.",
    "Examples:\r\n"
    "  fap20v egress queue settings_get 0 queue_threshold packet_flow_control\r\n"
    "Results in getting the packet flow control parameters in the queue threshold settings.\r\n",    "",
    {0,0,0,0,0,0,0,BIT(25)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_GET_Q_TH_SETTINGS_UP_LIMIT_ID,
    "upper_limit",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(&Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get upper limit queue threshold settings.",
    "Examples:\r\n"
    "  fap20v egress queue settings_get 0 queue_threshold upper_limit\r\n"
    "Results in getting the upper limit parameters in the queue threshold settings.\r\n",    "",
    {0,0,0,0,0,0,0,BIT(25)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_GET_Q_TH_SETTINGS_SCHD_ID,
    "scheduled",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(&Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get scheduled queue threshold settings.",
    "Examples:\r\n"
    "  fap20v egress queue settings_get 0 queue_threshold scheduled\r\n"
    "Results in getting the scheduled parameters in the queue threshold settings.\r\n",    "",
    {0,0,0,0,0,0,0,BIT(25)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_GET_Q_TH_SETTINGS_ALL_ID,
    "all",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(&Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get all queue threshold settings.",
    "Examples:\r\n"
    "  fap20v egress queue settings_get 0 queue_threshold all\r\n"
    "Results in getting all parameters in the queue threshold settings.\r\n",    "",
    {0,0,0,0,0,0,0,BIT(25)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_GET_Q_TH_SETTINGS_REC_ID,
    "recycle",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(&Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get recycle queue threshold settings.",
    "Examples:\r\n"
    "  fap20v egress queue settings_get 0 queue_threshold recycle\r\n"
    "Results in getting recycle parameters in the queue threshold settings.\r\n",    "",
    {0,0,0,0,0,0,0,BIT(25)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_GET_Q_SETTINGS_UNSCH_ID,
    "unscheduled",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(&Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get unscheduled settings.",
    "Examples:\r\n"
    "  fap20v egress queue settings_get 0 unscheduled\r\n"
    "Results in getting the unscheduled settings of device 0 EGQ.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(12)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_SETTINGS_ID,
    "settings_set",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set queue settings. Please specify unit.",
    "Examples:\r\n"
    "  fap20v egress queue settings_get 0 disable_mci 1\r\n"
    "Results in setting the disable_mci in the egress queue to TRUE.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(13) | BIT(14) | BIT(15) | BIT(16) | BIT(17) | BIT(18) |
                    BIT(19) | BIT(20) | BIT(21) | BIT(22) | BIT(23) | BIT(24) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_DISABLE_MCI_ID,
    "disable_mci",
    (PARAM_VAL_RULES *)&Fap20v_b_true_false_table[0],
    (sizeof(Fap20v_b_true_false_table) / sizeof(Fap20v_b_true_false_table[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set disable_mci parameter in egress queue.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 disable_mci false\r\n"
    "Results in setting the disable_mci in the egress queue to false.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(13)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_SHAPING_ID,
    "shaping",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set shaping parameters in egress queue.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 shaping ports_max_burst_in_bytes 40 \r\n"
    "Results in setting the ports_max_burst in the egress queue to 40 bytes.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(14) },
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_PRT_MAX_BURST_SIZE_ID,
    "ports_max_burst",
    (PARAM_VAL_RULES *)&Fap20v_b_egs_max_port_credit[0],
    (sizeof(Fap20v_b_egs_max_port_credit) / sizeof(Fap20v_b_egs_max_port_credit[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set max burst size of a single port (Bytes).",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 shaping ports_max_burst 40 \r\n"
    "Results in setting the ports_max_burst in the egress queue to 40 bytes.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_SPI_A_SHP_RATE_ID,
    "spi_a_shaper_rate",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set spi A shaping rate (Mbps)",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 spi_a_shaper_rate 69 \r\n"
    "Results in setting spi A shaper rate.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_SPI_B_SHP_RATE_ID,
    "spi_b_shaper_rate",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set spi B shaping rate (Mbps)",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 spi_b_shaper_rate 69 \r\n"
    "Results in setting spi B shaper rate.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_SPI_A_SHP_MAX_BRST_ID,
    "spi_a_max_burst",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set spi A max burst size of the SPI (Bytes)",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 spi_a_max_burst 10 \r\n"
    "Results in setting spi A shaper max burst to 10 bytes.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_SPI_B_SHP_MAX_BRST_ID,
    "spi_b_max_burst",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set spi B max burst size of the SPI (Bytes)",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 spi_b_max_burst 10 \r\n"
    "Results in setting spi B shaper max burst to 10 bytes.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_SPI_A_SHP_UCAST_ADJ_ID,
    "credit_adjust_spi_a_ucast",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set number of Bytes to be adjusted for every spi A unicast packet (Bytes)",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 credit_adjust_spi_a_ucast 2 \r\n"
    "Results in adjusting spi A unicast packet to 2 bytes.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_SPI_A_SHP_MCAST_ADJ_ID,
    "credit_adjust_spi_a_mcast",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set number of Bytes to be adjusted for every spi A unicast packet (Bytes)",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 credit_adjust_spi_a_mcast 2 \r\n"
    "Results in adjusting spi A multicast packet to 2 bytes.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_FAP20V_B_EGS_SET_Q_SPI_B_SHP_UCAST_ADJ_ID,
    "credit_adjust_spi_b_ucast",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set number of Bytes to be adjusted for every spi B unicast packet (Bytes)",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 credit_adjust_spi_a_ucast 2 \r\n"
    "Results in adjusting spi B unicast packet to 2 bytes.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_SPI_B_SHP_MCAST_ADJ_ID,
    "credit_adjust_spi_b_mcast",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set number of Bytes to be adjusted for every spi B unicast packet (Bytes)",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 credit_adjust_spi_b_mcast 2 \r\n"
    "Results in adjusting spi B multicast packet to 2 bytes.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_FC_BY_WORDS_ID,
    "flow_control_by_words",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress queue flow control parameters by words.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 flow_control_by_words total_schd_wrds_fc_th 45\r\n"
    "Results in setting threshold in all scheduled queues above which the device \r\nflow-control to the scheduler is asserted to 45.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(15) },
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_FC_BY_WORDS_SCH_TH_ID,
    "total_schd_wrds_fc_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress queue flow control parameters by words.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 flow_control_by_words total_schd_wrds_fc_th 45\r\n"
    "Results in setting threshold in all scheduled queues above which the device \r\nflow-control to the scheduler is asserted to 45.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(15)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_FC_BY_WORDS_SPIA_TH_ID,
    "total_spi_a_wrds_fc_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress queue spi A flow control threshold.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 flow_control_by_words total_spi_a_wrds_fc_th 5253\r\n"
    "Results in setting egress queue spi A flow control threshold to 5253 words.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(15)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_FC_BY_WORDS_SPIB_TH_ID,
    "total_spi_b_wrds_fc_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress queue spi B flow control threshold.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 flow_control_by_words total_spi_b_wrds_fc_th 5253\r\n"
    "Results in setting egress queue spi B flow control threshold to 5253 words.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(15)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_FC_BY_WORDS_P_A_SCH_ID,
    "port_a_schd_wrds_fc_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress queue port A threshold by words.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 flow_control_by_words port_a_schd_wrds_fc_th 99\r\n"
    "Results in setting gress queue port A threshold to 99 words.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(15)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_FC_BY_WORDS_P_B_SCH_ID,
    "port_b_schd_wrds_fc_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress queue port B threshold by words.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 flow_control_by_words port_b_schd_wrds_fc_th 99\r\n"
    "Results in setting gress queue port B threshold to 99 words.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(15)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_FC_BY_WORDS_P_C_SCH_ID,
    "port_c_schd_wrds_fc_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress queue port C threshold by words.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 flow_control_by_words port_c_schd_wrds_fc_th 99\r\n"
    "Results in setting gress queue port C threshold to 99 words.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(15)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_FC_BY_WORDS_P_D_SCH_ID,
    "port_d_schd_wrds_fc_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress queue port D threshold by words.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 flow_control_by_words port_d_schd_wrds_fc_th 99\r\n"
    "Results in setting gress queue port D threshold to 99 words.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(15)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_FC_BY_PKTS_ID,
    "flow_control_by_packets",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress queue flow control parameters by packets.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 flow_control_by_packets total_schd_pkts_fc_th 23\r\n"
    "Results in setting threshold in all scheduled queues above which the device \r\nflow-control to the scheduler is asserted to 23 packets.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(16) },
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_FC_BY_PKTS_SCH_TH_ID,
    "total_schd_pkts_fc_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress queue flow control parameters by packets.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 flow_control_by_packets total_schd_pkts_fc_th 23\r\n"
    "Results in setting threshold in all scheduled queues above which the device \r\nflow-control to the scheduler is asserted to 23 packets.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_FC_BY_PKTS_SPIA_TH_ID,
    "total_spi_a_schd_pkts_fc_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress queue spi A flow control threshold.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 flow_control_by_words total_spi_a_wrds_fc_th 200\r\n"
    "Results in setting egress queue spi A flow control threshold to 200 packets.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_FC_BY_PKTS_SPIB_TH_ID,
    "total_spi_b_schd_pkts_fc_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress queue spi B flow control threshold.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 flow_control_by_words total_spi_b_wrds_fc_th 200\r\n"
    "Results in setting egress queue spi B flow control threshold to 200 packets.\r\n",    "",
    {0,0,0,0,0,0,0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_SCHEDLUED,
    "scheduled",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress queue scheduled words rejected threshold.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 scheduled total_schd_wrds_th 10\r\n"
    "Results in setting scheduled words rejected threshold to 10 words\r\n",    "",
    {0,0,0,0,0,0,0, BIT(17) },
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_SCH_ALL_SCH_TH_WRD_ID,
    "total_schd_wrds_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress queue scheduled words rejected threshold.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 scheduled total_schd_wrds_th 10\r\n"
    "Results in setting scheduled words rejected threshold to 10 words\r\n",    "",
    {0,0,0,0,0,0,0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_SCH_ALL_SCH_TH_PKT_ID,
    "total_schd_pkts_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress queue scheduled packets rejected threshold.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 scheduled total_schd_pkts_th 10\r\n"
    "Results in setting scheduled words rejected threshold to 10 packets\r\n",    "",
    {0,0,0,0,0,0,0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_SCH_FREE_DESC_ID,
    "free_desc_schd_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress scheduled free descriptors rejected threshold.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 scheduled free_desc_schd_th 10\r\n"
    "Results in setting scheduled free descriptors rejected threshold to 10 packets\r\n",    "",
    {0,0,0,0,0,0,0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_SCH_PORT_A_TH_WRD,
    "port_a_schd_wrds_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress scheduled port A words threshold.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 scheduled port_a_schd_wrds_th 10\r\n"
    "Results in setting port A words threshold 10 words\r\n",    "",
    {0,0,0,0,0,0,0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_SCH_PORT_B_TH_WRD,
    "port_b_schd_wrds_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress scheduled port B words threshold.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 scheduled port_b_schd_wrds_th 10\r\n"
    "Results in setting port B words threshold 10 words\r\n",    "",
    {0,0,0,0,0,0,0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_SCH_PORT_C_TH_WRD,
    "port_c_schd_wrds_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress scheduled port C words threshold.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 scheduled port_c_schd_wrds_th 10\r\n"
    "Results in setting port C words threshold 10 words\r\n",    "",
    {0,0,0,0,0,0,0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_SCH_PORT_D_TH_WRD,
    "port_d_schd_wrds_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress scheduled port D words threshold.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 scheduled port_d_schd_wrds_th 10\r\n"
    "Results in setting port D words threshold 10 words\r\n",    "",
    {0,0,0,0,0,0,0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_DIS_MCI_ID,
    "mci",
    (PARAM_VAL_RULES *)&Fap20v_b_egs_mci_type[0],
    (sizeof(Fap20v_b_egs_mci_type) / sizeof(Fap20v_b_egs_mci_type[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress queue mci[0/1] threshold parameters.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 mci 0 total_unschd_wrds_th 10\r\n"
    "Results in setting the unscheduled word threshold for all queues above which mci0 is asserted\r\n",    "",
    {0,0,0,0,0,0,0, BIT(18)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_DIS_MCI_UNSC_WRD_TH_ID,
    "total_unschd_wrds_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress queue unscheduled word mci[0/1] threshold parameters.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 mci [0/1] total_unschd_wrds_th 10\r\n"
    "Results in setting the unscheduled word threshold for all queues above which mci[0/1] is asserted\r\n",    "",
    {0,0,0,0,0,0,0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_DIS_MCI_UNSC_PKT_TH_ID,
    "total_unschd_pkts_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress queue unscheduled packets mci[0/1] threshold parameters.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 mci [0/1] total_unschd_pkts_th 10\r\n"
    "Results in setting the unscheduled packets threshold for all queues above which mci[0/1] is asserted\r\n",    "",
    {0,0,0,0,0,0,0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_DIS_MCI_FREE_BUF_TH_ID,
    "free_buf_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress queue free buffers mci[0/1] threshold parameters.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 mci [0/1] free_buf_th 10\r\n"
    "Results in setting the free buffers threshold for all queues above which mci[0/1] is asserted\r\n",    "",
    {0,0,0,0,0,0,0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_DIS_MCI_FREE_DSC_TH_ID,
    "free_desc_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress queue free descriptors mci[0/1] threshold parameters.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 mci [0/1] free_desc_th 10\r\n"
    "Results in setting the free descriptors threshold for all queues above which mci[0/1] is asserted\r\n",    "",
    {0,0,0,0,0,0,0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_DIS_MCI_PORT_A_UNSC_ID,
    "port_a_unschd_wrds_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress queue unscheduled word mci[0/1] threshold for port A threshold parameters.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 mci [0/1] port_a_unschd_wrds_th 10\r\n"
    "Results in setting the unscheduled word threshold for port A which mci[0/1] is asserted to 10 words\r\n",    "",
    {0,0,0,0,0,0,0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_DIS_MCI_PORT_B_UNSC_ID,
    "port_b_unschd_wrds_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress queue unscheduled word mci[0/1] threshold for port B threshold parameters.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 mci [0/1] port_a_unschd_wrds_th 10\r\n"
    "Results in setting the unscheduled word threshold for port A which mci[0/1] is asserted to 10 words\r\n",    "",
    {0,0,0,0,0,0,0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_DIS_MCI_PORT_C_UNSC_ID,
    "port_c_unschd_wrds_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress queue unscheduled word mci[0/1] threshold for port C threshold parameters.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 mci [0/1] port_c_unschd_wrds_th 10\r\n"
    "Results in setting the unscheduled word threshold for port C which mci[0/1] is asserted to 10 words\r\n",    "",
    {0,0,0,0,0,0,0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_DIS_MCI_PORT_D_UNSC_ID,
    "port_d_unschd_wrds_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set egress queue unscheduled word mci[0/1] threshold for port D threshold parameters.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 mci [0/1] port_d_unschd_wrds_th 10\r\n"
    "Results in setting the unscheduled word threshold for port D which mci[0/1] is asserted to 10 words\r\n",    "",
    {0,0,0,0,0,0,0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_FAP20V_B_EGS_SET_Q_RCY_ALMOST_FULL_TH_ID,
    "recycle_almost_full_th  ",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set recycle queue almost full threshold generation to LBP.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 recycle_almost_full_th 10\r\n"
    "Results in setting recycle queue almost full threshold generation to 10 words\r\n",    "",
    {0,0,0,0,0,0,0, BIT(19)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_UP_LIMIT_TH,
    "upper_limit_threshold",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set upper limit thresholds.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 upper_limit_threshold free_buf_fc_th 10\r\n"
    "Results in setting avilable buffers below which device flow control to the scheduler is asserted\r\n",    "",
    {0,0,0,0,0,0,0, BIT(20)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_UP_LIMIT_TH_FREE_BUFF,
    "free_buf_fc_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set free buffers limit threshold.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 upper_limit_threshold free_buf_fc_th 10\r\n"
    "Results in setting avilable buffers below which device flow control to the scheduler is asserted\r\n",    "",
    {0,0,0,0,0,0,0, BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_Q_UP_LIMIT_TH_FREE_DESC,
    "free_desc_fc_th",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set free descriptors limit threshold.",
    "Examples:\r\n"
    "  fap20v egress queue settings_set 0 upper_limit_threshold free_desc_fc_th 10\r\n"
    "Results in setting avilable descriptors below which device flow control to the scheduler is asserted\r\n",    "",
    {0,0,0,0,0,0,0, BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SPI_API_ID,
    "spi",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver egress SPI configuration.",
    "Examples:\r\n"
    "  fap20v egress spi set_burst_size 0 burst size 64\r\n"
    "Results in setting on device 0 egress burst size to be 64 Bytes.\r\n",
    "",
    {0,0,0,0,0,0,0, BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_SET_BURST_SIZE_ID,
    "set_burst_size",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Set burst size to SPI. Please specify unit.",
    "Examples:\r\n"
    "  fap20v egress spi set_burst_size 0 burst size 64\r\n"
    "Results in setting on device 0 egress burst size to be 64 Bytes.\r\n",
    "",
    {0,0,0,0,0,0,0, BIT(0)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_GET_BURST_SIZE_ID,
    "get_burst_size",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Get burst size to SPI. Please specify unit.",
    "Examples:\r\n"
    "  fap20v egress spi set_burst_size 0 burst size 64\r\n"
    "Results in setting on device 0 egress burst size to be 64 Bytes.\r\n",
    "",
    {0,0,0,0,0,0,0, BIT(1)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_EGS_BURST_SIZE_VAL_ID,
    "size",
    (PARAM_VAL_RULES *)&Fap20v_burst_size_vals[0],
    (sizeof(Fap20v_burst_size_vals) / sizeof(Fap20v_burst_size_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Burst size to SPI. Please specify size in Bytes.",
    "Examples:\r\n"
    "  fap20v egress spi set_burst_size 0 burst size 64\r\n"
    "Results in setting on device 0 egress burst size to be 64 Bytes.\r\n",
    "",
    {0,0,0,0,0,0,0, BIT(0)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* END Egress }*/

  /* { SOC_SAND_FAP20V B Scheduler */
  {
    PARAM_FAP20V_B_SCHEDULER_API_ID,
    "scheduler",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver scheduler ports/flows services.",
    "Examples:\r\n"
    "  fap20v scheduler port set_scheduler_port 0x1 port_id 10 credit_bandwidth 100 nominal_bandwidth 90 wfq0_weight 2\r\n"
    "Results in setting on device 1 scheduler port 10 to:\r\n"
    " 100 MegbitSec credit, 90 MegbitSec nominal bandwidth,\r\n"
    " and with wfq0 weight of 2% for WFQ1 (98% for EDF1).\r\n",
    "",
    {0,0,0,0,0,0xFFFFFFFF},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* { Ports (bit 0,1,15)*/
  {
    PARAM_FAP20V_B_SCH_PORT_API_ID,
    "port",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver scheduler ports services.",
    "Examples:\r\n"
    "  fap20v scheduler port set_scheduler_port 0x1 port_id 10 hr_mode regular state enabled\r\n"
    "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
    "",
    {0,0,0,0,0, BIT(0) | BIT(1) | BIT(15)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_PORT_SET_ID,
    "open",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the scheduler port parameters.\r\n",
    "Examples:\r\n"
    "  fap20v scheduler port set_scheduler_port 0x1 port_id 10 hr_mode regular state enabled\r\n"
    "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
    "",
    {0,0,0,0,0, BIT(1)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_PORT_GET_ID,
    "get",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets the scheduler port parameters.\r\n",
    "Examples:\r\n"
    "  fap20v scheduler port get_scheduler_port 0x1 port_id 10\r\n"
    "Results in getting on device 1 scheduler port 10 parameters.\r\n",
    "",
    {0,0,0,0,0, BIT(0)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_PORT_DEL_ID,
    "close",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Close scheduler port parameters.\r\n",
    "Examples:\r\n"
    "  fap20v scheduler port close 0x1 port_id 10\r\n"
    "Results in getting on device 1 scheduler port 10 parameters.\r\n",
    "",
    {0,0,0,0,0, BIT(15)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_PORT_ID_ID,
    "port_id",
    (PARAM_VAL_RULES *)&Fap20v_scheduler_port_id_vals[0],
    (sizeof(Fap20v_scheduler_port_id_vals) / sizeof(Fap20v_scheduler_port_id_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Scheduler port to add/remove. Range: 0-64.\r\n",
    "Examples:\r\n"
    "  fap20v scheduler port set_scheduler_port 0x1 port_id 10 hr_mode regular state enabled\r\n"
    "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
    "",
    {0,0,0,0,0, BIT(0) | BIT(1) | BIT(15)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_PORT_HR_MODE,
    "hr_mode",
    (PARAM_VAL_RULES *)&Fap20v_b_hr_mode[0],
    (sizeof(Fap20v_b_hr_mode) / sizeof(Fap20v_b_hr_mode[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "HR scheduler mode.\r\n",
    "Examples:\r\n"
    "  fap20v scheduler port set_scheduler_port 0x1 port_id 10 hr_mode regular state enabled\r\n"
    "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
    "",
    {0,0,0,0,0, BIT(1)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_EG_PORT_CREDIT_ID,
    "credit_rate",
    (PARAM_VAL_RULES *)&Fap20v_b_port_credit_rate[0],
    (sizeof(Fap20v_b_port_credit_rate) / sizeof(Fap20v_b_port_credit_rate[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "credit_rate. Mega-Bit-Sec\r\n",
    "Examples:\r\n"
    "  fap20v scheduler port \r\n"
    "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
    "",
    {0,0,0,0,0, BIT(1)},
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_EG_PORT_NOMINAL_ID,
    "nominal_rate",
    (PARAM_VAL_RULES *)&Fap20v_b_port_credit_rate[0],
    (sizeof(Fap20v_b_port_credit_rate) / sizeof(Fap20v_b_port_credit_rate[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "nominal_rate. Mega-Bit-Sec\r\n",
    "Examples:\r\n"
    "  fap20v scheduler port \r\n"
    "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
    "",
    {0,0,0,0,0, BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_EG_PORT_RATE_ID,
    "max_rate",
    (PARAM_VAL_RULES *)&Fap20v_b_port_credit_rate[0],
    (sizeof(Fap20v_b_port_credit_rate) / sizeof(Fap20v_b_port_credit_rate[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "max_rate. Mega-Bit-Sec\r\n",
    "Examples:\r\n"
    "  fap20v scheduler port \r\n"
    "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
    "",
    {0,0,0,0,0, BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_EG_PORT_PRIO_ID,
    "priority",
    (PARAM_VAL_RULES *)&Fap20v_b_port_prio[0],
    (sizeof(Fap20v_b_port_prio) / sizeof(Fap20v_b_port_prio[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "priority.\r\n",
    "Examples:\r\n"
    "  fap20v scheduler port \r\n"
    "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
    "",
    {0,0,0,0,0, BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_EG_PORT_SCH_W_ID,
    "sch_weight",
    (PARAM_VAL_RULES *)&Fap20v_b_port_sch_weight[0],
    (sizeof(Fap20v_b_port_sch_weight) / sizeof(Fap20v_b_port_sch_weight[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sch_weight.\r\n",
    "Examples:\r\n"
    "  fap20v scheduler port \r\n"
    "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
    "",
    {0,0,0,0,0, BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_EG_PORT_UNSCH_W_ID,
    "unsch_weight",
    (PARAM_VAL_RULES *)&Fap20v_b_port_sch_weight[0],
    (sizeof(Fap20v_b_port_sch_weight) / sizeof(Fap20v_b_port_sch_weight[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "unsch_weight.\r\n",
    "Examples:\r\n"
    "  fap20v scheduler port \r\n"
    "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
    "",
    {0,0,0,0,0, BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_PORT_END_ID_ID,
    "end_port_id",
    (PARAM_VAL_RULES *)&Fap20v_scheduler_port_id_vals[0],
    (sizeof(Fap20v_scheduler_port_id_vals) / sizeof(Fap20v_scheduler_port_id_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "If appears, initiailze from 'port_id' to 'end_port_id' with the same parameters.\r\n"
    "  This command is usefull when all ports have the same configuration.",
    "Examples:\r\n"
    "  fap20v scheduler port set_scheduler_port 0x1 port_id 0 hr_mode regular state enabled end_port_id 11\r\n"
    "Results in setting on device 1 scheduler ports 0-11\r\n"
    " with regular hr scheduler.\r\n",
    "",
    {0,0,0,0,0, BIT(0) | BIT(1) | BIT(15)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* } Ports */
  /* { Aggregate (bit 2-8)*/
  {
    PARAM_FAP20V_B_SCH_AGG_API_ID,
    "aggregate",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver scheduler aggregate services.",
    "Examples:\r\n"
    "  fap20v scheduler aggregate set_aggregate 0x1 agg_id 10 hr_mode regular state enabled\r\n"
    "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
    "",
    {0,0,0,0,0, BIT(5) | BIT(6) | BIT(7) | BIT(2) | BIT(3) | BIT(4) | BIT(8)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_AGG_SET_ID,
    "open",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets the scheduler port parameters.\r\n",
    "Examples:\r\n"
    "  fap20v scheduler port set_scheduler_port 0x1 port_id 10 hr_mode regular state enabled\r\n"
    "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
    "",
    {0,0,0,0,0, BIT(5) | BIT(6) | BIT(7) | BIT(2) | BIT(3) | BIT(4)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_AGG_GET_ID,
    "get",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets the scheduler port parameters.\r\n",
    "Examples:\r\n"
    "  fap20v scheduler port get_scheduler_port 0x1 port_id 10\r\n"
    "Results in getting on device 1 scheduler port 10 parameters.\r\n",
    "",
    {0,0,0,0,0, BIT(8)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_AGG_CLOSE_ID,
    "close",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets the scheduler port parameters.\r\n",
    "Examples:\r\n"
    "  fap20v scheduler aggregate close_saggregate 0x1 port_id 10\r\n"
    "Results in getting on device 1 scheduler port 10 parameters.\r\n",
    "",
    {0,0,0,0,0, BIT(8)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_AGG_ID_ID,
    "agg_id",
    (PARAM_VAL_RULES *)&Fap20v_sch_agg_id_vals[0],
    (sizeof(Fap20v_sch_agg_id_vals) / sizeof(Fap20v_sch_agg_id_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Scheduler port to add/remove. Range: 0-3327.\r\n",
    "Examples:\r\n"
    "  fap20v scheduler port set_scheduler_port 0x1 port_id 10 hr_mode regular state enabled\r\n"
    "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
    "",
    {0,0,0,0,0,  BIT(5) | BIT(6) | BIT(7) | BIT(2) | BIT(3) | BIT(4) | BIT(8)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_AGG_HR_MODE,
    "hr_mode",
    (PARAM_VAL_RULES *)&Fap20v_b_hr_mode[0],
    (sizeof(Fap20v_b_hr_mode) / sizeof(Fap20v_b_hr_mode[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "HR scheduler mode.\r\n",
    "Examples:\r\n"
    "  fap20v scheduler port set_scheduler_port 0x1 port_id 10 hr_mode regular state enabled\r\n"
    "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
    "",
    {0,0,0,0,0, BIT(5) | BIT(2)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_AGG_CL_TYPE,
    "cl_type",
    (PARAM_VAL_RULES *)&Fap20v_b_sch_cl_type_vals[0],
    (sizeof(Fap20v_b_sch_cl_type_vals) / sizeof(Fap20v_b_sch_cl_type_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "CL scheduler type.\r\n",
    "Examples:\r\n"
    "  fap20v scheduler port set_scheduler_port 0x1 port_id 10 hr_mode regular state enabled\r\n"
    "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
    "",
    {0,0,0,0,0, BIT(6) | BIT(3)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_AGG_FQ_TYPE,
    "fq_type",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "HR scheduler mode.\r\n",
    "Examples:\r\n"
    "  fap20v scheduler port set_scheduler_port 0x1 port_id 10 hr_mode regular state enabled\r\n"
    "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
    "",
    {0,0,0,0,0, BIT(7) | BIT(4)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_AGG_1_TYPE_ID,
    "sub_flow_1",
    (PARAM_VAL_RULES *)&Fap20v_b_sch_agg_type_vals[0],
    (sizeof(Fap20v_b_sch_agg_type_vals) / sizeof(Fap20v_b_sch_agg_type_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Scheduler port to add/remove. Range: 0-3327.\r\n",
    "Examples:\r\n"
    "  fap20v scheduler port set_scheduler_port 0x1 port_id 10 hr_mode regular state enabled\r\n"
    "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
    "",
    {0,0,0,0,0,  BIT(5) | BIT(6) | BIT(7) | BIT(2) | BIT(3) | BIT(4)},
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_AGG_1_CREDIT_SRC_ID,
    "credit_src",
    (PARAM_VAL_RULES *)&Fap20v_sch_agg_id_vals[0],
    (sizeof(Fap20v_sch_agg_id_vals) / sizeof(Fap20v_sch_agg_id_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Scheduler port to add/remove. Range: 0-3327.\r\n",
    "Examples:\r\n"
    "  fap20v scheduler port set_scheduler_port 0x1 port_id 10 hr_mode regular state enabled\r\n"
    "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
    "",
    {0,0,0,0,0,  BIT(5) | BIT(6) | BIT(7) | BIT(2) | BIT(3) | BIT(4)},
    7,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_AGG_SUB_FLOW_2_DISABLE,
    "sub_flow_2_is_disabled",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "If appears, Only one active sub-flow.",
    "\r\n",
    "",
    {0,0,0,0,0, BIT(2) | BIT(3) | BIT(4)},
    8,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_AGG_2_TYPE_ID,
    "sub_flow_2",
    (PARAM_VAL_RULES *)&Fap20v_b_sch_agg_type_vals[0],
    (sizeof(Fap20v_b_sch_agg_type_vals) / sizeof(Fap20v_b_sch_agg_type_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Scheduler port to add/remove. Range: 0-3327.\r\n",
    "Examples:\r\n"
    "  fap20v scheduler port set_scheduler_port 0x1 port_id 10 hr_mode regular state enabled\r\n"
    "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
    "",
    {0,0,0,0,0,  BIT(5) | BIT(6) | BIT(7)},
    8,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_AGG_2_CREDIT_SRC_ID,
    "credit_src_2",
    (PARAM_VAL_RULES *)&Fap20v_sch_agg_id_vals[0],
    (sizeof(Fap20v_sch_agg_id_vals) / sizeof(Fap20v_sch_agg_id_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Scheduler port to add/remove. Range: 0-3327.\r\n",
    "Examples:\r\n"
    "  fap20v scheduler port set_scheduler_port 0x1 port_id 10 hr_mode regular state enabled\r\n"
    "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
    "",
    {0,0,0,0,0,  BIT(5) | BIT(6) | BIT(7)},
    9,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_AGG_1_WEIGHT_ID,
    "sub_flow_1_weight",
    (PARAM_VAL_RULES *)&Fap20v_b_sub_flow_weight_vals[0],
    (sizeof(Fap20v_b_sub_flow_weight_vals) / sizeof(Fap20v_b_sub_flow_weight_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "If appears, Weight for sub-flow 1.",
    "\r\n",
    "",
    {0,0,0,0,0, BIT(5) | BIT(6) | BIT(7) | BIT(2) | BIT(3) | BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_AGG_2_WEIGHT_ID,
    "sub_flow_2_weight",
    (PARAM_VAL_RULES *)&Fap20v_b_sub_flow_weight_vals[0],
    (sizeof(Fap20v_b_sub_flow_weight_vals) / sizeof(Fap20v_b_sub_flow_weight_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "If appears, Weight for sub-flow 2.",
    "\r\n",
    "",
    {0,0,0,0,0, BIT(5) | BIT(6) | BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_FAP20V_B_SCH_AGG_1_SHAPER_RATE_ID,
    "sub_flow_1_shaper_rate",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "If appears, This would be the sub-flow shaper rate.\n\r"
    "  Kilobit per second Resolution",
    "\r\n",
    "",
    {0,0,0,0,0, BIT(5) | BIT(6) | BIT(7) | BIT(2) | BIT(3) | BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_AGG_2_SHAPER_RATE_ID,
    "sub_flow_2_shaper_rate",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "If appears, This would be the sub-flow shaper rate.\n\r"
    "  Kilobit per second Resolution",
    "\r\n",
    "",
    {0,0,0,0,0, BIT(5) | BIT(6) | BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_FAP20V_B_SCH_AGG_1_SHAPER_BURST_ID,
    "sub_flow_1_shaper_burst",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "If appears, This would be the sub-flow shaper burst.\n\r"
    "  Byte Resolution.",
    "\r\n",
    "",
    {0,0,0,0,0, BIT(5) | BIT(6) | BIT(7) | BIT(2) | BIT(3) | BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_AGG_2_SHAPER_BURST_ID,
    "sub_flow_2_shaper_burst",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "If appears, This would be the sub-flow shaper burst."
    "  Byte Resolution.\r\n",
    "\r\n",
    "",
    {0,0,0,0,0, BIT(5) | BIT(6) | BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SCH_AGG_END_ID_ID,
    "end_agg_id",
    (PARAM_VAL_RULES *)&Fap20v_sch_agg_id_vals[0],
    (sizeof(Fap20v_sch_agg_id_vals) / sizeof(Fap20v_sch_agg_id_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "If appears, initiailze from 'port_id' to 'end_port_id' with the same parameters.\r\n"
    "  This command is usefull when all ports have the same configuration.",
    "Examples:\r\n"
    "  fap20v scheduler port set_scheduler_port 0x1 port_id 0 hr_mode regular state enabled end_port_id 11\r\n"
    "Results in setting on device 1 scheduler ports 0-11\r\n"
    " with regular hr scheduler.\r\n",
    "",
    {0,0,0,0,0, BIT(5) | BIT(6) | BIT(7) | BIT(2) | BIT(3) | BIT(4) | BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* } Aggregate */
  /*TODO!! remove*/
    {
      PARAM_FAP20V_B_SCH_SCM_ID,
      "scheme_set",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "",
      "",
      "",
      {0,0,0,0,0, BIT(30)},
      2,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_SCM_GET_ID,
      "scheme_get",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "",
      "",
      "",
      {0,0,0,0,0, BIT(31)},
      2,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_SCM_PORT_ID,
      "port_id",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "",
      "",
      "",
      {0,0,0,0,0, BIT(30) | BIT(31)},
      3,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_SCM_PORT_RATE_ID,
      "port_rate",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Mbps",
      "",
      "",
      {0,0,0,0,0, BIT(30)},
      4,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_SCM_ID_ID,
      "scheme_id",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "0 - strict, 1 = enhanced, 2 = weight_flow,3=weight_class,4=mixed_class,5=mixed_flow",
      "",
      "",
      {0,0,0,0,0, BIT(30)},
      5,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_SCM_MIN_RATE_ID,
      "min_rate_per_class",
      (PARAM_VAL_RULES *)&Fap20v_percentage_vals[0],
      (sizeof(Fap20v_percentage_vals) / sizeof(Fap20v_percentage_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Each class will have minimum_rate_by_percentage percentage of\n\r"
      "the port rate reserved to it.  \n\r"
      "0 - Stands for no assured rate.\n\r"
      "Valid values: 0-100.",
      "",
      "",
      {0,0,0,0,0, BIT(30)},
      LAST_ORDINAL,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_SCM_MAX_RATE_ID,
      "max_rate_per_class",
      (PARAM_VAL_RULES *)&Fap20v_percentage_vals[0],
      (sizeof(Fap20v_percentage_vals) / sizeof(Fap20v_percentage_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "No class will have more than maximum_rate_by_percentage\n\r"
      "percentage of the port rate.\n\r"
      "0 - Stands for not limitation on the class rate.  \n\r"
      "Valid values: 0-100.",
      "",
      "",
      {0,0,0,0,0, BIT(30)},
      LAST_ORDINAL,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_SCM_LOW_LATENCY_ID,
      "low_latency",
      (PARAM_VAL_RULES *)&Fap20v_percentage_vals[0],
      (sizeof(Fap20v_percentage_vals) / sizeof(Fap20v_percentage_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Add a second sub-flow that will decrease the latency.\n\r"
      "When this is enabled, the maximum_rate_by_percentage,\n\r"
      "will behave differently!!!!!                         \n\r"
      "0 - Disable, 1 - Enable.  \n\r",
      "",
      "",
      {0,0,0,0,0, BIT(30)},
      LAST_ORDINAL,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
  /*
  * { flow (bits 20-23)
  */
    {
      PARAM_FAP20V_B_SCH_FLOW_API_ID,
      "flow",
      (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
      (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "interface to fap20v driver scheduler aggregate services.",
      "Examples:\r\n"
      "  fap20v scheduler aggregate set_aggregate 0x1 agg_id 10 hr_mode regular state enabled\r\n"
      "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
      "",
      {0,0,0,0,0, BIT(20) | BIT(21) | BIT(22) | BIT(23) | BIT(16)},
      2,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_FLOW_SET_ID,
      "open",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "sets the scheduler port parameters.\r\n",
      "Examples:\r\n"
      "  fap20v scheduler port set_scheduler_port 0x1 port_id 10 hr_mode regular state enabled\r\n"
      "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
      "",
      {0,0,0,0,0, BIT(20) | BIT(21)},
      3,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_FLOW_UPDATE_ID,
      "update",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Update flow or aggregate unfer traffic.\r\n",
      "Examples:\r\n"
      "  fap20v scheduler port set_scheduler_port 0x1 port_id 10 hr_mode regular state enabled\r\n"
      "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
      "",
      {0,0,0,0,0, BIT(22)},
      3,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_DYN_FLOW_STAT_ID,
      "dynamic_status_get",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "gets the scheduler flow internal status.\r\n",
      "Examples:\r\n"
      "  fap20v scheduler flow dynamic_status_get 0x1 flow_id 1000 nof_subflows 1\r\n"
      "Results in getting the device status of flow 1000.\r\n",
      "",
      {0,0,0,0,0, BIT(16)},
      3,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_DYN_FLOW_ID_ID,
      "flow_id",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "gets the scheduler flow internal status.\r\n",
      "Examples:\r\n"
      "  fap20v scheduler flow dynamic_status_get 0x1 flow_id 1000 nof_subflows 1\r\n"
      "Results in getting the device status of flow 1000.\r\n",
      "",
      {0,0,0,0,0, BIT(16)},
      4,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_DYN_FLOW_NOF_SUB_ID,
      "nof_subflows",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "gets the scheduler flow internal status.\r\n",
      "Examples:\r\n"
      "  fap20v scheduler flow dynamic_status_get 0x1 flow_id 1000 nof_subflows 1\r\n"
      "Results in getting the device status of flow 1000.\r\n",
      "",
      {0,0,0,0,0, BIT(16)},
      5,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_FLOW_GET_ID,
      "get",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "gets the scheduler port parameters.\r\n",
      "Examples:\r\n"
      "  fap20v scheduler port get_scheduler_port 0x1 port_id 10\r\n"
      "Results in getting on device 1 scheduler port 10 parameters.\r\n",
      "",
      {0,0,0,0,0, BIT(23)},
      3,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_FLOW_CLOSE_ID,
      "close",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "gets the scheduler port parameters.\r\n",
      "Examples:\r\n"
      "  fap20v scheduler aggregate close_saggregate 0x1 port_id 10\r\n"
      "Results in getting on device 1 scheduler port 10 parameters.\r\n",
      "",
      {0,0,0,0,0, BIT(23)},
      3,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_FLOW_ID_ID,
      "flow_id",
      (PARAM_VAL_RULES *)&Fap20v_flow_id[0],
      (sizeof(Fap20v_flow_id) / sizeof(Fap20v_flow_id[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Scheduler flow to add/remove. Range: 0-8095.\r\n",
      "Examples:\r\n"
      "  fap20v scheduler port set_scheduler_port 0x1 port_id 10 hr_mode regular state enabled\r\n"
      "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
      "",
      {0,0,0,0,0,  BIT(20) | BIT(21) | BIT(22) | BIT(23)},
      4,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_FLOW_SRC_Q_ID,
      "source_queue",
      (PARAM_VAL_RULES *)&Fap20v_queue_id[0],
      (sizeof(Fap20v_queue_id) / sizeof(Fap20v_queue_id[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "The queue that attached to the flow.",
      "\r\n",
      "",
      {0,0,0,0,0, BIT(20) | BIT(21)},
      5,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_FLOW_SRC_FAP_ID,
      "source_fap",
      (PARAM_VAL_RULES *)&Fap20v_fap_id[0],
      (sizeof(Fap20v_fap_id) / sizeof(Fap20v_fap_id[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "The ingress fap that send the traffic to the flow.",
      "\r\n",
      "",
      {0,0,0,0,0, BIT(20) | BIT(21)},
      6,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },

    {
      PARAM_FAP20V_B_SCH_FLOW_1_TYPE_ID,
      "sub_flow_1",
      (PARAM_VAL_RULES *)&Fap20v_b_sch_agg_type_vals[0],
      (sizeof(Fap20v_b_sch_agg_type_vals) / sizeof(Fap20v_b_sch_agg_type_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Scheduler port to add/remove. Range: 0-3327.\r\n",
      "Examples:\r\n"
      "  fap20v scheduler port set_scheduler_port 0x1 port_id 10 hr_mode regular state enabled\r\n"
      "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
      "",
      {0,0,0,0,0,  BIT(20) | BIT(21)},
      7,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_FLOW_1_CREDIT_SRC_ID,
      "credit_src",
      (PARAM_VAL_RULES *)&Fap20v_sch_agg_id_vals[0],
      (sizeof(Fap20v_sch_agg_id_vals) / sizeof(Fap20v_sch_agg_id_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Scheduler port to add/remove. Range: 0-3327.\r\n",
      "Examples:\r\n"
      "  fap20v scheduler port set_scheduler_port 0x1 port_id 10 hr_mode regular state enabled\r\n"
      "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
      "",
      {0,0,0,0,0,  BIT(20) | BIT(21)},
      8,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_FLOW_SUB_FLOW_2_DISABLE,
      "sub_flow_2_is_disabled",
      (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
      (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "If appears, Only one active sub-flow.",
      "\r\n",
      "",
      {0,0,0,0,0, BIT(20) },
      9,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_FLOW_2_TYPE_ID,
      "sub_flow_2",
      (PARAM_VAL_RULES *)&Fap20v_b_sch_agg_type_vals[0],
      (sizeof(Fap20v_b_sch_agg_type_vals) / sizeof(Fap20v_b_sch_agg_type_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Scheduler port to add/remove. Range: 0-3327.\r\n",
      "Examples:\r\n"
      "  fap20v scheduler port set_scheduler_port 0x1 port_id 10 hr_mode regular state enabled\r\n"
      "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
      "",
      {0,0,0,0,0,  BIT(21)},
      9,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_FLOW_2_CREDIT_SRC_ID,
      "credit_src_2",
      (PARAM_VAL_RULES *)&Fap20v_sch_agg_id_vals[0],
      (sizeof(Fap20v_sch_agg_id_vals) / sizeof(Fap20v_sch_agg_id_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Scheduler port to add/remove. Range: 0-3327.\r\n",
      "Examples:\r\n"
      "  fap20v scheduler port set_scheduler_port 0x1 port_id 10 hr_mode regular state enabled\r\n"
      "Results in setting on device 1 scheduler port 10 with regular hr scheduler.\r\n",
      "",
      {0,0,0,0,0,  BIT(21)},
      10,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_FLOW_1_WEIGHT_ID,
      "sub_flow_1_weight",
      (PARAM_VAL_RULES *)&Fap20v_b_sub_flow_weight_vals[0],
      (sizeof(Fap20v_b_sub_flow_weight_vals) / sizeof(Fap20v_b_sub_flow_weight_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "If appears, Weight for sub-flow 1.",
      "\r\n",
      "",
      {0,0,0,0,0, BIT(20) | BIT(21)},
      LAST_ORDINAL,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_FLOW_2_WEIGHT_ID,
      "sub_flow_2_weight",
      (PARAM_VAL_RULES *)&Fap20v_b_sub_flow_weight_vals[0],
      (sizeof(Fap20v_b_sub_flow_weight_vals) / sizeof(Fap20v_b_sub_flow_weight_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "If appears, Weight for sub-flow 2.",
      "\r\n",
      "",
      {0,0,0,0,0, BIT(21) | BIT(22)},
      LAST_ORDINAL,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },

    {
      PARAM_FAP20V_B_SCH_FLOW_1_SHAPER_RATE_ID,
      "sub_flow_1_shaper_rate",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "If appears, This would be the sub-flow shaper rate.\n\r"
      "  Kilobit per second Resolution",
      "\r\n",
      "",
      {0,0,0,0,0, BIT(20) | BIT(21)},
      LAST_ORDINAL,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_FLOW_2_SHAPER_RATE_ID,
      "sub_flow_2_shaper_rate",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "If appears, This would be the sub-flow shaper rate.\n\r"
      "  Kilobit per second Resolution",
      "\r\n",
      "",
      {0,0,0,0,0, BIT(21)},
      LAST_ORDINAL,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },

    {
      PARAM_FAP20V_B_SCH_FLOW_1_SHAPER_BURST_ID,
      "sub_flow_1_shaper_burst",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "If appears, This would be the sub-flow shaper burst."
      "  Byte Resolution.\r\n",
      "\r\n",
      "",
      {0,0,0,0,0, BIT(20) | BIT(21)},
      LAST_ORDINAL,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_FLOW_2_SHAPER_BURST_ID,
      "sub_flow_2_shaper_burst",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "If appears, This would be the sub-flow shaper burst."
      "  Byte Resolution.\r\n",
      "\r\n",
      "",
      {0,0,0,0,0, BIT(21)},
      LAST_ORDINAL,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_FLOW_1_SLOW_INDEX_ID,
      "sub_flow_1_slow_index",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "B3 Only!!! - Enable choosing between 2 slow rates."
      "0 - first slow rate, 1 - second slow rate\r\n",
      "\r\n",
      "",
      {0,0,0,0,0, BIT(20) | BIT(21)},
      LAST_ORDINAL,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_FLOW_2_SLOW_INDEX_ID,
      "sub_flow_2_slow_index",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "B3 Only!!! - Enable choosing between 2 slow rates."
      "0 - first slow rate, 1 - second slow rate\r\n",
      "\r\n",
      "",
      {0,0,0,0,0, BIT(21)},
      LAST_ORDINAL,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },

    {
      PARAM_FAP20V_B_SCH_FLOW_IS_SLOW_ID,
      "enable_slow",
      (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
      (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "If appears, Slow is enabled.",
      "\r\n",
      "",
      {0,0,0,0,0, BIT(20) | BIT(21)},
      LAST_ORDINAL,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_FLOW_IS_LOW_DELAY_ID,
      "low_delay",
      (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
      (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "If appears, It would be Low Delay Flow.",
      "NOTE: The queue should also be configured as Low Delay.\r\n",
      "",
      {0,0,0,0,0, BIT(20) | BIT(21)},
      LAST_ORDINAL,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_FLOW_END_ID_ID,
      "end_flow_id",
      (PARAM_VAL_RULES *)&Fap20v_flow_id[0],
      (sizeof(Fap20v_flow_id) / sizeof(Fap20v_flow_id[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "If appears, initiailze from 'flow_id' to 'end_flow_id' with the same parameters.\r\n"
      "  This command is usefull when all flows have the same configuration.",
      "Examples:\r\n"
      "  fap20v scheduler port set_scheduler_port 0x1 port_id 0 hr_mode regular state enabled end_port_id 11\r\n"
      "Results in setting on device 1 scheduler ports 0-11\r\n"
      " with regular hr scheduler.\r\n",
      "",
      {0,0,0,0,0, BIT(20) | BIT(21)| BIT(23)},
      LAST_ORDINAL,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_SUB_FLOW_ID_ID,
      "sub_flow_id",
      (PARAM_VAL_RULES *)&Fap20v_flow_id[0],
      (sizeof(Fap20v_flow_id) / sizeof(Fap20v_flow_id[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Scheduler sub flow to update. Range: 0-16191.\r\n",
      "",
      "",
      {0,0,0,0,0,  BIT(22) },
      5,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_UPDATE_TYPE_ID,
      "update_type",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "0: Enable/Disable Slow, 1:Update shaper rate.\r\n"
      "2: Update shaper Max Burst, 3: Update weight.\r\n"
      "5: Update slow rate index",
      "",
      "",
      {0,0,0,0,0,  BIT(22) },
      6,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_UPDATE_VALUE_ID,
      "update_value",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "The value to be written to the update_type field.\r\n",
      "",
      "",
      {0,0,0,0,0,  BIT(22) },
      7,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
  /*
  * } flow
  */
  /*
  * { Device
  */

    {
      PARAM_FAP20V_B_SCH_DEV_RATE_ENTRY_SET_ID,
      "device_rate_set",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Set entry / entries in the device rate table, per RCI level and \n\r"
      " number of active links.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(24) },
      2,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_DEV_RATE_ENTRY_GET_ID,
      "device_rate_get",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Get entry / entries in the device rate table, per RCI level and \n\r"
      " number of active links.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(25)},
      2,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_DEV_RATE_ALL_TABLE_ID,
      "device_scheduler_get",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Get all the DRM table",
      "For example\r\n"
      "fap20v scheduler device_rate_get_all_table 0",
      "",
      {0,0,0,0,0, BIT(29) },
      2,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_PRINT_FLOW_UP_ID,
      "print_flow_and_up_cfg",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Print the flow, and all the scheduler levels above it",
      "For example\r\n"
      "fap20v scheduler print_flow_and_up 0 flow_id 3344",
      "",
      {0,0,0,0,0, BIT(19) },
      2,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_PRINT_FLOW_ID,
      "flow_id",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Print the flow, and all the scheduler levels above it",
      "For example\r\n"
      "fap20v scheduler print_flow_and_up 0 flow_id 3344",
      "",
      {0,0,0,0,0, BIT(19) },
      3,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_FLOW_UP_STAT_ID,
      "print_status",
      (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
      (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Print the flow, and all the scheduler levels above it",
      "For example\r\n"
      "fap20v scheduler print_flow_and_up 0 flow_id 3344",
      "",
      {0,0,0,0,0, BIT(19) },
      4,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_DEV_RCI_LEVEL_ID,
      "rci_level",
      (PARAM_VAL_RULES *)&Fap20v_rci_level_vals[0],
      (sizeof(Fap20v_rci_level_vals) / sizeof(Fap20v_rci_level_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "RCI level, which is an indication of the fabric congestion level",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(24) | BIT(25) },
      3,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_DEV_LINKS_NUM_ID,
      "num_active_links",
      (PARAM_VAL_RULES *)&Fap20v_nof_active_links_vals[0],
      (sizeof(Fap20v_nof_active_links_vals) / sizeof(Fap20v_nof_active_links_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "The number of active links that connect this device to the fabric",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(24) | BIT(25) },
      4,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_DEV_RATE_ID,
      "rate",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "The credit generation rate, in Mega-Bit-Sec.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(24)},
      5,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_DEV_RCI_LEVEL_END_ID,
      "up_to_rci_level",
      (PARAM_VAL_RULES *)&Fap20v_rci_level_vals[0],
      (sizeof(Fap20v_rci_level_vals) / sizeof(Fap20v_rci_level_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "To modify more than one ontry in the table.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(24) | BIT(25) },
      LAST_ORDINAL,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_DEV_LINKS_NUM_END_ID,
      "up_to_num_active_links",
      (PARAM_VAL_RULES *)&Fap20v_nof_active_links_vals[0],
      (sizeof(Fap20v_nof_active_links_vals) / sizeof(Fap20v_nof_active_links_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "To modify more than one ontry in the table.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(24) | BIT(25) },
      LAST_ORDINAL,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
  /*
  * } Device
  */
  /*
  * { SLOW RATE
  */
    {
      PARAM_FAP20V_B_SCH_SLOW_RATE_SET_ID,
      "slow_rate_set",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "This function sets the slow rate.\n\r"
      "A flow might be in slow state, and in that case lower\n\r"
      "rate is needed.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(17) },
      2,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_SLOW_RATE_GET_ID,
      "slow_rate_get",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "This function gets the slow rate.\n\r"
      "A flow might be in slow state, and in that case lower\n\r"
      "rate is needed.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(18) },
      2,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_SLOW_RATE_VAL_ID,
      "rate",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      " Maximal rate for slow flows Kbps.",
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(17) },
      3,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_SLOW_SCND_RATE_VAL_ID,
      "second_rate",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      " Maximal rate for slow flows Kbps.",
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(17) },
      4,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
  /*
  * } SLOW RATE
  */
  /*
  * { HR_MODE
  */
    {
      PARAM_FAP20V_B_SCH_HR_MODE_SET_ID,
      "hr_mode_set",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "The HR support two mode: single, and either dual or enhanced.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(26) },
      2,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_HR_MODE_GET_ID,
      "hr_mode_get",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "The HR support two mode: single, and either dual or enhanced.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(27) },
      2,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_HR_MODE_TYPE_ID,
      "supported_mode",
      (PARAM_VAL_RULES *)&Fap20v_b_supported_mode_vals[0],
      (sizeof(Fap20v_b_supported_mode_vals) / sizeof(Fap20v_b_supported_mode_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "The HR support two mode: single, and either dual or enhanced.",
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(26) },
      3,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
  /*
  * } HR_MODE
  */
  /*
  * { Device Interfaces.
  */
    {
      PARAM_FAP20V_B_SCH_DEV_IF_SET_ID,
      "device_interfaces_set",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Set the device interfaces scheduler.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(28) },
      2,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_DEV_IF_GET_ID,
      "device_interfaces_get",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Get the device interfaces scheduler.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(27) },
      2,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_DEV_IF_SPI_A_RATE_ID,
      "spi_a_max_rate",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Max rate for SPI A interface.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(28)},
      3,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_DEV_IF_SPI_B_RATE_ID,
      "spi_b_max_rate",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Max rate for SPI B interface.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(28)},
      4,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_DEV_IF_CPU_RATE_ID,
      "cpu_max_rate",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Max rate for CPU interface.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(28)},
      5,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_DEV_IF_RCY_RATE_ID,
      "recycling_max_rate",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Max rate for RCY interface.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(28)},
      6,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_DEV_IF_SPI_A_WT_ID,
      "spi_a_weight",
      (PARAM_VAL_RULES *)&Fap20v_device_if_vals_vals[0],
      (sizeof(Fap20v_device_if_vals_vals) / sizeof(Fap20v_device_if_vals_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "SPI A weight.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(28) },
      7,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_DEV_IF_SPI_B_WT_ID,
      "spi_b_weight",
      (PARAM_VAL_RULES *)&Fap20v_device_if_vals_vals[0],
      (sizeof(Fap20v_device_if_vals_vals) / sizeof(Fap20v_device_if_vals_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "SPI B weight.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(28) },
      8,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_DEV_IF_RCY_WT_ID,
      "rcy_weight",
      (PARAM_VAL_RULES *)&Fap20v_device_if_vals_vals[0],
      (sizeof(Fap20v_device_if_vals_vals) / sizeof(Fap20v_device_if_vals_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Recycling scheduler weight.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(28) },
      9,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
 /*
  * } Device Interfaces.
  */

 /*
  * { CL Class params.
 */
    {
      PARAM_FAP20V_B_SCH_CL_TYPE_SET_ID,
      "cl_types_set",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Set a CL class type parameters.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(10) | BIT(11) |BIT(12) | BIT(13) | BIT(14) },
      2,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_CL_TYPE_GET_ID,
      "cl_types_get",
      (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
      (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "Get a CL class type parameters.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(9) },
      2,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_CL_TYPE_ID_ID,
      "type_id",
      (PARAM_VAL_RULES *)&Fap20v_b_cl_type_id_vals[0],
      (sizeof(Fap20v_b_cl_type_id_vals) / sizeof(Fap20v_b_cl_type_id_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "The ID of the CL class type, to be configured.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(9) | BIT(10) | BIT(11) |BIT(12) | BIT(13) | BIT(14) },
      3,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_CL_TYPE_MODE_ID,
      "mode",
      (PARAM_VAL_RULES *)&Fap20v_b_type_mode_vals[0],
      (sizeof(Fap20v_b_type_mode_vals) / sizeof(Fap20v_b_type_mode_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "The CL class type mode.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(10) | BIT(11) |BIT(12) | BIT(13) | BIT(14) },
      4,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_CL_TYPE_W_MODE_ID,
      "weight_mode",
      (PARAM_VAL_RULES *)&Fap20v_b_weight_mode_vals[0],
      (sizeof(Fap20v_b_weight_mode_vals) / sizeof(Fap20v_b_weight_mode_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "The CL class type, weight mode.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(10) | BIT(11) |BIT(12) | BIT(13) | BIT(14) },
      5,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_CL_TYPE_W_1_ID,
      "discrete_weight_1",
      (PARAM_VAL_RULES *)&Fap20v_b_discrete_weight_vals[0],
      (sizeof(Fap20v_b_discrete_weight_vals) / sizeof(Fap20v_b_discrete_weight_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "The CL class type, weight mode.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(11) |BIT(12) | BIT(13) | BIT(14) },
      6,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_CL_TYPE_W_2_ID,
      "discrete_weight_2",
      (PARAM_VAL_RULES *)&Fap20v_b_discrete_weight_vals[0],
      (sizeof(Fap20v_b_discrete_weight_vals) / sizeof(Fap20v_b_discrete_weight_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "The CL class type, weight mode.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(12) | BIT(13) | BIT(14) },
      7,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_CL_TYPE_W_3_ID,
      "discrete_weight_3",
      (PARAM_VAL_RULES *)&Fap20v_b_discrete_weight_vals[0],
      (sizeof(Fap20v_b_discrete_weight_vals) / sizeof(Fap20v_b_discrete_weight_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "The CL class type, weight mode.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(13) | BIT(14) },
      8,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FAP20V_B_SCH_CL_TYPE_W_4_ID,
      "discrete_weight_4",
      (PARAM_VAL_RULES *)&Fap20v_b_discrete_weight_vals[0],
      (sizeof(Fap20v_b_discrete_weight_vals) / sizeof(Fap20v_b_discrete_weight_vals[0])) - 1,
      HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "The CL class type, weight mode.",
      "For example\r\n"
      "fap20v scheduler ",
      "",
      {0,0,0,0,0, BIT(14) },
      9,
        /*
         * Pointer to a function to call after symbolic parameter
         * has been accepted.
         */
      (VAL_PROC_PTR)NULL
    },


 /*
  * } CL Class params.
  */

  /* END SOC_SAND_FAP20V B Scheduler } */

  /* { fap20v_api_spatial_multicast.h interface */
  {
    PARAM_FAP20V_MULTICAST_API_ID,
    "multicast",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver spatial multicast services.",
    "Examples:\r\n"
    "  fap20v multicast spatial set_sms_conf 0x1 max_bandwidth 100 max_sm_percent 2\r\n"
    "Results in setting on device 1 spatial multicast scheduler to:\r\n"
    "100 MegbitSec credit - Maximal bandwidth to assign to spatial\r\n"
    "multicast packets, and with 2% maximal Spatial multicast traffic percentage\r\n"
    "from rate assigned to all ingress queues.\r\n",
    "",
    {0,
     BIT(3) | BIT(4) | BIT(5) | BIT(6) | BIT(17) | BIT(18) | BIT(19) | BIT(20) | BIT(21),
     BIT(20) | BIT(21) | BIT(22) | BIT(23) | BIT(24),
     BIT(6) },
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SPATIAL_MULTICAST_API_ID,
    "spatial",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver spatial multicast services.",
    "Examples:\r\n"
    "  fap20v multicast spatial set_sms_conf 0x1 max_bandwidth 100 max_sm_percent 2\r\n"
    "Results in setting on device 1 spatial multicast scheduler to:\r\n"
    "100 MegbitSec credit - Maximal bandwidth to assign to spatial\r\n"
    "multicast packets, and with 2% maximal Spatial multicast traffic percentage\r\n"
    "from rate assigned to all ingress queues.\r\n",
    "",
    {0,
     BIT(3) | BIT(4) | BIT(5) | BIT(6)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_CLOSE_SPATIAL_MULTICAST_FLOW_ID,
    "close_flow",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "close spatial multicast flow at the ingress FAP.\r\n",
    "Examples:\r\n"
    "  fap20v multicast spatial close_flow 0x1 group_id 1500 \r\n"
    "Results in closing on device 1 spatial multicast group 1500.",
    "",
    {0,
     BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_GET_SPATIAL_MULTICAST_FLOW_PARAMS_ID,
    "get_flow_params",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "get spatial multicast flow parameters at the ingress FAP.\r\n",
    "Examples:\r\n"
    "  fap20v multicast spatial get_flow_params 0x1 group_id 1500 \r\n"
    "Results in getting on device 1 spatial multicast group parameters.",
    "",
    {0,
     BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_OPEN_SPATIAL_MULTICAST_FLOW_ID,
    "open_flow",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "open spatial multicast flow at the ingress FAP.\r\n",
    "Examples:\r\n"
    "  fap20v multicast spatial open_flow 0x1 group_id 1500 queue 2\r\n"
    "Results in opening on device 1 spatial multicast flow with parameters:\r\n"
    "spatial multicast identifier 1500 and queue 2.",
    "",
    {0,
     BIT(5)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_UPDATE_SPATIAL_MULTICAST_FLOW_ID,
    "update_flow",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "update spatial multicast flow at the ingress FAP.\r\n",
    "Examples:\r\n"
    "  fap20v multicast spatial update_flow 0x1 group_id 1500 queue 2\r\n"
    "Results in updating on device 1 spatial multicast flow to parameters:\r\n"
    "spatial multicast identifier 1500 and queue 2.",
    "",
    {0,
     BIT(5)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SPATIAL_MULTICAST_NUM_ID,
    "group_id",
    (PARAM_VAL_RULES *)&Fap20v_spatial_multicast_id[0],
    (sizeof(Fap20v_spatial_multicast_id) / sizeof(Fap20v_spatial_multicast_id[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "spatial multicast group identifier.\r\n",
    "Examples:\r\n"
    "  fap20v multicast spatial open_flow 0x1 group_id 1500 queue 2\r\n"
     "Results in opening on device 1 spatial multicast flow with parameters:\r\n"
     "spatial multicast identifier 1500 and queue 2.",
    "",
    {0,
     BIT(5) | BIT(6)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SPATIAL_TYPE_ID,
    "queue",
    (PARAM_VAL_RULES *)&Fap20v_spatial_multicast_queue[0],
    (sizeof(Fap20v_spatial_multicast_queue) / sizeof(Fap20v_spatial_multicast_queue[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Spatial multicast queue. One of 4 spatial queues, to store flow packets.\n\r"
    "  Packet destinate to queue number 3 have absolute priority for credit.\r\n",
    "Examples:\r\n"
    "  fap20v multicast spatial open_sm_flow 0x1 group_id 1500 queue 2\r\n"
    "Results in opening on device 1 spatial multicast flow with parameters:\r\n"
    "spatial multicast identifier 1500 and queue 2.",
    "",
    {0,
     BIT(5)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_GET_SMS_CONFIGURATION_ID,
    "get_sms_conf",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "gets Spatial Multicast Scheduler parameters from the device.\r\n",
    "Examples:\r\n"
    "  fap20v multicast spatial get_sms_conf 0x1 \r\n"
    "Results in getting on device 1 spatial multicast scheduler parameters.\r\n",
    "",
    {0,
     BIT(4)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_SET_SMS_CONFIGURATION_ID,
    "set_sms_conf",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "sets Spatial Multicast Scheduler parameters at the device.\r\n",
    "Examples:\r\n"
    "  fap20v multicast spatial set_sms_conf 0 guaranteed_rate 300 best_effort_rate 1000\r\n"
    "  guaranteed_max_burst 10 best_effort_max_burst 5\r\n"
    "Results in setting on device 0 spatial multicast scheduler to:\r\n"
    "300 MegbitSec Max bandwidth for guaranteed multicast traffic\r\n"
    "When the max bust size is 10 KBytes and\r\n"
    "1 GigabitSec Max bandwidth for best effort multicast traffic\r\n"
    "When the max bust size is 5 KBytes\r\n",
    "",
    {0,
     BIT(3)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SMS_GR_RATE_ID,
    "guaranteed_rate",
    (PARAM_VAL_RULES *)&Fap20v_sms_max_bandwidth[0],
    (sizeof(Fap20v_sms_max_bandwidth) / sizeof(Fap20v_sms_max_bandwidth[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Max bandwidth for guaranteed multicast traffic.\r\n"
    "  Resolution of MegbitSec.\n\r",
    "Examples:\r\n"
    "  fap20v multicast spatial set_sms_conf 0 guaranteed_rate 300 best_effort_rate 1000\r\n"
    "  guaranteed_max_burst 10 best_effort_max_burst 5\r\n"
    "Results in setting on device 0 spatial multicast scheduler to:\r\n"
    "300 MegbitSec Max bandwidth for guaranteed multicast traffic\r\n"
    "When the max bust size is 10 KBytes and\r\n"
    "1 GigabitSec Max bandwidth for best effort multicast traffic\r\n"
    "When the max bust size is 5 KBytes\r\n",
    "",
    {0,
     BIT(3)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SMS_BE_RATE_ID,
    "best_effort_rate",
    (PARAM_VAL_RULES *)&Fap20v_sms_max_bandwidth[0],
    (sizeof(Fap20v_sms_max_bandwidth) / sizeof(Fap20v_sms_max_bandwidth[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Max bandwidth for best effort multicast traffic.\r\n"
    "  Resolution of MegbitSec.\n\r",
    "Examples:\r\n"
    "  fap20v multicast spatial set_sms_conf 0 guaranteed_rate 300 best_effort_rate 1000\r\n"
    "  guaranteed_max_burst 10 best_effort_max_burst 5\r\n"
    "Results in setting on device 0 spatial multicast scheduler to:\r\n"
    "300 MegbitSec Max bandwidth for guaranteed multicast traffic\r\n"
    "When the max bust size is 10 KBytes and\r\n"
    "1 GigabitSec Max bandwidth for best effort multicast traffic\r\n"
    "When the max bust size is 5 KBytes\r\n",
    "",
    {0,
     BIT(3)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_B_SMS_GR_BRST_ID,
    "guaranteed_max_burst",
    (PARAM_VAL_RULES *)&Fap20v_sms_max_burst_size[0],
    (sizeof(Fap20v_sms_max_burst_size) / sizeof(Fap20v_sms_max_burst_size[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Max burst size for guaranteed multicast traffic.\r\n"
    "  Resolution of Kilo Bytes.\n\r",
    "Examples:\r\n"
    "  fap20v multicast spatial set_sms_conf 0 guaranteed_rate 300 best_effort_rate 1000\r\n"
    "  guaranteed_max_burst 10 best_effort_max_burst 5\r\n"
    "Results in setting on device 0 spatial multicast scheduler to:\r\n"
    "300 MegbitSec Max bandwidth for guaranteed multicast traffic\r\n"
    "When the max bust size is 10 KBytes and\r\n"
    "1 GigabitSec Max bandwidth for best effort multicast traffic\r\n"
    "When the max bust size is 5 KBytes\r\n",
    "",
    {0,
     BIT(3)},
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
   {
    PARAM_FAP20V_B_SMS_BE_BRST_ID,
    "best_effort_max_burst",
    (PARAM_VAL_RULES *)&Fap20v_sms_max_burst_size[0],
    (sizeof(Fap20v_sms_max_burst_size) / sizeof(Fap20v_sms_max_burst_size[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Max burst size for best effort multicast traffic.\r\n"
    "  Resolution of Kilo Bytes.\n\r",
    "Examples:\r\n"
    "  fap20v multicast spatial set_sms_conf 0 guaranteed_rate 300 best_effort_rate 1000\r\n"
    "  guaranteed_max_burst 10 best_effort_max_burst 5\r\n"
    "Results in setting on device 0 spatial multicast scheduler to:\r\n"
    "300 MegbitSec Max bandwidth for guaranteed multicast traffic\r\n"
    "When the max bust size is 10 KBytes and\r\n"
    "1 GigabitSec Max bandwidth for best effort multicast traffic\r\n"
    "When the max bust size is 5 KBytes\r\n",
    "",
    {0,
     BIT(3)},
    7,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* END fap20v_api_spatial_multicast.h interface } */
  /* { fap20v_api_egress_multicast.h interface */
  {
    PARAM_FAP20V_EGRESS_MULTICAST_API_ID,
    "egress",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "interface to fap20v driver egress multicast services.",
    "Examples:\r\n"
    "  fap20v multicast egress open_group 0 multicast_id 100 port_copy_list_a 0 1 2 3\r\n"
    "Results in opening egress multicast group 100 with:\n\r"
    "  + port 0, 1 copy\n\r"
    "  + port 2, 3 copies\r\n",
    "",
    {0,
     0,
     BIT(20) /*open*/| BIT(21) /*get*/| BIT(22) /*close*/| BIT(23) /*add-port*/| BIT(24) /*delete-port*/,
     BIT(6) /*denug EG-FM*/},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_DEBUG_API_ID,
    "debug",
    (PARAM_VAL_RULES *)&Fap20v_empty_vals[0],
    (sizeof(Fap20v_empty_vals) / sizeof(Fap20v_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Egress Multicast debug functions.\r\n",
    "Examples:\r\n"
    "  fap20v multicast egress debug print_fm_table 0\r\n"
    "Results in printing for debug the FOP-Multicast-LUT0/1/2 table of the EGQ\n\r",
    "",
    {0,
     0,
     0,
     BIT(6) },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_EG_PRINT_EGQ_FM_ID,
    "print_fm_table",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Egress Multicast debug functions.\r\n",
    "Examples:\r\n"
    "  fap20v multicast egress debug print_fm_table 0\r\n"
    "Results in printing for debug the FOP-Multicast-LUT0/1/2 table of the EGQ\n\r"
    "This is not API function only for debug\n\r",
    "",
    {0,
     0,
     0,
     BIT(6) },
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_EG_PRINT_EGQ_FM_USAGE_ID,
    "get_fm_usage",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Egress Multicast debug functions.\r\n",
    "Examples:\r\n"
    "  fap20v multicast egress denug get_fm_usage 0\r\n"
    "Results in geting the EGQ table FOP-Multicast-LUT0/1/2 usage.\n\r"
    "  That is, how many LinkList entries and how may BitMap entries."
    "This is not API function only for debug\n\r",
    "",
    {0,
     0,
     0,
     BIT(6)},
     4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_EG_VALIDATE_EGQ_FM_ID,
    "validate_fm",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Egress Multicast debug functions.\r\n",
    "Examples:\r\n"
    "  fap20v multicast egress denug validate_fm 0\r\n"
    "Results in validating the EGQ table FOP-Multicast-LUT0/1/2.\n\r"
    "  That is, do some validation function on the FOP-Multicast-LUT0/1/2."
    "This is not API function only for debug\n\r",
    "",
    {0,
     0,
     0,
     BIT(6)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_EG_TEST_ID,
    "test",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Egress Multicast debug functions.\r\n",
    "Examples:\r\n"
    "  fap20v multicast egress denug test\r\n"
    "Results in do some directed test on the egress multicast module.\n\r"
    "  That is, directed test for egress multicast of the link-list.\n\r"
    "This is not API function only for debug\n\r",
    "",
    {0,
     0,
     0,
     BIT(6)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_EG_TEST_DIRECT_ID,
    "directed_test_ll",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Egress Multicast debug functions.\r\n",
    "Examples:\r\n"
    "  fap20v multicast egress denug directed_test_ll\r\n"
    "Results in do some directed test on the egress multicast module.\n\r"
    "  That is, directed test for egress multicast of the link-list.\n\r"
    "This is not API function only for debug\n\r",
    "",
    {0,
     0,
     0,
     BIT(6)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_EG_MULTICAST_OPEN_GROUP_ID,
    "open_group",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Per multicast_id group, set of Data-Ports and number of copies per Data-Ports.\r\n",
    "Examples:\r\n"
    "  fap20v multicast egress open_group 0 multicast_id 100 port_copy_list_a 0 1 2 3\r\n"
    "Results in opening egress multicast group 100 with:\n\r"
    "  + port 0, 1 copy\n\r"
    "  + port 2, 3 copies\r\n",
    "",
    {0,
     0,
     BIT(20),
     0 },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_MULTICAST_ID_ID,
    "multicast_id",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Multicast Group Id to manipulate. 0 - (8K-1)\r\n",
    "Examples:\r\n"
    "  fap20v multicast egress open_group 0 multicast_id 100 port_copy_list_a 0 1 2 3\r\n"
    "Results in opening egress multicast group 100 with:\n\r"
    "  + port 0, 1 copy\n\r"
    "  + port 2, 3 copies\r\n",
    "",
    {0,
     0,
     BIT(20) /*open*/| BIT(21) /*get*/| BIT(22) /*close*/| BIT(23) /*add-port*/| BIT(24) /*delete-port*/,
     0 },
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_EG_MULTICAST_COPY_PORT_LIST_A_ID,
    "port_copy_list_a",
    (PARAM_VAL_RULES *)&Fap20v_port_and_copy_list[0],
    (sizeof(Fap20v_port_and_copy_list) / sizeof(Fap20v_port_and_copy_list[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "List of ports and copies (per port). Up to 32 ports.\r\n",
    "  fap20v multicast egress open_group 0 multicast_id 100 port_copy_list_a 0 1 2 3\r\n"
    "Results in opening egress multicast group 100 with:\n\r"
    "  + port 0, 1 copy\n\r"
    "  + port 2, 3 copies\r\n",
    "",
    {0,
     0,
     BIT(20) /*open*/ | BIT(23) /*add-port*/,
     0 },
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_EG_MULTICAST_COPY_PORT_LIST_B_ID,
    "port_copy_list_b",
    (PARAM_VAL_RULES *)&Fap20v_port_and_copy_list[0],
    (sizeof(Fap20v_port_and_copy_list) / sizeof(Fap20v_port_and_copy_list[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "List of ports and copies (per port). Up to 32 ports.\r\n",
    "  fap20v multicast egress open_group 0 multicast_id 100 port_copy_list_a 0 1 2 3 port_copy_list_b 45 127\r\n"
    "Results in opening egress multicast group 100 with:\n\r"
    "  + port 0, 1 copy\n\r"
    "  + port 2, 3 copies\r\n"
    "  + port 45, 127 copies\r\n",
    "",
    {0,
     0,
     BIT(20) /*open*/ | BIT(23) /*add-port*/,
     0 },
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_FAP20V_EG_MULTICAST_GET_GROUP_ID,
    "get_group",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Per multicast_id group, get of Data-Ports and number of copies per Data-Ports.\r\n",
    "Examples:\r\n"
    "  fap20v multicast egress get_group 0 multicast_id 100\r\n"
    "Results in getting and printing egress multicast group 100.",
    "",
    {0,
     0,
     BIT(21),
     0 },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_EG_MULTICAST_CLOSE_GROUP_ID,
    "close_group",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Close, per 'multicast_id', the logical multicast group at the egress.\r\n",
    "Examples:\r\n"
    "  fap20v multicast egress close_group 0 multicast_id 100\r\n"
    "Results in closing egress multicast group 100.",
    "",
    {0,
     0,
     BIT(22),
     0 },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_EG_MULTICAST_ADD_PORT_ID,
    "add_ports",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Add/modify port member of the Egress-Multicast.\r\n",
    "Examples:\r\n"
    "  fap20v multicast egress add_ports 0 multicast_id 100 port_copy_list_a 5 6 7 8 9 10\r\n"
    "Results in adding to egress multicast group 100\n\r"
    "  port 5, 6 copies\n\r"
    "  port 7, 8 copies\n\r"
    "  port 9, 10 copies\n\r",
    "",
    {0,
     0,
     BIT(23),
     0 },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_EG_MULTICAST_DELETE_PORT_ID,
    "delete_port",
    (PARAM_VAL_RULES *)&Fap20v_indefinite_num_vals[0],
    (sizeof(Fap20v_indefinite_num_vals) / sizeof(Fap20v_indefinite_num_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Delete port member of the Egress-Multicast.\r\n",
    "Examples:\r\n"
    "  fap20v multicast egress delete_ports 0 multicast_id 100 port_id 0\r\n"
    "Results in deleting from egress multicast group 100 port 0\n\r",
    "",
    {0,
     0,
     BIT(24),
     0 },
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FAP20V_PORT_ID_ID,
    "port_id",
    (PARAM_VAL_RULES *)&Fap20v_max_nof_ports[0],
    (sizeof(Fap20v_max_nof_ports) / sizeof(Fap20v_max_nof_ports[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Port id to delete from the Egress-Multicast.\r\n",
    "Examples:\r\n"
    "  fap20v multicast egress delete_ports 0 multicast_id 100 port_id 0\r\n"
     "Results in deleting from egress multicast group 100 port 0\n\r",
    "",
    {0,
     0,
     BIT(24),
     0 },
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  /* END fap20v_api_egress_multicast.h interface } */

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

/*
 * }
 */
/*
#undef CONST
*/
/* } */
#endif
