/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UI_ROM_DEFI_ACCESS_TIMNA_INCLUDED__
/* { */
#define __UI_ROM_DEFI_ACCESS_TIMNA_INCLUDED__

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

#include <appl/dpp/UserInterface/ui_pure_defi_access_timna.h>
#include <soc/dpp/Timna/timna_chip_defines.h>
#include <soc/dpp/Timna/timna_tables_update.h>
#include <soc/dpp/Timna/timna_api_acl.h>
#include <soc/dpp/Timna/timna_api_cid_res.h>
#include <soc/dpp/Timna/timna_api_edit.h>
#include <soc/dpp/Timna/timna_api_filter.h>
#include <soc/dpp/Timna/timna_api_mac_lkup.h>
#include <soc/dpp/Timna/timna_api_mngr.h>
/********************************************************/
/********************************************************/

EXTERN CONST
   PARAM_VAL_RULES
     Access_timna_table_names[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "pcp_encoding_table",
    {
      {
        TIMNA_PCP_ENCODING_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "port_da_not_found_fwd_table",
    {
      {
        TIMNA_PORT_DA_NOT_FOUND_FWD_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "up_to_dp_table",
    {
      {
        TIMNA_UP_TO_DP_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "tunnel_encap_table",
    {
      {
        TIMNA_TUNNEL_ENCAP_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "burst_size_table",
    {
      {
        TIMNA_BURST_SIZE_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cid_second_a_table",
    {
      {
        TIMNA_CID_SECOND_A_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cid_second_b_table",
    {
      {
        TIMNA_CID_SECOND_B_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cid_port_transmit_tagged_table",
    {
      {
        TIMNA_CID_PORT_TRANSMIT_TAGGED_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "dap_table",
    {
      {
        TIMNA_DAP_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "egp_cid_att_table",
    {
      {
        TIMNA_EGP_CID_ATT_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "port_term_table",
    {
      {
        TIMNA_PORT_TERM_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "bridge_ctrl_to_fwd_table",
    {
      {
        TIMNA_BRIDGE_CTRL_TO_FWD_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "port_config2_table",
    {
      {
        TIMNA_PORT_CONFIG2_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "stp_state_table",
    {
      {
        TIMNA_STP_STATE_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "arp_table",
    {
      {
        TIMNA_ARP_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "modulo_table",
    {
      {
        TIMNA_MODULO_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "vid_subnet_table",
    {
      {
        TIMNA_VID_SUBNET_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "port_sa_not_found_fwd_table",
    {
      {
        TIMNA_PORT_SA_NOT_FOUND_FWD_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "term_stamp_table",
    {
      {
        TIMNA_TERM_STAMP_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "tcam_mem_table",
    {
      {
        TIMNA_TCAM_MEM_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cid_da_not_found_fwd_table",
    {
      {
        TIMNA_CID_DA_NOT_FOUND_FWD_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "acl_action_table",
    {
      {
        TIMNA_ACL_ACTION_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "eth_type2_vid_table",
    {
      {
        TIMNA_ETH_TYPE2_VID_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "sap_table",
    {
      {
        TIMNA_SAP_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "vid_to_fid_table",
    {
      {
        TIMNA_VID_TO_FID_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cid_mpls_table",
    {
      {
        TIMNA_CID_MPLS_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cid_first_table",
    {
      {
        TIMNA_CID_FIRST_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ing_outlif_resolution_table",
    {
      {
        TIMNA_ING_OUTLIF_RESOLUTION_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "glag_outlif_table",
    {
      {
        TIMNA_GLAG_OUTLIF_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "port_vid_table",
    {
      {
        TIMNA_PORT_VID_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "tc_to_up_table",
    {
      {
        TIMNA_TC_TO_UP_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "glag_range_table",
    {
      {
        TIMNA_GLAG_RANGE_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "mantissa_division_table",
    {
      {
        TIMNA_MANTISSA_DIVISION_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "up_to_tc_table",
    {
      {
        TIMNA_UP_TO_TC_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "port_config1_table",
    {
      {
        TIMNA_PORT_CONFIG1_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "outport_config1_table",
    {
      {
        TIMNA_OUTPORT_CONFIG1_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "outlif_att_table",
    {
      {
        TIMNA_OUTLIF_ATT_TABLE_ID,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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

EXTERN CONST
   PARAM_VAL_RULES
     Access_timna_field_name[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "rev_rfu_4",
    {
      {
        TIMNA_FIELD_REV_RFU_4,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "chip_type",
    {
      {
        TIMNA_FIELD_CHIP_TYPE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "dbg_ver",
    {
      {
        TIMNA_FIELD_DBG_VER,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "chip_ver",
    {
      {
        TIMNA_FIELD_CHIP_VER,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "rldram_576mb",
    {
      {
        TIMNA_FIELD_RLDRAM_576MB,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "rld_mem_init_done",
    {
      {
        TIMNA_FIELD_RLD_MEM_INIT_DONE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "qdc_mem_init_done",
    {
      {
        TIMNA_FIELD_QDC_MEM_INIT_DONE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "qdb_mem_init_done",
    {
      {
        TIMNA_FIELD_QDB_MEM_INIT_DONE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "mem_init_done",
    {
      {
        TIMNA_FIELD_MEM_INIT_DONE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "enable_policer",
    {
      {
        TIMNA_FIELD_ENABLE_POLICER,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "enable_aging",
    {
      {
        TIMNA_FIELD_ENABLE_AGING,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "age_intrvl",
    {
      {
        TIMNA_FIELD_AGE_INTRVL,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "msg_data",
    {
      {
        TIMNA_FIELD_MSG_DATA,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "msg_data_1",
    {
      {
        TIMNA_FIELD_MSG_DATA_1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "msg_data_2",
    {
      {
        TIMNA_FIELD_MSG_DATA_2,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "msg_ready",
    {
      {
        TIMNA_FIELD_MSG_READY,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ind_cmd_start_end",
    {
      {
        TIMNA_FIELD_IND_CMD_START_END,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ind_cmd_fifo_full",
    {
      {
        TIMNA_FIELD_IND_CMD_FIFO_FULL,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ind_cmd_type",
    {
      {
        TIMNA_FIELD_IND_CMD_TYPE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ind_cmd_dm",
    {
      {
        TIMNA_FIELD_IND_CMD_DM,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ind_cmd_count",
    {
      {
        TIMNA_FIELD_IND_CMD_COUNT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ind_cmd_address",
    {
      {
        TIMNA_FIELD_IND_CMD_ADDRESS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ind_cmd_bank",
    {
      {
        TIMNA_FIELD_IND_CMD_BANK,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ind_cmd_mem_sel",
    {
      {
        TIMNA_FIELD_IND_CMD_MEM_SEL,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ind_cmd_ext",
    {
      {
        TIMNA_FIELD_IND_CMD_EXT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ind_cmd_data_rd",
    {
      {
        TIMNA_FIELD_IND_CMD_DATA_RD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ind_cmd_data_rd_1",
    {
      {
        TIMNA_FIELD_IND_CMD_DATA_RD_1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ind_cmd_data_rd_2",
    {
      {
        TIMNA_FIELD_IND_CMD_DATA_RD_2,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ind_cmd_data_we",
    {
      {
        TIMNA_FIELD_IND_CMD_DATA_WE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ind_cmd_data_we_1",
    {
      {
        TIMNA_FIELD_IND_CMD_DATA_WE_1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ind_cmd_data_we_2",
    {
      {
        TIMNA_FIELD_IND_CMD_DATA_WE_2,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "error",
    {
      {
        TIMNA_FIELD_ERROR,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "mpls_tunnel_end_min",
    {
      {
        TIMNA_FIELD_MPLS_TUNNEL_END_MIN,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "mpls_tunnel_end_max",
    {
      {
        TIMNA_FIELD_MPLS_TUNNEL_END_MAX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "mpls_tunnel_end_ipv4_min",
    {
      {
        TIMNA_FIELD_MPLS_TUNNEL_END_IPV4_MIN,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "mpls_tunnel_end_ipv4_max",
    {
      {
        TIMNA_FIELD_MPLS_TUNNEL_END_IPV4_MAX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "mpls_tunnel_end_mpls_min",
    {
      {
        TIMNA_FIELD_MPLS_TUNNEL_END_MPLS_MIN,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "mpls_tunnel_end_mpls_max",
    {
      {
        TIMNA_FIELD_MPLS_TUNNEL_END_MPLS_MAX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "mpls_pwe3_min",
    {
      {
        TIMNA_FIELD_MPLS_PWE3_MIN,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "mpls_pwe3_max",
    {
      {
        TIMNA_FIELD_MPLS_PWE3_MAX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "rlag2_min",
    {
      {
        TIMNA_FIELD_RLAG2_MIN,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "rlag2_max",
    {
      {
        TIMNA_FIELD_RLAG2_MAX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "rlag4_min",
    {
      {
        TIMNA_FIELD_RLAG4_MIN,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "rlag4_max",
    {
      {
        TIMNA_FIELD_RLAG4_MAX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_tpid_00",
    {
      {
        TIMNA_FIELD_CFG_TPID_00,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_tpid_01",
    {
      {
        TIMNA_FIELD_CFG_TPID_01,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_tpid_10",
    {
      {
        TIMNA_FIELD_CFG_TPID_10,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_tpid_11",
    {
      {
        TIMNA_FIELD_CFG_TPID_11,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_dot1_ad",
    {
      {
        TIMNA_FIELD_CFG_DOT1_AD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_cpu_destination0",
    {
      {
        TIMNA_FIELD_CFG_CPU_DESTINATION0,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_cpu_destination1",
    {
      {
        TIMNA_FIELD_CFG_CPU_DESTINATION1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_cpu_destination2",
    {
      {
        TIMNA_FIELD_CFG_CPU_DESTINATION2,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_cpu_destination3",
    {
      {
        TIMNA_FIELD_CFG_CPU_DESTINATION3,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_drop_eligible0",
    {
      {
        TIMNA_FIELD_CFG_DROP_ELIGIBLE0,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_drop_eligible1",
    {
      {
        TIMNA_FIELD_CFG_DROP_ELIGIBLE1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_acl_default_action_index",
    {
      {
        TIMNA_FIELD_CFG_ACL_DEFAULT_ACTION_INDEX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_ieee_rsrv_mc_cpu_code",
    {
      {
        TIMNA_FIELD_CFG_IEEE_RSRV_MC_CPU_CODE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_arp_fwd_type",
    {
      {
        TIMNA_FIELD_CFG_ARP_FWD_TYPE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_arp_fwd_destination_index",
    {
      {
        TIMNA_FIELD_CFG_ARP_FWD_DESTINATION_INDEX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_arp_fwd_cos",
    {
      {
        TIMNA_FIELD_CFG_ARP_FWD_COS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_arp_fwd_cpu_code",
    {
      {
        TIMNA_FIELD_CFG_ARP_FWD_CPU_CODE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_arp_fwd_policer_pointer",
    {
      {
        TIMNA_FIELD_CFG_ARP_FWD_POLICER_POINTER,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_arp_cos_overwrite",
    {
      {
        TIMNA_FIELD_CFG_ARP_COS_OVERWRITE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_arp_snoop_action",
    {
      {
        TIMNA_FIELD_CFG_ARP_SNOOP_ACTION,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_igmp0_fwd_type",
    {
      {
        TIMNA_FIELD_CFG_IGMP0_FWD_TYPE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_igmp0_fwd_destination_index",
    {
      {
        TIMNA_FIELD_CFG_IGMP0_FWD_DESTINATION_INDEX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_igmp0_fwd_cos",
    {
      {
        TIMNA_FIELD_CFG_IGMP0_FWD_COS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_igmp0_fwd_cpu_code",
    {
      {
        TIMNA_FIELD_CFG_IGMP0_FWD_CPU_CODE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_igmp0_fwd_policer_pointer",
    {
      {
        TIMNA_FIELD_CFG_IGMP0_FWD_POLICER_POINTER,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_igmp0_cos_overwrite",
    {
      {
        TIMNA_FIELD_CFG_IGMP0_COS_OVERWRITE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_igmp0_snoop_action",
    {
      {
        TIMNA_FIELD_CFG_IGMP0_SNOOP_ACTION,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_igmp1_fwd_type",
    {
      {
        TIMNA_FIELD_CFG_IGMP1_FWD_TYPE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_igmp1_fwd_destination_index",
    {
      {
        TIMNA_FIELD_CFG_IGMP1_FWD_DESTINATION_INDEX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_igmp1_fwd_cos",
    {
      {
        TIMNA_FIELD_CFG_IGMP1_FWD_COS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_igmp1_fwd_cpu_code",
    {
      {
        TIMNA_FIELD_CFG_IGMP1_FWD_CPU_CODE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_igmp1_fwd_policer_pointer",
    {
      {
        TIMNA_FIELD_CFG_IGMP1_FWD_POLICER_POINTER,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_igmp1_cos_overwrite",
    {
      {
        TIMNA_FIELD_CFG_IGMP1_COS_OVERWRITE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_igmp1_snoop_action",
    {
      {
        TIMNA_FIELD_CFG_IGMP1_SNOOP_ACTION,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_vrrp_fwd_type",
    {
      {
        TIMNA_FIELD_CFG_VRRP_FWD_TYPE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_vrrp_fwd_destination_index",
    {
      {
        TIMNA_FIELD_CFG_VRRP_FWD_DESTINATION_INDEX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_vrrp_fwd_cos",
    {
      {
        TIMNA_FIELD_CFG_VRRP_FWD_COS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_vrrp_fwd_cpu_code",
    {
      {
        TIMNA_FIELD_CFG_VRRP_FWD_CPU_CODE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_vrrp_fwd_policer_pointer",
    {
      {
        TIMNA_FIELD_CFG_VRRP_FWD_POLICER_POINTER,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_vrrp_cos_overwrite",
    {
      {
        TIMNA_FIELD_CFG_VRRP_COS_OVERWRITE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_vrrp_snoop_action",
    {
      {
        TIMNA_FIELD_CFG_VRRP_SNOOP_ACTION,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "eth_type_dsap_ssap_0",
    {
      {
        TIMNA_FIELD_ETH_TYPE_DSAP_SSAP_0,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "eth_type_dsap_ssap_1",
    {
      {
        TIMNA_FIELD_ETH_TYPE_DSAP_SSAP_1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "eth_type_dsap_ssap_2",
    {
      {
        TIMNA_FIELD_ETH_TYPE_DSAP_SSAP_2,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "eth_type_dsap_ssap_3",
    {
      {
        TIMNA_FIELD_ETH_TYPE_DSAP_SSAP_3,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "encapsultion_0",
    {
      {
        TIMNA_FIELD_ENCAPSULTION_0,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "encapsultion_1",
    {
      {
        TIMNA_FIELD_ENCAPSULTION_1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "encapsultion_2",
    {
      {
        TIMNA_FIELD_ENCAPSULTION_2,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "encapsultion_3",
    {
      {
        TIMNA_FIELD_ENCAPSULTION_3,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "l4ops_src_dst_port",
    {
      {
        TIMNA_FIELD_L4OPS_SRC_DST_PORT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "tcam_size",
    {
      {
        TIMNA_FIELD_TCAM_SIZE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cam_search_en",
    {
      {
        TIMNA_FIELD_CAM_SEARCH_EN,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "eib_fifo_error_latch",
    {
      {
        TIMNA_FIELD_EIB_FIFO_ERROR_LATCH,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "iib_fifo_error_latch",
    {
      {
        TIMNA_FIELD_IIB_FIFO_ERROR_LATCH,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "eom_fifo_error_latch",
    {
      {
        TIMNA_FIELD_EOM_FIFO_ERROR_LATCH,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "iom_fifo_error_latch",
    {
      {
        TIMNA_FIELD_IOM_FIFO_ERROR_LATCH,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "tg1_fifo_error_latch",
    {
      {
        TIMNA_FIELD_TG1_FIFO_ERROR_LATCH,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "tg2_fifo_error_latch",
    {
      {
        TIMNA_FIELD_TG2_FIFO_ERROR_LATCH,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ni1_fifo_error_latch",
    {
      {
        TIMNA_FIELD_NI1_FIFO_ERROR_LATCH,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ni2_fifo_error_latch",
    {
      {
        TIMNA_FIELD_NI2_FIFO_ERROR_LATCH,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "qdc_fifo_error_latch",
    {
      {
        TIMNA_FIELD_QDC_FIFO_ERROR_LATCH,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "qdb_fifo_error_latch",
    {
      {
        TIMNA_FIELD_QDB_FIFO_ERROR_LATCH,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "slp_fifo_error_latch",
    {
      {
        TIMNA_FIELD_SLP_FIFO_ERROR_LATCH,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "rd1_uflow_latch",
    {
      {
        TIMNA_FIELD_RD1_UFLOW_LATCH,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "rd1_oflow_latch",
    {
      {
        TIMNA_FIELD_RD1_OFLOW_LATCH,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "icmd_fifo_error_latch",
    {
      {
        TIMNA_FIELD_ICMD_FIFO_ERROR_LATCH,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "iom_ne_iib_e_latch",
    {
      {
        TIMNA_FIELD_IOM_NE_IIB_E_LATCH,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "iom_seq_id_mism_latch",
    {
      {
        TIMNA_FIELD_IOM_SEQ_ID_MISM_LATCH,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "iom_hdr_fifo_full_latch",
    {
      {
        TIMNA_FIELD_IOM_HDR_FIFO_FULL_LATCH,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "eom_hdr_fifo_full_latch",
    {
      {
        TIMNA_FIELD_EOM_HDR_FIFO_FULL_LATCH,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "eom_seq_id_mism_latch",
    {
      {
        TIMNA_FIELD_EOM_SEQ_ID_MISM_LATCH,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "tg1_enable_rx",
    {
      {
        TIMNA_FIELD_TG1_ENABLE_RX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "tg2_enable_rx",
    {
      {
        TIMNA_FIELD_TG2_ENABLE_RX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "tg1_enable_tx",
    {
      {
        TIMNA_FIELD_TG1_ENABLE_TX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "tg2_enable_tx",
    {
      {
        TIMNA_FIELD_TG2_ENABLE_TX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "tg1_ing_fc",
    {
      {
        TIMNA_FIELD_TG1_ING_FC,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "tg2_ing_fc",
    {
      {
        TIMNA_FIELD_TG2_ING_FC,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "tg1_flush_rx",
    {
      {
        TIMNA_FIELD_TG1_FLUSH_RX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "tg2_flush_rx",
    {
      {
        TIMNA_FIELD_TG2_FLUSH_RX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "tg1_flush_tx",
    {
      {
        TIMNA_FIELD_TG1_FLUSH_TX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "tg2_flush_tx",
    {
      {
        TIMNA_FIELD_TG2_FLUSH_TX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "tg1_link_ok",
    {
      {
        TIMNA_FIELD_TG1_LINK_OK,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "tg2_link_ok",
    {
      {
        TIMNA_FIELD_TG2_LINK_OK,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "frm_length_max",
    {
      {
        TIMNA_FIELD_FRM_LENGTH_MAX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "iib_err_pkt_cnt_mac0",
    {
      {
        TIMNA_FIELD_IIB_ERR_PKT_CNT_MAC0,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "iib_err_pkt_cnt_mac1",
    {
      {
        TIMNA_FIELD_IIB_ERR_PKT_CNT_MAC1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "iib_rx_pkt_cnt_mac0",
    {
      {
        TIMNA_FIELD_IIB_RX_PKT_CNT_MAC0,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "iib_rx_pkt_cnt_mac1",
    {
      {
        TIMNA_FIELD_IIB_RX_PKT_CNT_MAC1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "iib_drop_pkt_cnt_mac0",
    {
      {
        TIMNA_FIELD_IIB_DROP_PKT_CNT_MAC0,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "iib_drop_pkt_cnt_mac1",
    {
      {
        TIMNA_FIELD_IIB_DROP_PKT_CNT_MAC1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "eib_err_pkt_cnt_nif0",
    {
      {
        TIMNA_FIELD_EIB_ERR_PKT_CNT_NIF0,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "eib_err_pkt_cnt_nif1",
    {
      {
        TIMNA_FIELD_EIB_ERR_PKT_CNT_NIF1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "eib_rx_pkt_cnt_nif0",
    {
      {
        TIMNA_FIELD_EIB_RX_PKT_CNT_NIF0,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "eib_rx_pkt_cnt_nif1",
    {
      {
        TIMNA_FIELD_EIB_RX_PKT_CNT_NIF1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "eib_drop_pkt_cnt_nif0",
    {
      {
        TIMNA_FIELD_EIB_DROP_PKT_CNT_NIF0,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "eib_drop_pkt_cnt_nif1",
    {
      {
        TIMNA_FIELD_EIB_DROP_PKT_CNT_NIF1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "iom_pkt_cnt_mac0",
    {
      {
        TIMNA_FIELD_IOM_PKT_CNT_MAC0,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "iom_pkt_cnt_mac1",
    {
      {
        TIMNA_FIELD_IOM_PKT_CNT_MAC1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "eom_pkt_cnt_nif0",
    {
      {
        TIMNA_FIELD_EOM_PKT_CNT_NIF0,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "eom_pkt_cnt_nif1",
    {
      {
        TIMNA_FIELD_EOM_PKT_CNT_NIF1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ni1_flush_rx",
    {
      {
        TIMNA_FIELD_NI1_FLUSH_RX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ni2_flush_rx",
    {
      {
        TIMNA_FIELD_NI2_FLUSH_RX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ni1_flush_tx",
    {
      {
        TIMNA_FIELD_NI1_FLUSH_TX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ni2_flush_tx",
    {
      {
        TIMNA_FIELD_NI2_FLUSH_TX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ni1_link_ok",
    {
      {
        TIMNA_FIELD_NI1_LINK_OK,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ni2_link_ok",
    {
      {
        TIMNA_FIELD_NI2_LINK_OK,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ni1_ser_lpbk",
    {
      {
        TIMNA_FIELD_NI1_SER_LPBK,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ni2_ser_lpbk",
    {
      {
        TIMNA_FIELD_NI2_SER_LPBK,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "tg1_ser_lpbk",
    {
      {
        TIMNA_FIELD_TG1_SER_LPBK,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "tg2_ser_lpbk",
    {
      {
        TIMNA_FIELD_TG2_SER_LPBK,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "status_dip_ok",
    {
      {
        TIMNA_FIELD_STATUS_DIP_OK,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "ingres_tm_mode",
    {
      {
        TIMNA_FIELD_INGRES_TM_MODE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "bypass_egp",
    {
      {
        TIMNA_FIELD_BYPASS_EGP,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "egp_keep_dune_hdrs",
    {
      {
        TIMNA_FIELD_EGP_KEEP_DUNE_HDRS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "illegal_trap_2_cpu",
    {
      {
        TIMNA_FIELD_ILLEGAL_TRAP_2_CPU,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "bypass_inp",
    {
      {
        TIMNA_FIELD_BYPASS_INP,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "keep_inp",
    {
      {
        TIMNA_FIELD_KEEP_INP,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "naui_write",
    {
      {
        TIMNA_FIELD_NAUI_WRITE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "naui1_busy",
    {
      {
        TIMNA_FIELD_NAUI1_BUSY,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "naui2_busy",
    {
      {
        TIMNA_FIELD_NAUI2_BUSY,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "xaui_write",
    {
      {
        TIMNA_FIELD_XAUI_WRITE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "xaui1_busy",
    {
      {
        TIMNA_FIELD_XAUI1_BUSY,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "xaui2_busy",
    {
      {
        TIMNA_FIELD_XAUI2_BUSY,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "rx_eqctrl",
    {
      {
        TIMNA_FIELD_RX_EQCTRL,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "rx_eqdcgain",
    {
      {
        TIMNA_FIELD_RX_EQDCGAIN,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "tx_preemp_0t",
    {
      {
        TIMNA_FIELD_TX_PREEMP_0T,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "tx_preemp_1t",
    {
      {
        TIMNA_FIELD_TX_PREEMP_1T,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "tx_preemp_2t",
    {
      {
        TIMNA_FIELD_TX_PREEMP_2T,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "tx_vodctrl",
    {
      {
        TIMNA_FIELD_TX_VODCTRL,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cpu_acc_per_clk",
    {
      {
        TIMNA_FIELD_CPU_ACC_PER_CLK,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_eth_acl_template_ptr",
    {
      {
        TIMNA_FIELD_CFG_ETH_ACL_TEMPLATE_PTR,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_mac_src_port_bypass",
    {
      {
        TIMNA_FIELD_CFG_MAC_SRC_PORT_BYPASS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_da_intercept_policy_pointer_1",
    {
      {
        TIMNA_FIELD_CFG_DA_INTERCEPT_POLICY_POINTER_1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_da_intercept_policy_pointer_2",
    {
      {
        TIMNA_FIELD_CFG_DA_INTERCEPT_POLICY_POINTER_2,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_da_intercept_policy_pointer_3",
    {
      {
        TIMNA_FIELD_CFG_DA_INTERCEPT_POLICY_POINTER_3,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_da_control_policy_pointer_1",
    {
      {
        TIMNA_FIELD_CFG_DA_CONTROL_POLICY_POINTER_1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_da_control_policy_pointer_2",
    {
      {
        TIMNA_FIELD_CFG_DA_CONTROL_POLICY_POINTER_2,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_da_control_policy_pointer_3",
    {
      {
        TIMNA_FIELD_CFG_DA_CONTROL_POLICY_POINTER_3,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_da_lookup_cos_overwrite",
    {
      {
        TIMNA_FIELD_CFG_DA_LOOKUP_COS_OVERWRITE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_da_lookup_cos",
    {
      {
        TIMNA_FIELD_CFG_DA_LOOKUP_COS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_da_lookup_cpu_code",
    {
      {
        TIMNA_FIELD_CFG_DA_LOOKUP_CPU_CODE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_sa_drop_fwd_type",
    {
      {
        TIMNA_FIELD_CFG_SA_DROP_FWD_TYPE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_ingress_filter_enable",
    {
      {
        TIMNA_FIELD_CFG_INGRESS_FILTER_ENABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_8021x_authorized_eth_type",
    {
      {
        TIMNA_FIELD_CFG_8021X_AUTHORIZED_ETH_TYPE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_8021x_filter_control",
    {
      {
        TIMNA_FIELD_CFG_8021X_FILTER_CONTROL,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_policer_1_color_aware",
    {
      {
        TIMNA_FIELD_CFG_POLICER_1_COLOR_AWARE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_policer_1_cir",
    {
      {
        TIMNA_FIELD_CFG_POLICER_1_CIR,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_policer_1_eir",
    {
      {
        TIMNA_FIELD_CFG_POLICER_1_EIR,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_policer_1_cbs",
    {
      {
        TIMNA_FIELD_CFG_POLICER_1_CBS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_policer_1_ebs",
    {
      {
        TIMNA_FIELD_CFG_POLICER_1_EBS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_policer_2_color_aware",
    {
      {
        TIMNA_FIELD_CFG_POLICER_2_COLOR_AWARE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_policer_2_cir",
    {
      {
        TIMNA_FIELD_CFG_POLICER_2_CIR,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_policer_2_eir",
    {
      {
        TIMNA_FIELD_CFG_POLICER_2_EIR,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_policer_2_cbs",
    {
      {
        TIMNA_FIELD_CFG_POLICER_2_CBS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_policer_2_ebs",
    {
      {
        TIMNA_FIELD_CFG_POLICER_2_EBS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_policer_3_color_aware",
    {
      {
        TIMNA_FIELD_CFG_POLICER_3_COLOR_AWARE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_policer_3_cir",
    {
      {
        TIMNA_FIELD_CFG_POLICER_3_CIR,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_policer_3_eir",
    {
      {
        TIMNA_FIELD_CFG_POLICER_3_EIR,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_policer_3_cbs",
    {
      {
        TIMNA_FIELD_CFG_POLICER_3_CBS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_policer_3_ebs",
    {
      {
        TIMNA_FIELD_CFG_POLICER_3_EBS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_plain_bridging",
    {
      {
        TIMNA_FIELD_CFG_PLAIN_BRIDGING,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_lag_hash_index",
    {
      {
        TIMNA_FIELD_CFG_LAG_HASH_INDEX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_bridge_l3_lag_hash",
    {
      {
        TIMNA_FIELD_CFG_BRIDGE_L3_LAG_HASH,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_l4_lag_hash",
    {
      {
        TIMNA_FIELD_CFG_L4_LAG_HASH,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_enable_tunnel_lag_hash",
    {
      {
        TIMNA_FIELD_CFG_ENABLE_TUNNEL_LAG_HASH,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_lag_tunnel_protocol0",
    {
      {
        TIMNA_FIELD_CFG_LAG_TUNNEL_PROTOCOL0,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_lag_tunnel_protocol1",
    {
      {
        TIMNA_FIELD_CFG_LAG_TUNNEL_PROTOCOL1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_lag_tunnel_protocol2",
    {
      {
        TIMNA_FIELD_CFG_LAG_TUNNEL_PROTOCOL2,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_lag_tunnel_protocol3",
    {
      {
        TIMNA_FIELD_CFG_LAG_TUNNEL_PROTOCOL3,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_cpu_trap_destination",
    {
      {
        TIMNA_FIELD_CFG_CPU_TRAP_DESTINATION,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_ingress_policing_base",
    {
      {
        TIMNA_FIELD_CFG_INGRESS_POLICING_BASE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_default_traffic_class0",
    {
      {
        TIMNA_FIELD_CFG_DEFAULT_TRAFFIC_CLASS0,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_default_traffic_class1",
    {
      {
        TIMNA_FIELD_CFG_DEFAULT_TRAFFIC_CLASS1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_default_traffic_class2",
    {
      {
        TIMNA_FIELD_CFG_DEFAULT_TRAFFIC_CLASS2,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_default_traffic_class3",
    {
      {
        TIMNA_FIELD_CFG_DEFAULT_TRAFFIC_CLASS3,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_sa_device",
    {
      {
        TIMNA_FIELD_CFG_SA_DEVICE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_sa_device_1",
    {
      {
        TIMNA_FIELD_CFG_SA_DEVICE_1,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_ttl_00",
    {
      {
        TIMNA_FIELD_CFG_TTL_00,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_ttl_01",
    {
      {
        TIMNA_FIELD_CFG_TTL_01,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_ttl_10",
    {
      {
        TIMNA_FIELD_CFG_TTL_10,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "cfg_ttl_11",
    {
      {
        TIMNA_FIELD_CFG_TTL_11,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "inp_err_vector",
    {
      {
        TIMNA_FIELD_INP_ERR_VECTOR,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
    "egp_err_vector",
    {
      {
        TIMNA_FIELD_EGP_ERR_VECTOR,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)"    timna field.",
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
     Access_timna_free_vals[]
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
   PARAM_VAL
     Timna_default_unit
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)0
  }
}
#endif
;

EXTERN CONST
   PARAM_VAL
     Timna_zero
#ifdef INIT
   =
{
  VAL_NUMERIC,
  0,
  {
    /*
     * Casting may be required here only to keep the compiler silent.
     */
    (unsigned long)0
  }
}
#endif
;

EXTERN CONST
   PARAM_VAL_RULES
     Timna_ind_data[]
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
     Timna_tcam_data[]
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
/********************************************************/
EXTERN CONST
   PARAM
     Access_timna_params[]
#ifdef INIT
   =
{
  {
    PARAM_ACCESS_TIMNA_READ_FIELD_ID,
    "field_read",
    (PARAM_VAL_RULES *)&Access_timna_field_name[0],
    (sizeof(Access_timna_field_name) / sizeof(Access_timna_field_name[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  read timna field.\r\n"
    "",
    "Examples:\r\n"
    "  YY field_read start_driver\r\n"
    "",
    "",
    {BIT(0)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_ACCESS_TIMNA_WRITE_FIELD_ID,
    "field_write",
    (PARAM_VAL_RULES *)&Access_timna_field_name[0],
    (sizeof(Access_timna_field_name) / sizeof(Access_timna_field_name[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  write to timna field.\r\n"
    "",
    "Examples:\r\n"
    "  YY field_write XX value XX\r\n"
    "",
    "",
    {BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_ACCESS_TIMNA_WRITE_FIELD_VALUE_ID,
    "value",
    (PARAM_VAL_RULES *)&Access_timna_free_vals[0],
    (sizeof(Access_timna_free_vals) / sizeof(Access_timna_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  read timna field.\r\n"
    "",
    "Examples:\r\n"
    "  YY field_write 0 value XX\r\n"
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
    PARAM_ACCESS_TIMNA_DEVICE_ID,
    "unit",
    (PARAM_VAL_RULES *)&Access_timna_free_vals[0],
    (sizeof(Access_timna_free_vals) / sizeof(Access_timna_free_vals[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Timna_default_unit,
    0,0,0,0,0,0,0,
    "  specify device id\r\n"
    "",
    "Examples:\r\n"
    "  YY field_write XX value 0 unit 0\r\n"
    "",
    "",
    {BIT(0) | BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_ACCESS_TIMNA_READ_IND_ID,
    "ind_read",
    (PARAM_VAL_RULES *)&Access_timna_free_vals[0],
    (sizeof(Access_timna_free_vals) / sizeof(Access_timna_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  read ind table.\r\n"
    "",
    "Examples:\r\n"
    "  timna_acc ind_read 0x75776775\r\n"
    "  Read the indirect address 0x75776775",
    "",
    {0,BIT(0)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_ACCESS_TIMNA_READ_TO_ADD_ID,
    "to",
    (PARAM_VAL_RULES *)&Access_timna_free_vals[0],
    (sizeof(Timna_ind_data) / sizeof(Timna_ind_data[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Timna_zero,
    0,0,0,0,0,0,0,
    "  write to timna field.\r\n"
    "",
    "Examples:\r\n"
    "  timna_acc ind_read 0 to 10 is_external 1 tbl_id 2 bank 1",
    "",
    {0,BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_ACCESS_TIMNA_WRITE_IND_ID,
    "ind_write",
    (PARAM_VAL_RULES *)&Access_timna_free_vals[0],
    (sizeof(Access_timna_free_vals) / sizeof(Access_timna_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  write to timna field. MSB first. (In the API driver, it is LSB first)\r\n"
    "",
    "Examples:\r\n"
    "  timna_acc ind_write 0x75776775 data 0x00433343 0x00437343\r\n"
    "  Write the to indirect address 0x75776775 the values 0x00433343 0x00437343",
    "",
    {0,BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_ACCESS_TIMNA_WRITE_IND_VAL_ID,
    "value_msb",
    (PARAM_VAL_RULES *)&Timna_ind_data[0],
    (sizeof(Timna_ind_data) / sizeof(Timna_ind_data[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  write to timna field. MSB first. (In the API driver, it is LS U32 first)\r\n"
    "",
    "Examples:\r\n"
    "  timna_acc ind_write 0x75776775 data 0x00433343 0x00437343\r\n"
    "  Write the to indirect address 0x75776775 the values 0x00433343 0x00437343",
    "",
    {0,BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_ACCESS_TIMNA_WRITE_IS_EX_ID,
    "is_external",
    (PARAM_VAL_RULES *)&Access_timna_free_vals[0],
    (sizeof(Timna_ind_data) / sizeof(Timna_ind_data[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Timna_zero,
    0,0,0,0,0,0,0,
    "  write to timna field.\r\n"
    "",
    "Examples:\r\n"
    "  timna_acc ind_write 0 data 0x00433343 0x00437343 is_external 1 tbl_id 2 bank 1",
    "",
    {0,BIT(0) | BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_ACCESS_TIMNA_WRITE_TBL_ID,
    "tbl_id",
    (PARAM_VAL_RULES *)&Access_timna_free_vals[0],
    (sizeof(Timna_ind_data) / sizeof(Timna_ind_data[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Timna_zero,
    0,0,0,0,0,0,0,
    "  write to timna field.\r\n"
    "",
    "Examples:\r\n"
    "  timna_acc ind_write 0 data 0x00433343 0x00437343 is_external 1 tbl_id 2 bank 1",
    "",
    {0,BIT(0) | BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_ACCESS_TIMNA_WRITE_BANK_ID,
    "bank",
    (PARAM_VAL_RULES *)&Access_timna_free_vals[0],
    (sizeof(Timna_ind_data) / sizeof(Timna_ind_data[0])) - 1,
    HAS_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)&Timna_zero,
    0,0,0,0,0,0,0,
    "  write to timna field.\r\n"
    "",
    "Examples:\r\n"
    "  timna_acc ind_write 0 data 0x00433343 0x00437343 is_external 1 tbl_id 2 bank 1",
    "",
    {0,BIT(0) | BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_ACCESS_TIMNA_IND_TABLE_ID,
    "table_name",
    (PARAM_VAL_RULES *)&Access_timna_table_names[0],
    (sizeof(Access_timna_table_names) / sizeof(Access_timna_table_names[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  table name.\r\n"
    "",
    "Examples:\r\n"
    "  timna_acc ind_write 0 data 0x00433343 0x00437343 table_name pcp_encoding_table\r\n"
    "",
    "",
    {0,BIT(0) | BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_ACCESS_TIMNA_TCAM_WRITE_ID,
    "tcam_write",
    (PARAM_VAL_RULES *)&Access_timna_free_vals[0],
    (sizeof(Access_timna_free_vals) / sizeof(Access_timna_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  write a TCAM entry.\r\n"
    "",
    "Examples:\r\n"
    "  timna_acc tcam_write 0x75776775 data 0x00433343 0x00437343\r\n"
    "  Write the to TCAM address 0x75776775 the values 0x00433343 0x00437343",
    "",
    {0,0,BIT(0)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_ACCESS_TIMNA_TCAM_INIT_ID,
    "tcam_init",
    (PARAM_VAL_RULES *)&Access_timna_free_vals[0],
    (sizeof(Access_timna_free_vals) / sizeof(Access_timna_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  write a TCAM entry.\r\n"
    "",
    "Examples:\r\n"
    "  timna_acc tcam_init 0\r\n"
    "  Init the TCAM",
    "",
    {0,0,BIT(1)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_ACCESS_TIMNA_TCAM_WRITE_VAL_ID,
    "value",
    (PARAM_VAL_RULES *)&Timna_tcam_data[0],
    (sizeof(Timna_tcam_data) / sizeof(Timna_tcam_data[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tcam_write.\r\n"
    "",
    "Examples:\r\n"
    "  timna_acc tcam_write 0x75776775 data 0x00433343 0x00437343\r\n"
    "  Write the to indirect address 0x75776775 the values 0x00433343 0x00437343",
    "",
    {0,0,BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_ACCESS_TIMNA_TCAM_WRITE_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&Timna_tcam_data[0],
    (sizeof(Timna_tcam_data) / sizeof(Timna_tcam_data[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tcam_write.\r\n"
    "",
    "Examples:\r\n"
    "  timna_acc tcam_write 0x75776775 data 0x00433343 0x00437343\r\n"
    "  Write the to indirect address 0x75776775 the values 0x00433343 0x00437343",
    "",
    {0,0,BIT(0)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_ACCESS_TIMNA_TCAM_READ_ID,
    "tcam_read",
    (PARAM_VAL_RULES *)&Access_timna_free_vals[0],
    (sizeof(Access_timna_free_vals) / sizeof(Access_timna_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  read a TCAM entry.\r\n"
    "",
    "Examples:\r\n"
    "  timna_acc tcam_read 0x75776775 ",
    "",
    {0,0,BIT(2)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_ACCESS_TIMNA_TCAM_CLEAR_ENTRY_ID,
    "tcam_clear_entry",
    (PARAM_VAL_RULES *)&Access_timna_free_vals[0],
    (sizeof(Access_timna_free_vals) / sizeof(Access_timna_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  read a TCAM entry.\r\n"
    "",
    "Examples:\r\n"
    "  tcam_clear_entry tcam_entry_clear 0x75776775 ",
    "",
    {0,0,BIT(3)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_ACCESS_TIMNA_TCAM_COMPARE_ID,
    "tcam_compare",
    (PARAM_VAL_RULES *)&Access_timna_free_vals[0],
    (sizeof(Access_timna_free_vals) / sizeof(Access_timna_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  find first match entry in the TCAM.\r\n"
    "",
    "Examples:\r\n"
    "  timna_acc tcam_compare value 0x75776775",
    "",
    {0,0,BIT(4)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_ACCESS_TIMNA_TCAM_COMPARE_VAL_ID,
    "value",
    (PARAM_VAL_RULES *)&Timna_tcam_data[0],
    (sizeof(Timna_tcam_data) / sizeof(Timna_tcam_data[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tcam_write.\r\n"
    "",
    "Examples:\r\n"
    "  timna_acc tcam_compare value 0x75776775",
    "",
    {0,0,BIT(4)},
    2,
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


/* } __UI_ROM_DEFI_ACCESS_TIMNA_INCLUDED__*/
#endif
