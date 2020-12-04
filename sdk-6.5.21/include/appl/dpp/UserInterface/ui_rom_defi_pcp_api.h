/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __UI_ROM_DEFI_PCP_API_INCLUDED__
/* { */
#define __UI_ROM_DEFI_PCP_API_INCLUDED__

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <appl/dpp/UserInterface/ui_pure_defi_pcp_api.h>
#include <soc/dpp/PCP/pcp_general.h>
#include <soc/dpp/PCP/pcp_oam_api_general.h>
#include <soc/dpp/PCP/pcp_oam_api_eth.h>
#include <soc/dpp/PCP/pcp_oam_api_bfd.h>
#include <soc/dpp/PCP/pcp_oam_api_mpls.h>
#include <soc/dpp/PCP/pcp_api_mgmt.h>


#define UI_MGMT
#define UI_STATISTICS
#define UI_DIAGNOSTICS
#define UI_GENERAL
#define UI_ETH
#define UI_BFD
#define UI_MPLS
#define UI_FRAMEWORK
 /* 
  * ENUM RULES 
  * {
  */

/********************************************************/
/********************************************************/

EXTERN CONST
   PARAM_VAL_RULES
     pcp_api_ipv4_addr_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        8,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    IPV6 address parameter. 8 hex digits. No 0 no 0x Prefix",
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
     pcp_api_ipv6_addr_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        32,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    IPV6 address parameter. 12 hex digits. No 0 no 0x Prefix",
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
     pcp_api_mac_addr_vals[]
