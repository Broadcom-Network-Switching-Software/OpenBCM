/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UI_ROM_DEFI_SWP_P_PP_API_INCLUDED__
/* { */
#define __UI_ROM_DEFI_SWP_P_PP_API_INCLUDED__

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <appl/dpp/UserInterface/ui_pure_defi_swp_p_pp_api.h>
#include <sweep/Petra/PP/swp_p_pp_app.h>
#include <sweep/Petra/PP/swp_p_pp_api_init.h>
#include <sweep/Petra/PP/swp_p_pp_api_vlan.h>
#include <sweep/Petra/PP/swp_p_pp_api_mact.h>
#include <sweep/Petra/PP/swp_p_pp_api_ipv4.h>
#include <sweep/Petra/PP/swp_p_pp_api_acl.h>
#include <sweep/Petra/PP/swp_p_pp_api_fec.h>
#if SWP_P_CSTMR_SPEC_TS_APP
#include <soc/dpp/../../Tst/Tst2/swp_p_pp_tst2_app.h>
#endif
#if SWP_P_CSTMR_SPEC_MLAG_APP
#include <soc/dpp/../../Tst/Tst1/swp_p_pp_tst1_app.h>
#endif
#include <appl/dpp/UserInterface/ui_rom_defi_petra_pp_api.h>



#define UI_FEC
#define UI_SWP_P_PP_APP
#if SWP_P_CSTMR_SPEC_TS_APP
#define UI_SWP_P_PP_TS_APP 1
#endif
#if SWP_P_CSTMR_SPEC_MLAG_APP
#define UI_SWP_P_PP_MLAG_APP 1
#endif

/*#define UI_INIT*/
#define UI_VLAN
#define UI_MACT
#define UI_IPV4_UC
#define UI_IPV4_MC
#define UI_IPV6
 /*
  * ENUM RULES
  * {
  */
/********************************************************/

/********************************************************/

#ifndef CONST
#define CONST const
#endif


/********************************************************/

