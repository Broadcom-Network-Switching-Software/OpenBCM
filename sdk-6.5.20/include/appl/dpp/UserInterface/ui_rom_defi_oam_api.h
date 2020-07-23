/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __UI_ROM_DEFI_OAM_API_INCLUDED__
/* { */
#define __UI_ROM_DEFI_OAM_API_INCLUDED__

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/oam/oam_api_eth.h>
#include <soc/dpp/oam/oam_api_bfd.h>
#include <soc/dpp/oam/oam_api_general.h>
#include <soc/dpp/oam/oam_api_mpls.h>

#include <appl/dpp/UserInterface/ui_pure_defi_oam_api.h>

/*
  * ENUM RULES
  * {
  */
/********************************************************/
/********************************************************/

EXTERN CONST
   PARAM_VAL_RULES
     OAM_MSG_EXCEPTION_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "ccm_timeout",
    {
      {
        OAM_MSG_CCM_TIMEOUT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_MSG_EXCEPTION_TYPE.OAM_MSG_CCM_TIMEOUT:\r\n"
        "  CCM session (identified by local MEP handle and remote MEP ID) has timed out.\r\n"
        "  info1 returns the local MEP handle. info2 returns the ID of the remote MEP. \r\n"
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
    "bfd_timeout",
    {
      {
        OAM_MSG_BFD_TIMEOUT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_MSG_EXCEPTION_TYPE.OAM_MSG_BFD_TIMEOUT:\r\n"
        "  BFD session has timed out. info1 returns My-discriminator of the timed-out\r\n"
        "  session. info2 is not used.\r\n"
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
    "mpls_dlockv_set",
    {
      {
        OAM_MSG_MPLS_DLOCKV_SET,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_MSG_EXCEPTION_TYPE.OAM_MSG_MPLS_TIMEOUT:\r\n"
        "  Monitored LSP has timed out. info1 returns the label of the timed-out\r\n"
        "  LSPInfo2 returns: '0' if monitored LSP is PWE'1' if monitored LSP is Tunnel\r\n"
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
    "mpls_dlockv_clear",
    {
      {
        OAM_MSG_MPLS_DLOCKV_CLEAR,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_MSG_EXCEPTION_TYPE.OAM_MSG_MPLS_RENEW:\r\n"
        "  Monitored LSP has recovered. info1 returns the label of the recovered\r\n"
        "  LSPInfo2 returns: '0' if monitored LSP is PWE'1' if monitored LSP is Tunnel\r\n"
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
    "mpls_fdi_clear",
    {
      {
        OAM_MSG_MPLS_FDI_CLEAR,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_MSG_EXCEPTION_TYPE.OAM_MSG_MPLS_FDI_CLEAR:\r\n"
        "  FDI state of the monitored LSP has been cleared (FDI messages ceased\r\n"
        "  arriving). info1 returns the label of the monitored LSPInfo2 returns: '0' if\r\n"
        "  monitored LSP is PWE'1' if monitored LSP is Tunnel\r\n"
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
    "mpls_bdi_clear",
    {
      {
        OAM_MSG_MPLS_BDI_CLEAR,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_MSG_EXCEPTION_TYPE.OAM_MSG_MPLS_BDI_CLEAR:\r\n"
        "  BDI state of the monitored LSP has been cleared (BDI messages ceased\r\n"
        "  arriving). info1 returns the label of the monitored LSPInfo2 returns: '0' if\r\n"
        "  monitored LSP is PWE'1' if monitored LSP is Tunnel\r\n"
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
     OAM_ETH_MEP_DIRECTION_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "up",
    {
      {
        OAM_ETH_MEP_DIRECTION_UP,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_ETH_MEP_DIRECTION.OAM_ETH_MEP_DIRECTION_UP:\r\n"
        "  Up MEP is a MEP that is external to the monitored domain.\r\n"
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
        OAM_ETH_MEP_DIRECTION_DOWN,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_ETH_MEP_DIRECTION.OAM_ETH_MEP_DIRECTION_DOWN:\r\n"
        "  Down MEP is a MEP that is internal to the monitored domain.\r\n"
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
     OAM_ETH_MP_LEVEL_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "0",
    {
      {
        OAM_ETH_MP_LEVEL_0,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_ETH_MP_LEVEL.OAM_ETH_MP_LEVEL_0:\r\n"
        "  Maintenance point level 0\r\n"
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
    "1",
    {
      {
        OAM_ETH_MP_LEVEL_1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_ETH_MP_LEVEL.OAM_ETH_MP_LEVEL_1:\r\n"
        "  Maintenance point level 1\r\n"
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
    "2",
    {
      {
        OAM_ETH_MP_LEVEL_2,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_ETH_MP_LEVEL.OAM_ETH_MP_LEVEL_2:\r\n"
        "  Maintenance point level 2\r\n"
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
    "3",
    {
      {
        OAM_ETH_MP_LEVEL_3,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_ETH_MP_LEVEL.OAM_ETH_MP_LEVEL_3:\r\n"
        "  Maintenance point level 3\r\n"
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
    "4",
    {
      {
        OAM_ETH_MP_LEVEL_4,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_ETH_MP_LEVEL.OAM_ETH_MP_LEVEL_4:\r\n"
        "  Maintenance point level 4\r\n"
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
    "5",
    {
      {
        OAM_ETH_MP_LEVEL_5,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_ETH_MP_LEVEL.OAM_ETH_MP_LEVEL_5:\r\n"
        "  Maintenance point level 5\r\n"
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
    "6",
    {
      {
        OAM_ETH_MP_LEVEL_6,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_ETH_MP_LEVEL.OAM_ETH_MP_LEVEL_6:\r\n"
        "  Maintenance point level 6\r\n"
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
    "7",
    {
      {
        OAM_ETH_MP_LEVEL_7,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_ETH_MP_LEVEL.OAM_ETH_MP_LEVEL_7:\r\n"
        "  Maintenance point level 7\r\n"
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
     OAM_ETH_INTERVAL_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "0",
    {
      {
        OAM_ETH_INTERVAL_0,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_ETH_INTERVAL.OAM_ETH_INTERVAL_0:\r\n"
        "  CCM every 3.3 milliseconds\r\n"
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
    "1",
    {
      {
        OAM_ETH_INTERVAL_1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_ETH_INTERVAL.OAM_ETH_INTERVAL_1:\r\n"
        "  CCM every 10 milliseconds\r\n"
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
    "2",
    {
      {
        OAM_ETH_INTERVAL_2,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_ETH_INTERVAL.OAM_ETH_INTERVAL_2:\r\n"
        "  CCM every 100 milliseconds\r\n"
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
    "3",
    {
      {
        OAM_ETH_INTERVAL_3,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_ETH_INTERVAL.OAM_ETH_INTERVAL_3:\r\n"
        "  CCM every 1 seconds\r\n"
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
    "4",
    {
      {
        OAM_ETH_INTERVAL_4,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_ETH_INTERVAL.OAM_ETH_INTERVAL_4:\r\n"
        "  CCM every 10 seconds\r\n"
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
    "5",
    {
      {
        OAM_ETH_INTERVAL_5,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_ETH_INTERVAL.OAM_ETH_INTERVAL_5:\r\n"
        "  CCM every 1 minutes\r\n"
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
    "6",
    {
      {
        OAM_ETH_INTERVAL_6,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_ETH_INTERVAL.OAM_ETH_INTERVAL_6:\r\n"
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the
           * compiler silent.
           */
          /*
           * Casting added here just to keep the compiler silent.
           */
        "  CCM every 10 minutes\r\n"
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
     OAM_BFD_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "mpls",
    {
      {
        OAM_BFD_TYPE_MPLS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_BFD_TYPE.OAM_BFD_TYPE_MPLS:\r\n"
        "  Tunnel label, routable source IP, destination IP 127/8, protocol UDP, UDP\r\n"
        "  port 3784, BFD PDU\r\n"
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
        OAM_BFD_TYPE_MPLS_PWE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_BFD_TYPE.OAM_BFD_TYPE_MPLS_PWE:\r\n"
        "  Tunnel label, PWE label, CW 0x21, routable source IP, destination IP,\r\n"
        "  protocol UDP, UDP port 3784, BFD PDU\r\n"
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
        OAM_BFD_TYPE_MPLS_PWE_WITH_IP,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_BFD_TYPE.OAM_BFD_TYPE_MPLS_PWE_WITH_IP:\r\n"
        "  Tunnel label, PWE label, CW 0x7, BFD PDU\r\n"
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
        OAM_BFD_TYPE_IP,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_BFD_TYPE.OAM_BFD_TYPE_IP:\r\n"
        "  Routable source IP, routable destination IP, protocol UDP, UDP port 3784, BFD\r\n"
        "  PDU\r\n"
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
     OAM_BFD_TX_RATE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "1",
    {
      {
        OAM_BFD_TX_RATE_1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_BFD_TX_RATE.OAM_BFD_TX_RATE_1:\r\n"
        "  1 PPS (every 1000 ms)\r\n"
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
    "2",
    {
      {
        OAM_BFD_TX_RATE_2,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_BFD_TX_RATE.OAM_BFD_TX_RATE_2:\r\n"
        "  2 PPS (every 500 ms)\r\n"
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
    "5",
    {
      {
        OAM_BFD_TX_RATE_5,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_BFD_TX_RATE.OAM_BFD_TX_RATE_5:\r\n"
        "  5 PPS (every 200 ms)\r\n"
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
    "10",
    {
      {
        OAM_BFD_TX_RATE_10,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_BFD_TX_RATE.OAM_BFD_TX_RATE_10:\r\n"
        "  10 PPS (every 100 ms)\r\n"
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
    "20",
    {
      {
        OAM_BFD_TX_RATE_20,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_BFD_TX_RATE.OAM_BFD_TX_RATE_20:\r\n"
        "  20 PPS (every 50 ms)\r\n"
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
    "50",
    {
      {
        OAM_BFD_TX_RATE_50,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_BFD_TX_RATE.OAM_BFD_TX_RATE_50:\r\n"
        "  50 PPS (every 20 ms)\r\n"
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
    "100",
    {
      {
        OAM_BFD_TX_RATE_100,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_BFD_TX_RATE.OAM_BFD_TX_RATE_100:\r\n"
        "  100 PPS (every 10 ms)\r\n"
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
     OAM_MPLS_CC_PKT_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "ffd_configurable",
    {
      {
        OAM_MPLS_CC_PKT_TYPE_FFD_CONFIGURABLE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_MPLS_CC_PKT_TYPE.OAM_MPLS_CC_PKT_TYPE_FFD_CONFIGURABLE:\r\n"
        "  Configurable rate\r\n"
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
        OAM_MPLS_CC_PKT_TYPE_FFD_10,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_MPLS_CC_PKT_TYPE.OAM_MPLS_CC_PKT_TYPE_FFD_10:\r\n"
        "  Frame (FFD) each 10 ms\r\n"
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
        OAM_MPLS_CC_PKT_TYPE_FFD_20,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_MPLS_CC_PKT_TYPE.OAM_MPLS_CC_PKT_TYPE_FFD_20:\r\n"
        "  Frame (FFD) each 20 ms\r\n"
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
        OAM_MPLS_CC_PKT_TYPE_FFD_50,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_MPLS_CC_PKT_TYPE.OAM_MPLS_CC_PKT_TYPE_FFD_50:\r\n"
        "  Frame (FFD) each 50 ms\r\n"
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
        OAM_MPLS_CC_PKT_TYPE_FFD_100,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_MPLS_CC_PKT_TYPE.OAM_MPLS_CC_PKT_TYPE_FFD_100:\r\n"
        "  Frame (FFD) each 100 ms\r\n"
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
        OAM_MPLS_CC_PKT_TYPE_FFD_200,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_MPLS_CC_PKT_TYPE.OAM_MPLS_CC_PKT_TYPE_FFD_200:\r\n"
        "  Frame (FFD) each 200 ms\r\n"
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
        OAM_MPLS_CC_PKT_TYPE_FFD_500,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_MPLS_CC_PKT_TYPE.OAM_MPLS_CC_PKT_TYPE_FFD_500:\r\n"
        "  Frame (FFD) each 500 ms\r\n"
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
        OAM_MPLS_CC_PKT_TYPE_FFD_1000,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_MPLS_CC_PKT_TYPE.OAM_MPLS_CC_PKT_TYPE_FFD_1000:\r\n"
        "  Frame (FFD) each 1000 ms\r\n"
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
        OAM_MPLS_CC_PKT_TYPE_CV,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_MPLS_CC_PKT_TYPE.OAM_MPLS_CC_PKT_TYPE_CV:\r\n"
        "  Frame (CV) each 1000 ms\r\n"
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
     OAM_MPLS_LSP_TX_DEFECT_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "server",
    {
      {
        OAM_MPLS_LSP_TX_DEFECT_TYPE_SERVER,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_MPLS_LSP_TX_DEFECT_TYPE.OAM_MPLS_LSP_TX_DEFECT_TYPE_SERVER:\r\n"
        "  As defined in ITU Y.1711\r\n"
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
        OAM_MPLS_LSP_TX_DEFECT_TYPE_PEER_ME,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_MPLS_LSP_TX_DEFECT_TYPE.OAM_MPLS_LSP_TX_DEFECT_TYPE_PEER_ME:\r\n"
        "  As defined in ITU Y.1711\r\n"
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
        OAM_MPLS_LSP_TX_DEFECT_TYPE_DLOCV,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_MPLS_LSP_TX_DEFECT_TYPE.OAM_MPLS_LSP_TX_DEFECT_TYPE_DLOCV:\r\n"
        "  As defined in ITU Y.1711\r\n"
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
        OAM_MPLS_LSP_TX_DEFECT_TYPE_TTSI_MISMATCH,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_MPLS_LSP_TX_DEFECT_TYPE.OAM_MPLS_LSP_TX_DEFECT_TYPE_TTSI_MISMATCH:\r\n"
        "  As defined in ITU Y.1711\r\n"
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
        OAM_MPLS_LSP_TX_DEFECT_TYPE_TTSI_MISMERGE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_MPLS_LSP_TX_DEFECT_TYPE.OAM_MPLS_LSP_TX_DEFECT_TYPE_TTSI_MISMERGE:\r\n"
        "  As defined in ITU Y.1711\r\n"
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
        OAM_MPLS_LSP_TX_DEFECT_TYPE_DEXCESS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_MPLS_LSP_TX_DEFECT_TYPE.OAM_MPLS_LSP_TX_DEFECT_TYPE_EXCESS:\r\n"
        "  As defined in ITU Y.1711\r\n"
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
        OAM_MPLS_LSP_TX_DEFECT_TYPE_UNKNOWN,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_MPLS_LSP_TX_DEFECT_TYPE.OAM_MPLS_LSP_TX_DEFECT_TYPE_UNKNOWN:\r\n"
        "  As defined in ITU Y.1711\r\n"
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
        OAM_MPLS_LSP_TX_DEFECT_TYPE_NONE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  OAM_MPLS_LSP_TX_DEFECT_TYPE.OAM_MPLS_LSP_TX_DEFECT_TYPE_NONE:\r\n"
        "  As defined in ITU Y.1711\r\n"
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
     Oam_api_ipv4_addr_vals[]
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
     Oam_api_ipv6_addr_vals[]
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
     Oam_api_mac_addr_vals[]
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

EXTERN CONST
   PARAM_VAL_RULES
     Oam_api_free_vals[]
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
     Oam_api_empty_vals[]
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
     Oam_api_params[]
#ifdef INIT
   =
{
#ifdef UI_GENERAL/* { general*/
  {
    PARAM_OAM_GENERAL_INFO_SET_INFO_SET_ID,
    "info_set",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This procedure apply general configuration of the OAMP. \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api general info_set cpu_dp 0 cpu_tc 0 cpu_port 0 source_sys_port 0\r\n"
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
    PARAM_OAM_GENERAL_INFO_SET_INFO_SET_INFO_CPU_DP_ID,
    "cpu_dp",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.cpu_dp:\r\n"
    "  Drop precedence. Used to construct ITMH when trapping packets to CPU from the\r\n"
    "  OAMP.\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api general info_set cpu_dp 0 cpu_tc 0 cpu_port 0 source_sys_port 0\r\n"
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
    PARAM_OAM_GENERAL_INFO_SET_INFO_SET_INFO_CPU_TC_ID,
    "cpu_tc",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.cpu_tc:\r\n"
    "  Traffic class. Used to construct ITMH when trapping packets to CPU from the\r\n"
    "  OAMP.\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api general info_set cpu_dp 0 cpu_tc 0 cpu_port 0 source_sys_port 0\r\n"
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
    PARAM_OAM_GENERAL_INFO_SET_INFO_SET_INFO_CPU_PORT_ID,
    "cpu_port",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.cpu_port:\r\n"
    "  CPU port. Used to construct ITMH when trapping packets to CPU from the OAMP.\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api general info_set cpu_dp 0 cpu_tc 0 cpu_port 0 source_sys_port 0\r\n"
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
    PARAM_OAM_GENERAL_INFO_SET_INFO_SET_INFO_SOURCE_SYS_PORT_ID,
    "source_sys_port",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.source_sys_port:\r\n"
    "  Source system port. Used to construct ITMH source port extension when\r\n"
    "  trapping packets to CPU from the OAMP..\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api general info_set cpu_dp 0 cpu_tc 0 cpu_port 0 source_sys_port 0\r\n"
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
    PARAM_OAM_GENERAL_INFO_GET_INFO_GET_ID,
    "info_get",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This procedure apply general configuration of the OAMP. \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api general info_get\r\n"
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
    PARAM_OAM_MSG_INFO_GET_MSG_INFO_GET_ID,
    "msg_info_get",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function reads and parses the head of the OAM message FIFO. If no\r\n"
    "  message exists an indication is returned.\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api general msg_info_get\r\n"
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
    PARAM_OAM_API_GENERAL_ID,
    "general",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4), 0, 0, 0},
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
    PARAM_OAM_ETH_GLOBAL_INFO_SET_GLOBAL_INFO_SET_ID,
    "global_info_set",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function maps the class of service to traffic class (for down MEP), drop\r\n"
    "  precedence (for down MEP) and user priority (for both up and down MEPs)\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth global_info_set up_mep_mac 0 dn_mep_mac 0\r\n"
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
    PARAM_OAM_ETH_GLOBAL_INFO_SET_GLOBAL_INFO_SET_INFO_UP_MEP_MAC_ID,
    "up_mep_mac",
    (PARAM_VAL_RULES *)&Oam_api_mac_addr_vals[0],
    (sizeof(Oam_api_mac_addr_vals) / sizeof(Oam_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.up_mep_mac:\r\n"
    "  Source MAC address. Applicable only for up MEPs. Used for:Tx: SA of\r\n"
    "  constructed CFMsRx: Validation of incoming CFMs' DA\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth global_info_set up_mep_mac 0 dn_mep_mac 0\r\n"
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
    PARAM_OAM_ETH_GLOBAL_INFO_SET_GLOBAL_INFO_SET_INFO_DN_MEP_MAC_ID,
    "dn_mep_mac",
    (PARAM_VAL_RULES *)&Oam_api_mac_addr_vals[0],
    (sizeof(Oam_api_mac_addr_vals) / sizeof(Oam_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.dn_mep_mac:\r\n"
    "  Base source MAC address. Applicable only for down MEPs. The MAC address is\r\n"
    "  calculated by ((dn_mep_mac & 0xffffffe000) | sys_dest_port), where\r\n"
    "  sys_dest_port is the system destination port to this base MAC address. Used\r\n"
    "  for:Tx: SA of constructed CFMsRx: Validation of incoming CFMs' DA\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth global_info_set up_mep_mac 0 dn_mep_mac 0\r\n"
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
    PARAM_OAM_ETH_GLOBAL_INFO_GET_GLOBAL_INFO_GET_ID,
    "global_info_get",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function maps the class of service to traffic class (for down MEP), drop\r\n"
    "  precedence (for down MEP) and user priority (for both up and down MEPs)\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth global_info_get\r\n"
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
    PARAM_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_ID,
    "cos_mapping_info_set",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function maps the class of service to traffic class (for down MEP), drop\r\n"
    "  precedence (for down MEP) and user priority (for both up and down MEPs)\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth cos_mapping_info_set cos_ndx 0 up 0 dp 0 tc 0\r\n"
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
    PARAM_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_COS_NDX_ID,
    "cos_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  cos_ndx:\r\n"
    "  Class of service. Range: 0-7  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth cos_mapping_info_set cos_ndx 0 up 0 dp 0 tc 0\r\n"
#endif
    "",
    "",
    {BIT(7), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_UP_ID,
    "up",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.up:\r\n"
    "  VLAN tag user priority. Range: 0-7\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth cos_mapping_info_set cos_ndx 0 up 0 dp 0 tc 0\r\n"
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
    PARAM_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_DP_ID,
    "dp",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.dp:\r\n"
    "  ITMH drop precedence. Relevant only for down MEPs. Range: 0-3\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth cos_mapping_info_set cos_ndx 0 up 0 dp 0 tc 0\r\n"
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
    PARAM_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_TC_ID,
    "tc",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.tc:\r\n"
    "  ITMH Traffic class. Relevant only for down MEPs. Range: 0-7\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth cos_mapping_info_set cos_ndx 0 up 0 dp 0 tc 0\r\n"
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
    PARAM_OAM_ETH_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_ID,
    "cos_mapping_info_get",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function maps the class of service to traffic class (for down MEP), drop\r\n"
    "  precedence (for down MEP) and user priority (for both up and down MEPs)\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth cos_mapping_info_get cos_ndx 0\r\n"
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
    PARAM_OAM_ETH_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_COS_NDX_ID,
    "cos_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  cos_ndx:\r\n"
    "  Class of service. Range: 0-7  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth cos_mapping_info_get cos_ndx 0\r\n"
#endif
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID,
    "acc_mep_info_set",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function configures an accelerated MEP.\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ACC_MEP_NDX_ID,
    "acc_mep_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acc_mep_ndx:\r\n"
    "  The user-defined handle, selected when the accelerated MEP was defined (see\r\n"
    "  oam_eth_mp_info_set()). Range: 0-4K  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
#endif
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_TST_INFO_DA_ID,
    "da",
    (PARAM_VAL_RULES *)&Oam_api_mac_addr_vals[0],
    (sizeof(Oam_api_mac_addr_vals) / sizeof(Oam_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.func_info.tst_info.da:\r\n"
    "  Destination MAC address of the TST packet\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_TST_INFO_NOF_PACKETS_ID,
    "nof_packets",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.func_info.tst_info.nof_packets:\r\n"
    "  The number of TST packet to generate\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_TST_INFO_PACKET_SIZE_ID,
    "packet_size",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.func_info.tst_info.packet_size:\r\n"
    "  The size of the constructed TST packet.\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_TST_INFO_RATE_ID,
    "rate",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.func_info.tst_info.rate:\r\n"
    "  The rate in which TST packet are sent\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_TST_INFO_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.func_info.tst_info.valid:\r\n"
    "  T - Enable Tx and Rx of TST packets for this MEPF - Disable Tx and Rx of TST\r\n"
    "  packets for this MEP\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_DLM_INFO_DA_ID,
    "da",
    (PARAM_VAL_RULES *)&Oam_api_mac_addr_vals[0],
    (sizeof(Oam_api_mac_addr_vals) / sizeof(Oam_api_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.func_info.dlm_info.da:\r\n"
    "  Destination MAC address of the DLM packet\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_DLM_INFO_INTERVAL_ID,
    "interval",
    (PARAM_VAL_RULES *)&OAM_ETH_INTERVAL_rule[0],
    (sizeof(OAM_ETH_INTERVAL_rule) / sizeof(OAM_ETH_INTERVAL_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.func_info.dlm_info.interval:\r\n"
    "  Interval between consecutive DLM messages\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_DLM_INFO_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.func_info.dlm_info.valid:\r\n"
    "  T - Enable DM for this MEPF - Disable DM for this MEP\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_CCM_INFO_LM_EN_ID,
    "lm_en",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.func_info.ccm_info.lm_en:\r\n"
    "  T - Enable LM for this MEPF - Disable LM for this MEP\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_CCM_INFO_CCM_INTERVAL_ID,
    "ccm_interval",
    (PARAM_VAL_RULES *)&OAM_ETH_INTERVAL_rule[0],
    (sizeof(OAM_ETH_INTERVAL_rule) / sizeof(OAM_ETH_INTERVAL_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.func_info.ccm_info.ccm_interval:\r\n"
    "  Interval between consecutive CCM messages\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FUNC_INFO_CCM_INFO_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.func_info.ccm_info.valid:\r\n"
    "  T - Enable CCM for this MEPF - Disable CCM for this MEP\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_COS_ID,
    "cos",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.frwd_info.down.cos:\r\n"
    "  Mapped to VLAN user priority\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_VID_ID,
    "vid",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.frwd_info.down.vid:\r\n"
    "  The VLAN of the packet\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_SOURCE_PP_PORT_ID,
    "source_pp_port",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.frwd_info.down.source_pp_port:\r\n"
    "  Source PP port. Range: 0-31\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_UP_COS_ID,
    "cos",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.frwd_info.up.cos:\r\n"
    "  Mapped to traffic class, drop precedence and VLAN user priority\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_UP_VID_ID,
    "vid",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.frwd_info.up.vid:\r\n"
    "  The VLAN of the OAM packet\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_UP_SYS_DEST_PORT_ID,
    "sys_dest_port",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.frwd_info.up.sys_dest_port:\r\n"
    "  System destination port. Refer to Soc_petra data sheet for more information.\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_UP_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.frwd_info.up.valid:\r\n"
    "  T - Enable MP for this ACF - Disable MP for this ACField name  Field\r\n"
    "  type  Field description\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_DIRECTION_ID,
    "direction",
    (PARAM_VAL_RULES *)&OAM_ETH_MEP_DIRECTION_rule[0],
    (sizeof(OAM_ETH_MEP_DIRECTION_rule) / sizeof(OAM_ETH_MEP_DIRECTION_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.direction:\r\n"
    "  MEP direction. One of two possible MEP directions.\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_MD_LEVEL_ID,
    "md_level",
    (PARAM_VAL_RULES *)&OAM_ETH_MP_LEVEL_rule[0],
    (sizeof(OAM_ETH_MP_LEVEL_rule) / sizeof(OAM_ETH_MP_LEVEL_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.md_level:\r\n"
    "  The maintenance domain level of the configured MEP. Range: 0-7\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_MEP_ID_ID,
    "mep_id",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.mep_id:\r\n"
    "  The identifier of the transmitting MEP. Stamped on the OAM packet. Unique for\r\n"
    "  all MEPs in the same maintenance association (MA) \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_MA_ID_ID,
    "ma_id",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.ma_id:\r\n"
    "  Maintenance Association is set of MEPs, each configured with the same ma_id\r\n"
    "  and maintenance domain Level, Range: 0-64K\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.valid:\r\n"
    "  T - MEP is validF - MEP is invalid. Incoming packets directed to this MEP are\r\n"
    "  trapped to CPU\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_set acc_mep_ndx 0 da 0 nof_packets 0 packet_size 0\r\n"
    "  rate 0 valid 0 da 0 interval 0 valid 0 lm_en 0 ccm_interval 0 valid 0 cos 0\r\n"
    "  vid 0 source_pp_port 0 cos 0 vid 0 sys_dest_port 0 valid 0 direction 0\r\n"
    "  md_level 0 mep_id 0 ma_id 0 valid 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_GET_ACC_MEP_INFO_GET_ID,
    "acc_mep_info_get",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function configures an accelerated MEP.\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_get acc_mep_ndx 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_INFO_GET_ACC_MEP_INFO_GET_ACC_MEP_NDX_ID,
    "acc_mep_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acc_mep_ndx:\r\n"
    "  The user-defined handle, selected when the accelerated MEP was defined (see\r\n"
    "  oam_eth_mp_info_set()). Range: 0-4K  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_info_get acc_mep_ndx 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_TX_RDI_SET_ACC_MEP_TX_RDI_SET_ID,
    "acc_mep_tx_rdi_set",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function sets/clears remote defect indication on outgoing CCM messages\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_tx_rdi_set acc_mep_ndx 0 rdi 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_TX_RDI_SET_ACC_MEP_TX_RDI_SET_ACC_MEP_NDX_ID,
    "acc_mep_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acc_mep_ndx:\r\n"
    "  The user-defined handle, selected when the accelerated MEP was defined (see\r\n"
    "  oam_eth_mp_info_set()). Range: 0-4K  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_tx_rdi_set acc_mep_ndx 0 rdi 0\r\n"
#endif
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
    PARAM_OAM_ETH_ACC_MEP_TX_RDI_SET_ACC_MEP_TX_RDI_SET_RDI_ID,
    "rdi",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  rdi:\r\n"
    "  Remote defect indication T - Set RDI in outgoing CCM packetF - Do not set RDI\r\n"
    "  in outgoing CCM packet\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_tx_rdi_set acc_mep_ndx 0 rdi 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_TX_RDI_GET_ACC_MEP_TX_RDI_GET_ID,
    "acc_mep_tx_rdi_get",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function sets/clears remote defect indication on outgoing CCM messages\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_tx_rdi_get acc_mep_ndx 0\r\n"
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
    PARAM_OAM_ETH_ACC_MEP_TX_RDI_GET_ACC_MEP_TX_RDI_GET_ACC_MEP_NDX_ID,
    "acc_mep_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acc_mep_ndx:\r\n"
    "  The user-defined handle, selected when the accelerated MEP was defined (see\r\n"
    "  oam_eth_mp_info_set()). Range: 0-4K  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth acc_mep_tx_rdi_get acc_mep_ndx 0\r\n"
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
    PARAM_OAM_ETH_REMOTE_MEP_SET_REMOTE_MEP_SET_ID,
    "remote_mep_set",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function adds/removes a remote MEP to list of remote MEPs associated\r\n"
    "  with the accelerated MEP identified by acc_mep_ndx.\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth remote_mep_set acc_mep_ndx 0 remote_mep_ndx 0 local_defect 0\r\n"
    "  rdi_received 0 ccm_interval 0 valid 0\r\n"
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
    PARAM_OAM_ETH_REMOTE_MEP_SET_REMOTE_MEP_SET_ACC_MEP_NDX_ID,
    "acc_mep_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acc_mep_ndx:\r\n"
    "  The user-defined handle, selected when the accelerated MEP was defined (see\r\n"
    "  oam_eth_mp_info_set()). Range: 0-4K  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth remote_mep_set acc_mep_ndx 0 remote_mep_ndx 0 local_defect 0\r\n"
    "  rdi_received 0 ccm_interval 0 valid 0\r\n"
#endif
    "",
    "",
    {BIT(13), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_ETH_REMOTE_MEP_SET_REMOTE_MEP_SET_REMOTE_MEP_NDX_ID,
    "remote_mep_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  remote_mep_ndx:\r\n"
    "  Remote MP to be monitored by the local MP. Range: 0 - 16K  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth remote_mep_set acc_mep_ndx 0 remote_mep_ndx 0 local_defect 0\r\n"
    "  rdi_received 0 ccm_interval 0 valid 0\r\n"
#endif
    "",
    "",
    {BIT(13), 0, 0, 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_ETH_REMOTE_MEP_SET_REMOTE_MEP_SET_INFO_LOCAL_DEFECT_ID,
    "local_defect",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.local_defect:\r\n"
    "  T - CCM session from this RMEP has timed out. When CPU sets this flag,\r\n"
    "  timeout interrupts are suppressed. F - No timeout detected for this RMEP. If\r\n"
    "  such occurs an interrupt will be asserted\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth remote_mep_set acc_mep_ndx 0 remote_mep_ndx 0 local_defect 0\r\n"
    "  rdi_received 0 ccm_interval 0 valid 0\r\n"
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
    PARAM_OAM_ETH_REMOTE_MEP_SET_REMOTE_MEP_SET_INFO_RDI_RECEIVED_ID,
    "rdi_received",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.rdi_received:\r\n"
    "  Remote MEP RDI state.T - Remote MEP is in RDI-set state. Suppress further\r\n"
    "  RDI-set interrupts for this RMEP F - Remote MEP is in RDI-clear state.\r\n"
    "  Suppress further RDI-clear interrupts for this RMEP\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth remote_mep_set acc_mep_ndx 0 remote_mep_ndx 0 local_defect 0\r\n"
    "  rdi_received 0 ccm_interval 0 valid 0\r\n"
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
    PARAM_OAM_ETH_REMOTE_MEP_SET_REMOTE_MEP_SET_INFO_CCM_INTERVAL_ID,
    "ccm_interval",
    (PARAM_VAL_RULES *)&OAM_ETH_INTERVAL_rule[0],
    (sizeof(OAM_ETH_INTERVAL_rule) / sizeof(OAM_ETH_INTERVAL_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.ccm_interval:\r\n"
    "  Interval between consecutive CCM messages. Used to calc CCM timeout for\r\n"
    "  remote MEPs\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth remote_mep_set acc_mep_ndx 0 remote_mep_ndx 0 local_defect 0\r\n"
    "  rdi_received 0 ccm_interval 0 valid 0\r\n"
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
    PARAM_OAM_ETH_REMOTE_MEP_SET_REMOTE_MEP_SET_INFO_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.valid:\r\n"
    "  T - CCM timeout calc is performed for this remote MEPF - CCM timeout calc is\r\n"
    "  not performed for this remote MEP\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth remote_mep_set acc_mep_ndx 0 remote_mep_ndx 0 local_defect 0\r\n"
    "  rdi_received 0 ccm_interval 0 valid 0\r\n"
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
    PARAM_OAM_ETH_REMOTE_MEP_GET_REMOTE_MEP_GET_ID,
    "remote_mep_get",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function adds/removes a remote MEP to list of remote MEPs associated\r\n"
    "  with the accelerated MEP identified by acc_mep_ndx.\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth remote_mep_get acc_mep_ndx 0 remote_mep_ndx 0\r\n"
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
    PARAM_OAM_ETH_REMOTE_MEP_GET_REMOTE_MEP_GET_ACC_MEP_NDX_ID,
    "acc_mep_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acc_mep_ndx:\r\n"
    "  The user-defined handle, selected when the accelerated MEP was defined (see\r\n"
    "  oam_eth_mp_info_set()). Range: 0-4K  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth remote_mep_get acc_mep_ndx 0 remote_mep_ndx 0\r\n"
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
    PARAM_OAM_ETH_REMOTE_MEP_GET_REMOTE_MEP_GET_REMOTE_MEP_NDX_ID,
    "remote_mep_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  remote_mep_ndx:\r\n"
    "  Remote MP to be monitored by the local MP. Range: 0 - 16K  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth remote_mep_get acc_mep_ndx 0 remote_mep_ndx 0\r\n"
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
    PARAM_OAM_ETH_MEP_DELAY_MEASUREMENT_GET_MEP_DELAY_MEASUREMENT_GET_ID,
    "mep_delay_measurement_get",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function returns the last measured delay. In case DM was not defined on\r\n"
    "  the provided MEP, an error is thrown\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth mep_delay_measurement_get acc_mep_ndx 0\r\n"
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
    PARAM_OAM_ETH_MEP_DELAY_MEASUREMENT_GET_MEP_DELAY_MEASUREMENT_GET_ACC_MEP_NDX_ID,
    "acc_mep_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acc_mep_ndx:\r\n"
    "  The user-defined handle, selected when the accelerated MEP was defined (see\r\n"
    "  oam_eth_mp_info_set()). Range: 0-4K  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth mep_delay_measurement_get acc_mep_ndx 0\r\n"
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
    PARAM_OAM_ETH_MEP_LOSS_MEASUREMENT_GET_MEP_LOSS_MEASUREMENT_GET_ID,
    "mep_loss_measurement_get",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function returns the loss measurement counters of the device (both local\r\n"
    "  and remote). In case LM was not defined on the provided MEP, an error is\r\n"
    "  thrown\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth mep_loss_measurement_get acc_mep_ndx 0\r\n"
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
    PARAM_OAM_ETH_MEP_LOSS_MEASUREMENT_GET_MEP_LOSS_MEASUREMENT_GET_ACC_MEP_NDX_ID,
    "acc_mep_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acc_mep_ndx:\r\n"
    "  The user-defined handle, selected when the accelerated MEP was defined (see\r\n"
    "  oam_eth_mp_info_set()).Only the first 128 MEPs are valid for loss measurement\r\n"
    "  purposes. Range: 0-127  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth mep_loss_measurement_get acc_mep_ndx 0\r\n"
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
    PARAM_OAM_ETH_MEP_TST_PACKET_COUNTER_GET_MEP_TST_PACKET_COUNTER_GET_ID,
    "mep_tst_packet_counter_get",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function returns the TST packet count\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth mep_tst_packet_counter_get\r\n"
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
    PARAM_OAM_ETH_MEP_TST_PACKET_COUNTER_CLEAR_MEP_TST_PACKET_COUNTER_CLEAR_ID,
    "mep_tst_packet_counter_clear",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function clears the TST packet counter\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api eth mep_tst_packet_counter_clear\r\n"
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
    PARAM_OAM_API_ETH_ID,
    "eth",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(5)|BIT(6)|BIT(7)|BIT(8)|BIT(9)|BIT(10)|BIT(11)|BIT(12)|BIT(13)|BIT(14)|BIT(15)|BIT(16)|BIT(17)|BIT(18), 0, 0, 0},
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
    PARAM_OAM_BFD_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_ID,
    "cos_mapping_info_set",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function maps 3 bit cos_ndx to a set of class of service attributes.\r\n"
    "  This mapping is used to build BFD packets\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd cos_mapping_info_set cos_ndx 0 pwe_exp 0 tnl_exp 0 dp 0 tc 0\r\n"
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
    PARAM_OAM_BFD_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_COS_NDX_ID,
    "cos_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  cos_ndx:\r\n"
    "  Class of service. Range: 0-7  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd cos_mapping_info_set cos_ndx 0 pwe_exp 0 tnl_exp 0 dp 0 tc 0\r\n"
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
    PARAM_OAM_BFD_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_PWE_EXP_ID,
    "pwe_exp",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.pwe_exp:\r\n"
    "  PWE label EXP. Range: 0-7\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd cos_mapping_info_set cos_ndx 0 pwe_exp 0 tnl_exp 0 dp 0 tc 0\r\n"
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
    PARAM_OAM_BFD_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_TNL_EXP_ID,
    "tnl_exp",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.tnl_exp:\r\n"
    "  Tunnel EXP. Range: 0-7\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd cos_mapping_info_set cos_ndx 0 pwe_exp 0 tnl_exp 0 dp 0 tc 0\r\n"
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
    PARAM_OAM_BFD_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_DP_ID,
    "dp",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.dp:\r\n"
    "  ITMH drop precedence. Range: 0-3\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd cos_mapping_info_set cos_ndx 0 pwe_exp 0 tnl_exp 0 dp 0 tc 0\r\n"
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
    PARAM_OAM_BFD_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_TC_ID,
    "tc",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.tc:\r\n"
    "  ITMH Traffic class. Range: 0-7\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd cos_mapping_info_set cos_ndx 0 pwe_exp 0 tnl_exp 0 dp 0 tc 0\r\n"
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
    PARAM_OAM_BFD_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_ID,
    "cos_mapping_info_get",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function maps 3 bit cos_ndx to a set of class of service attributes.\r\n"
    "  This mapping is used to build BFD packets\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd cos_mapping_info_get cos_ndx 0\r\n"
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
    PARAM_OAM_BFD_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_COS_NDX_ID,
    "cos_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  cos_ndx:\r\n"
    "  Class of service. Range: 0-7  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd cos_mapping_info_get cos_ndx 0\r\n"
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
    PARAM_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_ID,
    "ttl_mapping_info_set",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function maps 2 bit ttl_ndx to TTL information. This mapping is used to\r\n"
    "  build BFD packets. Sets the TTL of the MPLS tunnel label, and in case PWE\r\n"
    "  protection is applied, also sets the MPLS PWE label TTL.\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd ttl_mapping_info_set ttl_ndx 0 pwe_ttl 0 tnl_ttl 0\r\n"
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
    PARAM_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_TTL_NDX_ID,
    "ttl_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ttl_ndx:\r\n"
    "  TTL profile identifier. Range: 0-3  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd ttl_mapping_info_set ttl_ndx 0 pwe_ttl 0 tnl_ttl 0\r\n"
#endif
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
    PARAM_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_INFO_PWE_TTL_ID,
    "pwe_ttl",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.pwe_ttl:\r\n"
    "  PWE TTL. Range: 0-255\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd ttl_mapping_info_set ttl_ndx 0 pwe_ttl 0 tnl_ttl 0\r\n"
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
    PARAM_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_INFO_TNL_TTL_ID,
    "tnl_ttl",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.tnl_ttl:\r\n"
    "  Tunnel TTL. Range: 0-255\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd ttl_mapping_info_set ttl_ndx 0 pwe_ttl 0 tnl_ttl 0\r\n"
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
    PARAM_OAM_BFD_TTL_MAPPING_INFO_GET_TTL_MAPPING_INFO_GET_ID,
    "ttl_mapping_info_get",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function maps 2 bit ttl_ndx to TTL information. This mapping is used to\r\n"
    "  build BFD packets. Sets the TTL of the MPLS tunnel label, and in case PWE\r\n"
    "  protection is applied, also sets the MPLS PWE label TTL.\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd ttl_mapping_info_get ttl_ndx 0\r\n"
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
    PARAM_OAM_BFD_TTL_MAPPING_INFO_GET_TTL_MAPPING_INFO_GET_TTL_NDX_ID,
    "ttl_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ttl_ndx:\r\n"
    "  TTL profile identifier. Range: 0-3  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd ttl_mapping_info_get ttl_ndx 0\r\n"
#endif
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
    PARAM_OAM_BFD_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_ID,
    "ip_mapping_info_set",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function sets one of four IPv4 possible source addresses. These\r\n"
    "  addresses are used by the OAMP to generate BFD messages of types MPLS, PWE\r\n"
    "  with IP, IP. \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd ip_mapping_info_set ip_ndx 0 ip 0\r\n"
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
    PARAM_OAM_BFD_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_IP_NDX_ID,
    "ip_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ip_ndx:\r\n"
    "  IP profile identifier. Range 0-3  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd ip_mapping_info_set ip_ndx 0 ip 0\r\n"
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
    PARAM_OAM_BFD_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_INFO_IP_ID,
    "ip",
    (PARAM_VAL_RULES *)&Oam_api_ipv4_addr_vals[0],
    (sizeof(Oam_api_ipv4_addr_vals) / sizeof(Oam_api_ipv4_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.ip:\r\n"
    "  Source IPv4 address for BFD messages of types MPLS, PWE with IP, IP\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd ip_mapping_info_set ip_ndx 0 ip 0\r\n"
#endif
    "",
    "",
    {BIT(23), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_BFD_IP_MAPPING_INFO_GET_IP_MAPPING_INFO_GET_ID,
    "ip_mapping_info_get",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function sets one of four IPv4 possible source addresses. These\r\n"
    "  addresses are used by the OAMP to generate BFD messages of types MPLS, PWE\r\n"
    "  with IP, IP. \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd ip_mapping_info_get ip_ndx 0\r\n"
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
    PARAM_OAM_BFD_IP_MAPPING_INFO_GET_IP_MAPPING_INFO_GET_IP_NDX_ID,
    "ip_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ip_ndx:\r\n"
    "  IP profile identifier. Range 0-3  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd ip_mapping_info_get ip_ndx 0\r\n"
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
    PARAM_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_ID,
    "my_discriminator_range_set",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function sets the range of the local accelerated discriminators. The\r\n"
    "  value of disc_range.start is subtracted from the your_discriminator field of\r\n"
    "  the incoming BFD message, to determine the entry offset to access from\r\n"
    "  disc_range.base. The physical range is [disc_range.base, disc_range.base +\r\n"
    "  disc_range.end - disc_range.start].If the packet does not match the range, it\r\n"
    "  is forwarded to CPU.\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd my_discriminator_range_set end 0 start 0 base 0\r\n"
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
    PARAM_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_DISC_RANGE_END_ID,
    "end",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  disc_range.end:\r\n"
    "  Start of discriminator range Range: 0-232-1\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd my_discriminator_range_set end 0 start 0 base 0\r\n"
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
    PARAM_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_DISC_RANGE_START_ID,
    "start",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  disc_range.start:\r\n"
    "  Start of discriminator range Range: 0-232-1\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd my_discriminator_range_set end 0 start 0 base 0\r\n"
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
    PARAM_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_DISC_RANGE_BASE_ID,
    "base",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  disc_range.base:\r\n"
    "  Base entry in the table. Range: 0-8K\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd my_discriminator_range_set end 0 start 0 base 0\r\n"
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
    PARAM_OAM_BFD_MY_DISCRIMINATOR_RANGE_GET_MY_DISCRIMINATOR_RANGE_GET_ID,
    "my_discriminator_range_get",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function sets the range of the local accelerated discriminators. The\r\n"
    "  value of disc_range.start is subtracted from the your_discriminator field of\r\n"
    "  the incoming BFD message, to determine the entry offset to access from\r\n"
    "  disc_range.base. The physical range is [disc_range.base, disc_range.base +\r\n"
    "  disc_range.end - disc_range.start].If the packet does not match the range, it\r\n"
    "  is forwarded to CPU.\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd my_discriminator_range_get\r\n"
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
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID,
    "tx_info_set",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function configures/updates BFD message transmission information.\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
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
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_MY_DISCRIMINATOR_NDX_ID,
    "my_discriminator_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  my_discriminator_ndx:\r\n"
    "  Discriminator index. Must be in accelerated discriminator range (see\r\n"
    "  oam_bfd_my_discriminator_range_set for more information)   \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
#endif
    "",
    "",
    {BIT(27), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_IP_INFO_TTL_ID,
    "ttl",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.fw_info.ip_info.ttl:\r\n"
    "  TTL (time to live) profile. One of four preconfigured sets of values\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
#endif
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
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_IP_INFO_COS_ID,
    "cos",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.fw_info.ip_info.cos:\r\n"
    "  Class of service of transmitted packet. Mapped to traffic class and drop\r\n"
    "  precedence in ITMH\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
#endif
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
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_IP_INFO_DST_IP_ID,
    "dst_ip",
    (PARAM_VAL_RULES *)&Oam_api_ipv4_addr_vals[0],
    (sizeof(Oam_api_ipv4_addr_vals) / sizeof(Oam_api_ipv4_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.fw_info.ip_info.dst_ip:\r\n"
    "  IPv4 destination address\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
#endif
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
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_IP_INFO_SRC_IP_ID,
    "src_ip",
    (PARAM_VAL_RULES *)&Oam_api_ipv4_addr_vals[0],
    (sizeof(Oam_api_ipv4_addr_vals) / sizeof(Oam_api_ipv4_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.fw_info.ip_info.src_ip:\r\n"
    "  One of four preconfigured IPv4 source addresses\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
#endif
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
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_TTL_ID,
    "ttl",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.fw_info.mpls_pwe_with_ip_info.ttl:\r\n"
    "  TTL (time to live) profile. One of four preconfigured sets of values\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
#endif
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
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_COS_ID,
    "cos",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.fw_info.mpls_pwe_with_ip_info.cos:\r\n"
    "  Class of service of transmitted packet. Mapped to traffic class and drop\r\n"
    "  precedence in ITMH\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
#endif
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
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_PWE_ID,
    "pwe",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.fw_info.mpls_pwe_with_ip_info.pwe:\r\n"
    "  PWE label to append to the packet. CW 0x7 is automatically appended\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
#endif
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
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_EEP_ID,
    "eep",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.fw_info.mpls_pwe_with_ip_info.eep:\r\n"
    "  A pointer to the encapsulation information that should be appended to the\r\n"
    "  packet by the egress packet processor\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
#endif
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
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_SRC_IP_ID,
    "src_ip",
    (PARAM_VAL_RULES *)&Oam_api_ipv4_addr_vals[0],
    (sizeof(Oam_api_ipv4_addr_vals) / sizeof(Oam_api_ipv4_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.fw_info.mpls_pwe_with_ip_info.src_ip:\r\n"
    "  One of four preconfigured IPv4 source addresses\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
#endif
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
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_INFO_TTL_ID,
    "ttl",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.fw_info.mpls_pwe_info.ttl:\r\n"
    "  TTL (time to live) profile. One of four preconfigured sets of values\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
#endif
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
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_INFO_COS_ID,
    "cos",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.fw_info.mpls_pwe_info.cos:\r\n"
    "  CoS (Class of service) profile. One of eight preconfigured sets of values\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
#endif
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
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_INFO_PWE_ID,
    "pwe",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.fw_info.mpls_pwe_info.pwe:\r\n"
    "  PWE label to append to the packet. CW 0x21 is automatically appended\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
#endif
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
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_INFO_EEP_ID,
    "eep",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.fw_info.mpls_pwe_info.eep:\r\n"
    "  A pointer to the encapsulation information that should be appended to the\r\n"
    "  packet by the egress packet processor\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
#endif
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
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_TUNNEL_INFO_TTL_ID,
    "ttl",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.fw_info.mpls_tunnel_info.ttl:\r\n"
    "  TTL (time to live) profile. One of four preconfigured sets of values\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
#endif
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
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_TUNNEL_INFO_COS_ID,
    "cos",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.fw_info.mpls_tunnel_info.cos:\r\n"
    "  CoS (Class of service) profile. One of eight preconfigured sets of values\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
#endif
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
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_TUNNEL_INFO_EEP_ID,
    "eep",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.fw_info.mpls_tunnel_info.eep:\r\n"
    "  A pointer to the encapsulation information that should be appended to the\r\n"
    "  packet by the egress packet processor\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
#endif
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
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_NG_INFO_MIN_TX_INTERVAL_ID,
    "min_tx_interval",
    (PARAM_VAL_RULES *)&OAM_BFD_TX_RATE_rule[0],
    (sizeof(OAM_BFD_TX_RATE_rule) / sizeof(OAM_BFD_TX_RATE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.ng_info.min_tx_interval:\r\n"
    "  One of eight possible rates. This is the minimum interval, in microseconds\r\n"
    "  that the local system would like to use when transmitting BFD Control packets\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
#endif
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
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_NG_INFO_MIN_RX_INTERVAL_ID,
    "min_rx_interval",
    (PARAM_VAL_RULES *)&OAM_BFD_TX_RATE_rule[0],
    (sizeof(OAM_BFD_TX_RATE_rule) / sizeof(OAM_BFD_TX_RATE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.ng_info.min_rx_interval:\r\n"
    "  One of eight possible rates. This is the minimum interval, in microseconds,\r\n"
    "  between received BFD Control packets that this system is capable of supporting\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
#endif
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
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_NG_INFO_DETECT_MULT_ID,
    "detect_mult",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.ng_info.detect_mult:\r\n"
    "  Detection time multiplier. The negotiated transmit interval, multiplied by\r\n"
    "  this value, provides the Detection Time (timeout) for the BFD Monitoring\r\n"
    "  process\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
#endif
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
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_BFD_TYPE_ID,
    "bfd_type",
    (PARAM_VAL_RULES *)&OAM_BFD_TYPE_rule[0],
    (sizeof(OAM_BFD_TYPE_rule) / sizeof(OAM_BFD_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.bfd_type:\r\n"
    "  One of four BFD packet types\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
#endif
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
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_DISCRIMINATOR_ID,
    "discriminator",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.discriminator:\r\n"
    "  The BFD partner's discriminator\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
#endif
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
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_TX_RATE_ID,
    "tx_rate",
    (PARAM_VAL_RULES *)&OAM_BFD_TX_RATE_rule[0],
    (sizeof(OAM_BFD_TX_RATE_rule) / sizeof(OAM_BFD_TX_RATE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.tx_rate:\r\n"
    "  100/50/20/10/5/2/1 PPS (every 10/20/50/100/200/500/1000 ms)The scheduler\r\n"
    "  scans the table every period. For 16K table and 5ns clock cycle this yields\r\n"
    "  0.5ms accuracy which is 1/20 of the interval\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
#endif
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
    PARAM_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.valid:\r\n"
    "  T - Enable OAMP message generation for this sessionF - Disable OAMP message\r\n"
    "  generation for this session\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_set my_discriminator_ndx 0 ttl 0 cos 0 dst_ip 0 src_ip 0\r\n"
    "  ttl 0 cos 0 pwe 0 eep 0 src_ip 0 ttl 0 cos 0 pwe 0 eep 0 ttl 0 cos 0 eep 0\r\n"
    "  min_tx_interval 0 min_rx_interval 0 detect_mult 0 bfd_type 0 discriminator 0\r\n"
    "  tx_rate 0 valid 0\r\n"
#endif
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
    PARAM_OAM_BFD_TX_INFO_GET_TX_INFO_GET_ID,
    "tx_info_get",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function configures/updates BFD message transmission information.\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_get my_discriminator_ndx 0\r\n"
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
    PARAM_OAM_BFD_TX_INFO_GET_TX_INFO_GET_MY_DISCRIMINATOR_NDX_ID,
    "my_discriminator_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  my_discriminator_ndx:\r\n"
    "  Discriminator index. Must be in accelerated discriminator range (see\r\n"
    "  oam_bfd_my_discriminator_range_set for more information)   \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd tx_info_get my_discriminator_ndx 0\r\n"
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
    PARAM_OAM_BFD_RX_INFO_SET_RX_INFO_SET_ID,
    "rx_info_set",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function configures/updates BFD message reception and session monitoring\r\n"
    "  information.\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd rx_info_set my_discriminator_ndx 0 defect 0 life_time 0\r\n"
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
    PARAM_OAM_BFD_RX_INFO_SET_RX_INFO_SET_MY_DISCRIMINATOR_NDX_ID,
    "my_discriminator_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  my_discriminator_ndx:\r\n"
    "  Discriminator index. Must be in accelerated discriminator range (see\r\n"
    "  oam_bfd_my_discriminator_range_set for more information)   \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd rx_info_set my_discriminator_ndx 0 defect 0 life_time 0\r\n"
#endif
    "",
    "",
    {BIT(29), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_BFD_RX_INFO_SET_RX_INFO_SET_INFO_DEFECT_ID,
    "defect",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.defect:\r\n"
    "  T - All BFD packets on this session are forwarded to the CPU. F - This BFD\r\n"
    "  session is accelerated.\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd rx_info_set my_discriminator_ndx 0 defect 0 life_time 0\r\n"
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
    PARAM_OAM_BFD_RX_INFO_SET_RX_INFO_SET_INFO_LIFE_TIME_ID,
    "life_time",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.life_time:\r\n"
    "  Calculated timeout for BFD session.\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd rx_info_set my_discriminator_ndx 0 defect 0 life_time 0\r\n"
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
    PARAM_OAM_BFD_RX_INFO_GET_RX_INFO_GET_ID,
    "rx_info_get",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function configures/updates BFD message reception and session monitoring\r\n"
    "  information.\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd rx_info_get my_discriminator_ndx 0\r\n"
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
    PARAM_OAM_BFD_RX_INFO_GET_RX_INFO_GET_MY_DISCRIMINATOR_NDX_ID,
    "my_discriminator_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  my_discriminator_ndx:\r\n"
    "  Discriminator index. Must be in accelerated discriminator range (see\r\n"
    "  oam_bfd_my_discriminator_range_set for more information)   \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api bfd rx_info_get my_discriminator_ndx 0\r\n"
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
    PARAM_OAM_API_BFD_ID,
    "bfd",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(19)|BIT(20)|BIT(21)|BIT(22)|BIT(23)|BIT(24)|BIT(25)|BIT(26)|BIT(27)|BIT(28)|BIT(29)|BIT(30), 0, 0, 0},
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
    PARAM_OAM_MPLS_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_ID,
    "cos_mapping_info_set",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function maps 3 bit cos_ndx to a set of class of service attributes.\r\n"
    "  This mapping is used to build MPLS OAM packets\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls cos_mapping_info_set cos_ndx 0 exp 0 dp 0 tc 0\r\n"
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
    PARAM_OAM_MPLS_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_COS_NDX_ID,
    "cos_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  cos_ndx:\r\n"
    "  Class of service. Range: 0-7  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls cos_mapping_info_set cos_ndx 0 exp 0 dp 0 tc 0\r\n"
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
    PARAM_OAM_MPLS_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_EXP_ID,
    "exp",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.exp:\r\n"
    "  May be used as PWE/Tunnel label EXP, depending on the PP egress\r\n"
    "  configuration. Range: 0-7\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls cos_mapping_info_set cos_ndx 0 exp 0 dp 0 tc 0\r\n"
#endif
    "",
    "",
    {BIT(31), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_DP_ID,
    "dp",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.dp:\r\n"
    "  ITMH drop precedence. Range: 0-3\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls cos_mapping_info_set cos_ndx 0 exp 0 dp 0 tc 0\r\n"
#endif
    "",
    "",
    {BIT(31), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_TC_ID,
    "tc",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.tc:\r\n"
    "  ITMH Traffic class. Range: 0-7\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls cos_mapping_info_set cos_ndx 0 exp 0 dp 0 tc 0\r\n"
#endif
    "",
    "",
    {BIT(31), 0, 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_ID,
    "cos_mapping_info_get",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function maps 3 bit cos_ndx to a set of class of service attributes.\r\n"
    "  This mapping is used to build MPLS OAM packets\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls cos_mapping_info_get cos_ndx 0\r\n"
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
    PARAM_OAM_MPLS_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_COS_NDX_ID,
    "cos_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  cos_ndx:\r\n"
    "  Class of service. Range: 0-7  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls cos_mapping_info_get cos_ndx 0\r\n"
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
    PARAM_OAM_MPLS_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_ID,
    "ttl_mapping_info_set",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function maps 2 bit ttl_ndx to TTL information. This mapping is used to\r\n"
    "  build MPLS OAM packets\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls ttl_mapping_info_set ttl_ndx 0 ttl 0\r\n"
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
    PARAM_OAM_MPLS_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_TTL_NDX_ID,
    "ttl_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ttl_ndx:\r\n"
    "  TTL profile identifier. Range: 0-3  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls ttl_mapping_info_set ttl_ndx 0 ttl 0\r\n"
#endif
    "",
    "",
    {0, BIT(1), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_INFO_TTL_ID,
    "ttl",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.ttl:\r\n"
    "  May be used as PWE/Tunnel label TTL, depending on the PP egress\r\n"
    "  configuration. Range: 0-255\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls ttl_mapping_info_set ttl_ndx 0 ttl 0\r\n"
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
    PARAM_OAM_MPLS_TTL_MAPPING_INFO_GET_TTL_MAPPING_INFO_GET_ID,
    "ttl_mapping_info_get",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function maps 2 bit ttl_ndx to TTL information. This mapping is used to\r\n"
    "  build MPLS OAM packets\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls ttl_mapping_info_get ttl_ndx 0\r\n"
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
    PARAM_OAM_MPLS_TTL_MAPPING_INFO_GET_TTL_MAPPING_INFO_GET_TTL_NDX_ID,
    "ttl_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ttl_ndx:\r\n"
    "  TTL profile identifier. Range: 0-3  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls ttl_mapping_info_get ttl_ndx 0\r\n"
#endif
    "",
    "",
    {0, BIT(2), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_ID,
    "ip_mapping_info_set",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function maps 4 bit ip_ndx to IPv4/IPv6 information. This mapping is\r\n"
    "  used to determine the LSR identifier of the constructed MPLS OAM packets\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls ip_mapping_info_set ip_ndx 0 is_ipv6 0\r\n"
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
    PARAM_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_IP_NDX_ID,
    "ip_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ip_ndx:\r\n"
    "  IP profile identifier. Range: 0-15  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls ip_mapping_info_set ip_ndx 0 is_ipv6 0\r\n"
#endif
    "",
    "",
    {0, BIT(3), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_IS_IPV6_ID,
    "is_ipv6",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  is_ipv6:\r\n"
    "  T - The source is IPv6F - The source is IPv4  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls ip_mapping_info_set ip_ndx 0 is_ipv6 0\r\n"
#endif
    "",
    "",
    {0, BIT(3), 0, 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_INFO_IPV6_IP_ID,
    "ipv6",
    (PARAM_VAL_RULES *)&Oam_api_ipv6_addr_vals[0],
    (sizeof(Oam_api_ipv6_addr_vals) / sizeof(Oam_api_ipv6_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.ipv6.ip:\r\n"
    "  IPv6 address. Used as LSR ID when constructing MPLS OAM packet \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls ip_mapping_info_set ip_ndx 0 is_ipv6 0\r\n"
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
    PARAM_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_INFO_IPV4_IP_ID,
    "ipv4",
    (PARAM_VAL_RULES *)&Oam_api_ipv4_addr_vals[0],
    (sizeof(Oam_api_ipv4_addr_vals) / sizeof(Oam_api_ipv4_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.ipv4.ip:\r\n"
    "  IPv4 address. Used as LSR ID when constructing MPLS OAM packet \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls ip_mapping_info_set ip_ndx 0 is_ipv6 0\r\n"
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
    PARAM_OAM_MPLS_IP_MAPPING_INFO_GET_IP_MAPPING_INFO_GET_ID,
    "ip_mapping_info_get",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function maps 4 bit ip_ndx to IPv4/IPv6 information. This mapping is\r\n"
    "  used to determine the LSR identifier of the constructed MPLS OAM packets\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls ip_mapping_info_get ip_ndx 0\r\n"
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
    PARAM_OAM_MPLS_IP_MAPPING_INFO_GET_IP_MAPPING_INFO_GET_IP_NDX_ID,
    "ip_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ip_ndx:\r\n"
    "  IP profile identifier. Range: 0-15  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls ip_mapping_info_get ip_ndx 0\r\n"
#endif
    "",
    "",
    {0, BIT(4), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_ID,
    "label_ranges_set",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function sets label ranges for MPLS OAM. An incoming MPLS OAM packet is\r\n"
    "  accelerated if in_label (the first label after the MPLS OAM label, 14)\r\n"
    "  fulfils one of the following conditions: 1. in_label is between\r\n"
    "  tnl_range.start and tnl_range.end.2. in_label is between pwe_range.start and\r\n"
    "  pwe_range.end. pwe_range.base and tnl_range.base set the physical ranges\r\n"
    "  associated with pwe_range and tnl_range respectively. The physical range is\r\n"
    "  [range.base, range.base + range.end - range.start].if the packet does not\r\n"
    "  fulfill either conditions, it is forwarded to CPU.\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls label_ranges_set end 0 start 0 base 0\r\n"
#endif
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
    PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_TNL_RANGE_ENABLE_ID,
    "tnl_enable",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tnl_range.end:\r\n"
    "  Last label in the range. Range: 0-2^20-1\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls label_ranges_set end 0 start 0 base 0\r\n"
#endif
    "",
    "",
    {0, BIT(5), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_TNL_RANGE_END_ID,
    "tnl_end",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tnl_range.end:\r\n"
    "  Last label in the range. Range: 0-2^20-1\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls label_ranges_set end 0 start 0 base 0\r\n"
#endif
    "",
    "",
    {0, BIT(5), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_TNL_RANGE_START_ID,
    "tnl_start",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tnl_range.start:\r\n"
    "  First label in the range. Range: 0-2^20-1\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls label_ranges_set end 0 start 0 base 0\r\n"
#endif
    "",
    "",
    {0, BIT(5), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_TNL_RANGE_BASE_ID,
    "tnl_base",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tnl_range.base:\r\n"
    "  Base entry in the table. Range: 0-16K\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls label_ranges_set end 0 start 0 base 0\r\n"
#endif
    "",
    "",
    {0, BIT(5), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_PWE_RANGE_ENABLE_ID,
    "pwe_enable",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  pwe_range.end:\r\n"
    "  Last label in the range. Range: 0-2^20-1\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls label_ranges_set end 0 start 0 base 0\r\n"
#endif
    "",
    "",
    {0, BIT(5), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_PWE_RANGE_END_ID,
    "pwe_end",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  pwe_range.end:\r\n"
    "  Last label in the range. Range: 0-2^20-1\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls label_ranges_set end 0 start 0 base 0\r\n"
#endif
    "",
    "",
    {0, BIT(5), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_PWE_RANGE_START_ID,
    "pwe_start",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  pwe_range.start:\r\n"
    "  First label in the range. Range: 0-2^20-1\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls label_ranges_set end 0 start 0 base 0\r\n"
#endif
    "",
    "",
    {0, BIT(5), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_PWE_RANGE_BASE_ID,
    "pwe_base",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  pwe_range.base:\r\n"
    "  Base entry in the table. Range: 0-16K\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls label_ranges_set end 0 start 0 base 0\r\n"
#endif
    "",
    "",
    {0, BIT(5), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_LABEL_RANGES_GET_LABEL_RANGES_GET_ID,
    "label_ranges_get",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function sets label ranges for MPLS OAM. An incoming MPLS OAM packet is\r\n"
    "  accelerated if in_label (the first label after the MPLS OAM label, 14)\r\n"
    "  fulfils one of the following conditions: 1. in_label is between\r\n"
    "  tnl_range.start and tnl_range.end.2. in_label is between pwe_range.start and\r\n"
    "  pwe_range.end. pwe_range.base and tnl_range.base set the physical ranges\r\n"
    "  associated with pwe_range and tnl_range respectively. The physical range is\r\n"
    "  [range.base, range.base + range.end - range.start].if the packet does not\r\n"
    "  fulfill either conditions, it is forwarded to CPU.\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls label_ranges_get\r\n"
#endif
    "",
    "",
    {0, BIT(6), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_CONFIGURABLE_FFD_RATE_SET_ID,
    "configurable_ffd_rate_set",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function sets the configurable FFD rate. Rate is given by the interval\r\n"
    "  (milliseconds) between two consecutive frames. Range: 10 - 1270\r\n"
    "  (denominations of 10ms)\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls configurable_ffd_rate_set rate_ndx 0 interval 0\r\n"
#endif
    "",
    "",
    {0, BIT(7), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_CONFIGURABLE_FFD_RATE_SET_RATE_NDX_ID,
    "rate_ndx",
    (PARAM_VAL_RULES *)&OAM_MPLS_CC_PKT_TYPE_rule[0],
    (sizeof(OAM_MPLS_CC_PKT_TYPE_rule) / sizeof(OAM_MPLS_CC_PKT_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  rate_ndx:\r\n"
    "  The rate index to configure. Currently OAM_MPLS_CC_PKT_TYPE_FFD_CONFIGURABLE\r\n"
    "  is the only configurable message type, thus is also the only allowed index to\r\n"
    "  pass to this function  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls configurable_ffd_rate_set rate_ndx 0 interval 0\r\n"
#endif
    "",
    "",
    {0, BIT(7), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_CONFIGURABLE_FFD_RATE_SET_INTERVAL_ID,
    "interval",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  interval:\r\n"
    "  Interval between two consecutive frames in denomination of 10 ms\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls configurable_ffd_rate_set rate_ndx 0 interval 0\r\n"
#endif
    "",
    "",
    {0, BIT(7), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_CONFIGURABLE_FFD_RATE_GET_CONFIGURABLE_FFD_RATE_GET_ID,
    "configurable_ffd_rate_get",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function sets the configurable FFD rate. Rate is given by the interval\r\n"
    "  (milliseconds) between two consecutive frames. Range: 10 - 1270\r\n"
    "  (denominations of 10ms)\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls configurable_ffd_rate_get rate_ndx 0\r\n"
#endif
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
    PARAM_OAM_MPLS_CONFIGURABLE_FFD_RATE_GET_CONFIGURABLE_FFD_RATE_GET_RATE_NDX_ID,
    "rate_ndx",
    (PARAM_VAL_RULES *)&OAM_MPLS_CC_PKT_TYPE_rule[0],
    (sizeof(OAM_MPLS_CC_PKT_TYPE_rule) / sizeof(OAM_MPLS_CC_PKT_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  rate_ndx:\r\n"
    "  The rate index to configure. Currently OAM_MPLS_CC_PKT_TYPE_FFD_CONFIGURABLE\r\n"
    "  is the only configurable message type, thus is also the only allowed index to\r\n"
    "  pass to this function  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls configurable_ffd_rate_get rate_ndx 0\r\n"
#endif
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
    PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_ID,
    "lsp_tx_info_set",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function configures LSP-source message generator (CV/FFD/BDI/FDI)\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls lsp_tx_info_set mot_ndx 0 fdi_en 0 bdi_en 0 defect_location 0\r\n"
    "  defect_type 0 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0 system_port 0 type 0 valid 0\r\n"
#endif
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
    PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_MOT_NDX_ID,
    "mot_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mot_ndx:\r\n"
    "  Transmitter index. Range: 0-16K  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls lsp_tx_info_set mot_ndx 0 fdi_en 0 bdi_en 0 defect_location 0\r\n"
    "  defect_type 0 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0 system_port 0 type 0 valid 0\r\n"
#endif
    "",
    "",
    {0, BIT(9), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_DEFECT_INFO_FDI_EN_ID,
    "fdi_en",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.defect_info.fdi_en:\r\n"
    "  T - Enable sending of FDI messagesF - Disable sending of FDI messages\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls lsp_tx_info_set mot_ndx 0 fdi_en 0 bdi_en 0 defect_location 0\r\n"
    "  defect_type 0 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0 system_port 0 type 0 valid 0\r\n"
#endif
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
    PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_DEFECT_INFO_BDI_EN_ID,
    "bdi_en",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.defect_info.bdi_en:\r\n"
    "  T - Enable sending of BDI messagesF - Disable sending of BDI messages\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls lsp_tx_info_set mot_ndx 0 fdi_en 0 bdi_en 0 defect_location 0\r\n"
    "  defect_type 0 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0 system_port 0 type 0 valid 0\r\n"
#endif
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
    PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_DEFECT_INFO_DEFECT_LOCATION_ID,
    "defect_location",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.defect_info.defect_location:\r\n"
    "  The identity of the network in which the defect has been detected\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls lsp_tx_info_set mot_ndx 0 fdi_en 0 bdi_en 0 defect_location 0\r\n"
    "  defect_type 0 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0 system_port 0 type 0 valid 0\r\n"
#endif
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
    PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_DEFECT_INFO_DEFECT_TYPE_ID,
    "defect_type",
    (PARAM_VAL_RULES *)&OAM_MPLS_LSP_TX_DEFECT_TYPE_rule[0],
    (sizeof(OAM_MPLS_LSP_TX_DEFECT_TYPE_rule) / sizeof(OAM_MPLS_LSP_TX_DEFECT_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.defect_info.defect_type:\r\n"
    "  One of eight possible defect types as defined\r\n"
    "  in ITU-T Y.1711\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls lsp_tx_info_set mot_ndx 0 fdi_en 0 bdi_en 0 defect_location 0\r\n"
    "  defect_type 0 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0 system_port 0 type 0 valid 0\r\n"
#endif
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
    PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_LSP_ID_ID,
    "lsp_id",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.packet_info.lsp_id:\r\n"
    "  The 16 lsb of the LSP tunnel identifier (16 msb are global)\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls lsp_tx_info_set mot_ndx 0 fdi_en 0 bdi_en 0 defect_location 0\r\n"
    "  defect_type 0 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0 system_port 0 type 0 valid 0\r\n"
#endif
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
    PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_LSR_ID_ID,
    "lsr_id",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.packet_info.lsr_id:\r\n"
    "  One of 16 preconfigured IPv4/6 addresses\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls lsp_tx_info_set mot_ndx 0 fdi_en 0 bdi_en 0 defect_location 0\r\n"
    "  defect_type 0 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0 system_port 0 type 0 valid 0\r\n"
#endif
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
    PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_EEP_ID,
    "eep",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.packet_info.eep:\r\n"
    "  A pointer to the encapsulation information that should be appended to the\r\n"
    "  packet by the egress packet processor\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls lsp_tx_info_set mot_ndx 0 fdi_en 0 bdi_en 0 defect_location 0\r\n"
    "  defect_type 0 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0 system_port 0 type 0 valid 0\r\n"
#endif
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
    PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_TTL_ID,
    "ttl",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.packet_info.ttl:\r\n"
    "  TTL (Time to live) profile. One of four preconfigured sets of values\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls lsp_tx_info_set mot_ndx 0 fdi_en 0 bdi_en 0 defect_location 0\r\n"
    "  defect_type 0 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0 system_port 0 type 0 valid 0\r\n"
#endif
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
    PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_COS_ID,
    "cos",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.packet_info.cos:\r\n"
    "  CoS (Class of service) profile. One of eight preconfigured sets of values\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls lsp_tx_info_set mot_ndx 0 fdi_en 0 bdi_en 0 defect_location 0\r\n"
    "  defect_type 0 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0 system_port 0 type 0 valid 0\r\n"
#endif
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
    PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_SYSTEM_PORT_ID,
    "system_port",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.packet_info.system_port:\r\n"
    "  Destination system port to stamp on the ITMH\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls lsp_tx_info_set mot_ndx 0 fdi_en 0 bdi_en 0 defect_location 0\r\n"
    "  defect_type 0 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0 system_port 0 type 0 valid 0\r\n"
#endif
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
    PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_TYPE_ID,
    "type",
    (PARAM_VAL_RULES *)&OAM_MPLS_CC_PKT_TYPE_rule[0],
    (sizeof(OAM_MPLS_CC_PKT_TYPE_rule) / sizeof(OAM_MPLS_CC_PKT_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.type:\r\n"
    "  One of eight possible preconfigured types \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls lsp_tx_info_set mot_ndx 0 fdi_en 0 bdi_en 0 defect_location 0\r\n"
    "  defect_type 0 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0 system_port 0 type 0 valid 0\r\n"
#endif
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
    PARAM_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.valid:\r\n"
    "  T - Enable connectivity verification transmissionF - Disable connectivity\r\n"
    "  verification transmission\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls lsp_tx_info_set mot_ndx 0 fdi_en 0 bdi_en 0 defect_location 0\r\n"
    "  defect_type 0 lsp_id 0 lsr_id 0 eep 0 ttl 0 cos 0 system_port 0 type 0 valid 0\r\n"
#endif
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
    PARAM_OAM_MPLS_LSP_TX_INFO_GET_LSP_TX_INFO_GET_ID,
    "lsp_tx_info_get",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function configures LSP-source message generator (CV/FFD/BDI/FDI)\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls lsp_tx_info_get mot_ndx 0\r\n"
#endif
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
    PARAM_OAM_MPLS_LSP_TX_INFO_GET_LSP_TX_INFO_GET_MOT_NDX_ID,
    "mot_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mot_ndx:\r\n"
    "  Transmitter index. Range: 0-16K  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls lsp_tx_info_get mot_ndx 0\r\n"
#endif
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
    PARAM_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_ID,
    "lsp_rx_info_set",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function configures LSP-sink \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls lsp_rx_info_set mor_ndx 0 bdi_state 0 fdi_state 0 is_ffd 0 valid\r\n"
    "  0\r\n"
#endif
    "",
    "",
    {0, BIT(11), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_MOR_NDX_ID,
    "mor_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mor_ndx:\r\n"
    "  Offset of the incoming MPLS Label from the base of the MPLS-labels-range as\r\n"
    "  defined in oam_mpls_label_ranges_set(). Range: 0-16K  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls lsp_rx_info_set mor_ndx 0 bdi_state 0 fdi_state 0 is_ffd 0 valid\r\n"
    "  0\r\n"
#endif
    "",
    "",
    {0, BIT(11), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_INFO_IS_FFD_ID,
    "is_ffd",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.is_ffd:\r\n"
    "  T - The connectivity verification message received is expected to be of FFD\r\n"
    "  typeF - The connectivity verification message received is expected to be of\r\n"
    "  CV type\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls lsp_rx_info_set mor_ndx 0 bdi_state 0 fdi_state 0 is_ffd 0 valid\r\n"
    "  0\r\n"
#endif
    "",
    "",
    {0, BIT(11), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_INFO_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.valid:\r\n"
    "  T - Entry is valid. Handles incoming CV/FFC messages F - Entry is invalid.\r\n"
    "  Any incoming CV/FFD message to it is forwarded to the control-plane\r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls lsp_rx_info_set mor_ndx 0 bdi_state 0 fdi_state 0 is_ffd 0 valid\r\n"
    "  0\r\n"
#endif
    "",
    "",
    {0, BIT(11), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_LSP_RX_INFO_GET_LSP_RX_INFO_GET_ID,
    "lsp_rx_info_get",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function configures LSP-sink \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls lsp_rx_info_get mor_ndx 0\r\n"
#endif
    "",
    "",
    {0, BIT(12), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_MPLS_LSP_RX_INFO_GET_LSP_RX_INFO_GET_MOR_NDX_ID,
    "mor_ndx",
    (PARAM_VAL_RULES *)&Oam_api_free_vals[0],
    (sizeof(Oam_api_free_vals) / sizeof(Oam_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mor_ndx:\r\n"
    "  Offset of the incoming MPLS Label from the base of the MPLS-labels-range as\r\n"
    "  defined in oam_mpls_label_ranges_set(). Range: 0-16K  \r\n"
    "",
#if UI_OAM_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  oam_api mpls lsp_rx_info_get mor_ndx 0\r\n"
#endif
    "",
    "",
    {0, BIT(12), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_OAM_API_MPLS_ID,
    "mpls",
    (PARAM_VAL_RULES *)&Oam_api_empty_vals[0],
    (sizeof(Oam_api_empty_vals) / sizeof(Oam_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(31), BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(8)|BIT(9)|BIT(10)|BIT(11)|BIT(12), 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } mpls*/

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

/* } __UI_ROM_DEFI_OAM_API_INCLUDED__*/
#endif
