/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UI_ROM_DEFI_GSA_PETRA_API_INCLUDED__
/* { */
#define __UI_ROM_DEFI_GSA_PETRA_API_INCLUDED__

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <appl/dpp/UserInterface/ui_pure_defi_gsa_petra_api.h>
#include <appl/diag/dpp/gsa_api_multicast2.h>

#if !BCM_DUNE
#else
#include <appl/dpp/sweep/Petra/swp_p_tm_sch_schemes.h>
#endif

#define UI_MULTICAST
#define UI_SCHEMES
#define UI_LINKS

 /*
  * ENUM RULES
  * {
  */
/********************************************************/
#ifndef CONST
#define CONST const
#endif
/********************************************************/


/********************************************************/

EXTERN CONST
   PARAM_VAL_RULES
     GSA_PETRA_MC_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "all_auto",
    {
      {
        GSA_P_MC_TYPE_ALL_AUTO,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  GSA_PETRA_MC_TYPE.GSA_P_MC_TYPE_ALL_AUTO:\r\n"
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
        GSA_P_MC_TYPE_EGR_AUTO,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  GSA__MC_TYPE.GSA_P_MC_TYPE_EGR_AUTO:\r\n"
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
        GSA_P_MC_TYPE_EGR_VLAN,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  GSA_PETRA_MC_TYPE.GSA_P_MC_TYPE_EGR_VLAN:\r\n"
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
        GSA_P_MC_TYPE_EGR_GEN,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  GSA_PETRA_MC_TYPE.GSA_P_MC_TYPE_EGR_GEN:\r\n"
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
        GSA_P_MC_TYPE_INGR_AUTO,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  GSA_PETRA_MC_TYPE.GSA_P_MC_TYPE_INGR_AUTO:\r\n"
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
        GSA_P_MC_TYPE_INGR_DEV_SCHED,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  GSA_PETRA_MC_TYPE.GSA_P_MC_TYPE_INGR_DEV_SCHED:\r\n"
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
        GSA_P_MC_TYPE_INGR_PORT_SCHED,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  GSA_PETRA_MC_TYPE.GSA_P_MC_TYPE_INGR_PORT_SCHED:\r\n"
        "  Ingress multicast, Device-Level-Scheduled (set of destination system ports).\r\n"
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
        GSA_P_MC_TYPE_INGR_FABRIC,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  GSA_PETRA_MC_TYPE.GSA_P_MC_TYPE_INGR_FABRIC:\r\n"
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

 /*
  * } ENUM RULES
  */
/********************************************************/
/********************************************************/


EXTERN CONST
   PARAM_VAL_RULES
     gsa_petra_api_members_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,
    SYMB_NAME_NUMERIC,
    {
      {
        HAS_NO_MIN_VALUE | HAS_NO_MAX_VALUE,
        0xff,
        0,
        80,
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
     gsa_petra_api_free_vals[]
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
     gsa_petra_api_empty_vals[]
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
     GSA_PETRA_SCH_SCHEMES_HR_CLASS_rule[]
#ifdef INIT
   =
{
#if !DUNE_BCM
  {
    VAL_SYMBOL,
    "class_1",
    {
      {
        SWP_P_TM_SCH_FLOW_HR_CLASS_1,
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
        SWP_P_TM_SCH_FLOW_HR_CLASS_2,
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
        SWP_P_TM_SCH_FLOW_HR_CLASS_3_WFQ,
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
        SWP_P_TM_SCH_FLOW_HR_CLASS_4_WFQ,
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
        SWP_P_TM_SCH_FLOW_HR_CLASS_5,
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
        SWP_P_TM_SCH_FLOW_HR_CLASS_6,
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
        SWP_P_TM_SCH_FLOW_HR_CLASS_7,
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
        SWP_P_TM_SCH_FLOW_HR_CLASS_8_WFQ,
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
        SWP_P_TM_SCH_FLOW_HR_CLASS_9_WFQ,
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
        SWP_P_TM_SCH_FLOW_HR_CLASS_BE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "",
        (long)NULL
      }
    }
  },
#endif /* !DUNE_BCM  */
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

/********************************************************/

EXTERN CONST
   PARAM_VAL_RULES
     GSA_PETRA_SCH_SCHEMES_PORT_NDX_rule[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        GSA_MAX_NOF_SYS_PORTS - 1, 0,
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
  {
    VAL_SYMBOL,
    "all",
    {
      {
        GSA_MAX_NOF_SYS_PORTS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  GSA_MAX_NOF_SYS_PORTS:\r\n"
        "  All the ports. \r\n"
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
   PARAM_VAL_RULES
     GSA_PETRA_SCH_SCHEMES_CLS_NDX_rule[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        SOC_PETRA_NOF_TRAFFIC_CLASSES - 1, 0,
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
  {
    VAL_SYMBOL,
    "all",
    {
      {
        SOC_PETRA_NOF_TRAFFIC_CLASSES,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_PETRA_NOF_TRAFFIC_CLASSES:\r\n"
        "  All the traffic classes. \r\n"
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

/********************************************************/
EXTERN CONST
   PARAM_VAL_RULES
     GSA_PETRA_MC_GRP_MEMBERS_rule[]
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
     GSA_PETRA_PER_MEMBER_CUD_rule[]
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

/********************************************************/
EXTERN CONST
   PARAM
     gsa_petra_api_params[]
#ifdef INIT
   =
{

#ifdef UI_MULTICAST/* { multicast*/
  {
    PARAM_GSA_PETRA_MC_GRP_OPEN_MC_GRP_OPEN_ID,
    "mc_grp_open",
    (PARAM_VAL_RULES *)&gsa_petra_api_empty_vals[0],
    (sizeof(gsa_petra_api_empty_vals) / sizeof(gsa_petra_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Open Multicast Group.\r\n"
    "",
#if UI_GSA_PETRA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_petra_api multicast gsa_petra_mc_grp_open mc_id 0 mc_type 0 mc_grp_size 0\r\n"
    "  mc_grp_members 0 per_member_cud 0\r\n"
#endif
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
    PARAM_GSA_PETRA_MC_GRP_OPEN_MC_GRP_OPEN_MC_ID_ID,
    "mc_id",
    (PARAM_VAL_RULES *)&gsa_petra_api_free_vals[0],
    (sizeof(gsa_petra_api_free_vals) / sizeof(gsa_petra_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_id:\r\n"
    "  The index of the multicast group to open. Range: 0 - 16K-1.\r\n"
    "",
#if UI_GSA_PETRA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_petra_api multicast gsa_petra_mc_grp_open mc_id 0 mc_type 0 mc_grp_size 0\r\n"
    "  mc_grp_members 0 per_member_cud 0\r\n"
#endif
    "",
    "",
    {BIT(0), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_PETRA_MC_GRP_OPEN_MC_GRP_OPEN_MC_TYPE_ID,
    "mc_type",
    (PARAM_VAL_RULES *)&GSA_PETRA_MC_TYPE_rule[0],
    (sizeof(GSA_PETRA_MC_TYPE_rule) / sizeof(GSA_PETRA_MC_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_type:\r\n"
    "  The type of the multicast to open.\r\n"
    "",
#if UI_GSA_PETRA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_petra_api multicast gsa_petra_mc_grp_open mc_id 0 mc_type 0 mc_grp_size 0\r\n"
    "  mc_grp_members 0 per_member_cud 0\r\n"
#endif
    "",
    "",
    {BIT(0), 0, 0, 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_PETRA_MC_GRP_OPEN_MC_GRP_OPEN_MC_GRP_MEMBERS_ID,
    "mc_grp_members",
    (PARAM_VAL_RULES *)&GSA_PETRA_MC_GRP_MEMBERS_rule[0],
    (sizeof(GSA_PETRA_MC_GRP_MEMBERS_rule) / sizeof(GSA_PETRA_MC_GRP_MEMBERS_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_grp_members:\r\n"
    "  A list of multicast group members;\r\n"
    "  Destination Physical System Ports.\r\n"
    "  The size of the array is mc_grp_size.\r\n"
    "  Note: for GSA_P_MC_TYPE_INGR_FABRIC, this field is ignored\r\n"
    "  and may be set to NULL.\r\n"
    "",
#if UI_GSA_PETRA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_petra_api multicast gsa_petra_mc_grp_open mc_id 0 mc_type 0 mc_grp_size 0\r\n"
    "  mc_grp_members 0 per_member_cud 0\r\n"
#endif
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
    PARAM_GSA_PETRA_MC_GRP_OPEN_MC_GRP_OPEN_PER_MEMBER_CUD_ID,
    "per_member_cud",
    (PARAM_VAL_RULES *)&GSA_PETRA_PER_MEMBER_CUD_rule[0],
    (sizeof(GSA_PETRA_PER_MEMBER_CUD_rule) / sizeof(GSA_PETRA_PER_MEMBER_CUD_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  per_member_cud:\r\n"
    "  If applicable (e.g. for GSA_P_MC_TYPE_EGR_GEN multicast type) - the\r\n"
    "  Copy-Unique-Data to embed into the OTMH outlif field. Note: for Ethernet\r\n"
    "  Ports, this field is interpreted as the ARP pointer.\r\n"
    "  For MC types that don't need the CUD information, this field is ignored, and\r\n"
    "  may be set to NULL. \r\n"
    "",
#if UI_GSA_PETRA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_petra_api multicast gsa_petra_mc_grp_open mc_id 0 mc_type 0 mc_grp_size 0\r\n"
    "  mc_grp_members 0 per_member_cud 0\r\n"
#endif
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
    PARAM_GSA_PETRA_MC_GRP_CLOSE_MC_GRP_CLOSE_ID,
    "mc_grp_close",
    (PARAM_VAL_RULES *)&gsa_petra_api_empty_vals[0],
    (sizeof(gsa_petra_api_empty_vals) / sizeof(gsa_petra_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Close a Multicast Group if exists.\r\n"
    "",
#if UI_GSA_PETRA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_petra_api multicast gsa_petra_mc_grp_close mc_type 0 mc_id 0\r\n"
#endif
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
    PARAM_GSA_PETRA_MC_GRP_CLOSE_MC_GRP_CLOSE_MC_TYPE_ID,
    "mc_type",
    (PARAM_VAL_RULES *)&GSA_PETRA_MC_TYPE_rule[0],
    (sizeof(GSA_PETRA_MC_TYPE_rule) / sizeof(GSA_PETRA_MC_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_type:\r\n"
    "  SOC_SAND_IN GSA_P_MC_TYPE\r\n"
    "  mc_type\r\n"
    "",
#if UI_GSA_PETRA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_petra_api multicast gsa_petra_mc_grp_close mc_type 0 mc_id 0\r\n"
#endif
    "",
    "",
    {BIT(1), 0, 0, 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_PETRA_MC_GRP_CLOSE_MC_GRP_CLOSE_MC_ID_ID,
    "mc_id",
    (PARAM_VAL_RULES *)&gsa_petra_api_free_vals[0],
    (sizeof(gsa_petra_api_free_vals) / sizeof(gsa_petra_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_id:\r\n"
    "  The index of the multicast group to close. Range: 0 - 16K-1.\r\n"
    "  SOC_SAND_IN GSA_P_MC_TYPE\r\n"
    "  mc_type -\r\n"
    "  The type of the multicast to close.\r\n"
    "",
#if UI_GSA_PETRA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_petra_api multicast gsa_petra_mc_grp_close mc_type 0 mc_id 0\r\n"
#endif
    "",
    "",
    {BIT(1), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_PETRA_MC_GRP_GET_MC_GRP_GET_ID,
    "mc_grp_get",
    (PARAM_VAL_RULES *)&gsa_petra_api_empty_vals[0],
    (sizeof(gsa_petra_api_empty_vals) / sizeof(gsa_petra_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Open Multicast Group.\r\n"
    "",
#if UI_GSA_PETRA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_petra_api multicast gsa_mc_grp_get mc_id 0 mc_type 0 mc_grp_size 0\r\n"
#endif
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
    PARAM_GSA_PETRA_MC_GRP_GET_MC_GRP_GET_MC_ID_ID,
    "mc_id",
    (PARAM_VAL_RULES *)&gsa_petra_api_free_vals[0],
    (sizeof(gsa_petra_api_free_vals) / sizeof(gsa_petra_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_id:\r\n"
    "  The index of the multicast group to open. Range: 0 - 16K-1.\r\n"
    "",
#if UI_GSA_PETRA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_petra_api multicast gsa_mc_grp_get mc_id 0 mc_type 0 mc_grp_size 0\r\n"
#endif
    "",
    "",
    {BIT(2), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_PETRA_MC_GRP_GET_MC_GRP_GET_MC_TYPE_ID,
    "mc_type",
    (PARAM_VAL_RULES *)&GSA_PETRA_MC_TYPE_rule[0],
    (sizeof(GSA_PETRA_MC_TYPE_rule) / sizeof(GSA_PETRA_MC_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_type:\r\n"
    "  The type of the multicast to open.\r\n"
    "",
#if UI_GSA_PETRA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_petra_api multicast gsa_mc_grp_get mc_id 0 mc_type 0 mc_grp_size 0\r\n"
#endif
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
    PARAM_GSA_PETRA_API_MULTICAST_ID,
    "multicast",
    (PARAM_VAL_RULES *)&gsa_petra_api_empty_vals[0],
    (sizeof(gsa_petra_api_empty_vals) / sizeof(gsa_petra_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(0)|BIT(1)|BIT(2), 0, 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } multicast*/
#ifdef UI_SCHEMES/* { schemes*/
  {
    PARAM_GSA_PETRA_SCH_SCHEMES_ID,
    "scheme",
    (PARAM_VAL_RULES *)&gsa_petra_api_empty_vals[0],
    (sizeof(gsa_petra_api_empty_vals) / sizeof(gsa_petra_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(3)|BIT(4)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_GET_CLASS_NDX_ID,
    "cls_ndx",
    (PARAM_VAL_RULES *)&GSA_PETRA_SCH_SCHEMES_CLS_NDX_rule[0],
    (sizeof(GSA_PETRA_SCH_SCHEMES_CLS_NDX_rule) / sizeof(GSA_PETRA_SCH_SCHEMES_CLS_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(3)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_GET_PORT_NDX_ID,
    "system_port_ndx",
    (PARAM_VAL_RULES *)&GSA_PETRA_SCH_SCHEMES_PORT_NDX_rule[0],
    (sizeof(GSA_PETRA_SCH_SCHEMES_PORT_NDX_rule) / sizeof(GSA_PETRA_SCH_SCHEMES_PORT_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(3)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_GET_ID,
      "gen_sch_get",
      (PARAM_VAL_RULES *)&gsa_petra_api_empty_vals[0],
      (sizeof(gsa_petra_api_empty_vals) / sizeof(gsa_petra_api_empty_vals[0])) - 1,
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
    PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_SWP_P_TM_SCH_HR_CLASS_ID,
    "sp_cls",
    (PARAM_VAL_RULES *)&GSA_PETRA_SCH_SCHEMES_HR_CLASS_rule[0],
    (sizeof(GSA_PETRA_SCH_SCHEMES_HR_CLASS_rule) / sizeof(GSA_PETRA_SCH_SCHEMES_HR_CLASS_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(4)},
    7,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_WEIGHT_ID,
    "weight",
    (PARAM_VAL_RULES *)&gsa_petra_api_free_vals[0],
    (sizeof(gsa_petra_api_free_vals) / sizeof(gsa_petra_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_MAX_ID,
    "eir_by_percentage",
    (PARAM_VAL_RULES *)&gsa_petra_api_free_vals[0],
    (sizeof(gsa_petra_api_free_vals) / sizeof(gsa_petra_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(4)},
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_MIN_ID,
    "cir_by_percentage",
    (PARAM_VAL_RULES *)&gsa_petra_api_free_vals[0],
    (sizeof(gsa_petra_api_free_vals) / sizeof(gsa_petra_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(4)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_CLASS_NDX_ID,
    "cls_ndx",
    (PARAM_VAL_RULES *)&GSA_PETRA_SCH_SCHEMES_CLS_NDX_rule[0],
    (sizeof(GSA_PETRA_SCH_SCHEMES_CLS_NDX_rule) / sizeof(GSA_PETRA_SCH_SCHEMES_CLS_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(4)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_PORT_NDX_ID,
    "system_port_ndx",
    (PARAM_VAL_RULES *)&GSA_PETRA_SCH_SCHEMES_PORT_NDX_rule[0],
    (sizeof(GSA_PETRA_SCH_SCHEMES_PORT_NDX_rule) / sizeof(GSA_PETRA_SCH_SCHEMES_PORT_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
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
    PARAM_GSA_PETRA_SCH_SCHEMES_GENERAL_UPDATE_ID,
      "gen_sch_update",
      (PARAM_VAL_RULES *)&gsa_petra_api_empty_vals[0],
      (sizeof(gsa_petra_api_empty_vals) / sizeof(gsa_petra_api_empty_vals[0])) - 1,
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
#endif /* } schemes*/
#ifdef UI_LINKS/* { links*/
  {
    PARAM_GSA_PETRA_LINKS_ID,
      "links",
      (PARAM_VAL_RULES *)&gsa_petra_api_empty_vals[0],
      (sizeof(gsa_petra_api_empty_vals) / sizeof(gsa_petra_api_empty_vals[0])) - 1,
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
    PARAM_GSA_PETRA_CONNECTIVITY_MAP_GET_ID,
      "connectivity_map_get",
      (PARAM_VAL_RULES *)&gsa_petra_api_empty_vals[0],
      (sizeof(gsa_petra_api_empty_vals) / sizeof(gsa_petra_api_empty_vals[0])) - 1,
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
    PARAM_GSA_PETRA_CONNECTIVITY_MAP_GET_FAP_NDX_ID,
      "fap_ndx",
      (PARAM_VAL_RULES *)&gsa_petra_api_free_vals[0],
      (sizeof(gsa_petra_api_free_vals) / sizeof(gsa_petra_api_free_vals[0])) - 1,
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
    PARAM_GSA_PETRA_SERDES_POWER_ID,
      "link_power",
      (PARAM_VAL_RULES *)&gsa_petra_api_empty_vals[0],
      (sizeof(gsa_petra_api_empty_vals) / sizeof(gsa_petra_api_empty_vals[0])) - 1,
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
    PARAM_GSA_PETRA_SERDES_POWER_DOWN_ID,
      "down",
      (PARAM_VAL_RULES *)&gsa_petra_api_empty_vals[0],
      (sizeof(gsa_petra_api_empty_vals) / sizeof(gsa_petra_api_empty_vals[0])) - 1,
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
    PARAM_GSA_PETRA_SERDES_POWER_DOWN_FE_NDX_ID,
      "fap_ndx",
      (PARAM_VAL_RULES *)&gsa_petra_api_free_vals[0],
      (sizeof(gsa_petra_api_free_vals) / sizeof(gsa_petra_api_free_vals[0])) - 1,
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
    PARAM_GSA_PETRA_SERDES_POWER_DOWN_SERDES_NDX_ID,
      "link_ndx",
      (PARAM_VAL_RULES *)&gsa_petra_api_free_vals[0],
      (sizeof(gsa_petra_api_free_vals) / sizeof(gsa_petra_api_free_vals[0])) - 1,
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
    PARAM_GSA_PETRA_SERDES_POWER_UP_ID,
      "up",
      (PARAM_VAL_RULES *)&gsa_petra_api_empty_vals[0],
      (sizeof(gsa_petra_api_empty_vals) / sizeof(gsa_petra_api_empty_vals[0])) - 1,
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
    PARAM_GSA_PETRA_SERDES_POWER_UP_FE_NDX_ID,
      "fap_ndx",
      (PARAM_VAL_RULES *)&gsa_petra_api_free_vals[0],
      (sizeof(gsa_petra_api_free_vals) / sizeof(gsa_petra_api_free_vals[0])) - 1,
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
    PARAM_GSA_PETRA_SERDES_POWER_UP_SERDES_NDX_ID,
      "link_ndx",
      (PARAM_VAL_RULES *)&gsa_petra_api_free_vals[0],
      (sizeof(gsa_petra_api_free_vals) / sizeof(gsa_petra_api_free_vals[0])) - 1,
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
#endif /* } links*/

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

/* } __UI_ROM_DEFI_GSA_PETRA_API_INCLUDED__*/
#endif