EXTERN CONST
   PARAM_VAL_RULES
     SWP_SAND_PP_PORT_L2_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "vbp",
    {
      {
        SOC_SAND_PP_PORT_L2_TYPE_VBP,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_SAND_PP_PORT_L2_TYPE.SOC_SAND_PP_PORT_L2_TYPE_VBP:\r\n"
        "  VLAN Bridge Port \r\n"
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
    "cnp",
    {
      {
        SOC_SAND_PP_PORT_L2_TYPE_CNP,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_SAND_PP_PORT_L2_TYPE.SOC_SAND_PP_PORT_L2_TYPE_CNP:\r\n"
        "  Customer Network Port: An S-VLAN component Port on a Provider Bridge or\r\n"
        "  within a Provider Edge Bridge that receives and transmits frame for a single\r\n"
        "  customer. \r\n"
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
    "cep",
    {
      {
        SOC_SAND_PP_PORT_L2_TYPE_CEP,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_SAND_PP_PORT_L2_TYPE.SOC_SAND_PP_PORT_L2_TYPE_CEP:\r\n"
        "  Customer Edge Port: A C-VLAN component Port on a Provider Edge Bridge that is\r\n"
        "  connected to customer owned equipment and receives and transmits frames for a\r\n"
        "  single customer\r\n"
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
    "pnp",
    {
      {
        SOC_SAND_PP_PORT_L2_TYPE_PNP,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_SAND_PP_PORT_L2_TYPE.SOC_SAND_PP_PORT_L2_TYPE_PNP:\r\n"
        "  Provider Network Port: An S-VLAN component Port on a Provider Bridge that can\r\n"
        "  transmit and receive frames for multiple customers.\r\n"
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
     SWP_SAND_PP_L4_PORT_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "udp",
    {
      {
        SOC_SAND_PP_L4_PORT_TYPE_UDP,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_SAND_PP_L4_PORT_TYPE.SOC_SAND_PP_L4_PORT_TYPE_UDP:\r\n"
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
    "tcp",
    {
      {
        SOC_SAND_PP_L4_PORT_TYPE_TCP,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_SAND_PP_L4_PORT_TYPE.SOC_SAND_PP_L4_PORT_TYPE_TCP:\r\n"
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
     SWP_SAND_PP_FRWRD_ACTION_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "none",
    {
      {
        SOC_SAND_PP_FRWRD_ACTION_TYPE_NONE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_SAND_PP_FRWRD_ACTION_TYPE.SOC_SAND_PP_FRWRD_ACTION_TYPE_NONE:\r\n"
        "  No Forwarding Action\r\n"
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
    "normal",
    {
      {
        SOC_SAND_PP_FRWRD_ACTION_TYPE_NORMAL,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_SAND_PP_FRWRD_ACTION_TYPE.SOC_SAND_PP_FRWRD_ACTION_TYPE_NORMAL:\r\n"
        "  The packet should be forwarded normally\r\n"
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
    "intercept",
    {
      {
        SOC_SAND_PP_FRWRD_ACTION_TYPE_INTERCEPT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_SAND_PP_FRWRD_ACTION_TYPE.SOC_SAND_PP_FRWRD_ACTION_TYPE_INTERCEPT:\r\n"
        "  The packet should be intercepted, usually to the CPU.\r\n"
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
    "control",
    {
      {
        SOC_SAND_PP_FRWRD_ACTION_TYPE_CONTROL,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_SAND_PP_FRWRD_ACTION_TYPE.SOC_SAND_PP_FRWRD_ACTION_TYPE_CONTROL:\r\n"
        "  The packet is a control packet, usually destined to the CPU.\r\n"
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
     SWP_SAND_PP_SYS_PORT_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "single_port",
    {
      {
        SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_SAND_PP_SYS_PORT_TYPE.SOC_SAND_PP_SYS_PORT_TYPE_SINGLE_PORT:\r\n"
        "  Single port\r\n"
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
    "lag",
    {
      {
        SOC_SAND_PP_SYS_PORT_TYPE_LAG,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_SAND_PP_SYS_PORT_TYPE.SOC_SAND_PP_SYS_PORT_TYPE_LAG:\r\n"
        "  LAG\r\n"
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
     SWP_SAND_PP_DEST_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "drop",
    {
      {
        SOC_SAND_PP_DEST_TYPE_DROP,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_SAND_PP_DEST_TYPE.SOC_SAND_PP_DEST_TYPE_DROP:\r\n"
        "  Drop destination\r\n"
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
    "router",
    {
      {
        SOC_SAND_PP_DEST_TYPE_ROUTER,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_SAND_PP_DEST_TYPE.SOC_SAND_PP_DEST_TYPE_ROUTER:\r\n"
        "  Router destination\r\n"
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
    "single_port",
    {
      {
        SOC_SAND_PP_DEST_SINGLE_PORT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_SAND_PP_DEST_TYPE.SOC_SAND_PP_DEST_SINGLE_PORT:\r\n"
        "  Single port\r\n"
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
    "explicit_flow",
    {
      {
        SOC_SAND_PP_DEST_EXPLICIT_FLOW,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_SAND_PP_DEST_TYPE.SOC_SAND_PP_DEST_EXPLICIT_FLOW:\r\n"
        "  Single port\r\n"
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
    "lag",
    {
      {
        SOC_SAND_PP_DEST_LAG,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_SAND_PP_DEST_TYPE.SOC_SAND_PP_DEST_LAG:\r\n"
        "  LAG\r\n"
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
    "multicast_group",
    {
      {
        SOC_SAND_PP_DEST_MULTICAST,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_SAND_PP_DEST_TYPE.SOC_SAND_PP_DEST_MULTICAST:\r\n"
        "  Multicast Group Destination\r\n"
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
     SWP_SAND_FEC_COMMAND_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "drop",
    {
      {
        SOC_SAND_PP_FEC_COMMAND_TYPE_DROP,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_SAND_PP_FEC_COMMAND_TYPE.SOC_SAND_PP_FEC_COMMAND_TYPE_DROP:\r\n"
        "  Drop command; the packet is dropped,no further processing is done for the\r\n"
        "  packet.\r\n"
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
    "route",
    {
      {
        SOC_SAND_PP_FEC_COMMAND_TYPE_ROUTE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_SAND_PP_FEC_COMMAND_TYPE.SOC_SAND_PP_FEC_COMMAND_TYPE_ROUTE:\r\n"
        "  Route command, the packet is routed and sent to next destination.\r\n"
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
    "trap",
    {
      {
        SOC_SAND_PP_FEC_COMMAND_TYPE_TRAP,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_SAND_PP_FEC_COMMAND_TYPE.SOC_SAND_PP_FEC_COMMAND_TYPE_TRAP:\r\n"
        "  Trap command, the packet is forwarded to the CPU with a 'cpu_code'.\r\n"
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
/********************************************************/
/********************************************************/

EXTERN CONST
   PARAM_VAL_RULES
     SWP_P_PP_IPV4_UC_RANGE_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "random",
    {
      {
        SWP_P_PP_IPV4_RANGE_TYPE_RANDOM,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_PP_IPV4_RANGE_TYPE.SWP_P_PP_IPV4_UC_RANGE_TYPE_RANDOM:\r\n"
        "  Selects random ipv4 addresses from the given range.\r\n"
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
    "inc",
    {
      {
        SWP_P_PP_IPV4_RANGE_TYPE_INC,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_PP_IPV4_RANGE_TYPE.SWP_P_PP_IPV4_UC_RANGE_TYPE_INC:\r\n"
        "  Selects ipv4 addresses incrementally from the given range.\r\n"
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
    "dec",
    {
      {
        SWP_P_PP_IPV4_RANGE_TYPE_DEC,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_PP_IPV4_RANGE_TYPE.SWP_P_PP_IPV4_UC_RANGE_TYPE_DEC:\r\n"
        "  Selects decremented ipv4 addresses from the given range.\r\n"
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
    "nof_types",
    {
      {
        SWP_P_PP_IPV4_RANGE_NOF_TYPES,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_PP_IPV4_RANGE_TYPE.SWP_P_PP_IPV4_UC_RANGE_NOF_TYPES:\r\n"
        "  End of Enum.\r\n"
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
     SWP_P_PP_ACL_RANGE_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "random",
    {
      {
        SWP_P_PP_ACL_RANGE_TYPE_RANDOM,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_PP_ACL_RANGE_TYPE.SWP_P_PP_ACL_RANGE_TYPE_RANDOM:\r\n"
        "  Selects random acl addresses from the given range.\r\n"
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
    "inc",
    {
      {
        SWP_P_PP_ACL_RANGE_TYPE_INC,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_PP_ACL_RANGE_TYPE.SWP_P_PP_ACL_RANGE_TYPE_INC:\r\n"
        "  Selects acl addresses incrementally from the given range.\r\n"
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
    "dec",
    {
      {
        SWP_P_PP_ACL_RANGE_TYPE_DEC,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_PP_ACL_RANGE_TYPE.SWP_P_PP_ACL_RANGE_TYPE_DEC:\r\n"
        "  Selects decremented acl addresses from the given range.\r\n"
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
     SWP_P_PP_ACL_FIELD_RANGE_SELECT_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "cvid",
    {
      {
        SWP_P_PP_ACL_FIELD_RANGE_SELECT_CVID,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_PP_ACL_FIELD_RANGE_SELECT_TYPE.SWP_P_PP_ACL_FIELD_RANGE_SELECT_CVID:\r\n"
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
    "sa",
    {
      {
        SWP_P_PP_ACL_FIELD_RANGE_SELECT_SA,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_PP_ACL_FIELD_RANGE_SELECT_TYPE.SWP_P_PP_ACL_FIELD_RANGE_SELECT_SA:\r\n"
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
    "select_da",
    {
      {
        SWP_P_PP_ACL_FIELD_RANGE_SELECT_DA,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_PP_ACL_FIELD_RANGE_SELECT_TYPE.SWP_P_PP_ACL_FIELD_RANG_SELECT_DA:\r\n"
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
    "svid",
    {
      {
        SWP_P_PP_ACL_FIELD_RANGE_SELECT_SVID,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_PP_ACL_FIELD_RANGE_SELECT_TYPE.SWP_P_PP_ACL_FIELD_RANGE_SELECT_SVID:\r\n"
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
    "in_port_id",
    {
      {
        SWP_P_PP_ACL_FIELD_RANGE_SELECT_IN_PORT_ID,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_PP_ACL_FIELD_RANGE_SELECT_TYPE.SWP_P_PP_ACL_FIELD_RANGE_SELECT_IN_PORT_\r\n"
        "  D:\r\n"
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
    "src_ip",
    {
      {
        SWP_P_PP_ACL_FIELD_RANGE_SELECT_SRC_IP,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_PP_ACL_FIELD_RANGE_SELECT_TYPE.SWP_P_PP_ACL_FIELD_RANGE_SELECT_SRC_IP:\r\n"
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
    "dest_ip",
    {
      {
        SWP_P_PP_ACL_FIELD_RANGE_SELECT_DEST_IP,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_PP_ACL_FIELD_RANGE_SELECT_TYPE.SWP_P_PP_ACL_FIELD_RANGE_SELECT_DEST_IP:\r\n"
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
    "l4_src_port",
    {
      {
        SWP_P_PP_ACL_FIELD_RANGE_SELECT_L4_SRC_PORT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_PP_ACL_FIELD_RANGE_SELECT_TYPE.SWP_P_PP_ACL_FIELD_RANGE_SELECT_L4_SRC_P\r\n"
        "  RT:\r\n"
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
    "l4_dest_port",
    {
      {
        SWP_P_PP_ACL_FIELD_RANGE_SELECT_L4_DEST_PORT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_PP_ACL_FIELD_RANGE_SELECT_TYPE.SWP_P_PP_ACL_FIELD_RANGE_SELECT_L4_DEST_\r\n"
        "  ORT:\r\n"
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
     SWP_P_PP_FEC_ADD_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "new",
    {
      {
        SWP_P_PP_FEC_ADD_TYPE_NEW,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_PP_FEC_ADD_TYPE.SWP_P_PP_FEC_ADD_TYPE_NEW:\r\n"
        "  Create new FEC entry with the given sys-FEC-id and FEC info. If the\r\n"
        "  sys-FEC-id already in use returns Error.\r\n"
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
    "new_overrider",
    {
      {
        SWP_P_PP_FEC_ADD_TYPE_NEW_OVERRIDER,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_PP_FEC_ADD_TYPE.SWP_P_PP_FEC_ADD_TYPE_NEW_OVERRIDER:\r\n"
        "  Create new FEC with the given sys-FEC-id and FEC info. If the sys-FEC-id\r\n"
        "  already in use override it.\r\n"
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
    "concat",
    {
      {
        SWP_P_PP_FEC_ADD_TYPE_CONCAT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWP_P_PP_FEC_ADD_TYPE.SWP_P_PP_FEC_ADD_TYPE_CONCAT:\r\n"
        "  Concatenate the given FEC info to the sys-FEC-id info. If the sys-FEC-id not\r\n"
        "  in use then create new FEC with the given info.\r\n"
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
#ifdef UI_SWP_P_PP_TS_APP/* { swp_p_pp_ts_app*/ 
EXTERN CONST 
   PARAM_VAL_RULES 
     SWP_P_PP_TS_KEY_TYPE_rule[] 
#ifdef INIT 
   = 
{ 
  { 
    VAL_SYMBOL, 
    "udp_uc", 
    { 
      { 
        SWP_P_PP_TS_KEY_TYPE_UDP_UC, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  SWP_P_PP_TS_KEY_TYPE.SWP_P_PP_TS_KEY_TYPE_UDP_UC:\n\r"
        "  Key is for UDPoIPoETH Unicast (IP) packets, Key includes (table-id, port,\n\r"
        "  UDP-dest-port, DIP), where UDP-dest-port can be masked.\n\r"
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
    "udp_mc", 
    { 
      { 
        SWP_P_PP_TS_KEY_TYPE_UDP_MC, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  SWP_P_PP_TS_KEY_TYPE.SWP_P_PP_TS_KEY_TYPE_UDP_MC:\n\r"
        "  Key is for UDPoIPoETH Multicast (IP) packets. Key includes (table-id, port,\n\r"
        "  UDP-dest-port, SIP, DIP)\n\r"
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
    "l2_tpv3", 
    { 
      { 
        SWP_P_PP_TS_KEY_TYPE_L2_TPV3, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  SWP_P_PP_TS_KEY_TYPE.SWP_P_PP_TS_KEY_TYPE_L2_TPV3:\n\r"
        "  Key is for L2TPv3oIPoETH (Layer 2 Tunneling Protocol Version 3) packets. Key\n\r"
        "  includes (table-id, port, session-id, DIP)\n\r"
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
     SWP_P_PP_TS_TRAP_QUAL_rule[] 
#ifdef INIT 
   = 
{ 
  { 
    VAL_SYMBOL, 
    "bc", 
    { 
      { 
        SWP_P_PP_TS_TRAP_QUAL_PACKET_BC, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  SWP_P_PP_TS_TRAP_QUAL.SWP_P_PP_TS_TRAP_QUAL_PACKET_BC:\n\r"
        "  Trap BC packets. packets with DA = 0xFFFFFFFF\n\r"
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
    "cdp", 
    { 
      { 
        SWP_P_PP_TS_TRAP_QUAL_PACKET_CDP, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  SWP_P_PP_TS_TRAP_QUAL.SWP_P_PP_TS_TRAP_QUAL_PACKET_CDP:\n\r"
        "  Trap CDP packet with DA = X\"01000CCCCCCC\"\n\r"
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
    "igmp", 
    { 
      { 
        SWP_P_PP_TS_TRAP_QUAL_PACKET_IGMP, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  SWP_P_PP_TS_TRAP_QUAL.SWP_P_PP_TS_TRAP_QUAL_PACKET_IGMP:\n\r"
        "  Trap All IGMP traffic\n\r"
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
    "non_udp", 
    { 
      { 
        SWP_P_PP_TS_TRAP_QUAL_NON_UDP_PACKET_DIP, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  SWP_P_PP_TS_TRAP_QUAL.SWP_P_PP_TS_TRAP_QUAL_NON_UDP_PACKET_DIP:\n\r"
        "  Trap non-UDP IP packets with DIP value equal to 'value' field in\n\r"
        "  SWP_P_PP_TS_TRAP_KEY \n\r"
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
    "non_ip", 
    { 
      { 
        SWP_P_PP_TS_TRAP_QUAL_NON_UDP_IP_PACKET_DA, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  SWP_P_PP_TS_TRAP_QUAL.SWP_P_PP_TS_TRAP_QUAL_NON_UDP_IP_PACKET_DA:\n\r"
        "  Trap non-IP-UDP packets with Destination MAC address set in 'mac' field in\n\r"
        "  SWP_P_PP_TS_TRAP_KEY \n\r"
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
    "my_ip", 
    { 
      { 
        SWP_P_PP_TS_TRAP_QUAL_PACKET_DIP, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  SWP_P_PP_TS_TRAP_QUAL.SWP_P_PP_TS_TRAP_QUAL_PACKET_DIP:\n\r"
        "  Trap IP packets with DIP value equal to 'value' field in SWP_P_PP_TS_TRAP_KEY\n\r"
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

#endif/* } swp_p_pp_ts_app*/
/********************************************************/

#ifdef UI_VLAN
/********************************************************/        

EXTERN CONST 
   PARAM_VAL_RULES 
     SWP_P_PP_BR_PORT_TYPE_rule[] 
#ifdef INIT 
   = 
{ 
  { 
    VAL_SYMBOL, 
    "iptv", 
    { 
      { 
        SWP_P_PP_BR_PORT_TYPE_IPTV, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  SWP_P_PP_BR_PORT_TYPE.SWP_P_PP_BR_PORT_TYPE_IPTV:\n\r"
        "  port with IPTV processing\n\r"
          "", 
          /* 
           * Pointer to a function to call after symbolic value 
           * has been accepted. Casting should be 'VAL_PROC_PTR'. 
           * Casting to 'long' added here just to keep the stupid 
           * compiler silent. 
           */ 
        (long)NULL 
      } 
    } 
  }, 
  { 
    VAL_SYMBOL, 
    "hqos", 
    { 
      { 
        SWP_P_PP_BR_PORT_TYPE_HQOS, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  SWP_P_PP_BR_PORT_TYPE.SWP_P_PP_BR_PORT_TYPE_HQOS:\n\r"
        "  port with HQOS processing\n\r"
          "", 
          /* 
           * Pointer to a function to call after symbolic value 
           * has been accepted. Casting should be 'VAL_PROC_PTR'. 
           * Casting to 'long' added here just to keep the stupid 
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
     SWP_P_PP_BR_KEY_TYPE_rule[] 
#ifdef INIT 
   = 
{ 
  { 
    VAL_SYMBOL, 
    "iptv", 
    { 
      { 
        SWP_P_PP_BR_KEY_TYPE_IPTV, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  SWP_P_PP_BR_KEY_TYPE.SWP_P_PP_BR_KEY_TYPE_IPTV:\n\r"
        "  Key is for IPTV application\n\r"
          "", 
          /* 
           * Pointer to a function to call after symbolic value 
           * has been accepted. Casting should be 'VAL_PROC_PTR'. 
           * Casting to 'long' added here just to keep the stupid 
           * compiler silent. 
           */ 
        (long)NULL 
      } 
    } 
  }, 
  { 
    VAL_SYMBOL, 
    "hqos", 
    { 
      { 
        SWP_P_PP_BR_KEY_TYPE_HQOS, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  SWP_P_PP_BR_KEY_TYPE.SWP_P_PP_BR_KEY_TYPE_HQOS:\n\r"
        "  Key is for HQOS application\n\r"
          "", 
          /* 
           * Pointer to a function to call after symbolic value 
           * has been accepted. Casting should be 'VAL_PROC_PTR'. 
           * Casting to 'long' added here just to keep the stupid 
           * compiler silent. 
           */ 
        (long)NULL 
      } 
    } 
  }, 
  { 
    VAL_SYMBOL, 
    "iptv_hqos", 
    { 
      { 
        SWP_P_PP_BR_KEY_TYPE_IPTV_HQOS, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  SWP_P_PP_BR_KEY_TYPE.SWP_P_PP_BR_KEY_TYPE_IPTV_HQOS:\n\r"
        "  Either IPTV or HQOS, since same key is used for both application on get this\n\r"
        "  value is returned.\n\r"
          "", 
          /* 
           * Pointer to a function to call after symbolic value 
           * has been accepted. Casting should be 'VAL_PROC_PTR'. 
           * Casting to 'long' added here just to keep the stupid 
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
#endif/* } UI_SWP_P_PP_TST6*/


EXTERN CONST
   PARAM_VAL_RULES
     swp_p_pp_api_free_vals[]
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
     swp_p_pp_api_empty_vals[]
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
     swp_pp_api_members_vals[]
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
#if 0
  swp_p_pp_api_params(){}
#endif
EXTERN CONST
   PARAM
     swp_p_pp_api_params[]
#ifdef INIT
   =
{

#ifdef UI_FEC/* { fec*/
  {
    PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_ID,
    "uc_ecmp_add",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Add array of Unicast FEC entries (ECMP) equal-cost-multi-path entry, so\r\n"
    "  Unicast packets lookup result with this sys_fec_d are routed along one of\r\n"
    "  these FEC entries while ensuring load balancing.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 dest_val 0 1\r\n"
    "  dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
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
    PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_SYS_FEC_NDX_ID,
    "sys_fec_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC id. Range 0-(4G-1)\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 dest_val 0 1\r\n"
    "  dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
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
    PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_ADD_TYPE_ID,
    "add_type",
    (PARAM_VAL_RULES *)&SWP_P_PP_FEC_ADD_TYPE_rule[0],
    (sizeof(SWP_P_PP_FEC_ADD_TYPE_rule) / sizeof(SWP_P_PP_FEC_ADD_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  add_type:\r\n"
    "  create new/override/concatenate.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 dest_val 0 1\r\n"
    "  dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
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
    PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_STATISTICS_COUNTER_ID,
    "statistics_counter",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_members_vals[0],
    (sizeof(soc_petra_pp_api_members_vals) / sizeof(soc_petra_pp_api_members_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].statistics_counter:\r\n"
    "  Select a statistics counter that is incremented when this entry is accessed.\r\n"
    "  Range 0-7.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 dest_val 0 1\r\n"
    "  dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
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
    PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_ENABLE_RPF_ID,
    "enable_rpf",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].enable_rpf:\r\n"
    "  If TRUE, when searching with the IPv4 SrcIP during multicast routing, then\r\n"
    "  use the destination field of the entry for RPF check of multicast packets.\r\n"
    "  This applies only if RPF is not specified in the Multicast FEC Entry. In this\r\n"
    "  case, the Destination must match the incoming port for the RPF check to pass.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 dest_val 0 1\r\n"
    "  dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
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
    PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_CPU_CODE_ID,
    "cpu_code",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_members_vals[0],
    (sizeof(soc_petra_pp_api_members_vals) / sizeof(soc_petra_pp_api_members_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].cpu_code:\r\n"
    "  Value to be sent to the CPU(in the EPPH). Range 0-1023.Valid only if command\r\n"
    "  is Trap.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 dest_val 0 1\r\n"
    "  dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
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
    PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_OUT_VID_ID,
    "out_vid",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_members_vals[0],
    (sizeof(soc_petra_pp_api_members_vals) / sizeof(soc_petra_pp_api_members_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].out_vid:\r\n"
    "  Outgoing VLAN ID.Rang 0-4095. \r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 dest_val 0 1\r\n"
    "  dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
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
    PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_MAC_ADDR_ID,
    "mac_addr",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].mac_addr:\r\n"
    "  MAC address to be used as DA for outgoing packet. (next hop MAC address)\r\n"
    "  Valid only if command is not Trap.Range 0-4095.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 dest_val 0 1\r\n"
    "  dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
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
    PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_DEST_DEST_VAL_ID,
    "dest_val",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_members_vals[0],
    (sizeof(soc_petra_pp_api_members_vals) / sizeof(soc_petra_pp_api_members_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].dest.dest_val:\r\n"
    "  The destination value, according to the type (Single, LAG,LIF...)For LAG the\r\n"
    "  value is the group ID.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 dest_val 0 1\r\n"
    "  dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
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
    PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_DEST_DEST_TYPE_ID,
    "dest_type",
    (PARAM_VAL_RULES *)&SWP_SAND_PP_DEST_TYPE_rule[0],
    (sizeof(SWP_SAND_PP_DEST_TYPE_rule) / sizeof(SWP_SAND_PP_DEST_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].dest.dest_type:\r\n"
    "  The type of the following destination ID\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 dest_val 0 1\r\n"
    "  dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
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
    PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_ACTION_ID,
    "action",
    (PARAM_VAL_RULES *)&SOC_PETRA_PP_SAND_PP_FEC_COMMAND_TYPE_rule[0],
    (sizeof(SOC_PETRA_PP_SAND_PP_FEC_COMMAND_TYPE_rule) / sizeof(SOC_PETRA_PP_SAND_PP_FEC_COMMAND_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].action:\r\n"
    "  Drop, Route or Trap\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 dest_val 0 1\r\n"
    "  dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
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
    PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_RANGE_INC_MAC_ID,
    "range_inc_mac",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_cpu_code:\r\n"
    "  By how much to increment the CPU code value for each system FEC \r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 dest_val 0 1\r\n"
    "  dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
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
    PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_RANGE_INC_CPU_CODE_ID,
    "range_inc_cpu_code",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_vid:\r\n"
    "  By how much to increment the CPU code value for each system FEC \r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 dest_val 0 1\r\n"
    "  dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
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
    PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_RANGE_UP_TO_SYS_FEC_ID,
    "range_up_to_sys_fec",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_mac:\r\n"
    "  By how much to increment the CPU code value for each system FEC \r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 dest_val 0 1\r\n"
    "  dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
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
    PARAM_SWP_P_PP_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_RANGE_INC_VID_ID,
    "range_inc_out_vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_mac:\r\n"
    "  By how much to increment the out vidvalue for each system FEC \r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 dest_val 0 1\r\n"
    "  dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
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
    PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_ID,
    "uc_ecmp_update_entry",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Update one FEC entry in the array of Unicast FEC entries (ECMP).\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_update_entry sys_fec_ndx 0 entry_ndx 0\r\n"
    "  statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0 mac_addr 0 dest_val 0\r\n"
    "  dest_type 0 action 0\r\n"
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
    PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_SYS_FEC_NDX_ID,
    "sys_fec_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC id. Range 0-(4G-1)\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_update_entry sys_fec_ndx 0 entry_ndx 0\r\n"
    "  statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0 mac_addr 0 dest_val 0\r\n"
    "  dest_type 0 action 0\r\n"
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
    PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_ENTRY_NDX_ID,
    "entry_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_ndx:\r\n"
    "  Number of valid entries in the array. Range 0-15.\r\n"
    "  if the entry out of the ECMP range error will be returned.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_update_entry sys_fec_ndx 0 entry_ndx 0\r\n"
    "  statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0 mac_addr 0 dest_val 0\r\n"
    "  dest_type 0 action 0\r\n"
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
    PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_STATISTICS_COUNTER_ID,
    "statistics_counter",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_entry.statistics_counter:\r\n"
    "  Select a statistics counter that is incremented when this entry is accessed.\r\n"
    "  Range 0-7.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_update_entry sys_fec_ndx 0 entry_ndx 0\r\n"
    "  statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0 mac_addr 0 dest_val 0\r\n"
    "  dest_type 0 action 0\r\n"
#endif
    "",
    "",
    {BIT(1), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_ENABLE_RPF_ID,
    "enable_rpf",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_entry.enable_rpf:\r\n"
    "  If TRUE, when searching with the IPv4 SrcIP during multicast routing, then\r\n"
    "  use the destination field of the entry for RPF check of multicast packets.\r\n"
    "  This applies only if RPF is not specified in the Multicast FEC Entry. In this\r\n"
    "  case, the Destination must match the incoming port for the RPF check to pass.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_update_entry sys_fec_ndx 0 entry_ndx 0\r\n"
    "  statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0 mac_addr 0 dest_val 0\r\n"
    "  dest_type 0 action 0\r\n"
#endif
    "",
    "",
    {BIT(1), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_CPU_CODE_ID,
    "cpu_code",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_entry.cpu_code:\r\n"
    "  Value to be sent to the CPU(in the EPPH). Range 0-1023.Valid only if command\r\n"
    "  is Trap.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_update_entry sys_fec_ndx 0 entry_ndx 0\r\n"
    "  statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0 mac_addr 0 dest_val 0\r\n"
    "  dest_type 0 action 0\r\n"
#endif
    "",
    "",
    {BIT(1), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_OUT_VID_ID,
    "out_vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_entry.out_vid:\r\n"
    "  Outgoing VLAN ID.Rang 0-4095. \r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_update_entry sys_fec_ndx 0 entry_ndx 0\r\n"
    "  statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0 mac_addr 0 dest_val 0\r\n"
    "  dest_type 0 action 0\r\n"
#endif
    "",
    "",
    {BIT(1), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_MAC_ADDR_ID,
    "mac_addr",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_entry.mac_addr:\r\n"
    "  MAC address to be used as DA for outgoing packet. (next hop MAC address)\r\n"
    "  Valid only if command is not Trap.Range 0-4095.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_update_entry sys_fec_ndx 0 entry_ndx 0\r\n"
    "  statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0 mac_addr 0 dest_val 0\r\n"
    "  dest_type 0 action 0\r\n"
#endif
    "",
    "",
    {BIT(1), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_DEST_DEST_VAL_ID,
    "dest_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_entry.dest.dest_val:\r\n"
    "  The destination value, according to the type (Single, LAG,LIF...)For LAG the\r\n"
    "  value is the group ID.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_update_entry sys_fec_ndx 0 entry_ndx 0\r\n"
    "  statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0 mac_addr 0 dest_val 0\r\n"
    "  dest_type 0 action 0\r\n"
#endif
    "",
    "",
    {BIT(1), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_DEST_DEST_TYPE_ID,
    "dest_type",
    (PARAM_VAL_RULES *)&SWP_SAND_PP_DEST_TYPE_rule[0],
    (sizeof(SWP_SAND_PP_DEST_TYPE_rule) / sizeof(SWP_SAND_PP_DEST_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_entry.dest.dest_type:\r\n"
    "  The type of the following destination ID\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_update_entry sys_fec_ndx 0 entry_ndx 0\r\n"
    "  statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0 mac_addr 0 dest_val 0\r\n"
    "  dest_type 0 action 0\r\n"
#endif
    "",
    "",
    {BIT(1), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_ACTION_ID,
    "action",
    (PARAM_VAL_RULES *)&SOC_PETRA_PP_SAND_PP_FEC_COMMAND_TYPE_rule[0],
    (sizeof(SOC_PETRA_PP_SAND_PP_FEC_COMMAND_TYPE_rule) / sizeof(SOC_PETRA_PP_SAND_PP_FEC_COMMAND_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_entry.action:\r\n"
    "  Drop, Route or Trap\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_update_entry sys_fec_ndx 0 entry_ndx 0\r\n"
    "  statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0 mac_addr 0 dest_val 0\r\n"
    "  dest_type 0 action 0\r\n"
#endif
    "",
    "",
    {BIT(1), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_ID,
    "mc_fec_add",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Add Multicast FEC, so Multicast packets lookup result with this sys_fec_d are\r\n"
    "  routed according to this FEC.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_add sys_fec_ndx 0 add_type 0 statistics_counter 0 rpf_check 0\r\n"
    "  cpu_code 0 rpf_exp_in_port_id 0 rpf_exp_port_type 0 rpf_exp_in_vid 0 dest_val 0 dest_type 0\r\n"
    "  action 0 range_up_to_sys_fec_ndx 0 range_inc_cpu_code 0 range_inc_dest 0 \r\n"
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
    PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_SYS_FEC_NDX_ID,
    "sys_fec_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC id. Range 0-(4G-1)\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_add sys_fec_ndx 0 add_type 0 statistics_counter 0 rpf_check 0\r\n"
    "  cpu_code 0 rpf_exp_in_port_id 0 rpf_exp_port_type 0 rpf_exp_in_vid 0 dest_val 0 dest_type 0\r\n"
    "  action 0 range_up_to_sys_fec_ndx 0 range_inc_cpu_code 0 range_inc_dest 0 \r\n"
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
    PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_ADD_TYPE_ID,
    "add_type",
    (PARAM_VAL_RULES *)&SWP_P_PP_FEC_ADD_TYPE_rule[0],
    (sizeof(SWP_P_PP_FEC_ADD_TYPE_rule) / sizeof(SWP_P_PP_FEC_ADD_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  add_type:\r\n"
    "  create new/override/concatenate.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_add sys_fec_ndx 0 add_type 0 statistics_counter 0 rpf_check 0\r\n"
    "  cpu_code 0 rpf_exp_in_port_id 0 rpf_exp_port_type 0 rpf_exp_in_vid 0 dest_val 0 dest_type 0\r\n"
    "  action 0 range_up_to_sys_fec_ndx 0 range_inc_cpu_code 0 range_inc_dest 0 \r\n"
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
    PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_STATISTICS_COUNTER_ID,
    "statistics_counter",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.statistics_counter:\r\n"
    "  Select a statistics counter that is incremented when this entry is accessed.\r\n"
    "  Range 0-7.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_add sys_fec_ndx 0 add_type 0 statistics_counter 0 rpf_check 0\r\n"
    "  cpu_code 0 rpf_exp_in_port_id 0 rpf_exp_port_type 0 rpf_exp_in_vid 0 dest_val 0 dest_type 0\r\n"
    "  action 0 range_up_to_sys_fec_ndx 0 range_inc_cpu_code 0 range_inc_dest 0 \r\n"
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
    PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_CPU_CODE_ID,
    "cpu_code",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.cpu_code:\r\n"
    "  Value to be sent to the CPU(in the EPPH). Range 0-1023.Valid only if command\r\n"
    "  is Trap.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_add sys_fec_ndx 0 add_type 0 statistics_counter 0 rpf_check 0\r\n"
    "  cpu_code 0 rpf_exp_in_port_id 0 rpf_exp_port_type 0 rpf_exp_in_vid 0 dest_val 0 dest_type 0\r\n"
    "  action 0 range_up_to_sys_fec_ndx 0 range_inc_cpu_code 0 range_inc_dest 0 \r\n"
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
    PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_PORT_SYS_ID_ID,
    "rpf_exp_in_port_id",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.rpf_check.exp_in_port.sys_id:\r\n"
    "  The system port value, according to the type (Single or LAG)For LAG the value\r\n"
    "  is the group ID.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_add sys_fec_ndx 0 add_type 0 statistics_counter 0 rpf_check 0\r\n"
    "  cpu_code 0 rpf_exp_in_port_id 0 rpf_exp_port_type 0 rpf_exp_in_vid 0 dest_val 0 dest_type 0\r\n"
    "  action 0 range_up_to_sys_fec_ndx 0 range_inc_cpu_code 0 range_inc_dest 0 \r\n"
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
    PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_PORT_SYS_PORT_TYPE_ID,
    "rpf_exp_port_type",
    (PARAM_VAL_RULES *)&SOC_SAND_PP_SYS_PORT_TYPE_rule[0],
    (sizeof(SOC_SAND_PP_SYS_PORT_TYPE_rule) / sizeof(SOC_SAND_PP_SYS_PORT_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.rpf_check.exp_in_port.sys_port_type:\r\n"
    "  The system port type single/LAG.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_add sys_fec_ndx 0 add_type 0 statistics_counter 0 rpf_check 0\r\n"
    "  cpu_code 0 rpf_exp_in_port_id 0 rpf_exp_port_type 0 rpf_exp_in_vid 0 dest_val 0 dest_type 0\r\n"
    "  action 0 range_up_to_sys_fec_ndx 0 range_inc_cpu_code 0 range_inc_dest 0 \r\n"
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
    PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_VID_ID,
    "rpf_exp_in_vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.rpf_check.exp_in_vid:\r\n"
    "  Expected incoming VID to use in the RPF check. If the incoming packet VID is\r\n"
    "  different than this value and check_vid is TRUE then the RPF check fail.\r\n"
    "  valid only if the rpf check consider the VID is TRUE. Range 1-4095.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_add sys_fec_ndx 0 add_type 0 statistics_counter 0 rpf_check 0\r\n"
    "  cpu_code 0 rpf_exp_in_port_id 0 rpf_exp_port_type 0 rpf_exp_in_vid 0 dest_val 0 dest_type 0\r\n"
    "  action 0 range_up_to_sys_fec_ndx 0 range_inc_cpu_code 0 range_inc_dest 0 \r\n"
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
    PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_DEST_DEST_VAL_ID,
    "dest_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.dest.dest_val:\r\n"
    "  The destination value, according to the type (Single, LAG,LIF...)For LAG the\r\n"
    "  value is the group ID.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_add sys_fec_ndx 0 add_type 0 statistics_counter 0 rpf_check 0\r\n"
    "  cpu_code 0 rpf_exp_in_port_id 0 rpf_exp_port_type 0 rpf_exp_in_vid 0 dest_val 0 dest_type 0\r\n"
    "  action 0 range_up_to_sys_fec_ndx 0 range_inc_cpu_code 0 range_inc_dest 0 \r\n"
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
    PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_DEST_DEST_TYPE_ID,
    "dest_type",
    (PARAM_VAL_RULES *)&SWP_SAND_PP_DEST_TYPE_rule[0],
    (sizeof(SWP_SAND_PP_DEST_TYPE_rule) / sizeof(SWP_SAND_PP_DEST_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.dest.dest_type:\r\n"
    "  The type of the following destination ID\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_add sys_fec_ndx 0 add_type 0 statistics_counter 0 rpf_check 0\r\n"
    "  cpu_code 0 rpf_exp_in_port_id 0 rpf_exp_port_type 0 rpf_exp_in_vid 0 dest_val 0 dest_type 0\r\n"
    "  action 0 range_up_to_sys_fec_ndx 0 range_inc_cpu_code 0 range_inc_dest 0 \r\n"
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
    PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_ACTION_ID,
    "action",
    (PARAM_VAL_RULES *)&SOC_PETRA_PP_SAND_PP_FEC_COMMAND_TYPE_rule[0],
    (sizeof(SOC_PETRA_PP_SAND_PP_FEC_COMMAND_TYPE_rule) / sizeof(SOC_PETRA_PP_SAND_PP_FEC_COMMAND_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.action:\r\n"
    "  Drop, Route or Trap\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_add sys_fec_ndx 0 add_type 0 statistics_counter 0 rpf_check 0\r\n"
    "  cpu_code 0 rpf_exp_in_port_id 0 rpf_exp_port_type 0 rpf_exp_in_vid 0 dest_val 0 dest_type 0\r\n"
    "  action 0 range_up_to_sys_fec_ndx 0 range_inc_cpu_code 0 range_inc_dest 0 \r\n"
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
    PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_RPF_TYPE_ID,
    "rpf_check",
    (PARAM_VAL_RULES *)&SOC_PETRA_PP_FEC_RPF_CHECK_rule[0],
    (sizeof(SOC_PETRA_PP_FEC_RPF_CHECK_rule) / sizeof(SOC_PETRA_PP_FEC_RPF_CHECK_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Whether to perform RPF check (Reverse Path Forwarding)\r\n"
    "  and according to which parameters. Check according to \r\n"
    "  incoming port can be done only of the command is not trap.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_add sys_fec_ndx 0 add_type 0 statistics_counter 0 rpf_check 0\r\n"
    "  cpu_code 0 rpf_exp_in_port_id 0 rpf_exp_port_type 0 rpf_exp_in_vid 0 dest_val 0 dest_type 0\r\n"
    "  action 0 range_up_to_sys_fec_ndx 0 range_inc_cpu_code 0 range_inc_dest 0 \r\n"
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
    PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_UP_TO_SYS_FEC_NDX_ID,
    "range_up_to_sys_fec_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  up_to_sys_fec_ndx:\r\n"
    "  range upper limit of system FECs to add . Range 0-(4G-1)\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_add sys_fec_ndx 0 add_type 0 statistics_counter 0 rpf_check 0\r\n"
    "  cpu_code 0 rpf_exp_in_port_id 0 rpf_exp_port_type 0 rpf_exp_in_vid 0 dest_val 0 dest_type 0\r\n"
    "  action 0 range_up_to_sys_fec_ndx 0 range_inc_cpu_code 0 range_inc_dest 0 \r\n"
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
    PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_RANGE_INFO_INC_CPU_CODE_ID,
    "range_inc_cpu_code",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_cpu_code:\r\n"
    "  By how much to increment the CPU code value in each entry.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_add sys_fec_ndx 0 add_type 0 statistics_counter 0 rpf_check 0\r\n"
    "  cpu_code 0 rpf_exp_in_port_id 0 rpf_exp_port_type 0 rpf_exp_in_vid 0 dest_val 0 dest_type 0\r\n"
    "  action 0 range_up_to_sys_fec_ndx 0 range_inc_cpu_code 0 range_inc_dest 0 \r\n"
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
    PARAM_SWP_P_PP_FEC_MC_FEC_ADD_MC_FEC_ADD_RANGE_INFO_INC_DEST_ID,
    "range_inc_dest",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_dest:\r\n"
    "  By how much to increment the Destination Multicast Group value in each entry.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_add sys_fec_ndx 0 add_type 0 statistics_counter 0 rpf_check 0\r\n"
    "  cpu_code 0 rpf_exp_in_port_id 0 rpf_exp_port_type 0 rpf_exp_in_vid 0 dest_val 0 dest_type 0\r\n"
    "  action 0 range_up_to_sys_fec_ndx 0 range_inc_cpu_code 0 range_inc_dest 0 \r\n"
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
    PARAM_SWP_P_PP_FEC_UC_EMCP_REMOVE_ENTRY_UC_EMCP_REMOVE_ENTRY_ID,
    "uc_ecmp_remove_entry",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove one FEC from the ECMP.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_remove_entry sys_fec_ndx 0 fec_entry_ndx 0\r\n"
#endif
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
    PARAM_SWP_P_PP_FEC_UC_EMCP_REMOVE_ENTRY_UC_EMCP_REMOVE_ENTRY_SYS_FEC_NDX_ID,
    "sys_fec_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC id.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_remove_entry sys_fec_ndx 0 fec_entry_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(3), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_UC_EMCP_REMOVE_ENTRY_UC_EMCP_REMOVE_ENTRY_FEC_ENTRY_NDX_ID,
    "fec_entry_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  fec_entry_ndx:\r\n"
    "  the order of the FEC in the ECMP.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_remove_entry sys_fec_ndx 0 fec_entry_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(3), 0, 0, 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_REMOVE_REMOVE_ID,
    "remove",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Removes all the FEC entries that are identified by this\r\n"
    "  sys_fec_ndx, the FEC entry has to be not in use by any routing or\r\n"
    "  ACL table.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec remove sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
#endif
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
    PARAM_SWP_P_PP_FEC_REMOVE_REMOVE_SYS_FEC_NDX_ID,
    "sys_fec_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC id. Range 0-(4G-1). \r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec remove sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(4), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_REMOVE_UP_TO_SYS_FEC_NDX_ID,
    "up_to_sys_fec_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  up_to_sys_fec_ndx:\r\n"
    "  range upper limit of system FECs to add . Range 0-(4G-1)\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec remove sys_fec_ndx 0 up_to_sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
#endif
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
    PARAM_SWP_P_PP_FEC_UC_GET_UC_GET_ID,
    "uc_get",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get Unicast FEC entry/entries that are associated with the given sys_fec_ndx.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_get sys_fec_ndx 0\r\n"
#endif
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
    PARAM_SWP_P_PP_FEC_UC_GET_UC_GET_SYS_FEC_NDX_ID,
    "sys_fec_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC id. Range 0-(4G-1)\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_get sys_fec_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(5), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_MC_GET_MC_GET_ID,
    "mc_get",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get Multicast FEC entry that is associated with the given sys_fec_ndx.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_get sys_fec_ndx 0\r\n"
#endif
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
    PARAM_SWP_P_PP_FEC_MC_GET_MC_GET_SYS_FEC_NDX_ID,
    "sys_fec_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC id. Range 0-(4G-1)\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_get sys_fec_ndx 0\r\n"
#endif
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
    PARAM_SWP_P_PP_FEC_TABLE_UC_GET_BLOCK_TABLE_UC_GET_BLOCK_ID,
    "table_uc_get_block",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Traverse the FEC table and get all the UC FEC entries.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec table_uc_get_block\r\n"
#endif
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
    PARAM_SWP_P_PP_FEC_TABLE_MC_GET_BLOCK_TABLE_MC_GET_BLOCK_ID,
    "table_mc_get_block",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Traverse the FEC table and get all the MC FEC entries.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec table_mc_get_block\r\n"
#endif
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
    PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_ID,
    "uc_ecmp_range_add",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Add Range Unicast FEC entries (ECMP) equal-cost-multi-path entry, so Unicast\r\n"
    "  packets lookup result with this sys_fec_d are routed along one of these FEC\r\n"
    "  entries while ensuring load balancing. The addition type is new_override.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 dest_val 0 dest_type 0 action 0 nof_entries 0 inc_cpu_code 0\r\n"
    "  inc_vid 0 inc_mac 0\r\n"
#endif
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
    PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_SYS_FEC_NDX_ID,
    "sys_fec_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC id. Range 0-(4G-1)\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 dest_val 0 dest_type 0 action 0 nof_entries 0 inc_cpu_code 0\r\n"
    "  inc_vid 0 inc_mac 0\r\n"
#endif
    "",
    "",
    {BIT(9), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UP_TO_SYS_FEC_NDX_ID,
    "up_to_sys_fec_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  up_to_sys_fec_ndx:\r\n"
    "  range upper limit of system FECs to add . Range 0-(4G-1)\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 dest_val 0 dest_type 0 action 0 nof_entries 0 inc_cpu_code 0\r\n"
    "  inc_vid 0 inc_mac 0\r\n"
#endif
    "",
    "",
    {BIT(9), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_INDEX_ID,
    "uc_fec_array_index",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array_index:\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 dest_val 0 dest_type 0 action 0 nof_entries 0 inc_cpu_code 0\r\n"
    "  inc_vid 0 inc_mac 0\r\n"
#endif
    "",
    "",
    {BIT(9), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_STATISTICS_COUNTER_ID,
    "statistics_counter",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].statistics_counter:\r\n"
    "  Select a statistics counter that is incremented when this entry is accessed.\r\n"
    "  Range 0-7.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 dest_val 0 dest_type 0 action 0 nof_entries 0 inc_cpu_code 0\r\n"
    "  inc_vid 0 inc_mac 0\r\n"
#endif
    "",
    "",
    {BIT(9), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_ENABLE_RPF_ID,
    "enable_rpf",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].enable_rpf:\r\n"
    "  If TRUE, when searching with the IPv4 SrcIP during multicast routing, then\r\n"
    "  use the destination field of the entry for RPF check of multicast packets.\r\n"
    "  This applies only if RPF is not specified in the Multicast FEC Entry. In this\r\n"
    "  case, the Destination must match the incoming port for the RPF check to pass.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 dest_val 0 dest_type 0 action 0 nof_entries 0 inc_cpu_code 0\r\n"
    "  inc_vid 0 inc_mac 0\r\n"
#endif
    "",
    "",
    {BIT(9), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_CPU_CODE_ID,
    "cpu_code",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].cpu_code:\r\n"
    "  Value to be sent to the CPU(in the EPPH). Range 0-1023.Valid only if command\r\n"
    "  is Trap.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 dest_val 0 dest_type 0 action 0 nof_entries 0 inc_cpu_code 0\r\n"
    "  inc_vid 0 inc_mac 0\r\n"
#endif
    "",
    "",
    {BIT(9), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_OUT_VID_ID,
    "out_vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].out_vid:\r\n"
    "  Outgoing VLAN ID.Rang 0-4095. \r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 dest_val 0 dest_type 0 action 0 nof_entries 0 inc_cpu_code 0\r\n"
    "  inc_vid 0 inc_mac 0\r\n"
#endif
    "",
    "",
    {BIT(9), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_MAC_ADDR_ID,
    "mac_addr",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].mac_addr:\r\n"
    "  MAC address to be used as DA for outgoing packet. (next hop MAC address)\r\n"
    "  Valid only if command is not Trap.Range 0-4095.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 dest_val 0 dest_type 0 action 0 nof_entries 0 inc_cpu_code 0\r\n"
    "  inc_vid 0 inc_mac 0\r\n"
#endif
    "",
    "",
    {BIT(9), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_DEST_DEST_VAL_ID,
    "dest_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].dest.dest_val:\r\n"
    "  The destination value, according to the type (Single, LAG,LIF...)For LAG the\r\n"
    "  value is the group ID.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 dest_val 0 dest_type 0 action 0 nof_entries 0 inc_cpu_code 0\r\n"
    "  inc_vid 0 inc_mac 0\r\n"
#endif
    "",
    "",
    {BIT(9), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_DEST_DEST_TYPE_ID,
    "dest_type",
    (PARAM_VAL_RULES *)&SWP_SAND_PP_DEST_TYPE_rule[0],
    (sizeof(SWP_SAND_PP_DEST_TYPE_rule) / sizeof(SWP_SAND_PP_DEST_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].dest.dest_type:\r\n"
    "  The type of the following destination ID\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 dest_val 0 dest_type 0 action 0 nof_entries 0 inc_cpu_code 0\r\n"
    "  inc_vid 0 inc_mac 0\r\n"
#endif
    "",
    "",
    {BIT(9), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_ACTION_ID,
    "action",
    (PARAM_VAL_RULES *)&SOC_PETRA_PP_SAND_PP_FEC_COMMAND_TYPE_rule[0],
    (sizeof(SOC_PETRA_PP_SAND_PP_FEC_COMMAND_TYPE_rule) / sizeof(SOC_PETRA_PP_SAND_PP_FEC_COMMAND_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].action:\r\n"
    "  Drop, Route or Trap\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 dest_val 0 dest_type 0 action 0 nof_entries 0 inc_cpu_code 0\r\n"
    "  inc_vid 0 inc_mac 0\r\n"
#endif
    "",
    "",
    {BIT(9), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_NOF_ENTRIES_ID,
    "nof_entries",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  nof_entries:\r\n"
    "  Number of valid entries in the array. Range 1-16.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 dest_val 0 dest_type 0 action 0 nof_entries 0 inc_cpu_code 0\r\n"
    "  inc_vid 0 inc_mac 0\r\n"
#endif
    "",
    "",
    {BIT(9), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_RANGE_INFO_INC_CPU_CODE_ID,
    "inc_cpu_code",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_cpu_code:\r\n"
    "  By how much to increment the CPU code value in each entry.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 dest_val 0 dest_type 0 action 0 nof_entries 0 inc_cpu_code 0\r\n"
    "  inc_vid 0 inc_mac 0\r\n"
#endif
    "",
    "",
    {BIT(9), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_RANGE_INFO_INC_VID_ID,
    "inc_vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_vid:\r\n"
    "  By how much to increment the VID value in each entry.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 dest_val 0 dest_type 0 action 0 nof_entries 0 inc_cpu_code 0\r\n"
    "  inc_vid 0 inc_mac 0\r\n"
#endif
    "",
    "",
    {BIT(9), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_RANGE_INFO_INC_MAC_ID,
    "inc_mac",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_mac:\r\n"
    "  By how much to increment the MAC value in each entry. For ECMP this the added\r\n"
    "  value for the corresponding FEC in the previous ECMP. Zero to make no change. \r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 dest_val 0 dest_type 0 action 0 nof_entries 0 inc_cpu_code 0\r\n"
    "  inc_vid 0 inc_mac 0\r\n"
#endif
    "",
    "",
    {BIT(9), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_ID,
    "mc_fec_range_add",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Add Range of Multicast FEC entries, so Multicast packets lookup result with\r\n"
    "  this sys_fec_id are routed according to this FEC.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  dest_val 0 dest_type 0 action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
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
    PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_SYS_FEC_NDX_ID,
    "sys_fec_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC id. Range 0-(4G-1)\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  dest_val 0 dest_type 0 action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
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
    PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_UP_TO_SYS_FEC_NDX_ID,
    "up_to_sys_fec_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  up_to_sys_fec_ndx:\r\n"
    "  range upper limit of system FECs to add . Range 0-(4G-1)\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  dest_val 0 dest_type 0 action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
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
    PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_STATISTICS_COUNTER_ID,
    "statistics_counter",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.statistics_counter:\r\n"
    "  Select a statistics counter that is incremented when this entry is accessed.\r\n"
    "  Range 0-7.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  dest_val 0 dest_type 0 action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
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
    PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_CPU_CODE_ID,
    "cpu_code",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.cpu_code:\r\n"
    "  Value to be sent to the CPU(in the EPPH). Range 0-1023.Valid only if command\r\n"
    "  is Trap.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  dest_val 0 dest_type 0 action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
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
    PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_PORT_SYS_ID_ID,
    "sys_id",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.rpf_check.exp_in_port.sys_id:\r\n"
    "  The system port value, according to the type (Single or LAG)For LAG the value\r\n"
    "  is the group ID.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  dest_val 0 dest_type 0 action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
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
    PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_PORT_SYS_PORT_TYPE_ID,
    "sys_port_type",
    (PARAM_VAL_RULES *)&SOC_SAND_PP_SYS_PORT_TYPE_rule[0],
    (sizeof(SOC_SAND_PP_SYS_PORT_TYPE_rule) / sizeof(SOC_SAND_PP_SYS_PORT_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.rpf_check.exp_in_port.sys_port_type:\r\n"
    "  The system port type single/LAG.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  dest_val 0 dest_type 0 action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
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
    PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_VID_ID,
    "exp_in_vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.rpf_check.exp_in_vid:\r\n"
    "  Expected incoming VID to use in the RPF check. If the incoming packet VID is\r\n"
    "  different than this value and check_vid is TRUE then the RPF check fail.\r\n"
    "  valid only if the rpf check consider the VID is TRUE. Range 1-4095.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  dest_val 0 dest_type 0 action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
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
    PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_DEST_DEST_VAL_ID,
    "dest_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.dest.dest_val:\r\n"
    "  The destination value, according to the type (Single, LAG,LIF...)For LAG the\r\n"
    "  value is the group ID.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  dest_val 0 dest_type 0 action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
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
    PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_DEST_DEST_TYPE_ID,
    "dest_type",
    (PARAM_VAL_RULES *)&SWP_SAND_PP_DEST_TYPE_rule[0],
    (sizeof(SWP_SAND_PP_DEST_TYPE_rule) / sizeof(SWP_SAND_PP_DEST_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.dest.dest_type:\r\n"
    "  The type of the following destination ID\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  dest_val 0 dest_type 0 action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
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
    PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_ACTION_ID,
    "action",
    (PARAM_VAL_RULES *)&SOC_PETRA_PP_SAND_PP_FEC_COMMAND_TYPE_rule[0],
    (sizeof(SOC_PETRA_PP_SAND_PP_FEC_COMMAND_TYPE_rule) / sizeof(SOC_PETRA_PP_SAND_PP_FEC_COMMAND_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.action:\r\n"
    "  Drop, Route or Trap\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  dest_val 0 dest_type 0 action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
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
    PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_RANGE_INFO_INC_CPU_CODE_ID,
    "inc_cpu_code",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_cpu_code:\r\n"
    "  By how much to increment the CPU code value in each entry.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  dest_val 0 dest_type 0 action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
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
    PARAM_SWP_P_PP_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_RANGE_INFO_INC_DEST_ID,
    "inc_dest",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_dest:\r\n"
    "  By how much to increment the Destination Multicast Group value in each entry.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  dest_val 0 dest_type 0 action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
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
    PARAM_SWP_P_PP_FEC_RANGE_REMOVE_RANGE_REMOVE_ID,
    "range_remove",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove Range FEC entries.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec range_remove sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
#endif
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
    PARAM_SWP_P_PP_FEC_RANGE_REMOVE_RANGE_REMOVE_SYS_FEC_NDX_ID,
    "sys_fec_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC id. Range 0-(4G-1)\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec range_remove sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(11), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_RANGE_REMOVE_RANGE_REMOVE_UP_TO_SYS_FEC_NDX_ID,
    "up_to_sys_fec_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  up_to_sys_fec_ndx:\r\n"
    "  range upper limit of system FECs to add . Range 0-(4G-1)\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec range_remove sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(11), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_FEC_TABLE_CLEAR_TABLE_CLEAR_ID,
    "table_clear",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove Range FEC entries.\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec table_clear remove_default 0\r\n"
#endif
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
    PARAM_SWP_P_PP_FEC_TABLE_CLEAR_TABLE_CLEAR_REMOVE_DEFAULT_ID,
    "remove_default",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  remove_default:\r\n"
    "  whether to remove the Default entry. (system FEC 0).\r\n"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api fec table_clear remove_default 0\r\n"
#endif
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
    PARAM_SWP_P_PP_API_FEC_ID,
    "fec",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(8)/*|BIT(9)*/|BIT(10)/*|BIT(11)*/|BIT(12), 0, 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } fec*/
#ifdef UI_IPV4_MC/* { ipv4_mc*/
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_INIT_ID,
    "init",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  INIT ipv4 mc module\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc init use_mapped_port \r\n"
#endif
    "",
    "",
    {BIT(13), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_INIT_USE_PORT_ID,
    "use_port",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  IF presents use dest IP x src IP x in port \r\n"
    "  IF does NOT present use dest IP x src IP x in VLAN ID for routing \r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc init use_mapped_port \r\n"
#endif
    "",
    "",
    {BIT(13), 0, 0, 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ID,
    "add",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Binds between an ipv4 key (Src-Ip/prefix and group) and a multicast FEC\r\n"
    "  identifier.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_add in_vid_mask 0 in_vid 0 src_ip_address\r\n"
    "0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {BIT(14), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_IN_VID_VALID_ID,
    "in_vid_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid_mask:\r\n"
    "  Is the in_vid valid.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_add in_vid_mask 0 in_vid 0 src_ip_address\r\n"
    "0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {BIT(14), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_IN_VID_ID,
    "in_vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid:\r\n"
    "  The vid of the incoming packets.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_add in_vid_mask 0 in_vid 0 src_ip_address\r\n"
    "0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {BIT(14), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_COUNT_ID,
    "count",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid:\r\n"
    "  The vid of the incoming packets.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_add in_vid_mask 0 in_vid 0 src_ip_address\r\n"
    "0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {BIT(14), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_INC_SYS_FEC_NDX_ID,
    "inc_sys_fec",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid:\r\n"
    "  The vid of the incoming packets.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_add in_vid_mask 0 in_vid 0 src_ip_address\r\n"
    "0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {BIT(14), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_IN_PORT_VALID_ID,
    "in_port_valid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port_valid:\r\n"
    "  Is the in_port valid.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_add in_port_valid 0 in_port\r\n"
    "0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {BIT(14), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_IN_PORT_ID,
    "in_port",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port:\r\n"
    "  The port of the incoming packets.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_add in_port_valid 0 in_port\r\n"
    "0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {BIT(14), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_GROUP_PREFIX_LEN_ID,
    "group_prefix_len",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_add in_vid_mask 0 in_vid 0 src_ip_address\r\n"
    "0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {BIT(14), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_GROUP_IP_ADDRESS_ID,
    "group_ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_add in_vid_mask 0 in_vid 0 src_ip_address\r\n"
    "0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {BIT(14), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_SOURCE_PREFIX_LEN_ID,
    "src_prefix_len",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.source.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_add in_vid_mask 0 in_vid 0 src_ip_address\r\n"
    "0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {BIT(14), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_SOURCE_IP_ADDRESS_ID,
    "src_ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.source.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_add in_vid_mask 0 in_vid 0 src_ip_address\r\n"
    "0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {BIT(14), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_SYS_FEC_NDX_ID,
    "sys_fec_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC id. Range 0- 4G\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_add in_vid_mask 0 in_vid 0 src_ip_address\r\n"
    "0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {BIT(14), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_OVERRIDE_ID,
    "override",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  override:\r\n"
    "  whether to override the entry if it already exists, if this is\r\n"
    "  FALSE and the entry exist error is returned.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_add in_vid_mask 0 in_vid 0 src_ip_address\r\n"
    "0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {BIT(14), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ID,
    "get",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Gets the system-FEC-id associated the given key.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_get in_vid_mask 0 in_vid\r\n"
    "  0 group_prefix_len 0 group_ip_address 0 src_prefix_len 0 src_ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(15), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_IN_VID_VALID_ID,
    "in_vid_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid_mask:\r\n"
    "  Is the in_vid valid.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_get in_vid_mask 0 in_vid\r\n"
    "  0 group_prefix_len 0 group_ip_address 0 src_prefix_len 0 src_ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(15), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_IN_VID_ID,
    "in_vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid:\r\n"
    "  The vid of the incoming packets.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_get in_vid_mask 0 in_vid\r\n"
    "  0 group_prefix_len 0 group_ip_address 0 src_prefix_len 0 src_ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(15), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_IN_PORT_VALID_ID,
    "in_port_valid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port_valid:\r\n"
    "  Is the in_port valid.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_get in_port_valid 0 in_port\r\n"
    "  0 group_prefix_len 0 group_ip_address 0 src_prefix_len 0 src_ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(15), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_IN_PORT_ID,
    "in_port",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port:\r\n"
    "  The port of the incoming packets.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_get in_port_valid 0 in_port\r\n"
    "  0 group_prefix_len 0 group_ip_address 0 src_prefix_len 0 src_ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(15), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_GROUP_PREFIX_LEN_ID,
    "group_prefix_len",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_get in_vid_mask 0 in_vid\r\n"
    "  0 group_prefix_len 0 group_ip_address 0 src_prefix_len 0 src_ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(15), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_GROUP_IP_ADDRESS_ID,
    "group_ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_get in_vid_mask 0 in_vid\r\n"
    "  0 group_prefix_len 0 group_ip_address 0 src_prefix_len 0 src_ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(15), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_SOURCE_PREFIX_LEN_ID,
    "src_prefix_len",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.source.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_get in_vid_mask 0 in_vid\r\n"
    "  0 group_prefix_len 0 group_ip_address 0 src_prefix_len 0 src_ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(15), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_SOURCE_IP_ADDRESS_ID,
    "src_ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.source.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_get in_vid_mask 0 in_vid\r\n"
    "  0 group_prefix_len 0 group_ip_address 0 src_prefix_len 0 src_ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(15), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ID,
    "lpm_get",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get from the route table a route key with the longest prefix that matches the\r\n"
    "  given key.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_lpm_get in_vid_mask 0\r\n"
    "  in_vid 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(16), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_IN_VID_VALID_ID,
    "in_vid_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid_mask:\r\n"
    "  Is the in_vid valid.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_lpm_get in_vid_mask 0\r\n"
    "  in_vid 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(16), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_IN_VID_ID,
    "in_vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid:\r\n"
    "  The vid of the incoming packets.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_lpm_get in_vid_mask 0\r\n"
    "  in_vid 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(16), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_IN_PORT_VALID_ID,
    "in_port_valid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port_valid:\r\n"
    "  Is the in_port valid.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_lpm_get in_port_valid 0\r\n"
    "  in_port 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(16), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_IN_PORT_ID,
    "in_port",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port:\r\n"
    "  The port of the incoming packets.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_lpm_get in_port_valid 0\r\n"
    "  in_port 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(16), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_GROUP_PREFIX_LEN_ID,
    "group_prefix_len",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_lpm_get in_vid_mask 0\r\n"
    "  in_vid 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(16), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_GROUP_IP_ADDRESS_ID,
    "group_ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_lpm_get in_vid_mask 0\r\n"
    "  in_vid 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(16), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_SOURCE_PREFIX_LEN_ID,
    "src_prefix_len",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.source.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_lpm_get in_vid_mask 0\r\n"
    "  in_vid 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(16), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_SOURCE_IP_ADDRESS_ID,
    "src_ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.source.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_lpm_get in_vid_mask 0\r\n"
    "  in_vid 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(16), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_BLOCK_IPV4_MC_ROUTE_GET_BLOCK_ID,
    "get_block",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get block of entries from route table that match the keys between start key\r\n"
    "  and last key.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_get_block block_range 0\r\n"
#endif
    "",
    "",
    {BIT(17), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_GET_BLOCK_IPV4_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_ID,
    "block_range",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  block_range:\r\n"
    "  block range.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_get_block block_range 0\r\n"
#endif
    "",
    "",
    {BIT(17), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ID,
    "remove",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove the association of the IP-address to Unicast FEC.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_remove in_vid_mask 0 in_vid 0\r\n"
    "  src_ip_address 0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0\r\n"
#endif
    "",
    "",
    {BIT(18), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_IN_VID_VALID_ID,
    "in_vid_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid_mask:\r\n"
    "  Is the in_vid valid.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_remove in_vid_mask 0 in_vid 0\r\n"
    "  src_ip_address 0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0\r\n"
#endif
    "",
    "",
    {BIT(18), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_IN_VID_ID,
    "in_vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid:\r\n"
    "  The vid of the incoming packets.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_remove in_vid_mask 0 in_vid 0\r\n"
    "  src_ip_address 0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0\r\n"
#endif
    "",
    "",
    {BIT(18), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_IN_PORT_VALID_ID,
    "in_port_valid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port_valid:\r\n"
    "  Is the in_port valid.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_remove in_vid_mask 0 in_vid 0\r\n"
    "  src_ip_address 0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0\r\n"
#endif
    "",
    "",
    {BIT(18), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_IN_PORT_ID,
    "in_port",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port:\r\n"
    "  The port of the incoming packets.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_remove in_vid_mask 0 in_vid 0\r\n"
    "  src_ip_address 0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0\r\n"
#endif
    "",
    "",
    {BIT(18), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_GROUP_PREFIX_LEN_ID,
    "group_prefix_len",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_remove in_vid_mask 0 in_vid 0\r\n"
    "  src_ip_address 0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0\r\n"
#endif
    "",
    "",
    {BIT(18), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_GROUP_IP_ADDRESS_ID,
    "group_ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_remove in_vid_mask 0 in_vid 0\r\n"
    "  src_ip_address 0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0\r\n"
#endif
    "",
    "",
    {BIT(18), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_SOURCE_PREFIX_LEN_ID,
    "src_prefix_len",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.source.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_remove in_vid_mask 0 in_vid 0\r\n"
    "  src_ip_address 0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0\r\n"
#endif
    "",
    "",
    {BIT(18), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_SOURCE_IP_ADDRESS_ID,
    "src_ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.source.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "  swp_p_pp_api ipv4_mc ipv4_mc_route_remove in_vid_mask 0 in_vid 0\r\n"
    "  src_ip_address 0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0\r\n"
#endif
    "",
    "",
    {BIT(18), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV4_MC_ID,
    "ipv4_mc",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(13)|BIT(14)|BIT(15)|BIT(16)|BIT(17)|BIT(18)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } ipv4_mc*/


#ifdef UI_IPV6/* { ipv6*/
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_INIT_ID,
    "init",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  INIT ipv6 module\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 init use_mapped_port \r\n"
#endif
    "",
    "",
    {BIT(19), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_INIT_USE_PORT_ID,
    "use_port",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  IF presents use dest IP x in port \r\n"
    "  IF does NOT present use dest IP x in VLAN ID for routing \r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 init use_mapped_port \r\n"
#endif
    "",
    "",
    {BIT(19), 0, 0, 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },    {
    PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ID,
    "add",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Binds between an ipv6 key (Src-Ip/prefix and group) and a  FEC\r\n"
    "  identifier.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_add in_vid_mask 0 in_vid 0 dest_ip_address\r\n"
    "  0 prefix_len 0 ip_address 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {BIT(20), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_IN_VID_VALID_ID,
    "in_vid_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid_mask:\r\n"
    "  Is the in_vid valid.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_add in_vid_mask 0 in_vid 0 dest_ip_address\r\n"
    "  0 prefix_len 0 ip_address 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {BIT(20), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_IN_VID_ID,
    "in_vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid:\r\n"
    "  The vid of the incoming packets.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_add in_vid_mask 0 in_vid 0 dest_ip_address\r\n"
    "  0 prefix_len 0 ip_address 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {BIT(20), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_IN_PORT_VALID_ID,
    "in_port_valid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port_valid:\r\n"
    "  Is the in_port valid.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_add in_port_valid 0 in_port\r\n"
    "  0 prefix_len 0 ip_address 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {BIT(20), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_IN_PORT_ID,
    "in_port",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port:\r\n"
    "  The port of the incoming packets.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_add in_port_valid 0 in_port\r\n"
    "  0 prefix_len 0 ip_address 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {BIT(20), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_GROUP_PREFIX_LEN_ID,
    "dest_prefix_len",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_add in_vid_mask 0 in_vid 0 dest_ip_address\r\n"
    "  0 prefix_len 0 ip_address 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {BIT(20), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_GROUP_IP_ADDRESS_ID,
    "dest_ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ipv6_addr_vals[0],
    (sizeof(soc_petra_pp_api_ipv6_addr_vals) / sizeof(soc_petra_pp_api_ipv6_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.ip_address:\r\n"
    "  IPV6 destination address.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_add in_vid_mask 0 in_vid 0 dest_ip_address\r\n"
    "  0 prefix_len 0 ip_address 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {BIT(20), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_COUNT_ID,
    "count",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_add in_vid_mask 0 in_vid 0 dest_ip_address\r\n"
    "  0 prefix_len 0 ip_address 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {BIT(20), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_SYS_FEC_NDX_ID,
    "sys_fec_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC id. Range 0- 4G\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_add in_vid_mask 0 in_vid 0 dest_ip_address\r\n"
    "  0 prefix_len 0 ip_address 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {BIT(20), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_INC_SYS_FEC_NDX_ID,
    "inc_sys_fec",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC id. Range 0- 4G\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_add in_vid_mask 0 in_vid 0 dest_ip_address\r\n"
    "  0 prefix_len 0 ip_address 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {BIT(20), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_OVERRIDE_ID,
    "override",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  override:\r\n"
    "  whether to override the entry if it already exists, if this is\r\n"
    "  FALSE and the entry exist error is returned.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_add in_vid_mask 0 in_vid 0 dest_ip_address\r\n"
    "  0 prefix_len 0 ip_address 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {BIT(20), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ID,
    "get",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Gets the system-FEC-id associated the given key.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_get in_vid_mask 0 in_vid\r\n"
    "  0 prefix_len 0 dest_ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(21), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_IN_VID_VALID_ID,
    "in_vid_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid_mask:\r\n"
    "  Is the in_vid valid.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_get in_vid_mask 0 in_vid\r\n"
    "  0 prefix_len 0 dest_ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(21), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_IN_VID_ID,
    "in_vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid:\r\n"
    "  The vid of the incoming packets.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_get in_vid_mask 0 in_vid\r\n"
    "  0 prefix_len 0 dest_ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(21), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_IN_PORT_VALID_ID,
    "in_port_valid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port_valid:\r\n"
    "  Is the in_port valid.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_get in_port_valid 0 in_port\r\n"
    "  0 prefix_len 0 dest_ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(21), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_IN_PORT_ID,
    "in_port",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port:\r\n"
    "  The port of the incoming packets.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_get in_port_valid 0 in_port\r\n"
    "  0 prefix_len 0 dest_ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(21), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_GROUP_PREFIX_LEN_ID,
    "dest_prefix_len",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_get in_vid_mask 0 in_vid\r\n"
    "  0 prefix_len 0 dest_ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(21), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_GROUP_IP_ADDRESS_ID,
    "dest_ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ipv6_addr_vals[0],
    (sizeof(soc_petra_pp_api_ipv6_addr_vals) / sizeof(soc_petra_pp_api_ipv6_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.ip_address:\r\n"
    "  IPV6 destination address.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_get in_vid_mask 0 in_vid\r\n"
    "  0 prefix_len 0 dest_ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(21), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_SWP_P_PP_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ID,
    "lpm_get",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get from the route table a route key with the longest prefix that matches the\r\n"
    "  given key.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_lpm_get in_vid_mask 0\r\n"
    "  in_vid 0 dest_ip_address 0 prefix_len 0\r\n"
#endif
    "",
    "",
    {BIT(22), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_IN_VID_VALID_ID,
    "in_vid_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid_mask:\r\n"
    "  Is the in_vid valid.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_lpm_get in_vid_mask 0\r\n"
    "  in_vid 0 dest_ip_address 0 prefix_len 0\r\n"
#endif
    "",
    "",
    {BIT(22), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_IN_VID_ID,
    "in_vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid:\r\n"
    "  The vid of the incoming packets.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_lpm_get in_vid_mask 0\r\n"
    "  in_vid 0 dest_ip_address 0 prefix_len 0\r\n"
#endif
    "",
    "",
    {BIT(22), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_GROUP_PREFIX_LEN_ID,
    "prefix_len",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_lpm_get in_vid_mask 0\r\n"
    "  in_vid 0 dest_ip_address 0 prefix_len 0\r\n"
#endif
    "",
    "",
    {BIT(22), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_GROUP_IP_ADDRESS_ID,
    "dest_ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ipv6_addr_vals[0],
    (sizeof(soc_petra_pp_api_ipv6_addr_vals) / sizeof(soc_petra_pp_api_ipv6_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.ip_address:\r\n"
    "  IPV6 destination address.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_lpm_get in_vid_mask 0\r\n"
    "  in_vid 0 dest_ip_address 0 prefix_len 0\r\n"
#endif
    "",
    "",
    {BIT(22), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_IN_PORT_VALID_ID,
    "in_port_valid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port_valid:\r\n"
    "  Is the in_port valid.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 lpm_get in_port_valid 0\r\n"
    "  in_port 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(22), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_IN_PORT_ID,
    "in_port",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port:\r\n"
    "  The port of the incoming packets.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 lpm_get in_port_valid 0\r\n"
    "  in_port 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0\r\n"
#endif
    "",
    "",
    {BIT(22), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_SWP_P_PP_IPV6_ROUTE_GET_BLOCK_IPV6_ROUTE_GET_BLOCK_ID,
    "get_block",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get block of entries from route table that match the keys between start key\r\n"
    "  and last key.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_get_block start_from 0\r\n"
#endif
    "",
    "",
    {BIT(23), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_GET_BLOCK_IPV6_ROUTE_GET_BLOCK_BLOCK_RANGE_ID,
    "start_from",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  start_from:\r\n"
    "  block range.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_get_block start_from 0\r\n"
#endif
    "",
    "",
    {BIT(23), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ID,
    "remove",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove the association of the IP-address to Unicast FEC.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_remove in_vid_mask 0\r\n"
    "  in_vid 0 dest_ip_address 0 prefix_len 0\r\n"
#endif
    "",
    "",
    {BIT(24), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_IN_VID_VALID_ID,
    "in_vid_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid_mask:\r\n"
    "  Is the in_vid valid.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_remove in_vid_mask 0\r\n"
    "  in_vid 0 dest_ip_address 0 prefix_len 0\r\n"
#endif
    "",
    "",
    {BIT(24), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_IN_VID_ID,
    "in_vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid:\r\n"
    "  The vid of the incoming packets.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_remove in_vid_mask 0\r\n"
    "  in_vid 0 dest_ip_address 0 prefix_len 0\r\n"
#endif
    "",
    "",
    {BIT(24), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_IN_PORT_VALID_ID,
    "in_port_valid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port_valid:\r\n"
    "  Is the in port valid.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_remove in_vid_mask 0\r\n"
    "  in_vid 0 dest_ip_address 0 prefix_len 0\r\n"
#endif
    "",
    "",
    {BIT(24), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_IN_PORT_ID,
    "in_port",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid:\r\n"
    "  The vid of the incoming packets.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_remove in_vid_mask 0\r\n"
    "  in_vid 0 dest_ip_address 0 prefix_len 0\r\n"
#endif
    "",
    "",
    {BIT(24), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_GROUP_PREFIX_LEN_ID,
    "prefix_len",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_remove in_vid_mask 0\r\n"
    "  in_vid 0 dest_ip_address 0 prefix_len 0\r\n"
#endif
    "",
    "",
    {BIT(24), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_GROUP_IP_ADDRESS_ID,
    "dest_ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ipv6_addr_vals[0],
    (sizeof(soc_petra_pp_api_ipv6_addr_vals) / sizeof(soc_petra_pp_api_ipv6_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.ip_address:\r\n"
    "  IPV6 destination address.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv6 ipv6_route_remove in_vid_mask 0\r\n"
    "  in_vid 0 dest_ip_address 0 prefix_len 0\r\n"
#endif
    "",
    "",
    {BIT(24), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_IPV6_ID,
    "ipv6",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(19)|BIT(20)|BIT(21)|BIT(22)|BIT(23)|BIT(24)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } ipv6*/

#ifdef UI_SWP_P_PP_APP/* { swp_p_pp_app*/
  {
    PARAM_SWP_P_PP_APP_APP_ID,
    "clear",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Start the sweep application, prepare the system to receive and send traffic.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api swp_p_pp_app app silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(25)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_APP_APP_SILENT_ID,
    "silent",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  silent:\r\n"
    "  Indicator:\r\n"
    "  TRUE - Print messages while running this API.\r\n"
    "  FALSE - Do not print messages while running this API.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api swp_p_pp_app app silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(25)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_APP_RESET_APP_RESET_ID,
    "reset",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Reset the sweep application to the configuration at the end of the\r\n"
    "  initialization.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api swp_p_pp_app app_reset silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(24)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_APP_RESET_APP_RESET_SILENT_ID,
    "silent",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  silent:\r\n"
    "  Indicator:\r\n"
    "  TRUE - Print messages while running this API.\r\n"
    "  FALSE - Do not print messages while running this API.\r\n"
    "",
#ifdef UI_SWP_P_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api swp_p_pp_app app_reset silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(24)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_APP_ID,
    "utils",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, 0, 0, BIT(24)|BIT(25)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } swp_p_pp_app*/
#ifdef UI_INIT/* { init*/
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_ID,
    "sequence",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Device initialization: hardware interfaces and basic configuration. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_NOF_ECMPS_ID,
    "nof_ecmps",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l3_info.nof_ecmps:\r\n"
    "  Number of ECMPs to init the FEC table with.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_NOF_MC_ROUTES_ID,
    "nof_mc_routes",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l3_info.nof_mc_routes:\r\n"
    "  Number of MC routes to init the routing table with.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_STATISTICS_COUNTER_ID,
    "statistics_counter",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l3_info.default_mc_fec.statistics_counter:\r\n"
    "  Select a statistics counter that is incremented when this entry is accessed.\r\n"
    "  Range 0-7.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_RPF_CHECK_EXP_IN_PORT_SYS_ID_ID,
    "sys_id",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l3_info.default_mc_fec.rpf_check.exp_in_port.sys_id:\r\n"
    "  The system port value, according to the type (Single or GLAG)For LAG the\r\n"
    "  value is the group ID.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_RPF_CHECK_EXP_IN_PORT_SYS_PORT_TYPE_ID,
    "sys_port_type",
    (PARAM_VAL_RULES *)&SWP_SAND_PP_SYS_PORT_TYPE_rule[0],
    (sizeof(SWP_SAND_PP_SYS_PORT_TYPE_rule) / sizeof(SWP_SAND_PP_SYS_PORT_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l3_info.default_mc_fec.rpf_check.exp_in_port.sys_port_type:\r\n"
    "  The system port type single/LAG.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_RPF_CHECK_EXP_IN_VID_ID,
    "exp_in_vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l3_info.default_mc_fec.rpf_check.exp_in_vid:\r\n"
    "  Expected incoming VID to use in the RPF check. If the incoming packet VID is\r\n"
    "  different than this value and check_vid is TRUE then the RPF check fail.\r\n"
    "  valid only if the rpf check consider the VID is TRUE. Range 1-4095.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_RPF_CHECK_RPF_CHECK_ID,
    "rpf_check",
    (PARAM_VAL_RULES *)&SOC_PETRA_PP_FEC_RPF_CHECK_rule[0],
    (sizeof(SOC_PETRA_PP_FEC_RPF_CHECK_rule) / sizeof(SOC_PETRA_PP_FEC_RPF_CHECK_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l3_info.default_mc_fec.rpf_check.rpf_check:\r\n"
    "  According to which information to perform the rpf check.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_CPU_CODE_ID,
    "cpu_code",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l3_info.default_mc_fec.cpu_code:\r\n"
    "  Value to be sent to the CPU(in the EPPH). Range 0-1023.Valid only if command\r\n"
    "  is Trap.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_FRWRD_DESTINATION_DEST_VAL_ID,
    "dest_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l3_info.default_mc_fec.frwrd.destination.dest_val:\r\n"
    "  The destination value, according to the type (Single, RLAG or GLAG,LIF...)For\r\n"
    "  LAG the value is the group ID.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_FRWRD_DESTINATION_DEST_TYPE_ID,
    "dest_type",
    (PARAM_VAL_RULES *)&SWP_SAND_PP_DEST_TYPE_rule[0],
    (sizeof(SWP_SAND_PP_DEST_TYPE_rule) / sizeof(SWP_SAND_PP_DEST_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l3_info.default_mc_fec.frwrd.destination.dest_type:\r\n"
    "  The type of the following destination ID\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_FRWRD_OVERWRITE_COS_ID,
    "overwrite_cos",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l3_info.default_mc_fec.frwrd.overwrite_cos:\r\n"
    "  If TRUE, then the layer 2 CoS should be overwritten with the CoS specified by\r\n"
    "  CoS-Action\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_MC_FEC_FRWRD_COMMAND_ID,
    "command",
    (PARAM_VAL_RULES *)&SWP_SAND_FEC_COMMAND_TYPE_rule[0],
    (sizeof(SWP_SAND_FEC_COMMAND_TYPE_rule) / sizeof(SWP_SAND_FEC_COMMAND_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l3_info.default_mc_fec.frwrd.command:\r\n"
    "  Drop, Route or Trap\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_UC_FEC_STATISTICS_COUNTER_ID,
    "statistics_counter",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l3_info.default_uc_fec.statistics_counter:\r\n"
    "  Select a statistics counter that is incremented when this entry is accessed.\r\n"
    "  Range 0-7.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_UC_FEC_ENABLE_RPF_ID,
    "enable_rpf",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l3_info.default_uc_fec.enable_rpf:\r\n"
    "  If TRUE, when searching with the IPv4 SrcIP during multicast routing, then\r\n"
    "  use the destination field of the entry for RPF check of multicast packets.\r\n"
    "  This applies only if RPF is not specified in the Multicast FEC Entry. In this\r\n"
    "  case, the Destination must match the incoming port for the RPF check to pass.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_UC_FEC_CPU_CODE_ID,
    "cpu_code",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l3_info.default_uc_fec.cpu_code:\r\n"
    "  Value to be sent to the CPU(in the EPPH). Range 0-1023.Valid only if command\r\n"
    "  is Trap.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_UC_FEC_L2_EDIT_ARP_PTR_ID,
    "mac_addr",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l3_info.default_uc_fec.l2_edit.arp_ptr:\r\n"
    "  pointer to a MAC address to be used as DA for outgoing packet. Valid only if\r\n"
    "  command is not Trap.Range 0-4095.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_UC_FEC_L2_EDIT_OUT_VID_ID,
    "out_vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l3_info.default_uc_fec.l2_edit.out_vid:\r\n"
    "  Outgoing VLAN ID.Rang 0-4095. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_UC_FEC_FRWRD_DESTINATION_DEST_VAL_ID,
    "dest_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l3_info.default_uc_fec.frwrd.destination.dest_val:\r\n"
    "  The destination value, according to the type (Single, RLAG or GLAG,LIF...)For\r\n"
    "  LAG the value is the group ID.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_UC_FEC_FRWRD_DESTINATION_DEST_TYPE_ID,
    "dest_type",
    (PARAM_VAL_RULES *)&SWP_SAND_PP_DEST_TYPE_rule[0],
    (sizeof(SWP_SAND_PP_DEST_TYPE_rule) / sizeof(SWP_SAND_PP_DEST_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l3_info.default_uc_fec.frwrd.destination.dest_type:\r\n"
    "  The type of the following destination ID\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_UC_FEC_FRWRD_OVERWRITE_COS_ID,
    "overwrite_cos",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l3_info.default_uc_fec.frwrd.overwrite_cos:\r\n"
    "  If TRUE, then the layer 2 CoS should be overwritten with the CoS specified by\r\n"
    "  CoS-Action\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_DEFAULT_UC_FEC_FRWRD_COMMAND_ID,
    "command",
    (PARAM_VAL_RULES *)&SWP_SAND_FEC_COMMAND_TYPE_rule[0],
    (sizeof(SWP_SAND_FEC_COMMAND_TYPE_rule) / sizeof(SWP_SAND_FEC_COMMAND_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l3_info.default_uc_fec.frwrd.command:\r\n"
    "  Drop, Route or Trap\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_MY_MAC_ID,
    "my_mac",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l3_info.my_mac:\r\n"
    "  The MAC address of the device. Incoming packets with DA equal to this MAC\r\n"
    "  will be forwarded to the router engine. Also packets routed by this device\r\n"
    "  will be with SA MAC equal to this MAC address.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L3_INFO_L3_INIT_ID,
    "l3_init",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l3_info.l3_init:\r\n"
    "  If to init the l3 mini application.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L2_INFO_MACT_INFO_NOF_MC_MAC_ID,
    "nof_mc_mac",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l2_info.mact_info.nof_mc_mac:\r\n"
    "  number of MC mac addresses\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L2_INFO_MACT_INFO_AGING_TIME_ID,
    "aging_time",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l2_info.mact_info.aging_time:\r\n"
    "  The time (in seconds) of the aging cycle\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L2_INFO_MACT_INFO_ENABLE_AGING_ID,
    "enable_aging",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l2_info.mact_info.enable_aging:\r\n"
    "  whether to enable the aging on the MAC table\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L2_INFO_MACT_INIT_ID,
    "mact_init",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l2_info.mact_init:\r\n"
    "  whether to init the MACT table. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L2_INFO_NOF_SYS_PORTS_ID,
    "nof_sys_ports",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l2_info.nof_sys_ports:\r\n"
    "  Number of system ports. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_L2_INFO_NOF_VLANS_ID,
    "nof_vlans",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.l2_info.nof_vlans:\r\n"
    "  Number of VLANs to initialize.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SWEEP_INFO_OP_MODE_ID,
    "op_mode",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sweep_info.op_mode:\r\n"
    "  Soc_petra PP operational mode\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_DEVICE_INIT_SEQUENCE_SEQUENCE_SILENT_ID,
    "silent",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  silent:\r\n"
    "  Indicator.\r\n"
    "  TRUE - Print messages while running this API.\r\n"
    "  FALSE - Do not print messages while running this API.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api init sequence nof_ecmps 0 nof_mc_routes 0 statistics_counter 0\r\n"
    "  sys_id 0 sys_port_type 0 exp_in_vid 0 rpf_check 0 cpu_code 0 dest_val 0\r\n"
    "  dest_type 0 overwrite_cos 0 command 0 statistics_counter 0 enable_rpf 0\r\n"
    "  cpu_code 0 arp_ptr 0 out_vid 0 dest_val 0 dest_type 0 overwrite_cos 0 command\r\n"
    "  0 my_mac 0 l3_init 0 nof_mc_mac 0 aging_time 0 enable_aging 0 mact_init 0\r\n"
    "  nof_sys_ports 0 nof_vlans 0 op_mode 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_INIT_ID,
    "init",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } init*/
#ifdef UI_VLAN/* { vlan*/
  {
    PARAM_SWP_P_PP_VLAN_SET_SET_ID,
    "set",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the VLAN information.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api vlan set vlan_ndx 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(27)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_VLAN_SET_SET_VLAN_NDX_ID,
    "vlan_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vlan_ndx:\r\n"
    "  the VLAN ID to configure.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api vlan set vlan_ndx 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(27)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_VLAN_SET_SET_VLAN_UP_TO_ID,
    "up_to",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vlan_ndx:\r\n"
    "  to set Range of VLANS ID to configure.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api vlan set vlan_ndx 0 valid 0 topology 0 fld_group 0 membership 1 2 3 tagged 1\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(27)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_VLAN_SET_SET_VLAN_INFO_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vlan_info.valid:\r\n"
    "  If TRUE then VLAN is valid. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api vlan set vlan_ndx 0 valid 0 topology 0 fld_group 0 membership 1 2 3 tagged 1\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(27)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_VLAN_SET_SET_VLAN_PORTS_ENABLE_FLDING_ID,
    "enable_flooding",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vlan_info.valid:\r\n"
    "  If TRUE then VLAN is valid. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api vlan set vlan_ndx 0 valid 0 topology 0 fld_group 0 membership 1 2 3 tagged 1\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(27)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_VLAN_SET_SET_VLAN_PORTS_TOPLOGY_ID,
    "topology",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vlan_info.topology:\r\n"
    "  The topolgy the vlan is mapped to. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api vlan set vlan_ndx 0 valid 0 topology 0 fld_group 0 membership 1 2 3 tagged 1\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(27)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_VLAN_SET_SET_VLAN_PORTS_TAG_VALID_ID,
    "tagged_ports",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_members_vals[0],
    (sizeof(soc_petra_pp_api_members_vals) / sizeof(soc_petra_pp_api_members_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vlan_info.tagged_ports:\r\n"
    "  whether ALL ports are tagged or untagged.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api vlan set vlan_ndx 0 valid 0 topology 0 fld_group 0 membership 1 2 3 tagged_ports 1\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(27)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_VLAN_SET_SET_VLAN_MEMBERSHIP_PORTS_SET_MEMBER_PORTS_ID,
    "untagged_ports",
    (PARAM_VAL_RULES *)&swp_pp_api_members_vals[0],
    (sizeof(swp_pp_api_members_vals) / sizeof(swp_pp_api_members_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  member_ports:\r\n"
    "  the VLAN membership information: local ports belong to the VLAN.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  soc_petra_pp_api filter vlan_membership_ports_set vid_ndx 0 member_ports 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(27)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_VLAN_GET_GET_ID,
    "get",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the VLAN information.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api vlan get vlan_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(28)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_VLAN_GET_GET_VLAN_NDX_ID,
    "vlan_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vlan_ndx:\r\n"
    "  the VLAN ID to configure.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api vlan get vlan_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_VLAN_MEMBER_ADD_MEMBER_ADD_ID,
    "member_add",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Add system port as a member in the VLAN.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api vlan member_add vlan_ndx 0 sys_port 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(29)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_VLAN_MEMBER_ADD_MEMBER_ADD_VLAN_NDX_ID,
    "vlan_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vlan_ndx:\r\n"
    "  the VLAN ID. Range 0-4095.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api vlan member_add vlan_ndx 0 sys_port 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(29)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_VLAN_MEMBER_ADD_MEMBER_ADD_SYS_PORT_ID,
    "sys_port",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_port:\r\n"
    "  system port.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api vlan member_add vlan_ndx 0 sys_port 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(29)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_VLAN_MEMBER_ADD_MEMBER_ADD_TAGGED_ID,
    "tagged",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_port:\r\n"
    "  system port.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api vlan member_add vlan_ndx 0 sys_port 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(29)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_VLAN_MEMBER_REMOVE_MEMBER_REMOVE_ID,
    "member_remove",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove system port from the VLAN membership.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api vlan member_remove vlan_ndx 0 sys_port 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(30)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_VLAN_MEMBER_REMOVE_MEMBER_REMOVE_VLAN_NDX_ID,
    "vlan_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vlan_ndx:\r\n"
    "  the VLAN ID. Range 0-4095.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api vlan member_remove vlan_ndx 0 sys_port 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(30)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_VLAN_MEMBER_REMOVE_MEMBER_REMOVE_SYS_PORT_ID,
    "sys_port",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_port:\r\n"
    "  system port.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api vlan member_remove vlan_ndx 0 sys_port 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(30)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_VLAN_ID,
    "vlan",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, 0, 0, BIT(27)|BIT(28)|BIT(29)|BIT(30)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } vlan*/
#ifdef UI_MACT/* { mact*/
  {
    PARAM_SWP_P_PP_MACT_ENTRY_ADD_ENTRY_ADD_ID,
    "entry_add",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Add an entry with a given key and value. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact entry_add vid 0 mac 0 dest_val 0 dest_type 0 forward_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_MACT_ENTRY_ADD_ENTRY_ADD_KEY_VID_ID,
    "vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  key.vid:\r\n"
    "  The VID.Range 0-4095\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact entry_add vid 0 mac 0 dest_val 0 dest_type 0 forward_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(31)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_MACT_ENTRY_ADD_ENTRY_ADD_KEY_MAC_ID,
    "mac",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  key.mac:\r\n"
    "  MAC address\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact entry_add vid 0 mac 0 dest_val 0 dest_type 0 forward_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(31)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_DESTINATION_DEST_VAL_ID,
    "dest_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  value.destination.dest_val:\r\n"
    "  The destination value, according to the type (Single, RLAG or GLAG,LIF...)For\r\n"
    "  LAG the value is the group ID.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact entry_add vid 0 mac 0 dest_val 0 dest_type 0 forward_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(31)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_DESTINATION_DEST_TYPE_ID,
    "dest_type",
    (PARAM_VAL_RULES *)&SWP_SAND_PP_DEST_TYPE_rule[0],
    (sizeof(SWP_SAND_PP_DEST_TYPE_rule) / sizeof(SWP_SAND_PP_DEST_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  value.destination.dest_type:\r\n"
    "  The type of the following destination ID\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact entry_add vid 0 mac 0 dest_val 0 dest_type 0 forward_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(31)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_FORWARD_TYPE_ID,
    "forward_type",
    (PARAM_VAL_RULES *)&SWP_SAND_PP_FRWRD_ACTION_TYPE_rule[0],
    (sizeof(SWP_SAND_PP_FRWRD_ACTION_TYPE_rule) / sizeof(SWP_SAND_PP_FRWRD_ACTION_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  value.forward_type:\r\n"
    "  The forwarding action Destination type\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact entry_add vid 0 mac 0 dest_val 0 dest_type 0 forward_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(31)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_IS_DYNAMIC_ID,
    "dynamic",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  dynamic:\r\n"
    "  dynamic/static entry. default is statuc.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact entry_add vid 0 mac 0 dest_val 0 dest_type 0 forward_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(31)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_SA_DROP_ID,
    "sa_drop",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  dynamic:\r\n"
    "  dynamic/static entry. default is statuc.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact entry_add vid 0 mac 0 dest_val 0 dest_type 0 forward_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(31)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_MACT_RANGE_ADD_RANGE_ADD_ID,
    "range_add",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Add a range of MAC entry starting from the MAC in the key parameter up to\r\n"
    "  'up_to_mac' and bind them to the information in value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact range_add vid 0 mac 0 up_to_mac 0 dest_val 0 dest_type 0\r\n"
    "  forward_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_MACT_RANGE_ADD_RANGE_ADD_KEY_VID_ID,
    "vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  key.vid:\r\n"
    "  The VID.Range 0-4095\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact range_add vid 0 mac 0 up_to_mac 0 dest_val 0 dest_type 0\r\n"
    "  forward_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_MACT_RANGE_ADD_RANGE_ADD_KEY_MAC_ID,
    "mac",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  key.mac:\r\n"
    "  MAC address\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact range_add vid 0 mac 0 up_to_mac 0 dest_val 0 dest_type 0\r\n"
    "  forward_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_MACT_RANGE_ADD_RANGE_ADD_UP_TO_MAC_ID,
    "up_to_mac",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  up_to_mac:\r\n"
    "  the last MAC in the range.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact range_add vid 0 mac 0 up_to_mac 0 dest_val 0 dest_type 0\r\n"
    "  forward_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_MACT_RANGE_ADD_RANGE_ADD_VALUE_DESTINATION_DEST_VAL_ID,
    "dest_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  value.destination.dest_val:\r\n"
    "  The destination value, according to the type (Single, RLAG or GLAG,LIF...)For\r\n"
    "  LAG the value is the group ID.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact range_add vid 0 mac 0 up_to_mac 0 dest_val 0 dest_type 0\r\n"
    "  forward_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_MACT_RANGE_ADD_RANGE_ADD_VALUE_DESTINATION_DEST_TYPE_ID,
    "dest_type",
    (PARAM_VAL_RULES *)&SWP_SAND_PP_DEST_TYPE_rule[0],
    (sizeof(SWP_SAND_PP_DEST_TYPE_rule) / sizeof(SWP_SAND_PP_DEST_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  value.destination.dest_type:\r\n"
    "  The type of the following destination ID\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact range_add vid 0 mac 0 up_to_mac 0 dest_val 0 dest_type 0\r\n"
    "  forward_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_MACT_RANGE_ADD_RANGE_ADD_VALUE_FORWARD_TYPE_ID,
    "forward_type",
    (PARAM_VAL_RULES *)&SWP_SAND_PP_FRWRD_ACTION_TYPE_rule[0],
    (sizeof(SWP_SAND_PP_FRWRD_ACTION_TYPE_rule) / sizeof(SWP_SAND_PP_FRWRD_ACTION_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  value.forward_type:\r\n"
    "  The forwarding action Destination type\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact range_add vid 0 mac 0 up_to_mac 0 dest_val 0 dest_type 0\r\n"
    "  forward_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_MACT_ENTRY_REMOVE_ENTRY_REMOVE_ID,
    "entry_remove",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove an entry for given key and value. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact entry_remove vid 0 mac 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_MACT_ENTRY_REMOVE_ENTRY_REMOVE_KEY_VID_ID,
    "vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  key.vid:\r\n"
    "  The VID.Range 0-4095\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact entry_remove vid 0 mac 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_MACT_ENTRY_REMOVE_ENTRY_REMOVE_KEY_MAC_ID,
    "mac",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  key.mac:\r\n"
    "  MAC address\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact entry_remove vid 0 mac 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_MACT_RANGE_REMOVE_RANGE_REMOVE_ID,
    "range_remove",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove a range of MAC entry starting from the MAC in the key parameter up to\r\n"
    "  'up_to_mac'.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact range_remove vid 0 mac 0 up_to_mac 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(2)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_MACT_RANGE_REMOVE_RANGE_REMOVE_KEY_VID_ID,
    "vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  key.vid:\r\n"
    "  The VID.Range 0-4095\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact range_remove vid 0 mac 0 up_to_mac 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(2)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_MACT_RANGE_REMOVE_RANGE_REMOVE_KEY_MAC_ID,
    "mac",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  key.mac:\r\n"
    "  MAC address\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact range_remove vid 0 mac 0 up_to_mac 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(2)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_MACT_RANGE_REMOVE_RANGE_REMOVE_UP_TO_MAC_ID,
    "up_to_mac",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  up_to_mac:\r\n"
    "  the last MAC in the range.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact range_remove vid 0 mac 0 up_to_mac 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(2)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_ID,
    "flush_block",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Flush (clear/transplant) MAC table entries that match a given rule\r\n"
    "  (FID,destination,static). \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact flush_block dest_val 0 dest_type 0 including_dynamic 0\r\n"
    "  including_static 0 dest_val 0 dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
    "  mode 0\r\n"
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
    PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_NEW_DEST_DEST_VAL_ID,
    "dest_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.new_dest.dest_val:\r\n"
    "  The destination value, according to the type (Single, RLAG or GLAG,LIF...)For\r\n"
    "  LAG the value is the group ID.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact flush_block dest_val 0 dest_type 0 including_dynamic 0\r\n"
    "  including_static 0 dest_val 0 dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
    "  mode 0\r\n"
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
    PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_NEW_DEST_DEST_TYPE_ID,
    "dest_type",
    (PARAM_VAL_RULES *)&SWP_SAND_PP_DEST_TYPE_rule[0],
    (sizeof(SWP_SAND_PP_DEST_TYPE_rule) / sizeof(SWP_SAND_PP_DEST_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.new_dest.dest_type:\r\n"
    "  The type of the following destination ID\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact flush_block dest_val 0 dest_type 0 including_dynamic 0\r\n"
    "  including_static 0 dest_val 0 dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
    "  mode 0\r\n"
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
    PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_INCLUDING_DYNAMIC_ID,
    "including_dynamic",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.rule.including_dynamic:\r\n"
    "  If TRUE then dynamic entries are in the match. In flush operation has to be\r\n"
    "  TRUE.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact flush_block dest_val 0 dest_type 0 including_dynamic 0\r\n"
    "  including_static 0 dest_val 0 dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
    "  mode 0\r\n"
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
    PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_INCLUDING_STATIC_ID,
    "including_static",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.rule.including_static:\r\n"
    "  If TRUE then static entries are in the match. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact flush_block dest_val 0 dest_type 0 including_dynamic 0\r\n"
    "  including_static 0 dest_val 0 dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
    "  mode 0\r\n"
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
    PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_DEST_DEST_VAL_ID,
    "dest_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.rule.dest.dest_val:\r\n"
    "  The destination value, according to the type (Single, RLAG or GLAG,LIF...)For\r\n"
    "  LAG the value is the group ID.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact flush_block dest_val 0 dest_type 0 including_dynamic 0\r\n"
    "  including_static 0 dest_val 0 dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
    "  mode 0\r\n"
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
    PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_DEST_DEST_TYPE_ID,
    "dest_type",
    (PARAM_VAL_RULES *)&SWP_SAND_PP_DEST_TYPE_rule[0],
    (sizeof(SWP_SAND_PP_DEST_TYPE_rule) / sizeof(SWP_SAND_PP_DEST_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.rule.dest.dest_type:\r\n"
    "  The type of the following destination ID\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact flush_block dest_val 0 dest_type 0 including_dynamic 0\r\n"
    "  including_static 0 dest_val 0 dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
    "  mode 0\r\n"
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
    PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_COMPARE_DEST_ID,
    "compare_dest",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.rule.compare_dest:\r\n"
    "  Should the destination in the MAC table be compared with the destination in\r\n"
    "  this structure.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact flush_block dest_val 0 dest_type 0 including_dynamic 0\r\n"
    "  including_static 0 dest_val 0 dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
    "  mode 0\r\n"
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
    PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_FID_ID,
    "fid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.rule.fid:\r\n"
    "  When 'compare_fid' is asserted, the FID in the MACT is compared with this\r\n"
    "  value. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact flush_block dest_val 0 dest_type 0 including_dynamic 0\r\n"
    "  including_static 0 dest_val 0 dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
    "  mode 0\r\n"
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
    PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_COMPARE_FID_ID,
    "compare_fid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.rule.compare_fid:\r\n"
    "  Should the FID in the MACT be compared with the FID in this structure\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact flush_block dest_val 0 dest_type 0 including_dynamic 0\r\n"
    "  including_static 0 dest_val 0 dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
    "  mode 0\r\n"
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
    PARAM_SWP_P_PP_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_MODE_ID,
    "mode",
    (PARAM_VAL_RULES *)&SOC_PETRA_PP_MACT_FLUSH_MODE_rule[0],
    (sizeof(SOC_PETRA_PP_MACT_FLUSH_MODE_rule) / sizeof(SOC_PETRA_PP_MACT_FLUSH_MODE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.mode:\r\n"
    "  MACT flush mode, Clear, Transplant, Clear All, Transplant All.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact flush_block dest_val 0 dest_type 0 including_dynamic 0\r\n"
    "  including_static 0 dest_val 0 dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
    "  mode 0\r\n"
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
    PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_ID,
    "get_block",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Flush (clear/transplant) MAC table entries that match a given rule\r\n"
    "  (FID,destination,static). \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact get_block entries_to_act 0 entries_to_scan 0 iter 0\r\n"
    "  including_dynamic 0 including_static 0 dest_val 0 dest_type 0 compare_dest 0\r\n"
    "  fid 0 compare_fid 0\r\n"
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
    PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RANGE_INFO_ENTRIES_TO_ACT_ID,
    "entries_to_act",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.entries_to_act:\r\n"
    "  Number of entries to read/modify.Stop After retrieving this number of entries.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact get_block entries_to_act 0 entries_to_scan 0 iter 0\r\n"
    "  including_dynamic 0 including_static 0 dest_val 0 dest_type 0 compare_dest 0\r\n"
    "  fid 0 compare_fid 0\r\n"
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
    PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RANGE_INFO_ENTRIES_TO_SCAN_ID,
    "entries_to_scan",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.entries_to_scan:\r\n"
    "  The number of entries to scan.Stop After scanning this number of entries.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact get_block entries_to_act 0 entries_to_scan 0 iter 0\r\n"
    "  including_dynamic 0 including_static 0 dest_val 0 dest_type 0 compare_dest 0\r\n"
    "  fid 0 compare_fid 0\r\n"
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
    PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RANGE_INFO_ITER_ID,
    "iter",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.iter:\r\n"
    "  The index to start reading from in the MACT\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact get_block entries_to_act 0 entries_to_scan 0 iter 0\r\n"
    "  including_dynamic 0 including_static 0 dest_val 0 dest_type 0 compare_dest 0\r\n"
    "  fid 0 compare_fid 0\r\n"
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
    PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RULE_INCLUDING_DYNAMIC_ID,
    "including_dynamic",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  rule.including_dynamic:\r\n"
    "  If TRUE then dynamic entries are in the match. In flush operation has to be\r\n"
    "  TRUE.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact get_block entries_to_act 0 entries_to_scan 0 iter 0\r\n"
    "  including_dynamic 0 including_static 0 dest_val 0 dest_type 0 compare_dest 0\r\n"
    "  fid 0 compare_fid 0\r\n"
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
    PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RULE_INCLUDING_STATIC_ID,
    "including_static",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  rule.including_static:\r\n"
    "  If TRUE then static entries are in the match. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact get_block entries_to_act 0 entries_to_scan 0 iter 0\r\n"
    "  including_dynamic 0 including_static 0 dest_val 0 dest_type 0 compare_dest 0\r\n"
    "  fid 0 compare_fid 0\r\n"
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
    PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RULE_DEST_DEST_VAL_ID,
    "dest_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  rule.dest.dest_val:\r\n"
    "  The destination value, according to the type (Single, RLAG or GLAG,LIF...)For\r\n"
    "  LAG the value is the group ID.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact get_block entries_to_act 0 entries_to_scan 0 iter 0\r\n"
    "  including_dynamic 0 including_static 0 dest_val 0 dest_type 0 compare_dest 0\r\n"
    "  fid 0 compare_fid 0\r\n"
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
    PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RULE_DEST_DEST_TYPE_ID,
    "dest_type",
    (PARAM_VAL_RULES *)&SWP_SAND_PP_DEST_TYPE_rule[0],
    (sizeof(SWP_SAND_PP_DEST_TYPE_rule) / sizeof(SWP_SAND_PP_DEST_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  rule.dest.dest_type:\r\n"
    "  The type of the following destination ID\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact get_block entries_to_act 0 entries_to_scan 0 iter 0\r\n"
    "  including_dynamic 0 including_static 0 dest_val 0 dest_type 0 compare_dest 0\r\n"
    "  fid 0 compare_fid 0\r\n"
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
    PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RULE_COMPARE_DEST_ID,
    "compare_dest",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  rule.compare_dest:\r\n"
    "  Should the destination in the MAC table be compared with the destination in\r\n"
    "  this structure.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact get_block entries_to_act 0 entries_to_scan 0 iter 0\r\n"
    "  including_dynamic 0 including_static 0 dest_val 0 dest_type 0 compare_dest 0\r\n"
    "  fid 0 compare_fid 0\r\n"
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
    PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RULE_FID_ID,
    "fid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  rule.fid:\r\n"
    "  When 'compare_fid' is asserted, the FID in the MACT is compared with this\r\n"
    "  value. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact get_block entries_to_act 0 entries_to_scan 0 iter 0\r\n"
    "  including_dynamic 0 including_static 0 dest_val 0 dest_type 0 compare_dest 0\r\n"
    "  fid 0 compare_fid 0\r\n"
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
    PARAM_SWP_P_PP_MACT_GET_BLOCK_GET_BLOCK_RULE_COMPARE_FID_ID,
    "compare_fid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  rule.compare_fid:\r\n"
    "  Should the FID in the MACT be compared with the FID in this structure\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact get_block entries_to_act 0 entries_to_scan 0 iter 0\r\n"
    "  including_dynamic 0 including_static 0 dest_val 0 dest_type 0 compare_dest 0\r\n"
    "  fid 0 compare_fid 0\r\n"
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
    PARAM_SWP_P_PP_MACT_GET_ALL_GET_ALL_ID,
    "get_all",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get all the MAC table entries. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact get_all\r\n"
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
    PARAM_SWP_P_PP_MACT_PRINT_ALL_PRINT_ALL_ID,
    "print_all",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  print the MAC table entries. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact print_all\r\n"
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
    PARAM_SWP_P_PP_MACT_AGING_INFO_SET_AGING_INFO_SET_ID,
    "aging_info_set",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the aging information of the MACT. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact aging_info_set nano_sec 0 sec 0 enable_aging 0\r\n"
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
    PARAM_SWP_P_PP_MACT_AGING_INFO_SET_AGING_INFO_SET_AGING_INFO_META_CYCLE_NANO_SEC_ID,
    "nano_sec",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  aging_info.meta_cycle.nano_sec:\r\n"
    "  nano-secondsRange 0-999,999,999\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact aging_info_set nano_sec 0 sec 0 enable_aging 0\r\n"
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
    PARAM_SWP_P_PP_MACT_AGING_INFO_SET_AGING_INFO_SET_AGING_INFO_META_CYCLE_SEC_ID,
    "sec",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  aging_info.meta_cycle.sec:\r\n"
    "  seconds\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact aging_info_set nano_sec 0 sec 0 enable_aging 0\r\n"
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
    PARAM_SWP_P_PP_MACT_AGING_INFO_SET_AGING_INFO_SET_AGING_INFO_ENABLE_AGING_ID,
    "enable_aging",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  aging_info.enable_aging:\r\n"
    "  If TRUE then enable the aging process of the device (aging machine).\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact aging_info_set nano_sec 0 sec 0 enable_aging 0\r\n"
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
    PARAM_SWP_P_PP_MACT_AGING_INFO_GET_AGING_INFO_GET_ID,
    "aging_info_get",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the aging information of the MACT. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact aging_info_get\r\n"
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
    PARAM_SWP_P_PP_MACT_LEARN_MGMT_SET_LEARN_MGMT_SET_ID,
    "learn_mgmt_set",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the operation mode and management of the MACT and the destination to\r\n"
    "  inform upon learning/refresh events.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact learn_mgmt_set dest_val 0 dest_type 0 oper_mode 0\r\n"
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
    PARAM_SWP_P_PP_MACT_LEARN_MGMT_SET_LEARN_MGMT_SET_LEARN_INFO_DESTINATION_DEST_VAL_ID,
    "dest_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  learn_info.destination.dest_val:\r\n"
    "  The destination value, according to the type (Single, RLAG or GLAG,LIF...)For\r\n"
    "  LAG the value is the group ID.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact learn_mgmt_set dest_val 0 dest_type 0 oper_mode 0\r\n"
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
    PARAM_SWP_P_PP_MACT_LEARN_MGMT_SET_LEARN_MGMT_SET_LEARN_INFO_DESTINATION_DEST_TYPE_ID,
    "dest_type",
    (PARAM_VAL_RULES *)&SWP_SAND_PP_DEST_TYPE_rule[0],
    (sizeof(SWP_SAND_PP_DEST_TYPE_rule) / sizeof(SWP_SAND_PP_DEST_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  learn_info.destination.dest_type:\r\n"
    "  The type of the following destination ID\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact learn_mgmt_set dest_val 0 dest_type 0 oper_mode 0\r\n"
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
    PARAM_SWP_P_PP_MACT_LEARN_MGMT_SET_LEARN_MGMT_SET_LEARN_INFO_OPER_MODE_ID,
    "oper_mode",
    (PARAM_VAL_RULES *)&SOC_PETRA_PP_MACT_OPERATION_MODE_rule[0],
    (sizeof(SOC_PETRA_PP_MACT_OPERATION_MODE_rule) / sizeof(SOC_PETRA_PP_MACT_OPERATION_MODE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  learn_info.oper_mode:\r\n"
    "  How to manage MAC Tables in a system Distributed/Centralized by CPU/the\r\n"
    "  device. CPU is not supported.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact learn_mgmt_set dest_val 0 dest_type 0 oper_mode 0\r\n"
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
    PARAM_SWP_P_PP_MACT_LEARN_MGMT_SET_LEARN_MGMT_SET_LEARN_INFO_OLP_PORT_TYPE_ID,
    "olp_port_type",
    (PARAM_VAL_RULES *)&SOC_PETRA_PORT_HEADER_TYPE_rule[0],
    (sizeof(SOC_PETRA_PORT_HEADER_TYPE_rule) / sizeof(SOC_PETRA_PORT_HEADER_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  learn_info.oper_mode:\r\n"
    "  How to manage MAC Tables in a system Distributed/Centralized by CPU/the\r\n"
    "  device. CPU is not supported.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact learn_mgmt_set dest_val 0 dest_type 0 oper_mode 0\r\n"
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
    PARAM_SWP_P_PP_MACT_LEARN_MGMT_GET_LEARN_MGMT_GET_ID,
    "learn_mgmt_get",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the operation mode and management of the MACT and the destination to\r\n"
    "  inform upon learning/refresh events.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api mact learn_mgmt_get\r\n"
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
    PARAM_SWP_P_PP_API_MACT_ID,
    "mact",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, 0, 0, BIT(31), BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(7)|BIT(8)|BIT(9)|BIT(10)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } mact*/
#ifdef UI_IPV4_UC/* { ipv4_uc*/
  {
    PARAM_SWP_P_PP_IPV4_UC_ROUTE_ADD_ROUTE_ADD_ID,
    "add",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Binds between Ipv4 MC and UC route key (IP-address/prefix) and a next hop. As\r\n"
    "  a result of this operation Ipv4 packets designated to IP address match the\r\n"
    "  given key (as long there is no more specific route key) will be routed to the\r\n"
    "  given next hop.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc add pref 0 ip_address 0 mac 0 vid 0 dest_val\r\n"
    "  0 dest_type 0 override 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_ROUTE_ADD_ROUTE_ADD_SUBNET_PREFIX_LEN_ID,
    "pref",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  subnet.pref:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and pref 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc add pref 0 ip_address 0 mac 0 vid 0 dest_val\r\n"
    "  0 dest_type 0 override 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_ROUTE_ADD_ROUTE_ADD_SUBNET_IP_ADDRESS_ID,
    "ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  subnet.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc add pref 0 ip_address 0 mac 0 vid 0 dest_val\r\n"
    "  0 dest_type 0 override 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_ROUTE_ADD_ROUTE_ADD_OVERRIDE_ID,
    "override",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  override:\r\n"
    "  whether to override the entry if it already exists, if this is\r\n"
    "  FALSE and the entry exist error is returned.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc add pref 0 ip_address 0 mac 0 vid 0 dest_val\r\n"
    "  0 dest_type 0 override 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_ROUTE_ADD_ROUTE_ADD_SYS_FE_ID_ID,
    "sys_fec_id",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  System-FEC-id:\r\n"
    "  FEC id (next hop Id) to bind the given subnet with\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc add pref 0 ip_address 0 mac 0 vid 0 dest_val\r\n"
    "  0 dest_type 0 override 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_ROUTE_REMOVE_ROUTE_REMOVE_ID,
    "remove",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove Ipv4 MC and UC route key (IP-address/prefix) from the routing table.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc remove pref 0 ip_address 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_ROUTE_REMOVE_ROUTE_REMOVE_SUBNET_PREFIX_LEN_ID,
    "pref",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  subnet.pref:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and pref 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc remove pref 0 ip_address 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_ROUTE_REMOVE_ROUTE_REMOVE_SUBNET_IP_ADDRESS_ID,
    "ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  subnet.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc remove pref 0 ip_address 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_RANGE_ADD_RANGE_ADD_ID,
    "range_add",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Binds between Ipv4 Unicast route key (IP-address/prefix) and a Unicast FEC\r\n"
    "  entry identified by the given sys_fec_id for a given virtual router. As a\r\n"
    "  result of this operation Unicast Ipv4 packets designated to IP address match\r\n"
    "  the given key (as long there is no more specific route key) will be routed\r\n"
    "  according to the information in the FEC entry identified by sys_fec_id.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc range_add step 0 count 0 prefix 0 to 0 from 0 type 0 mac\r\n"
    "  0 vid 0 dest_val 0 dest_type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_RANGE_ADD_RANGE_ADD_RANGE_INFO_STEP_ID,
    "step",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.step:\r\n"
    "  Number of step between to consecutive IP addresses. Relevant only for\r\n"
    "  increment and decrement.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc range_add step 0 count 0 prefix 0 to 0 from 0 type 0 mac\r\n"
    "  0 vid 0 dest_val 0 dest_type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_RANGE_ADD_RANGE_ADD_RANGE_INFO_COUNT_ID,
    "count",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.count:\r\n"
    "  Number of IP addresses to add.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc range_add step 0 count 0 prefix 0 to 0 from 0 type 0 mac\r\n"
    "  0 vid 0 dest_val 0 dest_type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_RANGE_ADD_RANGE_ADD_RANGE_INFO_PREFIX_ID,
    "prefix",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.prefix:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and pref 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc range_add step 0 count 0 prefix 0 to 0 from 0 type 0 mac\r\n"
    "  0 vid 0 dest_val 0 dest_type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_RANGE_ADD_RANGE_ADD_RANGE_INFO_TO_ID,
    "to",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.to:\r\n"
    "  IP address, range ends at this IP address. Relevant only for random. For\r\n"
    "  increment/decrement types the end of the range is determined according to the\r\n"
    "  count and step.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc range_add step 0 count 0 prefix 0 to 0 from 0 type 0 mac\r\n"
    "  0 vid 0 dest_val 0 dest_type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_RANGE_ADD_RANGE_ADD_RANGE_INFO_FROM_ID,
    "from",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.from:\r\n"
    "  IP address, range starts from this IP address.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc range_add step 0 count 0 prefix 0 to 0 from 0 type 0 mac\r\n"
    "  0 vid 0 dest_val 0 dest_type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_RANGE_ADD_RANGE_ADD_RANGE_INFO_TYPE_ID,
    "type",
    (PARAM_VAL_RULES *)&SWP_P_PP_IPV4_UC_RANGE_TYPE_rule[0],
    (sizeof(SWP_P_PP_IPV4_UC_RANGE_TYPE_rule) / sizeof(SWP_P_PP_IPV4_UC_RANGE_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.type:\r\n"
    "  How to pick up addresses from the given range randomly incrementally etc.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc range_add step 0 count 0 prefix 0 to 0 from 0 type 0 mac\r\n"
    "  0 vid 0 dest_val 0 dest_type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_RANGE_ADD_RANGE_ADD_SYS_FEC_ID_ID,
    "sys_fec_id",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  System-FEC-id:\r\n"
    "  FEC id (next hop Id) to bind the given subnet with\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc range_add step 0 count 0 prefix 0 to 0 from 0 type 0 mac\r\n"
    "  0 vid 0 dest_val 0 dest_type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_RANGE_ADD_RANGE_ADD_INC_SYS_FEC_ID_ID,
    "inc_sys_fec",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  inc_sys_fec:\r\n"
    "  whether to increment the FEC id for each subnet or stay with the given sys-fec-id\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc range_add step 0 count 0 prefix 0 to 0 from 0 type 0 mac\r\n"
    "  0 vid 0 dest_val 0 dest_type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_RANGE_REMOVE_RANGE_REMOVE_ID,
    "range_remove",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove Range of Ipv4 Unicast routes (IP-address/prefix) the given virtual\r\n"
    "  routers.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc range_remove step 0 count 0 prefix 0 to 0 from 0 type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_RANGE_REMOVE_RANGE_REMOVE_RANGE_INFO_STEP_ID,
    "step",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.step:\r\n"
    "  Number of step between to consecutive IP addresses. Relevant only for\r\n"
    "  increment and decrement.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc range_remove step 0 count 0 prefix 0 to 0 from 0 type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_RANGE_REMOVE_RANGE_REMOVE_RANGE_INFO_COUNT_ID,
    "count",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.count:\r\n"
    "  Number of IP addresses to add.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc range_remove step 0 count 0 prefix 0 to 0 from 0 type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_RANGE_REMOVE_RANGE_REMOVE_RANGE_INFO_PREFIX_ID,
    "prefix",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.prefix:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and pref 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc range_remove step 0 count 0 prefix 0 to 0 from 0 type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_RANGE_REMOVE_RANGE_REMOVE_RANGE_INFO_TO_ID,
    "to",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.to:\r\n"
    "  IP address, range ends at this IP address. Relevant only for random. For\r\n"
    "  increment/decrement types the end of the range is determined according to the\r\n"
    "  count and step.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc range_remove step 0 count 0 prefix 0 to 0 from 0 type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_RANGE_REMOVE_RANGE_REMOVE_RANGE_INFO_FROM_ID,
    "from",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.from:\r\n"
    "  IP address, range starts from this IP address.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc range_remove step 0 count 0 prefix 0 to 0 from 0 type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_RANGE_REMOVE_RANGE_REMOVE_RANGE_INFO_TYPE_ID,
    "type",
    (PARAM_VAL_RULES *)&SWP_P_PP_IPV4_UC_RANGE_TYPE_rule[0],
    (sizeof(SWP_P_PP_IPV4_UC_RANGE_TYPE_rule) / sizeof(SWP_P_PP_IPV4_UC_RANGE_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.type:\r\n"
    "  How to pick up addresses from the given range randomly incrementally etc.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc range_remove step 0 count 0 prefix 0 to 0 from 0 type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_ROUTE_GET_ALL_ROUTE_GET_ALL_ID,
    "route_get_all",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get block of entries from the routing table in a given range.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc route_get_all order 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_ROUTE_GET_ALL_ROUTE_GET_ALL_ORDER_ID,
    "order",
    (PARAM_VAL_RULES *)&SOC_PETRA_PP_L3_TABLE_ITER_TYPE_rule[0],
    (sizeof(SOC_PETRA_PP_L3_TABLE_ITER_TYPE_rule) / sizeof(SOC_PETRA_PP_L3_TABLE_ITER_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  order:\r\n"
    "  in which order to get the routing table.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc route_get_all order 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_ROUTE_GET_BLOCK_ROUTE_GET_BLOCK_ID,
    "get_block",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get block of entries from the routing table in a given range.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc get_block entries_to_act 0 entries_to_scan 0\r\n"
    "  payload 0 type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_ROUTE_GET_BLOCK_ROUTE_GET_BLOCK_RANGE_ENTRIES_TO_ACT_ID,
    "entries_to_act",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range.entries_to_act:\r\n"
    "  Number of entries to read/modify.Stop After retrieving this number of entries.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc get_block entries_to_act 0 entries_to_scan 0\r\n"
    "  payload 0 type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_ROUTE_GET_BLOCK_ROUTE_GET_BLOCK_RANGE_ENTRIES_TO_SCAN_ID,
    "entries_to_scan",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range.entries_to_scan:\r\n"
    "  The number of entries to scan.Stop After scanning this number of entries.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc get_block entries_to_act 0 entries_to_scan 0\r\n"
    "  payload 0 type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_ROUTE_GET_BLOCK_ROUTE_GET_BLOCK_RANGE_START_PAYLOAD_ID,
    "payload",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range.start.payload:\r\n"
    "  Iterator information to be used to traverse the L3 routing tables.- this\r\n"
    "  information should be changed only by the given macros.- To set the iterator\r\n"
    "  to start scanning the routing table from the\r\n"
    "  beginning use SWP_P_PP_L3_TABLE_ITER_BEGIN()- To check if the returned\r\n"
    "  iteratror points to the end of the table use SWP_P_PP_L3_TABLE_ITER_IS_END()\r\n"
    "  - See a concrete example of using this Type at\r\n"
    "  swp_p_pp_ipv4_unicast_route_get_block()\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc get_block entries_to_act 0 entries_to_scan 0\r\n"
    "  payload 0 type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_ROUTE_GET_BLOCK_ROUTE_GET_BLOCK_RANGE_START_TYPE_ID,
    "type",
    (PARAM_VAL_RULES *)&SOC_PETRA_PP_L3_TABLE_ITER_TYPE_rule[0],
    (sizeof(SOC_PETRA_PP_L3_TABLE_ITER_TYPE_rule) / sizeof(SOC_PETRA_PP_L3_TABLE_ITER_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range.start.type:\r\n"
    "  The type of the iterator. (fast, ordered ...)\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc get_block entries_to_act 0 entries_to_scan 0\r\n"
    "  payload 0 type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_UC_ROUTE_PRINT_ALL_ROUTE_CLEAR_ALL_ID,
    "clear_all",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Clear the routing table entries. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc clear \r\n"
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
    PARAM_SWP_P_PP_IPV4_RANGE_ADD_IPV4_RANGE_ADD_ID,
    "ipv4_range_add",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Binds between Ipv4 route keys (IP-address/prefix) and a next hop.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_range_add step 0 count 0 prefix 0 to 0 from 0 type\r\n"
    "  0 mac 0 vid 0 dest_val 0 dest_type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_RANGE_ADD_IPV4_RANGE_ADD_RANGE_INFO_STEP_ID,
    "step",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.step:\r\n"
    "  Number of step between to consecutive IP addresses. Relevant only for\r\n"
    "  increment and decrement.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_range_add step 0 count 0 prefix 0 to 0 from 0 type\r\n"
    "  0 mac 0 vid 0 dest_val 0 dest_type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_RANGE_ADD_IPV4_RANGE_ADD_RANGE_INFO_COUNT_ID,
    "count",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.count:\r\n"
    "  Number of IP addresses to add.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_range_add step 0 count 0 prefix 0 to 0 from 0 type\r\n"
    "  0 mac 0 vid 0 dest_val 0 dest_type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_RANGE_ADD_IPV4_RANGE_ADD_RANGE_INFO_PREFIX_ID,
    "prefix",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.prefix:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and pref 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_range_add step 0 count 0 prefix 0 to 0 from 0 type\r\n"
    "  0 mac 0 vid 0 dest_val 0 dest_type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_RANGE_ADD_IPV4_RANGE_ADD_RANGE_INFO_TO_ID,
    "to",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.to:\r\n"
    "  IP address, range ends at this IP address. Relevant only for random. For\r\n"
    "  increment/decrement types the end of the range is determined according to the\r\n"
    "  count and step.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_range_add step 0 count 0 prefix 0 to 0 from 0 type\r\n"
    "  0 mac 0 vid 0 dest_val 0 dest_type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_RANGE_ADD_IPV4_RANGE_ADD_RANGE_INFO_FROM_ID,
    "from",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.from:\r\n"
    "  IP address, range starts from this IP address.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_range_add step 0 count 0 prefix 0 to 0 from 0 type\r\n"
    "  0 mac 0 vid 0 dest_val 0 dest_type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_RANGE_ADD_IPV4_RANGE_ADD_RANGE_INFO_TYPE_ID,
    "type",
    (PARAM_VAL_RULES *)&SWP_P_PP_IPV4_UC_RANGE_TYPE_rule[0],
    (sizeof(SWP_P_PP_IPV4_UC_RANGE_TYPE_rule) / sizeof(SWP_P_PP_IPV4_UC_RANGE_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.type:\r\n"
    "  How to pick up addresses from the given range randomly incrementally etc.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_range_add step 0 count 0 prefix 0 to 0 from 0 type\r\n"
    "  0 mac 0 vid 0 dest_val 0 dest_type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_RANGE_ADD_IPV4_RANGE_ADD_NEXT_HOP_MAC_ID,
    "mac",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  next_hop.mac:\r\n"
    "  The MAC address of the next hop.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_range_add step 0 count 0 prefix 0 to 0 from 0 type\r\n"
    "  0 mac 0 vid 0 dest_val 0 dest_type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_RANGE_ADD_IPV4_RANGE_ADD_NEXT_HOP_VID_ID,
    "vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  next_hop.vid:\r\n"
    "  The VLAN id, to set to the routed packets. Relevant only if destination is\r\n"
    "  system port.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_range_add step 0 count 0 prefix 0 to 0 from 0 type\r\n"
    "  0 mac 0 vid 0 dest_val 0 dest_type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_RANGE_ADD_IPV4_RANGE_ADD_NEXT_HOP_DESTINATION_DEST_VAL_ID,
    "dest_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  next_hop.destination.dest_val:\r\n"
    "  The destination value, according to the type (Single, RLAG or GLAG,LIF...)For\r\n"
    "  LAG the value is the group ID.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_range_add step 0 count 0 prefix 0 to 0 from 0 type\r\n"
    "  0 mac 0 vid 0 dest_val 0 dest_type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_RANGE_ADD_IPV4_RANGE_ADD_NEXT_HOP_DESTINATION_DEST_TYPE_ID,
    "dest_type",
    (PARAM_VAL_RULES *)&SWP_SAND_PP_DEST_TYPE_rule[0],
    (sizeof(SWP_SAND_PP_DEST_TYPE_rule) / sizeof(SWP_SAND_PP_DEST_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  next_hop.destination.dest_type:\r\n"
    "  The type of the following destination ID\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_range_add step 0 count 0 prefix 0 to 0 from 0 type\r\n"
    "  0 mac 0 vid 0 dest_val 0 dest_type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_RANGE_REMOVE_IPV4_RANGE_REMOVE_ID,
    "ipv4_range_remove",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove Range of Ipv4 routes (IP-address/prefix) the given virtual routers.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_range_remove step 0 count 0 prefix 0 to 0 from 0\r\n"
    "  type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_RANGE_REMOVE_IPV4_RANGE_REMOVE_RANGE_INFO_STEP_ID,
    "step",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.step:\r\n"
    "  Number of step between to consecutive IP addresses. Relevant only for\r\n"
    "  increment and decrement.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_range_remove step 0 count 0 prefix 0 to 0 from 0\r\n"
    "  type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_RANGE_REMOVE_IPV4_RANGE_REMOVE_RANGE_INFO_COUNT_ID,
    "count",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.count:\r\n"
    "  Number of IP addresses to add.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_range_remove step 0 count 0 prefix 0 to 0 from 0\r\n"
    "  type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_RANGE_REMOVE_IPV4_RANGE_REMOVE_RANGE_INFO_PREFIX_ID,
    "prefix",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.prefix:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and pref 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_range_remove step 0 count 0 prefix 0 to 0 from 0\r\n"
    "  type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_RANGE_REMOVE_IPV4_RANGE_REMOVE_RANGE_INFO_TO_ID,
    "to",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.to:\r\n"
    "  IP address, range ends at this IP address. Relevant only for random. For\r\n"
    "  increment/decrement types the end of the range is determined according to the\r\n"
    "  count and step.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_range_remove step 0 count 0 prefix 0 to 0 from 0\r\n"
    "  type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_RANGE_REMOVE_IPV4_RANGE_REMOVE_RANGE_INFO_FROM_ID,
    "from",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.from:\r\n"
    "  IP address, range starts from this IP address.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_range_remove step 0 count 0 prefix 0 to 0 from 0\r\n"
    "  type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_RANGE_REMOVE_IPV4_RANGE_REMOVE_RANGE_INFO_TYPE_ID,
    "type",
    (PARAM_VAL_RULES *)&SWP_P_PP_IPV4_UC_RANGE_TYPE_rule[0],
    (sizeof(SWP_P_PP_IPV4_UC_RANGE_TYPE_rule) / sizeof(SWP_P_PP_IPV4_UC_RANGE_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.type:\r\n"
    "  How to pick up addresses from the given range randomly incrementally etc.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_range_remove step 0 count 0 prefix 0 to 0 from 0\r\n"
    "  type 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_ROUTING_TBL_GET_ALL_IPV4_ROUTING_TBL_GET_ALL_ID,
    "ipv4_routing_tbl_get_all",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get the IPv4 routing table. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_routing_tbl_get_all\r\n"
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
    PARAM_SWP_P_PP_IPV4_ROUTING_TBL_GET_BLOCK_IPV4_ROUTING_TBL_GET_BLOCK_ID,
    "ipv4_routing_tbl_get_block",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get the IPv4 routing table. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_routing_tbl_get_block payload 0 type 0\r\n"
    "  entries_to_scan 0 entries_to_get 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_ROUTING_TBL_GET_BLOCK_IPV4_ROUTING_TBL_GET_BLOCK_ITER_PAYLOAD_ID,
    "payload",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  iter.payload:\r\n"
    "  Iterator information to be used to traverse the L3 routing tables.- this\r\n"
    "  information should be changed only by the given macros.- To set the iterator\r\n"
    "  to start scanning the routing table from the\r\n"
    "  beginning use SWP_P_PP_L3_TABLE_ITER_BEGIN()- To check if the returned\r\n"
    "  iteratror points to the end of the table use SWP_P_PP_L3_TABLE_ITER_IS_END()\r\n"
    "  - See a concrete example of using this Type at\r\n"
    "  swp_p_pp_ipv4_unicast_route_get_block()\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_routing_tbl_get_block payload 0 type 0\r\n"
    "  entries_to_scan 0 entries_to_get 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_ROUTING_TBL_GET_BLOCK_IPV4_ROUTING_TBL_GET_BLOCK_ITER_TYPE_ID,
    "type",
    (PARAM_VAL_RULES *)&SOC_PETRA_PP_L3_TABLE_ITER_TYPE_rule[0],
    (sizeof(SOC_PETRA_PP_L3_TABLE_ITER_TYPE_rule) / sizeof(SOC_PETRA_PP_L3_TABLE_ITER_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  iter.type:\r\n"
    "  The type of the iterator. (fast, ordered ...)\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_routing_tbl_get_block payload 0 type 0\r\n"
    "  entries_to_scan 0 entries_to_get 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_ROUTING_TBL_GET_BLOCK_IPV4_ROUTING_TBL_GET_BLOCK_ENTRIES_TO_SCAN_ID,
    "entries_to_scan",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entries_to_scan:\r\n"
    "  Number of entries to scan in the routing table\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_routing_tbl_get_block payload 0 type 0\r\n"
    "  entries_to_scan 0 entries_to_get 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_ROUTING_TBL_GET_BLOCK_IPV4_ROUTING_TBL_GET_BLOCK_ENTRIES_TO_GET_ID,
    "entries_to_get",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entries_to_get:\r\n"
    "  Number of entries to get from the routing table\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_routing_tbl_get_block payload 0 type 0\r\n"
    "  entries_to_scan 0 entries_to_get 0\r\n"
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
    PARAM_SWP_P_PP_IPV4_ROUTING_TBL_PRINT_ALL_IPV4_ROUTING_TBL_PRINT_ALL_ID,
    "ipv4_routing_tbl_print_all",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Print the routing table. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api ipv4_uc ipv4_routing_tbl_print_all\r\n"
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
    PARAM_SWP_P_PP_API_IPV4_UC_ID,
    "ipv4_uc",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, 0, 0, 0, BIT(11)|BIT(12)|BIT(13)|BIT(14)|BIT(16)|BIT(17)|BIT(18)|BIT(19)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } ipv4_uc*/

#ifdef UI_ACL/* { acl*/
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ID,
    "add",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Binds between ACE and a action. As a result of this operation packets match\r\n"
    "  the ACE will be process according to the given action.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0,  BIT(2)|BIT(7)|BIT(8)|BIT(9)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACL_NDX_ID,
    "acl_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acl_ndx:\r\n"
    "  ACL id.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(2)|BIT(7)|BIT(8)|BIT(9)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_L2_ID,
    "l2",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_empty_vals[0],
    (sizeof(soc_petra_pp_api_empty_vals) / sizeof(soc_petra_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Add ACE (Access Control Entry) to the given ACL. On packet lookup, between\r\n"
    "  all the ACEs that match the key constructed from the packet, the ACE with the\r\n"
    "  lowest priority will be chosen. For example If 'priority-1' < 'priority-2',\r\n"
    "  and both match the given key then 'priority-1' will be chosen.\r\n"
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(2)|BIT(7)|BIT(9)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_L3A_ID,
    "l3a",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_empty_vals[0],
    (sizeof(soc_petra_pp_api_empty_vals) / sizeof(soc_petra_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Add ACE (Access Control Entry) to the given ACL. On packet lookup, between\r\n"
    "  all the ACEs that match the key constructed from the packet, the ACE with the\r\n"
    "  lowest priority will be chosen. For example If 'priority-1' < 'priority-2',\r\n"
    "  and both match the given key then 'priority-1' will be chosen.\r\n"
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(2)|BIT(8)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_L3B_ID,
    "l3b",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_empty_vals[0],
    (sizeof(soc_petra_pp_api_empty_vals) / sizeof(soc_petra_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Add ACE (Access Control Entry) to the given ACL. On packet lookup, between\r\n"
    "  all the ACEs that match the key constructed from the packet, the ACE with the\r\n"
    "  lowest priority will be chosen. For example If 'priority-1' < 'priority-2',\r\n"
    "  and both match the given key then 'priority-1' will be chosen.\r\n"
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(2)|BIT(8)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_FIELD_TO_INC_ID,
    "range_field_to_inc",
    (PARAM_VAL_RULES *)&SWP_P_PP_ACL_FIELD_RANGE_SELECT_TYPE_rule[0],
    (sizeof(SWP_P_PP_ACL_FIELD_RANGE_SELECT_TYPE_rule) / sizeof(SWP_P_PP_ACL_FIELD_RANGE_SELECT_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.field_to_inc:\r\n"
    "  Select one field in order to increment\r\n"
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(2)|BIT(7)|BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_COUNT_ID,
    "range_count",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.count:\r\n"
    "  Number of IP addresses to add.\r\n"
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(2)|BIT(7)|BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_STEP_ID,
    "range_step",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.step:\r\n"
    "  Number of step between to consecutive IP addresses. Relevant only for\r\n"
    "  increment and decrement.\r\n"
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(2)|BIT(7)|BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_INC_SYS_FEC_ID_ID,
    "range_inc_sys_fec_id",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_mac:\r\n"
    "  Whether to increment MAC by 1 for each ACE.\r\n"
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(2)|BIT(7)|BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_INC_VID_ID,
    "range_inc_vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_mac:\r\n"
    "  Whether to increment MAC by 1 for each ACE.\r\n"
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(2)|BIT(7)|BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_VALID_ID,
    "l4_port_range_valid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.port_range_valid:\r\n"
    "  If TRUE then port_range considered as part of the ACE. Otherwise it masked\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MAX_DEST_PORT_ID,
    "l4_max_dest_port",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.port_range.max_dest_port:\r\n"
    "  The maximum Destination port in the range. Size: 16 bits. Range 0-65535.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MIN_DEST_PORT_ID,
    "l4_min_dest_port",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.port_range.min_dest_port:\r\n"
    "  The minimum Destination port in the range. Size: 16 bits. Range 0-65535.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MAX_SRC_PORT_ID,
    "l4_max_src_port",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.port_range.max_src_port:\r\n"
    "  The maximum Source port in the range. Size: 16 bits. Range 0-65535.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MIN_SRC_PORT_ID,
    "l4_min_src_port",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.port_range.min_src_port:\r\n"
    "  The minimum Source port in the range. Size: 16 bits. Range 0-65535.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_DEST_PORT_MASK_ID,
    "l4_dest_port_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.dest_port.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFF to consider the exact\r\n"
    "  value\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_DEST_PORT_VAL_ID,
    "l4_dest_port_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.dest_port.Val:\r\n"
    "  Value.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_SRC_PORT_MASK_ID,
    "l4_src_port_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.src_port.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFF to consider the exact\r\n"
    "  value\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_SRC_PORT_VAL_ID,
    "l4_src_port_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.src_port.Val:\r\n"
    "  Value.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_TCP_CTRL_OPS_MASK_ID,
    "l4_tcp_ctrl_ops_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.tcp_ctrl_ops.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",

    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_TCP_CTRL_OPS_VAL_ID,
    "l4_tcp_ctrl_ops_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.tcp_ctrl_ops.Val:\r\n"
    "  Value.\r\n"
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_DEST_IP_PREFIX_LEN_ID,
    "l3_dest_ip_prefix",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_DEST_IP_IP_ADDRESS_ID,
    "l3_dest_ip_addr",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.dest_ip.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_SRC_IP_PREFIX_LEN_ID,
    "l3_src_ip_prefix",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_SRC_IP_IP_ADDRESS_ID,
    "l3_src_ip_add",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.src_ip.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_PROTOCOL_CODE_MASK_ID,
    "l3_prtcl_code_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.protocol_code.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_PROTOCOL_CODE_VAL_ID,
    "l3_prtcl_code_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.protocol_code.Val:\r\n"
    "  Value.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_TOS_MASK_ID,
    "l3_tos_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.tos.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_TOS_VAL_ID,
    "l3_tos_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.tos.Val:\r\n"
    "  Value.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_VID_MASK_ID,
    "vid_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.in_vid.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFFFFFF to consider the\r\n"
    "  exact value\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_VID_VAL_ID,
    "vid_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.in_vid.Val:\r\n"
    "  Value.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_LOCAL_PORT_ID_MASK_ID,
    "port_id_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.in_local_port_id.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFFFFFF to consider the\r\n"
    "  exact value\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_LOCAL_PORT_ID_VAL_ID,
    "local_port_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.in_local_port_id.Val:\r\n"
    "  Value.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_DEST_MAC_MASK_INDEX_ID,
    "dest_mac_mask",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  dest_mac_mask_index:\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_DEST_MAC_ID,
    "dest_mac",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.dest_mac:\r\n"
    "  Destination MAC\r\n"
    "  Address. \r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_SRC_MAC_MASK_ID,
    "src_mac_mask",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.src_mac_mask[ prm_src_mac_mask_index]:\r\n"
    "  Mask on the source MAC address. 1 to consider the corresponding bit, and zero\r\n"
    "  to ignore it.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_SRC_MAC_ID,
    "src_mac",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.src_mac:\r\n"
    "  Source MAC Address. \r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_ETHER_TYPE_MASK_ID,
    "ether_type_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.ether_type.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFF to consider the exact\r\n"
    "  value\r\n"
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_ETHER_TYPE_DSAP_ID,
    "ether_type_dsap_sssap",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_empty_vals[0],
    (sizeof(soc_petra_pp_api_empty_vals) / sizeof(soc_petra_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.ether_type.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFF to consider the exact\r\n"
    "  value\r\n"
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_ETHER_TYPE_VAL_ID,
    "ether_type_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.ether_type.Val:\r\n"
    "  Value.\r\n"
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_DEI_MASK_ID,
    "s_dei_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.s_tag.Dei.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_DEI_VAL_ID,
    "s_dei_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.s_tag.Dei.Val:\r\n"
    "  Value.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_PCP_MASK_ID,
    "s_pcp_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.s_tag.Pcp.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_PCP_VAL_ID,
    "s_pcp_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.s_tag.Pcp.Val:\r\n"
    "  Value.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_VID_MASK_ID,
    "s_vid_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.s_tag.Vid.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFFFFFF to consider the\r\n"
    "  exact value\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_VID_VAL_ID,
    "s_vid_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.s_tag.Vid.Val:\r\n"
    "  Value.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_TAG_EXIST_MASK_ID,
    "s_exist_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.s_tag.tag_exist.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_TAG_EXIST_VAL_ID,
    "s_tag_exist_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.s_tag.tag_exist.Val:\r\n"
    "  Value.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_PCP_MASK_ID,
    "c_tag_up_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.c_tag.Pcp.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_PCP_VAL_ID,
    "c_tag_dei_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.c_tag.Pcp.Val:\r\n"
    "  Value.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_VID_MASK_ID,
    "c_vid_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.c_tag.Vid.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFFFFFF to consider the\r\n"
    "  exact value\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_VID_VAL_ID,
    "c_vid_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.c_tag.Vid.Val:\r\n"
    "  Value.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_TAG_EXIST_MASK_ID,
    "c_tag_exit_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.c_tag.tag_exist.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_TAG_EXIST_VAL_ID,
    "c_tag_exit_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.c_tag.tag_exist.Val:\r\n"
    "  Value.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_IN_LOCAL_PORT_ID_MASK_ID,
    "local_port_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.in_local_port_id.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFFFFFF to consider the\r\n"
    "  exact value\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_IN_LOCAL_PORT_ID_VAL_ID,
    "local_port_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.in_local_port_id.Val:\r\n"
    "  Value.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_IN_PORT_TYPE_VALID_ID,
    "in_port_type_valid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.in_port_type_valid:\r\n"
    "  Is in_port_type valid, or it's masked.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_IN_PORT_TYPE_ID,
    "in_port_type",
    (PARAM_VAL_RULES *)&SWP_SAND_PP_PORT_L2_TYPE_rule[0],
    (sizeof(SWP_SAND_PP_PORT_L2_TYPE_rule) / sizeof(SWP_SAND_PP_PORT_L2_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.in_port_type:\r\n"
    "  The l2 port type (VBP,CEP,CNP,PNP). \r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_PRIORITY_ID,
    "priority",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.priority:\r\n"
    "  The priority of the ACE to add. The smaller the priority value, the higher\r\n"
    "  will be the priority of the ACE. \r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(2)|BIT(7)|BIT(8)},
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACE_ID_ID,
    "ace_id_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.id:\r\n"
    "  The ID of the ACE, should be unique for each ACE.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0,  0, 0, 0, 0, 0, BIT(2)|BIT(7)|BIT(8)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_COS_ACTION_ID,
    "action_cos",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action_info.cos_action:\r\n"
    "  Pointer to a CoS Action.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)|BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_SYS_FEC_ID_ID,
    "action_sys_fec_id",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action_info.sys_fec_id:\r\n"
    "  traffic match this rule to be forwarded (routed) according to this FEC \r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)|BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_INFO_DESTINATION_DEST_VAL_ID,
    "action_dest_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action_info.l3_info.destination.dest_val:\r\n"
    "  The destination value, according to the type (Single, RLAG or GLAG,LIF...)For\r\n"
    "  LAG the value is the group ID.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)|BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_INFO_DESTINATION_DEST_TYPE_ID,
    "action_dest_type",
    (PARAM_VAL_RULES *)&SOC_PETRA_PP_SAND_PP_DEST_TYPE_rule[0],
    (sizeof(SOC_PETRA_PP_SAND_PP_DEST_TYPE_rule) / sizeof(SOC_PETRA_PP_SAND_PP_DEST_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action_info.l3_info.destination.dest_type:\r\n"
    "  The type of the following destination ID\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)|BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_INFO_VID_ID,
    "action_vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action_info.l2_info.vid:\r\n"
    "  The VLAN id, to set to the aced packets. Relevant only if destination is\r\n"
    "  system port.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)|BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    PARAM_SWP_P_PP_ACL_ACE_ADD_ACE_ADD_OVERRIDE_ID,
    "override",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  override:\r\n"
    "  whether to override the entry if it already exists, if this is\r\n"
    "  FALSE and the entry exist error is returned.\r\n"
    "",
    "  mac 0 vid 0 dest_val 0 dest_type 0 update_type 0 override 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(2)|BIT(7)|BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_REMOVE_ACE_REMOVE_ID,
    "remove",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove ACE\r\n"
    "  from ACL\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl ace_remove acl_ndx 0 ace_id_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(14)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_REMOVE_ACE_REMOVE_ACL_NDX_ID,
    "acl_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acl_ndx:\r\n"
    "  ACL ID\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl ace_remove acl_ndx 0 ace_id_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_REMOVE_ACE_REMOVE_ACE_ID_NDX_ID,
    "ace_id_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace_id_ndx:\r\n"
    "  ACE ID\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl ace_remove acl_ndx 0 ace_id_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_GET_ACE_GET_ID,
    "get",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get Binding between ACE and a action. As a result of this operation packets\r\n"
    "  match the ACE will be process according to the given action.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl ace_get acl_ndx 0 ace_id_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(15)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_GET_ACE_GET_ACL_NDX_ID,
    "acl_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acl_ndx:\r\n"
    "  ACL id.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl ace_get acl_ndx 0 ace_id_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(15)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_GET_ACE_GET_ACE_ID_NDX_ID,
    "ace_id_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace_id_ndx:\r\n"
    "  ace ID.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl ace_get acl_ndx 0 ace_id_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(15)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_GET_BLOCK_ACE_GET_BLOCK_ID,
    "get_block",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get block of entries from the routing table in a given range.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl ace_get_block acl_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(16)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_ACE_GET_BLOCK_ACE_GET_BLOCK_ACL_NDX_ID,
    "acl_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acl_ndx:\r\n"
    "  ACL id.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl ace_get_block acl_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ID,
    "range_add",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Binds between ace and actoions.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACL_NDX_ID,
    "acl_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acl_ndx:\r\n"
    "  ACL ID\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_VALID_ID,
    "port_range_valid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.port_range_valid:\r\n"
    "  If TRUE then port_range considered as part of the ACE. Otherwise it masked\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MAX_DEST_PORT_ID,
    "max_dest_port",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.port_range.max_dest_port:\r\n"
    "  The maximum Destination port in the range. Size: 16 bits. Range 0-65535.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MIN_DEST_PORT_ID,
    "min_dest_port",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.port_range.min_dest_port:\r\n"
    "  The minimum Destination port in the range. Size: 16 bits. Range 0-65535.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MAX_SRC_PORT_ID,
    "max_src_port",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.port_range.max_src_port:\r\n"
    "  The maximum Source port in the range. Size: 16 bits. Range 0-65535.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MIN_SRC_PORT_ID,
    "min_src_port",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.port_range.min_src_port:\r\n"
    "  The minimum Source port in the range. Size: 16 bits. Range 0-65535.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_DEST_PORT_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.dest_port.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFF to consider the exact\r\n"
    "  value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_DEST_PORT_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.dest_port.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_SRC_PORT_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.src_port.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFF to consider the exact\r\n"
    "  value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_SRC_PORT_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.src_port.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_TCP_CTRL_OPS_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.tcp_ctrl_ops.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_TCP_CTRL_OPS_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.tcp_ctrl_ops.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_DEST_IP_PREFIX_LEN_ID,
    "prefix_len",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.dest_ip.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_DEST_IP_IP_ADDRESS_ID,
    "ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.dest_ip.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_SRC_IP_PREFIX_LEN_ID,
    "prefix_len",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.src_ip.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_SRC_IP_IP_ADDRESS_ID,
    "ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.src_ip.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_PROTOCOL_CODE_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.protocol_code.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_PROTOCOL_CODE_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.protocol_code.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_TOS_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.tos.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_TOS_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.tos.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_VID_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.in_vid.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFFFFFF to consider the\r\n"
    "  exact value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_VID_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.in_vid.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_LOCAL_PORT_ID_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.in_local_port_id.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFFFFFF to consider the\r\n"
    "  exact value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_LOCAL_PORT_ID_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.in_local_port_id.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_DEST_MAC_MASK_INDEX_ID,
    "dest_mac_mask_index",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  dest_mac_mask_index:\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_DEST_MAC_MASK_ID,
    "dest_mac_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.dest_mac_mask[ prm_dest_mac_mask_index]:\r\n"
    "  Mask on the Destination MAC address. 1 to consider the corresponding bit, and\r\n"
    "  zero to ignore it.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_DEST_MAC_ID,
    "dest_mac",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.dest_mac:\r\n"
    "  Destination MAC\r\n"
    "  Address. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_SRC_MAC_MASK_INDEX_ID,
    "src_mac_mask_index",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  src_mac_mask_index:\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_SRC_MAC_MASK_ID,
    "src_mac_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.src_mac_mask[ prm_src_mac_mask_index]:\r\n"
    "  Mask on the source MAC address. 1 to consider the corresponding bit, and zero\r\n"
    "  to ignore it.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_SRC_MAC_ID,
    "src_mac",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.src_mac:\r\n"
    "  Source MAC Address. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_ETHER_TYPE_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.ether_type.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFF to consider the exact\r\n"
    "  value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_ETHER_TYPE_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.ether_type.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_DEI_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.s_tag.Dei.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_DEI_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.s_tag.Dei.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_PCP_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.s_tag.Pcp.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_PCP_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.s_tag.Pcp.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_VID_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.s_tag.Vid.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFFFFFF to consider the\r\n"
    "  exact value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_VID_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.s_tag.Vid.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_TAG_EXIST_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.s_tag.tag_exist.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_TAG_EXIST_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.s_tag.tag_exist.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_DEI_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.c_tag.Dei.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_DEI_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.c_tag.Dei.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_PCP_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.c_tag.Pcp.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_PCP_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.c_tag.Pcp.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_VID_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.c_tag.Vid.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFFFFFF to consider the\r\n"
    "  exact value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_VID_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.c_tag.Vid.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_TAG_EXIST_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.c_tag.tag_exist.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_TAG_EXIST_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.c_tag.tag_exist.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_IN_LOCAL_PORT_ID_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.in_local_port_id.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFFFFFF to consider the\r\n"
    "  exact value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_IN_LOCAL_PORT_ID_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.in_local_port_id.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_IN_PORT_TYPE_VALID_ID,
    "in_port_type_valid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.in_port_type_valid:\r\n"
    "  Is in_port_type valid, or it's masked.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_IN_PORT_TYPE_ID,
    "in_port_type",
    (PARAM_VAL_RULES *)&SWP_SAND_PP_PORT_L2_TYPE_rule[0],
    (sizeof(SWP_SAND_PP_PORT_L2_TYPE_rule) / sizeof(SWP_SAND_PP_PORT_L2_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.in_port_type:\r\n"
    "  The l2 port type (VBP,CEP,CNP,PNP). \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_TYPE_ID,
    "type",
    (PARAM_VAL_RULES *)&SOC_PETRA_PP_ACL_ACE_TYPE_rule[0],
    (sizeof(SOC_PETRA_PP_ACL_ACE_TYPE_rule) / sizeof(SOC_PETRA_PP_ACL_ACE_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.type:\r\n"
    "  Type of the ACE L2 or L3 (and L4).\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_PRIORITY_ID,
    "priority",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.priority:\r\n"
    "  The priority of the ACE to add. The smaller the priority value, the higher\r\n"
    "  will be the priority of the ACE. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACE_ID_ID,
    "id",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.id:\r\n"
    "  The ID of the ACE, should be unique for each ACE.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_INC_VID_ID,
    "inc_vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_vid:\r\n"
    "  Whether to increment VID by 1 for each ACE.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_INC_SYS_FEC_ID_ID,
    "inc_sys_fec_id",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_mac:\r\n"
    "  Whether to increment MAC by 1 for each ACE.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_STEP_ID,
    "step",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.step:\r\n"
    "  Number of step between to consecutive IP addresses. Relevant only for\r\n"
    "  increment and decrement.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_COUNT_ID,
    "count",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.count:\r\n"
    "  Number of IP addresses to add.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_TO_ID,
    "to",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.to:\r\n"
    "  IP address, range ends at this IP address. Relevant only for random. For\r\n"
    "  increment/decrement types the end of the range is determined according to the\r\n"
    "  count and step.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_FIELD_TO_INC_ID,
    "field_to_inc",
    (PARAM_VAL_RULES *)&SWP_P_PP_ACL_FIELD_RANGE_SELECT_TYPE_rule[0],
    (sizeof(SWP_P_PP_ACL_FIELD_RANGE_SELECT_TYPE_rule) / sizeof(SWP_P_PP_ACL_FIELD_RANGE_SELECT_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.field_to_inc:\r\n"
    "  Select one field in order to increment\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_TYPE_ID,
    "type",
    (PARAM_VAL_RULES *)&SWP_P_PP_ACL_RANGE_TYPE_rule[0],
    (sizeof(SWP_P_PP_ACL_RANGE_TYPE_rule) / sizeof(SWP_P_PP_ACL_RANGE_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.type:\r\n"
    "  How to pick up addresses from the given range randomly incrementally etc.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_ACTION_MASK_ID,
    "action_mask",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action.action_mask:\r\n"
    "  Mask over the field to be considered in the action. FEC pointer always set in\r\n"
    "  the mask.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_COS_ACTION_ID,
    "cos_action",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action.cos_action:\r\n"
    "  Pointer to a CoS Action.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_L3_INFO_SYS_FEC_ID_ID,
    "sys_fec_id",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.count:\r\n"
    "  Number of IP addresses to add.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_L3_INFO_VID_ID,
    "vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action.l3_info.vid:\r\n"
    "  The VLAN id, to set to the routed packets. Relevant only if destination is\r\n"
    "  system port.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_L3_INFO_DESTINATION_DEST_VAL_ID,
    "dest_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action.l3_info.destination.dest_val:\r\n"
    "  The destination value, according to the type (Single, RLAG or GLAG,LIF...)For\r\n"
    "  LAG the value is the group ID.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_L3_INFO_DESTINATION_DEST_TYPE_ID,
    "dest_type",
    (PARAM_VAL_RULES *)&SOC_PETRA_PP_SAND_PP_DEST_TYPE_rule[0],
    (sizeof(SOC_PETRA_PP_SAND_PP_DEST_TYPE_rule) / sizeof(SOC_PETRA_PP_SAND_PP_DEST_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action.l3_info.destination.dest_type:\r\n"
    "  The type of the following destination ID\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_L2_INFO_MAC_ID,
    "mac",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action.l2_info.mac:\r\n"
    "  The MAC address of the next hop.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_L2_INFO_VID_ID,
    "vid",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action.l2_info.vid:\r\n"
    "  The VLAN id, to set to the aced packets. Relevant only if destination is\r\n"
    "  system port.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_L2_INFO_DESTINATION_DEST_VAL_ID,
    "dest_val",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action.l2_info.destination.dest_val:\r\n"
    "  The destination value, according to the type (Single, RLAG or GLAG,LIF...)For\r\n"
    "  LAG the value is the group ID.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_L2_INFO_DESTINATION_DEST_TYPE_ID,
    "dest_type",
    (PARAM_VAL_RULES *)&SOC_PETRA_PP_SAND_PP_DEST_TYPE_rule[0],
    (sizeof(SOC_PETRA_PP_SAND_PP_DEST_TYPE_rule) / sizeof(SOC_PETRA_PP_SAND_PP_DEST_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action.l2_info.destination.dest_type:\r\n"
    "  The type of the following destination ID\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_ADD_RANGE_ADD_ACTION_UPDATE_TYPE_ID,
    "update_type",
    (PARAM_VAL_RULES *)&SOC_PETRA_PP_ACL_ACTION_TYPE_rule[0],
    (sizeof(SOC_PETRA_PP_ACL_ACTION_TYPE_rule) / sizeof(SOC_PETRA_PP_ACL_ACTION_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action.update_type:\r\n"
    "  update l2 or l3 information.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
    "  min_dest_port 0 max_src_port 0 min_src_port 0 mask 0 Val 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0 mask 0 Val 0 mask\r\n"
    "  0 Val 0 mask 0 Val 0 mask 0 Val 0 dest_mac_mask_index 0 dest_mac_mask 0\r\n"
    "  dest_mac 0 src_mac_mask_index 0 src_mac_mask 0 src_mac 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0 Val 0 mask 0\r\n"
    "  Val 0 mask 0 Val 0 mask 0 Val 0 in_port_type_valid 0 in_port_type 0 type 0\r\n"
    "  priority 0 id 0 inc_vid 0 inc_mac 0 step 0 count 0 to 0 field_to_inc 0 type 0\r\n"
    "  action_mask 0 cos_action 0 mac 0 vid 0 dest_val 0 dest_type 0 mac 0 vid 0\r\n"
    "  dest_val 0 dest_type 0 update_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_REMOVE_RANGE_REMOVE_ID,
    "range_remove",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove Range of ACEs there Ids.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_remove acl_ndx 0 from_ace_ndx 0 to_ace_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(18)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_REMOVE_RANGE_REMOVE_ACL_NDX_ID,
    "acl_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acl_ndx:\r\n"
    "  ACL id.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_remove acl_ndx 0 from_ace_ndx 0 to_ace_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_REMOVE_RANGE_REMOVE_FROM_ACE_NDX_ID,
    "from_ace_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  from_ace_ndx:\r\n"
    "  from this ACE.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_remove acl_ndx 0 from_ace_ndx 0 to_ace_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_ACL_RANGE_REMOVE_RANGE_REMOVE_TO_ACE_NDX_ID,
    "to_ace_ndx",
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0],
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  to_ace_ndx:\r\n"
    "  remove till this ace.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  swp_p_pp_api acl range_remove acl_ndx 0 from_ace_ndx 0 to_ace_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWP_P_PP_API_ACL_ID,
    "acl",
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0],
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(8)|BIT(9)|BIT(10)|BIT(14)|BIT(15)|BIT(16)/*|BIT(17)*/|BIT(18)|BIT(19)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } acl*/

#ifdef UI_SWP_P_PP_MLAG_APP
#ifdef UI_SWP_P_PP_MLAG_APP/* { swp_p_pp_mlag_app*/
  { 
    PARAM_SWP_P_PP_MLAG_APP_INIT_MLAG_APP_INIT_ID, 
    "init", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Init device configuration for MLAG application.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_mlag_app mlag_app_init ether_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_MLAG_APP_INIT_MLAG_APP_INIT_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_MLAG_APP_INIT_MLAG_APP_INIT_INIT_INFO_ETHER_TYPE_ID, 
    "ether_type", 
    (PARAM_VAL_RULES *)&SOC_PETRA_PP_MACT_MESSAGE_HEADER_TYPE_rule[0], 
    (sizeof(SOC_PETRA_PP_MACT_MESSAGE_HEADER_TYPE_rule) / sizeof(SOC_PETRA_PP_MACT_MESSAGE_HEADER_TYPE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  init_info.ether_type:\n\r"
    "  learn message ethertype\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_mlag_app mlag_app_init ether_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_MLAG_APP_INIT_MLAG_APP_INIT_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_MLAG_APP_INIT_MLAG_APP_PORT_SET_ID, 
    "port_set", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  port_learn_enable setting.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_mlag_app mlag_app_port_learn_enable ether_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_MLAG_APP_PORT_LEARN_ENABLE_MLAG_APP_PORT_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_MLAG_APP_INIT_MLAG_APP_PORT_ID_SET_ID, 
    "port_id", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  port_learn_enable setting.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_mlag_app mlag_app_port_learn_enable ether_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_MLAG_APP_PORT_LEARN_ENABLE_MLAG_APP_PORT_SET_ID), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_MLAG_APP_INIT_MLAG_APP_PORT_GET_ID, 
    "port_get", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  port_learn_enable setting.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_mlag_app mlag_app_port_learn_enable ether_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_MLAG_APP_PORT_LEARN_ENABLE_MLAG_APP_PORT_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_MLAG_APP_INIT_MLAG_APP_PORT_ID_GET_ID, 
    "port_id", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  port_learn_enable setting.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_mlag_app mlag_app_port_learn_enable ether_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_MLAG_APP_PORT_LEARN_ENABLE_MLAG_APP_PORT_GET_ID), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_MLAG_APP_INIT_MLAG_APP_LEARN_ENABLE_SET_ID, 
    "learn_enable", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  port_learn_enable setting.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_mlag_app mlag_app_port_learn_enable ether_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_MLAG_APP_PORT_LEARN_ENABLE_MLAG_APP_PORT_SET_ID), 
    4, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },    
  { 
    PARAM_SWP_P_PP_MLAG_APP_ID, 
    "mlag_app", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    BIT_UI_16_RNG(FUN_SWP_P_PP_MLAG_APP_FIRST_ID,FUN_SWP_P_PP_MLAG_APP_LAST_ID), 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
#endif /* UI_SWP_P_PP_MLAG_APP*/
#endif/* } swp_p_pp_mlag_app*/
#ifdef UI_SWP_P_PP_TS_APP/* { swp_p_pp_ts_app*/
  { 
    PARAM_SWP_P_PP_TS_APP_INIT_TS_APP_INIT_ID, 
    "init", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Init device configuration for TS application.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_app_init tbd 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_APP_INIT_TS_APP_INIT_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_APP_INIT_TS_APP_INIT_INIT_INFO_TBD_ID, 
    "tbd", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  init_info.tbd:\n\r"
    "  TBD, for future use\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_app_init tbd 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_APP_INIT_TS_APP_INIT_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ID, 
    "add", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Add TS entry (key, action) to the HW DB, Packets match this key will be\n\r"
    "  processed according to the given action (forwarded to the given destination).\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_add dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
    "  dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */  
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_L2_TP_DIP_ID, 
    "l2tp_dip", 
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0], 
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.l2_tp.dip:\n\r"
    "  Destination IP address\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_add dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
    "  dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_L2_TP_SESSION_ID, 
    "l2tp_session", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.l2_tp.session:\n\r"
    "  L2TPv3 Port\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_add dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
    "  dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_L2_TP_IN_LOCAL_PORT_ID, 
    "l2tp_in_local_port", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.l2_tp.in_local_port:\n\r"
    "  Incoming local FAP port. Range: 0 - 15.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_add dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
    "  dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_L2_TP_TABLE_ID_ID, 
    "l2tp_table_id", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.l2_tp.table_id:\n\r"
    "  Table ID. Range: 0 - 15.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_add dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
    "  dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_MC_SIP_ID, 
    "udp_mc_sip", 
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0], 
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_mc.sip:\n\r"
    "  Source IP address\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_add dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
    "  dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_MC_DIP_ID, 
    "udp_mc_dip", 
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0], 
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_mc.dip:\n\r"
    "  Destination IP address\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_add dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
    "  dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_MC_UDP_DEST_PORT_ID, 
    "udp_mc_udp_dest_port", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_mc.udp_dest_port:\n\r"
    "  UDP destination port\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_add dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
    "  dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_MC_IN_LOCAL_PORT_ID, 
    "udp_mc_in_local_port", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_mc.in_local_port:\n\r"
    "  Incoming local FAP port. Range: 0 - 15.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_add dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
    "  dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_MC_TABLE_ID_ID, 
    "udp_mc_table_id", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_mc.table_id:\n\r"
    "  Table ID. Range: 0 - 15.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_add dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
    "  dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_UC_DIP_ID, 
    "udp_uc_dip", 
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0], 
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_uc.dip:\n\r"
    "  Destination IP address\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_add dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
    "  dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_UC_UDP_DEST_PORT_VALID_ID, 
    "udp_uc_udp_dest_port_valid", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_uc.udp_dest_port_valid:\n\r"
    "  Whether UDP destination port is valid. IF TRUE then udp_dest_port is\n\r"
    "  considered, if FALSE udp_dest_port is masked (Don't Care)\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_add dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
    "  dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_UC_UDP_DEST_PORT_ID, 
    "udp_uc_udp_dest_port", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_uc.udp_dest_port:\n\r"
    "  UDP destination port\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_add dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
    "  dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_UC_IN_LOCAL_PORT_ID, 
    "udp_uc_in_local_port", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_uc.in_local_port:\n\r"
    "  Incoming local FAP port. Range: 0 - 15.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_add dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
    "  dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_VALUE_UDP_UC_TABLE_ID_ID, 
    "udp_uc_table_id", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_uc.table_id:\n\r"
    "  Table ID. Range: 0 - 15.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_add dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
    "  dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_KEY_TYPE_ID, 
    "key_type", 
    (PARAM_VAL_RULES *)&SWP_P_PP_TS_KEY_TYPE_rule[0], 
    (sizeof(SWP_P_PP_TS_KEY_TYPE_rule) / sizeof(SWP_P_PP_TS_KEY_TYPE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.type:\n\r"
    "  Key type\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_add dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
    "  dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ID), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ACTION_DEST_DEST_VAL_ID, 
    "dest_val", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  action.dest.dest_val:\n\r"
    "  The destination value, according to the type (Single, LAG,LIF...)For the LAG\n\r"
    "  type, the value is the group ID. Range: 0 - 255.For the DROP type, range: 0 -\n\r"
    "  0x3FFF.For the ROUTER type, range: 0 - 0x2000.For the MULTICAST type, range:\n\r"
    "  0 - 16383.For the Single_Port type, range: 0 - 4095.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_add dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
    "  dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ACTION_DEST_DEST_TYPE_ID, 
    "dest_type", 
    (PARAM_VAL_RULES *)&SWP_SAND_PP_DEST_TYPE_rule[0], 
    (sizeof(SWP_SAND_PP_DEST_TYPE_rule) / sizeof(SWP_SAND_PP_DEST_TYPE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  action.dest.dest_type:\n\r"
    "  The type of the following destination ID\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_add dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
    "  dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_ADD_TS_ENTRY_ADD_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_ID, 
    "get", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Returns the action the given key is mapped to. \n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_get dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_L2_TP_DIP_ID, 
    "l2tp_dip", 
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0], 
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.l2_tp.dip:\n\r"
    "  Destination IP address\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_get dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_L2_TP_SESSION_ID, 
    "l2tp_session", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.l2_tp.session:\n\r"
    "  L2TPv3 Port\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_get dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_L2_TP_IN_LOCAL_PORT_ID, 
    "l2tp_in_local_port", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.l2_tp.in_local_port:\n\r"
    "  Incoming local FAP port. Range: 0 - 15.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_get dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_L2_TP_TABLE_ID_ID, 
    "l2tp_table_id", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.l2_tp.table_id:\n\r"
    "  Table ID. Range: 0 - 15.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_get dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_MC_SIP_ID, 
    "udp_mc_sip", 
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0], 
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_mc.sip:\n\r"
    "  Source IP address\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_get dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_MC_DIP_ID, 
    "udp_mc_dip", 
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0], 
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_mc.dip:\n\r"
    "  Destination IP address\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_get dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_MC_UDP_DEST_PORT_ID, 
    "udp_mc_udp_dest_port", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_mc.udp_dest_port:\n\r"
    "  UDP destination port\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_get dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_MC_IN_LOCAL_PORT_ID, 
    "udp_mc_in_local_port", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_mc.in_local_port:\n\r"
    "  Incoming local FAP port. Range: 0 - 15.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_get dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_MC_TABLE_ID_ID, 
    "udp_mc_table_id", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_mc.table_id:\n\r"
    "  Table ID. Range: 0 - 15.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_get dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_UC_DIP_ID, 
    "udp_uc_dip", 
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0], 
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_uc.dip:\n\r"
    "  Destination IP address\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_get dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_UC_UDP_DEST_PORT_VALID_ID, 
    "udp_uc_udp_dest_port_valid", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_uc.udp_dest_port_valid:\n\r"
    "  Whether UDP destination port is valid. IF TRUE then udp_dest_port is\n\r"
    "  considered, if FALSE udp_dest_port is masked (Don't Care)\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_get dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_UC_UDP_DEST_PORT_ID, 
    "udp_uc_udp_dest_port", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_uc.udp_dest_port:\n\r"
    "  UDP destination port\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_get dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_UC_IN_LOCAL_PORT_ID, 
    "udp_uc_in_local_port", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_uc.in_local_port:\n\r"
    "  Incoming local FAP port. Range: 0 - 15.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_get dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_VALUE_UDP_UC_TABLE_ID_ID, 
    "udp_uc_table_id", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_uc.table_id:\n\r"
    "  Table ID. Range: 0 - 15.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_get dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_KEY_TYPE_ID, 
    "key_type", 
    (PARAM_VAL_RULES *)&SWP_P_PP_TS_KEY_TYPE_rule[0], 
    (sizeof(SWP_P_PP_TS_KEY_TYPE_rule) / sizeof(SWP_P_PP_TS_KEY_TYPE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.type:\n\r"
    "  Key type\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_get dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_GET_TS_ENTRY_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_GET_BLOCK_TS_GET_BLOCK_ID, 
    "get_block", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Returns block of entries\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_get_block key_type_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_GET_BLOCK_TS_GET_BLOCK_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_GET_BLOCK_TS_GET_BLOCK_KEY_TYPE_NDX_ID, 
    "key_type_ndx", 
    (PARAM_VAL_RULES *)&SWP_P_PP_TS_KEY_TYPE_rule[0], 
    (sizeof(SWP_P_PP_TS_KEY_TYPE_rule) / sizeof(SWP_P_PP_TS_KEY_TYPE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key_type_ndx:\n\r"
    "  Type of the keys to return.	\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_get_block key_type_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_GET_BLOCK_TS_GET_BLOCK_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_ID, 
    "remove", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Remove a key mapping from the HW DB.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_remove dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_L2_TP_DIP_ID, 
    "l2tp_dip", 
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0], 
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.l2_tp.dip:\n\r"
    "  Destination IP address\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_remove dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_L2_TP_SESSION_ID, 
    "l2tp_session", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.l2_tp.session:\n\r"
    "  L2TPv3 Port\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_remove dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_L2_TP_IN_LOCAL_PORT_ID, 
    "l2tp_in_local_port", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.l2_tp.in_local_port:\n\r"
    "  Incoming local FAP port. Range: 0 - 15.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_remove dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_L2_TP_TABLE_ID_ID, 
    "l2tp_table_id", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.l2_tp.table_id:\n\r"
    "  Table ID. Range: 0 - 15.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_remove dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_MC_SIP_ID, 
    "udp_mc_sip", 
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0], 
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_mc.sip:\n\r"
    "  Source IP address\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_remove dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_MC_DIP_ID, 
    "udp_mc_dip", 
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0], 
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_mc.dip:\n\r"
    "  Destination IP address\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_remove dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_MC_UDP_DEST_PORT_ID, 
    "udp_mc_udp_dest_port", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_mc.udp_dest_port:\n\r"
    "  UDP destination port\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_remove dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_MC_IN_LOCAL_PORT_ID, 
    "udp_mc_in_local_port", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_mc.in_local_port:\n\r"
    "  Incoming local FAP port. Range: 0 - 15.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_remove dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_MC_TABLE_ID_ID, 
    "udp_mc_table_id", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_mc.table_id:\n\r"
    "  Table ID. Range: 0 - 15.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_remove dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_UC_DIP_ID, 
    "udp_uc_dip", 
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0], 
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_uc.dip:\n\r"
    "  Destination IP address\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_remove dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_UC_UDP_DEST_PORT_VALID_ID, 
    "udp_uc_udp_dest_port_valid", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_uc.udp_dest_port_valid:\n\r"
    "  Whether UDP destination port is valid. IF TRUE then udp_dest_port is\n\r"
    "  considered, if FALSE udp_dest_port is masked (Don't Care)\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_remove dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_UC_UDP_DEST_PORT_ID, 
    "udp_uc_udp_dest_port", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_uc.udp_dest_port:\n\r"
    "  UDP destination port\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_remove dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_UC_IN_LOCAL_PORT_ID, 
    "udp_uc_in_local_port", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_uc.in_local_port:\n\r"
    "  Incoming local FAP port. Range: 0 - 15.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_remove dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_VALUE_UDP_UC_TABLE_ID_ID, 
    "udp_uc_table_id", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value.udp_uc.table_id:\n\r"
    "  Table ID. Range: 0 - 15.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_remove dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_KEY_TYPE_ID, 
    "key_type", 
    (PARAM_VAL_RULES *)&SWP_P_PP_TS_KEY_TYPE_rule[0], 
    (sizeof(SWP_P_PP_TS_KEY_TYPE_rule) / sizeof(SWP_P_PP_TS_KEY_TYPE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.type:\n\r"
    "  Key type\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_entry_remove dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_ENTRY_REMOVE_TS_ENTRY_REMOVE_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_CLEAR_ALL_TS_CLEAR_ALL_ID, 
    "clear_all", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Clear all keys from all DBs \n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_clear_all\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_CLEAR_ALL_TS_CLEAR_ALL_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_ID, 
    "trap_set", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Set Trap for packets matches the given key\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_trap_set mac 0 value 0 qual 0 trap 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_TRAP_KEY_MAC_ID, 
    "mac", 
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0], 
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  trap_key.mac:\n\r"
    "  Relevant when trap qualifier is PACKET_DA\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_trap_set mac 0 value 0 qual 0 trap 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_TRAP_KEY_VALUE_ID, 
    "ip_val", 
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0], 
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  trap_key.value:\n\r"
    "  Relevant when trap qualifier is PACKET_DIP\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_trap_set mac 0 value 0 qual 0 trap 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_TRAP_KEY_QUAL_ID, 
    "qual", 
    (PARAM_VAL_RULES *)&SWP_P_PP_TS_TRAP_QUAL_rule[0], 
    (sizeof(SWP_P_PP_TS_TRAP_QUAL_rule) / sizeof(SWP_P_PP_TS_TRAP_QUAL_rule[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  trap_key.qual:\n\r"
    "  Trap qualifier\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_trap_set mac 0 value 0 qual 0 trap 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_ID), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_TRAP_ID, 
    "trap", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  trap:\n\r"
    "  key will be trapped to CPU.FALSE to invalidate the Trap.	\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_trap_set mac 0 value 0 qual 0 trap 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_ID), 
    4, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_ACTION_DEST_DEST_VAL_ID, 
    "dest_val", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  action.dest.dest_val:\n\r"
    "  The destination value, according to the type (Single, LAG,LIF...)For the LAG\n\r"
    "  type, the value is the group ID. Range: 0 - 255.For the DROP type, range: 0 -\n\r"
    "  0x3FFF.For the ROUTER type, range: 0 - 0x2000.For the MULTICAST type, range:\n\r"
    "  0 - 16383.For the Single_Port type, range: 0 - 4095.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_trap_set dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
    "  dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_ACTION_DEST_DEST_TYPE_ID, 
    "dest_type", 
    (PARAM_VAL_RULES *)&SWP_SAND_PP_DEST_TYPE_rule[0], 
    (sizeof(SWP_SAND_PP_DEST_TYPE_rule) / sizeof(SWP_SAND_PP_DEST_TYPE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  action.dest.dest_type:\n\r"
    "  The type of the following destination ID\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_trap_set dip 0 session 0 in_local_port 0\n\r"
    "  table_id 0 sip 0 dip 0 udp_dest_port 0 in_local_port 0 table_id 0 dip 0\n\r"
    "  udp_dest_port_valid 0 udp_dest_port 0 in_local_port 0 table_id 0 type 0\n\r"
    "  dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_TRAP_SET_TS_TRAP_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_TRAP_GET_TS_TRAP_GET_ID, 
    "trap_get", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Set Trap for packets matches the given key\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_trap_get mac 0 value 0 qual 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_TRAP_GET_TS_TRAP_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_TRAP_GET_TS_TRAP_GET_TRAP_KEY_MAC_ID, 
    "mac", 
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0], 
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  trap_key.mac:\n\r"
    "  Relevant when trap qualifier is PACKET_DA\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_trap_get mac 0 value 0 qual 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_TRAP_GET_TS_TRAP_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_TRAP_GET_TS_TRAP_GET_TRAP_KEY_VALUE_ID, 
    "value", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  trap_key.value:\n\r"
    "  Relevant when trap qualifier is PACKET_DIP\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_trap_get mac 0 value 0 qual 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_TRAP_GET_TS_TRAP_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_TRAP_GET_TS_TRAP_GET_TRAP_KEY_QUAL_ID, 
    "qual", 
    (PARAM_VAL_RULES *)&SWP_P_PP_TS_TRAP_QUAL_rule[0], 
    (sizeof(SWP_P_PP_TS_TRAP_QUAL_rule) / sizeof(SWP_P_PP_TS_TRAP_QUAL_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  trap_key.qual:\n\r"
    "  Trap qualifier\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_trap_get mac 0 value 0 qual 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_TRAP_GET_TS_TRAP_GET_ID), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  }, 
  { 
    PARAM_SWP_P_PP_TS_TRAP_GET_BLOCK_TS_TRAP_GET_BLOCK_ID, 
    "traps_get_block", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Returns block of entries\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_ts_app ts_trap_get_block key_type_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_TS_TRAP_GET_BLOCK_TS_TRAP_GET_BLOCK_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TS_APP_ID, 
    "ts_app", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    BIT_UI_16_RNG(FUN_SWP_P_PP_TS_APP_FIRST_ID,FUN_SWP_P_PP_TS_APP_LAST_ID), 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
#endif /* } swp_p_pp_ts_app*/


#ifdef UI_VLAN /*  { swp_p_pp_tst6*/
     { 
    PARAM_SWP_P_PP_BR_APP_INIT_BR_APP_INIT_ID, 
    "app_init", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Init device configuration for TS application.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_app_init init_info 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_APP_INIT_BR_APP_INIT_ID), 
    2, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_APP_INIT_BR_APP_INIT_INIT_INFO_ID, 
    "init_info", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  init_info:\n\r"
    "  Information needed for Initialization \n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_app_init init_info 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_APP_INIT_BR_APP_INIT_ID), 
    LAST_ORDINAL, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_APP_CLOSE_BR_APP_CLOSE_ID, 
    "app_close", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  reset all configuration and release allocated memory\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_app_close init_info 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_APP_CLOSE_BR_APP_CLOSE_ID), 
    2, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_APP_CLOSE_BR_APP_CLOSE_INIT_INFO_ID, 
    "init_info", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  init_info:\n\r"
    "  should be called to close module\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_app_close init_info 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_APP_CLOSE_BR_APP_CLOSE_ID), 
    LAST_ORDINAL, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_ENTRY_ADD_BR_ENTRY_ADD_ID, 
    "entry_add", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Add entry (key, action) to the HW DB, Packets match this key will be\n\r"
    "  processed according to the given action (forwarded to the given destination).\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_entry_add prio 0 value 0 type 0x1 dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_ENTRY_ADD_BR_ENTRY_ADD_ID), 
    2, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_ENTRY_ADD_BR_ENTRY_ADD_KEY_PRIO_ID, 
    "prio", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.prio:\n\r"
    "  Priority of entry. Range: 0 - 1.- 0 entry will be added to TCAM- 1 entry will\n\r"
    "  be added to Exact Match lower value is higher priority.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_entry_add prio 0 value 0 type 0x1 dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_ENTRY_ADD_BR_ENTRY_ADD_ID), 
    LAST_ORDINAL, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_ENTRY_ADD_BR_ENTRY_ADD_KEY_VALUE_ID, 
    "value", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value:\n\r"
    "  Key value, for both types key value is value[0], bits 0 up to bit\n\r"
    "  21.represnts XPP-Header-Extension[21:0]\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_entry_add prio 0 value 0 type 0x1 dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_ENTRY_ADD_BR_ENTRY_ADD_ID), 
    LAST_ORDINAL, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_ENTRY_ADD_BR_ENTRY_ADD_KEY_TYPE_ID, 
    "type", 
    (PARAM_VAL_RULES *)&SWP_P_PP_BR_KEY_TYPE_rule[0], 
    (sizeof(SWP_P_PP_BR_KEY_TYPE_rule) / sizeof(SWP_P_PP_BR_KEY_TYPE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.type:\n\r"
    "  Type of Key\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_entry_add prio 0 value 0 type 0x1 dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_ENTRY_ADD_BR_ENTRY_ADD_ID), 
    LAST_ORDINAL, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_ENTRY_ADD_BR_ENTRY_ADD_ACTION_DEST_DEST_VAL_ID, 
    "dest_val", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  action.dest.dest_val:\n\r"
    "  The destination value, according to the type (Single, LAG,LIF?)For the LAG\n\r"
    "  type, the value is the group ID. Range: 0 - 255.For the DROP type, range: 0 -\n\r"
    "  0x3FFF.For the ROUTER type, range: 0 - 0x2000.For the MULTICAST type, range:\n\r"
    "  0 - 16383.For the Single_Port type, range: 0 - 4095.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_entry_add prio 0 value 0 type 0x1 dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_ENTRY_ADD_BR_ENTRY_ADD_ID), 
    LAST_ORDINAL, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_ENTRY_ADD_BR_ENTRY_ADD_ACTION_DEST_DEST_TYPE_ID, 
    "dest_type", 
    (PARAM_VAL_RULES *)&SWP_SAND_PP_DEST_TYPE_rule[0], 
    (sizeof(SWP_SAND_PP_DEST_TYPE_rule) / sizeof(SWP_SAND_PP_DEST_TYPE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  action.dest.dest_type:\n\r"
    "  The type of the following destination ID\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_entry_add prio 0 value 0 type 0x1 dest_val 0 dest_type 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_ENTRY_ADD_BR_ENTRY_ADD_ID), 
    LAST_ORDINAL, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_ENTRY_GET_BR_ENTRY_GET_ID, 
    "entry_get", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Returns the action the given key is mapped to. \n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_entry_get prio 0 value 0 type 0x1\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_ENTRY_GET_BR_ENTRY_GET_ID), 
    2, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_ENTRY_GET_BR_ENTRY_GET_KEY_PRIO_ID, 
    "prio", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.prio:\n\r"
    "  Priority of entry. Range: 0 - 1.- 0 entry will be added to TCAM- 1 entry will\n\r"
    "  be added to Exact Match lower value is higher priority.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_entry_get prio 0 value 0 type 0x1\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_ENTRY_GET_BR_ENTRY_GET_ID), 
    LAST_ORDINAL, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_ENTRY_GET_BR_ENTRY_GET_KEY_VALUE_ID, 
    "value", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value:\n\r"
    "  Key value, for both types key value is value[0], bits 0 up to bit\n\r"
    "  21.represnts XPP-Header-Extension[21:0]\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_entry_get prio 0 value 0 type 0x1\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_ENTRY_GET_BR_ENTRY_GET_ID), 
    LAST_ORDINAL, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_ENTRY_GET_BR_ENTRY_GET_KEY_TYPE_ID, 
    "type", 
    (PARAM_VAL_RULES *)&SWP_P_PP_BR_KEY_TYPE_rule[0], 
    (sizeof(SWP_P_PP_BR_KEY_TYPE_rule) / sizeof(SWP_P_PP_BR_KEY_TYPE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.type:\n\r"
    "  Type of Key\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_entry_get prio 0 value 0 type 0x1\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_ENTRY_GET_BR_ENTRY_GET_ID), 
    LAST_ORDINAL, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_GET_BLOCK_BR_GET_BLOCK_ID, 
    "get_block", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Returns block of entries\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_get_block key_type_ndx 0x1\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_GET_BLOCK_BR_GET_BLOCK_ID), 
    2, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_GET_BLOCK_BR_GET_BLOCK_KEY_TYPE_NDX_ID, 
    "key_type_ndx", 
    (PARAM_VAL_RULES *)&SWP_P_PP_BR_KEY_TYPE_rule[0], 
    (sizeof(SWP_P_PP_BR_KEY_TYPE_rule) / sizeof(SWP_P_PP_BR_KEY_TYPE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key_type_ndx:\n\r"
    "  Type of the keys to return.	\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_get_block key_type_ndx 0x1\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_GET_BLOCK_BR_GET_BLOCK_ID), 
    LAST_ORDINAL, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_ID, 
    "print_block", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Utility function to print keys in arrange table, call after calling\n\r"
    "  get-block. \n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_print_block key_type_ndx 0x1 prio 0 value 0 type 0x1\n\r"
    "  dest_val 0 dest_type 0 places 0 nof_entries 0 flavors 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_ID), 
    2, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_KEY_TYPE_NDX_ID, 
    "key_type_ndx", 
    (PARAM_VAL_RULES *)&SWP_P_PP_BR_KEY_TYPE_rule[0], 
    (sizeof(SWP_P_PP_BR_KEY_TYPE_rule) / sizeof(SWP_P_PP_BR_KEY_TYPE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key_type_ndx:\n\r"
    "  Type of the keys to return.	\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_print_block key_type_ndx 0x1 prio 0 value 0 type 0x1\n\r"
    "  dest_val 0 dest_type 0 places 0 nof_entries 0 flavors 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_ID), 
    LAST_ORDINAL, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_KEYS_LIST_PRIO_ID, 
    "prio", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  keys_list.prio:\n\r"
    "  Priority of entry. Range: 0 - 1.- 0 entry will be added to TCAM- 1 entry will\n\r"
    "  be added to Exact Match lower value is higher priority.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_print_block key_type_ndx 0x1 prio 0 value 0 type 0x1\n\r"
    "  dest_val 0 dest_type 0 places 0 nof_entries 0 flavors 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_ID), 
    LAST_ORDINAL, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_KEYS_LIST_VALUE_ID, 
    "key", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  keys_list.value:\n\r"
    "  Key value, for both types key value is value[0], bits 0 up to bit\n\r"
    "  21.represnts XPP-Header-Extension[21:0]\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_print_block key_type_ndx 0x1 prio 0 value 0 type 0x1\n\r"
    "  dest_val 0 dest_type 0 places 0 nof_entries 0 flavors 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_ID), 
    LAST_ORDINAL, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_KEYS_LIST_TYPE_ID, 
    "type", 
    (PARAM_VAL_RULES *)&SWP_P_PP_BR_KEY_TYPE_rule[0], 
    (sizeof(SWP_P_PP_BR_KEY_TYPE_rule) / sizeof(SWP_P_PP_BR_KEY_TYPE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  keys_list.type:\n\r"
    "  Type of Key\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_print_block key_type_ndx 0x1 prio 0 value 0 type 0x1\n\r"
    "  dest_val 0 dest_type 0 places 0 nof_entries 0 flavors 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_ID), 
    LAST_ORDINAL, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_ACTIONS_LIST_DEST_DEST_VAL_ID, 
    "dest_val", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  actions_list.dest.dest_val:\n\r"
    "  The destination value, according to the type (Single, LAG,LIF?)For the LAG\n\r"
    "  type, the value is the group ID. Range: 0 - 255.For the DROP type, range: 0 -\n\r"
    "  0x3FFF.For the ROUTER type, range: 0 - 0x2000.For the MULTICAST type, range:\n\r"
    "  0 - 16383.For the Single_Port type, range: 0 - 4095.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_print_block key_type_ndx 0x1 prio 0 value 0 type 0x1\n\r"
    "  dest_val 0 dest_type 0 places 0 nof_entries 0 flavors 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_ID), 
    LAST_ORDINAL, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_ACTIONS_LIST_DEST_DEST_TYPE_ID, 
    "dest_type", 
    (PARAM_VAL_RULES *)&SWP_SAND_PP_DEST_TYPE_rule[0], 
    (sizeof(SWP_SAND_PP_DEST_TYPE_rule) / sizeof(SWP_SAND_PP_DEST_TYPE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  actions_list.dest.dest_type:\n\r"
    "  The type of the following destination ID\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_print_block key_type_ndx 0x1 prio 0 value 0 type 0x1\n\r"
    "  dest_val 0 dest_type 0 places 0 nof_entries 0 flavors 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_ID), 
    LAST_ORDINAL, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_PLACES_ID, 
    "places", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  places:\n\r"
    "  The place of each entry in the TCAM, relevant only for entries reside in\n\r"
    "  TCAM.	\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_print_block key_type_ndx 0x1 prio 0 value 0 type 0x1\n\r"
    "  dest_val 0 dest_type 0 places 0 nof_entries 0 flavors 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_ID), 
    LAST_ORDINAL, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_NOF_ENTRIES_ID, 
    "nof_entries", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  nof_entries:\n\r"
    "  Number of valid entries in keys_list and actions_list.	\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_print_block key_type_ndx 0x1 prio 0 value 0 type 0x1\n\r"
    "  dest_val 0 dest_type 0 places 0 nof_entries 0 flavors 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_ID), 
    LAST_ORDINAL, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_FLAVORS_ID, 
    "flavors", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  flavors:\n\r"
    "  How to print the table, set to zero\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_print_block key_type_ndx 0x1 prio 0 value 0 type 0x1\n\r"
    "  dest_val 0 dest_type 0 places 0 nof_entries 0 flavors 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_PRINT_BLOCK_BR_PRINT_BLOCK_ID), 
    LAST_ORDINAL, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_ENTRY_REMOVE_BR_ENTRY_REMOVE_ID, 
    "entry_remove", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Remove a key mapping from the HW DB.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_entry_remove prio 0 value 0 type 0x1\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_ENTRY_REMOVE_BR_ENTRY_REMOVE_ID), 
    2, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_ENTRY_REMOVE_BR_ENTRY_REMOVE_KEY_PRIO_ID, 
    "prio", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.prio:\n\r"
    "  Priority of entry. Range: 0 - 1.- 0 entry will be added to TCAM- 1 entry will\n\r"
    "  be added to Exact Match lower value is higher priority.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_entry_remove prio 0 value 0 type 0x1\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_ENTRY_REMOVE_BR_ENTRY_REMOVE_ID), 
    LAST_ORDINAL, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_ENTRY_REMOVE_BR_ENTRY_REMOVE_KEY_VALUE_ID, 
    "value", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.value:\n\r"
    "  Key value, for both types key value is value[0], bits 0 up to bit\n\r"
    "  21.represnts XPP-Header-Extension[21:0]\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_entry_remove prio 0 value 0 type 0x1\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_ENTRY_REMOVE_BR_ENTRY_REMOVE_ID), 
    LAST_ORDINAL, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_ENTRY_REMOVE_BR_ENTRY_REMOVE_KEY_TYPE_ID, 
    "type", 
    (PARAM_VAL_RULES *)&SWP_P_PP_BR_KEY_TYPE_rule[0], 
    (sizeof(SWP_P_PP_BR_KEY_TYPE_rule) / sizeof(SWP_P_PP_BR_KEY_TYPE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  key.type:\n\r"
    "  Type of Key\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_entry_remove prio 0 value 0 type 0x1\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_ENTRY_REMOVE_BR_ENTRY_REMOVE_ID), 
    LAST_ORDINAL, 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_CLEAR_ALL_BR_CLEAR_ALL_ID, 
    "clear_all", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Clear all keys from all DBs \n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_clear_all\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_CLEAR_ALL_BR_CLEAR_ALL_ID), 
    2, 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_PORT_INFO_SET_BR_PORT_INFO_SET_ID, 
    "port_info_set", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  set port attributes, including which lookup/program to perform\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_port_info_set in_local_port 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_PORT_INFO_SET_BR_PORT_INFO_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_PORT_INFO_SET_BR_PORT_INFO_SET_IN_LOCAL_PORT_ID, 
    "in_local_port", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  in_local_port:\n\r"
    "  Trap key	\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_port_info_set in_local_port 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_PORT_INFO_SET_BR_PORT_INFO_SET_ID), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_PORT_INFO_SET_BR_PORT_INFO_SET_PORT_INFO_DFLT_ACTION_DEST_DEST_VAL_ID, 
    "dest_val", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  port_info.dflt_action.dest.dest_val:\n\r"
    "  The destination value, according to the type (Single, LAG,LIF?)For the LAG\n\r"
    "  type, the value is the group ID. Range: 0 - 255.For the DROP type, range: 0 -\n\r"
    "  0x3FFF.For the ROUTER type, range: 0 - 0x2000.For the MULTICAST type, range:\n\r"
    "  0 - 16383.For the Single_Port type, range: 0 - 4095.\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_port_info_set in_local_port 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_PORT_INFO_SET_BR_PORT_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_PORT_INFO_SET_BR_PORT_INFO_SET_PORT_INFO_DFLT_ACTION_DEST_DEST_TYPE_ID, 
    "dest_type", 
    (PARAM_VAL_RULES *)&SWP_SAND_PP_DEST_TYPE_rule[0], 
    (sizeof(SWP_SAND_PP_DEST_TYPE_rule) / sizeof(SWP_SAND_PP_DEST_TYPE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  port_info.dflt_action.dest.dest_type:\n\r"
    "  The type of the following destination ID\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_port_info_set in_local_port 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_PORT_INFO_SET_BR_PORT_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_PORT_INFO_SET_BR_PORT_INFO_SET_PORT_INFO_TYPE_ID, 
    "type", 
    (PARAM_VAL_RULES *)&SWP_P_PP_BR_PORT_TYPE_rule[0], 
    (sizeof(SWP_P_PP_BR_PORT_TYPE_rule) / sizeof(SWP_P_PP_BR_PORT_TYPE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  port_info.type:\n\r"
    "  port Type, IPTV or HQS, to set port typeto TM/CPU/... user\n\r"
    "  soc_petra_port_header_type_set \n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_port_info_set in_local_port 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_PORT_INFO_SET_BR_PORT_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_PORT_INFO_GET_BR_PORT_INFO_GET_ID, 
    "port_info_get", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  set port attributes, including which lookup/program to perform\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_port_info_get\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_PORT_INFO_GET_BR_PORT_INFO_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },
    { 
    PARAM_SWP_P_PP_BR_PORT_INFO_GET_BR_PORT_INFO_GET_IN_LOCAL_PORT_ID, 
    "in_local_port", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_free_vals[0], 
    (sizeof(swp_p_pp_api_free_vals) / sizeof(swp_p_pp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  in_local_port:\n\r"
    "  Trap key	\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_port_info_get in_local_port 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_PORT_INFO_GET_BR_PORT_INFO_GET_ID), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_BR_APP_RUN_BR_APP_RUN_ID, 
    "app_run", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  this is mini application that demonstrate how to use ts_application\n\r"
    "",
#if UI_SWP_P_PP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  swp_p_pp_api swp_p_pp_tst6 br_app_run\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_SWP_P_PP_BR_APP_RUN_BR_APP_RUN_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_SWP_P_PP_TST6_ID, 
    "tst6", 
    (PARAM_VAL_RULES *)&swp_p_pp_api_empty_vals[0], 
    (sizeof(swp_p_pp_api_empty_vals) / sizeof(swp_p_pp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    BIT_UI_16_RNG(FUN_SWP_P_PP_TST6_FIRST_ID,FUN_SWP_P_PP_TST6_LAST_ID), 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
#endif /* } swp_p_pp_tst6*/


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

/* } __UI_ROM_DEFI_SWP_P_PP_API_INCLUDED__*/

#endif
