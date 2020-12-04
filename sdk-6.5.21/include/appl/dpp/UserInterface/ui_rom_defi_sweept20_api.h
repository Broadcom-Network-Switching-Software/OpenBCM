/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#if LINK_TIMNA_LIBRARIES

#ifndef __UI_SWEEPT20_API_ROM_DEFI_SWEEPT20_API_INCLUDED__

#define __UI_SWEEPT20_API_ROM_DEFI_SWEEPT20_API_INCLUDED__

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

#include <appl/dpp/UserInterface/ui_pure_defi_sweept20_api.h>
#include <Sweep/Timna/sweept20_api_policer.h>
#include <Sweep/Timna/sweept20_api_acl.h>
#include <Sweep/Timna/sweept20_api_vsi.h>

#include <DuneApp/timna_app_mpls_te.h>

  /*
  * ENUM RULES
  * {
  */


EXTERN CONST
   PARAM_VAL_RULES
     Timna_ports_list[]
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
        SWEEPT20_VSI_MAX_PORTS_IN_VSI,
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
     MPLS_TE_SP_LEVEL_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "sp_1",
    {
      {
        MPLS_TE_SP_LEVEL_1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  Strict Priority Level 1 - Highest\r\n"
        "  \r\n"
        "",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "sp_2",
    {
      {
        MPLS_TE_SP_LEVEL_2,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  Strict Priority Level 2\r\n"
        "  \r\n"
        "",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "sp_3_with_weight",
    {
      {
        MPLS_TE_SP_LEVEL_3_WFQ,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  Strict Priority Level 3\r\n"
        "  User may also add weight, to differentiate between other EEPs \r\n"
        "  on the same strict priority level",
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "sp_4_with_weight",
    {
      {
        MPLS_TE_SP_LEVEL_4_WFQ,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  Strict Priority Level 4\r\n"
        "  User may also add weight, to differentiate between other EEPs \r\n"
        "  on the same strict priority level",
        (long)NULL
      }
    }
  },

  {
    VAL_SYMBOL,
    "sp_5",
    {
      {
        MPLS_TE_SP_LEVEL_5,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  Strict Priority Level 1 - Lowest\r\n"
        "  \r\n"
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


EXTERN CONST
   PARAM_VAL_RULES
     SWEEPT20_POLICE_OP_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "port_only",
    {
      {
        SWEEPT20_POLICE_OP_PORT_ONLY,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWEEPT20_POLICE_OP.SWEEPT20_POLICE_OP_PORT_ONLY:\r\n"
        "  Policer according to port only. \r\n"
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
    "in_vc_id",
    {
      {
        SWEEPT20_POLICE_OP_IN_VC_ID,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWEEPT20_POLICE_OP.SWEEPT20_POLICE_OP_IN_VC_ID:\r\n"
        "  VPLS Provider Packets are policed according to incoming VC label ID it\r\n"
        "  arrived with. \r\n"
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
    "vsid_ports",
    {
      {
        SWEEPT20_POLICE_OP_VSID_ALL_MEMBERS_ACS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWEEPT20_POLICE_OP.SWEEPT20_POLICE_OP_VSID_ALL_MEMBERS_PORTS:\r\n"
        "  Policing all Attachment Circuit of a VSID\r\n"
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
    "vsid_vcs",
    {
      {
        SWEEPT20_POLICE_OP_VSID_ALL_MEMBERS_VCS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWEEPT20_POLICE_OP.SWEEPT20_POLICE_OP_VSID_ALL_MEMBERS_VCS:\r\n"
        "  Policing all member VCs of a VSID\r\n"
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
    "s_vid_and_port",
    {
      {
        SWEEPT20_POLICE_OP_S_VID_AND_PORT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWEEPT20_POLICE_OP.SWEEPT20_POLICE_OP_S_VID_AND_PORT:\r\n"
        "  Customer Packets are policed according to incoming S-VID it arrived with, *\r\n"
        "  port ID\r\n"
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
    "c_vid_and_port",
    {
      {
        SWEEPT20_POLICE_OP_C_VID_AND_PORT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWEEPT20_POLICE_OP.SWEEPT20_POLICE_OP_C_VID_AND_PORT:\r\n"
        "  Customer Packets are policed according to incoming C-VID it arrived with, *\r\n"
        "  port ID\r\n"
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
    "s_vid",
    {
      {
        SWEEPT20_POLICE_OP_S_VID,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWEEPT20_POLICE_OP.SWEEPT20_POLICE_OP_S_VID:\r\n"
        "  Customer Packets are policed according to incoming S-VID it arrived with\r\n"
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
     SWEEPT20_ACL_KEY_TMPLT_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "mac",
    {
      {
        SWEEPT20_ACL_KEY_TMPLT_MAC,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWEEPT20_ACL_KEY_TMPLT.SWEEPT20_ACL_KEY_TMPLT_MAC:\r\n"
        "  The packets are examined according to destination MAC and / or source MAC\r\n"
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
    "5_tuple",
    {
      {
        SWEEPT20_ACL_KEY_TMPLT_5_TUPLE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWEEPT20_ACL_KEY_TMPLT.SWEEPT20_ACL_KEY_TMPLT_5_TUPLE:\r\n"
        "  The packets are examined according to the 5-Tuple [Src IP, Dst IP, Src Port,\r\n"
        "  Dst Port, Protocol]\r\n"
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
    "8021ad",
    {
      {
        SWEEPT20_ACL_KEY_TMPLT_8021AD,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWEEPT20_ACL_KEY_TMPLT.SWEEPT20_ACL_KEY_TMPLT_8021AD:\r\n"
        "  The packets are examined according to the 8021AD attributes: S-VID + C-VLAN\r\n"
        "  Present + C-VID + EtherTypeCode + S-VLAN_PCP + C-VLAN_PCP.\r\n"
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
     SWEEPT20_ACL_PRINT_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "print_all",
    {
      {
        SWEEPT20_ACL_PRINT_ALL,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWEEPT20_ACL_PRINT_TYPE.SWEEPT20_ACL_PRINT_ALL:\r\n"
        "  Print all valid entries\r\n"
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
    "print_drop_act",
    {
      {
        SWEEPT20_ACL_PRINT_DROP_ACT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWEEPT20_ACL_PRINT_TYPE.SWEEPT20_ACL_PRINT_DROP_ACT:\r\n"
        "  Print entries that results in dropping\r\n"
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
    "per_policer",
    {
      {
        SWEEPT20_ACL_PER_POLICER,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWEEPT20_ACL_PRINT_TYPE.SWEEPT20_ACL_PER_POLICER:\r\n"
        "  Print entries that results in binding to policer with id_ndx\r\n"
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
    "per_vsid",
    {
      {
        SWEEPT20_ACL_PER_VSID,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SWEEPT20_ACL_PRINT_TYPE.SWEEPT20_ACL_PER_VSID:\r\n"
        "  Print entries that relevant to specified VSID id_ndx\r\n"
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


EXTERN CONST
   PARAM_VAL_RULES
     sweept20_api_free_vals[]
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
     sweept20_api_empty_vals[]
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
   PARAM
     sweept20_api_params[]
#ifdef INIT
   =
{
#ifdef UI_SWEEPT20_API_INIT
  {
    PARAM_SWEEPT20_API_GENERIC_INIT_GENERIC_INIT_ID,
    "generic_init",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Register the device, initialize the device interfaces and databases, and set\r\n"
    "  the device with default values\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init generic_init app_dvc_hndl 0 pp_id 0\r\n"
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
    PARAM_SWEEPT20_API_GENERIC_INIT_GENERIC_INIT_APP_DEVICE_HANDLE_ID,
    "app_dvc_hndl",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  app_device_handle:\r\n"
    "  The device handle as the user application's see it. When the\r\n"
    "  driver calls the user callbacks for device read/write commands,\r\n"
    "  this is the handle it uses\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init generic_init app_dvc_hndl 0 pp_id 0\r\n"
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
    PARAM_SWEEPT20_API_GENERIC_INIT_GENERIC_INIT_PP_ID_ID,
    "pp_id",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  pp_id:\r\n"
    "  The device system unique ID.\r\n"
    "  SOC_SAND_IN TIMNA_FEATURE_LIST_EN *feature_list -\r\n"
    "  List of the features supported by the Timna.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init generic_init app_dvc_hndl 0 pp_id 0\r\n"
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
    PARAM_SWEEPT20_API_STAND_ALONE_INIT_STAND_ALONE_INIT_ID,
    "stand_alone_init",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Should be called when the Timna works in stand-alone mode.\r\n"
    "  Load default demo settings:\r\n"
    "  IP Subnets list and protocol, for VID and CoS resolution\r\n"
    "  When the DA is not found, the packet is dropped.\r\n"
    "  Outgoing packets leaves the Timna Tagged.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init stand_alone_init\r\n"
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
    PARAM_SWEEPT20_API_RESET_FAP_ID,
    "reset_fap",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  HW FAP reset.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init reset_fap\r\n"
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
    PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_ID,
    "bridge_init",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the default settings required for simple 802.1Q application.\r\n"
    "  After calling this function, learning is enabled.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init bridge_init nof_vlns 0 dflt_mcs_prcnfg_rng_3_lst_svd 0\r\n"
    "  dflt_mcs_prcnfg_rng_3_frst_svd 0 dflt_mcs_prcnfg_rng_3_lst_mac_ddrss 0\r\n"
    "  dflt_mcs_prcnfg_rng_3_frst_mac_ddrss 0 dflt_mcs_prcnfg_rng_2_lst_svd 0\r\n"
    "  dflt_mcs_prcnfg_rng_2_frst_svd 0 dflt_mcs_prcnfg_rng_2_lst_mac_ddrss 0\r\n"
    "  dflt_mcs_prcnfg_rng_2_frst_mac_ddrss 0 dflt_mcs_prcnfg_rng_1_lst_svd 0\r\n"
    "  dflt_mcs_prcnfg_rng_1_frst_svd 0 dflt_mcs_prcnfg_rng_1_lst_mac_ddrss 0\r\n"
    "  dflt_mcs_prcnfg_rng_1_frst_mac_ddrss 0 dflt_mcs_prcnfg_rng_0_lst_svd 0\r\n"
    "  dflt_mcs_prcnfg_rng_0_frst_svd 0 dflt_mcs_prcnfg_rng_0_lst_mac_ddrss 0\r\n"
    "  dflt_mcs_prcnfg_rng_0_frst_mac_ddrss 0 dflt_mcs_nbl 0\r\n"
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
    PARAM_SWEEPT20_API_BRIDGE_INIT_BRIDGE_INIT_NOF_VLANS_ID,
    "nof_vlns",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  nof_vlans:\r\n"
    "  Randge: 0-4K.\r\n"
    "  The number of Vlans to be opened by default.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init bridge_init nof_vlns 0 dflt_mcs_prcnfg_rng_3_lst_svd 0\r\n"
    "  dflt_mcs_prcnfg_rng_3_frst_svd 0 dflt_mcs_prcnfg_rng_3_lst_mac_ddrss 0\r\n"
    "  dflt_mcs_prcnfg_rng_3_frst_mac_ddrss 0 dflt_mcs_prcnfg_rng_2_lst_svd 0\r\n"
    "  dflt_mcs_prcnfg_rng_2_frst_svd 0 dflt_mcs_prcnfg_rng_2_lst_mac_ddrss 0\r\n"
    "  dflt_mcs_prcnfg_rng_2_frst_mac_ddrss 0 dflt_mcs_prcnfg_rng_1_lst_svd 0\r\n"
    "  dflt_mcs_prcnfg_rng_1_frst_svd 0 dflt_mcs_prcnfg_rng_1_lst_mac_ddrss 0\r\n"
    "  dflt_mcs_prcnfg_rng_1_frst_mac_ddrss 0 dflt_mcs_prcnfg_rng_0_lst_svd 0\r\n"
    "  dflt_mcs_prcnfg_rng_0_frst_svd 0 dflt_mcs_prcnfg_rng_0_lst_mac_ddrss 0\r\n"
    "  dflt_mcs_prcnfg_rng_0_frst_mac_ddrss 0 dflt_mcs_nbl 0\r\n"
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
    PARAM_SWEEPT20_API_AD_PEB_INIT_AD_PEB_INIT_ID,
    "ad_peb_init",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the default settings required for 802.1AD application.\r\n"
    "  After calling this function, learning is enabled.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init ad_peb_init nof_svlns 0 nof_cvlns 0\r\n"
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
    PARAM_SWEEPT20_API_AD_PEB_INIT_AD_PEB_INIT_NOF_SVLANS_ID,
    "nof_svlns",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  nof_svlans:\r\n"
    "  Randge: 1-4094.\r\n"
    "  The number of S-Vlans to be opened by default.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init ad_peb_init nof_svlns 0 nof_cvlns 0\r\n"
    "",
    "",
    {BIT(3), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_AD_PEB_INIT_AD_PEB_INIT_NOF_CVLANS_ID,
    "nof_cvlns",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  nof_cvlans:\r\n"
    "  Randge: 1-4094.\r\n"
    "  The number of C-clans to be opened by default.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init ad_peb_init nof_svlns 0 nof_cvlns 0\r\n"
    "",
    "",
    {BIT(3), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VPLS_INIT_VPLS_INIT_ID,
    "vpls_init",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the default settings required for 802.1AD application.\r\n"
    "  After calling this function, learning is enabled.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init vpls_init nof_vss 0 nof_vcs_per_vsi 0 cstmr_plcr 0\r\n"
    "  lyr3_plcr 0\r\n"
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
    PARAM_SWEEPT20_API_VPLS_INIT_VPLS_INIT_NOF_VSIS_ID,
    "nof_vss",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  nof_vsis:\r\n"
    "  Range: 1 - 16K.\r\n"
    "  The number of Virtual switches to be opened by default. Each VSI\r\n"
    "  is selected according to VC label from the provider port and\r\n"
    "  S-VID x C-VID from the customer port.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init vpls_init nof_vss 0 nof_vcs_per_vsi 0 cstmr_plcr 0\r\n"
    "  lyr3_plcr 0\r\n"
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
    PARAM_SWEEPT20_API_VPLS_INIT_VPLS_INIT_NOF_VCS_PER_VSI_ID,
    "nof_vcs_per_vsi",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  nof_vcs_per_vsi:\r\n"
    "  Randge: 1-256K; but nof_vsis * nof_vcs_per_vsi <= 256K.\r\n"
    "  Each VC per VSID has different tunnel ID and ARP MAC, and S-VID x\r\n"
    "  C-VID attached to it.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init vpls_init nof_vss 0 nof_vcs_per_vsi 0 cstmr_plcr 0\r\n"
    "  lyr3_plcr 0\r\n"
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
    PARAM_SWEEPT20_API_VPLS_INIT_VPLS_INIT_CUSTOMER_POLICER_ID,
    "cstmr_plcr",
    (PARAM_VAL_RULES *)&SWEEPT20_POLICE_OP_rule[0],
    (sizeof(SWEEPT20_POLICE_OP_rule) / sizeof(SWEEPT20_POLICE_OP_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  customer_policer:\r\n"
    "  Policing method. According to VSID * port / VID * [C-VID] ...\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init vpls_init nof_vss 0 nof_vcs_per_vsi 0 cstmr_plcr 0\r\n"
    "  lyr3_plcr 0\r\n"
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
    PARAM_SWEEPT20_API_VPLS_INIT_VPLS_INIT_LAYER3_POLICER_ID,
    "lyr3_plcr",
    (PARAM_VAL_RULES *)&SWEEPT20_POLICE_OP_rule[0],
    (sizeof(SWEEPT20_POLICE_OP_rule) / sizeof(SWEEPT20_POLICE_OP_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  layer3_policer:\r\n"
    "  Policing method. According to VSID / VC Label ...\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init vpls_init nof_vss 0 nof_vcs_per_vsi 0 cstmr_plcr 0\r\n"
    "  lyr3_plcr 0\r\n"
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
    PARAM_SWEEPT20_API_MPLS_LSR_INIT_MPLS_LSR_INIT_ID,
    "mpls_lsr_init",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the default settings required for MPLS LSR application.\r\n"
    "  After calling this function, traffic is enabled on the predefined\r\n"
    "  labels\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init mpls_lsr_init nof_lsr_labels 0 nof_ncp_ptns 0\r\n"
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
    PARAM_SWEEPT20_API_MPLS_LSR_INIT_MPLS_LSR_INIT_NOF_LSR_LABELS_ID,
    "nof_lsr_labels",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  nof_lsr_labels:\r\n"
    "  Number of preconfigured LSR Label\r\n"
    "  Randge: 1-224K;\r\n"
    "  Opens labels 32K to (32K + nof_lsr_labels).\r\n"
    "  Odd labels are forwarded to port '0' and even labels to port '1'.\r\n"
    "  Egress Labels are the ingress label +256K\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init mpls_lsr_init nof_lsr_labels 0 nof_ncp_ptns 0\r\n"
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
    PARAM_SWEEPT20_API_MPLS_LSR_INIT_MPLS_LSR_INIT_NOF_ENCAP_OPTIONS_ID,
    "nof_ncp_ptns",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  nof_encap_options:\r\n"
    "  Number of preconfigured encapsulation options\r\n"
    "  Range: 1 - 224K.\r\n"
    "  Opens labels 32K to (32K + nof_encap_options).\r\n"
    "  Each label is attached to encapsulation action.\r\n"
    "  In-Label (X-32K) is attached to encapsulation action\r\n"
    "  (X/( nof_lsr_labels/ nof_encap_options))+ 32K.\r\n"
    "  By default, tunnels are not pushed above the swapped label\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init mpls_lsr_init nof_lsr_labels 0 nof_ncp_ptns 0\r\n"
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
    PARAM_SWEEPT20_API_IPV4_ROUTING_INIT_IPV4_ROUTING_INIT_ID,
    "ipv4_routing_init",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the default settings required for IPV4 routing application.\r\n"
    "  After calling this function, traffic is enabled on the predefined\r\n"
    "  labels\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init ipv4_routing_init nof_vrfs 0 nof_outlifs 0 table_size 0\r\n"
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
    PARAM_SWEEPT20_API_IPV4_ROUTING_INIT_IPV4_ROUTING_INIT_NOF_VRFS_ID,
    "nof_vrfs",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  nof_vrfs:\r\n"
    "  Number of VRFs \r\n"
    "  Randge: 1-4K;\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init ipv4_routing_init nof_vrfs 0 nof_outlifs 0 table_size 0\r\n"
    "",
    "",
    {BIT(9), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_ROUTING_INIT_IPV4_ROUTING_INIT_START_VRF_ID_ID,
    "nof_outlifs",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  nof_vrfs:\r\n"
    "  vrf to init from\r\n"
    "  Randge: 1-4K;\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init ipv4_routing_init nof_vrfs 0 nof_outlifs 0 table_size 0\r\n"
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
    PARAM_SWEEPT20_API_IPV4_ROUTING_INIT_IPV4_ROUTING_INIT_VRF_TABLE_SIZE_ID,
    "table_size",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  nof_vrfs:\r\n"
    "  vrf to init from\r\n"
    "  Randge: 1-4K;\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init ipv4_routing_init nof_vrfs 0 nof_outlifs 0 table_size 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_INIT_IPV4_ACL_INIT_ID,
    "acl_init",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Init the ACL application.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init acl_init \r\n"
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
    PARAM_SWEEPT20_API_ENABLE_TRAFFIC_ENABLE_TRAFFIC_ID,
    "enable_traffic",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Enable / Disable traffic. Functions from the initialization sequence might\r\n"
    "  only be committed when traffic is disabled.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init enable_traffic do_nbl 0\r\n"
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
    PARAM_SWEEPT20_API_ENABLE_TRAFFIC_ENABLE_TRAFFIC_DO_ENABLE_ID,
    "do_nbl",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  do_enable:\r\n"
    "  TRUE to enable traffic; FALSE to disable.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init enable_traffic do_nbl 0\r\n"
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
    PARAM_SWEEPT20_API_CLOSE_CLOSE_ID,
    "close",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Reset the device, driver and application:1.  Disable traffic2.  Reset the\r\n"
    "  device3.  Deregister the device4.  Clean driver and application databases and\r\n"
    "  state machines.\r\n"
    "  After the application close, the user might start the application\r\n"
    "  once again.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app init close\r\n"
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
    PARAM_SWEEPT20_API_INIT_ID,
    "init",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(9)|BIT(10), 0, 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif
#ifdef UI_SWEEPT20_API_PORTS
  {
    PARAM_SWEEPT20_API_PORTS_MAPPING_TO_SYS_PORTS_MAPPING_TO_SYS_PORTS_ID,
    "mapping_to_sys_ports",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Map system device and local port to system physical port in the SW\r\n"
    "  databases and in the devices.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app ports mapping_to_sys_ports sys_dev_ndx 0 lcl_port_ndx 0\r\n"
    "  systm_phy_port 0\r\n"
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
    PARAM_SWEEPT20_API_PORTS_MAPPING_TO_SYS_PORTS_MAPPING_TO_SYS_PORTS_SYS_DEV_NDX_ID,
    "sys_dev_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_dev_ndx:\r\n"
    "  Device unique ID in the system.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app ports mapping_to_sys_ports sys_dev_ndx 0 lcl_port_ndx 0\r\n"
    "  systm_phy_port 0\r\n"
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
    PARAM_SWEEPT20_API_PORTS_MAPPING_TO_SYS_PORTS_MAPPING_TO_SYS_PORTS_LOCAL_PORT_NDX_ID,
    "lcl_port_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  local_port_ndx:\r\n"
    "  The port local ID in 'sys_dev_ndx'\r\n"
    "",
    "Examples:\r\n"
    "  timna_app ports mapping_to_sys_ports sys_dev_ndx 0 lcl_port_ndx 0\r\n"
    "  systm_phy_port 0\r\n"
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
    PARAM_SWEEPT20_API_PORTS_MAPPING_TO_SYS_PORTS_MAPPING_TO_SYS_PORTS_SYSTEM_PHY_PORT_ID,
    "systm_phy_port",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  system_phy_port:\r\n"
    "  The System physical port, to be associated with the local port in\r\n"
    "  the 'sys_dev_ndx' device.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app ports mapping_to_sys_ports sys_dev_ndx 0 lcl_port_ndx 0\r\n"
    "  systm_phy_port 0\r\n"
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
    PARAM_SWEEPT20_API_PORTS_MAP_TO_LAG_MAP_TO_LAG_ID,
    "map_to_lag",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Map system device and local port to system physical port in the SW\r\n"
    "  databases and in the devices.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app ports map_to_lag lag_ndx 0 sys_phy_ports_3 0\r\n"
    "  sys_phy_ports_2 0 sys_phy_ports_1 0 sys_phy_ports_0 0\r\n"
    "  nof_prst 0\r\n"
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
    PARAM_SWEEPT20_API_PORTS_MAP_TO_LAG_MAP_TO_LAG_LAG_NDX_ID,
    "lag_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lag_ndx:\r\n"
    "  GLAG / RLAG Id.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app ports map_to_lag lag_ndx 0 sys_phy_ports_3 0\r\n"
    "  sys_phy_ports_2 0 sys_phy_ports_1 0 sys_phy_ports_0 0\r\n"
    "  nof_prst 0\r\n"
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
    PARAM_SWEEPT20_API_PORTS_MAP_TO_LAG_MAP_TO_LAG_LAG_INFO_SYS_PHY_PORTS_3_ID,
    "sys_phy_ports_3",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lag_info.sys_phy_ports[3]:\r\n"
    "  The list of the system physical port Ids that construct the LAG.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app ports map_to_lag lag_ndx 0 sys_phy_ports_3 0\r\n"
    "  sys_phy_ports_2 0 sys_phy_ports_1 0 sys_phy_ports_0 0\r\n"
    "  nof_prst 0\r\n"
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
    PARAM_SWEEPT20_API_PORTS_MAP_TO_LAG_MAP_TO_LAG_LAG_INFO_SYS_PHY_PORTS_2_ID,
    "sys_phy_ports_2",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lag_info.sys_phy_ports[2]:\r\n"
    "  The list of the system physical port Ids that construct the LAG.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app ports map_to_lag lag_ndx 0 sys_phy_ports_3 0\r\n"
    "  sys_phy_ports_2 0 sys_phy_ports_1 0 sys_phy_ports_0 0\r\n"
    "  nof_prst 0\r\n"
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
    PARAM_SWEEPT20_API_PORTS_MAP_TO_LAG_MAP_TO_LAG_LAG_INFO_SYS_PHY_PORTS_1_ID,
    "sys_phy_ports_1",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lag_info.sys_phy_ports[1]:\r\n"
    "  The list of the system physical port Ids that construct the LAG.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app ports map_to_lag lag_ndx 0 sys_phy_ports_3 0\r\n"
    "  sys_phy_ports_2 0 sys_phy_ports_1 0 sys_phy_ports_0 0\r\n"
    "  nof_prst 0\r\n"
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
    PARAM_SWEEPT20_API_PORTS_MAP_TO_LAG_MAP_TO_LAG_LAG_INFO_SYS_PHY_PORTS_0_ID,
    "sys_phy_ports_0",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lag_info.sys_phy_ports[0]:\r\n"
    "  The list of the system physical port Ids that construct the LAG.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app ports map_to_lag lag_ndx 0 sys_phy_ports_3 0\r\n"
    "  sys_phy_ports_2 0 sys_phy_ports_1 0 sys_phy_ports_0 0\r\n"
    "  nof_prst 0\r\n"
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
    PARAM_SWEEPT20_API_PORTS_MAP_TO_LAG_MAP_TO_LAG_LAG_INFO_NOF_PORST_ID,
    "nof_prst",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lag_info.nof_porst:\r\n"
    "  Number of active entries from the 'sys_phy_ports' array\r\n"
    "",
    "Examples:\r\n"
    "  timna_app ports map_to_lag lag_ndx 0 sys_phy_ports_3 0\r\n"
    "  sys_phy_ports_2 0 sys_phy_ports_1 0 sys_phy_ports_0 0\r\n"
    "  nof_prst 0\r\n"
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
    PARAM_SWEEPT20_API_PORTS_ID,
    "ports",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(8)|BIT(9), 0, 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif
#ifdef UI_SWEEPT20_API_MAC
  {
    PARAM_SWEEPT20_API_MAC_LEARN_AGENT_LEARN_AGENT_ID,
    "learn_agent",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Run on a dedicated task.\r\n"
    "  An endless loop, that process the CPU message queue, and execute\r\n"
    "  learn and age commands. \r\n"
    "",
    "Examples:\r\n"
    "  timna_app mac learn_agent\r\n"
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
    PARAM_SWEEPT20_API_MAC_PROCESS_CPU_MSG_PROCESS_CPU_MSG_ID,
    "process_cpu_msg",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Read, parse, and when needed, execute a single CPU command from the\r\n"
    "  message queue \r\n"
    "",
    "Examples:\r\n"
    "  timna_app mac process_cpu_msg\r\n"
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
    PARAM_SWEEPT20_API_MAC_RANGE_ADD_RANGE_ADD_ID,
    "range_add",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Add range of keys (MAC + FID) that share the same forwarding\r\n"
    "  infomarion. \r\n"
    "",
    "Examples:\r\n"
    "  timna_app mac range_add lst_svd 0 frst_svd 0\r\n"
    "  lst_mac_ddrss 0 frst_mac_ddrss 0\r\n"
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
    PARAM_SWEEPT20_API_MAC_RANGE_ADD_RANGE_ADD_KEYS_RANGE_LAST_SVID_ID,
    "lst_svd",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  keys_range.last_svid:\r\n"
    "  The last SVID to be insertedfrwrd_info  TIMNA_MACT_ENTRY_VALUE  The forwarding\r\n"
    "  the information of the above range\r\n"
    "",
    "Examples:\r\n"
    "  timna_app mac range_add lst_svd 0 frst_svd 0\r\n"
    "  lst_mac_ddrss 0 frst_mac_ddrss 0\r\n"
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
    PARAM_SWEEPT20_API_MAC_RANGE_ADD_RANGE_ADD_KEYS_RANGE_FIRST_SVID_ID,
    "frst_svd",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  keys_range.first_svid:\r\n"
    "  The first SVID to be inserted\r\n"
    "",
    "Examples:\r\n"
    "  timna_app mac range_add lst_svd 0 frst_svd 0\r\n"
    "  lst_mac_ddrss 0 frst_mac_ddrss 0\r\n"
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
    PARAM_SWEEPT20_API_MAC_RANGE_ADD_RANGE_ADD_KEYS_RANGE_LAST_MAC_ADDRESS_ID,
    "lst_mac_ddrss",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  keys_range.last_mac_address:\r\n"
    "  The last MAC Address to be inserted\r\n"
    "",
    "Examples:\r\n"
    "  timna_app mac range_add lst_svd 0 frst_svd 0\r\n"
    "  lst_mac_ddrss 0 frst_mac_ddrss 0\r\n"
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
    PARAM_SWEEPT20_API_MAC_RANGE_ADD_RANGE_ADD_KEYS_RANGE_FIRST_MAC_ADDRESS_ID,
    "frst_mac_ddrss",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  keys_range.first_mac_address:\r\n"
    "  The first MAC Address to be inserted\r\n"
    "",
    "Examples:\r\n"
    "  timna_app mac range_add lst_svd 0 frst_svd 0\r\n"
    "  lst_mac_ddrss 0 frst_mac_ddrss 0\r\n"
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
    PARAM_SWEEPT20_API_MAC_RANGE_REMOVE_RANGE_REMOVE_ID,
    "range_remove",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Add range of keys (MAC + FID) from the database. \r\n"
    "",
    "Examples:\r\n"
    "  timna_app mac range_remove lst_svd 0 frst_svd 0\r\n"
    "  lst_mac_ddrss 0 frst_mac_ddrss 0\r\n"
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
    PARAM_SWEEPT20_API_MAC_RANGE_REMOVE_RANGE_REMOVE_KEYS_RANGE_LAST_SVID_ID,
    "lst_svd",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  keys_range.last_svid:\r\n"
    "  The last SVID to be insertedfrwrd_info  TIMNA_MACT_ENTRY_VALUE  The forwarding\r\n"
    "  the information of the above range\r\n"
    "",
    "Examples:\r\n"
    "  timna_app mac range_remove lst_svd 0 frst_svd 0\r\n"
    "  lst_mac_ddrss 0 frst_mac_ddrss 0\r\n"
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
    PARAM_SWEEPT20_API_MAC_RANGE_REMOVE_RANGE_REMOVE_KEYS_RANGE_FIRST_SVID_ID,
    "frst_svd",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  keys_range.first_svid:\r\n"
    "  The first SVID to be inserted\r\n"
    "",
    "Examples:\r\n"
    "  timna_app mac range_remove lst_svd 0 frst_svd 0\r\n"
    "  lst_mac_ddrss 0 frst_mac_ddrss 0\r\n"
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
    PARAM_SWEEPT20_API_MAC_RANGE_REMOVE_RANGE_REMOVE_KEYS_RANGE_LAST_MAC_ADDRESS_ID,
    "lst_mac_ddrss",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  keys_range.last_mac_address:\r\n"
    "  The last MAC Address to be inserted\r\n"
    "",
    "Examples:\r\n"
    "  timna_app mac range_remove lst_svd 0 frst_svd 0\r\n"
    "  lst_mac_ddrss 0 frst_mac_ddrss 0\r\n"
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
    PARAM_SWEEPT20_API_MAC_RANGE_REMOVE_RANGE_REMOVE_KEYS_RANGE_FIRST_MAC_ADDRESS_ID,
    "frst_mac_ddrss",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  keys_range.first_mac_address:\r\n"
    "  The first MAC Address to be inserted\r\n"
    "",
    "Examples:\r\n"
    "  timna_app mac range_remove lst_svd 0 frst_svd 0\r\n"
    "  lst_mac_ddrss 0 frst_mac_ddrss 0\r\n"
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
    PARAM_SWEEPT20_API_MAC_ID,
    "mac",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(10)|BIT(11)|BIT(12)|BIT(13), 0, 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif
#ifdef UI_SWEEPT20_API_VSI
  {
    PARAM_SWEEPT20_API_VSI_OPEN_OPEN_ID,
    "open",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Open a virtual switch, with user specified information. \r\n"
    "",
    "Examples:\r\n"
    "  timna_app vsi open vsid_ndx 0 nof_ports 0\r\n"
    "  3_is_tggd 0 2_is_tggd 0\r\n"
    "  1_is_tggd 0 0_is_tggd 0\r\n"
    "  ports_lst_3 0 ports_lst_2 0 ports_lst_1 0\r\n"
    "  ports_lst_0 0 fld_grp 0 grss_vid 0\r\n"
    "  nbl_fldng 0\r\n"
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
    PARAM_SWEEPT20_API_VSI_OPEN_OPEN_VSID_NDX_ID,
    "vsid_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vsid_ndx:\r\n"
    "  Service ID. Range: 0 - (16K-1)\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vsi open vsid_ndx 0 nof_ports 0\r\n"
    "  3_is_tggd 0 2_is_tggd 0\r\n"
    "  1_is_tggd 0 0_is_tggd 0\r\n"
    "  ports_lst_3 0 ports_lst_2 0 ports_lst_1 0\r\n"
    "  ports_lst_0 0 fld_grp 0 grss_vid 0\r\n"
    "  nbl_fldng 0\r\n"
    "",
    "",
    {BIT(14), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VSI_OPEN_OPEN_LAST_VSID_NDX_ID,
    "up_to_vsid_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vsid_ndx:\r\n"
    "  Last VSI to operate. Enable operations on range of VSIs\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vsi open vsid_ndx 0 nof_ports 0\r\n"
    "  3_is_tggd 0 2_is_tggd 0\r\n"
    "  1_is_tggd 0 0_is_tggd 0\r\n"
    "  ports_lst_3 0 ports_lst_2 0 ports_lst_1 0\r\n"
    "  ports_lst_0 0 fld_grp 0 grss_vid 0\r\n"
    "  nbl_fldng 0\r\n"
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
    PARAM_SWEEPT20_API_VSI_OPEN_OPEN_SERVICE_INFO_PORTS_LIST_0_ID,
    "member_ports_list",
    (PARAM_VAL_RULES *)&Timna_ports_list[0],
    (sizeof(Timna_ports_list) / sizeof(Timna_ports_list[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  service_info.ports_list[SWEEPT20_API_VSI_MAX_PORTS_IN_VSI]:\r\n"
    "  List of ports\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vsi open vsid_ndx 0 nof_ports 0\r\n"
    "  3_is_tggd 0 2_is_tggd 0\r\n"
    "  1_is_tggd 0 0_is_tggd 0\r\n"
    "  ports_lst_3 0 ports_lst_2 0 ports_lst_1 0\r\n"
    "  ports_lst_0 0 fld_grp 0 grss_vid 0\r\n"
    "  nbl_fldng 0\r\n"
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
    PARAM_SWEEPT20_API_VSI_OPEN_OPEN_SERVICE_INFO_EGRESS_VID_ID,
    "egress_vid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  service_info.egress_vid:\r\n"
    "  The VID that the packets leave the bridge with.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vsi open vsid_ndx 0 nof_ports 0\r\n"
    "  3_is_tggd 0 2_is_tggd 0\r\n"
    "  1_is_tggd 0 0_is_tggd 0\r\n"
    "  ports_lst_3 0 ports_lst_2 0 ports_lst_1 0\r\n"
    "  ports_lst_0 0 fld_grp 0 grss_vid 0\r\n"
    "  nbl_fldng 0\r\n"
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
    PARAM_SWEEPT20_API_VSI_OPEN_OPEN_SERVICE_INFO_ENABLE_FLOODING_ID,
    "enable_floodng",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  service_info.enable_flooding:\r\n"
    "  TRUE:\r\n"
    "  packet is floodedFALSE: packet is dropped\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vsi open vsid_ndx 0 nof_ports 0\r\n"
    "  3_is_tggd 0 2_is_tggd 0\r\n"
    "  1_is_tggd 0 0_is_tggd 0\r\n"
    "  ports_lst_3 0 ports_lst_2 0 ports_lst_1 0\r\n"
    "  ports_lst_0 0 fld_grp 0 grss_vid 0\r\n"
    "  nbl_fldng 0\r\n"
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
    PARAM_SWEEPT20_API_VSI_OPEN_OPEN_SERVICE_INFO_MSTP_TOPOLOGY_ID_ID,
    "mstp_tplgy_id",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  service_info.mstp_topology_id:\r\n"
    "  Topology ID for MSTP blocking\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vsi open vsid_ndx 0 nof_ports 0\r\n"
    "  3_is_tggd 0 2_is_tggd 0\r\n"
    "  1_is_tggd 0 0_is_tggd 0\r\n"
    "  ports_lst_3 0 ports_lst_2 0 ports_lst_1 0\r\n"
    "  ports_lst_0 0 fld_grp 0 grss_vid 0\r\n"
    "  nbl_fldng 0\r\n"
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
    PARAM_SWEEPT20_API_VSI_INFO_DISPLAY_INFO_DISPLAY_ID,
    "info_display",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Print VSI information and associated VC / Ports * vlan.\r\n"
    "  When up_to_vsid_ndx > vsid_ndx, the bellow is printed for all the\r\n"
    "  VSIDs in the range.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vsi info_display vsid_ndx 0 up_to_vsid_ndx 0\r\n"
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
    PARAM_SWEEPT20_API_VSI_INFO_DISPLAY_INFO_DISPLAY_VSID_NDX_ID,
    "vsid_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vsid_ndx:\r\n"
    "  Virtual Switching Instance ID. Range: 0 - (16K-1)\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vsi info_display vsid_ndx 0 up_to_vsid_ndx 0\r\n"
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
    PARAM_SWEEPT20_API_VSI_INFO_DISPLAY_INFO_DISPLAY_LAST_VSID_NDX_ID,
    "up_to_vsid_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  up_to_vsid_ndx:\r\n"
    "  Last Virtual Switching Instance ID. Range: 0 - (16K-1)\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vsi info_display vsid_ndx 0 up_to_vsid_ndx 0\r\n"
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
    PARAM_SWEEPT20_API_OUTLIF_INFO_DISPLAY_OUTLIF_INFO_DISPLAY_ID,
    "outlif_info_display",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Print Egress information according to egress Logical Interface.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vsi outlif_info_display lif_ndx 0 lst_lif_ndx 0\r\n"
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
    PARAM_SWEEPT20_API_OUTLIF_INFO_DISPLAY_OUTLIF_INFO_DISPLAY_LIF_NDX_ID,
    "lif_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lif_ndx:\r\n"
    "  Outgoing Logical Interface: 32K - (256K-1)\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vsi outlif_info_display lif_ndx 0 lst_lif_ndx 0\r\n"
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
    PARAM_SWEEPT20_API_OUTLIF_INFO_DISPLAY_OUTLIF_INFO_DISPLAY_LAST_LIF_NDX_ID,
    "up_to_lif_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  last_lif_ndx:\r\n"
    "  Last Outgoing Logical Interface: 32K - (256K-1)\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vsi outlif_info_display lif_ndx 0 lst_lif_ndx 0\r\n"
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
    PARAM_SWEEPT20_API_VSI_ID,
    "vsi",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(14)|BIT(15)|BIT(16)|BIT(17)|BIT(18)|BIT(19)|BIT(28)|BIT(29), 0, 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif
#ifdef UI_SWEEPT20_API_VLAN
  {
    PARAM_SWEEPT20_API_VLAN_PORTS_ADD_PORTS_ADD_ID,
    "vlan_ports_add",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Open a VPLS service, with user specified information.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vlan ports_add vsid_ndx 0 vln_c_vid_valid 0 vln_c_vid 0\r\n"
    "  vln_vid 0 ports_lst_nof_ports 0 ports_lst_ndcts 0 ports_lst_plcr_id_3 0\r\n"
    "  ports_lst_plcr_id_2 0 ports_lst_plcr_id_1 0 ports_lst_plcr_id_0 0\r\n"
    "  ports_lst_ports_mmbrshp_3 0 ports_lst_ports_mmbrshp_2 0 ports_lst_ports_mmbrshp_1 0\r\n"
    "  ports_lst_ports_mmbrshp_0 0\r\n"
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
    PARAM_SWEEPT20_API_VLAN_PORTS_ADD_PORTS_ADD_VSID_NDX_ID,
    "vsid_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vsid_ndx:\r\n"
    "  Service ID. Range: 0 - (16K-1)\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vlan ports_add vsid_ndx 0 vln_c_vid_valid 0 vln_c_vid 0\r\n"
    "  vln_vid 0 ports_lst_nof_ports 0 ports_lst_ndcts 0 ports_lst_plcr_id_3 0\r\n"
    "  ports_lst_plcr_id_2 0 ports_lst_plcr_id_1 0 ports_lst_plcr_id_0 0\r\n"
    "  ports_lst_ports_mmbrshp_3 0 ports_lst_ports_mmbrshp_2 0 ports_lst_ports_mmbrshp_1 0\r\n"
    "  ports_lst_ports_mmbrshp_0 0\r\n"
    "",
    "",
    {BIT(18)|BIT(19), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VLAN_PORTS_ADD_PORTS_ADD_LAST_VSID_NDX_ID,
    "up_to_vsid_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  up_to_vsid_ndx:\r\n"
    "  Last VSI to oparate. Enable operation above range of VSIs\r\n"
    "  When used, the VID is increamented with the VSID"
    "",
    "Examples:\r\n"
    "  timna_app vlan ports_add vsid_ndx 0 vln_c_vid_valid 0 vln_c_vid 0\r\n"
    "  vln_vid 0 ports_lst_nof_ports 0 ports_lst_ndcts 0 ports_lst_plcr_id_3 0\r\n"
    "  ports_lst_plcr_id_2 0 ports_lst_plcr_id_1 0 ports_lst_plcr_id_0 0\r\n"
    "  ports_lst_ports_mmbrshp_3 0 ports_lst_ports_mmbrshp_2 0 ports_lst_ports_mmbrshp_1 0\r\n"
    "  ports_lst_ports_mmbrshp_0 0\r\n"
    "",
    "",
    {BIT(18)|BIT(19), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VLAN_PORTS_ADD_PORTS_ADD_VLAN_INFO_C_VID_VALID_ID,
    "c_vid_valid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vlan_info.c_vid_valid:\r\n"
    "  If FALSE, the value of 'c_vid' is invalid\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vlan ports_add vsid_ndx 0 c_vid_valid 0 c_vid 0\r\n"
    "  vid 0 ports_lst_nof_ports 0 ports_lst_ndcts 0 ports_lst_plcr_id_3 0\r\n"
    "  ports_lst_plcr_id_2 0 ports_lst_plcr_id_1 0 ports_lst_plcr_id_0 0\r\n"
    "  ports_lst_ports_mmbrshp_3 0 ports_lst_ports_mmbrshp_2 0 ports_lst_ports_mmbrshp_1 0\r\n"
    "  ports_lst_ports_mmbrshp_0 0\r\n"
    "",
    "",
    {BIT(18)|BIT(19), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VLAN_PORTS_ADD_PORTS_ADD_VLAN_INFO_C_VID_ID,
    "c_vid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vlan_info.c_vid:\r\n"
    "  Range: 0 - (4K - 1) When the port is configured to only check for the S-VID,\r\n"
    "  the user is expected to set 'c_vid_valid' to FALSE.When the port is\r\n"
    "  configured to look according to the C-VID as well, the value of this field\r\n"
    "  should be the C-VID.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vlan ports_add vsid_ndx 0 c_vid_valid 0 c_vid 0\r\n"
    "  vid 0 ports_lst_nof_ports 0 ports_lst_ndcts 0 ports_lst_plcr_id_3 0\r\n"
    "  ports_lst_plcr_id_2 0 ports_lst_plcr_id_1 0 ports_lst_plcr_id_0 0\r\n"
    "  ports_lst_ports_mmbrshp_3 0 ports_lst_ports_mmbrshp_2 0 ports_lst_ports_mmbrshp_1 0\r\n"
    "  ports_lst_ports_mmbrshp_0 0\r\n"
    "",
    "",
    {BIT(18)|BIT(19), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VLAN_PORTS_ADD_PORTS_ADD_VLAN_INFO_VID_ID,
    "vid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vlan_info.vid:\r\n"
    "  Range: 0 - (4K - 1)VLAN ID to be associated with the given VSID and port\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vlan ports_add vsid_ndx 0 c_vid_valid 0 c_vid 0\r\n"
    "  vid 0 ports_lst_nof_ports 0 ports_lst_ndcts 0 ports_lst_plcr_id_3 0\r\n"
    "  ports_lst_plcr_id_2 0 ports_lst_plcr_id_1 0 ports_lst_plcr_id_0 0\r\n"
    "  ports_lst_ports_mmbrshp_3 0 ports_lst_ports_mmbrshp_2 0 ports_lst_ports_mmbrshp_1 0\r\n"
    "  ports_lst_ports_mmbrshp_0 0\r\n"
    "",
    "",
    {BIT(18)|BIT(19), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
/*
  {
    PARAM_SWEEPT20_API_VLAN_PORTS_ADD_PORTS_ADD_VLAN_INFO_VID_RANGE_ID,
    "nof_consequtive_vids",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Enable adding range of VIDs:\r\n"
    "  Range: 0 - (4K - 1)VLAN ID to be associated with the given VSID and port\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vlan ports_add vsid_ndx 0 c_vid_valid 0 c_vid 0\r\n"
    "  vid 0 ports_lst_nof_ports 0 ports_lst_ndcts 0 ports_lst_plcr_id_3 0\r\n"
    "  ports_lst_plcr_id_2 0 ports_lst_plcr_id_1 0 ports_lst_plcr_id_0 0\r\n"
    "  ports_lst_ports_mmbrshp_3 0 ports_lst_ports_mmbrshp_2 0 ports_lst_ports_mmbrshp_1 0\r\n"
    "  ports_lst_ports_mmbrshp_0 0\r\n"
    "",
    "",
    {BIT(18)|BIT(19), 0, 0, 0},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
*/
  {
    PARAM_SWEEPT20_API_VLAN_PORTS_ADD_PORTS_ADD_PORTS_LIST_PORTS_ID,
    "ports_list",
    (PARAM_VAL_RULES *)&Timna_ports_list[0],
    (sizeof(Timna_ports_list) / sizeof(Timna_ports_list[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ports_list[SWEEPT20_API_VSI_MAX_PORTS_IN_VSI]:\r\n"
    "  List of ports\r\n"
    "",
    "Examples:\r\n"
    "",
    "",
    {BIT(18)|BIT(19), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VLAN_PORTS_ADD_PORTS_ADD_PORTS_LIST_PORTS_SEND_TAGGED_ID,
    "send_tagged",
    (PARAM_VAL_RULES *)&Timna_ports_list[0],
    (sizeof(Timna_ports_list) / sizeof(Timna_ports_list[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  send_tagged[SWEEPT20_API_VSI_MAX_PORTS_IN_VSI]:\r\n"
    "  For each port, according to the order of ports_list\r\n"
    "  1 set packets leaving with the port and VSI to be sent tagged\r\n"
    "  0 set packets leaving with the port and VSI to be sent untagged\r\n"
    "",
    "Examples:\r\n"
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
    PARAM_SWEEPT20_API_VLAN_PORTS_REMOVE_PORTS_REMOVE_ID,
    "vlan_ports_remove",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Open a VPLS service, with user specified information.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vlan ports_remove vsid_ndx 0 c_vid_valid 0 c_vid 0\r\n"
    "  vid 0 ports_lst 0\r\n"
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
#endif
#ifdef UI_SWEEPT20_API_POLICER
  {
    PARAM_SWEEPT20_API_POLICER_MAP_RANGE_SET_MAP_RANGE_SET_ID,
    "map_set",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  MAP streams to policer, according to map_type.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app policer map_set port_ndx 0 plcr_ndx 0 lst_plcr_ndx 0\r\n"
    "  map_typ 0\r\n"
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
    PARAM_SWEEPT20_API_POLICER_MAP_RANGE_SET_MAP_RANGE_SET_POLICER_NDX_ID,
    "plcr_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  policer_ndx:\r\n"
    "  The policer to be mapped. Range: 0 - (32K-1)\r\n"
    "",
    "Examples:\r\n"
    "  timna_app policer map_set port_ndx 0 plcr_ndx 0 lst_plcr_ndx 0\r\n"
    "  map_typ 0\r\n"
    "",
    "",
    {BIT(20), 0, 0, 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_POLICER_MAP_RANGE_SET_MAP_RANGE_SET_PORT_NDX_ID,
    "port_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_ndx:\r\n"
    "  System port ID. Relevant when the map_type is port related.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app policer map_set port_ndx 0 plcr_ndx 0 lst_plcr_ndx 0\r\n"
    "  map_typ 0\r\n"
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
    PARAM_SWEEPT20_API_POLICER_MAP_RANGE_SET_MAP_RANGE_SET_LAST_POLICER_NDX_ID,
    "lst_plcr_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  last_policer_ndx:\r\n"
    "  Enables multi-policers attachment.\r\n"
    "  If last_policer_ndx > policer_ndx, and last_id > first_id,\r\n"
    "  'first_id - last_id' are mapped to policers\r\n"
    "  'policer_ndx -last_policer_ndx'.\r\n"
    "  For example:\r\n"
    "  policer_ndx\r\n"
    "  300\r\n"
    "  last_policer_ndx\r\n"
    "  9000\r\n"
    "  map_type\r\n"
    "  'SWEEPT20_API_POLICE_OP_IN_VC_ID'\r\n"
    "  first_id\r\n"
    "  300\r\n"
    "  last_id\r\n"
    "  9000\r\n"
    "  Results in mapping VC labels 300-9000 to policers IDs 300-9000,\r\n"
    "  when VC label 300 is attached to policer 300.\r\n"
    "  policer_ndx\r\n"
    "  1\r\n"
    "  last_policer_ndx 10000\r\n"
    "  map_type\r\n"
    "  'SWEEPT20_API_POLICE_OP_IN_VC_ID'\r\n"
    "  first_id\r\n"
    "  1\r\n"
    "  last_id\r\n"
    "  200000\r\n"
    "  Results in mapping VC labels 1-200000 to policers IDs 1-10000,\r\n"
    "  when VC labels 1-20 mapped to policer 1.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app policer map_set port_ndx 0 plcr_ndx 0 lst_plcr_ndx 0\r\n"
    "  map_typ 0\r\n"
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
    PARAM_SWEEPT20_API_POLICER_MAP_RANGE_SET_MAP_RANGE_SET_MAP_TYPE_ID,
    "map_typ",
    (PARAM_VAL_RULES *)&SWEEPT20_POLICE_OP_rule[0],
    (sizeof(SWEEPT20_POLICE_OP_rule) / sizeof(SWEEPT20_POLICE_OP_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  map_type:\r\n"
    "  The type of the mapping, might be according to VSID, port,\r\n"
    "  Ingress VLAN and other options\r\n"
    "",
    "Examples:\r\n"
    "  timna_app policer map_set port_ndx 0 plcr_ndx 0 lst_plcr_ndx 0\r\n"
    "  map_typ 0\r\n"
    "",
    "",
    {BIT(20), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_POLICER_MAP_RANGE_SET_MAP_RANGE_SET_FIRST_ID_ID,
    "map_id",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  map_id:\r\n"
    "  The ID of the entity defined in the map_type. For example when the map_type\r\n"
    "  is according to vsid, the map_id holds the ID of the VSID, that should be\r\n"
    "  mapped to the policer_ndx\r\n"
    "",
    "Examples:\r\n"
    "  timna_app policer map_set port_ndx 0 plcr_ndx 0 lst_plcr_ndx 0\r\n"
    "  map_typ 0\r\n"
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
    PARAM_SWEEPT20_API_POLICER_MAP_RANGE_SET_MAP_RANGE_SET_LAST_ID_ID,
    "lst_map_id",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  last_id:\r\n"
    "  When (first_id != last_id), ranges of the map_type are mapped to\r\n"
    "  the same policer.\r\n"
    "  For Example:\r\n"
    "  policer_ndx\r\n"
    "  20,\r\n"
    "  last_policer_ndx\r\n"
    "  20,\r\n"
    "  map_type\r\n"
    "  'SWEEPT20_API_POLICE_OP_IN_VC_ID'\r\n"
    "  first_id\r\n"
    "  300\r\n"
    "  last_id\r\n"
    "  9000\r\n"
    "  Results in mapping VC labels 300-9000 to policer ID 20.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app policer map_set port_ndx 0 plcr_ndx 0 lst_plcr_ndx 0\r\n"
    "  map_typ 0\r\n"
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
    PARAM_SWEEPT20_VSID_IN_VC_ASSOCIATED_POLICER_GET_IN_VC_ASSOCIATED_POLICER_GET_ID,
    "in_vc_associated_policer_get",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Gets the policer that a VC label is policed according.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vsid in_vc_associated_policer_get vc_lbl_ndx 0\r\n"
    "",
    "",
    {0, 0, BIT(31), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_VSID_IN_VC_ASSOCIATED_POLICER_GET_IN_VC_ASSOCIATED_POLICER_GET_VC_LABEL_NDX_ID,
    "vc_lbl_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vc_label_ndx:\r\n"
    "  the VC label.\r\n"
    "  SOC_SAND_IN uint32\r\n"
    "  *policer_id -\r\n"
    "  Policer ID to be attached with the VC label. Zero indicates no policing.\r\n"
    "  SOC_SAND_IN uint32\r\n"
    "  *control_ptr\r\n"
    "  defines how to treat the policer results (the resulted color), there are 8\r\n"
    "  controllers that are hard coded (not configurable. RETURN\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vsid in_vc_associated_policer_get vc_lbl_ndx 0\r\n"
    "",
    "",
    {0, 0, BIT(31), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_VSID_PORT_VLAN_ASSOCIATED_POLICER_GET_PORT_VLAN_ASSOCIATED_POLICER_GET_ID,
    "port_vlan_associated_policer_get",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Gets the policer that a (port,vlan) is policed according.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vsid port_vlan_associated_policer_get c_vid_vld 0\r\n"
    "  c_vid 0 vid 0 prt 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_VSID_PORT_VLAN_ASSOCIATED_POLICER_GET_PORT_VLAN_ASSOCIATED_POLICER_GET_ETH_INFO_NDX_C_VID_VALID_ID,
    "c_vid_vld",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  eth_info_ndx.c_vid_valid:\r\n"
    "  Whether to consider the c_vid.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vsid port_vlan_associated_policer_get c_vid_vld 0\r\n"
    "  c_vid 0 vid 0 prt 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_VSID_PORT_VLAN_ASSOCIATED_POLICER_GET_PORT_VLAN_ASSOCIATED_POLICER_GET_ETH_INFO_NDX_C_VID_ID,
    "c_vid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  eth_info_ndx.c_vid:\r\n"
    "  The vid of the c-tag (the external tag).\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vsid port_vlan_associated_policer_get c_vid_vld 0\r\n"
    "  c_vid 0 vid 0 prt 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_VSID_PORT_VLAN_ASSOCIATED_POLICER_GET_PORT_VLAN_ASSOCIATED_POLICER_GET_ETH_INFO_NDX_S_VID_ID,
    "vid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  eth_info_ndx.vid:\r\n"
    "  The vid of the s-tag (the external tag).\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vsid port_vlan_associated_policer_get c_vid_vld 0\r\n"
    "  c_vid 0 vid 0 prt 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_VSID_PORT_VLAN_ASSOCIATED_POLICER_GET_PORT_VLAN_ASSOCIATED_POLICER_GET_ETH_INFO_NDX_PORT_ID,
    "prt",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  eth_info_ndx.port:\r\n"
    "  The local port id.Range 0-31.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vsid port_vlan_associated_policer_get c_vid_vld 0\r\n"
    "  c_vid 0 vid 0 prt 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_SWEEPT20_API_POLICER_MAP_RANGE_GET_MAP_RANGE_GET_ID,
    "map_range_get",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  MAP streams to policer, according to map_type.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app policer map_range_get port_ndx 0 plcr_ndx 0 lst_plcr_ndx 0\r\n"
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
    PARAM_SWEEPT20_API_POLICER_MAP_RANGE_GET_MAP_RANGE_GET_PORT_NDX_ID,
    "port_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_ndx:\r\n"
    "  System port ID. Relevant when the map_type is port related.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app policer map_range_get port_ndx 0 plcr_ndx 0 lst_plcr_ndx 0\r\n"
    "",
    "",
    {BIT(21), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_POLICER_MAP_RANGE_GET_MAP_RANGE_GET_POLICER_NDX_ID,
    "plcr_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  policer_ndx:\r\n"
    "  The policer to be mapped. Range: 0 - (32K-1)\r\n"
    "",
    "Examples:\r\n"
    "  timna_app policer map_range_get port_ndx 0 plcr_ndx 0 lst_plcr_ndx 0\r\n"
    "",
    "",
    {BIT(21), 0, 0, 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_POLICER_MAP_RANGE_GET_MAP_RANGE_GET_LAST_POLICER_NDX_ID,
    "lst_plcr_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  last_policer_ndx:\r\n"
    "  Enables multi-policers attachment.\r\n"
    "  If last_policer_ndx > policer_ndx, and last_id > first_id,\r\n"
    "  'first_id - last_id' are mapped to policers\r\n"
    "  'policer_ndx -last_policer_ndx'.\r\n"
    "  For example:\r\n"
    "  policer_ndx\r\n"
    "  300\r\n"
    "  last_policer_ndx\r\n"
    "  9000\r\n"
    "  map_type\r\n"
    "  'SWEEPT20_API_POLICE_OP_IN_VC_ID'\r\n"
    "  first_id\r\n"
    "  300\r\n"
    "  last_id\r\n"
    "  9000\r\n"
    "  Results in mapping VC labels 300-9000 to policers IDs 300-9000,\r\n"
    "  when VC label 300 is attached to policer 300.\r\n"
    "  policer_ndx\r\n"
    "  1\r\n"
    "  last_policer_ndx 10000\r\n"
    "  map_type\r\n"
    "  'SWEEPT20_API_POLICE_OP_IN_VC_ID'\r\n"
    "  first_id\r\n"
    "  1\r\n"
    "  last_id\r\n"
    "  200000\r\n"
    "  Results in mapping VC labels 1-200000 to policers IDs 1-10000,\r\n"
    "  when VC labels 1-20 mapped to policer 1.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app policer map_range_get port_ndx 0 plcr_ndx 0 lst_plcr_ndx 0\r\n"
    "",
    "",
    {BIT(21), 0, 0, 0},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
    {
    PARAM_SWEEPT20_POLICER_GLBL_INFO_SET_GLBL_INFO_SET_ID,
    "glbl_info_set",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Sets the global information for policing. \r\n"
    "",
    "Ex:\r\n"
    "  |timna_app policer glbl_info_set policer_burst_vls_3 0\r\n"
    "  policer_burst_vls_2 0 policer_burst_vls_1 0 policer_burst_vls_0 0\r\n"
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
    PARAM_SWEEPT20_POLICER_GLBL_INFO_SET_GLBL_INFO_SET_POLICER_GLBL_INFO_EBS_VALS_3_ID,
    "policer_burst_value",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  policer_glbl_info.burst_vals[index]:\r\n"
    "  The global excessive burst size values. Each ingress policer may points to\r\n"
    "  one of these values.\r\n"
    "",
    "Ex:\r\n"
    "  |timna_app policer glbl_info_set policer_burst_vls_3 0\r\n"
    "  policer_burst_vls_2 0 policer_burst_vls_1 0 policer_burst_vls_0 0\r\n"
    "",
    "",
    {BIT(11), 0, 0, 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_POLICER_GLBL_INFO_SET_GLBL_INFO_SET_POLICER_GLBL_INFO_EBS_VALS_2_ID,
    "policer_burst_index",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  policer_glbl_info.burst_vals[index]:\r\n"
    "  The global excessive burst size values. Each ingress policer may points to\r\n"
    "  one of these values.\r\n"
    "",
    "Ex:\r\n"
    "  |timna_app policer glbl_info_set policer_burst_vls_3 0\r\n"
    "  policer_burst_vls_2 0 policer_burst_vls_1 0 policer_burst_vls_0 0\r\n"
    "",
    "",
    {BIT(11), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_POLICER_GLBL_INFO_GET_GLBL_INFO_GET_ID,
    "glbl_info_get",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Sets the global information for policing. \r\n"
    "",
    "Ex:\r\n"
    "  |timna_app policer glbl_info_get\r\n"
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
    PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_ID,
    "params_range_set",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the cir and eir rates for rang of policers \r\n"
    "",
    "Examples:\r\n"
    "  timna_app policer params_range_set plcr_ndx 0 lst_plcr_ndx 0\r\n"
    "  ebs 0 eir 0 cbs 0\r\n"
    "  cir 0 clr_wre 0 rte_mod 0\r\n"
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
    PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_GET_ID,
    "params_range_get",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the cir and eir rates for rang of policers \r\n"
    "",
    "Examples:\r\n"
    "  timna_app policer params_range_get plcr_ndx 0 lst_plcr_ndx 0\r\n"
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
    PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_POLICER_NDX_ID,
    "plcr_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  policer_ndx:\r\n"
    "  The policer to be mapped. Range: 0 - (32K-1)\r\n"
    "",
    "Examples:\r\n"
    "  timna_app policer params_range_set plcr_ndx 0 lst_plcr_ndx 0\r\n"
    "  ebs 0 eir 0 cbs 0\r\n"
    "  cir 0 clr_wre 0 rte_mod 0\r\n"
    "",
    "",
    {BIT(22)|BIT(23), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_LAST_POLICER_NDX_ID,
    "up_to_plcr_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  last_policer_ndx:\r\n"
    "  Enables multi-policers attachment.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app policer params_range_set plcr_ndx 0 lst_plcr_ndx 0\r\n"
    "  ebs 0 eir 0 cbs 0\r\n"
    "  cir 0 clr_wre 0 rte_mod 0\r\n"
    "",
    "",
    {BIT(22)|BIT(23), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_INGRESS_POLICER_PARAMS_EBS_ID,
    "ebs",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ingress_policer_params.ebs:\r\n"
    "  Excess Burst Size (EBS)In Ingress Policer: this points to one of 32 global\r\n"
    "  ebs values. Range 0-31.In Ethernet Policer: Amount of credit that can be\r\n"
    "  accumulated and enable a burst of traffic in the line rate (which is usually\r\n"
    "  Bucket Rate) Packet mode: Number of packets in burst.Byte mode: Number of\r\n"
    "  bytes in burst. Range 0-131071 bytes.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app policer params_range_set plcr_ndx 0 lst_plcr_ndx 0\r\n"
    "  ebs 0 eir 0 cbs 0\r\n"
    "  cir 0 clr_wre 0 rte_mod 0\r\n"
    "",
    "",
    {BIT(22), 0, 0, 0},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_INGRESS_POLICER_PARAMS_EIR_ID,
    "eir",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ingress_policer_params.eir:\r\n"
    "  Excess Information Rate (EIR) Packet mode: traffic rate in packet per second\r\n"
    "  (Kpps).Byte mode: traffic rate in bits per second (Kbps).Range 1000K -\r\n"
    "  4,894,464K (packets/bits per second) with resolution of 1.56%.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app policer params_range_set plcr_ndx 0 lst_plcr_ndx 0\r\n"
    "  ebs 0 eir 0 cbs 0\r\n"
    "  cir 0 clr_wre 0 rte_mod 0\r\n"
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
    PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_INGRESS_POLICER_PARAMS_CBS_ID,
    "cbs",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ingress_policer_params.cbs:\r\n"
    "  Committed Burst Size (CBS).In ingress policer this points to one of 32 global\r\n"
    "  cbs values. Range 0-31In Ethernet Policer: Amount of credit that can be\r\n"
    "  accumulated and enable a burst of traffic in the line rate (which is usually\r\n"
    "  Bucket Rate).Packet mode: Number of packets in burst.Byte mode: Number of\r\n"
    "  bytes in burst. Range 0-131071 bytes..\r\n"
    "",
    "Examples:\r\n"
    "  timna_app policer params_range_set plcr_ndx 0 lst_plcr_ndx 0\r\n"
    "  ebs 0 eir 0 cbs 0\r\n"
    "  cir 0 clr_wre 0 rte_mod 0\r\n"
    "",
    "",
    {BIT(22), 0, 0, 0},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_INGRESS_POLICER_PARAMS_CIR_ID,
    "cir",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ingress_policer_params.cir:\r\n"
    "  Committed Information Rate (CIR) Packet mode: traffic rate in packet per\r\n"
    "  second (Kpps).Byte mode: traffic rate in bits per second (Kbps).Range 1000K -\r\n"
    "  9,750,000K (packets/bits per second) with resolution of 1.56%.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app policer params_range_set plcr_ndx 0 lst_plcr_ndx 0\r\n"
    "  ebs 0 eir 0 cbs 0\r\n"
    "  cir 0 clr_wre 0 rte_mod 0\r\n"
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
    PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_INGRESS_POLICER_PARAMS_COLOR_AWARE_ID,
    "clr_wre",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ingress_policer_params.color_aware:\r\n"
    "  Indicator.1 - policer is color aware.\r\n"
    "  In the color-aware operation, the incoming packet color (Green, Yellow, or\r\n"
    "  Red) is considered to the policing and marking algorithm; 0 - color-blind\r\n"
    "  mode assumes all incoming packets are Green colored.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app policer params_range_set plcr_ndx 0 lst_plcr_ndx 0\r\n"
    "  ebs 0 eir 0 cbs 0\r\n"
    "  cir 0 clr_wre 0 rte_mod 0\r\n"
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
    PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_CIR_STEP_ID,
    "cir_stp",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  cir_step:\r\n"
    "  policer_ndx i will get eir rate = pramas.eir_rate + i * eir_step.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app policer params_range_set plcr_ndx 0 lst_plcr_ndx 0\r\n"
    "  ebs 0 eir 0 cbs 0\r\n"
    "  cir 0 clr_wre 0 rte_mod 0\r\n"
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
    PARAM_SWEEPT20_API_POLICER_PARAMS_RANGE_SET_PARAMS_RANGE_SET_EIR_STEP_ID,
    "eir_stp",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  eir_step:\r\n"
    "  policer_ndx i will get eir rate = pramas.eir_rate + i * eir_step\r\n"
    "",
    "Examples:\r\n"
    "  timna_app policer params_range_set plcr_ndx 0 lst_plcr_ndx 0\r\n"
    "  ebs 0 eir 0 cbs 0\r\n"
    "  cir 0 clr_wre 0 rte_mod 0\r\n"
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
    PARAM_SWEEPT20_API_POLICER_ID,
    "policer",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(11)|BIT(12)|BIT(20)/*|BIT(21)*/|BIT(22)|BIT(23), 0, BIT(31), BIT(0)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif

#ifdef UI_SWEEPT20_API_EGRESS_ENCAP
  {
    PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_ID,
    "pointer_set",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set  encapsulation pointer with tunnel information (optional) and ARP info.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app egress_encap pointer_set eep_ndx 0 entry_valid 0 tunnel_valid 0\r\n"
    "  out_tunnel_id 0 exp 0 arp_mac 0 arp_vid 0 arp_tagged 0 arp_user_priority 0 \r\n"
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
    PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_EGRESS_ENCAP_NDX_ID,
    "eep_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  egress encapsulation pointer_ndx:\r\n"
    "",
    "Examples:\r\n"
    "  timna_app egress_encap pointer_set eep_ndx 0 entry_valid 0 tunnel_valid 0\r\n"
    "  out_tunnel_id 0 exp 0 arp_mac 0 arp_vid 0 arp_tagged 0 arp_user_priority 0 \r\n"
    "",
    "",
    {BIT(24), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_RANGE_UP_TO_EGRESS_ENCAP_NDX_ID,
    "range_up_to_eep_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  egress encapsulation pointer_ndx range up to:\r\n"
    "",
    "Examples:\r\n"
    "  timna_app egress_encap pointer_set eep_ndx 0 entry_valid 0 tunnel_valid 0\r\n"
    "  out_tunnel_id 0 exp 0 arp_mac 0 arp_vid 0 arp_tagged 0 arp_user_priority 0 \r\n"
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
    PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_RANGE_INC_MAC_ID,
    "range_inc_mac",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  increment MAC for each eep index:\r\n"
    "",
    "Examples:\r\n"
    "  timna_app egress_encap pointer_set eep_ndx 0 entry_valid 0 tunnel_valid 0\r\n"
    "  out_tunnel_id 0 exp 0 arp_mac 0 arp_vid 0 arp_tagged 0 arp_user_priority 0 \r\n"
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
    PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_RANGE_INC_VID_ID,
    "range_inc_vid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  increment VID for each eep index:\r\n"
    "",
    "Examples:\r\n"
    "  timna_app egress_encap pointer_set eep_ndx 0 entry_valid 0 tunnel_valid 0\r\n"
    "  out_tunnel_id 0 exp 0 arp_mac 0 arp_vid 0 arp_tagged 0 arp_user_priority 0 \r\n"
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
    PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_RANGE_INC_LABEL_ID,
    "range_inc_label_id",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  increment LABEL ID for each eep index:\r\n"
    "",
    "Examples:\r\n"
    "  timna_app egress_encap pointer_set eep_ndx 0 entry_valid 0 tunnel_valid 0\r\n"
    "  out_tunnel_id 0 exp 0 arp_mac 0 arp_vid 0 arp_tagged 0 arp_user_priority 0 \r\n"
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
    PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_INFO_OUT_TUNNEL_ID_ID,
    "out_tunnel_id",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.out_tunnel_id:\r\n"
    "  Range: 0-(1M-1).The Tunnel ID that the packet is stamped with on its way\r\n"
    "  out.arp_info  TIMNA_ETHER_P2P_EDIT_INFO  The ARP information\r\n"
    "  contains:1.  Destination MAC2.  VLAN-ID3.  VLAN-UP4.  Transmit Tagged\r\n"
    "",
    "Examples:\r\n"
    "  timna_app egress_encap pointer_set eep_ndx 0 entry_valid 0 tunnel_valid 0\r\n"
    "  out_tunnel_id 0 exp 0 arp_mac 0 arp_vid 0 arp_tagged 0 arp_user_priority 0 \r\n"
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
    PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_INFO_IS_VALID_ID,
    "entry_valid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.is_valid:\r\n"
    "  Valid / Invalid encapsulation info.\r\n"
    "  IF FALSE then packet arrive this EEP is dropped.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app egress_encap pointer_set eep_ndx 0 entry_valid 0 tunnel_valid 0\r\n"
    "  out_tunnel_id 0 exp 0 arp_mac 0 arp_vid 0 arp_tagged 0 arp_user_priority 0 \r\n"
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
    PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_INFO_IS_TUNNEL_VALID_ID,
    "tunnel_valid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.is_valid:\r\n"
    "  Specifies whether the Tunnel is valid\r\n"
    "  If True then the packet will be encapsulated with\r\n"
    "  tunnel as specified in tunnel_label_info.\r\n"
    "  otherwise no tunnel is added.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app egress_encap pointer_set eep_ndx 0 entry_valid 0 tunnel_valid 0\r\n"
    "  out_tunnel_id 0 exp 0 arp_mac 0 arp_vid 0 arp_tagged 0 arp_user_priority 0 \r\n"
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
    PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_INFO_EXP_ID,
    "exp",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.exp:\r\n"
    "  Experimental bits outgoing traffic over this tunnel\r\n"
    "",
    "Examples:\r\n"
    "  timna_app egress_encap pointer_set eep_ndx 0 entry_valid 0 tunnel_valid 0\r\n"
    "  out_tunnel_id 0 exp 0 arp_mac 0 arp_vid 0 arp_tagged 0 arp_user_priority 0 \r\n"
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
    PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_INFO_TAGGED_ID,
    "arp_tagged",
    (PARAM_VAL_RULES *)&timna_api_free_vals[0],
    (sizeof(timna_api_free_vals) / sizeof(timna_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  edit_info.:\r\n"
    "  Whether to transmit the packet with VLAN TAG.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app egress_encap pointer_set eep_ndx 0 entry_valid 0 tunnel_valid 0\r\n"
    "  out_tunnel_id 0 exp 0 arp_mac 0 arp_vid 0 arp_tagged 0 arp_user_priority 0 \r\n"
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
    PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_INFO_USER_PRIORITY_ID,
    "arp_user_priority",
    (PARAM_VAL_RULES *)&timna_api_free_vals[0],
    (sizeof(timna_api_free_vals) / sizeof(timna_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  edit_info.user_priority:\r\n"
    "  Vlan ID, UP to build the out vlan tag.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app egress_encap pointer_set eep_ndx 0 entry_valid 0 tunnel_valid 0\r\n"
    "  out_tunnel_id 0 exp 0 arp_mac 0 arp_vid 0 arp_tagged 0 arp_user_priority 0 \r\n"
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
    PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_INFO_VID_ID,
    "arp_vid",
    (PARAM_VAL_RULES *)&timna_api_free_vals[0],
    (sizeof(timna_api_free_vals) / sizeof(timna_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  edit_info.vid:\r\n"
    "  Vlan ID, to build the out vlan tag.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app egress_encap pointer_set eep_ndx 0 entry_valid 0 tunnel_valid 0\r\n"
    "  out_tunnel_id 0 exp 0 arp_mac 0 arp_vid 0 arp_tagged 0 arp_user_priority 0 \r\n"
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
    PARAM_SWEEPT20_API_EGRESS_ENCAP_SET_EGRESS_ENCAP_SET_INFO_MAC_ID,
    "arp_mac",
    (PARAM_VAL_RULES *)&Timna_mac_addr_vals[0],
    (sizeof(Timna_mac_addr_vals) / sizeof(Timna_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  edit_info.mac:\r\n"
    "  Mac address.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app egress_encap pointer_set eep_ndx 0 entry_valid 0 tunnel_valid 0\r\n"
    "  out_tunnel_id 0 exp 0 arp_mac 0 arp_vid 0 arp_tagged 0 arp_user_priority 0 \r\n"
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
    PARAM_SWEEPT20_API_EGRESS_ENCAP_GET_EGRESS_ENCAP_GET_ID,
    "pointer_get",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set tunnel encapsulation pointer with tunnel information.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app egress_encap pointer_get eep_ndx 0 \r\n"
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
    PARAM_SWEEPT20_API_EGRESS_ENCAP_GET_EGRESS_ENCAP_GET_EGRESS_ENCAP_NDX_ID,
    "eep_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  egress encapsulation pointer_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  egress encapsulation pointer_ndx\r\n"
    "",
    "Examples:\r\n"
    "  timna_app egress_encap pointer_get eep_ndx 0 \r\n"
    "",
    "",
    {BIT(25), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_EEP_ID,
    "egress_encap",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(24)|BIT(25), 0, 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif
#ifdef UI_SWEEPT20_API_MPLS
  {
    PARAM_SWEEPT20_API_MPLS_LSR_LABEL_SET_LSR_LABEL_SET_ID,
    "lsr_label_set",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Per MPLS label in the ingress, set the forwarding and editing\r\n"
    "  information.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app mpls lsr_label_set in_label_ndx 0 eep_id 0 is_php 0\r\n"
    "  out_label 0 out_sys_port 0 is_valid 0\r\n"
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
    PARAM_SWEEPT20_API_MPLS_LSR_LABEL_SET_LSR_LABEL_SET_IN_LABEL_NDX_ID,
    "in_label_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  in_label_ndx:\r\n"
    "  In MPLS Label ID. Range: 0 - (256K-1)\r\n"
    "",
    "Examples:\r\n"
    "  timna_app mpls lsr_label_set in_label_ndx 0 eep_id 0 is_php 0\r\n"
    "  out_label 0 out_sys_port 0 is_valid 0\r\n"
    "",
    "",
    {BIT(26), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MPLS_LSR_LABEL_SET_LSR_LABEL_SET_INFO_TUNNEL_ENCAP_ID_ID,
    "eep_id",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.tunnel_encap_id:\r\n"
    "  Range: 0-256K. Tunnel encapsulation ID. Point to the tunnel information.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app mpls lsr_label_set in_label_ndx 0 eep_id 0 is_php 0\r\n"
    "  out_label 0 out_sys_port 0 is_valid 0\r\n"
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
    PARAM_SWEEPT20_API_MPLS_LSR_LABEL_SET_LSR_LABEL_SET_INFO_IS_PHP_ID,
    "is_php",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.is_php:\r\n"
    "  Penultimate Hop Popping.When asserted, the out-label is not appended to the\r\n"
    "  packet, and therefore the 'out_label' field is ignored.The feature is\r\n"
    "  applicable when the next hop lack the capability of label termination.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app mpls lsr_label_set in_label_ndx 0 eep_id 0 is_php 0\r\n"
    "  out_label 0 out_sys_port 0 is_valid 0\r\n"
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
    PARAM_SWEEPT20_API_MPLS_LSR_LABEL_SET_LSR_LABEL_SET_INFO_OUT_LABEL_ID,
    "out_label",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.out_label:\r\n"
    "  The MPLS Label ID that the packet is forwarded to.Range: 0-(256K-1).\r\n"
    "",
    "Examples:\r\n"
    "  timna_app mpls lsr_label_set in_label_ndx 0 eep_id 0 is_php 0\r\n"
    "  out_label 0 out_sys_port 0 is_valid 0\r\n"
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
    PARAM_SWEEPT20_API_MPLS_LSR_LABEL_SET_LSR_LABEL_SET_INFO_OUT_SYS_PORT_ID,
    "out_sys_port",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.out_sys_port:\r\n"
    "  Out System port ID / LAG / Multicast IDRange 0-(32K-1).\r\n"
    "",
    "Examples:\r\n"
    "  timna_app mpls lsr_label_set in_label_ndx 0 eep_id 0 is_php 0\r\n"
    "  out_label 0 out_sys_port 0 is_valid 0\r\n"
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
    PARAM_SWEEPT20_API_MPLS_LSR_LABEL_SET_LSR_LABEL_SET_INFO_IS_VALID_ID,
    "is_valid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.is_valid:\r\n"
    "  Valid / Invalid In-Label.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app mpls lsr_label_set in_label_ndx 0 eep_id 0 is_php 0\r\n"
    "  out_label 0 out_sys_port 0 is_valid 0\r\n"
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
    PARAM_SWEEPT20_API_MPLS_LSR_LABEL_GET_LSR_LABEL_GET_ID,
    "lsr_label_get",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Per MPLS label in the ingress, set the forwarding and editing\r\n"
    "  information.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app mpls lsr_label_get in_label_ndx 0\r\n"
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
    PARAM_SWEEPT20_API_MPLS_LSR_LABEL_GET_LSR_LABEL_GET_IN_LABEL_NDX_ID,
    "in_label_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  in_label_ndx:\r\n"
    "  In MPLS Label ID. Range: 0 - (256K-1)\r\n"
    "",
    "Examples:\r\n"
    "  timna_app mpls lsr_label_get in_label_ndx 0\r\n"
    "",
    "",
    {BIT(27), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MPLS_ID,
    "mpls_lsr",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(26)|BIT(27), 0, 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif
#ifdef UI_SWEEPT20_API_VPLS
  {
    PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_ID,
    "vc_add",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Add a VC label to VSI, with user specified information.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vpls vc_add vsid_ndx 0 provider_port_ndx 0 plcr_id 0\r\n"
    "  eep 0 out_vc_label 0 in_vc_label 0\r\n"
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
    PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_VSID_NDX_ID,
    "vsid_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vsid_ndx:\r\n"
    "  Service ID. Range: 0 - (16K-1)\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vpls vc_add vsid_ndx 0 provider_port_ndx 0 plcr_id 0\r\n"
    "  eep 0 out_vc_label 0 in_vc_label 0\r\n"
    "",
    "",
    {BIT(28), 0, 0, 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_LAST_VSID_NDX_ID,
    "up_to_vsid_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vsid_ndx:\r\n"
    "  Last VSI\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vpls vc_add vsid_ndx 0 provider_port_ndx 0 plcr_id 0\r\n"
    "  eep 0 out_vc_label 0 in_vc_label 0\r\n"
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
    PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_PROVIDER_PORT_NDX_ID,
    "provider_port_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  provider_port_ndx:\r\n"
    "  The System port ID of the provider port.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vpls vc_add vsid_ndx 0 provider_port_ndx 0 plcr_id 0\r\n"
    "  eep 0 out_vc_label 0 in_vc_label 0\r\n"
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
    PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_VC_INFO_POLICER_ID_ID,
    "plcr_id",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vc_info.policer_id:\r\n"
    "  0 - 32K for policer pointer. SWEEPT20_API_DONT_SET_POLICER, indicates that the\r\n"
    "  policer is not affected\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vpls vc_add vsid_ndx 0 provider_port_ndx 0 plcr_id 0\r\n"
    "  eep 0 out_vc_label 0 in_vc_label 0\r\n"
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
    PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_VC_INFO_TUNNEL_ENCAP_ID,
    "eep",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vc_info.tunnel_encap:\r\n"
    "  Index of tunnel encapsulation information. The encapsulation pointer\r\n"
    "  attachment to information is done in the MPLS module\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vpls vc_add vsid_ndx 0 provider_port_ndx 0 plcr_id 0\r\n"
    "  eep 0 out_vc_label 0 in_vc_label 0\r\n"
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
    PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_VC_INFO_OUT_VC_LABEL_ID,
    "out_vc_label",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vc_info.out_vc_label:\r\n"
    "  Range: 0-(1M-1).The VC Label that the packet is stamped with on its way out.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vpls vc_add vsid_ndx 0 provider_port_ndx 0 plcr_id 0\r\n"
    "  eep 0 out_vc_label 0 in_vc_label 0\r\n"
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
    PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_VC_INFO_IN_VC_LABEL_ID,
    "in_vc_label_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vc_info.in_vc_label:\r\n"
    "  Range: 256K-(512K-1).The first 256K MPLS labels Are tunnel IDs\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vpls vc_add vsid_ndx 0 provider_port_ndx 0 plcr_id 0\r\n"
    "  eep 0 out_vc_label 0 in_vc_label 0\r\n"
    "",
    "",
    {BIT(28), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VPLS_VC_ADD_VC_ADD_VC_INFO_UP_TO_IN_VC_LABEL_ID,
    "nof_consecutive_vc_labels",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Enable inserting range of VC labels:\r\n"
    "  Each VSI will be opened with 'nof_consecutive_vc_labels' in_vc and out_vc labels\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vpls vc_add in_vc_label_ndx 4 vsid_ndx 0 provider_port_ndx 0 plcr_id 0\r\n"
    "  eep 0 out_vc_label 0 nof_consecutive_vc_labels \r\n"
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
    PARAM_SWEEPT20_API_VPLS_VC_REMOVE_VC_REMOVE_ID,
    "vc_remove",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove VC label.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vpls vc_remove vsid_ndx 0 provider_port_ndx 0 plcr_id 0\r\n"
    "  eep 0 out_vc_label 0 in_vc_label 0\r\n"
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
    PARAM_SWEEPT20_API_VPLS_VC_REMOVE_VC_REMOVE_VSID_NDX_ID,
    "vsid_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vsid_ndx:\r\n"
    "  Service ID. Range: 0 - (16K-1)\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vpls vc_remove vsid_ndx 0 provider_port_ndx 0 plcr_id 0\r\n"
    "  eep 0 out_vc_label 0 in_vc_label 0\r\n"
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
    PARAM_SWEEPT20_API_VPLS_VC_REMOVE_VC_REMOVE_PROVIDER_PORT_NDX_ID,
    "provider_port_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  provider_port_ndx:\r\n"
    "  The System port ID of the provider port.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vpls vc_remove vsid_ndx 0 provider_port_ndx 0 plcr_id 0\r\n"
    "  eep 0 out_vc_label 0 in_vc_label 0\r\n"
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
    PARAM_SWEEPT20_API_VPLS_VC_REMOVE_VC_REMOVE_VC_INFO_IN_VC_LABEL_ID,
    "in_vc_label",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vc_info.in_vc_label:\r\n"
    "  Range: 256K-(512K-1).The first 256K MPLS labels Are tunnel IDs\r\n"
    "",
    "Examples:\r\n"
    "  timna_app vpls vc_remove vsid_ndx 0 provider_port_ndx 0 plcr_id 0\r\n"
    "  eep 0 out_vc_label 0 in_vc_label 0\r\n"
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
#endif
#ifdef UI_SWEEPT20_API_BSP
  {
    PARAM_SWEEPT20_API_BSP_RESET_DEVICE_RESET_DEVICE_ID,
    "reset_device",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Perform HW reset to the device.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app bsp reset_device\r\n"
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
    PARAM_SWEEPT20_API_BSP_ID,
    "bsp",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(5), 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif
#ifdef UI_SWEEPT20_API_SYS_INFO
  {
    PARAM_SWEEPT20_API_SYS_INFO_GET_LOCAL_PORT_GET_LOCAL_PORT_ID,
    "get_local_port",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Return the local port related to the system port.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app sys_info get_local_port sys_port_id 0\r\n"
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
    PARAM_SWEEPT20_API_SYS_INFO_GET_LOCAL_PORT_GET_LOCAL_PORT_SYS_PORT_ID_ID,
    "sys_port_id",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_port_id:\r\n"
    "  System port ID\r\n"
    "",
    "Examples:\r\n"
    "  timna_app sys_info get_local_port sys_port_id 0\r\n"
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
    PARAM_SWEEPT20_API_SYS_INFO_ID,
    "sys_info",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(6)|BIT(7)|BIT(8), 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif
#ifdef UI_SWEEPT20_API_PROFILER
  {
    PARAM_SWEEPT20_API_PROFILER_IN_LABEL_TO_LIF_IN_LABEL_TO_LIF_ID,
    "vc_to_lif",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Map VC to Lif\r\n"
    "",
    "Examples:\r\n"
    "  timna_app profiler in_label_to_lif in_label 0\r\n"
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
    PARAM_SWEEPT20_API_PROFILER_IN_LABEL_TO_LIF_IN_LABEL_TO_LIF_IN_LABEL_ID,
    "in_label",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  in_label:\r\n"
    "  In MPLS Label.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app profiler in_label_to_lif in_label 0\r\n"
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
    PARAM_SWEEPT20_API_PROFILER_LIF_TO_IN_LABEL_LIF_TO_IN_LABEL_ID,
    "lif_to_in_label",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Map VC to Lif\r\n"
    "",
    "Examples:\r\n"
    "  timna_app profiler lif_to_in_label lif 0\r\n"
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
    PARAM_SWEEPT20_API_PROFILER_LIF_TO_IN_LABEL_LIF_TO_IN_LABEL_LIF_ID,
    "lif",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lif:\r\n"
    "  Logical Interface\r\n"
    "",
    "Examples:\r\n"
    "  timna_app profiler lif_to_in_label lif 0\r\n"
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
    PARAM_SWEEPT20_API_PROFILER_ID,
    "profiler",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(9)|BIT(10), 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif
#ifdef UI_SWEEPT20_API_8021AD
  {
    PARAM_SWEEPT20_API_8021AD_C_VID_REGISTER_C_VID_REGISTER_ID,
    "c_vid_register",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  - Register the customer VLAN c_vid_ndx to the C-component\r\n"
    "  identified by port_ndx.\r\n"
    "  - Add the Provider Edge Port identified by (port_ndx,s_vid)\r\n"
    "  to the member set of the C-VLAN component identified by\r\n"
    "  port_ndx.\r\n"
    "  - Associate the Customer VLAN Identifier c_vid_ndx with Provider\r\n"
    "  Edge Port identified by (port_ndx,s_vid).\r\n"
    "  - Maps from a Customer VLAN Identifier c_vid_ndx to service\r\n"
    "  instance Represented by a service VLAN Identifier s_vid for the\r\n"
    "  C-component identified by port_ndx.\r\n"
    "  - Configure the PVID of the internal Customer Network Port on the\r\n"
    "  S-VLAN component to s_vid.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq c_vid_register prt_ndx 0 c_vid_ndx 0 s_vid 0\r\n"
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
    PARAM_SWEEPT20_API_8021AD_C_VID_REGISTER_C_VID_REGISTER_PORT_NDX_ID,
    "prt_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_ndx:\r\n"
    "  local port, this port have to be CEP. Range 0-31.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq c_vid_register prt_ndx 0 c_vid_ndx 0 s_vid 0\r\n"
    "",
    "",
    {0, 0, BIT(0), 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_8021AD_C_VID_REGISTER_C_VID_REGISTER_C_VID_NDX_ID,
    "c_vid_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  c_vid_ndx:\r\n"
    "  The customer VID to register and associate with the PEP (port_ndx,s_vid).\r\n"
    "  Range 0-4095,TIMNA_QINQ_ALL_OTHERS_VLANS.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq c_vid_register prt_ndx 0 c_vid_ndx 0 s_vid 0\r\n"
    "",
    "",
    {0, 0, BIT(0), 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_8021AD_C_VID_REGISTER_C_VID_REGISTER_S_VID_ID,
    "s_vid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  s_vid:\r\n"
    "  the service-VID that the c-vid mapped to in the C-component identified by\r\n"
    "  (port_ndx). Range 0-4095.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq c_vid_register prt_ndx 0 c_vid_ndx 0 s_vid 0\r\n"
    "",
    "",
    {0, 0, BIT(0), 0},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_ID,
    "pep_info_set",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Sets general information (PVID, default user priority) for the PEP.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq pep_info_set port 0 svid 0 ingress_s_dei 0\r\n"
    "  ingress_s_usr_prrty 0 ingress_c_tggd 0 egress_c_usr_prrty 0\r\n"
    "  pep_pvid 0 egress_ccptd_frms 0\r\n"
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
    PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_PORT_ID_NDX_ID,
    "port",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  pep_id_ndx:\r\n"
    "  The port ID of the PEP.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq pep_info_set port 0 svid 0 ingress_s_dei 0\r\n"
    "  ingress_s_usr_prrty 0 ingress_c_tggd 0 egress_c_usr_prrty 0\r\n"
    "  pep_pvid 0 egress_ccptd_frms 0\r\n"
    "",
    "",
    {0, 0, BIT(1), 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_SVID_NDX_ID,
    "svid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  pep_id_ndx:\r\n"
    "  The svid of the PEP.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq pep_info_set port 0 svid 0 ingress_s_dei 0\r\n"
    "  ingress_s_usr_prrty 0 ingress_c_tggd 0 egress_c_usr_prrty 0\r\n"
    "  pep_pvid 0 egress_ccptd_frms 0\r\n"
    "",
    "",
    {0, 0, BIT(1), 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_INGRESS_S_DEI_ID,
    "ingress_s_dei",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  pep_info.ingress.s_dei:\r\n"
    "  If TRUE then the PEP sets the DEI bit when generating S-tag for the SVLAN\r\n"
    "  component.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq pep_info_set port 0 svid 0 ingress_s_dei 0\r\n"
    "  ingress_s_usr_prrty 0 ingress_c_tggd 0 egress_c_usr_prrty 0\r\n"
    "  pep_pvid 0 egress_ccptd_frms 0\r\n"
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
    PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_INGRESS_S_USER_PRIORITY_ID,
    "ingress_s_usr_prrty",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  pep_info.ingress.s_user_priority:\r\n"
    "  The User Priority of S-Tag when generating S-tag for the SVLAN component.\r\n"
    "  Range 0-7.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq pep_info_set port 0 svid 0 ingress_s_dei 0\r\n"
    "  ingress_s_usr_prrty 0 ingress_c_tggd 0 egress_c_usr_prrty 0\r\n"
    "  pep_pvid 0 egress_ccptd_frms 0\r\n"
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
    PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_INGRESS_C_TAGGED_ID,
    "ingress_c_tggd",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  pep_info.ingress.c_:\r\n"
    "  If TRUE then the PEP port sends packets with C-tag toward the S-component. If\r\n"
    "  incoming packet is un then it assigned VID according to VLAN\r\n"
    "  classification configuration, and UP and CFI according to COS configuration.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq pep_info_set port 0 svid 0 ingress_s_dei 0\r\n"
    "  ingress_s_usr_prrty 0 ingress_c_tggd 0 egress_c_usr_prrty 0\r\n"
    "  pep_pvid 0 egress_ccptd_frms 0\r\n"
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
    PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_EGRESS_C_USER_PRIORITY_ID,
    "egress_c_usr_prrty",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  pep_info.egress.c_user_priority:\r\n"
    "  The User Priority of C-Tag when generating C-tag in the PEP port toward the\r\n"
    "  C-Component 0-7.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq pep_info_set port 0 svid 0 ingress_s_dei 0\r\n"
    "  ingress_s_usr_prrty 0 ingress_c_tggd 0 egress_c_usr_prrty 0\r\n"
    "  pep_pvid 0 egress_ccptd_frms 0\r\n"
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
    PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_EGRESS_C_VID_ID,
    "pep_pvid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  pep_info.egress.c_vid:\r\n"
    "  The Vlan ID of C-Tag when generating C-tag in the PEP port toward the\r\n"
    "  C-Component 0-7.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq pep_info_set port 0 svid 0 ingress_s_dei 0\r\n"
    "  ingress_s_usr_prrty 0 ingress_c_tggd 0 egress_c_usr_prrty 0\r\n"
    "  pep_pvid 0 egress_ccptd_frms 0\r\n"
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
    PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_EGRESS_C_TAGGED_ID,
    "egress_c_tggd",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  pep_info.egress.c_:\r\n"
    "  If TRUE then the PEP port sends packets with C-tag toward the C-component. If\r\n"
    "  incoming packet is un then it assigned VID according with 'c_vid'.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq pep_info_set port 0 svid 0 ingress_s_dei 0\r\n"
    "  ingress_s_usr_prrty 0 ingress_c_tggd 0 egress_c_usr_prrty 0\r\n"
    "  pep_pvid 0 egress_ccptd_frms 0\r\n"
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
    PARAM_SWEEPT20_API_8021AD_PEP_INFO_SET_PEP_INFO_SET_PEP_INFO_EGRESS_ACCEPTED_FRAMES_ID,
    "egress_ccptd_frms",
    (PARAM_VAL_RULES *)&TIMNA_FILTER_ACCEPT_FRAME_TYPE_rule[0],
    (sizeof(TIMNA_FILTER_ACCEPT_FRAME_TYPE_rule) / sizeof(TIMNA_FILTER_ACCEPT_FRAME_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  pep_info.egress.accepted_frames:\r\n"
    "  Determines which frames are accepted and which are filtered (, un\r\n"
    "  and Priority-Tagged)\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq pep_info_set port 0 svid 0 ingress_s_dei 0\r\n"
    "  ingress_s_usr_prrty 0 ingress_c_tggd 0 egress_c_usr_prrty 0\r\n"
    "  pep_pvid 0 egress_ccptd_frms 0\r\n"
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
    PARAM_SWEEPT20_API_8021AD_PEP_INFO_GET_PEP_INFO_GET_ID,
    "pep_info_get",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Sets general information (PVID, default user priority) for the PEP.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq pep_info_get pep_id_ndx 0\r\n"
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
    PARAM_SWEEPT20_API_8021AD_PEP_INFO_GET_PEP_INFO_GET_PORT_ID_NDX_ID,
    "port",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port:\r\n"
    "  The port ID of the PEP.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq pep_info_get port 0 s_vid 0\r\n"
    "",
    "",
    {0, 0, BIT(2), 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
    {
    PARAM_SWEEPT20_API_8021AD_PEP_INFO_GET_PEP_INFO_GET_SVID_NDX_ID,
    "s_vid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  s_vid:\r\n"
    "  The s_vid of the PEP.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq pep_info_get port 0 s_vid 0\r\n"
    "",
    "",
    {0, 0, BIT(2), 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_8021AD_CEP_INFO_SET_CEP_INFO_SET_ID,
    "cep_info_set",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Sets general information (PVID, default user priority) for the CEP.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq cep_info_set cep_id_ndx 0 cvid 0 egress_c_tggd 0\r\n"
    "",
    "",
    {0, 0, BIT(7)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_8021AD_CEP_INFO_SET_CEP_INFO_SET_CEP_ID_NDX_ID,
    "cep_id_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  cep_id_ndx:\r\n"
    "  The ID of the PEP.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq cep_info_set cep_id_ndx 0 cvid 0 egress_c_tggd 0\r\n"
    "",
    "",
    {0, 0, BIT(7)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_8021AD_CEP_INFO_SET_CEP_INFO_SET_CEP_VLAN_ID_NDX_ID,
    "cvid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  cep_id_ndx:\r\n"
    "  The cvid .\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq cep_info_set cep_id_ndx 0 cvid 0 egress_c_tggd 0\r\n"
    "",
    "",
    {0, 0, BIT(7)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_8021AD_CEP_INFO_SET_CEP_INFO_SET_CEP_INFO_ID_C_TAG_ID,
    "egress_c_tggd",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  cep_info.egress.c_:\r\n"
    "  If TRUE then the CEP port sends packets with C-tag toward the Customer. \r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq cep_info_set cep_id_ndx 0 cvid 0 egress_c_tggd 0\r\n"
    "",
    "",
    {0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_8021AD_CEP_INFO_GET_CEP_INFO_GET_ID,
    "cep_info_get",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Sets general information (PVID, default user priority) for the CEP.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq cep_info_get cep_id_ndx 0 cvid 0 \r\n"
    "",
    "",
    {0, 0, BIT(8)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_8021AD_CEP_INFO_GET_CEP_INFO_GET_ID_CEP_ID_NDX,
    "cep_id_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  cep_id_ndx:\r\n"
    "  The ID of the PEP.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq cep_info_get cep_id_ndx 0 cvid 0 \r\n"
    "",
    "",
    {0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_8021AD_CEP_INFO_GET_CEP_INFO_GET_CEP_VLAN_ID_NDX_ID,
    "cvid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  cvid:\r\n"
    "  the cvlan id .\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq cep_info_get cep_id_ndx 0 cvid 0 \r\n"
    "",
    "",
    {0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_8021AD_C_VID_UNREGISTER_C_VID_UNREGISTER_ID,
    "c_vid_unregister",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_ndx:\r\n"
    "  local port, this port have to be CEP. Range 0-31.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq c_vid_unregister prt_ndx 0 c_vid_ndx 0\r\n"
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
    PARAM_SWEEPT20_API_8021AD_C_VID_UNREGISTER_C_VID_UNREGISTER_PORT_NDX_ID,
    "prt_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_ndx:\r\n"
    "  local port, this port have to be CEP. Range 0-31.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq c_vid_unregister prt_ndx 0 c_vid_ndx 0\r\n"
    "",
    "",
    {0, 0, BIT(3), 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_8021AD_C_VID_UNREGISTER_C_VID_UNREGISTER_C_VID_NDX_ID,
    "c_vid_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  c_vid_ndx:\r\n"
    "  The customer VID that to be detached from the C-component\r\n"
    "  port_ndx and the proper PEP (port_ndx,s_vid). Range 0-4095.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq c_vid_unregister prt_ndx 0 c_vid_ndx 0\r\n"
    "",
    "",
    {0, 0, BIT(3), 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_8021AD_PEP_REGISTERATION_STATUS_GET_ID,
    "pep_registeration_status_get",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  - Gets the C-VIDs that are associated with the PEP\r\n"
    "  (port_ndx,s_vid).\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq pep_registeration_status_get pep_id_ndx 0\r\n"
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
    PARAM_SWEEPT20_API_8021AD_PEP_REGISTERATION_STATUS_GET_PEP_PORT_ID_NDX_ID,
    "port",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  pep_id_ndx:\r\n"
    "  Pep port id.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq pep_registeration_status_get port 0 s_vid 0 max_members 100 iter 5\r\n"
    "",
    "",
    {0, 0, BIT(6), 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_8021AD_PEP_REGISTERATION_STATUS_GET_PEP_SVID_NDX_ID,
    "s_vid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  s_vid:\r\n"
    "  Pep s_vid.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq pep_registeration_status_get port 0 s_vid 0 max_members 100 iter 5\r\n"
    "",
    "",
    {0, 0, BIT(6), 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_8021AD_PEP_REGISTERATION_STATUS_GET_MAX_MEMBERS_ID,
    "max_members",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  max_members:\r\n"
    "  number of members to get.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq pep_registeration_status_get port 0 s_vid 0 max_members 100 iter 5\r\n"
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
    PARAM_SWEEPT20_API_8021AD_PEP_REGISTERATION_STATUS_GET_ITER_ID,
    "iter",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  iter:\r\n"
    "  from which member to start getting the members.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq pep_registeration_status_get port 0 s_vid 0 max_members 100 iter 5\r\n"
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
    PARAM_SWEEPT20_API_8021AD_C_VID_REGISTERATION_STATUS_GET_C_VID_REGISTERATION_STATUS_GET_ID,
    "c_vid_registeration_status_get",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  - Gets the service identifier (s_vid) that the Customer\r\n"
    "  VLAN(c_vid) mapped to. If c_vid_ndx is not registered in this C-\r\n"
    "  component then registered is FALSE and s_vid unaffected. \r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq c_vid_registeration_status_get prt_ndx 0 c_vid_ndx 0\r\n"
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
    PARAM_SWEEPT20_API_8021AD_C_VID_REGISTERATION_STATUS_GET_C_VID_REGISTERATION_STATUS_GET_PORT_NDX_ID,
    "prt_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_ndx:\r\n"
    "  local port, this port have to be CEP. Range 0-31.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq c_vid_registeration_status_get prt_ndx 0 c_vid_ndx 0\r\n"
    "",
    "",
    {0, 0, BIT(5), 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_8021AD_C_VID_REGISTERATION_STATUS_GET_C_VID_REGISTERATION_STATUS_GET_C_VID_NDX_ID,
    "c_vid_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  c_vid_ndx:\r\n"
    "  The Customer VLAN ID. Range 0-4095.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app qinq c_vid_registeration_status_get prt_ndx 0 c_vid_ndx 0\r\n"
    "",
    "",
    {0, 0, BIT(5), 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_8021AD_ID,
    "qinq",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(5)|BIT(6)|BIT(7)|BIT(8)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif
#ifdef UI_SWEEPT20_API_FILTER
 {
    PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_ID,
    "port_info_set",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the per port filtering information.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app filter port_info_set prt_ndx 0 unknown_da_unknown_bc 0\r\n"
    "  unknown_da_unknown_mc 0 unknown_da_unknown_uc 0\r\n"
    "  not_authorized 0 egress_fltr 0 frwrd_action_typs 0\r\n"
    "  ccptd_frms 0\r\n"
    "",
    "",
    {0, 0, BIT(4), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_NDX_ID,
    "prt_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_ndx:\r\n"
    "  Local port ID. Range 0-31.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app filter port_info_set prt_ndx 0 unknown_da_unknown_bc 0\r\n"
    "  unknown_da_unknown_mc 0 unknown_da_unknown_uc 0\r\n"
    "  not_authorized 0 egress_fltr 0 frwrd_action_typs 0\r\n"
    "  ccptd_frms 0\r\n"
    "",
    "",
    {0, 0, BIT(4), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_UNKNOWN_DA_UNKNOWN_BC_ID,
    "unknown_da_unknown_bc",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_info.unknown_da.unknown_bc:\r\n"
    "  If TRUE drop packets with unknown Broadcast destination address. \r\n"
    "",
    "Ex:\r\n"
    "  timna_app filter port_info_set prt_ndx 0 unknown_da_unknown_bc 0\r\n"
    "  unknown_da_unknown_mc 0 unknown_da_unknown_uc 0\r\n"
    "  not_authorized 0 egress_fltr 0 frwrd_action_typs 0\r\n"
    "  ccptd_frms 0\r\n"
    "",
    "",
    {0, 0, BIT(4), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_UNKNOWN_DA_UNKNOWN_MC_ID,
    "unknown_da_unknown_mc",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_info.unknown_da.unknown_mc:\r\n"
    "  If TRUE drop packets with unknown Multicast destination address. \r\n"
    "",
    "Ex:\r\n"
    "  timna_app filter port_info_set prt_ndx 0 unknown_da_unknown_bc 0\r\n"
    "  unknown_da_unknown_mc 0 unknown_da_unknown_uc 0\r\n"
    "  not_authorized 0 egress_fltr 0 frwrd_action_typs 0\r\n"
    "  ccptd_frms 0\r\n"
    "",
    "",
    {0, 0, BIT(4), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_UNKNOWN_DA_UNKNOWN_UC_ID,
    "unknown_da_unknown_uc",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_info.unknown_da.unknown_uc:\r\n"
    "  If TRUE drop packets with unknown Unicast destination address. \r\n"
    "",
    "Ex:\r\n"
    "  timna_app filter port_info_set prt_ndx 0 unknown_da_unknown_bc 0\r\n"
    "  unknown_da_unknown_mc 0 unknown_da_unknown_uc 0\r\n"
    "  not_authorized 0 egress_fltr 0 frwrd_action_typs 0\r\n"
    "  ccptd_frms 0\r\n"
    "",
    "",
    {0, 0, BIT(4), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_AUTHORIZATION_INFO_NOT_AUTHORIZED_ID,
    "not_authorized",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_info.authorization_info.not_authorized:\r\n"
    "  If TRUE, then port operates in non-authorized 802.1x mode and only 802.1x\r\n"
    "  traffic is allowed. (With Ethertype equal 0x888e according to IEEE Std 802.1X\r\n"
    "  - Port-based network access. May be changed by logical access.)If FALSE, then\r\n"
    "  the port is authorized and all traffic is allowed.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app filter port_info_set prt_ndx 0 unknown_da_unknown_bc 0\r\n"
    "  unknown_da_unknown_mc 0 unknown_da_unknown_uc 0\r\n"
    "  not_authorized 0 egress_fltr 0 frwrd_action_typs 0\r\n"
    "  ccptd_frms 0\r\n"
    "",
    "",
    {0, 0, BIT(4), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_EGRESS_FILTER_ID,
    "egress_fltr",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_info.egress_filter:\r\n"
    "  Enable egress filtering.Confirms that the port is a member of the VID to\r\n"
    "  which the packet is classified.If not the packet is dropped\r\n"
    "",
    "Ex:\r\n"
    "  timna_app filter port_info_set prt_ndx 0 unknown_da_unknown_bc 0\r\n"
    "  unknown_da_unknown_mc 0 unknown_da_unknown_uc 0\r\n"
    "  not_authorized 0 egress_fltr 0 frwrd_action_typs 0\r\n"
    "  ccptd_frms 0\r\n"
    "",
    "",
    {0, 0, BIT(4), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_FRWRD_ACTION_TYPES_ID,
    "frwrd_action_typs",
    (PARAM_VAL_RULES *)&TIMNA_FILTER_ACCEPT_FRWRD_ACTION_TYPE_rule[0],
    (sizeof(TIMNA_FILTER_ACCEPT_FRWRD_ACTION_TYPE_rule) / sizeof(TIMNA_FILTER_ACCEPT_FRWRD_ACTION_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_info.frwrd_action_types:\r\n"
    "  Determines which packets to filter according to their forwarding action type:\r\n"
    "  control or 'intercept or snoop'.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app filter port_info_set prt_ndx 0 unknown_da_unknown_bc 0\r\n"
    "  unknown_da_unknown_mc 0 unknown_da_unknown_uc 0\r\n"
    "  not_authorized 0 egress_fltr 0 frwrd_action_typs 0\r\n"
    "  ccptd_frms 0\r\n"
    "",
    "",
    {0, 0, BIT(4), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_FILTER_PORT_INFO_SET_PORT_INFO_SET_PORT_INFO_ACCEPTED_FRAMES_ID,
    "ccptd_frms",
    (PARAM_VAL_RULES *)&TIMNA_FILTER_ACCEPT_FRAME_TYPE_rule[0],
    (sizeof(TIMNA_FILTER_ACCEPT_FRAME_TYPE_rule) / sizeof(TIMNA_FILTER_ACCEPT_FRAME_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_info.accepted_frames:\r\n"
    "  Determines which frames are accepted and which are filtered (, un\r\n"
    "  and Priority-Tagged)\r\n"
    "",
    "Ex:\r\n"
    "  timna_app filter port_info_set prt_ndx 0 unknown_da_unknown_bc 0\r\n"
    "  unknown_da_unknown_mc 0 unknown_da_unknown_uc 0\r\n"
    "  not_authorized 0 egress_fltr 0 frwrd_action_typs 0\r\n"
    "  ccptd_frms 0\r\n"
    "",
    "",
    {0, 0, BIT(4), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_FILTER_ID,
    "filter",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(4), 0, 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif
#ifdef UI_SWEEPT20_API_EDIT/* { edit*/
  {
    PARAM_SWEEPT20_EDIT_PORT_INFO_SET_PORT_INFO_SET_ID,
    "vsid_port_info_set",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Sets the Editing information for VBP port and a VLAN.\r\n"
    "",
    "Ex:\r\n"
    "  timna_api edit vsid_port_info_set vsd_ndx 0 prt_ndx 0 tggd 0\r\n"
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
    PARAM_SWEEPT20_EDIT_PORT_INFO_SET_PORT_INFO_SET_PORT_NDX_ID,
    "prt_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_ndx:\r\n"
    "  Local port ID. Range 0-31.\r\n"
    "",
    "Ex:\r\n"
    "  timna_api edit port_info_set prt_ndx 0 vsd_ndx 0 tggd 0\r\n"
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
    PARAM_SWEEPT20_EDIT_PORT_INFO_SET_PORT_INFO_SET_VSID_NDX_ID,
    "vsd_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vsid_ndx:\r\n"
    "  the VLAN ID. Range 0-4095.\r\n"
    "",
    "Ex:\r\n"
    "  timna_api edit vsid_port_info_set vsd_ndx 0 prt_ndx 0 tggd 0\r\n"
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
    PARAM_SWEEPT20_EDIT_PORT_INFO_SET_PORT_INFO_SET_TAGGED_ID,
    "tggd",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  :\r\n"
    "  If the port send the packets .\r\n"
    "",
    "Ex:\r\n"
    "  timna_api edit vsid_port_info_set vsd_ndx 0 prt_ndx 0 tggd 0\r\n"
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
    PARAM_SWEEPT20_EDIT_PORT_INFO_GET_PORT_INFO_GET_ID,
    "vsid_port_info_get",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Sets the Editing information for VBP port and a VLAN.\r\n"
    "",
    "Ex:\r\n"
    "  timna_api edit vsid_port_info_set vsd_ndx 0 prt_ndx 0 tggd 0\r\n"
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
    PARAM_SWEEPT20_EDIT_PORT_INFO_GET_PORT_INFO_GET_PORT_NDX_ID,
    "prt_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_ndx:\r\n"
    "  Local port ID. Range 0-31.\r\n"
    "",
    "Ex:\r\n"
    "  timna_api edit vsid_port_info_set vsd_ndx 0 prt_ndx 0 tggd 0\r\n"
    "",
    "",
    {0, 0, BIT(21), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_EDIT_PORT_INFO_GET_PORT_INFO_GET_VSID_NDX_ID,
    "vsd_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vsid_ndx:\r\n"
    "  the VLAN ID. Range 0-4095.\r\n"
    "",
    "Ex:\r\n"
    "  timna_api edit vsid_port_info_set vsd_ndx 0 prt_ndx 0 tggd 0\r\n"
    "",
    "",
    {0, 0, BIT(21), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_EDIT_ID,
    "edit",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(20)|BIT(21), 0, 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } edit*/

#ifdef UI_SWEEPT20_API_STATUS
  {
    PARAM_SWEEPT20_API_STATUS_GET_ID,
    "status_get",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get various system on-the-fly statuses.\r\n"
    "",
    "Examples:\r\n"
    "  timna_app status_get\r\n"
    "",
    "",
    {0, 0, 0, BIT(31)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif
#ifdef UI_SWEEPT20_API_MACT
  {
    PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_ID,
    "get_block",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get block of entries from the MACT, starting from start_indx, till the first\r\n"
    "  of these is happened-\r\n"
    "  Reach End of the table.-\r\n"
    "  'entries_to_scan' entries are examined.-\r\n"
    "  'entries_to_act' entries are retrieved after matching the given rule. Only\r\n"
    "  valid entries are considered.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup get_block frz_mct 0 rng_ntrs_to_act 0\r\n"
    "  rng_ntrs_to_scn 0 rng_strt_ndx 0 rul_ncludng_sttc 0 rul_ncludng_dynmc 0 rul_dst_dst_val 0\r\n"
    "  rul_dst_dst_typ 0 rul_cmpr_dst 0 rul_fid 0 rul_cmpr_fid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(6), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_FREEZE_MACT_ID,
    "frz_mct",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  freeze_mact:\r\n"
    "  If TRUE Before getting the block Stop the activity of the device on the MACT\r\n"
    "  so keeping the MACT unchanged. On completion the function restores the device\r\n"
    "  activity status as it was before calling this function.No supported, ignored.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup get_block frz_mct 0 rng_ntrs_to_act 0\r\n"
    "  rng_ntrs_to_scn 0 rng_strt_ndx 0 rul_ncludng_sttc 0 rul_ncludng_dynmc 0 rul_dst_dst_val 0\r\n"
    "  rul_dst_dst_typ 0 rul_cmpr_dst 0 rul_fid 0 rul_cmpr_fid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RANGE_ENTRIES_TO_ACT_ID,
    "rng_ntrs_to_act",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range.entries_to_act:\r\n"
    "  Number of entries to read/modify.Stop After retrieving this number of entries.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup get_block frz_mct 0 rng_ntrs_to_act 0\r\n"
    "  rng_ntrs_to_scn 0 rng_strt_ndx 0 rul_ncludng_sttc 0 rul_ncludng_dynmc 0 rul_dst_dst_val 0\r\n"
    "  rul_dst_dst_typ 0 rul_cmpr_dst 0 rul_fid 0 rul_cmpr_fid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RANGE_ENTRIES_TO_SCAN_ID,
    "rng_ntrs_to_scn",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range.entries_to_scan:\r\n"
    "  The number of entries to scan.Stop After scanning this number of entries.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup get_block frz_mct 0 rng_ntrs_to_act 0\r\n"
    "  rng_ntrs_to_scn 0 rng_strt_ndx 0 rul_ncludng_sttc 0 rul_ncludng_dynmc 0 rul_dst_dst_val 0\r\n"
    "  rul_dst_dst_typ 0 rul_cmpr_dst 0 rul_fid 0 rul_cmpr_fid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RANGE_START_INDX_ID,
    "rng_strt_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range.start_indx:\r\n"
    "  The index to start reading from in the MACT\r\n"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup get_block frz_mct 0 rng_ntrs_to_act 0\r\n"
    "  rng_ntrs_to_scn 0 rng_strt_ndx 0 rul_ncludng_sttc 0 rul_ncludng_dynmc 0 rul_dst_dst_val 0\r\n"
    "  rul_dst_dst_typ 0 rul_cmpr_dst 0 rul_fid 0 rul_cmpr_fid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
    {
    PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RULE_FLUSH_STATIC_ID,
    "rul_ncludng_sttc",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  rule.flush_sttc:\r\n"
    "  If TRUE then static entries are in match"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup get_block frz_mct 0 rng_ntrs_to_act 0\r\n"
    "  rng_ntrs_to_scn 0 rng_strt_ndx 0 rul_ncludng_sttc 0 rul_ncludng_dynmc 0 rul_dst_dst_val 0\r\n"
    "  rul_dst_dst_typ 0 rul_cmpr_dst 0 rul_fid 0 rul_cmpr_fid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RULE_FLUSH_DYNAMIC_ID,
    "rul_ncludng_dynmc",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  rule.flush_static:\r\n"
    "  If TRUE then dynamic entries are in match"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup get_block frz_mct 0 rng_ntrs_to_act 0\r\n"
    "  rng_ntrs_to_scn 0 rng_strt_ndx 0 rul_ncludng_sttc 0 rul_ncludng_dynmc 0 rul_dst_dst_val 0\r\n"
    "  rul_dst_dst_typ 0 rul_cmpr_dst 0 rul_fid 0 rul_cmpr_fid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RULE_DEST_DEST_VAL_ID,
    "rul_dst_dst_val",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  rule.dest.dest_val:\r\n"
    "  The destination value, according to the type (Single, RLAG or GLAG,LIF...)For\r\n"
    "  LAG the value is the group ID.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup get_block frz_mct 0 rng_ntrs_to_act 0\r\n"
    "  rng_ntrs_to_scn 0 rng_strt_ndx 0 rul_ncludng_sttc 0 rul_ncludng_dynmc 0 rul_dst_dst_val 0\r\n"
    "  rul_dst_dst_typ 0 rul_cmpr_dst 0 rul_fid 0 rul_cmpr_fid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RULE_DEST_DEST_TYPE_ID,
    "rul_dst_dst_typ",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  rule.dest.dest_type:\r\n"
    "  The type of the following destination ID\r\n"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup get_block frz_mct 0 rng_ntrs_to_act 0\r\n"
    "  rng_ntrs_to_scn 0 rng_strt_ndx 0 rul_ncludng_sttc 0 rul_ncludng_dynmc 0 rul_dst_dst_val 0\r\n"
    "  rul_dst_dst_typ 0 rul_cmpr_dst 0 rul_fid 0 rul_cmpr_fid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RULE_COMPARE_DEST_ID,
    "rul_cmpr_dst",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  rule.compare_dest:\r\n"
    "  Should the destination in the MAC table be compared with the destination in\r\n"
    "  this structure.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup get_block frz_mct 0 rng_ntrs_to_act 0\r\n"
    "  rng_ntrs_to_scn 0 rng_strt_ndx 0 rul_ncludng_sttc 0 rul_dst_dst_val 0\r\n"
    "  rul_dst_dst_typ 0 rul_cmpr_dst 0 rul_fid 0 rul_cmpr_fid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RULE_FID_ID,
    "rul_fid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  rule.fid:\r\n"
    "  When 'compare_fid' is asserted, the FID in the MACT is compared with this\r\n"
    "  value. \r\n"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup get_block frz_mct 0 rng_ntrs_to_act 0\r\n"
    "  rng_ntrs_to_scn 0 rng_strt_ndx 0 rul_ncludng_sttc 0 rul_dst_dst_val 0\r\n"
    "  rul_dst_dst_typ 0 rul_cmpr_dst 0 rul_fid 0 rul_cmpr_fid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_ENTRY_GET_BLOCK_MACT_ENTRY_GET_BLOCK_RULE_COMPARE_FID_ID,
    "rul_cmpr_fid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  rule.compare_fid:\r\n"
    "  Should the FID in the MACT be compared with the FID in this structure\r\n"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup get_block frz_mct 0 rng_ntrs_to_act 0\r\n"
    "  rng_ntrs_to_scn 0 rng_strt_ndx 0 rul_ncludng_sttc 0 rul_dst_dst_val 0\r\n"
    "  rul_dst_dst_typ 0 rul_cmpr_dst 0 rul_fid 0 rul_cmpr_fid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(6), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_ID,
    "flush",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Performs Flush Action (clear or transplant) to entries that fulfill a Rule.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup flush new_dst_dst_val 0\r\n"
    "  new_dst_dst_typ 0 rul_flsh_sttc 0 rul_flsh_dynmc 0 rul_dst_dst_val\r\n"
    "  0 rul_dst_dst_typ 0 rul_cmpr_dst 0 rul_fid 0\r\n"
    "  rul_cmpr_fid 0 mod 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(7), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_NEW_DEST_DEST_VAL_ID,
    "new_dst_dst_val",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.new_dest.dest_val:\r\n"
    "  The destination value, according to the type (Single, RLAG or GLAG,LIF...)For\r\n"
    "  LAG the value is the group ID.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup flush new_dst_dst_val 0\r\n"
    "  new_dst_dst_typ 0 rul_flsh_sttc 0 rul_flsh_dynmc 0 rul_dst_dst_val\r\n"
    "  0 rul_dst_dst_typ 0 rul_cmpr_dst 0 rul_fid 0\r\n"
    "  rul_cmpr_fid 0 mod 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(7), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_NEW_DEST_DEST_TYPE_ID,
    "new_dst_dst_typ",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.new_dest.dest_type:\r\n"
    "  The type of the following destination ID\r\n"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup flush new_dst_dst_val 0\r\n"
    "  new_dst_dst_typ 0 rul_flsh_sttc 0 rul_flsh_dynmc 0 rul_dst_dst_val\r\n"
    "  0 rul_dst_dst_typ 0 rul_cmpr_dst 0 rul_fid 0\r\n"
    "  rul_cmpr_fid 0 mod 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(7), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_RULE_FLUSH_STATIC_ID,
    "rul_flsh_sttc",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.rule.flush_static:\r\n"
    "  If TRUE then both static and dynamic entries match. If FALSE then only\r\n"
    "  dynamic entries are match.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup flush new_dst_dst_val 0\r\n"
    "  new_dst_dst_typ 0 rul_flsh_sttc 0 rul_flsh_dynmc 0 rul_dst_dst_val\r\n"
    "  0 rul_dst_dst_typ 0 rul_cmpr_dst 0 rul_fid 0\r\n"
    "  rul_cmpr_fid 0 mod 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(7), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_RULE_FLUSH_STATIC_ID,
    "rul_flsh_sttc",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.rule.flush_static:\r\n"
    "  If TRUE then statuc entries match.    ",
    "Ex:\r\n"
    "  timna_app mac_lkup flush new_dst_dst_val 0\r\n"
    "  new_dst_dst_typ 0 rul_flsh_sttc 0 rul_flsh_dynmc 0 rul_dst_dst_val\r\n"
    "  0 rul_dst_dst_typ 0 rul_cmpr_dst 0 rul_fid 0\r\n"
    "  rul_cmpr_fid 0 mod 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(7), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_RULE_FLUSH_DYNAMIC_ID,
    "rul_flsh_dynmc",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.rule.flush_static:\r\n"
    "  If TRUE then dynamic entries match."
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup flush new_dst_dst_val 0\r\n"
    "  new_dst_dst_typ 0 rul_flsh_sttc 0 rul_flsh_dynmc 0 rul_dst_dst_val\r\n"
    "  0 rul_dst_dst_typ 0 rul_cmpr_dst 0 rul_fid 0\r\n"
    "  rul_cmpr_fid 0 mod 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(7), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_RULE_DEST_DEST_VAL_ID,
    "rul_dst_dst_val",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.rule.dest.dest_val:\r\n"
    "  The destination value, according to the type (Single, RLAG or GLAG,LIF...)For\r\n"
    "  LAG the value is the group ID.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup flush new_dst_dst_val 0\r\n"
    "  new_dst_dst_typ 0 rul_flsh_sttc 0  rul_flsh_dynmc 0rul_dst_dst_val\r\n"
    "  0 rul_dst_dst_typ 0 rul_cmpr_dst 0 rul_fid 0\r\n"
    "  rul_cmpr_fid 0 mod 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(7), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_RULE_DEST_DEST_TYPE_ID,
    "rul_dst_dst_typ",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.rule.dest.dest_type:\r\n"
    "  The type of the following destination ID\r\n"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup flush new_dst_dst_val 0\r\n"
    "  new_dst_dst_typ 0 rul_flsh_sttc 0 rul_flsh_dynmc 0 rul_dst_dst_val\r\n"
    "  0 rul_dst_dst_typ 0 rul_cmpr_dst 0 rul_fid 0\r\n"
    "  rul_cmpr_fid 0 mod 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(7), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_RULE_COMPARE_DEST_ID,
    "rul_cmpr_dst",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.rule.compare_dest:\r\n"
    "  Should the destination in the MAC table be compared with the destination in\r\n"
    "  this structure.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup flush new_dst_dst_val 0\r\n"
    "  new_dst_dst_typ 0 rul_flsh_sttc 0 rul_flsh_dynmc 0 rul_dst_dst_val\r\n"
    "  0 rul_dst_dst_typ 0 rul_cmpr_dst 0 rul_fid 0\r\n"
    "  rul_cmpr_fid 0 mod 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(7), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_RULE_FID_ID,
    "rul_fid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.rule.fid:\r\n"
    "  When 'compare_fid' is asserted, the FID in the MACT is compared with this\r\n"
    "  value. \r\n"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup flush new_dst_dst_val 0\r\n"
    "  new_dst_dst_typ 0 rul_flsh_sttc 0 rul_flsh_dynmc 0 rul_dst_dst_val\r\n"
    "  0 rul_dst_dst_typ 0 rul_cmpr_dst 0 rul_fid 0\r\n"
    "  rul_cmpr_fid 0 mod 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(7), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_RULE_COMPARE_FID_ID,
    "rul_cmpr_fid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flush_info.rule.compare_fid:\r\n"
    "  Should the FID in the MACT be compared with the FID in this structure\r\n"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup flush new_dst_dst_val 0\r\n"
    "  new_dst_dst_typ 0 rul_flsh_sttc 0 rul_flsh_dynmc 0 rul_dst_dst_val\r\n"
    "  0 rul_dst_dst_typ 0 rul_cmpr_dst 0 rul_fid 0\r\n"
    "  rul_cmpr_fid 0 mod 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(7), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_FLUSH_MACT_FLUSH_FLUSH_INFO_MODE_ID,
    "mod",
    (PARAM_VAL_RULES *)&TIMNA_MACT_FLUSH_MODE_rule[0],
    (sizeof(TIMNA_MACT_FLUSH_MODE_rule) / sizeof(TIMNA_MACT_FLUSH_MODE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,

    0,0,0,0,0,0,0,
    "  flush_info.mode:\r\n"
    "  MACT flush mode, Clear, Transplant, Clear All, Transplant All.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup flush new_dst_dst_val 0\r\n"
    "  new_dst_dst_typ 0 rul_flsh_sttc 0 rul_flsh_dynmc 0 rul_dst_dst_val\r\n"
    "  0 rul_dst_dst_typ 0 rul_cmpr_dst 0 rul_fid 0\r\n"
    "  rul_cmpr_fid 0 mod 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(7), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_AGING_INFO_SET_MACT_AGING_INFO_SET_ID,
    "aging_info_set",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Sets the aging info\r\n"
    "  - enable aging\r\n"
    "  - cycle, the time to start traverses the MACT entries for aging.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup aging_info_set cycl_nno_sec 0\r\n"
    "  cycl_sec 0 nbl_gng 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(8), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_AGING_INFO_SET_MACT_AGING_INFO_SET_AGING_INFO_CYCLE_NANO_SEC_ID,
    "cycl_nno_sec",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  aging_info.cycle.nano_sec:\r\n"
    "  nano-seconds.Range 0-999,999,999\r\n"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup aging_info_set cycl_nno_sec 0\r\n"
    "  cycl_sec 0 nbl_gng 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(8), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_AGING_INFO_SET_MACT_AGING_INFO_SET_AGING_INFO_CYCLE_SEC_ID,
    "cycl_sec",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  aging_info.cycle.sec:\r\n"
    "  seconds\r\n"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup aging_info_set cycl_nno_sec 0\r\n"
    "  cycl_sec 0 nbl_gng 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(8), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MACT_AGING_INFO_SET_MACT_AGING_INFO_SET_AGING_INFO_ENABLE_AGING_ID,
    "nbl_gng",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  aging_info.enable_aging:\r\n"
    "  If TRUE then enable the aging process of the device (aging machine).\r\n"
    "",
    "Ex:\r\n"
    "  timna_app mac_lkup aging_info_set cycl_nno_sec 0\r\n"
    "  cycl_sec 0 nbl_gng 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(8), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_MAC_LKUP_ID,
    "mac_lkup",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, 0, BIT(6)|BIT(7)|BIT(8), 0, 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif
#ifdef UI_SWEEPT20_API_VID_RES
  {
    PARAM_SWEEPT20_API_VID_PORT_INFO_SET_VID_PORT_INFO_SET_ID,
    "vid_port_info_set",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the port information (which vid resolution algorithms to perform for this\r\n"
    "  port and default pvid) for the VLAN ID resolution module.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vid_res vid_port_info_set port_ndx 0 enable_portcl 0\r\n"
    "  enable_sbnt_ip 0 pvid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(15), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VID_PORT_INFO_SET_VID_PORT_INFO_SET_PORT_NDX_ID,
    "port_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_ndx:\r\n"
    "  Local physical port ID. Range 0-31.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vid_res vid_port_info_set port_ndx 0 enable_portcl 0\r\n"
    "  enable_sbnt_ip 0 pvid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(15), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VID_PORT_INFO_SET_VID_PORT_INFO_SET_PORT_INFO_ENABLE_PROTOCOL_ID,
    "enable_portcl",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_info.enable_protocol:\r\n"
    "  Enable VID resolution according to protocol parsing\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vid_res vid_port_info_set port_ndx 0 enable_portcl 0\r\n"
    "  enable_sbnt_ip 0 pvid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(15), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VID_PORT_INFO_SET_VID_PORT_INFO_SET_PORT_INFO_ENABLE_SUBNET_IP_ID,
    "enable_sbnt_ip",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_info.enable_subnet_ip:\r\n"
    "  Enable VID resolution according to subnet IP parsing\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vid_res vid_port_info_set port_ndx 0 enable_portcl 0\r\n"
    "  enable_sbnt_ip 0 pvid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(15), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VID_PORT_INFO_SET_VID_PORT_INFO_SET_PORT_INFO_PVID_ID,
    "pvid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_info.pvid:\r\n"
    "  Port default VLAN IDRange 0-4095\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vid_res vid_port_info_set port_ndx 0 enable_portcl 0\r\n"
    "  enable_sbnt_ip 0 pvid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(15), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VID_PORT_INFO_GET_VID_PORT_INFO_GET_ID,
    "vid_port_info_get",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the port information (which vid resolution algorithms to perform for this\r\n"
    "  port and default pvid) for the VLAN ID resolution module.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vid_res vid_port_info_get port_ndx 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(16), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VID_PORT_INFO_GET_VID_PORT_INFO_GET_PORT_NDX_ID,
    "port_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_ndx:\r\n"
    "  Local physical port ID. Range 0-31.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vid_res vid_port_info_get port_ndx 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(16), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_REMOVE_VID_COS_IP_SUBNET_INFO_REMOVE_ID,
    "vid_cos_ip_subnet_info_remove",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the per subnet IP information for the VLAN ID resolution module.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vid_res vid_cos_ip_subnet_info_set sbnt_ip_ndx 0\r\n"
    "  cvln_usr_prrty 0 trffc_clss 0 vid 0\r\n"
    "  sbnt_msk 0 ip_ddrss 0 is_ntry_valid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(17), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_REMOVE_VID_COS_IP_SUBNET_INFO_REMOVE_SUBNET_IP_INFO_SUBNET_MASK_ID,
    "sbnt_msk",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  subnet_ip_info.subnet_mask:\r\n"
    "  The number of most significant bits from the IP-Address that should be\r\n"
    "  matched to IPv4-Hdr.SrcIP.Range 1-32.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vid_res vid_cos_ip_subnet_info_set sbnt_ip_ndx 0\r\n"
    "  cvln_usr_prrty 0 trffc_clss 0 vid 0\r\n"
    "  sbnt_msk 0 ip_ddrss 0 is_ntry_vld 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(17), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_REMOVE_VID_COS_IP_SUBNET_INFO_REMOVE_SUBNET_IP_INFO_IP_ADDRESS_ID,
    "ip_address",
    (PARAM_VAL_RULES *)&Subnet_ip_rule[0],
    (sizeof(Subnet_ip_rule) / sizeof(Subnet_ip_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  subnet_ip_info.ip_address:\r\n"
    "  IP address\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vid_res vid_cos_ip_subnet_info_set sbnt_ip_ndx 0\r\n"
    "  cvln_usr_prrty 0 trffc_clss 0 vid 0\r\n"
    "  sbnt_msk 0 ip_ddrss 0 is_ntry_vld 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(17), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_ADD_VID_COS_IP_SUBNET_INFO_ADD_ID,
    "vid_cos_ip_subnet_info_add",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the per subnet IP information for the VLAN ID resolution module.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vid_res vid_cos_ip_subnet_info_set sbnt_ip_ndx 0\r\n"
    "  cvln_usr_prrty 0 trffc_clss 0 vid 0\r\n"
    "  sbnt_msk 0 ip_ddrss 0 is_ntry_valid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(18), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_ADD_VID_COS_IP_SUBNET_INFO_ADD_SUBNET_IP_INFO_CVLAN_USER_PRIORITY_ID,
    "cvln_usr_prrty",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  subnet_ip_info.cvlan_user_priority:\r\n"
    "  CVLAN User Priority.Relevant only for CEP ports according to 802.1ad standard\r\n"
    "  for a provider edge bridge and the packet arrives with two headers C-VLAN and\r\n"
    "  S-VLANRange 0-7\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vid_res vid_cos_ip_subnet_info_set sbnt_ip_ndx 0\r\n"
    "  cvln_usr_prrty 0 trffc_clss 0 vid 0\r\n"
    "  sbnt_msk 0 ip_ddrss 0 is_ntry_valid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(18), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_ADD_VID_COS_IP_SUBNET_INFO_ADD_SUBNET_IP_INFO_TRAFFIC_CLASS_ID,
    "trffc_clss",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  subnet_ip_info.traffic_class:\r\n"
    "  Traffic classRange 0-7.This used for COS resolution.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vid_res vid_cos_ip_subnet_info_set sbnt_ip_ndx 0\r\n"
    "  cvln_usr_prrty 0 trffc_clss 0 vid 0\r\n"
    "  sbnt_msk 0 ip_ddrss 0 is_ntry_valid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(18), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_ADD_VID_COS_IP_SUBNET_INFO_ADD_SUBNET_IP_INFO_VID_ID,
    "vid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  subnet_ip_info.vid:\r\n"
    "  VLAN IDRange 0-4095\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vid_res vid_cos_ip_subnet_info_set sbnt_ip_ndx 0\r\n"
    "  cvln_usr_prrty 0 trffc_clss 0 vid 0\r\n"
    "  sbnt_msk 0 ip_ddrss 0 is_ntry_valid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(18), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_ADD_VID_COS_IP_SUBNET_INFO_ADD_SUBNET_IP_INFO_SUBNET_MASK_ID,
    "sbnt_msk",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  subnet_ip_info.subnet_mask:\r\n"
    "  The number of most significant bits from the IP-Address that should be\r\n"
    "  matched to IPv4-Hdr.SrcIP.Range 1-32.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vid_res vid_cos_ip_subnet_info_set sbnt_ip_ndx 0\r\n"
    "  cvln_usr_prrty 0 trffc_clss 0 vid 0\r\n"
    "  sbnt_msk 0 ip_ddrss 0 is_ntry_valid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(18), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_ADD_VID_COS_IP_SUBNET_INFO_ADD_SUBNET_IP_INFO_IP_ADDRESS_ID,
    "ip_address",
    (PARAM_VAL_RULES *)&Subnet_ip_rule[0],
    (sizeof(Subnet_ip_rule) / sizeof(Subnet_ip_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  subnet_ip_info.ip_address:\r\n"
    "  IP address\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vid_res vid_cos_ip_subnet_info_set sbnt_ip_ndx 0\r\n"
    "  cvln_usr_prrty 0 trffc_clss 0 vid 0\r\n"
    "  sbnt_msk 0 ip_ddrss 0 is_ntry_valid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(18), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_ADD_VID_COS_IP_SUBNET_INFO_ADD_SUBNET_IP_INFO_IS_ENTRY_VALID_ID,
    "is_ntry_valid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  subnet_ip_info.is_entry_valid:\r\n"
    "  If set then this prefix should be considered.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vid_res vid_cos_ip_subnet_info_set sbnt_ip_ndx 0\r\n"
    "  cvln_usr_prrty 0 trffc_clss 0 vid 0\r\n"
    "  sbnt_msk 0 ip_ddrss 0 is_ntry_valid 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(18), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_GET_VID_COS_IP_SUBNET_INFO_GET_ID,
    "vid_cos_ip_subnet_info_get",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the per subnet IP information for the VLAN ID resolution module.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vid_res vid_cos_ip_subnet_info_get sbnt_ip_ndx 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(19), 0, 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VID_COS_IP_SUBNET_INFO_GET_VID_COS_IP_SUBNET_INFO_GET_SUBNET_IP_NDX_ID,
    "sbnt_ip_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  subnet_ip_ndx:\r\n"
    "  Index in the subnet IP mapping table.\r\n"
    "  Range 0-15 (the device support 16 different subnets)\r\n"
    "",
    "Ex:\r\n"
    "  timna_app vid_res vid_cos_ip_subnet_info_get sbnt_ip_ndx 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(19), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_VID_RES_ID,
    "vid_res",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, 0, BIT(15)|BIT(16)|BIT(17)|BIT(18)|BIT(19), 0, 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif
#ifdef UI_SWEEPT20_API_IPV4_UC
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_ADD_IPV4_UNICAST_ROUTE_ADD_ID,
    "route_add",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Binds between Ipv4 Unicast route key (IP-address/prefix) and a Unicast FEC\r\n"
    "  entry identified by the given sys_fec_id for a given virtual router. As a\r\n"
    "  result of this operation Unicast Ipv4 packets designated to IP address match\r\n"
    "  the given key (as long there is no more specific route key) will be routed\r\n"
    "  according to the information in the FEC entry identified by sys_fec_id.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc add vrf_id 0 prfx_len 0\r\n"
    "  ip_ddrss 0 sys_fec_id 0 overwrite 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_ADD_IPV4_UNICAST_ROUTE_ADD_VRF_ID_ID,
    "vrf_id",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vrf_id:\r\n"
    "  Virtual router id. Added for forward compatibility,\r\n"
    "  Range 0-0. Has to be zero, error is returned otherwise.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc add vrf_id 0 prfx_len 0\r\n"
    "  ip_ddrss 0 sys_fec_id 0 overwrite 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(0)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_ADD_IPV4_UNICAST_ROUTE_ADD_ROUTE_KEY_PREFIX_LEN_ID,
    "prfx_len",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  key.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc add vrf_id 0 prfx_len 0\r\n"
    "  ip_ddrss 0 sys_fec_id 0 overwrite 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_ADD_IPV4_UNICAST_ROUTE_ADD_ROUTE_KEY_IP_ADDRESS_ID,
    "ip_address",
    (PARAM_VAL_RULES *)&Subnet_ip_rule[0],
    (sizeof(Subnet_ip_rule) / sizeof(Subnet_ip_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  key.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc add vrf_id 0 prfx_len 0\r\n"
    "  ip_ddrss 0 sys_fec_id 0 overwrite 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_ADD_IPV4_UNICAST_ROUTE_ADD_SYS_FEC_ID_ID,
    "sys_fec_id",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_id:\r\n"
    "  system FEC id. Range 0- 4G\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc add vrf_id 0 prfx_len 0\r\n"
    "  ip_ddrss 0 sys_fec_id 0 overwrite 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_ADD_IPV4_UNICAST_ROUTE_ADD_OVERRIDE_ID,
    "overwrite",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  override:\r\n"
    "  whether to override the entry if it already exists, if this is\r\n"
    "  FALSE and the entry exist error is returned.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc add vrf_id 0 prfx_len 0\r\n"
    "  ip_ddrss 0 sys_fec_id 0 overwrite 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_GET_IPV4_UNICAST_ROUTE_GET_ID,
    "route_get",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Gets the system-FEC-id associated with the given key (IP-\r\n"
    "  address/prefix).\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc get vrf_id 0 prfx_len 0\r\n"
    "  ip_ddrss 0 exact_match 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_GET_IPV4_UNICAST_ROUTE_GET_VRF_ID_ID,
    "vrf_id",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vrf_id:\r\n"
    "  Virtual router id. Added for forward compatibility,\r\n"
    "  Range 0-0. Has to be zero, error is returned otherwise.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc get vrf_id 0 prfx_len 0\r\n"
    "  ip_ddrss 0 exact_match 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(1)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_GET_IPV4_UNICAST_ROUTE_GET_ROUTE_KEY_PREFIX_LEN_ID,
    "prfx_len",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  key.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc get vrf_id 0 prfx_len 0\r\n"
    "  ip_ddrss 0 exact_match 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_GET_IPV4_UNICAST_ROUTE_GET_ROUTE_KEY_IP_ADDRESS_ID,
    "ip_address",
    (PARAM_VAL_RULES *)&Subnet_ip_rule[0],
    (sizeof(Subnet_ip_rule) / sizeof(Subnet_ip_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  key.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc get vrf_id 0 prfx_len 0\r\n"
    "  ip_ddrss 0 exact_match 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_GET_IPV4_UNICAST_ROUTE_GET_EXACT_MATCH_ID,
    "exact_match",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  exact_match:\r\n"
    "  to find only exact match or best match.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc get vrf_id 0 prfx_len 0\r\n"
    "  ip_ddrss 0 exact_match 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_ID,
    "route_range_add",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Binds between Ipv4 Unicast route key (IP-address/prefix) and a Unicast FEC\r\n"
    "  entry identified by the given sys_fec_id for a given virtual router. As a\r\n"
    "  result of this operation Unicast Ipv4 packets designated to IP address match\r\n"
    "  the given key (as long there is no more specific route key) will be routed\r\n"
    "  according to the information in the FEC entry identified by sys_fec_id.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test add frst_vrf_id 0 lst_vrf_id 0\r\n"
    "  step 0 count 0 prefix 0 from_ip 0 to_ip 0\r\n"
    "  range_type 0 fec_range_type 0 sys_fec_id 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(2)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_SYS_FEC_ID_ID,
    "sys_fec_id",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_fec_id:\r\n"
    "  system FEC id. Range 0-4G. if fec_range_type is incremental then\r\n"
    "  this is the first sys fec to start from.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc add frst_vrf_id 0 lst_vrf_id 0\r\n"
    "  step 0 count 0 prefix 0 from_ip 0 to_ip 0\r\n"
    "  range_type 0 fec_range_type 0 sys_fec_id 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(2)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_FIRST_VRF_ID_ID,
    "frst_vrf_id",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  first_vrf_id:\r\n"
    "  first Virtual router id to set.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test add frst_vrf_id 0 lst_vrf_id 0\r\n"
    "  step 0 count 0 prefix 0 from_ip 0 to_ip 0\r\n"
    "  range_type 0 fec_range_type 0 sys_fec_id 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(2)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_LAST_VRF_ID_ID,
    "lst_vrf_id",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  last_vrf_id:\r\n"
    "  last Virtual router id to set.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test add frst_vrf_id 0 lst_vrf_id 0\r\n"
    "  step 0 count 0 prefix 0 from_ip 0 to_ip 0\r\n"
    "  range_type 0 fec_range_type 0 sys_fec_id 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(2)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_RANGE_INFO_STEP_ID,
    "step",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.step:\r\n"
    "  Number of step between to consecutive IP addresses. Relevant only for\r\n"
    "  increment and decrement.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test add frst_vrf_id 0 lst_vrf_id 0\r\n"
    "  step 0 count 0 prefix 0 from_ip 0 to_ip 0\r\n"
    "  range_type 0 fec_range_type 0 sys_fec_id 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(2)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_RANGE_INFO_COUNT_ID,
    "count",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.count:\r\n"
    "  Number of IP addresses to add.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test add frst_vrf_id 0 lst_vrf_id 0\r\n"
    "  step 0 count 0 prefix 0 from_ip 0 to_ip 0\r\n"
    "  range_type 0 fec_range_type 0 sys_fec_id 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(2)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_RANGE_INFO_PREFIX_ID,
    "prefix",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.prefix:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test add frst_vrf_id 0 lst_vrf_id 0\r\n"
    "  step 0 count 0 prefix 0 from_ip 0 to_ip 0\r\n"
    "  range_type 0 fec_range_type 0 sys_fec_id 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(2)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_RANGE_INFO_TO_ID,
    "to_ip",
    (PARAM_VAL_RULES *)&Subnet_ip_rule[0],
    (sizeof(Subnet_ip_rule) / sizeof(Subnet_ip_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.to:\r\n"
    "  IP address, range ends at this IP address. Relevant only for random. For\r\n"
    "  increment/decrement types the end of the range is determined according to the\r\n"
    "  count and step.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test add frst_vrf_id 0 lst_vrf_id 0\r\n"
    "  step 0 count 0 prefix 0 from_ip 0 to_ip 0\r\n"
    "  range_type 0 fec_range_type 0 sys_fec_id 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(2)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_RANGE_INFO_FROM_ID,
    "from_ip",
    (PARAM_VAL_RULES *)&Subnet_ip_rule[0],
    (sizeof(Subnet_ip_rule) / sizeof(Subnet_ip_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.from:\r\n"
    "  IP address, range starts from this IP address.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test add frst_vrf_id 0 lst_vrf_id 0\r\n"
    "  step 0 count 0 prefix 0 from_ip 0 to_ip 0\r\n"
    "  range_type 0 fec_range_type 0 sys_fec_id 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(2)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_RANGE_INFO_TYPE_ID,
    "range_type",
    (PARAM_VAL_RULES *)&TIMNA_IPV4_RANGE_TYPE_rule[0],
    (sizeof(TIMNA_IPV4_RANGE_TYPE_rule) / sizeof(TIMNA_IPV4_RANGE_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.type:\r\n"
    "  How to pick up addresses from the given range randomly incrementally etc.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test add frst_vrf_id 0 lst_vrf_id 0\r\n"
    "  step 0 count 0 prefix 0 from_ip 0 to_ip 0\r\n"
    "  range_type 0 fec_range_type 0 sys_fec_id 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(2)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_ADD_IPV4_UNICAST_RANGE_ADD_FEC_RANGE_TYPE_ID,
    "fec_range_type",
    (PARAM_VAL_RULES *)&TIMNA_IPV4_SYS_FEC_RANGE_TYPE_rule[0],
    (sizeof(TIMNA_IPV4_SYS_FEC_RANGE_TYPE_rule) / sizeof(TIMNA_IPV4_SYS_FEC_RANGE_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  fec_range_type:\r\n"
    "  to which fec to map the IP range.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test add frst_vrf_id 0 lst_vrf_id 0\r\n"
    "  step 0 count 0 prefix 0 from_ip 0 to_ip 0\r\n"
    "  range_type 0 fec_range_type 0 sys_fec_id 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(2)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_REMOVE_IPV4_UNICAST_ROUTE_REMOVE_ID,
    "route_remove",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove the association of the IP-address to Unicast FEC.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc remove vrf_id 0 prfx_len 0\r\n"
    "  ip_ddrss 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_REMOVE_IPV4_UNICAST_ROUTE_REMOVE_VRF_ID_ID,
    "vrf_id",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vrf_id:\r\n"
    "  Virtual router id. Added for forward compatibility,\r\n"
    "  Range 0-0. Has to be zero, error is returned otherwise.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc remove vrf_id 0 prfx_len 0\r\n"
    "  ip_ddrss 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(3)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_REMOVE_IPV4_UNICAST_ROUTE_REMOVE_ROUTE_KEY_PREFIX_LEN_ID,
    "prfx_len",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  key.prefix_len:\r\n"
    "  Number of bits to consider in the IP address starting from the msb. Range\r\n"
    "  0-32.Example for key ip_address 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination Address of the form 192.168.1.x\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc remove vrf_id 0 prfx_len 0\r\n"
    "  ip_ddrss 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(3)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_ROUTE_REMOVE_IPV4_UNICAST_ROUTE_REMOVE_ROUTE_KEY_IP_ADDRESS_ID,
    "ip_address",
    (PARAM_VAL_RULES *)&Subnet_ip_rule[0],
    (sizeof(Subnet_ip_rule) / sizeof(Subnet_ip_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  key.ip_address:\r\n"
    "  Ipv4 destination address.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc remove vrf_id 0 prfx_len 0\r\n"
    "  ip_ddrss 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(3)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_ID,
    "route_range_remove",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove Ipv4 Unicast route keys (IP-removeress/prefix) Range from the routing table.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test remove frst_vrf_id 0 lst_vrf_id 0\r\n"
    "  step 0 count 0 prefix 0 from_ip 0 to_ip 0\r\n"
    "  range_type 0 \r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(4)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_FIRST_VRF_ID_ID,
    "frst_vrf_id",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  first_vrf_id:\r\n"
    "  first Virtual router id to set.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test remove frst_vrf_id 0 lst_vrf_id 0\r\n"
    "  step 0 count 0 prefix 0 from_ip 0 to_ip 0\r\n"
    "  range_type 0 \r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(4)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_LAST_VRF_ID_ID,
    "lst_vrf_id",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  last_vrf_id:\r\n"
    "  last Virtual router id to set.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test remove frst_vrf_id 0 lst_vrf_id 0\r\n"
    "  step 0 count 0 prefix 0 from_ip 0 to_ip 0\r\n"
    "  range_type 0 \r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(4)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_RANGE_INFO_STEP_ID,
    "step",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.step:\r\n"
    "  Number of step between to consecutive IP removeresses. Relevant only for\r\n"
    "  increment and decrement.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test remove frst_vrf_id 0 lst_vrf_id 0\r\n"
    "  step 0 count 0 prefix 0 from_ip 0 to_ip 0\r\n"
    "  range_type 0 \r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_RANGE_INFO_COUNT_ID,
    "count",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.count:\r\n"
    "  Number of IP removeresses to remove.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test remove frst_vrf_id 0 lst_vrf_id 0\r\n"
    "  step 0 count 0 prefix 0 from_ip 0 to_ip 0\r\n"
    "  range_type 0 \r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_RANGE_INFO_PREFIX_ID,
    "prefix",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.prefix:\r\n"
    "  Number of bits to consider in the IP removeress starting from the msb. Range\r\n"
    "  0-32.Example for key ip_removeress 192.168.1.0 and prefix_len 24\r\n"
    "  would match any IP Destination REMOVEress of the form 192.168.1.x\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test remove frst_vrf_id 0 lst_vrf_id 0\r\n"
    "  step 0 count 0 prefix 0 from_ip 0 to_ip 0\r\n"
    "  range_type 0 \r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_RANGE_INFO_TO_ID,
    "to_ip",
    (PARAM_VAL_RULES *)&Subnet_ip_rule[0],
    (sizeof(Subnet_ip_rule) / sizeof(Subnet_ip_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.to:\r\n"
    "  IP removeress, range ends at this IP removeress. Relevant only for random. For\r\n"
    "  increment/decrement types the end of the range is determined according to the\r\n"
    "  count and step.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test remove frst_vrf_id 0 lst_vrf_id 0\r\n"
    "  step 0 count 0 prefix 0 from_ip 0 to_ip 0\r\n"
    "  range_type 0 \r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_RANGE_INFO_FROM_ID,
    "from_ip",
    (PARAM_VAL_RULES *)&Subnet_ip_rule[0],
    (sizeof(Subnet_ip_rule) / sizeof(Subnet_ip_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.from:\r\n"
    "  IP removeress, range starts from this IP removeress.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test remove frst_vrf_id 0 lst_vrf_id 0\r\n"
    "  step 0 count 0 prefix 0 from_ip 0 to_ip 0\r\n"
    "  range_type 0 \r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UNICAST_RANGE_REMOVE_IPV4_UNICAST_RANGE_REMOVE_RANGE_INFO_TYPE_ID,
    "range_type",
    (PARAM_VAL_RULES *)&TIMNA_IPV4_RANGE_TYPE_rule[0],
    (sizeof(TIMNA_IPV4_RANGE_TYPE_rule) / sizeof(TIMNA_IPV4_RANGE_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  range_info.type:\r\n"
    "  How to pick up removeresses from the given range randomly incrementally etc.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test remove frst_vrf_id 0 lst_vrf_id 0\r\n"
    "  step 0 count 0 prefix 0 from_ip 0 to_ip 0\r\n"
    "  range_type 0 \r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_ID,
    "fec_range_set",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set Range of outlif for ipv4 unicast routing editing, if the packet\r\n"
    "  encapsulated with VC label, then there is possibility to bind the packets\r\n"
    "  incrementally with VC labels.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test fec_range_set fec 10 up_to_fec 20 \r\n"
    "  vc_label_id 8 sys_port_id 17 range_inc_eep 1 range_inc_vc_lbl 1 encap_ptr 18\r\n"
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
    PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_FIRST_OUTLIF_ID,
    "fec",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  first_outlif:\r\n"
    "  first outlif to set.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test fec_range_set fec 10 up_to_fec 20 \r\n"
    "  vc_label_id 8 sys_port_id 17 range_inc_eep 1 range_inc_vc_lbl 1 encap_ptr 18\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, BIT(11)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_LAST_OUTLIF_ID,
    "up_to_fec",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  last_outlif:\r\n"
    "  last outlif to set.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test fec_range_set fec 10 up_to_fec 20 \r\n"
    "  vc_label_id 8 sys_port_id 17 range_inc_eep 1 range_inc_vc_lbl 1 encap_ptr 18\r\n"
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
    PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_EDIT_INFO_ENCAP_POINTER_ID,
    "encap_ptr",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  edit_info.encap_pointer:\r\n"
    "  Pointer for encapsulation entry to set whether to add tunnel label, and a\r\n"
    "  pointer to ARP entry.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test fec_range_set fec 10 up_to_fec 20 \r\n"
    "  vc_label_id 8 sys_port_id 17 range_inc_eep 1 range_inc_vc_lbl 1 encap_ptr 18\r\n"
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
    PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_EDIT_INFO_LABEL_TTL_PTR_ID,
    "vc_lbl_ttl_ptr",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  edit_info.label.ttl_ptr:\r\n"
    "  Pointer to 4 configured TTL.Should be configured by\r\n"
    "  timna_mpls_glbl_info_set()When the packet is MPLS LSR packet, this setting is\r\n"
    "  ignored, and the TTL is inherited from the LSR label.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test fec_range_set fec 10 up_to_fec 20 \r\n"
    "  vc_label_id 8 sys_port_id 17 range_inc_eep 1 range_inc_vc_lbl 1 encap_ptr 18\r\n"
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
    PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_EDIT_INFO_LABEL_EXP_QOS_ID,
    "vc_lbl_exp_qos",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  edit_info.label.exp_qos:\r\n"
    "  Experimental bits outgoing traffic over this Tunnel.When the packet is MPLS\r\n"
    "  LSR packet, this setting is ignored, and the EXP is inherited from the LSR\r\n"
    "  label.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test fec_range_set fec 10 up_to_fec 20 \r\n"
    "  vc_label_id 8 sys_port_id 17 range_inc_eep 1 range_inc_vc_lbl 1 encap_ptr 18\r\n"
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
    PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_EDIT_INFO_LABEL_LABEL_ID_ID,
    "vc_label_id",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  edit_info.label.label_id:\r\n"
    "  Tunnel label\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test fec_range_set fec 10 up_to_fec 20 \r\n"
    "  vc_label_id 8 sys_port_id 17 range_inc_eep 1 range_inc_vc_lbl 1 encap_ptr 18\r\n"
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
    PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_EDIT_INFO_VC_VALID_ID,
    "vc_label_vld",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  edit_info.vc_valid:\r\n"
    "  If True then the Ipv4 packet is encapsulated with a VC label. L3VPN\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test fec_range_set fec 10 up_to_fec 20 \r\n"
    "  vc_label_id 8 sys_port_id 17 range_inc_eep 1 range_inc_vc_lbl 1 encap_ptr 18\r\n"
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
    PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_INC_VC_LABEL_ID,
    "range_inc_vc_lbl",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  inc_vc_label:\r\n"
    "  whether to increment the VC label.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test fec_range_set fec 10 up_to_fec 20 \r\n"
    "  vc_label_id 8 sys_port_id 17 range_inc_eep 1 range_inc_vc_lbl 1 encap_ptr 18\r\n"
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
    PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_INC_EEP_ID,
    "range_inc_eep",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  inc_vc_label:\r\n"
    "  whether to increment the EEP pointer.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test fec_range_set fec 10 up_to_fec 20 \r\n"
    "  vc_label_id 8 sys_port_id 17 range_inc_eep 1 range_inc_vc_lbl 1 encap_ptr 18\r\n"
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
    PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_SET_IPV4_UNICAST_TEST_EDIT_RANGE_SET_SYS_PORT_ID,
    "sys_port_id",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_port_id:\r\n"
    "  System port id to forwared packet to.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test fec_range_set fec 10 up_to_fec 20 \r\n"
    "  vc_label_id 8 sys_port_id 17 range_inc_eep 1 range_inc_vc_lbl 1 encap_ptr 18\r\n"
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
    PARAM_SWEEPT20_IPV4_UNICAST_TEST_EDIT_RANGE_GET_IPV4_UNICAST_TEST_EDIT_RANGE_GET_ID,
    "edit_range_get",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set Range of outlif for ipv4 unicast routing editing, if the packet\r\n"
    "  encapsulated with VC label, then there is possibility to bind the packets\r\n"
    "  incrementally with VC labels.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc_test fec_range_set fec 10 up_to_fec 20 \r\n"
    "  vc_label_id 8 sys_port_id 17 range_inc_eep 1 range_inc_vc_lbl 1 encap_ptr 18\r\n"
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
    PARAM_SWEEPT20_IPV4_UNICAST_ROUTE_GET_BLOCK_IPV4_UNICAST_ROUTE_GET_BLOCK_ID,
    "get_block",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get block of entries from the routing table in a given range.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc get_block vrf_id 0 \r\n"
    "  ntrs_to_get 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(11)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_IPV4_UNICAST_ROUTE_GET_BLOCK_IPV4_UNICAST_ROUTE_GET_BLOCK_VRF_ID_ID,
    "vrf_id",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vrf_id:\r\n"
    "  Virtual router id. Added for forward compatibility,\r\n"
    "  Range 0-0. Has to be zero, error is returned otherwise.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app ipv4_uc get_block vrf_id 0 \r\n"
    "  ntrs_to_get 0\r\n"
    "",
    "",
    {0, 0, 0, 0, BIT(11)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_IPV4_UC_ID,
    "ipv4_uc",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, 0, 0, BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(11),BIT(11)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif
#ifdef UI_SWEEPT20_API_ACL
  {
    PARAM_SWEEPT20_API_ACL_TEMPLATE_ADD_TEMPLATE_ADD_ID,
    "template_add",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Add ACL (Access Control List) of predefined key templates, and associate it\r\n"
    "  with the given traffic type, so packets terminated or processed of this\r\n"
    "  traffic type will be matched against the ACEs (Access Control Entries) of the\r\n"
    "  given ACL.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl template_add predefined_tmplt 0 size 0 traffic_type\r\n"
    "  0\r\n"
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
    PARAM_SWEEPT20_API_ACL_TEMPLATE_ADD_TEMPLATE_ADD_ACL_INFO_PREDEFINED_TEMPLATE_ID,
    "predefined_tmplt",
    (PARAM_VAL_RULES *)&TIMNA_ACL_KEY_TEMPLATE_rule[0],
    (sizeof(TIMNA_ACL_KEY_TEMPLATE_rule) / sizeof(TIMNA_ACL_KEY_TEMPLATE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acl_info.predefined_template:\r\n"
    "  Predefined templates that determine which fields of the packets to be\r\n"
    "  examined.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl template_add predefined_tmplt 0 size 0 traffic_type\r\n"
    "  0\r\n"
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
    PARAM_SWEEPT20_API_ACL_TEMPLATE_ADD_TEMPLATE_ADD_ACL_INFO_SIZE_ID,
    "size",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acl_info.size:\r\n"
    "  Capacity of the ACL, maximum number of ACEs.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl template_add predefined_tmplt 0 size 0 traffic_type\r\n"
    "  0\r\n"
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
    PARAM_SWEEPT20_API_ACL_TEMPLATE_ADD_TEMPLATE_ADD_ACL_INFO_TRAFFIC_ID,
    "traffic_type",
    (PARAM_VAL_RULES *)&TIMNA_ACL_TRAFFIC_TYPE_rule[0],
    (sizeof(TIMNA_ACL_TRAFFIC_TYPE_rule) / sizeof(TIMNA_ACL_TRAFFIC_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acl_info.traffic:\r\n"
    "  The type of the client of the ACL (termination (Ethernet...), or main engine\r\n"
    "  (Ethernet...) \r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl template_add predefined_tmplt 0 size 0 traffic_type\r\n"
    "  0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ID,
    "ace_add",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Add ACE (Access Control Entry) to the given ACL. On packet lookup, between\r\n"
    "  all the ACEs that match the key constructed from the packet, the ACE with the\r\n"
    "  lowest ace_ndx will be chosen. For example If 'ace-ndx-1' < 'ace-ndx-2', and\r\n"
    "  both match the given key then 'acea-priority-1' will be chosen.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACL_NDX_ID,
    "acl_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acl_ndx:\r\n"
    "  ACL ID to add to. Range 0-7.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, BIT(1)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_NDX_ID,
    "ace_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace_ndx:\r\n"
    "  The entry index to add the ACE to. The smaller the ace_ndx,\r\n"
    "  the higher will be the priority of the ACE. Range 0-(ACL size-\r\n"
    "  1).\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, BIT(1)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L4_INFO_PORT_RANGE_MAX_PORT_ID,
    "l4_prt_rng_max_prt",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l4_info.port_range.max_port:\r\n"
    "  The maximum port in the range. Size: 16 bits. Range 0-65535.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L4_INFO_PORT_RANGE_MIN_PORT_ID,
    "l4_prt_rng_min_prt",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l4_info.port_range.min_port:\r\n"
    "  The minimum port in the range. Size: 16 bits. Range 0-65535.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L4_INFO_PORT_RANGE_IS_SRC_PORT_ID,
    "l4_prt_rng_is_src_prt",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l4_info.port_range.is_src_port:\r\n"
    "  If TRUE then this define a source port range. Otherwise it defines a\r\n"
    "  destination l4 port range. Size: 10 bits.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L4_INFO_DEST_PORT_MASK_ID,
    "l4_dst_prt_msk",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l4_info.dest_port_mask:\r\n"
    "  A mask on the dest_port (bitmap) indicates which bits to consider and which\r\n"
    "  are masked. 0 indicates all bits are masked, 0Xffff indicates to consider all\r\n"
    "  the bits in the key.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L4_INFO_DEST_PORT_ID,
    "l4_dst_prt",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l4_info.dest_port:\r\n"
    "  Layer 4 Destination Port, second 16b after IPv4 header.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L4_INFO_SRC_PORT_MASK_ID,
    "l4_src_prt_msk",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l4_info.src_port_mask:\r\n"
    "  A mask on the src_port (bitmap) indicates which bits to consider and which\r\n"
    "  are masked. 0 indicates all bits are masked, 0Xffff indicates to consider all\r\n"
    "  the bits in the key.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L4_INFO_SRC_PORT_ID,
    "l4_src_prt",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l4_info.src_port:\r\n"
    "  Layer 4 Source Port, first 16b after IPv4 header.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L3_INFO_DEST_IP_ADD_ID,
    "l3_dst_ip_address",
    (PARAM_VAL_RULES *)&Subnet_ip_rule[0],
    (sizeof(Subnet_ip_rule) / sizeof(Subnet_ip_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l3_info.dest_ip.address:\r\n"
    "  Ipv4 address.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L3_INFO_DEST_IP_PREFFIX_ID,
    "l3_dst_ip_pref",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l3_info.dest_ip.pref:\r\n"
    "  how many bits to consider from the ip, starting from the msb.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L3_INFO_SRC_IP_ADD_ID,
    "l3_src_ip_address",
    (PARAM_VAL_RULES *)&Subnet_ip_rule[0],
    (sizeof(Subnet_ip_rule) / sizeof(Subnet_ip_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l3_info.src_ip.address:\r\n"
    "  IP address.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L3_INFO_SRC_IP_PREFFIX_ID,
    "l3_src_ip_pref",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l3_info.src_ip.pref:\r\n"
    "  how many bits to consider from the ip, starting from the msb.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L3_INFO_PROTOCOL_CODE_VALID_ID,
    "l3_prtcl_cod_vld",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l3_info.protocol_code_valid:\r\n"
    "  If TRUE then protocol code considered as part of the ACE. Otherwise it masked\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L3_INFO_PROTOCOL_CODE_ID,
    "l3_prtcl_cod",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l3_info.protocol_code:\r\n"
    "  The Protocol field from the Ipv4 header. Size: 8 bits. Range 0-255.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L3_INFO_TOS_MASK_ID,
    "l3_tos_msk",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l3_info.tos_mask:\r\n"
    "  A mask on the tos (bitmap) indicates which bits to consider and which are\r\n"
    "  masked. 0 indicates all bits are masked, 0Xff indicates to consider all the\r\n"
    "  bits in the key.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L3_INFO_TOS_ID,
    "l3_tos",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l3_info.tos:\r\n"
    "  The TOS field from the Ipv4 header. Size: 8 bits. Range 0-255.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_DEST_MAC_VALID_ID,
    "l2_dst_mac_vld",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l2_info.dest_mac_valid:\r\n"
    "  If TRUE then dest_mac considered as part of the ACE. Otherwise it masked. \r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_DEST_MAC_ID,
    "l2_dst_mac",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l2_info.dest_mac:\r\n"
    "  Destination MAC\r\n"
    "  Address. Size: 48 bits.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_SRC_MAC_VALID_ID,
    "l2_src_mac_vld",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l2_info.src_mac_valid:\r\n"
    "  If TRUE then src_mac considered as part of the ACE. Otherwise it masked. \r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_SRC_MAC_ID,
    "l2_src_mac",
    (PARAM_VAL_RULES *)&Timna_mac_addr_vals[0],
    (sizeof(Timna_mac_addr_vals) / sizeof(Timna_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l2_info.src_mac:\r\n"
    "  Source MAC Address. Size: 48 bits.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_S_TAG_DEI_VALID_ID,
    "l2_s_tag_dei_vld",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l2_info.s_tag.dei_valid:\r\n"
    "  If TRUE then dei considered as part of the ACE. Otherwise it masked. \r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_S_TAG_DEI_ID,
    "l2_s_tag_dei",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l2_info.s_tag.dei:\r\n"
    "  the value returned in the Ethernet Parsing of the DEI (drop eligibility\r\n"
    "  indicator). Size: 1 bit. Range 0-1. Relevant only for the S-TAG.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_S_TAG_PCP_VALID_ID,
    "l2_s_tag_pcp_vld",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l2_info.s_tag.pcp_valid:\r\n"
    "  If TRUE then pcp considered as part of the ACE. Otherwise it masked. \r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_S_TAG_PCP_ID,
    "l2_s_tag_pcp",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l2_info.s_tag.pcp:\r\n"
    "  the value returned in the Ethernet Parsing of the PCP (priority code point).\r\n"
    "  Size: 3 bits. Range 0-7.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_S_TAG_TAG_EXIST_VALID_ID,
    "l2_s_tag_exist_vld",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l2_info.s_tag.tag_exist_valid:\r\n"
    "  If TRUE then tag_exist considered as part of the ACE. Otherwise it masked. \r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_S_TAG_TAG_EXIST_ID,
    "l2_s_tag_exist",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l2_info.s_tag.tag_exist:\r\n"
    "  True if the Vlan tag is present in the packet. Size: 1 bit.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_S_TAG_VID_MASK_ID,
    "l2_s_tag_vid_msk",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l2_info.s_tag.vid_mask:\r\n"
    "  A mask on the vid (bitmap) indicates which bits to consider and which are\r\n"
    "  masked. 0 indicates all bits are masked, 0Xfff indicates to consider all the\r\n"
    "  bits in the key.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_S_TAG_VID_ID,
    "l2_s_tag_vid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l2_info.s_tag.vid:\r\n"
    "  The Vlan Id. Size: 12 bits. Range 0-4095.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_C_TAG_DEI_VALID_ID,
    "l2_c_tag_dei_vld",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l2_info.c_tag.dei_valid:\r\n"
    "  If TRUE then dei considered as part of the ACE. Otherwise it masked. \r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_C_TAG_DEI_ID,
    "l2_c_tag_dei",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l2_info.c_tag.dei:\r\n"
    "  the value returned in the Ethernet Parsing of the DEI (drop eligibility\r\n"
    "  indicator). Size: 1 bit. Range 0-1. Relevant only for the S-TAG.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_C_TAG_PCP_VALID_ID,
    "l2_c_tag_pcp_vld",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l2_info.c_tag.pcp_valid:\r\n"
    "  If TRUE then pcp considered as part of the ACE. Otherwise it masked. \r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_C_TAG_PCP_ID,
    "l2_c_tag_pcp",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l2_info.c_tag.pcp:\r\n"
    "  the value returned in the Ethernet Parsing of the PCP (priority code point).\r\n"
    "  Size: 3 bits. Range 0-7.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_C_TAG_TAG_EXIST_VALID_ID,
    "l2_c_tag_exist_vld",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l2_info.c_tag.tag_exist_valid:\r\n"
    "  If TRUE then tag_exist considered as part of the ACE. Otherwise it masked. \r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_C_TAG_TAG_EXIST_ID,
    "l2_c_tag_exist",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l2_info.c_tag.tag_exist:\r\n"
    "  True if the Vlan tag is present in the packet. Size: 1 bit.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_C_TAG_VID_MASK_ID,
    "l2_c_tag_vid_msk",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l2_info.c_tag.vid_mask:\r\n"
    "  A mask on the vid (bitmap) indicates which bits to consider and which are\r\n"
    "  masked. 0 indicates all bits are masked, 0Xfff indicates to consider all the\r\n"
    "  bits in the key.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_C_TAG_VID_ID,
    "l2_c_tag_vid",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l2_info.c_tag.vid:\r\n"
    "  The Vlan Id. Size: 12 bits. Range 0-4095.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_PORT_TYPE_VALID_ID,
    "l2_prt_typ_vld",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l2_info.port_type_valid:\r\n"
    "  If TRUE then port_type considered as part of the ACE. Otherwise it masked. \r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_PORT_TYPE_ID,
    "l2_prt_typ",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l2_info.port_type:\r\n"
    "  The l2 port type (VBP,CEP,CNP,PNP). Size: 2 bits.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_ETHER_TYPE_VALID_ID,
    "l2_thr_typ_vld",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l2_info.ether_type_valid:\r\n"
    "  If TRUE then ether_type considered as part of the ACE. Otherwise it masked. \r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_L2_INFO_ETHER_TYPE_ID,
    "l2_thr_typ",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.l2_info.ether_type:\r\n"
    "  The EtherType field of the Ethernet header. Range: must be one of the values\r\n"
    "  set in the global information setting. Size: 3 bits.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_GENERAL_INFO_INLIF_VALID_ID,
    "general_nlf_vld",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.general_info.inlif_valid:\r\n"
    "  If TRUE then inlif considered as part of the ACE. Otherwise it masked. \r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_GENERAL_INFO_INLIF_ID,
    "general_nlf",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.general_info.inlif:\r\n"
    "  The source Id of the packet. In 802.1AD and 802.1Q it is the system port ID.\r\n"
    "  Size: 19 bits. Range 0-(16k-1). \r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_GENERAL_INFO_VSID_VALID_ID,
    "general_vsd_vld",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.general_info.vsid_valid:\r\n"
    "  If TRUE then vsid considered as part of the ACE. Otherwise it masked. \r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_GENERAL_INFO_VSID_ID,
    "general_vsd",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.general_info.vsid:\r\n"
    "  Virtual Switch ID. Size: 14 bits. Range 0-(16k-1).\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_GENERAL_INFO_LOCAL_PORT_VALID_ID,
    "general_lcl_prt_vld",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.general_info.local_port_valid:\r\n"
    "  If TRUE then local_port considered as part of the ACE. Otherwise it masked. \r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACE_GENERAL_INFO_LOCAL_PORT_ID,
    "general_lcl_prt",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace.general_info.local_port:\r\n"
    "  Local port Id. Size: 6 bits. Range 0-31.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_CPU_CODE_ID,
    "action_cpu_cod",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action_info.cpu_code:\r\n"
    "  Code to be sent in the packets header to the CPU. Relevant only if the ACL\r\n"
    "  action type includes OutLIF.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_OVERWRITE_CONTROL_ID,
    "action_overwrite_cntrl",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action_info.overwrite_control:\r\n"
    "  IF TRUE then Ethernet bridged packet will be overwrite by the ACL result\r\n"
    "  regardless of the bridge Forwarding Type. If FALSE then if the bridge\r\n"
    "  forwarding type is control then the ACL rule will not overwrite by the ACL\r\n"
    "  action.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_COS_PARAMS_DP_ID,
    "action_cos_prms_DP",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action_info.cos_params.DP:\r\n"
    "  Drop precedenceRange 0-3.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_COS_PARAMS_TC_ID,
    "action_cos_prms_TC",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action_info.cos_params.TC:\r\n"
    "  Traffic ClassRange 0-7.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_COS_PARAMS_UP_ID,
    "action_cos_prms_UP",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action_info.cos_params.UP:\r\n"
    "  User PriorityRange 0-7.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_POLICER_CONTROLLER_ID,
    "action_policer_cntrllr",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action_info.policer_controller:\r\n"
    "  Select the policer controller (set of parameters to control the actions\r\n"
    "  related to the Policer) results. Range 0-8. Relevant only if the acl action\r\n"
    "  type includes Policing.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_POLICER_POINTER_ID,
    "action_policer_pntr",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action_info.policer_pointer:\r\n"
    "  Select the policer to police the packets. Range 0-65535. Relevant only if the\r\n"
    "  acl action type includes Policing.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_OUTLIF_ID,
    "action_tlf",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action_info.outlif:\r\n"
    "  OutLIF to overwrite the exist value. The destination of the packet. In 802.1q\r\n"
    "  and 802.1ad this is the system port. Range 0-524287. Relevant only if the acl\r\n"
    "  action type includes OutLIF.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_ACTION_INFO_TYPE_ID,
    "action_typ",
    (PARAM_VAL_RULES *)&TIMNA_ACL_ACTION_TYPE_rule[0],
    (sizeof(TIMNA_ACL_ACTION_TYPE_rule) / sizeof(TIMNA_ACL_ACTION_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action_info.type:\r\n"
    "  The type of the action, indicates which fields to modify. The rest of the\r\n"
    "  fields are considered / ignored according to this type. see\r\n"
    "  TIMNA_ACL_ACTION_TYPE. \r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_ADD_ACE_ADD_OVERWRITE_ID,
    "vrwrt",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  overwrite:\r\n"
    "  whether to overwrite the exist ACE on the given ace_ndx,\r\n"
    "  if over_write is FALSE and there is already ACE on the given\r\n"
    "  index, then error is returned.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_add acl_ndx 0 ace_ndx 0 l4_prt_rng_max_prt 0\r\n"
    "  l4_prt_rng_min_prt 0 l4_prt_rng_is_src_prt 0\r\n"
    "  l4_dst_prt_msk 0 l4_dst_prt 0 l4_src_prt_msk 0\r\n"
    "  l4_src_prt 0 l3_dst_ip_address 0 l3_dst_ip_pref 0\r\n"
    "  l3_src_ip_address 0 l3_src_ip_pref 0\r\n"
    "  l3_prtcl_cod_vld 0 l3_prtcl_cod 0\r\n"
    "  l3_tos_msk 0 l3_tos 0 l2_dst_mac_vld 0\r\n"
    "  l2_dst_mac 0 l2_src_mac_vld 0 l2_src_mac 0\r\n"
    "  l2_s_tag_dei_vld 0 l2_s_tag_dei 0 l2_s_tag_pcp_vld 0\r\n"
    "  l2_s_tag_pcp 0 l2_s_tag_exist_vld 0\r\n"
    "  l2_s_tag_exist 0 l2_s_tag_vid_msk 0 l2_s_tag_vid 0\r\n"
    "  l2_c_tag_dei_vld 0 l2_c_tag_dei 0 l2_c_tag_pcp_vld 0\r\n"
    "  l2_c_tag_pcp 0 l2_c_tag_exist_vld 0\r\n"
    "  l2_c_tag_exist 0 l2_c_tag_vid_msk 0 l2_c_tag_vid 0\r\n"
    "  l2_prt_typ_vld 0 l2_prt_typ 0 l2_thr_typ_vld 0\r\n"
    "  l2_thr_typ 0 general_nlf_vld 0 general_nlf 0\r\n"
    "  general_vsd_vld 0 general_vsd 0 general_lcl_prt_vld 0\r\n"
    "  general_lcl_prt 0 action_cpu_cod 0 action_overwrite_cntrl 0\r\n"
    "  action_cos_prms_DP 0 action_cos_prms_TC 0 action_cos_prms_UP 0\r\n"
    "  action_policer_cntrllr 0 action_policer_pntr 0 action_tlf 0 action_typ 0 vrwrt 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_GET_BLOCK_ACE_GET_BLOCK_ID,
    "ace_get_block",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Gets a block of ACEs (Access Control Entry) with the ace_ndx in the given ACL.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_get_block acl_ndx 0 blck_rng_ntrs_to_act 0\r\n"
    "  blck_rng_ntrs_to_scn 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_GET_BLOCK_ACE_GET_BLOCK_ACL_NDX_ID,
    "acl_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acl_ndx:\r\n"
    "  ACL ID to get the ACEs from. Range 0-7.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_get_block acl_ndx 0 blck_rng_ntrs_to_act 0\r\n"
    "  blck_rng_ntrs_to_scn 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_GET_BLOCK_ACE_GET_BLOCK_BLOCK_RANGE_ENTRIES_TO_ACT_ID,
    "blck_rng_ntrs_to_act",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  block_range.entries_to_act:\r\n"
    "  Number of valid ACEs to scan and act on in the ACL\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_get_block acl_ndx 0 blck_rng_ntrs_to_act 0\r\n"
    "  blck_rng_ntrs_to_scn 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_GET_BLOCK_ACE_GET_BLOCK_BLOCK_RANGE_ENTRIES_TO_SCAN_ID,
    "blck_rng_ntrs_to_scn",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  block_range.entries_to_scan:\r\n"
    "  Number of ACEs to scan in the ACL.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_get_block acl_ndx 0 blck_rng_ntrs_to_act 0\r\n"
    "  blck_rng_ntrs_to_scn 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_REMOVE_ACE_REMOVE_ID,
    "ace_remove",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Remove ACE (Access Control Entry) from the given ACL. \r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_remove acl_ndx 0 ace_ndx 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_REMOVE_ACE_REMOVE_ACL_NDX_ID,
    "acl_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acl_ndx:\r\n"
    "  ACL ID to remove ACE from. Range 0-7.\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_remove acl_ndx 0 ace_ndx 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ACE_REMOVE_ACE_REMOVE_ACE_NDX_ID,
    "ace_ndx",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ace_ndx:\r\n"
    "  The entry index to add the ACE to. The smaller the ace_ndx,\r\n"
    "  the higher will be the priority of the ACE. Range 0-(ACL size-\r\n"
    "  1).\r\n"
    "",
    "Ex:\r\n"
    "  timna_app acl ace_remove acl_ndx 0 ace_ndx 0\r\n"
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
    PARAM_SWEEPT20_API_ACL_ID,
    "acl",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, 0, 0, 0, BIT(0)|BIT(1)|BIT(2)|BIT(3)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_SWEEPT20_STATUS_GET_ID,
    "status_get",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, 0, 0, 0, BIT(4)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

#endif
  {
    PARAM_SWEEPT20_SWEEPT20_MPLS_TE_CIR_ID,
    "cir",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Committed Information Rate. In Kbps",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(6) | BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_SWEEPT20_MPLS_TE_EIR_ID,
    "eir",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Excess Information Rate. In Kbps",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(6) | BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_SWEEPT20_MPLS_TE_CBS_ID,
    "cbs",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "committed burst size. In Bytes",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(6) | BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_SWEEPT20_MPLS_TE_EBS_ID,
    "ebs",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "excess burst size. In Bytes",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(6) | BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_SWEEPT20_MPLS_TE_SP_ID,
    "strict_priority_level",
    (PARAM_VAL_RULES *)&MPLS_TE_SP_LEVEL_rule[0],
    (sizeof(MPLS_TE_SP_LEVEL_rule) / sizeof(MPLS_TE_SP_LEVEL_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Excess Strict Priority Level.",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_SWEEPT20_MPLS_TE_WEIGHT_ID,
    "weight",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "  User may set the weight, to differentiate between other FECs \r\n"
    "  in regard to the Excess Information Rate",
    "",
    {0, 0, 0, 0, 0, 0, BIT(6) | BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_SWEEPT20_MPLS_TE_VC_FEC_QOS_SET_ID,
    "in_vc_qos_set",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "In VC label to which traffic is limited",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(7)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_SWEEPT20_MPLS_TE_EEP_QOS_SET_ID,
    "eep_qos_set",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Egress Encapsulation Pointer IDs to which traffic is limited",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(6)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_SWEEPT20_MPLS_TE_VC_FEC_QOS_GET_ID,
    "in_vc_qos_get",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "In VC label to which traffic is limited",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(5)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_SWEEPT20_MPLS_TE_EEP_QOS_GET_ID,
    "eep_qos_get",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "EEP ID to which traffic is limited",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(5)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_SWEEPT20_MPLS_TE_NOF_CONSEQUTIVE_IDS_ID,
    "nof_consequtive_elements",
    (PARAM_VAL_RULES *)&sweept20_api_free_vals[0],
    (sizeof(sweept20_api_free_vals) / sizeof(sweept20_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Enable commit the SET / GET operation above a range of VCs / EEPs",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(5)|BIT(6)|BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_SWEEPT20_MPLS_TE_ENABLE_ID,
    "enable",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "Should be called before any other MPLS TE CLI",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(8)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_SWEEPT20_SWEEPT20_MPLS_TE_ID,
    "mpls_te",
    (PARAM_VAL_RULES *)&sweept20_api_empty_vals[0],
    (sizeof(sweept20_api_empty_vals) / sizeof(sweept20_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, BIT(5) | BIT(6) | BIT(7)| BIT(8)},
    1,
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




#ifdef _MSC_VER
  #pragma pack(pop)
#endif

#ifdef  __cplusplus
}
#endif



#endif

#endif
