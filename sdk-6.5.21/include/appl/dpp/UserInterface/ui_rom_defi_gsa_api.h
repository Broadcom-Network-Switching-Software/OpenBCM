/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UI_ROM_DEFI_GSA_API_INCLUDED__
/* { */
#define __UI_ROM_DEFI_GSA_API_INCLUDED__

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <appl/dpp/UserInterface/ui_pure_defi_gsa_api.h>
#include <DuneApp/gsa_api_vlan.h>
#include <DuneApp/gsa_api_mact.h>
#include <DuneApp/gsa_api_fec.h>
#include <DuneApp/gsa_api_ipv4_uc.h>
#include <DuneApp/gsa_sys_info.h>

#ifdef  PETRA_PP_MINIMAL
  #define UI_VLAN
  #define UI_MACT
#else
  #define UI_FEC
  #define UI_IPV4_UC
  #define UI_IPV4_MC
  #define UI_IPV6
  #define UI_ACL
  #define UI_VLAN
  #define UI_MACT
#endif
 /*
  * ENUM RULES
  * {
  */
/********************************************************/
#ifndef CONST
#define CONST const
#endif
/********************************************************/

EXTERN CONST
   PARAM_VAL_RULES
     GSA_MACT_FRWRD_ACTION_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "drop",
    {
      {
        GSA_MACT_FRWRD_ACTION_TYPE_DROP,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  GSA_MACT_FRWRD_ACTION_TYPE.GSA_MACT_FRWRD_ACTION_TYPE_DROP:\r\n"
        "  Drop packet.\r\n"
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
    "bridge",
    {
      {
        GSA_MACT_FRWRD_ACTION_TYPE_BRIDGE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  GSA_MACT_FRWRD_ACTION_TYPE.GSA_MACT_FRWRD_ACTION_TYPE_BRIDGE:\r\n"
        "  forward packet withing the bridge\r\n"
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
        GSA_MACT_FRWRD_ACTION_TYPE_ROUTE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  GSA_MACT_FRWRD_ACTION_TYPE.GSA_MACT_FRWRD_ACTION_TYPE_ROUTE:\r\n"
        "  forward packet to Router Engine.\r\n"
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



 /*
  * } ENUM RULES
  */
/********************************************************/
/********************************************************/


EXTERN CONST
   PARAM_VAL_RULES
     gsa_api_members_vals[]
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
     gsa_api_free_vals[]
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
     gsa_api_empty_vals[]
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
/********************************************************/
EXTERN CONST
   PARAM
     gsa_api_params[]
#ifdef INIT
   =
{
#ifdef UI_SAND_PP_GENERAL/* { soc_sand_pp_general*/
#endif /* } soc_sand_pp_general*/
#ifdef UI_VLAN/* { vlan*/
#ifndef PETRA_PP_MINIMAL /* { */
  {
    PARAM_GSA_VLAN_SET_SET_ID,
    "set",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the VLAN information.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api vlan set vlan_ndx 0 enable_flooding 0 topology 0\r\n"
    "  tagged_ports 0 1 2 83 untagged_ports 7 6 85 valid 0\r\n"
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
    PARAM_GSA_VLAN_SET_SET_ID,
      "range_set",
      (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
      (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "  Set the VLAN information.\r\n"
      "",
#if UI_GSA_API_ADD_EXAMPLE
      "Examples:\r\n"
      "  gsa_api vlan set vlan_ndx 0 enable_flooding 0 topology 0\r\n"
      "  tagged_ports 0 1 2 83 untagged_ports 7 6 85 valid 0\r\n"
#endif
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
    PARAM_GSA_VLAN_SET_SET_UP_TO_VLAN_ID,
    "up_to_vlan",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vlan_ndx:\r\n"
    "  the VLAN ID to configure.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api vlan set vlan_ndx 0 enable_flooding 0 topology 0\r\n"
    "  tagged_ports 0 1 2 83 untagged_ports 7 6 85 valid 0\r\n"
#endif
    "",
    "",
    {0, BIT(4), 0, 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_VLAN_SET_SET_VLAN_NDX_ID,
    "vlan_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vlan_ndx:\r\n"
    "  the VLAN ID to configure.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api vlan set vlan_ndx 0 enable_flooding 0 topology 0\r\n"
    "  tagged_ports 0 1 2 83 untagged_ports 7 6 85 valid 0\r\n"
#endif
    "",
    "",
    {BIT(0), BIT(4), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_VLAN_SET_SET_VLAN_INFO_ENABLE_FLOODING_ID,
    "enable_flooding",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vlan_info.enable_flooding:\r\n"
    "  VLAN forwarding for unknown DA packets (when performing lookup on the MAC\r\n"
    "  table). if flooding is enable then send to all member ports, otherwise drop\r\n"
    "  the packet.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api vlan set vlan_ndx 0 enable_flooding 0 topology 0\r\n"
    "  tagged_ports 0 1 2 83 untagged_ports 7 6 85 valid 0\r\n"
#endif
    "",
    "",
    {BIT(0), BIT(4), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_VLAN_SET_SET_VLAN_INFO_TOPOLOGY_ID,
    "topology",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vlan_info.topology:\r\n"
    "  Topology ID the VLAN is mapped to, for MSTP.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api vlan set vlan_ndx 0 enable_flooding 0 topology 0\r\n"
    "  tagged_ports 0 1 2 83 untagged_ports 7 6 85 valid 0\r\n"
#endif
    "",
    "",
    {BIT(0), BIT(4), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_VLAN_SET_SET_VLAN_INFO_MEMBERSHIP_TAGGED_PORTS_ID,
    "tagged_ports",
    (PARAM_VAL_RULES *)&gsa_api_members_vals[0],
    (sizeof(gsa_api_members_vals) / sizeof(gsa_api_members_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vlan_info.membership.nof_members:\r\n"
    "  Tagged members in the VLAN.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api vlan set vlan_ndx 0 enable_flooding 0 topology 0\r\n"
    "  tagged_ports 0 1 2 83 untagged_ports 7 6 85 valid 0\r\n"
#endif
    "",
    "",
    {BIT(0), BIT(4), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_VLAN_SET_SET_VLAN_INFO_MEMBERSHIP_UNTAGGED_PORTS_ID,
    "untagged_ports",
    (PARAM_VAL_RULES *)&gsa_api_members_vals[0],
    (sizeof(gsa_api_members_vals) / sizeof(gsa_api_members_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vlan_info.membership.nof_members:\r\n"
    "  un-Tagged members in the VLAN.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api vlan set vlan_ndx 0 enable_flooding 0 topology 0\r\n"
    "  tagged_ports 0 1 2 83 untagged_ports 7 6 85 valid 0\r\n"
#endif
    "",
    "",
    {BIT(0), BIT(4), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_VLAN_SET_SET_VLAN_INFO_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vlan_info.valid:\r\n"
    "  If TRUE then VLAN is valid. \r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api vlan set vlan_ndx 0 enable_flooding 0 topology 0\r\n"
    "  tagged_ports 0 1 2 83 untagged_ports 7 6 85 valid 0\r\n"
#endif
    "",
    "",
    {BIT(0), BIT(4), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_VLAN_GET_GET_ID,
    "get",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the VLAN information.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api vlan get vlan_ndx 0\r\n"
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
    PARAM_GSA_VLAN_GET_GET_VLAN_NDX_ID,
    "vlan_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vlan_ndx:\r\n"
    "  the VLAN ID to configure.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api vlan get vlan_ndx 0\r\n"
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
    PARAM_GSA_VLAN_MEMBER_ADD_MEMBER_ADD_ID,
    "add_member",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Add system port as a member in the VLAN.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api vlan add_member vlan_ndx 0 sys_port 0 tagged 0\r\n"
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
    PARAM_GSA_VLAN_MEMBER_ADD_MEMBER_ADD_VLAN_NDX_ID,
    "vlan_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vlan_ndx:\r\n"
    "  the VLAN ID. Range 0-4095.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api vlan add_member vlan_ndx 0 sys_port 0 tagged 0\r\n"
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
    PARAM_GSA_VLAN_MEMBER_ADD_MEMBER_ADD_SYS_PORT_ID,
    "sys_port",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_port:\r\n"
    "  system port to add as member to the vlan.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api vlan add_member vlan_ndx 0 sys_port 0 tagged 0\r\n"
#endif
    "",
    "",
    {BIT(2), 0, 0, 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_VLAN_MEMBER_ADD_MEMBER_ADD_TAGGED_ID,
    "tagged",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tagged:\r\n"
    "  whether this port transmit tagged/untagged packets in this VLAN.\r\n"
    "  Default: 0 (untagged)",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api vlan add_member vlan_ndx 0 sys_port 0 tagged 0\r\n"
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
    PARAM_GSA_VLAN_MEMBER_REMOVE_MEMBER_REMOVE_ID,
    "remove_member",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove system port from the VLAN membership.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api vlan remove_member vlan_ndx 0 sys_port 0\r\n"
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
    PARAM_GSA_VLAN_MEMBER_REMOVE_MEMBER_REMOVE_VLAN_NDX_ID,
    "vlan_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vlan_ndx:\r\n"
    "  the VLAN ID. Range 0-4095.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api vlan remove_member vlan_ndx 0 sys_port 0\r\n"
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
    PARAM_GSA_VLAN_MEMBER_REMOVE_MEMBER_REMOVE_SYS_PORT_ID,
    "sys_port",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_port:\r\n"
    "  system port.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api vlan remove_member vlan_ndx 0 sys_port 0\r\n"
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
    PARAM_GSA_VLAN_MEMBER_ADD_MEMBER_ADD_ID,
    "add_member",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Add system port as a member in the VLAN.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api vlan add_member vlan_ndx 0 sys_port 0 tagged 0\r\n"
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
    PARAM_GSA_EDIT_PORT_INFO_SET_PORT_INFO_SET_ID,
    "port_tag_set",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Sets the Editing information for VBP port and a VLAN.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_gsa vlan port_tag_set port_ndx 0 vid_ndx 0 tagged 0\r\n"
#endif
    "",
    "",
    {0, BIT(6), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_EDIT_PORT_INFO_SET_PORT_INFO_SET_PORT_NDX_ID,
    "port_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_ndx:\r\n"
    "  Local port ID. Range 0-79.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_gsa vlan port_tag_set port_ndx 0 vid_ndx 0 tagged 0\r\n"
#endif
    "",
    "",
    {0, BIT(6), 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_EDIT_PORT_INFO_SET_PORT_INFO_SET_VID_NDX_ID,
    "vid_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vid_ndx:\r\n"
    "  the VLAN ID. Range 0-4095.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_gsa vlan port_tag_set port_ndx 0 vid_ndx 0 tagged 0\r\n"
#endif
    "",
    "",
    {0, BIT(6), 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_EDIT_PORT_INFO_SET_PORT_INFO_SET_TAGGED_ID,
    "tagged",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tagged:\r\n"
    "  If the port send the packets tagged.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_gsa vlan port_tag_set port_ndx 0 vid_ndx 0 tagged 0\r\n"
#endif
    "",
    "",
    {0, BIT(6), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_EDIT_PORT_INFO_GET_PORT_INFO_GET_ID,
    "port_tag_get",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Sets the Editing information for VBP port and a VLAN.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_gsa vlan port_tag_get port_ndx 0 vid_ndx 0\r\n"
#endif
    "",
    "",
    {0, BIT(7), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_EDIT_PORT_INFO_GET_PORT_INFO_GET_PORT_NDX_ID,
    "port_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_ndx:\r\n"
    "  Local port ID. Range 0-79.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_gsa vlan port_tag_get port_ndx 0 vid_ndx 0\r\n"
#endif
    "",
    "",
    {0, BIT(7), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_EDIT_PORT_INFO_GET_PORT_INFO_GET_VID_NDX_ID,
    "vid_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vid_ndx:\r\n"
    "  the VLAN ID. Range 0-4095.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_gsa vlan port_tag_get port_ndx 0 vid_ndx 0\r\n"
#endif
    "",
    "",
    {0, BIT(7), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* PETRA_PP_MINIMAL  */
  {
    PARAM_GSA_EDIT_PORT_INFO_SET_PORT_PVID_SET_ID,
    "port_pvid_set",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Sets the Editing information for VBP port and a VLAN.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_gsa vlan port_pvid_set port_ndx 0 pvid 0\r\n"
#endif
    "",
    "",
    {0, BIT(8), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_EDIT_PORT_INFO_SET_PORT_PVID_SET_PORT_NDX_ID,
    "port_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_ndx:\r\n"
    "  Local port ID. Range 0-99.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_gsa vlan port_pvid_set port_ndx 0 pvid 0\r\n"
#endif
    "",
    "",
    {0, BIT(8), 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_EDIT_PORT_INFO_SET_PORT_PVID_SET_PVID_ID,
    "pvid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  pvid:\r\n"
    "  If the port send the packets pvid.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_gsa vlan port_pvid_set port_ndx 0 pvid 0\r\n"
#endif
    "",
    "",
    {0, BIT(8), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_EDIT_PORT_INFO_GET_PORT_PVID_GET_ID,
    "port_pvid_get",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Sets the Editing information for VBP port and a VLAN.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_gsa vlan port_pvid_get port_ndx 0 vid_ndx 0\r\n"
#endif
    "",
    "",
    {0, BIT(9), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_EDIT_PORT_INFO_GET_PORT_PVID_GET_PORT_NDX_ID,
    "port_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_ndx:\r\n"
    "  Local port ID. Range 0-99.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_gsa vlan port_pvid_get port_ndx 0 vid_ndx 0\r\n"
#endif
    "",
    "",
    {0, BIT(9), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },


  {
    PARAM_GSA_API_VLAN_ID,
    "vlan",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(0)|BIT(1)|BIT(2)|BIT(3), BIT(4)|BIT(6)|BIT(7)|BIT(8)|BIT(9), 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } vlan */

#ifdef UI_MACT/* { mact*/
  {
    PARAM_GSA_MACT_ENTRY_ADD_ENTRY_ADD_ID,
    "add",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Add an entry with a given key and value. \r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact add vid 0 mac 0 sa_drop 0 is_dynamic 0 nof_ports 0\r\n"
    "  dests_ports_index 0 sys_port_id 0 type 0 frwrd_action 0\r\n"
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
    PARAM_GSA_MACT_ENTRY_ADD_ENTRY_ADD_KEY_VID_ID,
    "vid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  key.vid:\r\n"
    "  The VID.Range 0-4095\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact add vid 0 mac 0 sa_drop 0 is_dynamic 0 nof_ports 0\r\n"
    "  dests_ports_index 0 sys_port_id 0 type 0 frwrd_action 0\r\n"
#endif
    "",
    "",
    {BIT(4)|BIT(5), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_MACT_ENTRY_ADD_ENTRY_ADD_KEY_MAC_ID,
    "mac",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  key.mac:\r\n"
    "  MAC address\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact add vid 0 mac 0 sa_drop 0 is_dynamic 0 nof_ports 0\r\n"
    "  dests_ports_index 0 sys_port_id 0 type 0 frwrd_action 0\r\n"
#endif
    "",
    "",
    {BIT(4)|BIT(5), 0, 0, 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#ifndef PETRA_PP_MINIMAL
  {
    PARAM_GSA_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_SA_DROP_ID,
    "sa_drop",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  value.sa_drop:\r\n"
    "  If TRUE - For an SA lookup match, then the forwarding destination drop. If\r\n"
    "  FALSE - SA lookup match doesn't affect forwarding.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact add vid 0 mac 0 sa_drop 0 is_dynamic 0 nof_ports 0\r\n"
    "  dests_ports_index 0 sys_port_id 0 type 0 frwrd_action 0\r\n"
#endif
    "",
    "",
    {BIT(4)|BIT(5), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_IS_DYNAMIC_ID,
    "is_dynamic",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  value.is_dynamic:\r\n"
    "  Is the entry dynamic (i.e. isn't static) and participates in the aging\r\n"
    "  process.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact add vid 0 mac 0 sa_drop 0 is_dynamic 0 nof_ports 0\r\n"
    "  dests_ports_index 0 sys_port_id 0 type 0 frwrd_action 0\r\n"
#endif
    "",
    "",
    {BIT(4)|BIT(5), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /*PETRA_PP_MINIMAL  } */
  {
    PARAM_GSA_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_DESTS_DESTS_PHY_PORTS_ID_ID,
    "dest_phy_ports",
    (PARAM_VAL_RULES *)&gsa_api_members_vals[0],
    (sizeof(gsa_api_members_vals) / sizeof(gsa_api_members_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  value.dests.dests_ports :\r\n"
    "  list of destination sys_port_id. Range: 0 -\r\n"
    "  System Physical Port. Range: 0 - 4K-1.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact add vid 0 mac 0 sa_drop 0 is_dynamic 0 nof_ports 0\r\n"
    "  dests_ports_index 0 sys_port_id 0 type 0 frwrd_action 0\r\n"
#endif
    "",
    "",
    {BIT(4)|BIT(5), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_DESTS_DESTS_LAG_PORTS_ID_ID,
    "dest_lags",
    (PARAM_VAL_RULES *)&gsa_api_members_vals[0],
    (sizeof(gsa_api_members_vals) / sizeof(gsa_api_members_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  value.dests.dests_ports :\r\n"
    "  list of destination LAGs.  -\r\n"
    "  System LAG . Range: 0 - 255.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact add vid 0 mac 0 sa_drop 0 is_dynamic 0 nof_ports 0\r\n"
    "  dests_ports_index 0 sys_port_id 0 type 0 frwrd_action 0\r\n"
#endif
    "",
    "",
    {BIT(4)|BIT(5), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#ifndef PETRA_PP_MINIMAL
  {
    PARAM_GSA_MACT_ENTRY_ADD_ENTRY_ADD_VALUE_FRWRD_ACTION_ID,
    "frwrd_action",
    (PARAM_VAL_RULES *)&GSA_MACT_FRWRD_ACTION_TYPE_rule[0],
    (sizeof(GSA_MACT_FRWRD_ACTION_TYPE_rule) / sizeof(GSA_MACT_FRWRD_ACTION_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  value.frwrd_action:\r\n"
    "  The forwarding action (bridge, drop, route)\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact add vid 0 mac 0 sa_drop 0 is_dynamic 0 nof_ports 0\r\n"
    "  dests_ports_index 0 sys_port_id 0 type 0 frwrd_action 0\r\n"
#endif
    "",
    "",
    {BIT(4)|BIT(5), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /*PETRA_PP_MINIMAL  } */
  {
    PARAM_GSA_MACT_RANGE_ADD_RANGE_ADD_ID,
    "range_add",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Add a range of MAC entry starting from the MAC in the key parameter up to\r\n"
    "  'up_to_mac' and bind them to the information in value.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact range_add vid 0 mac 0 up_to_mac 0 sa_drop 0 is_dynamic 0\r\n"
    "  nof_ports 0 dests_ports_index 0 sys_port_id 0 type 0 frwrd_action 0\r\n"
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
    PARAM_GSA_MACT_RANGE_ADD_RANGE_ADD_UP_TO_MAC_ID,
    "up_to_mac",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  up_to_mac:\r\n"
    "  the last MAC in the range.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact range_add vid 0 mac 0 up_to_mac 0 sa_drop 0 is_dynamic 0\r\n"
    "  nof_ports 0 dests_ports_index 0 sys_port_id 0 type 0 frwrd_action 0\r\n"
#endif
    "",
    "",
    {BIT(5), 0, 0, 0},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_GSA_MACT_ENTRY_REMOVE_ENTRY_REMOVE_ID,
    "remove",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove an entry for given key and value. \r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact remove vid 0 mac 0\r\n"
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
    PARAM_GSA_MACT_ENTRY_REMOVE_ENTRY_REMOVE_KEY_VID_ID,
    "vid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  key.vid:\r\n"
    "  The VID.Range 0-4095\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact remove vid 0 mac 0\r\n"
#endif
    "",
    "",
    {BIT(6), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_MACT_ENTRY_REMOVE_ENTRY_REMOVE_KEY_MAC_ID,
    "mac",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  key.mac:\r\n"
    "  MAC address\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact remove vid 0 mac 0\r\n"
#endif
    "",
    "",
    {BIT(6), 0, 0, 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_MACT_RANGE_REMOVE_RANGE_REMOVE_ID,
    "range_remove",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove a range of MAC entry starting from the MAC in the key parameter up to\r\n"
    "  'up_to_mac'.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact range_remove vid 0 mac 0 up_to_mac 0\r\n"
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
    PARAM_GSA_MACT_RANGE_REMOVE_RANGE_REMOVE_KEY_VID_ID,
    "vid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  key.vid:\r\n"
    "  The VID.Range 0-4095\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact range_remove vid 0 mac 0 up_to_mac 0\r\n"
#endif
    "",
    "",
    {BIT(7), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_MACT_RANGE_REMOVE_RANGE_REMOVE_KEY_MAC_ID,
    "mac",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  key.mac:\r\n"
    "  MAC address\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact range_remove vid 0 mac 0 up_to_mac 0\r\n"
#endif
    "",
    "",
    {BIT(7), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_MACT_RANGE_REMOVE_RANGE_REMOVE_UP_TO_MAC_ID,
    "up_to_mac",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  up_to_mac:\r\n"
    "  the last MAC in the range.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact range_remove vid 0 mac 0 up_to_mac 0\r\n"
#endif
    "",
    "",
    {BIT(7), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_MACT_FLUSH_BLOCK_FLUSH_BLOCK_ID,
    "flush",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Flush (clear/transplant) MAC table entries that match a given rule\r\n"
    "  (FID,destination,static). \r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact flush dest_val 0 dest_type 0 including_dynamic 0\r\n"
    "  including_static 0 dest_val 0 dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
    "  mode 0\r\n"
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
#ifndef PETRA_PP_MINIMAL
  {
    PARAM_GSA_MACT_FLUSH_BLOCK_TRANSPLANT_BLOCK_ID,
    "transplant",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Flush (clear/transplant) MAC table entries that match a given rule\r\n"
    "  (FID,destination,static). \r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact transplant dest_val 0 dest_type 0 including_dynamic 0\r\n"
    "  including_static 0 dest_val 0 dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
    "  mode 0\r\n"
#endif
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
    PARAM_GSA_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_NEW_DEST_DEST_VAL_ID,
    "new_dest_val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.new_dest.dest_val:\r\n"
    "  The destination value, according to the type (Single, LAG,LIF...)For LAG the\r\n"
    "  value is the group ID.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact flush dest_val 0 dest_type 0 including_dynamic 0\r\n"
    "  including_static 0 dest_val 0 dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
    "  mode 0\r\n"
#endif
    "",
    "",
    {0, BIT(3), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_NEW_DEST_DEST_TYPE_ID,
    "new_dest_type",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.new_dest.dest_type:\r\n"
    "  The type of the following destination ID\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact flush dest_val 0 dest_type 0 including_dynamic 0\r\n"
    "  including_static 0 dest_val 0 dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
    "  mode 0\r\n"
#endif
    "",
    "",
    {0, BIT(3), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_INCLUDING_DYNAMIC_ID,
    "including_dynamic",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.rule.including_dynamic:\r\n"
    "  If TRUE then dynamic entries are in the match. In flush operation has to be\r\n"
    "  TRUE.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact flush dest_val 0 dest_type 0 including_dynamic 0\r\n"
    "  including_static 0 dest_val 0 dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
    "  mode 0\r\n"
#endif
    "",
    "",
    {BIT(8), BIT(3), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_INCLUDING_STATIC_ID,
    "including_static",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.rule.including_static:\r\n"
    "  If TRUE then static entries are in the match. \r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact flush dest_val 0 dest_type 0 including_dynamic 0\r\n"
    "  including_static 0 dest_val 0 dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
    "  mode 0\r\n"
#endif
    "",
    "",
    {BIT(8), BIT(3), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_DEST_DEST_VAL_ID,
    "dest_val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.rule.dest.dest_val:\r\n"
    "  The destination value, according to the type (Single, LAG,LIF...)For LAG the\r\n"
    "  value is the group ID.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact flush dest_val 0 dest_type 0 including_dynamic 0\r\n"
    "  including_static 0 dest_val 0 dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
    "  mode 0\r\n"
#endif
    "",
    "",
    {BIT(8), BIT(3), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_DEST_DEST_TYPE_ID,
    "dest_type",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.rule.dest.dest_type:\r\n"
    "  The type of the following destination ID\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact flush dest_val 0 dest_type 0 including_dynamic 0\r\n"
    "  including_static 0 dest_val 0 dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
    "  mode 0\r\n"
#endif
    "",
    "",
    {BIT(8), BIT(3), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /*PETRA_PP_MINIMAL*/
  {
    PARAM_GSA_MACT_FLUSH_BLOCK_FLUSH_BLOCK_FLUSH_INFO_RULE_FID_ID,
    "fid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.rule.fid:\r\n"
    "  When 'compare_fid' is asserted, the FID in the MACT is compared with this\r\n"
    "  value. \r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact flush dest_val 0 dest_type 0 including_dynamic 0\r\n"
    "  including_static 0 dest_val 0 dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
    "  mode 0\r\n"
#endif
    "",
    "",
    {BIT(8), BIT(3), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_MACT_GET_BLOCK_GET_BLOCK_ID,
    "get_block",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Flush (clear/transplant) MAC table entries that match a given rule\r\n"
    "  (FID,destination,static). \r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact get_block including_dynamic 0 including_static 0 dest_val 0\r\n"
    "  dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
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
#ifndef PETRA_PP_MINIMAL
  {
    PARAM_GSA_MACT_GET_BLOCK_GET_BLOCK_RULE_INCLUDING_DYNAMIC_ID,
    "including_dynamic",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  rule.including_dynamic:\r\n"
    "  If TRUE then dynamic entries are in the match. In flush operation has to be\r\n"
    "  TRUE.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact get_block including_dynamic 0 including_static 0 dest_val 0\r\n"
    "  dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
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
    PARAM_GSA_MACT_GET_BLOCK_GET_BLOCK_RULE_INCLUDING_STATIC_ID,
    "including_static",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  rule.including_static:\r\n"
    "  If TRUE then static entries are in the match. \r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact get_block including_dynamic 0 including_static 0 dest_val 0\r\n"
    "  dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
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
#endif /*PETRA_PP_MINIMAL*/
  {
    PARAM_GSA_MACT_GET_BLOCK_GET_FAP_ID_ID,
    "fap_id",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  fap_id:\r\n"
    "  Upper card is  fap_id = 0. \r\n"
    "  Bottom card is fap_id = 1. \r\n"
    "  Default is zero. \r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact get_block including_dynamic 0 including_static 0 dest_val 0\r\n"
    "  dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
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
#ifndef PETRA_PP_MINIMAL
    {
    PARAM_GSA_MACT_GET_BLOCK_GET_PRINT_MINIMAL_ID,
    "minimal_print",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flavor,minimal_print:\r\n"
    "  print shorted table. \r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact get_block including_dynamic 0 including_static 0 dest_val 0\r\n"
    "  dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
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
#endif
  {
    PARAM_GSA_MACT_GET_BLOCK_GET_BLOCK_RULE_DEST_DEST_VAL_ID,
    "dest_val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  rule.dest.dest_val:\r\n"
    "  The destination value, according to the type (Single, LAG,LIF...)For LAG the\r\n"
    "  value is the group ID.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact get_block including_dynamic 0 including_static 0 dest_val 0\r\n"
    "  dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
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
    PARAM_GSA_MACT_GET_BLOCK_GET_BLOCK_RULE_DEST_DEST_TYPE_ID,
    "dest_type",
    (PARAM_VAL_RULES *)&SOC_PETRA_PP_SAND_PP_DEST_TYPE_rule[0],
    (sizeof(SOC_PETRA_PP_SAND_PP_DEST_TYPE_rule) / sizeof(SOC_PETRA_PP_SAND_PP_DEST_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  rule.dest.dest_type:\r\n"
    "  The type of the following destination ID\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact get_block including_dynamic 0 including_static 0 dest_val 0\r\n"
    "  dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
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
    PARAM_GSA_MACT_GET_BLOCK_GET_BLOCK_RULE_FID_ID,
    "fid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  rule.fid:\r\n"
    "  When 'compare_fid' is asserted, the FID in the MACT is compared with this\r\n"
    "  value. \r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact get_block including_dynamic 0 including_static 0 dest_val 0\r\n"
    "  dest_type 0 compare_dest 0 fid 0 compare_fid 0\r\n"
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
    PARAM_GSA_MACT_PRINT_ALL_PRINT_ALL_ID,
    "print_all",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  print the MAC table entries. \r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact print_all\r\n"
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
#ifndef PETRA_PP_MINIMAL
  {
    PARAM_GSA_MACT_AGING_INFO_SET_AGING_INFO_SET_ID,
    "aging_info_set",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the aging information of the MACT. \r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact aging_info_set aging_cycle 0 enable_aging 0\r\n"
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
    PARAM_GSA_MACT_AGING_INFO_SET_AGING_INFO_SET_AGING_INFO_META_CYCLE_SEC_ID,
    "aging_cycle",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  aging_info.meta_cycle.sec:\r\n"
    "  in seconds Range 0-140000\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact aging_info_set aging_cycle 0 enable_aging 0\r\n"
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
    PARAM_GSA_MACT_AGING_INFO_SET_AGING_INFO_SET_AGING_INFO_ENABLE_AGING_ID,
    "enable_aging",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  aging_info.enable_aging:\r\n"
    "  If TRUE then enable the aging process of the device (aging machine).\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact aging_info_set aging_cycle 0 enable_aging 0\r\n"
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
    PARAM_GSA_MACT_AGING_INFO_GET_AGING_INFO_GET_ID,
    "aging_info_get",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the aging information of the MACT. \r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact aging_info_get\r\n"
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
    PARAM_GSA_MACT_MY_MAC_ADD_MY_MAC_ADD_ID,
    "my_mac_add",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Add entry (VID,MAC) so that L2 packets with the given VID and DA-MAC, is\r\n"
    "  terminated and forwarded to the Routing engine.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact my_mac_add vid 0 mac 0\r\n"
#endif
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
    PARAM_GSA_MACT_MY_MAC_ADD_MY_MAC_ADD_KEY_VID_ID,
    "vid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  key.vid:\r\n"
    "  The VID.Range 0-4095\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact my_mac_add vid 0 mac 0\r\n"
#endif
    "",
    "",
    {BIT(13), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_MACT_MY_MAC_ADD_MY_MAC_ADD_KEY_MAC_ID,
    "mac",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  key.mac:\r\n"
    "  MAC address\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact my_mac_add vid 0 mac 0\r\n"
#endif
    "",
    "",
    {BIT(13), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_MACT_MY_MAC_REMOVE_MY_MAC_REMOVE_ID,
    "my_mac_remove",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove My MAC entry (VID,MAC)\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact my_mac_remove vid 0 mac 0\r\n"
#endif
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
    PARAM_GSA_MACT_MY_MAC_REMOVE_MY_MAC_REMOVE_KEY_VID_ID,
    "vid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  key.vid:\r\n"
    "  The VID.Range 0-4095\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact my_mac_remove vid 0 mac 0\r\n"
#endif
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
    PARAM_GSA_MACT_MY_MAC_REMOVE_MY_MAC_REMOVE_KEY_MAC_ID,
    "mac",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  key.mac:\r\n"
    "  MAC address\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mact my_mac_remove vid 0 mac 0\r\n"
#endif
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
#endif /* PETRA_PP_MINIMAL */
  {
    PARAM_GSA_API_MACT_ID,
    "mact",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(8)|BIT(9)|/*|BIT(10)*/BIT(11)|BIT(12)/*|BIT(13)|BIT(14)*/, BIT(3), 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } mact*/
#ifndef PETRA_PP_MINIMAL
#ifdef UI_FEC/* { fec*/
  {
    PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_ID,
    "uc_ecmp_add",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Add array of Unicast FEC entries (ECMP) equal-cost-multi-path entry, so\r\n"
    "  Unicast packets lookup result with this sys_fec_d are routed along one of\r\n"
    "  these FEC entries while ensuring load balancing.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 \r\n"
    "  dest_sys_port_id 0 1 dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_SYS_FEC_NDX_ID,
    "sys_fec_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC id. Range 0-(4G-1)\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 \r\n"
    "  dest_sys_port_id 0 1 dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
#endif
    "",
    "",
    {BIT(15), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_ADD_TYPE_ID,
    "add_type",
    (PARAM_VAL_RULES *)&SWP_P_PP_FEC_ADD_TYPE_rule[0],
    (sizeof(SWP_P_PP_FEC_ADD_TYPE_rule) / sizeof(SWP_P_PP_FEC_ADD_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  add_type:\r\n"
    "  create new/override/concatenate.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 \r\n"
    "  dest_sys_port_id 0 1 dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
#endif
    "",
    "",
    {BIT(15), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_STATISTICS_COUNTER_ID,
    "statistics_counter",
    (PARAM_VAL_RULES *)&gsa_api_members_vals[0],
    (sizeof(gsa_api_members_vals) / sizeof(gsa_api_members_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].statistics_counter:\r\n"
    "  Select a statistics counter that is incremented when this entry is accessed.\r\n"
    "  Range 0-7.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 \r\n"
    "  dest_sys_port_id 0 1 dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
#endif
    "",
    "",
    {BIT(15), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_ENABLE_RPF_ID,
    "enable_rpf",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].enable_rpf:\r\n"
    "  If TRUE, when searching with the IPv4 SrcIP during multicast routing, then\r\n"
    "  use the destination field of the entry for RPF check of multicast packets.\r\n"
    "  This applies only if RPF is not specified in the Multicast FEC Entry. In this\r\n"
    "  case, the Destination must match the incoming port for the RPF check to pass.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 \r\n"
    "  dest_sys_port_id 0 1 dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
#endif
    "",
    "",
    {BIT(15), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_CPU_CODE_ID,
    "cpu_code",
    (PARAM_VAL_RULES *)&gsa_api_members_vals[0],
    (sizeof(gsa_api_members_vals) / sizeof(gsa_api_members_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].cpu_code:\r\n"
    "  Value to be sent to the CPU(in the EPPH). Range 0-1023.Valid only if command\r\n"
    "  is Trap.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 \r\n"
    "  dest_sys_port_id 0 1 dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
#endif
    "",
    "",
    {BIT(15), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_OUT_VID_ID,
    "out_vid",
    (PARAM_VAL_RULES *)&gsa_api_members_vals[0],
    (sizeof(gsa_api_members_vals) / sizeof(gsa_api_members_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].out_vid:\r\n"
    "  Outgoing VLAN ID.Rang 0-4095. \r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 \r\n"
    "  dest_sys_port_id 0 1 dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
#endif
    "",
    "",
    {BIT(15), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_MAC_ADDR_ID,
    "mac_addr",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].mac_addr:\r\n"
    "  MAC address to be used as DA for outgoing packet. (next hop MAC address)\r\n"
    "  Valid only if command is not Trap.Range 0-4095.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 \r\n"
    "  dest_sys_port_id 0 1 dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
#endif
    "",
    "",
    {BIT(15), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_DEST_SYS_PORT_ID_ID,
    "dest_sys_port_id",
    (PARAM_VAL_RULES *)&gsa_api_members_vals[0],
    (sizeof(gsa_api_members_vals) / sizeof(gsa_api_members_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].dest.dest_val:\r\n"
    "  The destination value, according to the type (Single, LAG,LIF...)For LAG the\r\n"
    "  value is the group ID.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 \r\n"
    "  dest_sys_port_id 0 1 dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
#endif
    "",
    "",
    {BIT(15), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_DEST_SYS_PORT_TYPE_ID,
    "dest_sys_port_type",
    (PARAM_VAL_RULES *)&SOC_PETRA_DEST_SYS_PORT_TYPE_rule[0],
    (sizeof(SOC_PETRA_DEST_SYS_PORT_TYPE_rule) / sizeof(SOC_PETRA_DEST_SYS_PORT_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].dest.dest_type:\r\n"
    "  The type of the following destination ID\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 \r\n"
    "  dest_sys_port_id 0 1 dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
#endif
    "",
    "",
    {BIT(15), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_ARRAY_ACTION_ID,
    "action",
    (PARAM_VAL_RULES *)&SOC_PETRA_PP_SAND_PP_FEC_COMMAND_TYPE_rule[0],
    (sizeof(SOC_PETRA_PP_SAND_PP_FEC_COMMAND_TYPE_rule) / sizeof(SOC_PETRA_PP_SAND_PP_FEC_COMMAND_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].action:\r\n"
    "  Drop, Route or Trap\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 \r\n"
    "  dest_sys_port_id 0 1 dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
#endif
    "",
    "",
    {BIT(15), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_RANGE_INC_MAC_ID,
    "range_inc_mac",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_cpu_code:\r\n"
    "  By how much to increment the CPU code value for each system FEC \r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 \r\n"
    "  dest_sys_port_id 0 1 dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
#endif
    "",
    "",
    {BIT(15), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_RANGE_INC_CPU_CODE_ID,
    "range_inc_cpu_code",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_vid:\r\n"
    "  By how much to increment the CPU code value for each system FEC \r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 \r\n"
    "  dest_sys_port_id 0 1 dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
#endif
    "",
    "",
    {BIT(15), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_RANGE_UP_TO_SYS_FEC_ID,
    "range_up_to_sys_fec",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_mac:\r\n"
    "  By how much to increment the CPU code value for each system FEC \r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 \r\n"
    "  dest_sys_port_id 0 1 dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
#endif
    "",
    "",
    {BIT(15), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_UC_ECMP_ADD_UC_ECMP_ADD_UC_FEC_RANGE_INC_VID_ID,
    "range_inc_out_vid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_mac:\r\n"
    "  By how much to increment the out vidvalue for each system FEC \r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_add sys_fec_ndx 0 add_type 0 enable_rpf\r\n"
    "  statistics_counter 0 1 cpu_code 0 1 out_vid 0 1 mac_addr 0 \r\n"
    "  dest_sys_port_id 0 1 dest_type 0 action 0  range_up_to_sys_fec 0\r\n"
    "  range_inc_mac 0 range_inc_out_vid 0 range_inc_cpu_code 0\r\n"
#endif
    "",
    "",
    {BIT(15), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
{
    PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_ID,
    "uc_ecmp_update_entry",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Update one FEC entry in the array of Unicast FEC entries (ECMP).\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_update_entry sys_fec_ndx 0 entry_ndx 0 statistics_counter\r\n"
    "  0 enable_rpf 0 cpu_code 0 out_vid 0 mac_addr 0 sys_port_id 0 type 0 action 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_SYS_FEC_NDX_ID,
    "sys_fec_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC sys_port_id. Range 0-(4G-1)\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_update_entry sys_fec_ndx 0 entry_ndx 0 statistics_counter\r\n"
    "  0 enable_rpf 0 cpu_code 0 out_vid 0 mac_addr 0 sys_port_id 0 type 0 action 0\r\n"
#endif
    "",
    "",
    {BIT(16), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_ENTRY_NDX_ID,
    "entry_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_ndx:\r\n"
    "  Number of valid entries in the array. Range 0-15.\r\n"
    "  if the entry out of the ECMP range error will be returned.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_update_entry sys_fec_ndx 0 entry_ndx 0 statistics_counter\r\n"
    "  0 enable_rpf 0 cpu_code 0 out_vid 0 mac_addr 0 sys_port_id 0 type 0 action 0\r\n"
#endif
    "",
    "",
    {BIT(16), 0, 0, 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_STATISTICS_COUNTER_ID,
    "statistics_counter",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_entry.statistics_counter:\r\n"
    "  Select a statistics counter that is incremented when this entry is accessed.\r\n"
    "  Range 0-7.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_update_entry sys_fec_ndx 0 entry_ndx 0 statistics_counter\r\n"
    "  0 enable_rpf 0 cpu_code 0 out_vid 0 mac_addr 0 sys_port_id 0 type 0 action 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_ENABLE_RPF_ID,
    "enable_rpf",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_entry.enable_rpf:\r\n"
    "  If TRUE, when searching with the IPv4 SrcIP during multicast routing, then\r\n"
    "  use the destination field of the entry for RPF check of multicast packets.\r\n"
    "  This applies only if RPF is not specified in the Multicast FEC Entry. In this\r\n"
    "  case, the Destination must match the incoming port for the RPF check to pass.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_update_entry sys_fec_ndx 0 entry_ndx 0 statistics_counter\r\n"
    "  0 enable_rpf 0 cpu_code 0 out_vid 0 mac_addr 0 sys_port_id 0 type 0 action 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_CPU_CODE_ID,
    "cpu_code",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_entry.cpu_code:\r\n"
    "  Value to be sent to the CPU(in the EPPH). Range 0-1023.Valid only if command\r\n"
    "  is Trap.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_update_entry sys_fec_ndx 0 entry_ndx 0 statistics_counter\r\n"
    "  0 enable_rpf 0 cpu_code 0 out_vid 0 mac_addr 0 sys_port_id 0 type 0 action 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_OUT_VID_ID,
    "out_vid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_entry.out_vid:\r\n"
    "  Outgoing VLAN ID.Rang 0-4095. \r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_update_entry sys_fec_ndx 0 entry_ndx 0 statistics_counter\r\n"
    "  0 enable_rpf 0 cpu_code 0 out_vid 0 mac_addr 0 sys_port_id 0 type 0 action 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_MAC_ADDR_ID,
    "mac_addr",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_entry.mac_addr:\r\n"
    "  MAC address to be used as DA for outgoing packet. (next hop MAC address)\r\n"
    "  Valid only if command is not Trap.Range 0-4095.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_update_entry sys_fec_ndx 0 entry_ndx 0 statistics_counter\r\n"
    "  0 enable_rpf 0 cpu_code 0 out_vid 0 mac_addr 0 sys_port_id 0 type 0 action 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_DEST_SYS_PORT_ID_ID,
    "sys_port_id",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_entry.dest_sys_port.id:\r\n"
    "  According to the System-Port type, one of the following:LAG sys_port_id. Range: 0 -\r\n"
    "  255.System Physical Port. Range: 0 - 4K-1.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_update_entry sys_fec_ndx 0 entry_ndx 0 statistics_counter\r\n"
    "  0 enable_rpf 0 cpu_code 0 out_vid 0 mac_addr 0 sys_port_id 0 type 0 action 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_DEST_SYS_PORT_TYPE_ID,
    "sys_port_type",
    (PARAM_VAL_RULES *)&SOC_PETRA_DEST_SYS_PORT_TYPE_rule[0],
    (sizeof(SOC_PETRA_DEST_SYS_PORT_TYPE_rule) / sizeof(SOC_PETRA_DEST_SYS_PORT_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_entry.dest_sys_port.type:\r\n"
    "  LAG sys_port_id or system-physical-port sys_port_id.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_update_entry sys_fec_ndx 0 entry_ndx 0 statistics_counter\r\n"
    "  0 enable_rpf 0 cpu_code 0 out_vid 0 mac_addr 0 sys_port_id 0 type 0 action 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_ECMP_UPDATE_ENTRY_UC_ECMP_UPDATE_ENTRY_UC_FEC_ENTRY_ACTION_ID,
    "action",
    (PARAM_VAL_RULES *)&SOC_PETRA_PP_SAND_PP_FEC_COMMAND_TYPE_rule[0],
    (sizeof(SOC_PETRA_PP_SAND_PP_FEC_COMMAND_TYPE_rule) / sizeof(SOC_PETRA_PP_SAND_PP_FEC_COMMAND_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_entry.action:\r\n"
    "  Drop, Route or Trap\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_update_entry sys_fec_ndx 0 entry_ndx 0 statistics_counter\r\n"
    "  0 enable_rpf 0 cpu_code 0 out_vid 0 mac_addr 0 sys_port_id 0 type 0 action 0\r\n"
#endif
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
    PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_ID,
    "mc_fec_add",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Add Multicast FEC, so Multicast packets lookup result with this sys_fec_d are\r\n"
    "  routed according to this FEC.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mc_fec_add sys_fec_ndx 0 add_type 0 action 0 dest_phy_ports 0 1\r\n"
    "  vids_dest_ports 0 1 dest_lags 0 1 vids_dest_lags 0 1\r\n"
    "  rpf_check 0 rpf_exp_in_port_id  0 rpf_exp_in_port_type 0 rpf_exp_in_vid 0\r\n"
    "  statistics_counter 0 range_up_to_sys_fec_ndx 0\r\n"
#endif
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
    PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_SYS_FEC_NDX_ID,
    "sys_fec_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC sys_port_id. Range 0-(4G-1)\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mc_fec_add sys_fec_ndx 0 add_type 0 action 0 dest_phy_ports 0 1\r\n"
    "  vids_dest_ports 0 1 dest_lags 0 1 vids_dest_lags 0 1\r\n"
    "  rpf_check 0 rpf_exp_in_port_id  0 rpf_exp_in_port_type 0 rpf_exp_in_vid 0\r\n"
    "  statistics_counter 0 range_up_to_sys_fec_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(17), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_ADD_TYPE_ID,
    "add_type",
    (PARAM_VAL_RULES *)&SWP_P_PP_FEC_ADD_TYPE_rule[0],
    (sizeof(SWP_P_PP_FEC_ADD_TYPE_rule) / sizeof(SWP_P_PP_FEC_ADD_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  add_type:\r\n"
    "  create new/override/concatenate.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mc_fec_add sys_fec_ndx 0 add_type 0 action 0 dest_phy_ports 0 1\r\n"
    "  vids_dest_ports 0 1 dest_lags 0 1 vids_dest_lags 0 1\r\n"
    "  rpf_check 0 rpf_exp_in_port_id  0 rpf_exp_in_port_type 0 rpf_exp_in_vid 0\r\n"
    "  statistics_counter 0 range_up_to_sys_fec_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(17), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_STATISTICS_COUNTER_ID,
    "statistics_counter",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.statistics_counter:\r\n"
    "  Select a statistics counter that is incremented when this entry is accessed.\r\n"
    "  Range 0-7.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mc_fec_add sys_fec_ndx 0 add_type 0 action 0 dest_phy_ports 0 1\r\n"
    "  vids_dest_ports 0 1 dest_lags 0 1 vids_dest_lags 0 1\r\n"
    "  rpf_check 0 rpf_exp_in_port_id  0 rpf_exp_in_port_type 0 rpf_exp_in_vid 0\r\n"
    "  statistics_counter 0 range_up_to_sys_fec_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(17), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_CPU_CODE_ID,
    "cpu_code",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.cpu_code:\r\n"
    "  Value to be sent to the CPU(in the EPPH). Range 0-1023.Valid only if command\r\n"
    "  is Trap.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mc_fec_add sys_fec_ndx 0 add_type 0 action 0 dest_phy_ports 0 1\r\n"
    "  vids_dest_ports 0 1 dest_lags 0 1 vids_dest_lags 0 1\r\n"
    "  rpf_check 0 rpf_exp_in_port_id  0 rpf_exp_in_port_type 0 rpf_exp_in_vid 0\r\n"
    "  statistics_counter 0 range_up_to_sys_fec_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(17), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_PORT_SYS_ID_ID,
    "rpf_exp_in_port_id",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.rpf_check.exp_in_port.sys_id:\r\n"
    "  The system port value, according to the type (Single or LAG)For LAG the value\r\n"
    "  is the group ID.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mc_fec_add sys_fec_ndx 0 add_type 0 action 0 dest_phy_ports 0 1\r\n"
    "  vids_dest_ports 0 1 dest_lags 0 1 vids_dest_lags 0 1\r\n"
    "  rpf_check 0 rpf_exp_in_port_id  0 rpf_exp_in_port_type 0 rpf_exp_in_vid 0\r\n"
    "  statistics_counter 0 range_up_to_sys_fec_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(17), 0, 0, 0},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_PORT_SYS_PORT_TYPE_ID,
    "rpf_exp_in_port_type",
    (PARAM_VAL_RULES *)&SOC_PETRA_DEST_SYS_PORT_TYPE_rule[0],
    (sizeof(SOC_PETRA_DEST_SYS_PORT_TYPE_rule) / sizeof(SOC_PETRA_DEST_SYS_PORT_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.rpf_check.exp_in_port.sys_port_type:\r\n"
    "  The system port type single/LAG.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mc_fec_add sys_fec_ndx 0 add_type 0 action 0 dest_phy_ports 0 1\r\n"
    "  vids_dest_ports 0 1 dest_lags 0 1 vids_dest_lags 0 1\r\n"
    "  rpf_check 0 rpf_exp_in_port_id  0 rpf_exp_in_port_type 0 rpf_exp_in_vid 0\r\n"
    "  statistics_counter 0 range_up_to_sys_fec_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(17), 0, 0, 0},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_VID_ID,
    "rpf_exp_in_vid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.rpf_check.exp_in_vid:\r\n"
    "  Expected incoming VID to use in the RPF check. If the incoming packet VID is\r\n"
    "  different than this value and check_vid is TRUE then the RPF check fail.\r\n"
    "  valid only if the rpf check consider the VID is TRUE. Range 1-4095.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mc_fec_add sys_fec_ndx 0 add_type 0 action 0 dest_phy_ports 0 1\r\n"
    "  vids_dest_ports 0 1 dest_lags 0 1 vids_dest_lags 0 1\r\n"
    "  rpf_check 0 rpf_exp_in_port_id  0 rpf_exp_in_port_type 0 rpf_exp_in_vid 0\r\n"
    "  statistics_counter 0 range_up_to_sys_fec_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(17), 0, 0, 0},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_RPF_CHECK_RPF_CHECK_ID,
    "rpf_check",
    (PARAM_VAL_RULES *)&SOC_PETRA_PP_FEC_RPF_CHECK_rule[0],
    (sizeof(SOC_PETRA_PP_FEC_RPF_CHECK_rule) / sizeof(SOC_PETRA_PP_FEC_RPF_CHECK_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.rpf_check.rpf_check:\r\n"
    "  According to which information to perform the rpf check.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mc_fec_add sys_fec_ndx 0 add_type 0 action 0 dest_phy_ports 0 1\r\n"
    "  vids_dest_ports 0 1 dest_lags 0 1 vids_dest_lags 0 1\r\n"
    "  rpf_check 0 rpf_exp_in_port_id  0 rpf_exp_in_port_type 0 rpf_exp_in_vid 0\r\n"
    "  statistics_counter 0 range_up_to_sys_fec_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(17), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_PORTS_VIDS_ID,
    "vids_dest_ports",
    (PARAM_VAL_RULES *)&gsa_api_members_vals[0],
    (sizeof(gsa_api_members_vals) / sizeof(gsa_api_members_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.vids[ prm_vids_index]:\r\n"
    "  the VLAN ID each copy is tagged with dests.dests_ports[i] tagged with VLAN ID\r\n"
    "  at vids[i] should be initialized according to number of copies\r\n"
    "  (dests.nof_ports)\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mc_fec_add sys_fec_ndx 0 add_type 0 action 0 dest_phy_ports 0 1\r\n"
    "  vids_dest_ports 0 1 dest_lags 0 1 vids_dest_lags 0 1\r\n"
    "  rpf_check 0 rpf_exp_in_port_id  0 rpf_exp_in_port_type 0 rpf_exp_in_vid 0\r\n"
    "  statistics_counter 0 range_up_to_sys_fec_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(17), 0, 0, 0},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_LAGS_VIDS_ID,
    "vids_dest_lags",
    (PARAM_VAL_RULES *)&gsa_api_members_vals[0],
    (sizeof(gsa_api_members_vals) / sizeof(gsa_api_members_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.vids[ prm_vids_index]:\r\n"
    "  the VLAN ID each copy is tagged with dests.dests_ports[i] tagged with VLAN ID\r\n"
    "  at vids[i] should be initialized according to number of copies\r\n"
    "  (dests.nof_ports)\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mc_fec_add sys_fec_ndx 0 add_type 0 action 0 dest_phy_ports 0 1\r\n"
    "  vids_dest_ports 0 1 dest_lags 0 1 vids_dest_lags 0 1\r\n"
    "  rpf_check 0 rpf_exp_in_port_id  0 rpf_exp_in_port_type 0 rpf_exp_in_vid 0\r\n"
    "  statistics_counter 0 range_up_to_sys_fec_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(17), 0, 0, 0},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_DESTS_DESTS_PHY_PORTS_ID_ID,
    "dest_phy_ports",
    (PARAM_VAL_RULES *)&gsa_api_members_vals[0],
    (sizeof(gsa_api_members_vals) / sizeof(gsa_api_members_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.dests.dests_ports.id:\r\n"
    "  List of destination physical ports. Range: 0 - 4K-1\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mc_fec_add sys_fec_ndx 0 add_type 0 action 0 dest_phy_ports 0 1\r\n"
    "  vids_dest_ports 0 1 dest_lags 0 1 vids_dest_lags 0 1\r\n"
    "  rpf_check 0 rpf_exp_in_port_id  0 rpf_exp_in_port_type 0 rpf_exp_in_vid 0\r\n"
    "  statistics_counter 0 range_up_to_sys_fec_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(17), 0, 0, 0},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_DESTS_DESTS_LAGS_ID_ID,
    "dest_lags",
    (PARAM_VAL_RULES *)&gsa_api_members_vals[0],
    (sizeof(gsa_api_members_vals) / sizeof(gsa_api_members_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.dests.dests_ports.id:\r\n"
    "  List of destination Lags. Range: 0 -255.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mc_fec_add sys_fec_ndx 0 add_type 0 action 0 dest_phy_ports 0 1\r\n"
    "  vids_dest_ports 0 1 dest_lags 0 1 vids_dest_lags 0 1\r\n"
    "  rpf_check 0 rpf_exp_in_port_id  0 rpf_exp_in_port_type 0 rpf_exp_in_vid 0\r\n"
    "  statistics_counter 0 range_up_to_sys_fec_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(17), 0, 0, 0},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_ENTRY_ACTION_ID,
    "action",
    (PARAM_VAL_RULES *)&SOC_PETRA_PP_SAND_PP_FEC_COMMAND_TYPE_rule[0],
    (sizeof(SOC_PETRA_PP_SAND_PP_FEC_COMMAND_TYPE_rule) / sizeof(SOC_PETRA_PP_SAND_PP_FEC_COMMAND_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.action:\r\n"
    "  Drop, Route or Trap\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mc_fec_add sys_fec_ndx 0 add_type 0 action 0 dest_phy_ports 0 1\r\n"
    "  vids_dest_ports 0 1 dest_lags 0 1 vids_dest_lags 0 1\r\n"
    "  rpf_check 0 rpf_exp_in_port_id  0 rpf_exp_in_port_type 0 rpf_exp_in_vid 0\r\n"
    "  statistics_counter 0 range_up_to_sys_fec_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(17), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_ADD_MC_FEC_ADD_MC_FEC_RANGE_UP_TO_SYS_FEC_NDX_ID,
    "range_up_to_sys_fec_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.action:\r\n"
    "  Drop, Route or Trap\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api mc_fec_add sys_fec_ndx 0 add_type 0 action 0 dest_phy_ports 0 1\r\n"
    "  vids_dest_ports 0 1 dest_lags 0 1 vids_dest_lags 0 1\r\n"
    "  rpf_check 0 rpf_exp_in_port_id  0 rpf_exp_in_port_type 0 rpf_exp_in_vid 0\r\n"
    "  statistics_counter 0 range_up_to_sys_fec_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(17), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_GSA_FEC_UC_EMCP_REMOVE_ENTRY_UC_EMCP_REMOVE_ENTRY_ID,
    "uc_ecmp_remove_entry",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove one FEC from the ECMP.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_remove_entry sys_fec_ndx 0 fec_entry_ndx 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_EMCP_REMOVE_ENTRY_UC_EMCP_REMOVE_ENTRY_SYS_FEC_NDX_ID,
    "sys_fec_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC sys_port_id.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_remove_entry sys_fec_ndx 0 fec_entry_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(18), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_UC_EMCP_REMOVE_ENTRY_UC_EMCP_REMOVE_ENTRY_FEC_ENTRY_NDX_ID,
    "fec_entry_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  fec_entry_ndx:\r\n"
    "  the order of the FEC in the ECMP.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_remove_entry sys_fec_ndx 0 fec_entry_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(18), 0, 0, 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_REMOVE_REMOVE_ID,
    "remove",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Removes all the FEC entries that are identified by this\r\n"
    "  sys_fec_ndx, the FEC entry has to be not in use by any routing or\r\n"
    "  ACL table.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec remove sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
#endif
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
    PARAM_GSA_FEC_REMOVE_REMOVE_SYS_FEC_NDX_ID,
    "sys_fec_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC sys_port_id. Range 0-(4G-1). \r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec remove sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(19), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_REMOVE_UP_TO_SYS_FEC_NDX_ID,
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
    "  gsa_api fec remove sys_fec_ndx 0 up_to_sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(19), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_UC_GET_UC_GET_ID,
    "uc_get",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get Unicast FEC entry/entries that are associated with the given sys_fec_ndx.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_get sys_fec_ndx 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_GET_UC_GET_SYS_FEC_NDX_ID,
    "sys_fec_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC sys_port_id. Range 0-(4G-1)\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_get sys_fec_ndx 0\r\n"
#endif
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
    PARAM_GSA_FEC_MC_GET_MC_GET_ID,
    "mc_get",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get Multicast FEC entry that is associated with the given sys_fec_ndx.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec mc_get sys_fec_ndx 0\r\n"
#endif
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
    PARAM_GSA_FEC_MC_GET_MC_GET_SYS_FEC_NDX_ID,
    "sys_fec_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC sys_port_id. Range 0-(4G-1)\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec mc_get sys_fec_ndx 0\r\n"
#endif
    "",
    "",
    {BIT(21), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_TABLE_UC_GET_BLOCK_TABLE_UC_GET_BLOCK_ID,
    "uc_get_block",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Traverse the FEC table and get all the UC FEC entries.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_get_block start_from 0\r\n"
#endif
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
    PARAM_GSA_FEC_TABLE_UC_GET_BLOCK_TABLE_UC_GET_BLOCK_START_FROM_ID,
    "start_from",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Traverse the FEC table and get all the UC FEC entries.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_get_block start_from 0\r\n"
#endif
    "",
    "",
    {BIT(22), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_TABLE_MC_GET_BLOCK_TABLE_MC_GET_BLOCK_ID,
    "mc_get_block",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Traverse the FEC table and get all the MC FEC entries.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec mc_get_block start_from 0\r\n"
#endif
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
    PARAM_GSA_FEC_TABLE_MC_GET_BLOCK_TABLE_MC_GET_BLOCK_START_FROM_ID,
    "start_from",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Traverse the FEC table and get all the UC FEC entries.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec mc_get_block start_from 0\r\n"
#endif
    "",
    "",
    {BIT(23), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_ID,
    "uc_ecmp_range_add",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Add Range Unicast FEC entries (ECMP) equal-cost-multi-path entry, so Unicast\r\n"
    "  packets lookup result with this sys_fec_d are routed along one of these FEC\r\n"
    "  entries while ensuring load balancing. The addition type is new_override.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 sys_port_id 0 type 0 action 0 nof_entries 0 inc_cpu_code 0 inc_vid 0\r\n"
    "  inc_mac 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_SYS_FEC_NDX_ID,
    "sys_fec_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC sys_port_id. Range 0-(4G-1)\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 sys_port_id 0 type 0 action 0 nof_entries 0 inc_cpu_code 0 inc_vid 0\r\n"
    "  inc_mac 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UP_TO_SYS_FEC_NDX_ID,
    "up_to_sys_fec_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  up_to_sys_fec_ndx:\r\n"
    "  range upper limit of system FECs to add . Range 0-(4G-1)\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 sys_port_id 0 type 0 action 0 nof_entries 0 inc_cpu_code 0 inc_vid 0\r\n"
    "  inc_mac 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_INDEX_ID,
    "uc_fec_array_index",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array_index:\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 sys_port_id 0 type 0 action 0 nof_entries 0 inc_cpu_code 0 inc_vid 0\r\n"
    "  inc_mac 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_STATISTICS_COUNTER_ID,
    "statistics_counter",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].statistics_counter:\r\n"
    "  Select a statistics counter that is incremented when this entry is accessed.\r\n"
    "  Range 0-7.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 sys_port_id 0 type 0 action 0 nof_entries 0 inc_cpu_code 0 inc_vid 0\r\n"
    "  inc_mac 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_ENABLE_RPF_ID,
    "enable_rpf",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].enable_rpf:\r\n"
    "  If TRUE, when searching with the IPv4 SrcIP during multicast routing, then\r\n"
    "  use the destination field of the entry for RPF check of multicast packets.\r\n"
    "  This applies only if RPF is not specified in the Multicast FEC Entry. In this\r\n"
    "  case, the Destination must match the incoming port for the RPF check to pass.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 sys_port_id 0 type 0 action 0 nof_entries 0 inc_cpu_code 0 inc_vid 0\r\n"
    "  inc_mac 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_CPU_CODE_ID,
    "cpu_code",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].cpu_code:\r\n"
    "  Value to be sent to the CPU(in the EPPH). Range 0-1023.Valid only if command\r\n"
    "  is Trap.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 sys_port_id 0 type 0 action 0 nof_entries 0 inc_cpu_code 0 inc_vid 0\r\n"
    "  inc_mac 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_OUT_VID_ID,
    "out_vid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].out_vid:\r\n"
    "  Outgoing VLAN ID.Rang 0-4095. \r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 sys_port_id 0 type 0 action 0 nof_entries 0 inc_cpu_code 0 inc_vid 0\r\n"
    "  inc_mac 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_MAC_ADDR_ID,
    "mac_addr",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_mac_addr_vals[0],
    (sizeof(soc_petra_pp_api_mac_addr_vals) / sizeof(soc_petra_pp_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].mac_addr:\r\n"
    "  MAC address to be used as DA for outgoing packet. (next hop MAC address)\r\n"
    "  Valid only if command is not Trap.Range 0-4095.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 sys_port_id 0 type 0 action 0 nof_entries 0 inc_cpu_code 0 inc_vid 0\r\n"
    "  inc_mac 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_DEST_SYS_PORT_ID_ID,
    "sys_port_id",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].dest_sys_port.id:\r\n"
    "  According to the System-Port type, one of the following:LAG sys_port_id. Range: 0 -\r\n"
    "  255.System Physical Port. Range: 0 - 4K-1.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 sys_port_id 0 type 0 action 0 nof_entries 0 inc_cpu_code 0 inc_vid 0\r\n"
    "  inc_mac 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_DEST_SYS_PORT_TYPE_ID,
    "type",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].dest_sys_port.type:\r\n"
    "  LAG sys_port_id or system-physical-port sys_port_id.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 sys_port_id 0 type 0 action 0 nof_entries 0 inc_cpu_code 0 inc_vid 0\r\n"
    "  inc_mac 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_UC_FEC_ARRAY_ACTION_ID,
    "action",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  uc_fec_array[ prm_uc_fec_array_index].action:\r\n"
    "  Drop, Route or Trap\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 sys_port_id 0 type 0 action 0 nof_entries 0 inc_cpu_code 0 inc_vid 0\r\n"
    "  inc_mac 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_NOF_ENTRIES_ID,
    "nof_entries",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  nof_entries:\r\n"
    "  Number of valid entries in the array. Range 1-16.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 sys_port_id 0 type 0 action 0 nof_entries 0 inc_cpu_code 0 inc_vid 0\r\n"
    "  inc_mac 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_RANGE_INFO_INC_CPU_CODE_ID,
    "inc_cpu_code",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_cpu_code:\r\n"
    "  By how much to increment the CPU code value in each entry.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 sys_port_id 0 type 0 action 0 nof_entries 0 inc_cpu_code 0 inc_vid 0\r\n"
    "  inc_mac 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_RANGE_INFO_INC_VID_ID,
    "inc_vid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_vid:\r\n"
    "  By how much to increment the VID value in each entry.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 sys_port_id 0 type 0 action 0 nof_entries 0 inc_cpu_code 0 inc_vid 0\r\n"
    "  inc_mac 0\r\n"
#endif
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
    PARAM_GSA_FEC_UC_ECMP_RANGE_ADD_UC_ECMP_RANGE_ADD_RANGE_INFO_INC_MAC_ID,
    "inc_mac",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_mac:\r\n"
    "  By how much to increment the MAC value in each entry. For ECMP this the added\r\n"
    "  value for the corresponding FEC in the previous ECMP. Zero to make no change. \r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec uc_ecmp_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  uc_fec_array_index 0 statistics_counter 0 enable_rpf 0 cpu_code 0 out_vid 0\r\n"
    "  mac_addr 0 sys_port_id 0 type 0 action 0 nof_entries 0 inc_cpu_code 0 inc_vid 0\r\n"
    "  inc_mac 0\r\n"
#endif
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
    PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_ID,
    "mc_fec_range_add",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Add Range of Multicast FEC entries, so Multicast packets lookup result with\r\n"
    "  this sys_fec_id are routed according to this FEC.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  rpf_check 0 vids_index 0 vids 0 nof_ports 0 dests_ports_index 0 sys_port_id 0 type 0\r\n"
    "  action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
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
    PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_SYS_FEC_NDX_ID,
    "sys_fec_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC sys_port_id. Range 0-(4G-1)\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  rpf_check 0 vids_index 0 vids 0 nof_ports 0 dests_ports_index 0 sys_port_id 0 type 0\r\n"
    "  action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
    "",
    "",
    {BIT(25), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_UP_TO_SYS_FEC_NDX_ID,
    "up_to_sys_fec_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  up_to_sys_fec_ndx:\r\n"
    "  range upper limit of system FECs to add . Range 0-(4G-1)\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  rpf_check 0 vids_index 0 vids 0 nof_ports 0 dests_ports_index 0 sys_port_id 0 type 0\r\n"
    "  action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
    "",
    "",
    {BIT(25), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_STATISTICS_COUNTER_ID,
    "statistics_counter",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.statistics_counter:\r\n"
    "  Select a statistics counter that is incremented when this entry is accessed.\r\n"
    "  Range 0-7.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  rpf_check 0 vids_index 0 vids 0 nof_ports 0 dests_ports_index 0 sys_port_id 0 type 0\r\n"
    "  action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
    "",
    "",
    {BIT(25), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_CPU_CODE_ID,
    "cpu_code",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.cpu_code:\r\n"
    "  Value to be sent to the CPU(in the EPPH). Range 0-1023.Valid only if command\r\n"
    "  is Trap.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  rpf_check 0 vids_index 0 vids 0 nof_ports 0 dests_ports_index 0 sys_port_id 0 type 0\r\n"
    "  action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
    "",
    "",
    {BIT(25), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_PORT_SYS_ID_ID,
    "sys_id",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.rpf_check.exp_in_port.sys_id:\r\n"
    "  The system port value, according to the type (Single or LAG)For LAG the value\r\n"
    "  is the group ID.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  rpf_check 0 vids_index 0 vids 0 nof_ports 0 dests_ports_index 0 sys_port_id 0 type 0\r\n"
    "  action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
    "",
    "",
    {BIT(25), 0, 0, 0},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_PORT_SYS_PORT_TYPE_ID,
    "sys_port_type",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.rpf_check.exp_in_port.sys_port_type:\r\n"
    "  The system port type single/LAG.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  rpf_check 0 vids_index 0 vids 0 nof_ports 0 dests_ports_index 0 sys_port_id 0 type 0\r\n"
    "  action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
    "",
    "",
    {BIT(25), 0, 0, 0},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_RPF_CHECK_EXP_IN_VID_ID,
    "exp_in_vid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.rpf_check.exp_in_vid:\r\n"
    "  Expected incoming VID to use in the RPF check. If the incoming packet VID is\r\n"
    "  different than this value and check_vid is TRUE then the RPF check fail.\r\n"
    "  valid only if the rpf check consider the VID is TRUE. Range 1-4095.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  rpf_check 0 vids_index 0 vids 0 nof_ports 0 dests_ports_index 0 sys_port_id 0 type 0\r\n"
    "  action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
    "",
    "",
    {BIT(25), 0, 0, 0},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_RPF_CHECK_RPF_CHECK_ID,
    "rpf_check",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.rpf_check.rpf_check:\r\n"
    "  According to which information to perform the rpf check.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  rpf_check 0 vids_index 0 vids 0 nof_ports 0 dests_ports_index 0 sys_port_id 0 type 0\r\n"
    "  action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
    "",
    "",
    {BIT(25), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_VIDS_INDEX_ID,
    "vids_index",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vids_index:\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  rpf_check 0 vids_index 0 vids 0 nof_ports 0 dests_ports_index 0 sys_port_id 0 type 0\r\n"
    "  action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
    "",
    "",
    {BIT(25), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_VIDS_ID,
    "vids",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.vids[ prm_vids_index]:\r\n"
    "  the VLAN ID each copy is tagged with dests.dests_ports[i] tagged with VLAN ID\r\n"
    "  at vids[i] should be initialized according to number of copies\r\n"
    "  (dests.nof_ports)\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  rpf_check 0 vids_index 0 vids 0 nof_ports 0 dests_ports_index 0 sys_port_id 0 type 0\r\n"
    "  action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
    "",
    "",
    {BIT(25), 0, 0, 0},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_DESTS_NOF_PORTS_ID,
    "nof_ports",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.dests.nof_ports:\r\n"
    "  Number of port\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  rpf_check 0 vids_index 0 vids 0 nof_ports 0 dests_ports_index 0 sys_port_id 0 type 0\r\n"
    "  action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
    "",
    "",
    {BIT(25), 0, 0, 0},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_DESTS_DESTS_PORTS_INDEX_ID,
    "dests_ports_index",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  dests_ports_index:\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  rpf_check 0 vids_index 0 vids 0 nof_ports 0 dests_ports_index 0 sys_port_id 0 type 0\r\n"
    "  action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
    "",
    "",
    {BIT(25), 0, 0, 0},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_DESTS_DESTS_PORTS_ID_ID,
    "sys_port_id",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.dests.dests_ports[ prm_dests_ports_index].id:\r\n"
    "  According to the System-Port type, one of the following:LAG sys_port_id. Range: 0 -\r\n"
    "  255.System Physical Port. Range: 0 - 4K-1.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  rpf_check 0 vids_index 0 vids 0 nof_ports 0 dests_ports_index 0 sys_port_id 0 type 0\r\n"
    "  action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
    "",
    "",
    {BIT(25), 0, 0, 0},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_DESTS_DESTS_PORTS_TYPE_ID,
    "type",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.dests.dests_ports[ prm_dests_ports_index].type:\r\n"
    "  LAG sys_port_id or system-physical-port sys_port_id.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  rpf_check 0 vids_index 0 vids 0 nof_ports 0 dests_ports_index 0 sys_port_id 0 type 0\r\n"
    "  action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
    "",
    "",
    {BIT(25), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_MC_FEC_ENTRY_ACTION_ID,
    "action",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mc_fec_entry.action:\r\n"
    "  Drop, Route or Trap\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  rpf_check 0 vids_index 0 vids 0 nof_ports 0 dests_ports_index 0 sys_port_id 0 type 0\r\n"
    "  action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
    "",
    "",
    {BIT(25), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_RANGE_INFO_INC_CPU_CODE_ID,
    "inc_cpu_code",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_cpu_code:\r\n"
    "  By how much to increment the CPU code value in each entry.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  rpf_check 0 vids_index 0 vids 0 nof_ports 0 dests_ports_index 0 sys_port_id 0 type 0\r\n"
    "  action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
    "",
    "",
    {BIT(25), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_MC_FEC_RANGE_ADD_MC_FEC_RANGE_ADD_RANGE_INFO_INC_DEST_ID,
    "inc_dest",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_dest:\r\n"
    "  By how much to increment the Destination Multicast Group value in each entry.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec mc_fec_range_add sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
    "  statistics_counter 0 cpu_code 0 sys_id 0 sys_port_type 0 exp_in_vid 0\r\n"
    "  rpf_check 0 vids_index 0 vids 0 nof_ports 0 dests_ports_index 0 sys_port_id 0 type 0\r\n"
    "  action 0 inc_cpu_code 0 inc_dest 0\r\n"
#endif
    "",
    "",
    {BIT(25), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_FEC_RANGE_REMOVE_RANGE_REMOVE_ID,
    "range_remove",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove Range FEC entries.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec range_remove sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
#endif
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
    PARAM_GSA_FEC_RANGE_REMOVE_RANGE_REMOVE_SYS_FEC_NDX_ID,
    "sys_fec_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC sys_port_id. Range 0-(4G-1)\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec range_remove sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
#endif
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
    PARAM_GSA_FEC_RANGE_REMOVE_RANGE_REMOVE_UP_TO_SYS_FEC_NDX_ID,
    "up_to_sys_fec_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  up_to_sys_fec_ndx:\r\n"
    "  range upper limit of system FECs to add . Range 0-(4G-1)\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec range_remove sys_fec_ndx 0 up_to_sys_fec_ndx 0\r\n"
#endif
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
    PARAM_GSA_FEC_TABLE_CLEAR_TABLE_CLEAR_ID,
    "table_clear",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove Range FEC entries.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api fec table_clear remove_default 0\r\n"
#endif
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
    PARAM_GSA_API_FEC_ID,
    "fec",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(15)|BIT(16)|BIT(17)|BIT(18)|BIT(19)|BIT(20)|BIT(21)|BIT(22)|BIT(23)/*|BIT(24)*//*|BIT(25)*//*|BIT(26)*/|BIT(27), 0, 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } fec*/
#ifdef UI_IPV4_UC/* { ipv4_uc*/
  {
    PARAM_GSA_IPV4_UC_ROUTE_ADD_ROUTE_ADD_ID,
    "add_route",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Binds between Ipv4 UC route key (S,G,vlan/port) and a FEC entry identified by\r\n"
    "  the given sys_fec_id for a given virtual router. As a result of this\r\n"
    "  operation Ipv4 UC packets designated to IP address match the given key (as\r\n"
    "  long there is no more specific route key) will be routed according to the\r\n"
    "  information in the FEC entry identified by sys_fec_id.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api ipv4_uc add_route prefix_len 0 ip_address 0 sys_fec_id 0 override 0\r\n"
#endif
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
    PARAM_GSA_IPV4_UC_ROUTE_ADD_ROUTE_ADD_ROUTE_KEY_PREFIX_LEN_ID,
    "prefix_len",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api ipv4_uc add_route prefix_len 0 ip_address 0 sys_fec_id 0 override 0\r\n"
#endif
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
    PARAM_GSA_IPV4_UC_ROUTE_ADD_ROUTE_ADD_ROUTE_KEY_IP_ADDRESS_ID,
    "ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api ipv4_uc add_route prefix_len 0 ip_address 0 sys_fec_id 0 override 0\r\n"
#endif
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
    PARAM_GSA_IPV4_UC_ROUTE_ADD_ROUTE_ADD_SYS_FEC_ID_ID,
    "sys_fec_id",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_id:\r\n"
    "  system FEC sys_port_id. Range 0- 4G\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api ipv4_uc add_route prefix_len 0 ip_address 0 sys_fec_id 0 override 0\r\n"
#endif
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
    PARAM_GSA_IPV4_UC_ROUTE_ADD_ROUTE_ADD_OVERRIDE_ID,
    "override",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  override:\r\n"
    "  whether to override the entry if it already exists, if this is\r\n"
    "  FALSE and the entry exist error is returned.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api ipv4_uc add_route prefix_len 0 ip_address 0 sys_fec_id 0 override 0\r\n"
#endif
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
    PARAM_GSA_IPV4_UC_ROUTE_GET_ROUTE_GET_ID,
    "get_route",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Gets the system-FEC-id associated with the given key (IP-\r\n"
    "  address\\prefix).\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api ipv4_uc get_route prefix_len 0 ip_address 0 exact_match 0\r\n"
#endif
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
    PARAM_GSA_IPV4_UC_ROUTE_GET_ROUTE_GET_ROUTE_KEY_PREFIX_LEN_ID,
    "prefix_len",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api ipv4_uc get_route prefix_len 0 ip_address 0 exact_match 0\r\n"
#endif
    "",
    "",
    {BIT(29), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_UC_ROUTE_GET_ROUTE_GET_ROUTE_KEY_IP_ADDRESS_ID,
    "ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api ipv4_uc get_route prefix_len 0 ip_address 0 exact_match 0\r\n"
#endif
    "",
    "",
    {BIT(29), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_UC_ROUTE_GET_ROUTE_GET_EXACT_MATCH_ID,
    "exact_match",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  exact_match:\r\n"
    "  to find only exact match or best match.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api ipv4_uc get_route prefix_len 0 ip_address 0 exact_match 0\r\n"
#endif
    "",
    "",
    {BIT(29), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_UC_ROUTE_LPM_GET_ROUTE_LPM_GET_ID,
    "lpm_get",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get from the routing table a route key with the longest prefix\r\n"
    "  that matches the given key.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api ipv4_uc lpm_get prefix_len 0 ip_address 0\r\n"
#endif
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
    PARAM_GSA_IPV4_UC_ROUTE_LPM_GET_ROUTE_LPM_GET_ROUTE_KEY_PREFIX_LEN_ID,
    "prefix_len",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api ipv4_uc lpm_get prefix_len 0 ip_address 0\r\n"
#endif
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
    PARAM_GSA_IPV4_UC_ROUTE_LPM_GET_ROUTE_LPM_GET_ROUTE_KEY_IP_ADDRESS_ID,
    "ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api ipv4_uc lpm_get prefix_len 0 ip_address 0\r\n"
#endif
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
    PARAM_GSA_IPV4_UC_GET_BLOCK_GET_BLOCK_ID,
    "get_block",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get block of entries from the routing table in a given range.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api ipv4_uc get_block\r\n"
#endif
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
    PARAM_GSA_IPV4_UC_GET_BLOCK_GET_BLOCK_SHOW_FEC_INFO_ID,
    "show_fec_info",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get block of entries from the routing table in a given range.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api ipv4_uc get_block\r\n"
#endif
    "",
    "",
    {BIT(31), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_UC_PRINT_BLOCK_PRINT_BLOCK_ID,
    "print_block",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get block of entries from the routing table in a given range.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api ipv4_uc print_block sys_fec_id 0 prefix_len 0 ip_address 0\r\n"
    "  nof_entries 0 flavors 0\r\n"
#endif
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
    PARAM_GSA_IPV4_UC_PRINT_BLOCK_PRINT_BLOCK_ROUTE_TABLE_SYS_FEC_ID_ID,
    "sys_fec_id",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_table.sys_fec_id:\r\n"
    "  The FEC pointer associated with the key.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api ipv4_uc print_block sys_fec_id 0 prefix_len 0 ip_address 0\r\n"
    "  nof_entries 0 flavors 0\r\n"
#endif
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
    PARAM_GSA_IPV4_UC_PRINT_BLOCK_PRINT_BLOCK_ROUTE_TABLE_KEY_PREFIX_LEN_ID,
    "prefix_len",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_table.key.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api ipv4_uc print_block sys_fec_id 0 prefix_len 0 ip_address 0\r\n"
    "  nof_entries 0 flavors 0\r\n"
#endif
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
    PARAM_GSA_IPV4_UC_PRINT_BLOCK_PRINT_BLOCK_ROUTE_TABLE_KEY_IP_ADDRESS_ID,
    "ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_table.key.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api ipv4_uc print_block sys_fec_id 0 prefix_len 0 ip_address 0\r\n"
    "  nof_entries 0 flavors 0\r\n"
#endif
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
    PARAM_GSA_IPV4_UC_PRINT_BLOCK_PRINT_BLOCK_NOF_ENTRIES_ID,
    "nof_entries",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  nof_entries:\r\n"
    "  number of valid entries in route_table.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api ipv4_uc print_block sys_fec_id 0 prefix_len 0 ip_address 0\r\n"
    "  nof_entries 0 flavors 0\r\n"
#endif
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
    PARAM_GSA_IPV4_UC_PRINT_BLOCK_PRINT_BLOCK_FLAVORS_ID,
    "flavors",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flavors:\r\n"
    "  how and what to print.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api ipv4_uc print_block sys_fec_id 0 prefix_len 0 ip_address 0\r\n"
    "  nof_entries 0 flavors 0\r\n"
#endif
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
    PARAM_GSA_IPV4_UC_ROUTE_REMOVE_ROUTE_REMOVE_ID,
    "remove_route",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove the association of the IP-address to FEC.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api ipv4_uc remove_route prefix_len 0 ip_address 0\r\n"
#endif
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
    PARAM_GSA_IPV4_UC_ROUTE_REMOVE_ROUTE_REMOVE_ROUTE_KEY_PREFIX_LEN_ID,
    "prefix_len",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api ipv4_uc remove_route prefix_len 0 ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(1), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_UC_ROUTE_REMOVE_ROUTE_REMOVE_ROUTE_KEY_IP_ADDRESS_ID,
    "ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api ipv4_uc remove_route prefix_len 0 ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(1), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_UC_ROUTES_CLEAR_ROUTES_CLEAR_ID,
    "clear_routing_table",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove all the entries in the route table of the given virtual router.\r\n"
    "",
#if UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api ipv4_uc clear_routing_table\r\n"
#endif
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
    PARAM_GSA_API_IPV4_UC_ID,
    "ipv4_uc",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(28)|BIT(29)|BIT(30)|BIT(31), BIT(0)|BIT(1)|BIT(2), 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } ipv4_uc*/
#ifdef UI_IPV4_MC/* { ipv4_mc*/
  {
    PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_INIT_ID,
    "init",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  INIT ipv4 mc module\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc init use_mapped_port \r\n"
#endif
    "",
    "",
    {0, BIT(13), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_INIT_USE_PORT_ID,
    "use_port",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  IF presents use dest IP x src IP x in port \r\n"
    "  IF does NOT present use dest IP x src IP x in VLAN ID for routing \r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc init use_mapped_port \r\n"
#endif
    "",
    "",
    {0, BIT(13), 0, 0, 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ID,
    "add",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Binds between an ipv4 key (Src-Ip/prefix and group) and a multicast FEC\r\n"
    "  identifier.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_add in_vid_mask 0 in_vid 0 src_ip_address\r\n"
    "0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {0, BIT(14), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_IN_VID_VALID_ID,
    "in_vid_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid_mask:\r\n"
    "  Is the in_vid valid.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_add in_vid_mask 0 in_vid 0 src_ip_address\r\n"
    "0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {0, BIT(14), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_IN_VID_ID,
    "in_vid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid:\r\n"
    "  The vid of the incoming packets.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_add in_vid_mask 0 in_vid 0 src_ip_address\r\n"
    "0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {0, BIT(14), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_IN_PORT_VALID_ID,
    "in_port_valid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port_valid:\r\n"
    "  Is the in_port valid.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_add in_port_valid 0 in_port\r\n"
    "0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {0, BIT(14), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_IN_PORT_ID,
    "in_port",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port:\r\n"
    "  The port of the incoming packets.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_add in_port_valid 0 in_port\r\n"
    "0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {0, BIT(14), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_GROUP_PREFIX_LEN_ID,
    "group_prefix_len",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_add in_vid_mask 0 in_vid 0 src_ip_address\r\n"
    "0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {0, BIT(14), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_GROUP_IP_ADDRESS_ID,
    "group_ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_add in_vid_mask 0 in_vid 0 src_ip_address\r\n"
    "0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {0, BIT(14), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_SOURCE_PREFIX_LEN_ID,
    "src_prefix_len",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.source.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_add in_vid_mask 0 in_vid 0 src_ip_address\r\n"
    "0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {0, BIT(14), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_ROUTE_KEY_SOURCE_IP_ADDRESS_ID,
    "src_ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.source.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_add in_vid_mask 0 in_vid 0 src_ip_address\r\n"
    "0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {0, BIT(14), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_SYS_FEC_NDX_ID,
    "sys_fec_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC id. Range 0- 4G\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_add in_vid_mask 0 in_vid 0 src_ip_address\r\n"
    "0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {0, BIT(14), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_ADD_IPV4_MC_ROUTE_ADD_OVERRIDE_ID,
    "override",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  override:\r\n"
    "  whether to override the entry if it already exists, if this is\r\n"
    "  FALSE and the entry exist error is returned.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_add in_vid_mask 0 in_vid 0 src_ip_address\r\n"
    "0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {0, BIT(14), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ID,
    "get",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Gets the system-FEC-id associated the given key.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_get in_vid_mask 0 in_vid\r\n"
    "  0 group_prefix_len 0 group_ip_address 0 src_prefix_len 0 src_ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(15), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_IN_VID_VALID_ID,
    "in_vid_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid_mask:\r\n"
    "  Is the in_vid valid.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_get in_vid_mask 0 in_vid\r\n"
    "  0 group_prefix_len 0 group_ip_address 0 src_prefix_len 0 src_ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(15), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_IN_VID_ID,
    "in_vid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid:\r\n"
    "  The vid of the incoming packets.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_get in_vid_mask 0 in_vid\r\n"
    "  0 group_prefix_len 0 group_ip_address 0 src_prefix_len 0 src_ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(15), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_IN_PORT_VALID_ID,
    "in_port_valid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port_valid:\r\n"
    "  Is the in_port valid.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_get in_port_valid 0 in_port\r\n"
    "  0 group_prefix_len 0 group_ip_address 0 src_prefix_len 0 src_ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(15), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_IN_PORT_ID,
    "in_port",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port:\r\n"
    "  The port of the incoming packets.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_get in_port_valid 0 in_port\r\n"
    "  0 group_prefix_len 0 group_ip_address 0 src_prefix_len 0 src_ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(15), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_GROUP_PREFIX_LEN_ID,
    "group_prefix_len",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_get in_vid_mask 0 in_vid\r\n"
    "  0 group_prefix_len 0 group_ip_address 0 src_prefix_len 0 src_ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(15), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_GROUP_IP_ADDRESS_ID,
    "group_ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_get in_vid_mask 0 in_vid\r\n"
    "  0 group_prefix_len 0 group_ip_address 0 src_prefix_len 0 src_ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(15), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_SOURCE_PREFIX_LEN_ID,
    "src_prefix_len",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.source.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_get in_vid_mask 0 in_vid\r\n"
    "  0 group_prefix_len 0 group_ip_address 0 src_prefix_len 0 src_ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(15), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_GET_IPV4_MC_ROUTE_GET_ROUTE_KEY_SOURCE_IP_ADDRESS_ID,
    "src_ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.source.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_get in_vid_mask 0 in_vid\r\n"
    "  0 group_prefix_len 0 group_ip_address 0 src_prefix_len 0 src_ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(15), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ID,
    "lpm_get",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get from the route table a route key with the longest prefix that matches the\r\n"
    "  given key.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_lpm_get in_vid_mask 0\r\n"
    "  in_vid 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(16), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_IN_VID_VALID_ID,
    "in_vid_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid_mask:\r\n"
    "  Is the in_vid valid.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_lpm_get in_vid_mask 0\r\n"
    "  in_vid 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(16), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_IN_VID_ID,
    "in_vid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid:\r\n"
    "  The vid of the incoming packets.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_lpm_get in_vid_mask 0\r\n"
    "  in_vid 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(16), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_IN_PORT_VALID_ID,
    "in_port_valid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port_valid:\r\n"
    "  Is the in_port valid.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_lpm_get in_port_valid 0\r\n"
    "  in_port 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(16), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_IN_PORT_ID,
    "in_port",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port:\r\n"
    "  The port of the incoming packets.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_lpm_get in_port_valid 0\r\n"
    "  in_port 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(16), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_GROUP_PREFIX_LEN_ID,
    "group_prefix_len",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_lpm_get in_vid_mask 0\r\n"
    "  in_vid 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(16), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_GROUP_IP_ADDRESS_ID,
    "group_ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_lpm_get in_vid_mask 0\r\n"
    "  in_vid 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(16), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_SOURCE_PREFIX_LEN_ID,
    "src_prefix_len",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.source.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_lpm_get in_vid_mask 0\r\n"
    "  in_vid 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(16), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_LPM_GET_IPV4_MC_ROUTE_LPM_GET_ROUTE_KEY_SOURCE_IP_ADDRESS_ID,
    "src_ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.source.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_lpm_get in_vid_mask 0\r\n"
    "  in_vid 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(16), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_GET_BLOCK_IPV4_MC_ROUTE_GET_BLOCK_ID,
    "get_block",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get block of entries from route table that match the keys between start key\r\n"
    "  and last key.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_get_block block_range 0\r\n"
#endif
    "",
    "",
    {0, BIT(17), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_GET_BLOCK_IPV4_MC_ROUTE_GET_BLOCK_BLOCK_RANGE_ID,
    "show_fec_info",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  block_range:\r\n"
    "  block range.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_get_block show_fec_info 0\r\n"
#endif
    "",
    "",
    {0, BIT(17), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ID,
    "remove",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove the association of the IP-address to Unicast FEC.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc ipv4_mc_route_remove in_vid_mask 0 in_vid 0\r\n"
    "  src_ip_address 0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0\r\n"
#endif
    "",
    "",
    {0, BIT(18), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_IN_VID_VALID_ID,
    "in_vid_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid_mask:\r\n"
    "  Is the in_vid valid.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "  app_petra_pp ipv4_mc ipv4_mc_route_remove in_vid_mask 0 in_vid 0\r\n"
    "  src_ip_address 0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0\r\n"
#endif
    "",
    "",
    {0, BIT(18), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_IN_VID_ID,
    "in_vid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid:\r\n"
    "  The vid of the incoming packets.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "  app_petra_pp ipv4_mc ipv4_mc_route_remove in_vid_mask 0 in_vid 0\r\n"
    "  src_ip_address 0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0\r\n"
#endif
    "",
    "",
    {0, BIT(18), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_IN_PORT_VALID_ID,
    "in_port_valid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port_valid:\r\n"
    "  Is the in_port valid.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "  app_petra_pp ipv4_mc ipv4_mc_route_remove in_vid_mask 0 in_vid 0\r\n"
    "  src_ip_address 0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0\r\n"
#endif
    "",
    "",
    {0, BIT(18), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_IN_PORT_ID,
    "in_port",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port:\r\n"
    "  The port of the incoming packets.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "  app_petra_pp ipv4_mc ipv4_mc_route_remove in_vid_mask 0 in_vid 0\r\n"
    "  src_ip_address 0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0\r\n"
#endif
    "",
    "",
    {0, BIT(18), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_GROUP_PREFIX_LEN_ID,
    "group_prefix_len",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "  app_petra_pp ipv4_mc ipv4_mc_route_remove in_vid_mask 0 in_vid 0\r\n"
    "  src_ip_address 0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0\r\n"
#endif
    "",
    "",
    {0, BIT(18), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_GROUP_IP_ADDRESS_ID,
    "group_ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "  app_petra_pp ipv4_mc ipv4_mc_route_remove in_vid_mask 0 in_vid 0\r\n"
    "  src_ip_address 0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0\r\n"
#endif
    "",
    "",
    {0, BIT(18), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_SOURCE_PREFIX_LEN_ID,
    "src_prefix_len",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.source.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "  app_petra_pp ipv4_mc ipv4_mc_route_remove in_vid_mask 0 in_vid 0\r\n"
    "  src_ip_address 0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0\r\n"
#endif
    "",
    "",
    {0, BIT(18), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ROUTE_KEY_SOURCE_IP_ADDRESS_ID,
    "src_ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ip_address_vals[0],
    (sizeof(soc_petra_pp_api_ip_address_vals) / sizeof(soc_petra_pp_api_ip_address_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.source.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "  app_petra_pp ipv4_mc ipv4_mc_route_remove in_vid_mask 0 in_vid 0\r\n"
    "  src_ip_address 0 src_prefix_len 0 group_ip_address 0 group_prefix_len 0\r\n"
#endif
    "",
    "",
    {0, BIT(18), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ROUTE_REMOVE_IPV4_MC_ROUTE_REMOVE_ALL_ID,
    "clear_all",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Clear all IPv4 MC table. default will point to FEC 0\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv4_mc clear_all \r\n"
#endif
    "",
    "",
    {0, BIT(12), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV4_MC_ID,
    "ipv4_mc",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(12)|BIT(13)|BIT(14)|BIT(15)|BIT(16)|BIT(17)|BIT(18)},
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
    PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_INIT_ID,
    "init",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  INIT ipv6 module\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 init use_mapped_port \r\n"
#endif
    "",
    "",
    {0, BIT(19), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_INIT_USE_PORT_ID,
    "use_port",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  IF presents use dest IP x in port \r\n"
    "  IF does NOT present use dest IP x in VLAN ID for routing \r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 init use_mapped_port \r\n"
#endif
    "",
    "",
    {0, BIT(19), 0, 0, 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ID,
    "add",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Binds between an ipv6 key (Src-Ip/prefix and group) and a  FEC\r\n"
    "  identifier.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_add in_vid_mask 0 in_vid 0 dest_ip_address\r\n"
    "  0 prefix_len 0 ip_address 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {0, BIT(20), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_IN_VID_VALID_ID,
    "in_vid_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid_mask:\r\n"
    "  Is the in_vid valid.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_add in_vid_mask 0 in_vid 0 dest_ip_address\r\n"
    "  0 prefix_len 0 ip_address 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {0, BIT(20), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_IN_VID_ID,
    "in_vid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid:\r\n"
    "  The vid of the incoming packets.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_add in_vid_mask 0 in_vid 0 dest_ip_address\r\n"
    "  0 prefix_len 0 ip_address 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {0, BIT(20), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_IN_PORT_VALID_ID,
    "in_port_valid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port_valid:\r\n"
    "  Is the in_port valid.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_add in_port_valid 0 in_port\r\n"
    "  0 prefix_len 0 ip_address 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {0, BIT(20), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_IN_PORT_ID,
    "in_port",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port:\r\n"
    "  The port of the incoming packets.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_add in_port_valid 0 in_port\r\n"
    "  0 prefix_len 0 ip_address 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {0, BIT(20), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_GROUP_PREFIX_LEN_ID,
    "dest_prefix_len",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_add in_vid_mask 0 in_vid 0 dest_ip_address\r\n"
    "  0 prefix_len 0 ip_address 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {0, BIT(20), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_ROUTE_KEY_GROUP_IP_ADDRESS_ID,
    "dest_ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ipv6_addr_vals[0],
    (sizeof(soc_petra_pp_api_ipv6_addr_vals) / sizeof(soc_petra_pp_api_ipv6_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.ip_address:\r\n"
    "  IPV6 destination address.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_add in_vid_mask 0 in_vid 0 dest_ip_address\r\n"
    "  0 prefix_len 0 ip_address 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {0, BIT(20), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_SYS_FEC_NDX_ID,
    "sys_fec_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_ndx:\r\n"
    "  system FEC id. Range 0- 4G\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_add in_vid_mask 0 in_vid 0 dest_ip_address\r\n"
    "  0 prefix_len 0 ip_address 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {0, BIT(20), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_ADD_IPV6_ROUTE_ADD_OVERRIDE_ID,
    "override",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  override:\r\n"
    "  whether to override the entry if it already exists, if this is\r\n"
    "  FALSE and the entry exist error is returned.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_add in_vid_mask 0 in_vid 0 dest_ip_address\r\n"
    "  0 prefix_len 0 ip_address 0 sys_fec_ndx 0 override 0\r\n"
#endif
    "",
    "",
    {0, BIT(20), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ID,
    "get",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Gets the system-FEC-id associated the given key.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_get in_vid_mask 0 in_vid\r\n"
    "  0 prefix_len 0 dest_ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(21), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_IN_VID_VALID_ID,
    "in_vid_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid_mask:\r\n"
    "  Is the in_vid valid.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_get in_vid_mask 0 in_vid\r\n"
    "  0 prefix_len 0 dest_ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(21), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_IN_VID_ID,
    "in_vid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid:\r\n"
    "  The vid of the incoming packets.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_get in_vid_mask 0 in_vid\r\n"
    "  0 prefix_len 0 dest_ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(21), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_IN_PORT_VALID_ID,
    "in_port_valid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port_valid:\r\n"
    "  Is the in_port valid.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_get in_port_valid 0 in_port\r\n"
    "  0 prefix_len 0 dest_ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(21), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_IN_PORT_ID,
    "in_port",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port:\r\n"
    "  The port of the incoming packets.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_get in_port_valid 0 in_port\r\n"
    "  0 prefix_len 0 dest_ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(21), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_GROUP_PREFIX_LEN_ID,
    "dest_prefix_len",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_get in_vid_mask 0 in_vid\r\n"
    "  0 prefix_len 0 dest_ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(21), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_GET_IPV6_ROUTE_GET_ROUTE_KEY_GROUP_IP_ADDRESS_ID,
    "dest_ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ipv6_addr_vals[0],
    (sizeof(soc_petra_pp_api_ipv6_addr_vals) / sizeof(soc_petra_pp_api_ipv6_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.ip_address:\r\n"
    "  IPV6 destination address.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_get in_vid_mask 0 in_vid\r\n"
    "  0 prefix_len 0 dest_ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(21), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_GSA_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ID,
    "lpm_get",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get from the route table a route key with the longest prefix that matches the\r\n"
    "  given key.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_lpm_get in_vid_mask 0\r\n"
    "  in_vid 0 dest_ip_address 0 prefix_len 0\r\n"
#endif
    "",
    "",
    {0, BIT(22), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_IN_VID_VALID_ID,
    "in_vid_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid_mask:\r\n"
    "  Is the in_vid valid.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_lpm_get in_vid_mask 0\r\n"
    "  in_vid 0 dest_ip_address 0 prefix_len 0\r\n"
#endif
    "",
    "",
    {0, BIT(22), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_IN_VID_ID,
    "in_vid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid:\r\n"
    "  The vid of the incoming packets.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_lpm_get in_vid_mask 0\r\n"
    "  in_vid 0 dest_ip_address 0 prefix_len 0\r\n"
#endif
    "",
    "",
    {0, BIT(22), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_GROUP_PREFIX_LEN_ID,
    "prefix_len",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_lpm_get in_vid_mask 0\r\n"
    "  in_vid 0 dest_ip_address 0 prefix_len 0\r\n"
#endif
    "",
    "",
    {0, BIT(22), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_GROUP_IP_ADDRESS_ID,
    "dest_ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ipv6_addr_vals[0],
    (sizeof(soc_petra_pp_api_ipv6_addr_vals) / sizeof(soc_petra_pp_api_ipv6_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.ip_address:\r\n"
    "  IPV6 destination address.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_lpm_get in_vid_mask 0\r\n"
    "  in_vid 0 dest_ip_address 0 prefix_len 0\r\n"
#endif
    "",
    "",
    {0, BIT(22), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_IN_PORT_VALID_ID,
    "in_port_valid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port_valid:\r\n"
    "  Is the in_port valid.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 lpm_get in_port_valid 0\r\n"
    "  in_port 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(22), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_LPM_GET_IPV6_ROUTE_LPM_GET_ROUTE_KEY_IN_PORT_ID,
    "in_port",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port:\r\n"
    "  The port of the incoming packets.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 lpm_get in_port_valid 0\r\n"
    "  in_port 0 prefix_len 0 ip_address 0 prefix_len 0 ip_address 0\r\n"
#endif
    "",
    "",
    {0, BIT(22), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_GSA_IPV6_ROUTE_GET_BLOCK_IPV6_ROUTE_GET_BLOCK_ID,
    "get_block",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get block of entries from route table that match the keys between start key\r\n"
    "  and last key.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_get_block start_from 0\r\n"
#endif
    "",
    "",
    {0, BIT(23), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_GSA_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ID,
    "remove",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove the association of the IP-address to Unicast FEC.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_remove in_vid_mask 0\r\n"
    "  in_vid 0 dest_ip_address 0 prefix_len 0\r\n"
#endif
    "",
    "",
    {0, BIT(24), 0, 0, 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_IN_VID_VALID_ID,
    "in_vid_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid_mask:\r\n"
    "  Is the in_vid valid.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_remove in_vid_mask 0\r\n"
    "  in_vid 0 dest_ip_address 0 prefix_len 0\r\n"
#endif
    "",
    "",
    {0, BIT(24), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_IN_VID_ID,
    "in_vid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid:\r\n"
    "  The vid of the incoming packets.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_remove in_vid_mask 0\r\n"
    "  in_vid 0 dest_ip_address 0 prefix_len 0\r\n"
#endif
    "",
    "",
    {0, BIT(24), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_IN_PORT_VALID_ID,
    "in_port_valid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_port_valid:\r\n"
    "  Is the in port valid.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_remove in_vid_mask 0\r\n"
    "  in_vid 0 dest_ip_address 0 prefix_len 0\r\n"
#endif
    "",
    "",
    {0, BIT(24), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_IN_PORT_ID,
    "in_port",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.in_vid:\r\n"
    "  The vid of the incoming packets.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_remove in_vid_mask 0\r\n"
    "  in_vid 0 dest_ip_address 0 prefix_len 0\r\n"
#endif
    "",
    "",
    {0, BIT(24), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_GROUP_PREFIX_LEN_ID,
    "prefix_len",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_remove in_vid_mask 0\r\n"
    "  in_vid 0 dest_ip_address 0 prefix_len 0\r\n"
#endif
    "",
    "",
    {0, BIT(24), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_IPV6_ROUTE_REMOVE_IPV6_ROUTE_REMOVE_ROUTE_KEY_GROUP_IP_ADDRESS_ID,
    "dest_ip_address",
    (PARAM_VAL_RULES *)&soc_petra_pp_api_ipv6_addr_vals[0],
    (sizeof(soc_petra_pp_api_ipv6_addr_vals) / sizeof(soc_petra_pp_api_ipv6_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  route_key.group.ip_address:\r\n"
    "  IPV6 destination address.\r\n"
    "",
#ifdef UI_GSA_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  app_petra_pp ipv6 ipv6_route_remove in_vid_mask 0\r\n"
    "  in_vid 0 dest_ip_address 0 prefix_len 0\r\n"
#endif
    "",
    "",
    {0, BIT(24), 0, 0, 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_API_IPV6_ID,
    "ipv6",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(19)|BIT(20)|BIT(21)|BIT(22)|BIT(23)|BIT(24)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } ipv6*/
#ifdef UI_ACL/* { acl*/
  {
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ID,
    "add",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACL_NDX_ID,
    "acl_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_L2_ID,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_L3A_ID,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_L3B_ID,
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
/*
  {
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_FIELD_TO_INC_ID,
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
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_COUNT_ID,
    "range_count",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.count:\r\n"
    "  Number of IP addresses to add.\r\n"
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(2)|BIT(7)|BIT(8)},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_STEP_ID,
    "range_step",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_INC_SYS_FEC_ID_ID,
    "range_inc_sys_fec_id",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_mac:\r\n"
    "  Whether to increment MAC by 1 for each ACE.\r\n"
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(2)|BIT(7)|BIT(8)},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_INC_VID_ID,
    "range_inc_vid",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_mac:\r\n"
    "  Whether to increment MAC by 1 for each ACE.\r\n"
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(2)|BIT(7)|BIT(8)},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
*/
  {
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_VALID_ID,
    "l4_port_range_valid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MAX_DEST_PORT_ID,
    "l4_max_dest_port",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MIN_DEST_PORT_ID,
    "l4_min_dest_port",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MAX_SRC_PORT_ID,
    "l4_max_src_port",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MIN_SRC_PORT_ID,
    "l4_min_src_port",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_DEST_PORT_MASK_ID,
    "l4_dest_port_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_DEST_PORT_VAL_ID,
    "l4_dest_port_val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_SRC_PORT_MASK_ID,
    "l4_src_port_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_SRC_PORT_VAL_ID,
    "l4_src_port_val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_TCP_CTRL_OPS_MASK_ID,
    "l4_tcp_ctrl_ops_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_TCP_CTRL_OPS_VAL_ID,
    "l4_tcp_ctrl_ops_val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_DEST_IP_PREFIX_LEN_ID,
    "l3_dest_ip_prefix",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_DEST_IP_IP_ADDRESS_ID,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_SRC_IP_PREFIX_LEN_ID,
    "l3_src_ip_prefix",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_SRC_IP_IP_ADDRESS_ID,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_PROTOCOL_CODE_MASK_ID,
    "l3_prtcl_code_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_PROTOCOL_CODE_VAL_ID,
    "l3_prtcl_code_val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_TOS_MASK_ID,
    "l3_tos_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_TOS_VAL_ID,
    "l3_tos_val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_VID_MASK_ID,
    "vid_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_VID_VAL_ID,
    "vid_val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_LOCAL_PORT_ID_MASK_ID,
    "local_port_valid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_LOCAL_PORT_ID_VAL_ID,
    "local_port_val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_DEST_MAC_MASK_INDEX_ID,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_DEST_MAC_ID,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_SRC_MAC_MASK_ID,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_SRC_MAC_ID,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_ETHER_TYPE_MASK_ID,
    "ether_type_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_ETHER_TYPE_DSAP_ID,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_ETHER_TYPE_VAL_ID,
    "ether_type_val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_DEI_MASK_ID,
    "s_dei_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_DEI_VAL_ID,
    "s_dei_val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_PCP_MASK_ID,
    "s_pcp_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_PCP_VAL_ID,
    "s_pcp_val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_VID_MASK_ID,
    "s_vid_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_VID_VAL_ID,
    "s_vid_val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_TAG_EXIST_MASK_ID,
    "s_exist_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_S_TAG_TAG_EXIST_VAL_ID,
    "s_tag_exist_val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_PCP_MASK_ID,
    "c_tag_up_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_PCP_VAL_ID,
    "c_tag_up_val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_VID_MASK_ID,
    "c_vid_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_VID_VAL_ID,
    "c_vid_val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_TAG_EXIST_MASK_ID,
    "c_tag_exit_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_C_TAG_TAG_EXIST_VAL_ID,
    "c_tag_exit_val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_IN_LOCAL_PORT_ID_MASK_ID,
    "in_port_valid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_IN_LOCAL_PORT_ID_VAL_ID,
    "local_port_val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_IN_PORT_TYPE_VALID_ID,
    "in_port_type_valid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_VAL_L2_INFO_IN_PORT_TYPE_ID,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_PRIORITY_ID,
    "priority",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACE_ID_ID,
    "ace_id_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_COS_ACTION_ID,
    "action_cos",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_SYS_FEC_ID_ID,
    "action_sys_fec_id",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_INFO_DESTINATION_DEST_VAL_ID,
    "action_dest_val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_INFO_DESTINATION_DEST_TYPE_ID,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_L3_INFO_VID_ID,
    "action_vid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_ADD_ACE_ADD_OVERRIDE_ID,
    "override",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
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
    PARAM_GSA_ACL_ACE_REMOVE_ACE_REMOVE_ID,
    "remove",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove ACE\r\n"
    "  from ACL\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl remove acl_ndx 0 ace_id_ndx 0\r\n"
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
    PARAM_GSA_ACL_ACE_REMOVE_ACE_REMOVE_ACL_NDX_ID,
    "acl_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acl_ndx:\r\n"
    "  ACL ID\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl remove acl_ndx 0 ace_id_ndx 0\r\n"
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
    PARAM_GSA_ACL_ACE_REMOVE_ACE_REMOVE_ACE_ID_NDX_ID,
    "ace_id_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace_id_ndx:\r\n"
    "  ACE ID\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl remove acl_ndx 0 ace_id_ndx 0\r\n"
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
    PARAM_GSA_ACL_ACE_GET_ACE_GET_ID,
    "get",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get Binding between ACE and a action. As a result of this operation packets\r\n"
    "  match the ACE will be process according to the given action.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl get acl_ndx 0 ace_id_ndx 0\r\n"
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
    PARAM_GSA_ACL_ACE_GET_ACE_GET_ACL_NDX_ID,
    "acl_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acl_ndx:\r\n"
    "  ACL id.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl get acl_ndx 0 ace_id_ndx 0\r\n"
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
    PARAM_GSA_ACL_ACE_GET_ACE_GET_ACE_ID_NDX_ID,
    "ace_id_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace_id_ndx:\r\n"
    "  ace ID.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl get acl_ndx 0 ace_id_ndx 0\r\n"
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
    PARAM_GSA_ACL_ACE_GET_BLOCK_ACE_GET_BLOCK_ID,
    "get_block",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get block of entries from the routing table in a given range.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl get_block acl_ndx 0\r\n"
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
    PARAM_GSA_ACL_ACE_GET_BLOCK_ACE_GET_BLOCK_ACL_NDX_ID,
    "acl_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acl_ndx:\r\n"
    "  ACL id.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl get_block acl_ndx 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ID,
    "range_add",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Binds between ace and actoions.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACL_NDX_ID,
    "acl_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acl_ndx:\r\n"
    "  ACL ID\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_VALID_ID,
    "port_range_valid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.port_range_valid:\r\n"
    "  If TRUE then port_range considered as part of the ACE. Otherwise it masked\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MAX_DEST_PORT_ID,
    "max_dest_port",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.port_range.max_dest_port:\r\n"
    "  The maximum Destination port in the range. Size: 16 bits. Range 0-65535.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MIN_DEST_PORT_ID,
    "min_dest_port",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.port_range.min_dest_port:\r\n"
    "  The minimum Destination port in the range. Size: 16 bits. Range 0-65535.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MAX_SRC_PORT_ID,
    "max_src_port",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.port_range.max_src_port:\r\n"
    "  The maximum Source port in the range. Size: 16 bits. Range 0-65535.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_PORT_RANGE_MIN_SRC_PORT_ID,
    "min_src_port",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.port_range.min_src_port:\r\n"
    "  The minimum Source port in the range. Size: 16 bits. Range 0-65535.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_DEST_PORT_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.dest_port.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFF to consider the exact\r\n"
    "  value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_DEST_PORT_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.dest_port.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_SRC_PORT_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.src_port.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFF to consider the exact\r\n"
    "  value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_SRC_PORT_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.src_port.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_TCP_CTRL_OPS_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.tcp_ctrl_ops.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L4_INFO_TCP_CTRL_OPS_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l4_info.tcp_ctrl_ops.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_DEST_IP_PREFIX_LEN_ID,
    "prefix_len",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.dest_ip.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_DEST_IP_IP_ADDRESS_ID,
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
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_SRC_IP_PREFIX_LEN_ID,
    "prefix_len",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.src_ip.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_SRC_IP_IP_ADDRESS_ID,
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
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_PROTOCOL_CODE_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.protocol_code.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_PROTOCOL_CODE_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.protocol_code.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_TOS_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.tos.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_TOS_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.tos.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_VID_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.in_vid.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFFFFFF to consider the\r\n"
    "  exact value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_VID_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.in_vid.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_LOCAL_PORT_ID_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.in_local_port_id.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFFFFFF to consider the\r\n"
    "  exact value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L3_L4_INFO_L3_INFO_IN_LOCAL_PORT_ID_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l3_l4_info.l3_info.in_local_port_id.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_DEST_MAC_MASK_INDEX_ID,
    "dest_mac_mask_index",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  dest_mac_mask_index:\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_DEST_MAC_MASK_ID,
    "dest_mac_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.dest_mac_mask[ prm_dest_mac_mask_index]:\r\n"
    "  Mask on the Destination MAC address. 1 to consider the corresponding bit, and\r\n"
    "  zero to ignore it.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_DEST_MAC_ID,
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
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_SRC_MAC_MASK_INDEX_ID,
    "src_mac_mask_index",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  src_mac_mask_index:\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_SRC_MAC_MASK_ID,
    "src_mac_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.src_mac_mask[ prm_src_mac_mask_index]:\r\n"
    "  Mask on the source MAC address. 1 to consider the corresponding bit, and zero\r\n"
    "  to ignore it.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_SRC_MAC_ID,
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
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_ETHER_TYPE_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.ether_type.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFF to consider the exact\r\n"
    "  value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_ETHER_TYPE_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.ether_type.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_DEI_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.s_tag.Dei.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_DEI_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.s_tag.Dei.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_PCP_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.s_tag.Pcp.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_PCP_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.s_tag.Pcp.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_VID_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.s_tag.Vid.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFFFFFF to consider the\r\n"
    "  exact value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_VID_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.s_tag.Vid.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_TAG_EXIST_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.s_tag.tag_exist.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_S_TAG_TAG_EXIST_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.s_tag.tag_exist.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_DEI_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.c_tag.Dei.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_DEI_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.c_tag.Dei.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_PCP_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.c_tag.Pcp.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_PCP_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.c_tag.Pcp.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_VID_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.c_tag.Vid.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFFFFFF to consider the\r\n"
    "  exact value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_VID_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.c_tag.Vid.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_TAG_EXIST_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.c_tag.tag_exist.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFF to consider the exact\r\n"
    "  value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_C_TAG_TAG_EXIST_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.c_tag.tag_exist.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_IN_LOCAL_PORT_ID_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.in_local_port_id.mask:\r\n"
    "  Mask on the value. 1 to consider the corresponding bit, and zero to ignore\r\n"
    "  it. Set mask to zero to mask all value, and to 0xFFFFFFFF to consider the\r\n"
    "  exact value\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_IN_LOCAL_PORT_ID_VAL_ID,
    "Val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.in_local_port_id.Val:\r\n"
    "  Value.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_IN_PORT_TYPE_VALID_ID,
    "in_port_type_valid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.val.l2_info.in_port_type_valid:\r\n"
    "  Is in_port_type valid, or it's masked.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_VAL_L2_INFO_IN_PORT_TYPE_ID,
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
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_TYPE_ID,
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
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_PRIORITY_ID,
    "priority",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.priority:\r\n"
    "  The priority of the ACE to add. The smaller the priority value, the higher\r\n"
    "  will be the priority of the ACE. \r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACE_ID_ID,
    "id",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.id:\r\n"
    "  The ID of the ACE, should be unique for each ACE.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_INC_VID_ID,
    "inc_vid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_vid:\r\n"
    "  Whether to increment VID by 1 for each ACE.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_INC_SYS_FEC_ID_ID,
    "inc_sys_fec_id",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.inc_mac:\r\n"
    "  Whether to increment MAC by 1 for each ACE.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_STEP_ID,
    "step",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.step:\r\n"
    "  Number of step between to consecutive IP addresses. Relevant only for\r\n"
    "  increment and decrement.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_COUNT_ID,
    "count",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.count:\r\n"
    "  Number of IP addresses to add.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_TO_ID,
    "to",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.to:\r\n"
    "  IP address, range ends at this IP address. Relevant only for random. For\r\n"
    "  increment/decrement types the end of the range is determined according to the\r\n"
    "  count and step.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_FIELD_TO_INC_ID,
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
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_RANGE_INFO_TYPE_ID,
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
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACTION_ACTION_MASK_ID,
    "action_mask",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action.action_mask:\r\n"
    "  Mask over the field to be considered in the action. FEC pointer always set in\r\n"
    "  the mask.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACTION_COS_ACTION_ID,
    "cos_action",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action.cos_action:\r\n"
    "  Pointer to a CoS Action.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACTION_L3_INFO_SYS_FEC_ID_ID,
    "sys_fec_id",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.count:\r\n"
    "  Number of IP addresses to add.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACTION_L3_INFO_VID_ID,
    "vid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action.l3_info.vid:\r\n"
    "  The VLAN id, to set to the routed packets. Relevant only if destination is\r\n"
    "  system port.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACTION_L3_INFO_DESTINATION_DEST_VAL_ID,
    "dest_val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action.l3_info.destination.dest_val:\r\n"
    "  The destination value, according to the type (Single, RLAG or GLAG,LIF...)For\r\n"
    "  LAG the value is the group ID.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACTION_L3_INFO_DESTINATION_DEST_TYPE_ID,
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
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACTION_L2_INFO_MAC_ID,
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
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACTION_L2_INFO_VID_ID,
    "vid",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action.l2_info.vid:\r\n"
    "  The VLAN id, to set to the aced packets. Relevant only if destination is\r\n"
    "  system port.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACTION_L2_INFO_DESTINATION_DEST_VAL_ID,
    "dest_val",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action.l2_info.destination.dest_val:\r\n"
    "  The destination value, according to the type (Single, RLAG or GLAG,LIF...)For\r\n"
    "  LAG the value is the group ID.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACTION_L2_INFO_DESTINATION_DEST_TYPE_ID,
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
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_ADD_RANGE_ADD_ACTION_UPDATE_TYPE_ID,
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
    "  gsa_api acl range_add acl_ndx 0 port_range_valid 0 max_dest_port 0\r\n"
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
    PARAM_GSA_ACL_RANGE_REMOVE_RANGE_REMOVE_ID,
    "range_remove",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove Range of ACEs there Ids.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_remove acl_ndx 0 from_ace_ndx 0 to_ace_ndx 0\r\n"
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
    PARAM_GSA_ACL_RANGE_REMOVE_RANGE_REMOVE_ACL_NDX_ID,
    "acl_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acl_ndx:\r\n"
    "  ACL id.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_remove acl_ndx 0 from_ace_ndx 0 to_ace_ndx 0\r\n"
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
    PARAM_GSA_ACL_RANGE_REMOVE_RANGE_REMOVE_FROM_ACE_NDX_ID,
    "from_ace_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  from_ace_ndx:\r\n"
    "  from this ACE.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_remove acl_ndx 0 from_ace_ndx 0 to_ace_ndx 0\r\n"
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
    PARAM_GSA_ACL_RANGE_REMOVE_RANGE_REMOVE_TO_ACE_NDX_ID,
    "to_ace_ndx",
    (PARAM_VAL_RULES *)&gsa_api_free_vals[0],
    (sizeof(gsa_api_free_vals) / sizeof(gsa_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  to_ace_ndx:\r\n"
    "  remove till this ace.\r\n"
    "",
#ifdef UI_SWP_PP_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  gsa_api acl range_remove acl_ndx 0 from_ace_ndx 0 to_ace_ndx 0\r\n"
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
    PARAM_GSA_API_ACL_ID,
    "acl",
    (PARAM_VAL_RULES *)&gsa_api_empty_vals[0],
    (sizeof(gsa_api_empty_vals) / sizeof(gsa_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(8)|BIT(9)|BIT(10)|BIT(14)|BIT(15)|BIT(16)|/*|BIT(17)|*/BIT(18)|BIT(19)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } acl*/
#endif /* PETRA_PP_MINIMAL */
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

/* } __UI_ROM_DEFI_GSA_API_INCLUDED__*/
#endif