#ifdef INIT
   =
{
  {
    VAL_TEXT,
    (char *)0,
    {
      {
          /*
           * Maximal number of characters.
           */
        12,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)"    MAC address parameter. 12 hex digits. No 0 no 0x Prefix",
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
     PCP_DEST_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "queue",
    {
      {
        PCP_DEST_TYPE_QUEUE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  PCP_DEST_TYPE.PCP_DEST_TYPE_QUEUE:\r\n"
        "  Destination type: Incoming Queue.Matching Index Range: 0 - 32K-1\r\n"
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
    "multicast",
    {
      {
        PCP_DEST_TYPE_MULTICAST,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  PCP_DEST_TYPE.PCP_DEST_TYPE_MULTICAST:\r\n"
        "  Destination type: Multicast Group.Matching Index Range: 0 - 16K-1\r\n"
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
    "sys_phy_port",
    {
      {
        PCP_DEST_TYPE_SYS_PHY_PORT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  PCP_DEST_TYPE.PCP_DEST_TYPE_SYS_PHY_PORT:\r\n"
        "  System Physical FAP Port.Matching Index Range: 0 - 4095.\r\n"
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
        PCP_DEST_TYPE_LAG,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  PCP_DEST_TYPE.PCP_DEST_TYPE_LAG:\r\n"
        "  System LAG Id. Matching Index Range: 0 - 255.\r\n"
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
     PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_rule[] 
#ifdef INIT 
   = 
{ 
  { 
    VAL_SYMBOL, 
    "mbit_288", 
    { 
      { 
        PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_288, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_MGMT_HW_IF_DRAM_SIZE_MBIT.PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_288:\n\r"
        "  RLDRAM memory size, 288 Mbits\n\r"
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
    "mbit_576", 
    { 
      { 
        PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_576, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_MGMT_HW_IF_DRAM_SIZE_MBIT.PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_576:\n\r"
        "  RLDRAM memory size, 576 Mbits\n\r"
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
     PCP_MGMT_HW_IF_QDR_SIZE_MBIT_rule[] 
#ifdef INIT 
   = 
{ 
  { 
    VAL_SYMBOL, 
    "pcp_mgmt_hw_if_qdr_size_mbit_18", 
    { 
      { 
        PCP_MGMT_HW_IF_QDR_SIZE_MBIT_18, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_MGMT_HW_IF_QDR_SIZE_MBIT .PCP_MGMT_HW_IF_QDR_SIZE_MBIT_18:\n\r"
        "  DRAM memory size, 18 Mbits\n\r"
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
    "pcp_mgmt_hw_if_qdr_size_mbit_36", 
    { 
      { 
        PCP_MGMT_HW_IF_QDR_SIZE_MBIT_36, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_MGMT_HW_IF_QDR_SIZE_MBIT .PCP_MGMT_HW_IF_QDR_SIZE_MBIT_36:\n\r"
        "  DRAM memory size, 36 Mbits\n\r"
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
     PCP_MGMT_ELK_LKP_MODE_rule[] 
#ifdef INIT 
   = 
{ 
  { 
    VAL_SYMBOL, 
    "em", 
    { 
      { 
        PCP_MGMT_ELK_LKP_MODE_EM, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_MGMT_ELK_LKP_MODE.PCP_MGMT_ELK_LKP_MODE_EM:\n\r"
        "  All ELK memory is used for exact match lookups (e.g., MAC, ILM...)\n\r"
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
    "lpm", 
    { 
      { 
        PCP_MGMT_ELK_LKP_MODE_LPM, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_MGMT_ELK_LKP_MODE.PCP_MGMT_ELK_LKP_MODE_LPM:\n\r"
        "  All ELK memory is used for LPM lookups (vrf + DIP)\n\r"
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
    "both", 
    { 
      { 
        PCP_MGMT_ELK_LKP_MODE_BOTH, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_MGMT_ELK_LKP_MODE.PCP_MGMT_ELK_LKP_MODE_BOTH:\n\r"
        "  Half of ELK memory is used for EM lookups, and the other half is used for LPM\n\r"
        "  lookups\n\r"
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
     PCP_OAM_MSG_EXCEPTION_TYPE_rule[] 
#ifdef INIT 
   = 
{ 
  { 
    VAL_SYMBOL, 
    "pcp_oam_msg_none", 
    { 
      { 
        PCP_OAM_MSG_NONE, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MSG_EXCEPTION_TYPE.PCP_OAM_MSG_NONE:\n\r"
        "  CCM session (identified by local MEP handle and remote MEP ID) has timed out.\n\r"
        "  info1 returns the local MEP handle. info2 returns the ID of the remote MEP. \n\r"
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
    "pcp_oam_msg_ccm_timeout", 
    { 
      { 
        PCP_OAM_MSG_CCM_TIMEOUT, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MSG_EXCEPTION_TYPE.PCP_OAM_MSG_CCM_TIMEOUT:\n\r"
        "  CCM session (identified by local MEP handle and remote MEP ID) has timed out.\n\r"
        "  info1 returns the local MEP handle. info2 returns the ID of the remote MEP. \n\r"
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
    "pcp_oam_msg_bfd_timeout", 
    { 
      { 
        PCP_OAM_MSG_BFD_TIMEOUT, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MSG_EXCEPTION_TYPE.PCP_OAM_MSG_BFD_TIMEOUT:\n\r"
        "  BFD session has timed out. info1 returns My-discriminator of the timed-out\n\r"
        "  session. info2 is not used.\n\r"
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
    "pcp_oam_msg_mpls_dlockv_set", 
    { 
      { 
        PCP_OAM_MSG_MPLS_DLOCKV_SET, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MSG_EXCEPTION_TYPE.PCP_OAM_MSG_MPLS_DLOCKV_SET:\n\r"
        "  Monitored LSP has timed out. info1 returns the label of the timed-out\n\r"
        "  LSPInfo2 returns: '0' if monitored LSP is PWE'1' if monitored LSP is Tunnel\n\r"
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
    "pcp_oam_msg_mpls_dlockv_clear", 
    { 
      { 
        PCP_OAM_MSG_MPLS_DLOCKV_CLEAR, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MSG_EXCEPTION_TYPE.PCP_OAM_MSG_MPLS_DLOCKV_CLEAR:\n\r"
        "  Monitored LSP has recovered. info1 returns the label of the recovered\n\r"
        "  LSPInfo2 returns: '0' if monitored LSP is PWE'1' if monitored LSP is Tunnel\n\r"
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
    "pcp_oam_msg_mpls_fdi_clear", 
    { 
      { 
        PCP_OAM_MSG_MPLS_FDI_CLEAR, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MSG_EXCEPTION_TYPE.PCP_OAM_MSG_MPLS_FDI_CLEAR:\n\r"
        "  FDI state of the monitored LSP has been cleared (FDI messages ceased\n\r"
        "  arriving). info1 returns the label of the monitored LSPInfo2 returns: '0' if\n\r"
        "  monitored LSP is PWE'1' if monitored LSP is Tunnel\n\r"
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
    "pcp_oam_msg_mpls_bdi_clear", 
    { 
      { 
        PCP_OAM_MSG_MPLS_BDI_CLEAR, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MSG_EXCEPTION_TYPE.PCP_OAM_MSG_MPLS_BDI_CLEAR:\n\r"
        "  BDI state of the monitored LSP has been cleared (BDI messages ceased\n\r"
        "  arriving). info1 returns the label of the monitored LSPInfo2 returns: '0' if\n\r"
        "  monitored LSP is PWE'1' if monitored LSP is Tunnel\n\r"
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
    "pcp_oam_msg_mpls_dexcess_set", 
    { 
      { 
        PCP_OAM_MSG_MPLS_DEXCESS_SET, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MSG_EXCEPTION_TYPE.PCP_OAM_MSG_MPLS_DEXCESS_SET:\n\r"
        "  Monitored LSP has received dExcess message. info1 returns the label of the\n\r"
        "  timed-out LSPInfo2 returns: '0' if monitored LSP is PWE'1' if monitored LSP\n\r"
        "  is Tunnel\n\r"
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
    "pcp_oam_msg_mpls_dexcess_clear", 
    { 
      { 
        PCP_OAM_MSG_MPLS_DEXCESS_CLEAR, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MSG_EXCEPTION_TYPE.PCP_OAM_MSG_MPLS_DEXCESS_CLEAR:\n\r"
        "  Monitored LSP has recovered. info1 returns the label of the recovered\n\r"
        "  LSPInfo2 returns: '0' if monitored LSP is PWE'1' if monitored LSP is Tunnel\n\r"
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
    "pcp_oam_msg_unknown", 
    { 
      { 
        PCP_OAM_MSG_UNKNOWN, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MSG_EXCEPTION_TYPE.PCP_OAM_MSG_UNKNOWN:\n\r"
        "  The message type arrived is unidentified\n\r"
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
     PCP_OAM_ETH_MEP_DIRECTION_rule[] 
#ifdef INIT 
   = 
{ 
  { 
    VAL_SYMBOL, 
    "up", 
    { 
      { 
        PCP_OAM_ETH_MEP_DIRECTION_UP, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_ETH_MEP_DIRECTION.PCP_OAM_ETH_MEP_DIRECTION_UP:\n\r"
        "  Up MEP is a MEP that is external to the monitored domain.\n\r"
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
    "down", 
    { 
      { 
        PCP_OAM_ETH_MEP_DIRECTION_DOWN, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_ETH_MEP_DIRECTION.PCP_OAM_ETH_MEP_DIRECTION_DOWN:\n\r"
        "  Down MEP is a MEP that is internal to the monitored domain.\n\r"
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
     PCP_OAM_ETH_INTERVAL_rule[] 
#ifdef INIT 
   = 
{ 
  { 
    VAL_SYMBOL, 
    "interval_0", 
    { 
      { 
        PCP_OAM_ETH_INTERVAL_0, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_ETH_INTERVAL.PCP_OAM_ETH_INTERVAL_0:\n\r"
        "  CCM every 3.3 milliseconds\n\r"
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
    "interval_1", 
    { 
      { 
        PCP_OAM_ETH_INTERVAL_1, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_ETH_INTERVAL.PCP_OAM_ETH_INTERVAL_1:\n\r"
        "  CCM every 10 milliseconds\n\r"
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
    "interval_2", 
    { 
      { 
        PCP_OAM_ETH_INTERVAL_2, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_ETH_INTERVAL.PCP_OAM_ETH_INTERVAL_2:\n\r"
        "  CCM every 100 milliseconds\n\r"
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
    "interval_3", 
    { 
      { 
        PCP_OAM_ETH_INTERVAL_3, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_ETH_INTERVAL.PCP_OAM_ETH_INTERVAL_3:\n\r"
        "  CCM every 1 seconds\n\r"
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
    "interval_4", 
    { 
      { 
        PCP_OAM_ETH_INTERVAL_4, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_ETH_INTERVAL.PCP_OAM_ETH_INTERVAL_4:\n\r"
        "  CCM every 10 seconds\n\r"
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
    "interval_5", 
    { 
      { 
        PCP_OAM_ETH_INTERVAL_5, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_ETH_INTERVAL.PCP_OAM_ETH_INTERVAL_5:\n\r"
        "  CCM every 1 minutes\n\r"
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
    "interval_6", 
    { 
      { 
        PCP_OAM_ETH_INTERVAL_6, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_ETH_INTERVAL.PCP_OAM_ETH_INTERVAL_6:\n\r"
        "  CCM every 10 minutes\n\r"
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
     PCP_OAM_BFD_TYPE_rule[] 
#ifdef INIT 
   = 
{ 
  { 
    VAL_SYMBOL, 
    "mpls", 
    { 
      { 
        PCP_OAM_BFD_TYPE_MPLS, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_BFD_TYPE.PCP_OAM_BFD_TYPE_MPLS:\n\r"
        "  Tunnel label, routable source IP, destination IP 127/8, protocol UDP, UDP\n\r"
        "  port 3784, BFD PDU\n\r"
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
    "mpls_pwe", 
    { 
      { 
        PCP_OAM_BFD_TYPE_MPLS_PWE, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_BFD_TYPE.PCP_OAM_BFD_TYPE_MPLS_PWE:\n\r"
        "  Tunnel label, PWE label, CW 0x21, routable source IP, destination IP,\n\r"
        "  protocol UDP, UDP port 3784, BFD PDU\n\r"
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
    "mpls_pwe_with_ip", 
    { 
      { 
        PCP_OAM_BFD_TYPE_MPLS_PWE_WITH_IP, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_BFD_TYPE.PCP_OAM_BFD_TYPE_MPLS_PWE_WITH_IP:\n\r"
        "  Tunnel label, PWE label, CW 0x7, BFD PDU\n\r"
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
    "ip", 
    { 
      { 
        PCP_OAM_BFD_TYPE_IP, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_BFD_TYPE.PCP_OAM_BFD_TYPE_IP:\n\r"
        "  Routable source IP, routable destination IP, protocol UDP, UDP port 3784, BFD\n\r"
        "  PDU\n\r"
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
     PCP_OAM_BFD_TX_RATE_rule[] 
#ifdef INIT 
   = 
{ 
  { 
    VAL_SYMBOL, 
    "rate_1", 
    { 
      { 
        PCP_OAM_BFD_TX_RATE_1, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_BFD_TX_RATE.PCP_OAM_BFD_TX_RATE_1:\n\r"
        "  1 PPS (every 1000 ms)\n\r"
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
    "rate_2", 
    { 
      { 
        PCP_OAM_BFD_TX_RATE_2, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_BFD_TX_RATE.PCP_OAM_BFD_TX_RATE_2:\n\r"
        "  2 PPS (every 500 ms)\n\r"
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
    "rate_5", 
    { 
      { 
        PCP_OAM_BFD_TX_RATE_5, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_BFD_TX_RATE.PCP_OAM_BFD_TX_RATE_5:\n\r"
        "  5 PPS (every 200 ms)\n\r"
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
    "rate_10", 
    { 
      { 
        PCP_OAM_BFD_TX_RATE_10, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_BFD_TX_RATE.PCP_OAM_BFD_TX_RATE_10:\n\r"
        "  10 PPS (every 100 ms)\n\r"
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
    "rate_20", 
    { 
      { 
        PCP_OAM_BFD_TX_RATE_20, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_BFD_TX_RATE.PCP_OAM_BFD_TX_RATE_20:\n\r"
        "  20 PPS (every 50 ms)\n\r"
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
    "rate_50", 
    { 
      { 
        PCP_OAM_BFD_TX_RATE_50, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_BFD_TX_RATE.PCP_OAM_BFD_TX_RATE_50:\n\r"
        "  50 PPS (every 20 ms)\n\r"
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
    "rate_100", 
    { 
      { 
        PCP_OAM_BFD_TX_RATE_100, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_BFD_TX_RATE.PCP_OAM_BFD_TX_RATE_100:\n\r"
        "  100 PPS (every 10 ms)\n\r"
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
     PCP_OAM_MPLS_CC_PKT_TYPE_rule[] 
#ifdef INIT 
   = 
{ 
  { 
    VAL_SYMBOL, 
    "ffd_configurable", 
    { 
      { 
        PCP_OAM_MPLS_CC_PKT_TYPE_FFD_CONFIGURABLE, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_CC_PKT_TYPE.PCP_OAM_MPLS_CC_PKT_TYPE_FFD_CONFIGURABLE:\n\r"
        "  Configurable rate\n\r"
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
    "ffd_10", 
    { 
      { 
        PCP_OAM_MPLS_CC_PKT_TYPE_FFD_10, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_CC_PKT_TYPE.PCP_OAM_MPLS_CC_PKT_TYPE_FFD_10:\n\r"
        "  Frame (FFD) each 10 ms\n\r"
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
    "ffd_20", 
    { 
      { 
        PCP_OAM_MPLS_CC_PKT_TYPE_FFD_20, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_CC_PKT_TYPE.PCP_OAM_MPLS_CC_PKT_TYPE_FFD_20:\n\r"
        "  Frame (FFD) each 20 ms\n\r"
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
    "ffd_50", 
    { 
      { 
        PCP_OAM_MPLS_CC_PKT_TYPE_FFD_50, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_CC_PKT_TYPE.PCP_OAM_MPLS_CC_PKT_TYPE_FFD_50:\n\r"
        "  Frame (FFD) each 50 ms\n\r"
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
    "ffd_100", 
    { 
      { 
        PCP_OAM_MPLS_CC_PKT_TYPE_FFD_100, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_CC_PKT_TYPE.PCP_OAM_MPLS_CC_PKT_TYPE_FFD_100:\n\r"
        "  Frame (FFD) each 100 ms\n\r"
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
    "ffd_200", 
    { 
      { 
        PCP_OAM_MPLS_CC_PKT_TYPE_FFD_200, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_CC_PKT_TYPE.PCP_OAM_MPLS_CC_PKT_TYPE_FFD_200:\n\r"
        "  Frame (FFD) each 200 ms\n\r"
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
    "ffd_500", 
    { 
      { 
        PCP_OAM_MPLS_CC_PKT_TYPE_FFD_500, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_CC_PKT_TYPE.PCP_OAM_MPLS_CC_PKT_TYPE_FFD_500:\n\r"
        "  Frame (FFD) each 500 ms\n\r"
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
    "ffd_1000", 
    { 
      { 
        PCP_OAM_MPLS_CC_PKT_TYPE_FFD_1000, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_CC_PKT_TYPE.PCP_OAM_MPLS_CC_PKT_TYPE_FFD_1000:\n\r"
        "  Frame (FFD) each 1000 ms\n\r"
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
    "cv", 
    { 
      { 
        PCP_OAM_MPLS_CC_PKT_TYPE_CV, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_CC_PKT_TYPE.PCP_OAM_MPLS_CC_PKT_TYPE_CV:\n\r"
        "  Frame (CV) each 1000 ms\n\r"
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
     PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_rule[] 
#ifdef INIT 
   = 
{ 
  { 
    VAL_SYMBOL, 
    "server", 
    { 
      { 
        PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_SERVER, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE.PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_SERVER:\n\r"
        "  As defined in ITU Y.1711\n\r"
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
    "peer_me", 
    { 
      { 
        PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_PEER_ME, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE.PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_PEER_ME:\n\r"
        "  As defined in ITU Y.1711\n\r"
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
    "dlocv", 
    { 
      { 
        PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_DLOCV, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE.PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_DLOCV:\n\r"
        "  As defined in ITU Y.1711\n\r"
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
    "ttsi_mismatch", 
    { 
      { 
        PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_TTSI_MISMATCH, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE.PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_TTSI_MISMATCH:\n\r"
        "  As defined in ITU Y.1711\n\r"
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
    "ttsi_mismerge", 
    { 
      { 
        PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_TTSI_MISMERGE, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE.PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_TTSI_MISMERGE:\n\r"
        "  As defined in ITU Y.1711\n\r"
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
    "excess", 
    { 
      { 
        PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_EXCESS, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE.PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_EXCESS:\n\r"
        "  As defined in ITU Y.1711\n\r"
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
    "unknown", 
    { 
      { 
        PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_UNKNOWN, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE.PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_UNKNOWN:\n\r"
        "  As defined in ITU Y.1711\n\r"
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
    "none", 
    { 
      { 
        PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_NONE, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE.PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_NONE:\n\r"
        "  As defined in ITU Y.1711\n\r"
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
     PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_rule[] 
#ifdef INIT 
   = 
{ 
  { 
    VAL_SYMBOL, 
    "server", 
    { 
      { 
        PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_SERVER, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE.PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_SERVER:\n\r"
        "  As defined in ITU Y.1711\n\r"
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
    "peer_me", 
    { 
      { 
        PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_PEER_ME, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE.PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_PEER_ME:\n\r"
        "  As defined in ITU Y.1711\n\r"
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
    "dlocv", 
    { 
      { 
        PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_DLOCV, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE.PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_DLOCV:\n\r"
        "  As defined in ITU Y.1711\n\r"
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
    "ttsi_mismatch", 
    { 
      { 
        PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_TTSI_MISMATCH, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE.PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_TTSI_MISMATCH:\n\r"
        "  As defined in ITU Y.1711\n\r"
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
    "ttsi_mismerge", 
    { 
      { 
        PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_TTSI_MISMERGE, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE.PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_TTSI_MISMERGE:\n\r"
        "  As defined in ITU Y.1711\n\r"
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
    "excess", 
    { 
      { 
        PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_EXCESS, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE.PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_EXCESS:\n\r"
        "  As defined in ITU Y.1711\n\r"
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
    "unknown", 
    { 
      { 
        PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_UNKNOWN, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE.PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_UNKNOWN:\n\r"
        "  As defined in ITU Y.1711\n\r"
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
    "none", 
    { 
      { 
        PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_NONE, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE.PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE_NONE:\n\r"
        "  As defined in ITU Y.1711\n\r"
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
    "pcp_oam_mpls_lsp_rx_defect_filter_disable_defect_set", 
    { 
      { 
        PCP_OAM_MPLS_LSP_RX_DEFECT_FILTER_DISABLE_DEFECT_SET, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE.PCP_OAM_MPLS_LSP_RX_DEFECT_FILTER_DISABLE_DEF\n\r"
        "  CT_SET:\n\r"
        "  As defined in ITU Y.1711\n\r"
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
    "pcp_oam_mpls_lsp_rx_defect_filter_disable_defect_clear", 
    { 
      { 
        PCP_OAM_MPLS_LSP_RX_DEFECT_FILTER_DISABLE_DEFECT_CLEAR, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  PCP_OAM_MPLS_LSP_RX_DEFECT_TYPE.PCP_OAM_MPLS_LSP_RX_DEFECT_FILTER_DISABLE_DEF\n\r"
        "  CT_CLEAR:\n\r"
        "  As defined in ITU Y.1711\n\r"
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
     pcp_api_free_vals[] 
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
     pcp_api_empty_vals[] 
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
     pcp_api_params[] 
#ifdef INIT 
   = 
{ 
#ifdef UI_DEFAULT_SECTION/* { default_section*/
#endif /* } default_section*/
#ifdef UI_GENERAL/* { general*/
#endif /* } general*/
#ifdef UI_MGMT/* { mgmt*/
  { 
    PARAM_PCP_REGISTER_DEVICE_REGISTER_DEVICE_ID, 
    "register_device", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This procedure registers a new device to be taken care of by this device\n\r"
    "  driver. Physical device must be accessible by CPU when this call is made\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt register_device base_address 0 reset_device_ptr 0 unit_ptr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_REGISTER_DEVICE_REGISTER_DEVICE_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_REGISTER_DEVICE_REGISTER_DEVICE_BASE_ADDRESS_ID, 
    "base_address", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  base_address:\n\r"
    "  Base address of direct access memory assigned for device's registers. This\n\r"
    "  parameter needs to be specified even if physical access to device is not by\n\r"
    "  direct access memory since all logic, within driver, up to actual physical\n\r"
    "  access, assumes 'virtual' direct access memory. Memory block assigned by this\n\r"
    "  pointer must not overlap other memory blocks in user's system and certainly\n\r"
    "  not memory blocks assigned to other devices using this procedure.	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt register_device base_address 0 reset_device_ptr 0 unit_ptr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_REGISTER_DEVICE_REGISTER_DEVICE_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_REGISTER_DEVICE_REGISTER_DEVICE_RESET_DEVICE_PTR_ID, 
    "reset_device_ptr", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  reset_device_ptr:\n\r"
    "  BSP-function for device reset. Refer to 'SOC_SAND_RESET_DEVICE_FUNC_PTR'\n\r"
    "  definition	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt register_device base_address 0 reset_device_ptr 0 unit_ptr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_REGISTER_DEVICE_REGISTER_DEVICE_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_REGISTER_DEVICE_REGISTER_DEVICE_DEVICE_ID_PTR_ID, 
    "unit_ptr", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  unit_ptr:\n\r"
    "  This procedure loads pointed memory with identifier of newly added device.\n\r"
    "  This identifier is to be used by the caller for further accesses to this\n\r"
    "  device\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt register_device base_address 0 reset_device_ptr 0 unit_ptr 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_REGISTER_DEVICE_REGISTER_DEVICE_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_UNREGISTER_DEVICE_UNREGISTER_DEVICE_ID, 
    "unregister_device", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Undo pcp_register_device\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt unregister_device\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_UNREGISTER_DEVICE_UNREGISTER_DEVICE_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_ID, 
    "op_mode_set", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Set PCP device operation mode. \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt op_mode_set conf_word 0 enable 0 enable 0 cal_m 0 cal_len 0 drop\n\r"
    "  0 fc 0 buff_size 0 head_ptr_index 0 head_ptr 0 enable 0 drop 0 fc 0 head_ptr\n\r"
    "  0 enable 0 enable 0 mode 0 enable 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_STS_CONF_WORD_ID, 
    "conf_word", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.sts.conf_word:\n\r"
    "  Power up configuration word passed to the Soc_petra on the statistics interface\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt op_mode_set conf_word 0 enable 0 enable 0 cal_m 0 cal_len 0 drop\n\r"
    "  0 fc 0 buff_size 0 head_ptr_index 0 head_ptr 0 enable 0 drop 0 fc 0 head_ptr\n\r"
    "  0 enable 0 enable 0 mode 0 enable 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_STS_ENABLE_ID, 
    "enable", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.sts.enable:\n\r"
    "  Enable statistics configuration\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt op_mode_set conf_word 0 enable 0 enable 0 cal_m 0 cal_len 0 drop\n\r"
    "  0 fc 0 buff_size 0 head_ptr_index 0 head_ptr 0 enable 0 drop 0 fc 0 head_ptr\n\r"
    "  0 enable 0 enable 0 mode 0 enable 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_OAM_ENABLE_ID, 
    "enable", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.oam.enable:\n\r"
    "  Enable OAM configuration\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt op_mode_set conf_word 0 enable 0 enable 0 cal_m 0 cal_len 0 drop\n\r"
    "  0 fc 0 buff_size 0 head_ptr_index 0 head_ptr 0 enable 0 drop 0 fc 0 head_ptr\n\r"
    "  0 enable 0 enable 0 mode 0 enable 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_RX_MODE_CAL_M_ID, 
    "cal_m", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.dma.rx_mode.cal_m:\n\r"
    "  Determines the calendar multiplier of the out of band interface. Multiplying\n\r"
    "  this value by cal_len should be equal to multiplication of the cal_m and\n\r"
    "  cal_len configured in the Soc_petra\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt op_mode_set conf_word 0 enable 0 enable 0 cal_m 0 cal_len 0 drop\n\r"
    "  0 fc 0 buff_size 0 head_ptr_index 0 head_ptr 0 enable 0 drop 0 fc 0 head_ptr\n\r"
    "  0 enable 0 enable 0 mode 0 enable 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_RX_MODE_CAL_LEN_ID, 
    "cal_len", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.dma.rx_mode.cal_len:\n\r"
    "  Determines the calendar length of the out of band interface. Multiplying this\n\r"
    "  value by cal_m should be equal to multiplication of the cal_m and cal_len\n\r"
    "  configured in the Soc_petra\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt op_mode_set conf_word 0 enable 0 enable 0 cal_m 0 cal_len 0 drop\n\r"
    "  0 fc 0 buff_size 0 head_ptr_index 0 head_ptr 0 enable 0 drop 0 fc 0 head_ptr\n\r"
    "  0 enable 0 enable 0 mode 0 enable 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_RX_MODE_THRESHOLDS_DROP_ID, 
    "drop", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.dma.rx_mode.thresholds.drop:\n\r"
    "  Drop threshold. When FIFO exceeds this threshold drop indication is sent\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt op_mode_set conf_word 0 enable 0 enable 0 cal_m 0 cal_len 0 drop\n\r"
    "  0 fc 0 buff_size 0 head_ptr_index 0 head_ptr 0 enable 0 drop 0 fc 0 head_ptr\n\r"
    "  0 enable 0 enable 0 mode 0 enable 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_RX_MODE_THRESHOLDS_FC_ID, 
    "fc", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.dma.rx_mode.thresholds.fc:\n\r"
    "  Flow control threshold. When FIFO exceeds this threshold flow control\n\r"
    "  indication is sent\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt op_mode_set conf_word 0 enable 0 enable 0 cal_m 0 cal_len 0 drop\n\r"
    "  0 fc 0 buff_size 0 head_ptr_index 0 head_ptr 0 enable 0 drop 0 fc 0 head_ptr\n\r"
    "  0 enable 0 enable 0 mode 0 enable 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_RX_MODE_BUFF_SIZE_ID, 
    "buff_size", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.dma.rx_mode.buff_size:\n\r"
    "  Buffer size configuration. The buffer size assigned in the CPU memory for\n\r"
    "  packet payload.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt op_mode_set conf_word 0 enable 0 enable 0 cal_m 0 cal_len 0 drop\n\r"
    "  0 fc 0 buff_size 0 head_ptr_index 0 head_ptr 0 enable 0 drop 0 fc 0 head_ptr\n\r"
    "  0 enable 0 enable 0 mode 0 enable 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_RX_MODE_HEAD_PTR_INDEX_ID, 
    "head_ptr_index", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  head_ptr_index:\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt op_mode_set conf_word 0 enable 0 enable 0 cal_m 0 cal_len 0 drop\n\r"
    "  0 fc 0 buff_size 0 head_ptr_index 0 head_ptr 0 enable 0 drop 0 fc 0 head_ptr\n\r"
    "  0 enable 0 enable 0 mode 0 enable 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_RX_MODE_HEAD_PTR_ID, 
    "head_ptr", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.dma.rx_mode.head_ptr[ prm_head_ptr_index]:\n\r"
    "  Pointer to the head of the descriptors RX chain. Must be a physical address\n\r"
    "  (not virtual). The user must make sure that the pointed memory is\n\r"
    "  synchronized with CPU cache. One chain for each traffic class\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt op_mode_set conf_word 0 enable 0 enable 0 cal_m 0 cal_len 0 drop\n\r"
    "  0 fc 0 buff_size 0 head_ptr_index 0 head_ptr 0 enable 0 drop 0 fc 0 head_ptr\n\r"
    "  0 enable 0 enable 0 mode 0 enable 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_RX_MODE_ENABLE_ID, 
    "enable", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.dma.rx_mode.enable:\n\r"
    "  Enable DMA configuration\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt op_mode_set conf_word 0 enable 0 enable 0 cal_m 0 cal_len 0 drop\n\r"
    "  0 fc 0 buff_size 0 head_ptr_index 0 head_ptr 0 enable 0 drop 0 fc 0 head_ptr\n\r"
    "  0 enable 0 enable 0 mode 0 enable 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_TX_THRESHOLDS_DROP_ID, 
    "drop", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.dma.TX.thresholds.drop:\n\r"
    "  Drop threshold. When FIFO exceeds this threshold drop indication is sent\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt op_mode_set conf_word 0 enable 0 enable 0 cal_m 0 cal_len 0 drop\n\r"
    "  0 fc 0 buff_size 0 head_ptr_index 0 head_ptr 0 enable 0 drop 0 fc 0 head_ptr\n\r"
    "  0 enable 0 enable 0 mode 0 enable 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_TX_THRESHOLDS_FC_ID, 
    "fc", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.dma.TX.thresholds.fc:\n\r"
    "  Flow control threshold. When FIFO exceeds this threshold flow control\n\r"
    "  indication is sent\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt op_mode_set conf_word 0 enable 0 enable 0 cal_m 0 cal_len 0 drop\n\r"
    "  0 fc 0 buff_size 0 head_ptr_index 0 head_ptr 0 enable 0 drop 0 fc 0 head_ptr\n\r"
    "  0 enable 0 enable 0 mode 0 enable 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_TX_HEAD_PTR_ID, 
    "head_ptr", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.dma.TX.head_ptr:\n\r"
    "  Pointer to the head of the descriptors TX chain. Must be a physical address\n\r"
    "  (not virtual). The user must make sure that the pointed memory is\n\r"
    "  synchronized with CPU cache.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt op_mode_set conf_word 0 enable 0 enable 0 cal_m 0 cal_len 0 drop\n\r"
    "  0 fc 0 buff_size 0 head_ptr_index 0 head_ptr 0 enable 0 drop 0 fc 0 head_ptr\n\r"
    "  0 enable 0 enable 0 mode 0 enable 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_TX_ENABLE_ID, 
    "enable", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.dma.TX.enable:\n\r"
    "  Enable DMA configuration\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt op_mode_set conf_word 0 enable 0 enable 0 cal_m 0 cal_len 0 drop\n\r"
    "  0 fc 0 buff_size 0 head_ptr_index 0 head_ptr 0 enable 0 drop 0 fc 0 head_ptr\n\r"
    "  0 enable 0 enable 0 mode 0 enable 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_ENABLE_ID, 
    "enable", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.dma.enable:\n\r"
    "  Enable DMA configurationtx_mode\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt op_mode_set conf_word 0 enable 0 enable 0 cal_m 0 cal_len 0 drop\n\r"
    "  0 fc 0 buff_size 0 head_ptr_index 0 head_ptr 0 enable 0 drop 0 fc 0 head_ptr\n\r"
    "  0 enable 0 enable 0 mode 0 enable 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_ELK_MODE_ID, 
    "mode", 
    (PARAM_VAL_RULES *)&PCP_MGMT_ELK_LKP_MODE_rule[0], 
    (sizeof(PCP_MGMT_ELK_LKP_MODE_rule) / sizeof(PCP_MGMT_ELK_LKP_MODE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.elk.mode:\n\r"
    "  ELK mode\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt op_mode_set conf_word 0 enable 0 enable 0 cal_m 0 cal_len 0 drop\n\r"
    "  0 fc 0 buff_size 0 head_ptr_index 0 head_ptr 0 enable 0 drop 0 fc 0 head_ptr\n\r"
    "  0 enable 0 enable 0 mode 0 enable 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_ELK_ENABLE_ID, 
    "enable", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.elk.enable:\n\r"
    "  Enable ELK configuration\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt op_mode_set conf_word 0 enable 0 enable 0 cal_m 0 cal_len 0 drop\n\r"
    "  0 fc 0 buff_size 0 head_ptr_index 0 head_ptr 0 enable 0 drop 0 fc 0 head_ptr\n\r"
    "  0 enable 0 enable 0 mode 0 enable 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_OP_MODE_GET_OP_MODE_GET_ID, 
    "op_mode_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Set PCP device operation mode. \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt op_mode_get\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_OP_MODE_GET_OP_MODE_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_ID, 
    "init_sequence_phase1", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Initialize the device, including:1.	Configuration of Soc_petra power up\n\r"
    "  configuration word2.	Initialize the device tables and registers to default\n\r"
    "  values 3.	Initialize board-specific hardware interfaces according to\n\r"
    "  configurable information, as passed in 'hw_adjust' 4.	Perform basic device\n\r"
    "  initialization 5.	DMA initialization\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt init_sequence_phase1 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\n\r"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_flow_ctrl 0 enable 0\n\r"
    "  qdr_size_mbit 0 size_mbit 0 burst_size 0 enable 0 silent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_ELK_IF_SERDES_INFO_TX_VODCTRL_ID, 
    "tx_vodctrl", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  hw_adjust.elk_if.serdes_info.tx_vodctrl:\n\r"
    "  Voltage Output Differential Control. Range 0 - 7\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt init_sequence_phase1 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\n\r"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_flow_ctrl 0 enable 0\n\r"
    "  qdr_size_mbit 0 size_mbit 0 burst_size 0 enable 0 silent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_ELK_IF_SERDES_INFO_TX_PREEMP_2T_ID, 
    "tx_preemp_2t", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  hw_adjust.elk_if.serdes_info.tx_preemp_2t:\n\r"
    "  2nd Post-tap Pre-emphasis Control. Range 0 - 15\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt init_sequence_phase1 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\n\r"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_flow_ctrl 0 enable 0\n\r"
    "  qdr_size_mbit 0 size_mbit 0 burst_size 0 enable 0 silent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_ELK_IF_SERDES_INFO_TX_PREEMP_1T_ID, 
    "tx_preemp_1t", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  hw_adjust.elk_if.serdes_info.tx_preemp_1t:\n\r"
    "  1st Post-tap Pre-emphasis Control. Range 0 - 15\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt init_sequence_phase1 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\n\r"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_flow_ctrl 0 enable 0\n\r"
    "  qdr_size_mbit 0 size_mbit 0 burst_size 0 enable 0 silent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_ELK_IF_SERDES_INFO_TX_PREEMP_0T_ID, 
    "tx_preemp_0t", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  hw_adjust.elk_if.serdes_info.tx_preemp_0t:\n\r"
    "  Pre-tap Pre-emphasis Control. Range 0 - 15\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt init_sequence_phase1 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\n\r"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_flow_ctrl 0 enable 0\n\r"
    "  qdr_size_mbit 0 size_mbit 0 burst_size 0 enable 0 silent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_ELK_IF_SERDES_INFO_RX_EQ_DC_GAIN_ID, 
    "rx_eq_dc_gain", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  hw_adjust.elk_if.serdes_info.rx_eq_dc_gain:\n\r"
    "  Equalizer DC Gain Control. Range 0 - 3\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt init_sequence_phase1 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\n\r"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_flow_ctrl 0 enable 0\n\r"
    "  qdr_size_mbit 0 size_mbit 0 burst_size 0 enable 0 silent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_ELK_IF_SERDES_INFO_RX_EQ_CTRL_ID, 
    "rx_eq_ctrl", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  hw_adjust.elk_if.serdes_info.rx_eq_ctrl:\n\r"
    "  Equalization Control. Range 0 - 15\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt init_sequence_phase1 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\n\r"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_flow_ctrl 0 enable 0\n\r"
    "  qdr_size_mbit 0 size_mbit 0 burst_size 0 enable 0 silent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_ELK_IF_ENABLE_FLOW_CTRL_ID, 
    "enable_flow_ctrl", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  hw_adjust.elk_if.enable_flow_ctrl:\n\r"
    "  Enable/disable Flow Control\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt init_sequence_phase1 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\n\r"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_flow_ctrl 0 enable 0\n\r"
    "  qdr_size_mbit 0 size_mbit 0 burst_size 0 enable 0 silent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_ELK_IF_ENABLE_ID, 
    "enable", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  hw_adjust.elk_if.enable:\n\r"
    "  Enable/disable the interface \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt init_sequence_phase1 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\n\r"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_flow_ctrl 0 enable 0\n\r"
    "  qdr_size_mbit 0 size_mbit 0 burst_size 0 enable 0 silent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_QDR_QDR_SIZE_MBIT_ID, 
    "qdr_size_mbit", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  hw_adjust.qdr.qdr_size_mbit:\n\r"
    "  QDR memory size. Units: Mbits\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt init_sequence_phase1 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\n\r"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_flow_ctrl 0 enable 0\n\r"
    "  qdr_size_mbit 0 size_mbit 0 burst_size 0 enable 0 silent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_DRAM_SIZE_MBIT_ID, 
    "size_mbit", 
    (PARAM_VAL_RULES *)&PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_rule[0], 
    (sizeof(PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_rule) / sizeof(PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  hw_adjust.dram.size_mbit:\n\r"
    "  RLDRAM memory size. Units: Mbits\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt init_sequence_phase1 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\n\r"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_flow_ctrl 0 enable 0\n\r"
    "  qdr_size_mbit 0 size_mbit 0 burst_size 0 enable 0 silent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_PCI_BURST_SIZE_ID, 
    "burst_size", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  hw_adjust.pci.burst_size:\n\r"
    "  PCI Express burst size configuration. Maximal burst size that may be read\n\r"
    "  from the CPU in a single read command.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt init_sequence_phase1 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\n\r"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_flow_ctrl 0 enable 0\n\r"
    "  qdr_size_mbit 0 size_mbit 0 burst_size 0 enable 0 silent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_PCI_ENABLE_ID, 
    "enable", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  hw_adjust.pci.enable:\n\r"
    "  Enable/disable the interface \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt init_sequence_phase1 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\n\r"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_flow_ctrl 0 enable 0\n\r"
    "  qdr_size_mbit 0 size_mbit 0 burst_size 0 enable 0 silent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_SILENT_ID, 
    "silent", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  silent:\n\r"
    "  If TRUE, progress printing will be suppressed\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt init_sequence_phase1 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\n\r"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_flow_ctrl 0 enable 0\n\r"
    "  qdr_size_mbit 0 size_mbit 0 burst_size 0 enable 0 silent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE2_INIT_SEQUENCE_PHASE2_ID, 
    "init_sequence_phase2", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This API enables all interfaces, as set in the operation mode. \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt init_sequence_phase2 silent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_INIT_SEQUENCE_PHASE2_INIT_SEQUENCE_PHASE2_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE2_INIT_SEQUENCE_PHASE2_SILENT_ID, 
    "silent", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  silent:\n\r"
    "  If TRUE, progress printing will be suppressed\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mgmt init_sequence_phase2 silent 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_MGMT_INIT_SEQUENCE_PHASE2_INIT_SEQUENCE_PHASE2_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_API_MGMT_ID, 
    "mgmt", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    BIT_UI_16_RNG(FUN_PCP_API_MGMT_FIRST_ID,FUN_PCP_API_MGMT_LAST_ID), 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
#endif /* } mgmt*/
#ifdef UI_INTERRUPTS/* { interrupts*/
#endif /* } interrupts*/
#ifdef UI_CALLBACK/* { callback*/
#endif /* } callback*/
#ifdef UI_STATISTICS/* { statistics*/
  { 
    PARAM_PCP_STS_ING_COUNTER_GET_STS_ING_COUNTER_GET_ID, 
    "sts_ing_counter_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This API retrieves statistics on the specified ingress counter index\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api statistics sts_ing_counter_get counter_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_STS_ING_COUNTER_GET_STS_ING_COUNTER_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_STS_ING_COUNTER_GET_STS_ING_COUNTER_GET_COUNTER_NDX_ID, 
    "counter_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  counter_ndx:\n\r"
    "  Selects the counter on which statistics should be retrieved	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api statistics sts_ing_counter_get counter_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_STS_ING_COUNTER_GET_STS_ING_COUNTER_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_STS_EGR_COUNTER_GET_STS_EGR_COUNTER_GET_ID, 
    "sts_egr_counter_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This API retrieves statistics on the specified egress counter index\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api statistics sts_egr_counter_get counter_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_STS_EGR_COUNTER_GET_STS_EGR_COUNTER_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_STS_EGR_COUNTER_GET_STS_EGR_COUNTER_GET_COUNTER_NDX_ID, 
    "counter_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  counter_ndx:\n\r"
    "  Selects the counter on which statistics should be retrieved	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api statistics sts_egr_counter_get counter_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_STS_EGR_COUNTER_GET_STS_EGR_COUNTER_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_API_STATISTICS_ID, 
    "statistics", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    BIT_UI_16_RNG(FUN_PCP_API_STATISTICS_FIRST_ID,FUN_PCP_API_STATISTICS_LAST_ID), 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
#endif /* } statistics*/
#ifdef UI_DIAGNOSTICS/* { diagnostics*/
  { 
    PARAM_PCP_DIAG_STS_SYNC_STATUS_GET_DIAG_STS_SYNC_STATUS_GET_ID, 
    "diag_sts_sync_status_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This API returns the sync status of the statistics interface\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api diagnostics diag_sts_sync_status_get\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_DIAG_STS_SYNC_STATUS_GET_DIAG_STS_SYNC_STATUS_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_DIAG_DMA_RX_STATUS_GET_DIAG_DMA_RX_STATUS_GET_ID, 
    "diag_dma_rx_status_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This API returns if there are any packets waiting to be handled by the CPU on\n\r"
    "  the class specified\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api diagnostics diag_dma_rx_status_get traffic_class 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_DIAG_DMA_RX_STATUS_GET_DIAG_DMA_RX_STATUS_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_DIAG_DMA_RX_STATUS_GET_DIAG_DMA_RX_STATUS_GET_TRAFFIC_CLASS_ID, 
    "traffic_class", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  traffic_class:\n\r"
    "  Select the traffic class that should be handled	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api diagnostics diag_dma_rx_status_get traffic_class 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_DIAG_DMA_RX_STATUS_GET_DIAG_DMA_RX_STATUS_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_API_DIAGNOSTICS_ID, 
    "diagnostics", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    BIT_UI_16_RNG(FUN_PCP_API_DIAGNOSTICS_FIRST_ID,FUN_PCP_API_DIAGNOSTICS_LAST_ID), 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
#endif /* } diagnostics*/
#ifdef UI_GENERAL/* { general*/
  { 
    PARAM_PCP_OAM_GENERAL_INFO_SET_INFO_SET_ID, 
    "info_set", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This procedure applies general configuration of the OAMP. \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api general info_set cpu_dp 0 cpu_tc 0 cpu_dst_sys_port 0 source_sys_port\n\r"
    "  0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_GENERAL_INFO_SET_INFO_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_GENERAL_INFO_SET_INFO_SET_INFO_CPU_DP_ID, 
    "cpu_dp", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.cpu_dp:\n\r"
    "  Drop precedence. Used to construct ITMH when trapping packets to CPU from the\n\r"
    "  OAMP.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api general info_set cpu_dp 0 cpu_tc 0 cpu_dst_sys_port 0 source_sys_port\n\r"
    "  0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_GENERAL_INFO_SET_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_GENERAL_INFO_SET_INFO_SET_INFO_CPU_TC_ID, 
    "cpu_tc", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.cpu_tc:\n\r"
    "  Traffic class. Used to construct ITMH when trapping packets to CPU from the\n\r"
    "  OAMP.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api general info_set cpu_dp 0 cpu_tc 0 cpu_dst_sys_port 0 source_sys_port\n\r"
    "  0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_GENERAL_INFO_SET_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_GENERAL_INFO_SET_INFO_SET_INFO_CPU_DST_SYS_PORT_ID, 
    "cpu_dst_sys_port", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.cpu_dst_sys_port:\n\r"
    "  CPU port. Used to construct ITMH when trapping packets to CPU from the OAMP.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api general info_set cpu_dp 0 cpu_tc 0 cpu_dst_sys_port 0 source_sys_port\n\r"
    "  0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_GENERAL_INFO_SET_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_GENERAL_INFO_GET_INFO_GET_ID, 
    "info_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This procedure applies general configuration of the OAMP. \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api general info_get\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_GENERAL_INFO_GET_INFO_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_CALLBACK_FUNCTION_REGISTER_OAM_CALLBACK_FUNCTION_REGISTER_ID, 
    "oam_callback_function_register", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This procedure registers user callback that will be called as a response to a\n\r"
    "  certain interrupt.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api general oam_callback_function_register user_callback_struct 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_CALLBACK_FUNCTION_REGISTER_OAM_CALLBACK_FUNCTION_REGISTER_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_CALLBACK_FUNCTION_REGISTER_OAM_CALLBACK_FUNCTION_REGISTER_USER_CALLBACK_STRUCT_ID, 
    "user_callback_struct", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  user_callback_struct:\n\r"
    "  Structure holding information about the required interrupt callback.	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api general oam_callback_function_register user_callback_struct 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_CALLBACK_FUNCTION_REGISTER_OAM_CALLBACK_FUNCTION_REGISTER_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_INTERRUPT_HANDLER_OAM_INTERRUPT_HANDLER_ID, 
    "oam_interrupt_handler", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This procedure is called from the CPU ISR when interrupt handling for OAMP is\n\r"
    "  required.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api general oam_interrupt_handler\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_INTERRUPT_HANDLER_OAM_INTERRUPT_HANDLER_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MSG_INFO_GET_OAM_MSG_INFO_GET_ID, 
    "oam_msg_info_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function reads and parses the head of the OAM message FIFO. If no\n\r"
    "  message exists an indication is returned.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api general oam_msg_info_get\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MSG_INFO_GET_OAM_MSG_INFO_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_API_GENERAL_ID, 
    "general", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    BIT_UI_16_RNG(FUN_PCP_OAM_API_GENERAL_FIRST_ID,FUN_PCP_OAM_API_GENERAL_LAST_ID), 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
#endif /* } general*/
#ifdef UI_ETH/* { eth*/
  { 
    PARAM_PCP_OAM_ETH_GENERAL_INFO_SET_GENERAL_INFO_SET_ID, 
    "general_info_set", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function sets general configuration of Ethernet OAM.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth general_info_set custom_header_size 0 mep_sa_msb 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_GENERAL_INFO_SET_GENERAL_INFO_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_GENERAL_INFO_SET_GENERAL_INFO_SET_INFO_CUSTOM_HEADER_SIZE_ID, 
    "custom_header_size", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.custom_header_size:\n\r"
    "  Determines the size (in bytes) of the user custom header. Applicable for all\n\r"
    "  accelerated MEPs (See Procedure pcp_oam_eth_acc_mep_custom_header_set ()).\n\r"
    "  Set this value to zero to disable custom header mechanism. Range: 0-4\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth general_info_set custom_header_size 0 mep_sa_msb 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_GENERAL_INFO_SET_GENERAL_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_GENERAL_INFO_SET_GENERAL_INFO_SET_INFO_MEP_SA_MSB_ID, 
    "mep_sa_msb", 
    (PARAM_VAL_RULES *)&pcp_api_mac_addr_vals[0], 
    (sizeof(pcp_api_mac_addr_vals) / sizeof(pcp_api_mac_addr_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.mep_sa_msb:\n\r"
    "  41 upper bits of the source MAC address of the CFM packets (down/up MEP) \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth general_info_set custom_header_size 0 mep_sa_msb 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_GENERAL_INFO_SET_GENERAL_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_GENERAL_INFO_GET_GENERAL_INFO_GET_ID, 
    "general_info_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function sets general configuration of Ethernet OAM.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth general_info_get\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_GENERAL_INFO_GET_GENERAL_INFO_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_ID, 
    "cos_mapping_info_set", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function maps the cos profile and the user priority (both are MEP\n\r"
    "  attributes) to TC and DP for the ITMH (relevant for down MEP injection)\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth cos_mapping_info_set cos_ndx 0 prio_ndx 0 dp 0 tc 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_COS_NDX_ID, 
    "cos_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  cos_ndx:\n\r"
    "  Class of service. Range: 0-3	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth cos_mapping_info_set cos_ndx 0 prio_ndx 0 dp 0 tc 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_ID), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_PRIO_NDX_ID, 
    "prio_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  prio_ndx:\n\r"
    "  VLAN tag's user priority. Range: 0 - 7	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth cos_mapping_info_set cos_ndx 0 prio_ndx 0 dp 0 tc 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_ID), 
    4, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_DP_ID, 
    "dp", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.dp:\n\r"
    "  ITMH drop precedence. Relevant only for down MEPs. Range: 0-3\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth cos_mapping_info_set cos_ndx 0 prio_ndx 0 dp 0 tc 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_TC_ID, 
    "tc", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.tc:\n\r"
    "  ITMH Traffic class. Relevant only for down MEPs. Range: 0-7\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth cos_mapping_info_set cos_ndx 0 prio_ndx 0 dp 0 tc 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_ID, 
    "cos_mapping_info_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function maps the cos profile and the user priority (both are MEP\n\r"
    "  attributes) to TC and DP for the ITMH (relevant for down MEP injection)\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth cos_mapping_info_get cos_ndx 0 prio_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_COS_NDX_ID, 
    "cos_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  cos_ndx:\n\r"
    "  Class of service. Range: 0-3	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth cos_mapping_info_get cos_ndx 0 prio_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_PRIO_NDX_ID, 
    "prio_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  prio_ndx:\n\r"
    "  VLAN tag's user priority. Range: 0 - 7	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth cos_mapping_info_get cos_ndx 0 prio_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MP_INFO_SET_OAM_MP_INFO_SET_ID, 
    "oam_mp_info_set", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function is used to define a Maintenance Point (MP) on an\n\r"
    "  Attachment-Circuit (incoming/outgoing), MD-level, and MP direction (up/down).\n\r"
    "  If the MP is one of the 4K accelerated MEPs, the function configures the\n\r"
    "  related OAMP databases and associates the AC, MD-Level and the MP type with a\n\r"
    "  user-provided handle. This handle is later used by user to access OAMP\n\r"
    "  database for this MEP. \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth oam_mp_info_set direction 0 level 0 acc_mep_id 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MP_INFO_SET_OAM_MP_INFO_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MP_INFO_SET_OAM_MP_INFO_SET_MP_KEY_DIRECTION_ID, 
    "direction", 
    (PARAM_VAL_RULES *)&PCP_OAM_ETH_MEP_DIRECTION_rule[0], 
    (sizeof(PCP_OAM_ETH_MEP_DIRECTION_rule) / sizeof(PCP_OAM_ETH_MEP_DIRECTION_rule[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  mp_key.direction:\n\r"
    "  MP direction\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth oam_mp_info_set direction 0 level 0 acc_mep_id 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MP_INFO_SET_OAM_MP_INFO_SET_ID), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MP_INFO_SET_OAM_MP_INFO_SET_MP_KEY_LEVEL_ID, 
    "level", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  mp_key.level:\n\r"
    "  CFM level. Range: 0 - 7\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth oam_mp_info_set direction 0 level 0 acc_mep_id 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MP_INFO_SET_OAM_MP_INFO_SET_ID), 
    4, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MP_INFO_SET_OAM_MP_INFO_SET_MP_KEY_AC_NDX_ID, 
    "ac_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  mp_key.ac_ndx:\n\r"
    "  AC index. Range: 0 - 16k\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth oam_mp_info_set direction 0 level 0 ac_ndx 0 acc_mep_id 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MP_INFO_SET_OAM_MP_INFO_SET_ID), 
    5, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MP_INFO_SET_OAM_MP_INFO_SET_INFO_ACC_MEP_ID_ID, 
    "acc_mep_id", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.acc_mep_id:\n\r"
    "  The accelerated MEP handle to associate with the specified attachment circuit\n\r"
    "  and level. Relevant only to APIs of accelerated MEPs. (E.g. CCM). Range: 0-4K\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth oam_mp_info_set direction 0 level 0 acc_mep_id 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MP_INFO_SET_OAM_MP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MP_INFO_SET_OAM_MP_INFO_SET_INFO_VALID_ID, 
    "valid", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.valid:\n\r"
    "  T - Enable MP for this ACF - Disable MP for this AC\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth oam_mp_info_set direction 0 level 0 acc_mep_id 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MP_INFO_SET_OAM_MP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MP_INFO_GET_OAM_MP_INFO_GET_ID, 
    "oam_mp_info_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function is used to define a Maintenance Point (MP) on an\n\r"
    "  Attachment-Circuit (incoming/outgoing), MD-level, and MP direction (up/down).\n\r"
    "  If the MP is one of the 4K accelerated MEPs, the function configures the\n\r"
    "  related OAMP databases and associates the AC, MD-Level and the MP type with a\n\r"
    "  user-provided handle. This handle is later used by user to access OAMP\n\r"
    "  database for this MEP. \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth oam_mp_info_get direction 0 level 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MP_INFO_GET_OAM_MP_INFO_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MP_INFO_GET_OAM_MP_INFO_GET_MP_KEY_DIRECTION_ID, 
    "direction", 
    (PARAM_VAL_RULES *)&PCP_OAM_ETH_MEP_DIRECTION_rule[0], 
    (sizeof(PCP_OAM_ETH_MEP_DIRECTION_rule) / sizeof(PCP_OAM_ETH_MEP_DIRECTION_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  mp_key.direction:\n\r"
    "  MP direction\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth oam_mp_info_get direction 0 level 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MP_INFO_GET_OAM_MP_INFO_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MP_INFO_GET_OAM_MP_INFO_GET_MP_KEY_LEVEL_ID, 
    "level", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  mp_key.level:\n\r"
    "  CFM level. Range: 0 - 7\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth oam_mp_info_get direction 0 level 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MP_INFO_GET_OAM_MP_INFO_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MP_INFO_GET_OAM_MP_INFO_GET_MP_KEY_AC_NDX_ID, 
    "ac_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  mp_key.ac_ndx:\n\r"
    "  AC index. Range: 0 - 16k\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth oam_mp_info_get direction 0 level 0 ac_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MP_INFO_GET_OAM_MP_INFO_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID, 
    "acc_mep_info_set", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function configures an accelerated MEP.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_info_set acc_mep_ndx 0 dlm_index 0 dlm_enable 0 is_dual 0\n\r"
    "  lmm_index 0 lmm_enable 0 ccm_priority 0 ccm_interval 0 cos_profile 0 ac 0\n\r"
    "  destination 0 dn_vid 0 dn_sa_lsb 0 up_vid 0 up_sa_lsb 0 direction 0 md_level\n\r"
    "  0 mep_id 0 ma_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ACC_MEP_NDX_ID, 
    "acc_mep_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  acc_mep_ndx:\n\r"
    "  The user-defined handle, selected when the accelerated MEP was defined (see\n\r"
    "  pcp_oam_eth_mp_info_set()). Range: 0-4K	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_info_set acc_mep_ndx 0 dlm_index 0 dlm_enable 0 is_dual 0\n\r"
    "  lmm_index 0 lmm_enable 0 ccm_priority 0 ccm_interval 0 cos_profile 0 ac 0\n\r"
    "  destination 0 dn_vid 0 dn_sa_lsb 0 up_vid 0 up_sa_lsb 0 direction 0 md_level\n\r"
    "  0 mep_id 0 ma_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_PEFORMANCE_MONITORING_INFO_DLM_INFO_DLM_INDEX_ID, 
    "dlm_index", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.peformance_monitoring_info.dlm_info.dlm_index:\n\r"
    "  Accelerated DLM index. Range: 0 - 127\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_info_set acc_mep_ndx 0 dlm_index 0 dlm_enable 0 is_dual 0\n\r"
    "  lmm_index 0 lmm_enable 0 ccm_priority 0 ccm_interval 0 cos_profile 0 ac 0\n\r"
    "  destination 0 dn_vid 0 dn_sa_lsb 0 up_vid 0 up_sa_lsb 0 direction 0 md_level\n\r"
    "  0 mep_id 0 ma_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_PEFORMANCE_MONITORING_INFO_DLM_INFO_DLM_ENABLE_ID, 
    "dlm_enable", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.peformance_monitoring_info.dlm_info.dlm_enable:\n\r"
    "  T - enableF - disable\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_info_set acc_mep_ndx 0 dlm_index 0 dlm_enable 0 is_dual 0\n\r"
    "  lmm_index 0 lmm_enable 0 ccm_priority 0 ccm_interval 0 cos_profile 0 ac 0\n\r"
    "  destination 0 dn_vid 0 dn_sa_lsb 0 up_vid 0 up_sa_lsb 0 direction 0 md_level\n\r"
    "  0 mep_id 0 ma_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_PEFORMANCE_MONITORING_INFO_LMM_INFO_IS_DUAL_ID, 
    "is_dual", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.peformance_monitoring_info.lmm_info.is_dual:\n\r"
    "  Dual-ended. T - The configured MEP generates LM packets over CCM (according\n\r"
    "  to ITU Y.1731). \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_info_set acc_mep_ndx 0 dlm_index 0 dlm_enable 0 is_dual 0\n\r"
    "  lmm_index 0 lmm_enable 0 ccm_priority 0 ccm_interval 0 cos_profile 0 ac 0\n\r"
    "  destination 0 dn_vid 0 dn_sa_lsb 0 up_vid 0 up_sa_lsb 0 direction 0 md_level\n\r"
    "  0 mep_id 0 ma_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_PEFORMANCE_MONITORING_INFO_LMM_INFO_LMM_INDEX_ID, 
    "lmm_index", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.peformance_monitoring_info.lmm_info.lmm_index:\n\r"
    "  Accelerated LMM index. Range: 0 - 127\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_info_set acc_mep_ndx 0 dlm_index 0 dlm_enable 0 is_dual 0\n\r"
    "  lmm_index 0 lmm_enable 0 ccm_priority 0 ccm_interval 0 cos_profile 0 ac 0\n\r"
    "  destination 0 dn_vid 0 dn_sa_lsb 0 up_vid 0 up_sa_lsb 0 direction 0 md_level\n\r"
    "  0 mep_id 0 ma_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_PEFORMANCE_MONITORING_INFO_LMM_INFO_LMM_ENABLE_ID, 
    "lmm_enable", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.peformance_monitoring_info.lmm_info.lmm_enable:\n\r"
    "  T - enableF - disable\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_info_set acc_mep_ndx 0 dlm_index 0 dlm_enable 0 is_dual 0\n\r"
    "  lmm_index 0 lmm_enable 0 ccm_priority 0 ccm_interval 0 cos_profile 0 ac 0\n\r"
    "  destination 0 dn_vid 0 dn_sa_lsb 0 up_vid 0 up_sa_lsb 0 direction 0 md_level\n\r"
    "  0 mep_id 0 ma_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_PEFORMANCE_MONITORING_INFO_LMM_INFO_LMR_ENABLE_ID, 
    "lmr_enable", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.peformance_monitoring_info.lmm_info.lmr_enable:\n\r"
    "  T - far end identification, F - Disabled\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_info_set acc_mep_ndx 0 dlm_index 0 dlm_enable 0 is_dual 0\n\r"
    "  lmm_index 0 lmm_enable 0 ccm_priority 0 ccm_interval 0 cos_profile 0 ac 0\n\r"
    "  destination 0 dn_vid 0 dn_sa_lsb 0 up_vid 0 up_sa_lsb 0 direction 0 md_level\n\r"
    "  0 mep_id 0 ma_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_PEFORMANCE_MONITORING_INFO_CCM_INFO_CCM_PRIORITY_ID, 
    "ccm_priority", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.peformance_monitoring_info.ccm_info.ccm_priority:\n\r"
    "  Determines the priority of the generated CFM packet\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_info_set acc_mep_ndx 0 dlm_index 0 dlm_enable 0 is_dual 0\n\r"
    "  lmm_index 0 lmm_enable 0 ccm_priority 0 ccm_interval 0 cos_profile 0 ac 0\n\r"
    "  destination 0 dn_vid 0 dn_sa_lsb 0 up_vid 0 up_sa_lsb 0 direction 0 md_level\n\r"
    "  0 mep_id 0 ma_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_PEFORMANCE_MONITORING_INFO_CCM_INFO_CCM_INTERVAL_ID, 
    "ccm_interval", 
    (PARAM_VAL_RULES *)&PCP_OAM_ETH_INTERVAL_rule[0], 
    (sizeof(PCP_OAM_ETH_INTERVAL_rule) / sizeof(PCP_OAM_ETH_INTERVAL_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.peformance_monitoring_info.ccm_info.ccm_interval:\n\r"
    "  Interval between consecutive CCM messages\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_info_set acc_mep_ndx 0 dlm_index 0 dlm_enable 0 is_dual 0\n\r"
    "  lmm_index 0 lmm_enable 0 ccm_priority 0 ccm_interval 0 cos_profile 0 ac 0\n\r"
    "  destination 0 dn_vid 0 dn_sa_lsb 0 up_vid 0 up_sa_lsb 0 direction 0 md_level\n\r"
    "  0 mep_id 0 ma_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_MEP_COS_PROFILE_ID, 
    "cos_profile", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.frwd_info.down_mep.cos_profile:\n\r"
    "  Used to map traffic class and drop precedence. See Procedure\n\r"
    "  pcp_oam_eth_cos_mapping_info_set()\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_info_set acc_mep_ndx 0 dlm_index 0 dlm_enable 0 is_dual 0\n\r"
    "  lmm_index 0 lmm_enable 0 ccm_priority 0 ccm_interval 0 cos_profile 0 ac 0\n\r"
    "  destination 0 dn_vid 0 dn_sa_lsb 0 up_vid 0 up_sa_lsb 0 direction 0 md_level\n\r"
    "  0 mep_id 0 ma_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_MEP_AC_ID, 
    "ac", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.frwd_info.down_mep.ac:\n\r"
    "  This field determines the out_ac of generated CFM packet. If set to Type\n\r"
    "  Definition PCP_OAM_ETH_NO_OUT_AC no out-AC is generated.Range: 0 - 16k.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_info_set acc_mep_ndx 0 dlm_index 0 dlm_enable 0 is_dual 0\n\r"
    "  lmm_index 0 lmm_enable 0 ccm_priority 0 ccm_interval 0 cos_profile 0 ac 0\n\r"
    "  destination 0 dn_vid 0 dn_sa_lsb 0 up_vid 0 up_sa_lsb 0 direction 0 md_level\n\r"
    "  0 mep_id 0 ma_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_MEP_DEST_ID_ID, 
    "dest_id", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.frwd_info.down_mep.destination:\n\r"
    "  Used by the OAMP to determine the destination to stamp on the ITMH (unicast\n\r"
    "  flow/unicast direct). Refer to Soc_petra data sheet for more information.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_info_set acc_mep_ndx 0 dlm_index 0 dlm_enable 0 is_dual 0\n\r"
    "  lmm_index 0 lmm_enable 0 ccm_priority 0 ccm_interval 0 cos_profile 0 ac 0\n\r"
    "  destination 0 dn_vid 0 dn_sa_lsb 0 up_vid 0 up_sa_lsb 0 direction 0 md_level\n\r"
    "  0 mep_id 0 ma_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
   { 
    PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_MEP_DEST_TYPE_ID, 
    "dest_type", 
    (PARAM_VAL_RULES *)&PCP_DEST_TYPE_rule[0],
    (sizeof(PCP_DEST_TYPE_rule) / sizeof(PCP_DEST_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.frwd_info.down_mep.destination:\n\r"
    "  Used by the OAMP to determine the destination to stamp on the ITMH (unicast\n\r"
    "  flow/unicast direct). Refer to Soc_petra data sheet for more information.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_info_set acc_mep_ndx 0 dlm_index 0 dlm_enable 0 is_dual 0\n\r"
    "  lmm_index 0 lmm_enable 0 ccm_priority 0 ccm_interval 0 cos_profile 0 ac 0\n\r"
    "  destination 0 dn_vid 0 dn_sa_lsb 0 up_vid 0 up_sa_lsb 0 direction 0 md_level\n\r"
    "  0 mep_id 0 ma_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_MEP_DN_VID_ID, 
    "dn_vid", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.frwd_info.down_mep.dn_vid:\n\r"
    "  This field determines the vid of generated CFM packet. If set to Type\n\r"
    "  Definition PCP_OAM_ETH_NO_VID\n\r"
    "  no vid is generated.Range: 0 - 4k.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_info_set acc_mep_ndx 0 dlm_index 0 dlm_enable 0 is_dual 0\n\r"
    "  lmm_index 0 lmm_enable 0 ccm_priority 0 ccm_interval 0 cos_profile 0 ac 0\n\r"
    "  destination 0 dn_vid 0 dn_sa_lsb 0 up_vid 0 up_sa_lsb 0 direction 0 md_level\n\r"
    "  0 mep_id 0 ma_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_MEP_DN_SA_LSB_ID, 
    "dn_sa_lsb", 
    (PARAM_VAL_RULES *)&pcp_api_mac_addr_vals[0], 
    (sizeof(pcp_api_mac_addr_vals) / sizeof(pcp_api_mac_addr_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.frwd_info.down_mep.dn_sa_lsb:\n\r"
    "  7 lower bits of the source MAC address of the CFM packet. \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_info_set acc_mep_ndx 0 dlm_index 0 dlm_enable 0 is_dual 0\n\r"
    "  lmm_index 0 lmm_enable 0 ccm_priority 0 ccm_interval 0 cos_profile 0 ac 0\n\r"
    "  destination 0 dn_vid 0 dn_sa_lsb 0 up_vid 0 up_sa_lsb 0 direction 0 md_level\n\r"
    "  0 mep_id 0 ma_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_UP_MEP_UP_VID_ID, 
    "up_vid", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.frwd_info.up_mep.up_vid:\n\r"
    "  This field determines the vid of generated CFM packet. If set to zero no vid\n\r"
    "  is generated\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_info_set acc_mep_ndx 0 dlm_index 0 dlm_enable 0 is_dual 0\n\r"
    "  lmm_index 0 lmm_enable 0 ccm_priority 0 ccm_interval 0 cos_profile 0 ac 0\n\r"
    "  destination 0 dn_vid 0 dn_sa_lsb 0 up_vid 0 up_sa_lsb 0 direction 0 md_level\n\r"
    "  0 mep_id 0 ma_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_UP_MEP_UP_SA_LSB_ID, 
    "up_sa_lsb", 
    (PARAM_VAL_RULES *)&pcp_api_mac_addr_vals[0], 
    (sizeof(pcp_api_mac_addr_vals) / sizeof(pcp_api_mac_addr_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.frwd_info.up_mep.up_sa_lsb:\n\r"
    "  7 lower bits of the source MAC address of the CFM packet. \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_info_set acc_mep_ndx 0 dlm_index 0 dlm_enable 0 is_dual 0\n\r"
    "  lmm_index 0 lmm_enable 0 ccm_priority 0 ccm_interval 0 cos_profile 0 ac 0\n\r"
    "  destination 0 dn_vid 0 dn_sa_lsb 0 up_vid 0 up_sa_lsb 0 direction 0 md_level\n\r"
    "  0 mep_id 0 ma_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_DIRECTION_ID, 
    "direction", 
    (PARAM_VAL_RULES *)&PCP_OAM_ETH_MEP_DIRECTION_rule[0], 
    (sizeof(PCP_OAM_ETH_MEP_DIRECTION_rule) / sizeof(PCP_OAM_ETH_MEP_DIRECTION_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.direction:\n\r"
    "  MEP direction (up/down)\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_info_set acc_mep_ndx 0 dlm_index 0 dlm_enable 0 is_dual 0\n\r"
    "  lmm_index 0 lmm_enable 0 ccm_priority 0 ccm_interval 0 cos_profile 0 ac 0\n\r"
    "  destination 0 dn_vid 0 dn_sa_lsb 0 up_vid 0 up_sa_lsb 0 direction 0 md_level\n\r"
    "  0 mep_id 0 ma_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_MD_LEVEL_ID, 
    "md_level", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.md_level:\n\r"
    "  The maintenance domain level of the configured MEP. Range: 0-7\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_info_set acc_mep_ndx 0 dlm_index 0 dlm_enable 0 is_dual 0\n\r"
    "  lmm_index 0 lmm_enable 0 ccm_priority 0 ccm_interval 0 cos_profile 0 ac 0\n\r"
    "  destination 0 dn_vid 0 dn_sa_lsb 0 up_vid 0 up_sa_lsb 0 direction 0 md_level\n\r"
    "  0 mep_id 0 ma_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_MEP_ID_ID, 
    "mep_id", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.mep_id:\n\r"
    "  The identifier of the transmitting MEP. Stamped on the CFM packet. Unique for\n\r"
    "  all MEPs in the same maintenance association (MA). Range: 0-8K\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_info_set acc_mep_ndx 0 dlm_index 0 dlm_enable 0 is_dual 0\n\r"
    "  lmm_index 0 lmm_enable 0 ccm_priority 0 ccm_interval 0 cos_profile 0 ac 0\n\r"
    "  destination 0 dn_vid 0 dn_sa_lsb 0 up_vid 0 up_sa_lsb 0 direction 0 md_level\n\r"
    "  0 mep_id 0 ma_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_MA_ID_ID, 
    "ma_id", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.ma_id:\n\r"
    "  Maintenance Association is set of MEPs, each configured with the same ma_id\n\r"
    "  and maintenance domain Level, Range: 0-64K\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_info_set acc_mep_ndx 0 dlm_index 0 dlm_enable 0 is_dual 0\n\r"
    "  lmm_index 0 lmm_enable 0 ccm_priority 0 ccm_interval 0 cos_profile 0 ac 0\n\r"
    "  destination 0 dn_vid 0 dn_sa_lsb 0 up_vid 0 up_sa_lsb 0 direction 0 md_level\n\r"
    "  0 mep_id 0 ma_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_INFO_GET_ACC_MEP_INFO_GET_ID, 
    "acc_mep_info_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function configures an accelerated MEP.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_info_get acc_mep_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_INFO_GET_ACC_MEP_INFO_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_INFO_GET_ACC_MEP_INFO_GET_ACC_MEP_NDX_ID, 
    "acc_mep_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  acc_mep_ndx:\n\r"
    "  The user-defined handle, selected when the accelerated MEP was defined (see\n\r"
    "  pcp_oam_eth_mp_info_set()). Range: 0-4K	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_info_get acc_mep_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_INFO_GET_ACC_MEP_INFO_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_SET_ACC_MEP_CUSTOM_HEADER_SET_ID, 
    "acc_mep_custom_header_set", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function sets user custom header.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_custom_header_set acc_mep_ndx 0 header 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_SET_ACC_MEP_CUSTOM_HEADER_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_SET_ACC_MEP_CUSTOM_HEADER_SET_ACC_MEP_NDX_ID, 
    "acc_mep_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  acc_mep_ndx:\n\r"
    "  The user-defined handle, selected when the accelerated MEP was defined (see\n\r"
    "  pcp_oam_eth_mp_info_set()). Range: 0-4K	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_custom_header_set acc_mep_ndx 0 header 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_SET_ACC_MEP_CUSTOM_HEADER_SET_ID), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_SET_ACC_MEP_CUSTOM_HEADER_SET_HEADER_ID, 
    "header", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  header:\n\r"
    "  Placed at the beginning of the packet\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_custom_header_set acc_mep_ndx 0 header 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_SET_ACC_MEP_CUSTOM_HEADER_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_GET_ACC_MEP_CUSTOM_HEADER_GET_ID, 
    "acc_mep_custom_header_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function sets user custom header.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_custom_header_get acc_mep_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_GET_ACC_MEP_CUSTOM_HEADER_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_GET_ACC_MEP_CUSTOM_HEADER_GET_ACC_MEP_NDX_ID, 
    "acc_mep_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  acc_mep_ndx:\n\r"
    "  The user-defined handle, selected when the accelerated MEP was defined (see\n\r"
    "  pcp_oam_eth_mp_info_set()). Range: 0-4K	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_custom_header_get acc_mep_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_GET_ACC_MEP_CUSTOM_HEADER_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_ID, 
    "acc_lmm_set", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function sets LMM info for the specified LMM identifier.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_lmm_set acc_lmm_ndx 0 priority 0 da 0 interval 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_ACC_LMM_NDX_ID, 
    "acc_lmm_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  acc_lmm_ndx:\n\r"
    "  LMM user-defined handle, selected when the accelerated MEP was defined (see\n\r"
    "  pcp_oam_eth_acc_mep_info_set()). Range: 0-127	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_lmm_set acc_lmm_ndx 0 priority 0 da 0 interval 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_ID), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_INFO_PRIORITY_ID, 
    "priority", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.priority:\n\r"
    "  Determines the priority of the generated CFM packets\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_lmm_set acc_lmm_ndx 0 priority 0 da 0 interval 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_INFO_DA_ID, 
    "da", 
    (PARAM_VAL_RULES *)&pcp_api_mac_addr_vals[0], 
    (sizeof(pcp_api_mac_addr_vals) / sizeof(pcp_api_mac_addr_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.da:\n\r"
    "  LMM destination MAC address. Used when OAMP injects LMM packets.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_lmm_set acc_lmm_ndx 0 priority 0 da 0 interval 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_INFO_INTERVAL_ID, 
    "interval", 
    (PARAM_VAL_RULES *)&PCP_OAM_ETH_INTERVAL_rule[0], 
    (sizeof(PCP_OAM_ETH_INTERVAL_rule) / sizeof(PCP_OAM_ETH_INTERVAL_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.interval:\n\r"
    "  Interval between consecutive LMM messages. \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_lmm_set acc_lmm_ndx 0 priority 0 da 0 interval 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },
  { 
    PARAM_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_INFO_RX_COUNTER_ID, 
    "rx_counter", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.rx_counter:\n\r"
    "  11 msb bits of pointer to receive counters in statistics DB. \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_lmm_set acc_lmm_ndx 0 priority 0 da 0 interval 0 rx_counter 0 tx_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_INFO_TX_COUNTER_ID, 
    "tx_counter", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.tx_counter:\n\r"
    "  11 msb bits of pointer to transmit counters in statistics DB. \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_lmm_set acc_lmm_ndx 0 priority 0 da 0 interval 0 rx_counter 0 tx_counter 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_LMM_GET_ACC_LMM_GET_ID, 
    "acc_lmm_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function sets LMM info for the specified LMM identifier.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_lmm_get acc_lmm_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_LMM_GET_ACC_LMM_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_LMM_GET_ACC_LMM_GET_ACC_LMM_NDX_ID, 
    "acc_lmm_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  acc_lmm_ndx:\n\r"
    "  LMM user-defined handle, selected when the accelerated MEP was defined (see\n\r"
    "  pcp_oam_eth_acc_mep_info_set()). Range: 0-127	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_lmm_get acc_lmm_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_LMM_GET_ACC_LMM_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_DLM_SET_ACC_DLM_SET_ID, 
    "acc_dlm_set", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function sets DLM info for the specified DLM identifier.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_dlm_set acc_dlm_ndx 0 priority 0 da 0 interval 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_DLM_SET_ACC_DLM_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_DLM_SET_ACC_DLM_SET_ACC_DLM_NDX_ID, 
    "acc_dlm_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  acc_dlm_ndx:\n\r"
    "  DLM user-defined handle, selected when the accelerated MEP was defined (see\n\r"
    "  pcp_oam_eth_acc_mep_info_set()). Range: 0-127	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_dlm_set acc_dlm_ndx 0 priority 0 da 0 interval 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_DLM_SET_ACC_DLM_SET_ID), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_DLM_SET_ACC_DLM_SET_INFO_PRIORITY_ID, 
    "priority", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.priority:\n\r"
    "  Determines the priority of the generated CFM packets\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_dlm_set acc_dlm_ndx 0 priority 0 da 0 interval 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_DLM_SET_ACC_DLM_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_DLM_SET_ACC_DLM_SET_INFO_DA_ID, 
    "da", 
    (PARAM_VAL_RULES *)&pcp_api_mac_addr_vals[0], 
    (sizeof(pcp_api_mac_addr_vals) / sizeof(pcp_api_mac_addr_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.da:\n\r"
    "  Destination MAC address of the DLM packet\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_dlm_set acc_dlm_ndx 0 priority 0 da 0 interval 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_DLM_SET_ACC_DLM_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_DLM_SET_ACC_DLM_SET_INFO_INTERVAL_ID, 
    "interval", 
    (PARAM_VAL_RULES *)&PCP_OAM_ETH_INTERVAL_rule[0], 
    (sizeof(PCP_OAM_ETH_INTERVAL_rule) / sizeof(PCP_OAM_ETH_INTERVAL_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.interval:\n\r"
    "  Interval between consecutive DLM messages\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_dlm_set acc_dlm_ndx 0 priority 0 da 0 interval 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_DLM_SET_ACC_DLM_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_DLM_GET_ACC_DLM_GET_ID, 
    "acc_dlm_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function sets DLM info for the specified DLM identifier.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_dlm_get acc_dlm_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_DLM_GET_ACC_DLM_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_DLM_GET_ACC_DLM_GET_ACC_DLM_NDX_ID, 
    "acc_dlm_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  acc_dlm_ndx:\n\r"
    "  DLM user-defined handle, selected when the accelerated MEP was defined (see\n\r"
    "  pcp_oam_eth_acc_mep_info_set()). Range: 0-127	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_dlm_get acc_dlm_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_DLM_GET_ACC_DLM_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_RMEP_ADD_RMEP_ADD_ID, 
    "rmep_add", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function adds a remote MEP to list of remote MEPs associated with the\n\r"
    "  accelerated MEP identified by acc_mep_ndx.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth rmep_add rmep_id 0 acc_mep_id 0 rmep 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_RMEP_ADD_RMEP_ADD_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_RMEP_ADD_RMEP_ADD_RMEP_KEY_RMEP_ID_ID, 
    "rmep_id", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  rmep_key.rmep_id:\n\r"
    "  Remote MEP index as arrives on the CFM packet. Range: 0 - 8K\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth rmep_add rmep_id 0 acc_mep_id 0 rmep 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_RMEP_ADD_RMEP_ADD_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_RMEP_ADD_RMEP_ADD_RMEP_KEY_ACC_MEP_ID_ID, 
    "acc_mep_id", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  rmep_key.acc_mep_id:\n\r"
    "  Accelerated MEP index as defined by Procedure pcp_oam_eth_acc_mep_info_set().\n\r"
    "  Range: 0 - 4K\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth rmep_add rmep_id 0 acc_mep_id 0 rmep 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_RMEP_ADD_RMEP_ADD_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_RMEP_ADD_RMEP_ADD_RMEP_ID, 
    "rmep", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  rmep:\n\r"
    "  Handle to the remote MEP object that is monitored by the local MP. Range: 0 -\n\r"
    "  8K\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth rmep_add rmep_id 0 acc_mep_id 0 rmep 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_RMEP_ADD_RMEP_ADD_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_RMEP_REMOVE_RMEP_REMOVE_ID, 
    "rmep_remove", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function removes a remote MEP to list of remote MEPs associated with the\n\r"
    "  accelerated MEP identified by acc_mep_ndx.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth rmep_remove rmep_id 0 acc_mep_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_RMEP_REMOVE_RMEP_REMOVE_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_RMEP_REMOVE_RMEP_REMOVE_RMEP_KEY_RMEP_ID_ID, 
    "rmep_id", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  rmep_key.rmep_id:\n\r"
    "  Remote MEP index as arrives on the CFM packet. Range: 0 - 8K\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth rmep_remove rmep_id 0 acc_mep_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_RMEP_REMOVE_RMEP_REMOVE_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_RMEP_REMOVE_RMEP_REMOVE_RMEP_KEY_ACC_MEP_ID_ID, 
    "acc_mep_id", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  rmep_key.acc_mep_id:\n\r"
    "  Accelerated MEP index as defined by Procedure pcp_oam_eth_acc_mep_info_set().\n\r"
    "  Range: 0 - 4K\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth rmep_remove rmep_id 0 acc_mep_id 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_RMEP_REMOVE_RMEP_REMOVE_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_RMEP_INFO_SET_RMEP_INFO_SET_ID, 
    "rmep_info_set", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This API sets a remote MEP info. \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth rmep_info_set rmep_ndx 0 local_defect 0 rdi_received 0 interval 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_RMEP_INFO_SET_RMEP_INFO_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_RMEP_INFO_SET_RMEP_INFO_SET_RMEP_NDX_ID, 
    "rmep_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  rmep_ndx:\n\r"
    "  Remote MP to be monitored by the local MP. Range: 0 - 8K	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth rmep_info_set rmep_ndx 0 local_defect 0 rdi_received 0 interval 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_RMEP_INFO_SET_RMEP_INFO_SET_ID), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_RMEP_INFO_SET_RMEP_INFO_SET_RMEP_INFO_LOCAL_DEFECT_ID, 
    "local_defect", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  rmep_info.local_defect:\n\r"
    "  T - CCM session from this RMEP has timed out. When CPU sets this flag,\n\r"
    "  timeout interrupts are suppressed. F - No timeout detected for this RMEP. If\n\r"
    "  such occurs an interrupt will be asserted\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth rmep_info_set rmep_ndx 0 local_defect 0 rdi_received 0 interval 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_RMEP_INFO_SET_RMEP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_RMEP_INFO_SET_RMEP_INFO_SET_RMEP_INFO_RDI_RECEIVED_ID, 
    "rdi_received", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  rmep_info.rdi_received:\n\r"
    "  Remote MEP RDI state.T - Remote MEP is in RDI-set state. Suppress further\n\r"
    "  RDI-set interrupts for this RMEP F - Remote MEP is in RDI-clear state.\n\r"
    "  Suppress further RDI-clear interrupts for this RMEP\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth rmep_info_set rmep_ndx 0 local_defect 0 rdi_received 0 interval 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_RMEP_INFO_SET_RMEP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_RMEP_INFO_SET_RMEP_INFO_SET_RMEP_INFO_INTERVAL_ID, 
    "interval", 
    (PARAM_VAL_RULES *)&PCP_OAM_ETH_INTERVAL_rule[0], 
    (sizeof(PCP_OAM_ETH_INTERVAL_rule) / sizeof(PCP_OAM_ETH_INTERVAL_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  rmep_info.interval:\n\r"
    "  Interval between consecutive CCM messages. Used to calc CCM timeout for\n\r"
    "  remote MEPs\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth rmep_info_set rmep_ndx 0 local_defect 0 rdi_received 0 interval 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_RMEP_INFO_SET_RMEP_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_RMEP_INFO_GET_RMEP_INFO_GET_ID, 
    "rmep_info_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This API sets a remote MEP info. \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth rmep_info_get rmep_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_RMEP_INFO_GET_RMEP_INFO_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_RMEP_INFO_GET_RMEP_INFO_GET_RMEP_NDX_ID, 
    "rmep_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  rmep_ndx:\n\r"
    "  Remote MP to be monitored by the local MP. Range: 0 - 8K	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth rmep_info_get rmep_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_RMEP_INFO_GET_RMEP_INFO_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_TX_RDI_SET_ACC_MEP_TX_RDI_SET_ID, 
    "acc_mep_tx_rdi_set", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function sets/clears remote defect indication on outgoing CCM messages\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_tx_rdi_set acc_mep_ndx 0 rdi 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_TX_RDI_SET_ACC_MEP_TX_RDI_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_TX_RDI_SET_ACC_MEP_TX_RDI_SET_ACC_MEP_NDX_ID, 
    "acc_mep_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  acc_mep_ndx:\n\r"
    "  The user-defined handle, selected when the accelerated MEP was defined (see\n\r"
    "  pcp_oam_eth_mp_info_set()). Range: 0-4K	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_tx_rdi_set acc_mep_ndx 0 rdi 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_TX_RDI_SET_ACC_MEP_TX_RDI_SET_ID), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_TX_RDI_SET_ACC_MEP_TX_RDI_SET_RDI_ID, 
    "rdi", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  rdi:\n\r"
    "  Remote defect indication T - Set RDI in outgoing CCM packetF - Do not set RDI\n\r"
    "  in outgoing CCM packet\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_tx_rdi_set acc_mep_ndx 0 rdi 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_TX_RDI_SET_ACC_MEP_TX_RDI_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_TX_RDI_GET_ACC_MEP_TX_RDI_GET_ID, 
    "acc_mep_tx_rdi_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function sets/clears remote defect indication on outgoing CCM messages\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_tx_rdi_get acc_mep_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_TX_RDI_GET_ACC_MEP_TX_RDI_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_ACC_MEP_TX_RDI_GET_ACC_MEP_TX_RDI_GET_ACC_MEP_NDX_ID, 
    "acc_mep_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  acc_mep_ndx:\n\r"
    "  The user-defined handle, selected when the accelerated MEP was defined (see\n\r"
    "  pcp_oam_eth_mp_info_set()). Range: 0-4K	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth acc_mep_tx_rdi_get acc_mep_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_ACC_MEP_TX_RDI_GET_ACC_MEP_TX_RDI_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_MEP_DELAY_MEASUREMENT_GET_MEP_DELAY_MEASUREMENT_GET_ID, 
    "mep_delay_measurement_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function returns the last measured delay. In case DM was not defined on\n\r"
    "  the provided MEP, an error is thrown\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth mep_delay_measurement_get acc_dlm_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_MEP_DELAY_MEASUREMENT_GET_MEP_DELAY_MEASUREMENT_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_MEP_DELAY_MEASUREMENT_GET_MEP_DELAY_MEASUREMENT_GET_ACC_DLM_NDX_ID, 
    "acc_dlm_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  acc_dlm_ndx:\n\r"
    "  The user-defined handle, selected when the accelerated MEP was defined (see\n\r"
    "  pcp_oam_eth_mep_info_set()). Range: 0-127	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth mep_delay_measurement_get acc_dlm_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_MEP_DELAY_MEASUREMENT_GET_MEP_DELAY_MEASUREMENT_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_MEP_LOSS_MEASUREMENT_GET_MEP_LOSS_MEASUREMENT_GET_ID, 
    "mep_loss_measurement_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function returns the loss measurement counters of the device (both local\n\r"
    "  and remote). In case LM was not defined on the provided MEP, an error is\n\r"
    "  thrown\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth mep_loss_measurement_get acc_lmm_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_MEP_LOSS_MEASUREMENT_GET_MEP_LOSS_MEASUREMENT_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_ETH_MEP_LOSS_MEASUREMENT_GET_MEP_LOSS_MEASUREMENT_GET_ACC_LMM_NDX_ID, 
    "acc_lmm_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  acc_lmm_ndx:\n\r"
    "  The user-defined handle, selected when the accelerated MEP was defined (see\n\r"
    "  pcp_oam_eth_mep_info_set()). Range: 0-127	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api eth mep_loss_measurement_get acc_lmm_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_ETH_MEP_LOSS_MEASUREMENT_GET_MEP_LOSS_MEASUREMENT_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_API_ETH_ID, 
    "eth", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    BIT_UI_16_RNG(FUN_PCP_OAM_API_ETH_FIRST_ID,FUN_PCP_OAM_API_ETH_LAST_ID), 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
#endif /* } eth*/
#ifdef UI_BFD/* { bfd*/
  { 
    PARAM_PCP_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_ID, 
    "ttl_mapping_info_set", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function maps 2 bit ttl_ndx to TTL information. This mapping is used to\n\r"
    "  build BFD packets. Sets the TTL of the MPLS tunnel label, and in case PWE\n\r"
    "  protection is applied, also sets the MPLS PWE label TTL.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd ttl_mapping_info_set ttl_ndx 0 pwe_ttl 0 tnl_ttl 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_TTL_NDX_ID, 
    "ttl_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  ttl_ndx:\n\r"
    "  TTL profile identifier. Range: 0-3	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd ttl_mapping_info_set ttl_ndx 0 pwe_ttl 0 tnl_ttl 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_ID), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_INFO_PWE_TTL_ID, 
    "pwe_ttl", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.pwe_ttl:\n\r"
    "  PWE TTL. Range: 0-255\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd ttl_mapping_info_set ttl_ndx 0 pwe_ttl 0 tnl_ttl 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_INFO_TNL_TTL_ID, 
    "tnl_ttl", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.tnl_ttl:\n\r"
    "  Tunnel TTL. Range: 0-255\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd ttl_mapping_info_set ttl_ndx 0 pwe_ttl 0 tnl_ttl 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TTL_MAPPING_INFO_GET_TTL_MAPPING_INFO_GET_ID, 
    "ttl_mapping_info_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function maps 2 bit ttl_ndx to TTL information. This mapping is used to\n\r"
    "  build BFD packets. Sets the TTL of the MPLS tunnel label, and in case PWE\n\r"
    "  protection is applied, also sets the MPLS PWE label TTL.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd ttl_mapping_info_get ttl_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TTL_MAPPING_INFO_GET_TTL_MAPPING_INFO_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TTL_MAPPING_INFO_GET_TTL_MAPPING_INFO_GET_TTL_NDX_ID, 
    "ttl_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  ttl_ndx:\n\r"
    "  TTL profile identifier. Range: 0-3	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd ttl_mapping_info_get ttl_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TTL_MAPPING_INFO_GET_TTL_MAPPING_INFO_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_ID, 
    "ip_mapping_info_set", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function sets one of four IPv4 possible source addresses. These\n\r"
    "  addresses are used by the OAMP to generate BFD messages of types MPLS, PWE\n\r"
    "  with IP, IP. \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd ip_mapping_info_set ip_ndx 0 ip 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_IP_NDX_ID, 
    "ip_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  ip_ndx:\n\r"
    "  IP profile identifier. Range 0-3	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd ip_mapping_info_set ip_ndx 0 ip 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_ID), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_INFO_IP_ID, 
    "ip", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.ip:\n\r"
    "  Source IPv4 address for BFD messages of types MPLS, PWE with IP, IP\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd ip_mapping_info_set ip_ndx 0 ip 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_IP_MAPPING_INFO_GET_IP_MAPPING_INFO_GET_ID, 
    "ip_mapping_info_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function sets one of four IPv4 possible source addresses. These\n\r"
    "  addresses are used by the OAMP to generate BFD messages of types MPLS, PWE\n\r"
    "  with IP, IP. \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd ip_mapping_info_get ip_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_IP_MAPPING_INFO_GET_IP_MAPPING_INFO_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_IP_MAPPING_INFO_GET_IP_MAPPING_INFO_GET_IP_NDX_ID, 
    "ip_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  ip_ndx:\n\r"
    "  IP profile identifier. Range 0-3	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd ip_mapping_info_get ip_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_IP_MAPPING_INFO_GET_IP_MAPPING_INFO_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_ID, 
    "my_discriminator_range_set", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function sets the range of the local accelerated discriminators. The\n\r"
    "  value of disc_range.start is subtracted from the your_discriminator field of\n\r"
    "  the incoming BFD message, to determine the entry offset to access from\n\r"
    "  disc_range.base. The physical range is [disc_range.base, disc_range.base +\n\r"
    "  disc_range.end - disc_range.start].If the packet does not match the range, it\n\r"
    "  is forwarded to CPU.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd my_discriminator_range_set end 0 start 0 base 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_DISC_RANGE_END_ID, 
    "end", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  disc_range.end:\n\r"
    "  Start of discriminator range Range: 0-232-1\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd my_discriminator_range_set end 0 start 0 base 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_DISC_RANGE_START_ID, 
    "start", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  disc_range.start:\n\r"
    "  Start of discriminator range Range: 0-232-1\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd my_discriminator_range_set end 0 start 0 base 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_DISC_RANGE_BASE_ID, 
    "base", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  disc_range.base:\n\r"
    "  Base entry in the table. Range: 0-8K\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd my_discriminator_range_set end 0 start 0 base 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_GET_MY_DISCRIMINATOR_RANGE_GET_ID, 
    "my_discriminator_range_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function sets the range of the local accelerated discriminators. The\n\r"
    "  value of disc_range.start is subtracted from the your_discriminator field of\n\r"
    "  the incoming BFD message, to determine the entry offset to access from\n\r"
    "  disc_range.base. The physical range is [disc_range.base, disc_range.base +\n\r"
    "  disc_range.end - disc_range.start].If the packet does not match the range, it\n\r"
    "  is forwarded to CPU.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd my_discriminator_range_get\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_GET_MY_DISCRIMINATOR_RANGE_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID, 
    "tx_info_set", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function configures/updates BFD message transmission information.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_MY_DISCRIMINATOR_NDX_ID, 
    "my_discriminator_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  my_discriminator_ndx:\n\r"
    "  Discriminator index. Must be in accelerated discriminator range (see\n\r"
    "  pcp_oam_bfd_my_discriminator_range_set for more information) 	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_IP_INFO_TTL_ID, 
    "ttl", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.fw_info.ip_info.ttl:\n\r"
    "  TTL (time to live) profile. One of four preconfigured sets of values\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_IP_INFO_COS_ID, 
    "cos", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.fw_info.ip_info.cos:\n\r"
    "  Class of service of transmitted packet. Mapped to traffic class and drop\n\r"
    "  precedence in ITMH\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_IP_INFO_DST_IP_ID, 
    "dst_ip", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.fw_info.ip_info.dst_ip:\n\r"
    "  IPv4 destination address\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_IP_INFO_SRC_IP_ID, 
    "src_ip", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.fw_info.ip_info.src_ip:\n\r"
    "  One of four preconfigured IPv4 source addresses\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_TTL_ID, 
    "ttl", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.fw_info.mpls_pwe_with_ip_info.ttl:\n\r"
    "  TTL (time to live) profile. One of four preconfigured sets of values\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_COS_ID, 
    "cos", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.fw_info.mpls_pwe_with_ip_info.cos:\n\r"
    "  Class of service of transmitted packet. Mapped to traffic class and drop\n\r"
    "  precedence in ITMH\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_PWE_ID, 
    "pwe", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.fw_info.mpls_pwe_with_ip_info.pwe:\n\r"
    "  PWE label to append to the packet. CW 0x7 is automatically appended\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_EEP_ID, 
    "eep", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.fw_info.mpls_pwe_with_ip_info.eep:\n\r"
    "  A pointer to the encapsulation information that should be appended to the\n\r"
    "  packet by the egress packet processor\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_SRC_IP_ID, 
    "src_ip", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.fw_info.mpls_pwe_with_ip_info.src_ip:\n\r"
    "  One of four preconfigured IPv4 source addresses\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_INFO_TTL_ID, 
    "ttl", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.fw_info.mpls_pwe_info.ttl:\n\r"
    "  TTL (time to live) profile. One of four preconfigured sets of values\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_INFO_COS_ID, 
    "cos", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.fw_info.mpls_pwe_info.cos:\n\r"
    "  CoS (Class of service) profile. One of eight preconfigured sets of values\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_INFO_PWE_ID, 
    "pwe", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.fw_info.mpls_pwe_info.pwe:\n\r"
    "  PWE label to append to the packet. CW 0x21 is automatically appended\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_INFO_EEP_ID, 
    "eep", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.fw_info.mpls_pwe_info.eep:\n\r"
    "  A pointer to the encapsulation information that should be appended to the\n\r"
    "  packet by the egress packet processor\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_TUNNEL_INFO_TTL_ID, 
    "ttl", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.fw_info.mpls_tunnel_info.ttl:\n\r"
    "  TTL (time to live) profile. One of four preconfigured sets of values\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_TUNNEL_INFO_COS_ID, 
    "cos", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.fw_info.mpls_tunnel_info.cos:\n\r"
    "  CoS (Class of service) profile. One of eight preconfigured sets of values\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_TUNNEL_INFO_EEP_ID, 
    "eep", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.fw_info.mpls_tunnel_info.eep:\n\r"
    "  A pointer to the encapsulation information that should be appended to the\n\r"
    "  packet by the egress packet processor\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_NG_INFO_MIN_TX_INTERVAL_ID, 
    "min_tx_interval", 
    (PARAM_VAL_RULES *)&PCP_OAM_BFD_TX_RATE_rule[0], 
    (sizeof(PCP_OAM_BFD_TX_RATE_rule) / sizeof(PCP_OAM_BFD_TX_RATE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.ng_info.min_tx_interval:\n\r"
    "  One of eight possible rates. This is the minimum interval, in microseconds\n\r"
    "  that the local system would like to use when transmitting BFD Control packets\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_NG_INFO_MIN_RX_INTERVAL_ID, 
    "min_rx_interval", 
    (PARAM_VAL_RULES *)&PCP_OAM_BFD_TX_RATE_rule[0], 
    (sizeof(PCP_OAM_BFD_TX_RATE_rule) / sizeof(PCP_OAM_BFD_TX_RATE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.ng_info.min_rx_interval:\n\r"
    "  One of eight possible rates. This is the minimum interval, in microseconds,\n\r"
    "  between received BFD Control packets that this system is capable of supporting\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_NG_INFO_DETECT_MULT_ID, 
    "detect_mult", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.ng_info.detect_mult:\n\r"
    "  Detection time multiplier. The negotiated transmit interval, multiplied by\n\r"
    "  this value, provides the Detection Time (timeout) for the BFD Monitoring\n\r"
    "  process\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_BFD_TYPE_ID, 
    "bfd_type", 
    (PARAM_VAL_RULES *)&PCP_OAM_BFD_TYPE_rule[0], 
    (sizeof(PCP_OAM_BFD_TYPE_rule) / sizeof(PCP_OAM_BFD_TYPE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.bfd_type:\n\r"
    "  One of four BFD packet types\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_DISCRIMINATOR_ID, 
    "discriminator", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.discriminator:\n\r"
    "  The BFD partner's discriminator\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_TX_RATE_ID, 
    "tx_rate", 
    (PARAM_VAL_RULES *)&PCP_OAM_BFD_TX_RATE_rule[0], 
    (sizeof(PCP_OAM_BFD_TX_RATE_rule) / sizeof(PCP_OAM_BFD_TX_RATE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.tx_rate:\n\r"
    "  100/50/20/10/5/2/1 PPS (every 10/20/50/100/200/500/1000 ms)The scheduler\n\r"
    "  scans the table every period. For 16K table and 5ns clock cycle this yields\n\r"
    "  0.5ms accuracy which is 1/20 of the interval\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_VALID_ID, 
    "valid", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.valid:\n\r"
    "  T - Enable OAMP message generation for this sessionF - Disable OAMP message\n\r"
    "  generation for this session\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\n\r"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\n\r"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\n\r"
    "  tx_rate 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_GET_TX_INFO_GET_ID, 
    "tx_info_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function configures/updates BFD message transmission information.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_get my_discriminator_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_GET_TX_INFO_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_TX_INFO_GET_TX_INFO_GET_MY_DISCRIMINATOR_NDX_ID, 
    "my_discriminator_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  my_discriminator_ndx:\n\r"
    "  Discriminator index. Must be in accelerated discriminator range (see\n\r"
    "  pcp_oam_bfd_my_discriminator_range_set for more information) 	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd tx_info_get my_discriminator_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_TX_INFO_GET_TX_INFO_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_RX_INFO_SET_RX_INFO_SET_ID, 
    "rx_info_set", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function configures/updates BFD message reception and session monitoring\n\r"
    "  information.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd rx_info_set my_discriminator_ndx 0 defect 0 life_time 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_RX_INFO_SET_RX_INFO_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_RX_INFO_SET_RX_INFO_SET_MY_DISCRIMINATOR_NDX_ID, 
    "my_discriminator_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  my_discriminator_ndx:\n\r"
    "  Discriminator index. Must be in accelerated discriminator range (see\n\r"
    "  pcp_oam_bfd_my_discriminator_range_set for more information) 	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd rx_info_set my_discriminator_ndx 0 defect 0 life_time 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_RX_INFO_SET_RX_INFO_SET_ID), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_RX_INFO_SET_RX_INFO_SET_INFO_DEFECT_ID, 
    "defect", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.defect:\n\r"
    "  T - All BFD packets on this session are forwarded to the CPU. F - This BFD\n\r"
    "  session is accelerated.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd rx_info_set my_discriminator_ndx 0 defect 0 life_time 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_RX_INFO_SET_RX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_RX_INFO_SET_RX_INFO_SET_INFO_LIFE_TIME_ID, 
    "life_time", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.life_time:\n\r"
    "  Calculated timeout for BFD session.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd rx_info_set my_discriminator_ndx 0 defect 0 life_time 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_RX_INFO_SET_RX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_RX_INFO_GET_RX_INFO_GET_ID, 
    "rx_info_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function configures/updates BFD message reception and session monitoring\n\r"
    "  information.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd rx_info_get my_discriminator_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_RX_INFO_GET_RX_INFO_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_BFD_RX_INFO_GET_RX_INFO_GET_MY_DISCRIMINATOR_NDX_ID, 
    "my_discriminator_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  my_discriminator_ndx:\n\r"
    "  Discriminator index. Must be in accelerated discriminator range (see\n\r"
    "  pcp_oam_bfd_my_discriminator_range_set for more information) 	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api bfd rx_info_get my_discriminator_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_BFD_RX_INFO_GET_RX_INFO_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_API_BFD_ID, 
    "bfd", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    BIT_UI_16_RNG(FUN_PCP_OAM_API_BFD_FIRST_ID,FUN_PCP_OAM_API_BFD_LAST_ID), 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
#endif /* } bfd*/
#ifdef UI_MPLS/* { mpls*/
  { 
    PARAM_PCP_OAM_MPLS_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_ID, 
    "ttl_mapping_info_set", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function maps 2 bit ttl_ndx to TTL information. This mapping is used to\n\r"
    "  build MPLS OAM packets\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls ttl_mapping_info_set ttl_ndx 0 ttl 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_TTL_NDX_ID, 
    "ttl_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  ttl_ndx:\n\r"
    "  TTL profile identifier. Range: 0-3	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls ttl_mapping_info_set ttl_ndx 0 ttl 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_ID), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_INFO_TTL_ID, 
    "ttl", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.ttl:\n\r"
    "  May be used as PWE/Tunnel label TTL, depending on the PP egress\n\r"
    "  configuration. Range: 0-255\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls ttl_mapping_info_set ttl_ndx 0 ttl 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_TTL_MAPPING_INFO_GET_TTL_MAPPING_INFO_GET_ID, 
    "ttl_mapping_info_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function maps 2 bit ttl_ndx to TTL information. This mapping is used to\n\r"
    "  build MPLS OAM packets\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls ttl_mapping_info_get ttl_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_TTL_MAPPING_INFO_GET_TTL_MAPPING_INFO_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_TTL_MAPPING_INFO_GET_TTL_MAPPING_INFO_GET_TTL_NDX_ID, 
    "ttl_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  ttl_ndx:\n\r"
    "  TTL profile identifier. Range: 0-3	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls ttl_mapping_info_get ttl_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_TTL_MAPPING_INFO_GET_TTL_MAPPING_INFO_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_ID, 
    "ip_mapping_info_set", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function maps 4 bit ip_ndx to IPv4/IPv6 information. This mapping is\n\r"
    "  used to determine the LSR identifier of the constructed MPLS OAM packets\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls ip_mapping_info_set ip_ndx 0 is_ipv6 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_IP_NDX_ID, 
    "ip_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  ip_ndx:\n\r"
    "  IP profile identifier. Range: 0-15	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls ip_mapping_info_set ip_ndx 0 is_ipv6 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_ID), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_IS_IPV6_ID, 
    "is_ipv6", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  is_ipv6:\n\r"
    "  T - The source is IPv6F - The source is IPv4	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls ip_mapping_info_set ip_ndx 0 is_ipv6 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_INFO_IPV6_IP_ID, 
    "ip", 
    (PARAM_VAL_RULES *)&pcp_api_ipv6_addr_vals[0], 
    (sizeof(pcp_api_ipv6_addr_vals) / sizeof(pcp_api_ipv6_addr_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.ipv6.ip:\n\r"
    "  IPv6 address. Used as LSR ID when constructing MPLS OAM packet \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls ip_mapping_info_set ip_ndx 0 is_ipv6 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_INFO_IPV4_IP_ID, 
    "ip", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.ipv4.ip:\n\r"
    "  IPv4 address. Used as LSR ID when constructing MPLS OAM packet \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls ip_mapping_info_set ip_ndx 0 is_ipv6 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_IP_MAPPING_INFO_GET_IP_MAPPING_INFO_GET_ID, 
    "ip_mapping_info_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function maps 4 bit ip_ndx to IPv4/IPv6 information. This mapping is\n\r"
    "  used to determine the LSR identifier of the constructed MPLS OAM packets\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls ip_mapping_info_get ip_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_IP_MAPPING_INFO_GET_IP_MAPPING_INFO_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_IP_MAPPING_INFO_GET_IP_MAPPING_INFO_GET_IP_NDX_ID, 
    "ip_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  ip_ndx:\n\r"
    "  IP profile identifier. Range: 0-15	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls ip_mapping_info_get ip_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_IP_MAPPING_INFO_GET_IP_MAPPING_INFO_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_ID, 
    "label_ranges_set", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
#ifdef COMPILER_STRING_CONST_LIMIT
    "  Full documentation cannot be displayed with -pendantic compiler\n\r",
#else
    "  This function sets label ranges for MPLS OAM. An incoming MPLS OAM packet is\n\r"
    "  accelerated if in_label (the first label after the MPLS OAM label, 14)\n\r"
    "  fulfils one of the following conditions: 1. in_label is between\n\r"
    "  tnl_range.start and tnl_range.end.2. in_label is between pwe_range.start and\n\r"
    "  pwe_range.end. pwe_range.base and tnl_range.base set the physical ranges\n\r"
    "  associated with pwe_range and tnl_range respectively. The physical range is\n\r"
    "  [range.base, range.base + range.end - range.start].if the packet does not\n\r"
    "  fulfill either conditions, it is forwarded to CPU.\n\r"
    "",
#endif /* COMPILER_STRING_CONST_LIMIT */
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls label_ranges_set end 0 start 0 base 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_TNL_RANGE_END_ID, 
    "end", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tnl_range.end:\n\r"
    "  Last label in the range. Range: 0-2^20-1\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls label_ranges_set end 0 start 0 base 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_TNL_RANGE_START_ID, 
    "start", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tnl_range.start:\n\r"
    "  First label in the range. Range: 0-2^20-1\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls label_ranges_set end 0 start 0 base 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_TNL_RANGE_BASE_ID, 
    "base", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  tnl_range.base:\n\r"
    "  Base entry in the table. Range: 0-16K\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls label_ranges_set end 0 start 0 base 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_PWE_RANGE_END_ID, 
    "end", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  pwe_range.end:\n\r"
    "  Last label in the range. Range: 0-2^20-1\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls label_ranges_set end 0 start 0 base 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_PWE_RANGE_START_ID, 
    "start", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  pwe_range.start:\n\r"
    "  First label in the range. Range: 0-2^20-1\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls label_ranges_set end 0 start 0 base 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_PWE_RANGE_BASE_ID, 
    "base", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  pwe_range.base:\n\r"
    "  Base entry in the table. Range: 0-16K\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls label_ranges_set end 0 start 0 base 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LABEL_RANGES_GET_LABEL_RANGES_GET_ID, 
    "label_ranges_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
#ifdef COMPILER_STRING_CONST_LIMIT
    "  Full documentation cannot be displayed with -pendantic compiler\n\r",
#else
    "  This function sets label ranges for MPLS OAM. An incoming MPLS OAM packet is\n\r"
    "  accelerated if in_label (the first label after the MPLS OAM label, 14)\n\r"
    "  fulfils one of the following conditions: 1. in_label is between\n\r"
    "  tnl_range.start and tnl_range.end.2. in_label is between pwe_range.start and\n\r"
    "  pwe_range.end. pwe_range.base and tnl_range.base set the physical ranges\n\r"
    "  associated with pwe_range and tnl_range respectively. The physical range is\n\r"
    "  [range.base, range.base + range.end - range.start].if the packet does not\n\r"
    "  fulfill either conditions, it is forwarded to CPU.\n\r"
    "",
#endif /* COMPILER_STRING_CONST_LIMIT */
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls label_ranges_get\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LABEL_RANGES_GET_LABEL_RANGES_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_CONFIGURABLE_FFD_RATE_SET_ID, 
    "configurable_ffd_rate_set", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function sets the configurable FFD rate. Rate is given by the interval\n\r"
    "  (milliseconds) between two consecutive frames. Range: 10 - 1270\n\r"
    "  (denominations of 10ms)\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls configurable_ffd_rate_set rate_ndx 0 interval 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_CONFIGURABLE_FFD_RATE_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_CONFIGURABLE_FFD_RATE_SET_RATE_NDX_ID, 
    "rate_ndx", 
    (PARAM_VAL_RULES *)&PCP_OAM_MPLS_CC_PKT_TYPE_rule[0], 
    (sizeof(PCP_OAM_MPLS_CC_PKT_TYPE_rule) / sizeof(PCP_OAM_MPLS_CC_PKT_TYPE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  rate_ndx:\n\r"
    "  The rate index to configure. Currently\n\r"
    "  PCP_OAM_MPLS_CC_PKT_TYPE_FFD_CONFIGURABLE is the only configurable message\n\r"
    "  type, thus is also the only allowed index to pass to this function	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls configurable_ffd_rate_set rate_ndx 0 interval 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_CONFIGURABLE_FFD_RATE_SET_ID), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_CONFIGURABLE_FFD_RATE_SET_INTERVAL_ID, 
    "interval", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  interval:\n\r"
    "  Interval between two consecutive frames in denomination of 10 ms\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls configurable_ffd_rate_set rate_ndx 0 interval 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_CONFIGURABLE_FFD_RATE_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_GET_CONFIGURABLE_FFD_RATE_GET_ID, 
    "configurable_ffd_rate_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function sets the configurable FFD rate. Rate is given by the interval\n\r"
    "  (milliseconds) between two consecutive frames. Range: 10 - 1270\n\r"
    "  (denominations of 10ms)\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls configurable_ffd_rate_get rate_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_GET_CONFIGURABLE_FFD_RATE_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_GET_CONFIGURABLE_FFD_RATE_GET_RATE_NDX_ID, 
    "rate_ndx", 
    (PARAM_VAL_RULES *)&PCP_OAM_MPLS_CC_PKT_TYPE_rule[0], 
    (sizeof(PCP_OAM_MPLS_CC_PKT_TYPE_rule) / sizeof(PCP_OAM_MPLS_CC_PKT_TYPE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  rate_ndx:\n\r"
    "  The rate index to configure. Currently\n\r"
    "  PCP_OAM_MPLS_CC_PKT_TYPE_FFD_CONFIGURABLE is the only configurable message\n\r"
    "  type, thus is also the only allowed index to pass to this function	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls configurable_ffd_rate_get rate_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_GET_CONFIGURABLE_FFD_RATE_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_ID, 
    "lsp_tx_info_set", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function configures LSP-source message generator (CV/FFD/BDI/FDI)\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls lsp_tx_info_set mot_ndx 0 fdi_enable 0 bdi_enable 0\n\r"
    "  defect_location 0 defect_type 1 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0\n\r"
    "  system_port 0 type 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_MOT_NDX_ID, 
    "mot_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  mot_ndx:\n\r"
    "  Transmitter index. Range: 0-16K	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls lsp_tx_info_set mot_ndx 0 fdi_enable 0 bdi_enable 0\n\r"
    "  defect_location 0 defect_type 1 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0\n\r"
    "  system_port 0 type 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_ID), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_DEFECT_INFO_FDI_ENABLE_ID, 
    "fdi_enable", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.defect_info.fdi_enable:\n\r"
    "  T - Enable sending of FDI messagesF - Disable sending of FDI messages\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls lsp_tx_info_set mot_ndx 0 fdi_enable 0 bdi_enable 0\n\r"
    "  defect_location 0 defect_type 1 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0\n\r"
    "  system_port 0 type 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_DEFECT_INFO_BDI_ENABLE_ID, 
    "bdi_enable", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.defect_info.bdi_enable:\n\r"
    "  T - Enable sending of BDI messagesF - Disable sending of BDI messages\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls lsp_tx_info_set mot_ndx 0 fdi_enable 0 bdi_enable 0\n\r"
    "  defect_location 0 defect_type 1 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0\n\r"
    "  system_port 0 type 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_DEFECT_INFO_DEFECT_LOCATION_ID, 
    "defect_location", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.defect_info.defect_location:\n\r"
    "  The identity of the network in which the defect has been detected\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls lsp_tx_info_set mot_ndx 0 fdi_enable 0 bdi_enable 0\n\r"
    "  defect_location 0 defect_type 1 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0\n\r"
    "  system_port 0 type 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_DEFECT_INFO_DEFECT_TYPE_ID, 
    "defect_type", 
    (PARAM_VAL_RULES *)&PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_rule[0], 
    (sizeof(PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_rule) / sizeof(PCP_OAM_MPLS_LSP_TX_DEFECT_TYPE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.defect_info.defect_type:\n\r"
    "  One of eight possible defect types as defined\n\r"
    "  in ITU-T Y.1711\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls lsp_tx_info_set mot_ndx 0 fdi_enable 0 bdi_enable 0\n\r"
    "  defect_location 0 defect_type 1 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0\n\r"
    "  system_port 0 type 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_LSP_ID_ID, 
    "lsp_id", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.packet_info.lsp_id:\n\r"
    "  The 16 lsb of the LSP tunnel identifier (16 msb are global)\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls lsp_tx_info_set mot_ndx 0 fdi_enable 0 bdi_enable 0\n\r"
    "  defect_location 0 defect_type 1 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0\n\r"
    "  system_port 0 type 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_LSR_ID_ID, 
    "lsr_id", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.packet_info.lsr_id:\n\r"
    "  One of 16 preconfigured IPv4/6 addresses\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls lsp_tx_info_set mot_ndx 0 fdi_enable 0 bdi_enable 0\n\r"
    "  defect_location 0 defect_type 1 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0\n\r"
    "  system_port 0 type 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_EEP_ID, 
    "eep", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.packet_info.eep:\n\r"
    "  A pointer to the encapsulation information that should be appended to the\n\r"
    "  packet by the egress packet processor\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls lsp_tx_info_set mot_ndx 0 fdi_enable 0 bdi_enable 0\n\r"
    "  defect_location 0 defect_type 1 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0\n\r"
    "  system_port 0 type 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_TTL_ID, 
    "ttl", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.packet_info.ttl:\n\r"
    "  TTL (Time to live) profile. One of four preconfigured sets of values\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls lsp_tx_info_set mot_ndx 0 fdi_enable 0 bdi_enable 0\n\r"
    "  defect_location 0 defect_type 1 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0\n\r"
    "  system_port 0 type 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_COS_ID, 
    "cos", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.packet_info.cos:\n\r"
    "  CoS (Class of service) profile. One of eight preconfigured sets of values\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls lsp_tx_info_set mot_ndx 0 fdi_enable 0 bdi_enable 0\n\r"
    "  defect_location 0 defect_type 1 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0\n\r"
    "  system_port 0 type 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_SYSTEM_PORT_ID, 
    "system_port", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.packet_info.system_port:\n\r"
    "  Destination system port to stamp on the ITMH\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls lsp_tx_info_set mot_ndx 0 fdi_enable 0 bdi_enable 0\n\r"
    "  defect_location 0 defect_type 1 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0\n\r"
    "  system_port 0 type 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_TYPE_ID, 
    "type", 
    (PARAM_VAL_RULES *)&PCP_OAM_MPLS_CC_PKT_TYPE_rule[0], 
    (sizeof(PCP_OAM_MPLS_CC_PKT_TYPE_rule) / sizeof(PCP_OAM_MPLS_CC_PKT_TYPE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.type:\n\r"
    "  One of eight possible preconfigured types \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls lsp_tx_info_set mot_ndx 0 fdi_enable 0 bdi_enable 0\n\r"
    "  defect_location 0 defect_type 1 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0\n\r"
    "  system_port 0 type 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_VALID_ID, 
    "valid", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.valid:\n\r"
    "  T - Enable connectivity verification transmissionF - Disable connectivity\n\r"
    "  verification transmission\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls lsp_tx_info_set mot_ndx 0 fdi_enable 0 bdi_enable 0\n\r"
    "  defect_location 0 defect_type 1 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0\n\r"
    "  system_port 0 type 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LSP_TX_INFO_GET_LSP_TX_INFO_GET_ID, 
    "lsp_tx_info_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function configures LSP-source message generator (CV/FFD/BDI/FDI)\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls lsp_tx_info_get mot_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LSP_TX_INFO_GET_LSP_TX_INFO_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LSP_TX_INFO_GET_LSP_TX_INFO_GET_MOT_NDX_ID, 
    "mot_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  mot_ndx:\n\r"
    "  Transmitter index. Range: 0-16K	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls lsp_tx_info_get mot_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LSP_TX_INFO_GET_LSP_TX_INFO_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_ID, 
    "lsp_rx_info_set", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function configures LSP-sink \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls lsp_rx_info_set mor_ndx 0 bdi_msg_type_to_filter 0\n\r"
    "  fdi_msg_type_to_filter 0 is_ffd 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_MOR_NDX_ID, 
    "mor_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  mor_ndx:\n\r"
    "  Offset of the incoming MPLS Label from the base of the MPLS-labels-range as\n\r"
    "  defined in pcp_oam_mpls_label_ranges_set(). Range: 0-16K	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls lsp_rx_info_set mor_ndx 0 bdi_msg_type_to_filter 0\n\r"
    "  fdi_msg_type_to_filter 0 is_ffd 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_ID), 
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_INFO_BDI_MSG_TYPE_TO_FILTER_ID, 
    "bdi_msg_type_to_filter", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.bdi_msg_type_to_filter:\n\r"
    "  Specified the message defect type to filter. OAM processor filters BDI\n\r"
    "  message with defect type equals to the one specified for this field.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls lsp_rx_info_set mor_ndx 0 bdi_msg_type_to_filter 0\n\r"
    "  fdi_msg_type_to_filter 0 is_ffd 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_INFO_FDI_MSG_TYPE_TO_FILTER_ID, 
    "fdi_msg_type_to_filter", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.fdi_msg_type_to_filter:\n\r"
    "  Specified the message defect type to filter. OAM processor filters FDI\n\r"
    "  message with defect type equals to the one specified for this field.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls lsp_rx_info_set mor_ndx 0 bdi_msg_type_to_filter 0\n\r"
    "  fdi_msg_type_to_filter 0 is_ffd 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_INFO_IS_FFD_ID, 
    "is_ffd", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.is_ffd:\n\r"
    "  T - The connectivity verification message received is expected to be of FFD\n\r"
    "  typeF - The connectivity verification message received is expected to be of\n\r"
    "  CV type\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls lsp_rx_info_set mor_ndx 0 bdi_msg_type_to_filter 0\n\r"
    "  fdi_msg_type_to_filter 0 is_ffd 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_INFO_VALID_ID, 
    "valid", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.valid:\n\r"
    "  T - Entry is valid. Handles incoming CV/FFC messages F - Entry is invalid.\n\r"
    "  Any incoming CV/FFD message to it is forwarded to the control-plane\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls lsp_rx_info_set mor_ndx 0 bdi_msg_type_to_filter 0\n\r"
    "  fdi_msg_type_to_filter 0 is_ffd 0 valid 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LSP_RX_INFO_GET_LSP_RX_INFO_GET_ID, 
    "lsp_rx_info_get", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function configures LSP-sink \n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls lsp_rx_info_get mor_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LSP_RX_INFO_GET_LSP_RX_INFO_GET_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_MPLS_LSP_RX_INFO_GET_LSP_RX_INFO_GET_MOR_NDX_ID, 
    "mor_ndx", 
    (PARAM_VAL_RULES *)&pcp_api_free_vals[0], 
    (sizeof(pcp_api_free_vals) / sizeof(pcp_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  mor_ndx:\n\r"
    "  Offset of the incoming MPLS Label from the base of the MPLS-labels-range as\n\r"
    "  defined in pcp_oam_mpls_label_ranges_set(). Range: 0-16K	\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api mpls lsp_rx_info_get mor_ndx 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_OAM_MPLS_LSP_RX_INFO_GET_LSP_RX_INFO_GET_ID), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_OAM_API_MPLS_ID, 
    "mpls", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    BIT_UI_16_RNG(FUN_PCP_OAM_API_MPLS_FIRST_ID,FUN_PCP_OAM_API_MPLS_LAST_ID), 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
#endif /* } mpls*/
#ifdef UI_FRAMEWORK/* { framework*/
  { 
    PARAM_PCP_PROCEDURE_DESC_ADD_PROCEDURE_DESC_ADD_ID, 
    "procedure_desc_add", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Add the pool of SOC_SAND_FAP20V procedure descriptors to the all-system sorted pool.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api framework procedure_desc_add\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_PROCEDURE_DESC_ADD_PROCEDURE_DESC_ADD_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_ERRORS_DESC_ADD_ERRORS_DESC_ADD_ID, 
    "errors_desc_add", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Add the pool of error descriptors to the all-system sorted pool.\n\r"
    "",
#if UI_PCP_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  pcp_api framework errors_desc_add\n\r"
#endif 
    "",
    "",
    BIT_UI_16(FUN_PCP_ERRORS_DESC_ADD_ERRORS_DESC_ADD_ID), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_PCP_API_FRAMEWORK_ID, 
    "framework", 
    (PARAM_VAL_RULES *)&pcp_api_empty_vals[0], 
    (sizeof(pcp_api_empty_vals) / sizeof(pcp_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "",
    "", 
    "",
    BIT_UI_16_RNG(FUN_PCP_API_FRAMEWORK_FIRST_ID,FUN_PCP_API_FRAMEWORK_LAST_ID), 
    1, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
#endif /* } framework*/

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

/* } __UI_ROM_DEFI_PCP_API_INCLUDED__*/
#endif

