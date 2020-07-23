/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __UI_ROM_DEFI_T20E_ACC_INCLUDED__
/* { */
#define __UI_ROM_DEFI_T20E_ACC_INCLUDED__

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <appl/dpp/UserInterface/ui_pure_defi_t20e_acc.h>
#include <soc/dpp/T20E/t20e_lem_access.h>
#include <soc/dpp/T20E/t20e_api_hw_if.h>
#include <soc/dpp/T20E/t20e_api_mgmt.h>
#include <soc/dpp/T20E/t20e_api_diagnostics.h>


#define UI_T20E_ACC_PP
#define UI_T20E_ACC_MACT
#define UI_T20E_ACC_OAM
#define UI_T20E_ACC_OAM
#define UI_T20E_REG_ACCESS
#define UI_T20E_HW_IF
#define UI_T20E_MGMT
#define UI_T20E_DIAGNOSTICS

 /*
  * ENUM RULES
  * {
  */
/********************************************************/

EXTERN CONST
   PARAM_VAL_RULES
     t20e_acc_mac_addr_vals[]
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
     t20e_addr_and_data[]
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
     T20E_PP_LEM_TBL_KEY_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "direct",
    {
      {
        T20E_PP_LEM_TBL_KEY_TYPE_DIRECT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_PP_LEM_TBL_KEY_TYPE.T20E_PP_LEM_TBL_KEY_TYPE_DIRECT:\r\n"
        "  *\r\n"
        "  */\r\n"
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
    "ilm",
    {
      {
        T20E_PP_LEM_TBL_KEY_TYPE_ILM,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_PP_LEM_TBL_KEY_TYPE.T20E_PP_LEM_TBL_KEY_TYPE_ILM:\r\n"
        "  T20E_PP_LEM_TBL_KEY_TYPE_ILM\r\n"
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
     T20E_DIAG_MIB_COUNTER_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "ing_good_octets",
    {
      {
        T20E_DIAG_MIB_COUNTER_ING_GOOD_OCTETS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MIB_COUNTER_TYPE.:\r\n"
        "  \r\n"
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
    "ing_good_packets",
    {
      {
        T20E_DIAG_MIB_COUNTER_ING_GOOD_PACKETS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MIB_COUNTER_TYPE.:\r\n"
        "  \r\n"
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
    "ing_frame_errors",
    {
      {
        T20E_DIAG_MIB_COUNTER_ING_FRAME_ERRORS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MIB_COUNTER_TYPE.:\r\n"
        "  \r\n"
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
    "ing_crc_errors",
    {
      {
        T20E_DIAG_MIB_COUNTER_ING_CRC_ERRORS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MIB_COUNTER_TYPE.:\r\n"
        "  \r\n"
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
    "ing_multicast_packets",
    {
      {
        T20E_DIAG_MIB_COUNTER_ING_MULTICAST_PACKETS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MIB_COUNTER_TYPE.:\r\n"
        "  \r\n"
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
    "ing_broadcast_packets",
    {
      {
        T20E_DIAG_MIB_COUNTER_ING_BROADCAST_PACKETS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MIB_COUNTER_TYPE.:\r\n"
        "  \r\n"
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
    "ing_pause_frames",
    {
      {
        T20E_DIAG_MIB_COUNTER_ING_PAUSE_FRAMES,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MIB_COUNTER_TYPE.:\r\n"
        "  \r\n"
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
    "ing_packets_0_63",
    {
      {
        T20E_DIAG_MIB_COUNTER_ING_PACKETS_0_63,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MIB_COUNTER_TYPE.:\r\n"
        "  \r\n"
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
    "ing_packets_64_64",
    {
      {
        T20E_DIAG_MIB_COUNTER_ING_PACKETS_64_64,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MIB_COUNTER_TYPE.:\r\n"
        "  \r\n"
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
    "ing_packets_65_127",
    {
      {
        T20E_DIAG_MIB_COUNTER_ING_PACKETS_65_127,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MIB_COUNTER_TYPE.:\r\n"
        "  \r\n"
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
    "ing_packets_128_255",
    {
      {
        T20E_DIAG_MIB_COUNTER_ING_PACKETS_128_255,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MIB_COUNTER_TYPE.:\r\n"
        "  \r\n"
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
    "ing_packets_256_511",
    {
      {
        T20E_DIAG_MIB_COUNTER_ING_PACKETS_256_511,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MIB_COUNTER_TYPE.:\r\n"
        "  \r\n"
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
    "ing_packets_512_1023",
    {
      {
        T20E_DIAG_MIB_COUNTER_ING_PACKETS_512_1023,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MIB_COUNTER_TYPE.:\r\n"
        "  \r\n"
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
    "ing_packets_1024_1517",
    {
      {
        T20E_DIAG_MIB_COUNTER_ING_PACKETS_1024_1517,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MIB_COUNTER_TYPE.:\r\n"
        "  \r\n"
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
    "ing_packets_1518_max",
    {
      {
        T20E_DIAG_MIB_COUNTER_ING_PACKETS_1518_MAX,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MIB_COUNTER_TYPE.:\r\n"
        "  \r\n"
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
    "egr_good_octets",
    {
      {
        T20E_DIAG_MIB_COUNTER_EGR_GOOD_OCTETS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MIB_COUNTER_TYPE.:\r\n"
        "  \r\n"
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
    "egr_good_packets",
    {
      {
        T20E_DIAG_MIB_COUNTER_EGR_GOOD_PACKETS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MIB_COUNTER_TYPE.:\r\n"
        "  \r\n"
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
    "egr_multicast_packets",
    {
      {
        T20E_DIAG_MIB_COUNTER_EGR_MULTICAST_PACKETS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MIB_COUNTER_TYPE.:\r\n"
        "  \r\n"
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
    "egr_broadcast_packets",
    {
      {
        T20E_DIAG_MIB_COUNTER_EGR_BROADCAST_PACKETS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MIB_COUNTER_TYPE.:\r\n"
        "  \r\n"
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
EXTERN CONST
   PARAM_VAL_RULES
     T20E_ACC_IWRITE_VAL_rule[]
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
/********************************************************/
/********************************************************/
EXTERN CONST
  PARAM_VAL_RULES
    T20E_ACC_SIZE_ID_rule[]
#ifdef INIT
=
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
          3, 1,
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
     T20E_HW_IF_NIF_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "xaui",
    {
      {
        T20E_HW_IF_NIF_TYPE_XAUI,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_HW_IF_NIF_TYPE.T20E_HW_IF_NIF_TYPE_XAUI:\r\n"
        "  XAUI interface\r\n"
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
    "spaui",
    {
      {
        T20E_HW_IF_NIF_TYPE_SPAUI,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_HW_IF_NIF_TYPE.T20E_HW_IF_NIF_TYPE_SPAUI:\r\n"
        "  SPAUI interface\r\n"
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
     T20E_HW_IF_DRAM_SIZE_MBIT_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "288",
    {
      {
        T20E_HW_IF_DRAM_SIZE_MBIT_288,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_HW_IF_DRAM_SIZE_MBIT.T20E_HW_IF_DRAM_SIZE_MBIT_288:\r\n"
        "  DRAM memory size, 288 Mbits.\r\n"
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
    "576",
    {
      {
        T20E_HW_IF_DRAM_SIZE_MBIT_576,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_HW_IF_DRAM_SIZE_MBIT.T20E_HW_IF_DRAM_SIZE_MBIT_576:\r\n"
        "  DRAM memory size, 576 Mbits.\r\n"
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
     T20E_HW_IF_QDR_SIZE_MBIT_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "18",
    {
      {
        T20E_HW_IF_QDR_SIZE_MBIT_18,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_HW_IF_QDR_SIZE_MBIT.T20E_HW_IF_QDR_SIZE_MBIT_18:\r\n"
        "  QDR SRAM memory size, 18 Mbits.\r\n"
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
    "36",
    {
      {
        T20E_HW_IF_QDR_SIZE_MBIT_36,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_HW_IF_QDR_SIZE_MBIT.T20E_HW_IF_QDR_SIZE_MBIT_36:\r\n"
        "  QDR SRAM memory size, 36 Mbits.\r\n"
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
     T20E_MGMT_REVISION_ID_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "1",
    {
      {
        T20E_MGMT_REVISION_ID_1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_MGMT_REVISION_ID.T20E_MGMT_REVISION_ID_1:\r\n"
        "  T20E device revision: 1\r\n"
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
    "nof_ids",
    {
      {
        T20E_MGMT_REVISION_NOF_IDS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_MGMT_REVISION_ID.T20E_MGMT_REVISION_NOF_IDS:\r\n"
        "  Total number of T20E revisions\r\n"
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
     T20E_MGMT_PHASE1_HW_ADJUST_SELECT_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "dram",
    {
      {
        T20E_MGMT_PHASE1_HW_ADJUST_SELECT_DRAM,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_MGMT_PHASE1_HW_ADJUST_SELECT.T20E_MGMT_PHASE1_HW_ADJUST_SELECT_DRAM:\r\n"
        "  Set the DRAM configuration.\r\n"
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
    "qdr",
    {
      {
        T20E_MGMT_PHASE1_HW_ADJUST_SELECT_QDR,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_MGMT_PHASE1_HW_ADJUST_SELECT.T20E_MGMT_PHASE1_HW_ADJUST_SELECT_QDR:\r\n"
        "  Set the QDR configuration.\r\n"
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
    "xaui_0",
    {
      {
        T20E_MGMT_PHASE1_HW_ADJUST_SELECT_XAUI_0,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_MGMT_PHASE1_HW_ADJUST_SELECT.T20E_MGMT_PHASE1_HW_ADJUST_SELECT_XAUI_0:\r\n"
        "  Set the XAUI 0 configuration.\r\n"
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
    "xaui_1",
    {
      {
        T20E_MGMT_PHASE1_HW_ADJUST_SELECT_XAUI_1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_MGMT_PHASE1_HW_ADJUST_SELECT.T20E_MGMT_PHASE1_HW_ADJUST_SELECT_XAUI_1:\r\n"
        "  Set the XAUI 1 configuration.\r\n"
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
    "spaui_0",
    {
      {
        T20E_MGMT_PHASE1_HW_ADJUST_SELECT_SPAUI_0,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_MGMT_PHASE1_HW_ADJUST_SELECT.T20E_MGMT_PHASE1_HW_ADJUST_SELECT_SPAUI_0:\r\n"
        "  Set the SPAUI 0 configuration.\r\n"
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
    "spaui_1",
    {
      {
        T20E_MGMT_PHASE1_HW_ADJUST_SELECT_SPAUI_1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_MGMT_PHASE1_HW_ADJUST_SELECT.T20E_MGMT_PHASE1_HW_ADJUST_SELECT_SPAUI_1:\r\n"
        "  Set the SPAUI 1 configuration.\r\n"
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
    "alla",
    {
      {
        T20E_MGMT_PHASE1_HW_ADJUST_SELECT_ALL,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_MGMT_PHASE1_HW_ADJUST_SELECT.T20E_MGMT_PHASE1_HW_ADJUST_SELECT_ALLa:\r\n"
        "  Set all the Hadrwad adjustment including (CPU, DRAM, QDR, and NIFs).\r\n"
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
     SOC_SAND_PRINT_FLAVORS_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "short",
    {
      {
        SOC_SAND_PRINT_FLAVORS_SHORT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_SAND_PRINT_FLAVORS.SOC_SAND_PRINT_FLAVORS_SHORT:\r\n"
        "  Errors/indications resulted by ingress packet processing.\r\n"
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
    "no_zeros",
    {
      {
        SOC_SAND_PRINT_FLAVORS_NO_ZEROS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_SAND_PRINT_FLAVORS.SOC_SAND_PRINT_FLAVORS_NO_ZEROS:\r\n"
        "  Errors/indications resulted by egress packet processing.\r\n"
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
    "errs_only",
    {
      {
        SOC_SAND_PRINT_FLAVORS_ERRS_ONLY,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  SOC_SAND_PRINT_FLAVORS.SOC_SAND_PRINT_FLAVORS_ERRS_ONLY:\r\n"
        "  General Errors occurred in PP block (mainly FIFO and resources errors)\r\n"
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
     T20E_DIAG_ERR_SELECT_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "packet_ing",
    {
      {
        T20E_DIAG_ERR_SELECT_PACKET_ING,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_ERR_SELECT.T20E_DIAG_ERR_SELECT_PACKET_ING:\r\n"
        "  Errors/indications resulted by ingress packet processing.\r\n"
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
    "packet_eg",
    {
      {
        T20E_DIAG_ERR_SELECT_PACKET_EG,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_ERR_SELECT.T20E_DIAG_ERR_SELECT_PACKET_EG:\r\n"
        "  Errors/indications resulted by egress packet processing.\r\n"
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
    "mact_block",
    {
      {
        T20E_DIAG_ERR_SELECT_MACT_BLOCK,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_ERR_SELECT.T20E_DIAG_ERR_SELECT_MACT_BLOCK:\r\n"
        "  Errors occurred in MACT block (mainly FIFO and resources errors and illegal\r\n"
        "  operation on the MACT DB)\r\n"
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
    "all",
    {
      {
        T20E_DIAG_ERR_SELECT_ALL,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_ERR_SELECT.T20E_DIAG_ERR_SELECT_ALL:\r\n"
        "  All Errors and indications\r\n"
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
     T20E_DIAG_PACKET_ING_ERR_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "not_vlan_mem",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_NOT_VLAN_MEM,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_NOT_VLAN_MEM:\r\n"
        "  Ingress VLAN membership check failed. i.e. Packet's source port is not member\r\n"
        "  in the VLAN the packet associated to\r\n"
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
    "ac_key_tbl_miss",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_AC_KEY_TBL_MISS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_AC_KEY_TBL_MISS:\r\n"
        "  AC key (port, VID, [VID]) lookup failed, key was not found.\r\n"
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
    "ac_key_tbl_invld",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_AC_KEY_TBL_INVLD,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_AC_KEY_TBL_INVLD:\r\n"
        "  AC key (port, VID, [VID]) lookup retrieved an invalid entry.\r\n"
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
    "pwe_tbl_miss",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_PWE_TBL_MISS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_PWE_TBL_MISS:\r\n"
        "  Enter an invalid entry in PWE Table.\r\n"
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
    "ac_tbl_miss",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_AC_TBL_MISS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_AC_TBL_MISS:\r\n"
        "  Enter an invalid entry in PWE Table (According to base and opcode)\r\n"
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
    "mac_p_eth_miss",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_MAC_ETH_MISS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_MAC_P_ETH_MISS:\r\n"
        "  MAC lookup failed in Ethernet forwarding.\r\n"
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
    "mac_p_lsr_miss",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_MAC_LSR_MISS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_MAC_P_LSR_MISS:\r\n"
        "  MAC lookup failed in MPLS LSR forwarding.\r\n"
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
    "mpls1_ttl_0",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_MPLS1_TTL_0,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_MPLS1_TTL_0:\r\n"
        "  MPLS TTL0 was encountered in first label.\r\n"
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
    "mpls1_inv_lbl",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_MPLS1_INV_LBL,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_MPLS1_INV_LBL:\r\n"
        "  Invalid (First) Label was encountered.\r\n"
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
    "mpls1_ip_over_mpls",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_MPLS1_IP_OVER_MPLS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_MPLS1_IP_OVER_MPLS:\r\n"
        "  IP over MPLS label was encountered\r\n"
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
    "mpls2_ttl_0",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_MPLS2_TTL_0,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_MPLS2_TTL_0:\r\n"
        "  MPLS TTL0 was encountered in second label.\r\n"
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
    "mpls2_inv_lbl",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_MPLS2_INV_LBL,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_MPLS2_INV_LBL:\r\n"
        "  Invalid (Second) Label was encountered.\r\n"
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
    "mpls2_ip_over_mpls",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_MPLS2_IP_OVER_MPLS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_MPLS2_IP_OVER_MPLS:\r\n"
        "  IP over MPLS over MPLS was encountered\r\n"
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
    "mpls3_ttl_0",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_MPLS3_TTL_0,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_MPLS3_TTL_0:\r\n"
        "  MPLS TTL0 was encountered in third label.\r\n"
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
    "mpls3_inv_lbl",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_MPLS3_INV_LBL,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_MPLS3_INV_LBL:\r\n"
        "  Invalid (Third) Label was encountered.\r\n"
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
    "mpls3_ip_over_mpls",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_MPLS3_IP_OVER_MPLS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_MPLS3_IP_OVER_MPLS:\r\n"
        "  IP over MPLS over MPLS over MPLS was encountered\r\n"
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
    "my_mac_l3_unknown",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_MY_MAC_L3_UNKNOWN,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_MY_MAC_L3_UNKNOWN:\r\n"
        "  Unknown L3, not MPLS.\r\n"
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
    "my_mac_mpls_dsbl",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_MY_MAC_MPLS_DSBL,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_MY_MAC_MPLS_DSBL:\r\n"
        "  MPLS Routing is not enabled on in-LIF.\r\n"
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
    "pwe_nxt_ptcl_invld",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_PWE_NXT_PTCL_INVLD,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_PWE_NXT_PTCL_INVLD:\r\n"
        "  PWE Next protocol is invalid\r\n"
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
    "mpls_after_pwe_ttl_0",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_MPLS_AFTER_PWE_TTL_0,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_MPLS_AFTER_PWE_TTL_0:\r\n"
        "  TTL of PWE after MPLS is zero.\r\n"
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
    "pwe_eth_no_bos",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_PWE_ETH_NO_BOS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_PWE_ETH_NO_BOS:\r\n"
        "  PWE with no bottom of stack.\r\n"
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
    "stp_block",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_STP_BLOCK,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_STP_BLOCK:\r\n"
        "  STP block state was encountered.\r\n"
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
    "mpls_lsr_ttl_0",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_MPLS_LSR_TTL_0,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_MPLS_LSR_TTL_0:\r\n"
        "  TTL is zero for MPLS Label on LSR.\r\n"
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
    "mpls_lsr_ttl_1",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_MPLS_LSR_TTL_1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_MPLS_LSR_TTL_1:\r\n"
        "  TTL is one for MPLS Label on LSR.\r\n"
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
    "two_ing_counters",
    {
      {
        T20E_DIAG_PACKET_ING_ERR_TWO_ING_COUNTERS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_ING_ERR.T20E_DIAG_PACKET_ING_ERR_TWO_ING_COUNTERS:\r\n"
        "  Packet assigned two counters.\r\n"
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
     T20E_DIAG_PACKET_EG_ERR_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "filter_vsi_membership",
    {
      {
        T20E_DIAG_PACKET_EG_ERR_FILTER_VSI_MEMBERSHIP,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_EG_ERR.T20E_DIAG_PACKET_EG_ERR_FILTER_VSI_MEMBERSHIP:\r\n"
        "  Ingress VSI membership check failed. i.e. Packet's source port is not member\r\n"
        "  in the VSI the packet associated to.\r\n"
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
    "filter_stp_state",
    {
      {
        T20E_DIAG_PACKET_EG_ERR_FILTER_STP_STATE,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_EG_ERR.T20E_DIAG_PACKET_EG_ERR_FILTER_STP_STATE:\r\n"
        "  STP block state was encountered at egress.\r\n"
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
    "filter_pep_acceptable_frames",
    {
      {
        T20E_DIAG_PACKET_EG_ERR_FILTER_PEP_ACCEPTABLE_FRAMES,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_EG_ERR.T20E_DIAG_PACKET_EG_ERR_FILTER_PEP_ACCEPTABLE_FRAMES:\r\n"
        "  Unexpected frame type was encountered on PEP port at egress.\r\n"
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
    "filter_split_horizon",
    {
      {
        T20E_DIAG_PACKET_EG_ERR_FILTER_SPLIT_HORIZON,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_EG_ERR.T20E_DIAG_PACKET_EG_ERR_FILTER_SPLIT_HORIZON:\r\n"
        "  Split-Horizon check failed. i.e. Incoming-Orientation and\r\n"
        "  Outgoing-Orientation are both hub\r\n"
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
    "too_many_counters",
    {
      {
        T20E_DIAG_PACKET_EG_ERR_TOO_MANY_COUNTERS,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_EG_ERR.T20E_DIAG_PACKET_EG_ERR_TOO_MANY_COUNTERS:\r\n"
        "  Too Many counters assigned to Packet.\r\n"
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
    "filter_unkown_da_uc",
    {
      {
        T20E_DIAG_PACKET_EG_ERR_FILTER_UNKOWN_DA_UC,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_EG_ERR.T20E_DIAG_PACKET_EG_ERR_FILTER_UNKOWN_DA_UC:\r\n"
        "  Unknown UC DA was encountered.\r\n"
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
    "filter_unkown_da_mc",
    {
      {
        T20E_DIAG_PACKET_EG_ERR_FILTER_UNKOWN_DA_MC,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_EG_ERR.T20E_DIAG_PACKET_EG_ERR_FILTER_UNKOWN_DA_MC:\r\n"
        "  Unknown MC DA was encountered.\r\n"
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
    "filter_unkown_da_bc",
    {
      {
        T20E_DIAG_PACKET_EG_ERR_FILTER_UNKOWN_DA_BC,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_EG_ERR.T20E_DIAG_PACKET_EG_ERR_FILTER_UNKOWN_DA_BC:\r\n"
        "  Unknown BC DA was encountered\r\n"
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
    "filter_hair_pin",
    {
      {
        T20E_DIAG_PACKET_EG_ERR_FILTER_HAIR_PIN,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_EG_ERR.T20E_DIAG_PACKET_EG_ERR_FILTER_HAIR_PIN:\r\n"
        "  Hair-Pin check Failed, Packets filtered if source interface equals\r\n"
        "  destination interface.\r\n"
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
    "filter_mtu",
    {
      {
        T20E_DIAG_PACKET_EG_ERR_FILTER_MTU,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_EG_ERR.T20E_DIAG_PACKET_EG_ERR_FILTER_MTU:\r\n"
        "  MTU check failed, packet size > Out-PP-Port.MTU.\r\n"
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
    "forwarding_offset_too_big",
    {
      {
        T20E_DIAG_PACKET_EG_ERR_FORWARDING_OFFSET_TOO_BIG,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_PACKET_EG_ERR.T20E_DIAG_PACKET_EG_ERR_FORWARDING_OFFSET_TOO_BIG:\r\n"
        "  Forwarding offset is too Big.\r\n"
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
     T20E_DIAG_MACT_BLOCK_ERR_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "delete_unknown_key",
    {
      {
        T20E_DIAG_MACT_BLOCK_ERR_DELETE_UNKNOWN_KEY,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MACT_BLOCK_ERR.T20E_DIAG_MACT_BLOCK_ERR_DELETE_UNKNOWN_KEY:\r\n"
        "  delete of a non-existing entry was attempted. \r\n"
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
    "reached_entry_limit",
    {
      {
        T20E_DIAG_MACT_BLOCK_ERR_REACHED_ENTRY_LIMIT,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MACT_BLOCK_ERR.T20E_DIAG_MACT_BLOCK_ERR_REACHED_ENTRY_LIMIT:\r\n"
        "  insertion failed as a result of number of entries limit . \r\n"
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
    "inserted_existing",
    {
      {
        T20E_DIAG_MACT_BLOCK_ERR_INSERTED_EXISTING,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MACT_BLOCK_ERR.T20E_DIAG_MACT_BLOCK_ERR_INSERTED_EXISTING:\r\n"
        "  existing entry was inserted. (Same Key) \r\n"
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
    "learn_over_static",
    {
      {
        T20E_DIAG_MACT_BLOCK_ERR_LEARN_OVER_STATIC,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MACT_BLOCK_ERR.T20E_DIAG_MACT_BLOCK_ERR_LEARN_OVER_STATIC:\r\n"
        "  learn over a static entry was attempted (and failed). \r\n"
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
    "refresh_over_static",
    {
      {
        T20E_DIAG_MACT_BLOCK_ERR_REFRESH_OVER_STATIC,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MACT_BLOCK_ERR.T20E_DIAG_MACT_BLOCK_ERR_REFRESH_OVER_STATIC:\r\n"
        "  refresh over a static entry was attempted (and failed). \r\n"
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
    "refresh_non_exist_from_self",
    {
      {
        T20E_DIAG_MACT_BLOCK_ERR_REFRESH_NON_EXIST_FROM_SELF,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MACT_BLOCK_ERR.T20E_DIAG_MACT_BLOCK_ERR_REFRESH_NON_EXIST_FROM_SELF:\r\n"
        "  refresh for a non existing entry was received from this device. Entry was\r\n"
        "  inserted. \r\n"
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
    "dropped_lrf_command",
    {
      {
        T20E_DIAG_MACT_BLOCK_ERR_DROPPED_LRF_COMMAND,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MACT_BLOCK_ERR.T20E_DIAG_MACT_BLOCK_ERR_DROPPED_LRF_COMMAND:\r\n"
        "  MRQ discarded LRF events. \r\n"
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
    "event_ready",
    {
      {
        T20E_DIAG_MACT_BLOCK_ERR_EVENT_READY,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MACT_BLOCK_ERR.T20E_DIAG_MACT_BLOCK_ERR_EVENT_READY:\r\n"
        "  asserts when an event is ready for read \r\n"
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
    "event_fifo_event_drop",
    {
      {
        T20E_DIAG_MACT_BLOCK_ERR_EVENT_FIFO_EVENT_DROP,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MACT_BLOCK_ERR.T20E_DIAG_MACT_BLOCK_ERR_EVENT_FIFO_EVENT_DROP:\r\n"
        "  one of the Event-FIFO drop counters NEQ 0 \r\n"
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
    "event_fifo_high_threshold_reached",
    {
      {
        T20E_DIAG_MACT_BLOCK_ERR_EVENT_FIFO_HIGH_THRESHOLD_REACHED,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MACT_BLOCK_ERR.T20E_DIAG_MACT_BLOCK_ERR_EVENT_FIFO_HIGH_THRESHOLD_R\r\n"
        "  ACHED:\r\n"
        "  Event FIFO reached high threshold \r\n"
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
    "reply_fifo_reply_drop",
    {
      {
        T20E_DIAG_MACT_BLOCK_ERR_REPLY_FIFO_REPLY_DROP,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MACT_BLOCK_ERR.T20E_DIAG_MACT_BLOCK_ERR_REPLY_FIFO_REPLY_DROP:\r\n"
        "  Reply-FIFO drop counters NEQ 0 \r\n"
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
    "diagnostic_command_completed",
    {
      {
        T20E_DIAG_MACT_BLOCK_ERR_DIAGNOSTIC_COMMAND_COMPLETED,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MACT_BLOCK_ERR.T20E_DIAG_MACT_BLOCK_ERR_DIAGNOSTIC_COMMAND_COMPLETE\r\n"
        "  :\r\n"
        "  diagnostics command had completed \r\n"
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
    "amsg_drop",
    {
      {
        T20E_DIAG_MACT_BLOCK_ERR_AMSG_DROP,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MACT_BLOCK_ERR.T20E_DIAG_MACT_BLOCK_ERR_AMSG_DROP:\r\n"
        "  the AMSG FIFO dropped an entry \r\n"
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
    "fmsg_drop",
    {
      {
        T20E_DIAG_MACT_BLOCK_ERR_FMSG_DROP,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MACT_BLOCK_ERR.T20E_DIAG_MACT_BLOCK_ERR_FMSG_DROP:\r\n"
        "  the FMSG FIFO dropped an entry \r\n"
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
    "fcnt_counter_overflow",
    {
      {
        T20E_DIAG_MACT_BLOCK_ERR_FCNT_COUNTER_OVERFLOW,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_MACT_BLOCK_ERR.T20E_DIAG_MACT_BLOCK_ERR_FCNT_COUNTER_OVERFLOW:\r\n"
        "  CID limit counter had overflowed \r\n"
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
     T20E_DIAG_CNT_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "eib_err_pkt_mac0",
    {
      {
        T20E_DIAG_CNT_TYPE_EIB_ERR_PKT_MAC0,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_CNT_TYPE.T20E_DIAG_CNT_TYPE_EIB_ERR_PKT_MAC0:\r\n"
        "  Counter of error packet received by EIB from MAC\r\n"
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
    "eib_err_pkt_mac1",
    {
      {
        T20E_DIAG_CNT_TYPE_EIB_ERR_PKT_MAC1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_CNT_TYPE.T20E_DIAG_CNT_TYPE_EIB_ERR_PKT_MAC1:\r\n"
        "  Counter of error packet received by EIB from MAC\r\n"
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
    "eib_rx_pkt_mac0",
    {
      {
        T20E_DIAG_CNT_TYPE_EIB_RX_PKT_MAC0,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_CNT_TYPE.T20E_DIAG_CNT_TYPE_EIB_RX_PKT_MAC0:\r\n"
        "  Counter of packet received by EIB from MAC\r\n"
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
    "eib_rx_pkt_mac1",
    {
      {
        T20E_DIAG_CNT_TYPE_EIB_RX_PKT_MAC1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_CNT_TYPE.T20E_DIAG_CNT_TYPE_EIB_RX_PKT_MAC1:\r\n"
        "  Counter of packet received by EIB from MAC\r\n"
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
    "eib_rx_oct_mac0",
    {
      {
        T20E_DIAG_CNT_TYPE_EIB_RX_OCT_MAC0,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_CNT_TYPE.T20E_DIAG_CNT_TYPE_EIB_RX_OCT_MAC0:\r\n"
        "  Counter of bytes received by EIB from MAC\r\n"
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
    "eib_rx_oct_mac1",
    {
      {
        T20E_DIAG_CNT_TYPE_EIB_RX_OCT_MAC1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_CNT_TYPE.T20E_DIAG_CNT_TYPE_EIB_RX_OCT_MAC1:\r\n"
        "  Counter of bytes received by EIB from MAC\r\n"
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
    "eib_drop_pkt_mac0",
    {
      {
        T20E_DIAG_CNT_TYPE_EIB_DROP_PKT_MAC0,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_CNT_TYPE.T20E_DIAG_CNT_TYPE_EIB_DROP_PKT_MAC0:\r\n"
        "  Counter of packet dropped by EIB from MAC\r\n"
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
    "eib_drop_pkt_mac1",
    {
      {
        T20E_DIAG_CNT_TYPE_EIB_DROP_PKT_MAC1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_CNT_TYPE.T20E_DIAG_CNT_TYPE_EIB_DROP_PKT_MAC1:\r\n"
        "  Counter of packet dropped by EIB from MAC\r\n"
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
    "iom_pkt_mac0",
    {
      {
        T20E_DIAG_CNT_TYPE_IOM_PKT_MAC0,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_CNT_TYPE.T20E_DIAG_CNT_TYPE_IOM_PKT_MAC0:\r\n"
        "  Counter of packet transmitted to NIF from IOM\r\n"
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
    "iom_pkt_mac1",
    {
      {
        T20E_DIAG_CNT_TYPE_IOM_PKT_MAC1,
          /*
           * Casting added here just to keep the compiler silent.
           */
        (long)""
        "  T20E_DIAG_CNT_TYPE.T20E_DIAG_CNT_TYPE_IOM_PKT_MAC1:\r\n"
        "  Counter of packet transmitted to NIF from IOM\r\n"
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
/********************************************************/
EXTERN CONST
   PARAM_VAL_RULES
     t20e_acc_free_vals[]
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
     t20e_acc_empty_vals[]
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
T20E_ACC_MODULE_ID_rule[]
#ifdef INIT
=
{
  {
    VAL_SYMBOL,
      "pp",
    {
      {
        T20E_PP_ID,
          /*
          * Casting added here just to keep the compiler silent.
          */
          (long)""
          "  T20E_MODULE_ID.T20E_OLP_ID \r\n"
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
      "mact",
    {
      {
        T20E_MACT_ID,
          /*
          * Casting added here just to keep the compiler silent.
          */
          (long)""
          "  T20E_MODULE_ID.T20E_IDR_ID \r\n"
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
   PARAM
     t20e_acc_params[]
#ifdef INIT
   =
{
#ifdef UI_DEFAULT_SECTION/* { default_section*/
#endif /* } default_section*/
#ifdef UI_T20E_ACC_PP/* { pp*/
  {
    PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_ID,
    "ac_access_entry_add_unsafe",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_entry_add_unsafe inner_vid 0 outer_vid 0\r\n"
    "  vlan_domain 0 mp_max_level_valid 0 mp_max_level 0 mep_index_base_valid 0\r\n"
    "  mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0 enable_my_mac 0\r\n"
    "  stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
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
    PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_KEY_INNER_VID_ID,
    "inner_vid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ac_key.inner_vid:\r\n"
    "  uint32 inner_vid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_entry_add_unsafe inner_vid 0 outer_vid 0\r\n"
    "  vlan_domain 0 mp_max_level_valid 0 mp_max_level 0 mep_index_base_valid 0\r\n"
    "  mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0 enable_my_mac 0\r\n"
    "  stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
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
    PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_KEY_OUTER_VID_ID,
    "outer_vid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ac_key.outer_vid:\r\n"
    "  /*\r\n"
    "  *\r\n"
    "  Inner VID. Should be 'T20E_LIF_IGNORE_INTERNAL_VID' when\r\n"
    "  *\r\n"
    "  the vlan_domain refers to ports that do not support VSI\r\n"
    "  *\r\n"
    "  according to 2 VIDs\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_entry_add_unsafe inner_vid 0 outer_vid 0\r\n"
    "  vlan_domain 0 mp_max_level_valid 0 mp_max_level 0 mep_index_base_valid 0\r\n"
    "  mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0 enable_my_mac 0\r\n"
    "  stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
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
    PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_KEY_VLAN_DOMAIN_ID,
    "vlan_domain",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ac_key.vlan_domain:\r\n"
    "  /*\r\n"
    "  *\r\n"
    "  Outer VID. Set to T20E_LIF_IGNORE_OUTER_VID in order to\r\n"
    "  *\r\n"
    "  ignore outer_VID, then also the inner VID will be\r\n"
    "  *\r\n"
    "  ignored and the AC is set according to vlan_domain only.\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_entry_add_unsafe inner_vid 0 outer_vid 0\r\n"
    "  vlan_domain 0 mp_max_level_valid 0 mp_max_level 0 mep_index_base_valid 0\r\n"
    "  mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0 enable_my_mac 0\r\n"
    "  stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
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
    PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_MP_MAX_LEVEL_VALID_ID,
    "mp_max_level_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  payload.mp_max_level_valid:\r\n"
    "  uint32 mp_max_level_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_entry_add_unsafe inner_vid 0 outer_vid 0\r\n"
    "  vlan_domain 0 mp_max_level_valid 0 mp_max_level 0 mep_index_base_valid 0\r\n"
    "  mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0 enable_my_mac 0\r\n"
    "  stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
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
    PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_MP_MAX_LEVEL_ID,
    "mp_max_level",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  payload.mp_max_level:\r\n"
    "  uint32 mp_max_level\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_entry_add_unsafe inner_vid 0 outer_vid 0\r\n"
    "  vlan_domain 0 mp_max_level_valid 0 mp_max_level 0 mep_index_base_valid 0\r\n"
    "  mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0 enable_my_mac 0\r\n"
    "  stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
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
    PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_MEP_INDEX_BASE_VALID_ID,
    "mep_index_base_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  payload.mep_index_base_valid:\r\n"
    "  uint32 mep_index_base_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_entry_add_unsafe inner_vid 0 outer_vid 0\r\n"
    "  vlan_domain 0 mp_max_level_valid 0 mp_max_level 0 mep_index_base_valid 0\r\n"
    "  mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0 enable_my_mac 0\r\n"
    "  stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
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
    PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_MEP_INDEX_BASE_ID,
    "mep_index_base",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  payload.mep_index_base:\r\n"
    "  uint32 mep_index_base\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_entry_add_unsafe inner_vid 0 outer_vid 0\r\n"
    "  vlan_domain 0 mp_max_level_valid 0 mp_max_level 0 mep_index_base_valid 0\r\n"
    "  mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0 enable_my_mac 0\r\n"
    "  stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
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
    PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_ENABLE_ROUTING_MPLS_ID,
    "enable_routing_mpls",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  payload.enable_routing_mpls:\r\n"
    "  uint32 enable_routing_mpls\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_entry_add_unsafe inner_vid 0 outer_vid 0\r\n"
    "  vlan_domain 0 mp_max_level_valid 0 mp_max_level 0 mep_index_base_valid 0\r\n"
    "  mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0 enable_my_mac 0\r\n"
    "  stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
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
    PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_MY_MAC_LSB_ID,
    "my_mac_lsb",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  payload.my_mac_lsb:\r\n"
    "  uint32 my_mac_lsb\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_entry_add_unsafe inner_vid 0 outer_vid 0\r\n"
    "  vlan_domain 0 mp_max_level_valid 0 mp_max_level 0 mep_index_base_valid 0\r\n"
    "  mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0 enable_my_mac 0\r\n"
    "  stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
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
    PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_ENABLE_MY_MAC_ID,
    "enable_my_mac",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  payload.enable_my_mac:\r\n"
    "  uint32 enable_my_mac\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_entry_add_unsafe inner_vid 0 outer_vid 0\r\n"
    "  vlan_domain 0 mp_max_level_valid 0 mp_max_level 0 mep_index_base_valid 0\r\n"
    "  mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0 enable_my_mac 0\r\n"
    "  stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
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
    PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_STP_TOPOLOGY_ID_ID,
    "stp_topology_id",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  payload.stp_topology_id:\r\n"
    "  uint32 stp_topology_id\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_entry_add_unsafe inner_vid 0 outer_vid 0\r\n"
    "  vlan_domain 0 mp_max_level_valid 0 mp_max_level 0 mep_index_base_valid 0\r\n"
    "  mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0 enable_my_mac 0\r\n"
    "  stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
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
    PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_AC_OP_CODE_ID,
    "ac_op_code",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  payload.ac_op_code:\r\n"
    "  uint32 ac_op_code\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_entry_add_unsafe inner_vid 0 outer_vid 0\r\n"
    "  vlan_domain 0 mp_max_level_valid 0 mp_max_level 0 mep_index_base_valid 0\r\n"
    "  mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0 enable_my_mac 0\r\n"
    "  stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
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
    PARAM_T20E_AC_ACCESS_ENTRY_ADD_UNSAFE_AC_ACCESS_ENTRY_ADD_UNSAFE_PAYLOAD_AC_BASE_ID,
    "ac_base",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  payload.ac_base:\r\n"
    "  uint32 ac_base\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_entry_add_unsafe inner_vid 0 outer_vid 0\r\n"
    "  vlan_domain 0 mp_max_level_valid 0 mp_max_level 0 mep_index_base_valid 0\r\n"
    "  mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0 enable_my_mac 0\r\n"
    "  stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
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
    PARAM_T20E_AC_ACCESS_ENTRY_REMOVE_UNSAFE_AC_ACCESS_ENTRY_REMOVE_UNSAFE_ID,
    "ac_access_entry_remove_unsafe",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_entry_remove_unsafe inner_vid 0 outer_vid 0\r\n"
    "  vlan_domain 0\r\n"
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
    PARAM_T20E_AC_ACCESS_ENTRY_REMOVE_UNSAFE_AC_ACCESS_ENTRY_REMOVE_UNSAFE_AC_KEY_INNER_VID_ID,
    "inner_vid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ac_key.inner_vid:\r\n"
    "  uint32 inner_vid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_entry_remove_unsafe inner_vid 0 outer_vid 0\r\n"
    "  vlan_domain 0\r\n"
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
    PARAM_T20E_AC_ACCESS_ENTRY_REMOVE_UNSAFE_AC_ACCESS_ENTRY_REMOVE_UNSAFE_AC_KEY_OUTER_VID_ID,
    "outer_vid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ac_key.outer_vid:\r\n"
    "  /*\r\n"
    "  *\r\n"
    "  Inner VID. Should be 'T20E_LIF_IGNORE_INTERNAL_VID' when\r\n"
    "  *\r\n"
    "  the vlan_domain refers to ports that do not support VSI\r\n"
    "  *\r\n"
    "  according to 2 VIDs\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_entry_remove_unsafe inner_vid 0 outer_vid 0\r\n"
    "  vlan_domain 0\r\n"
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
    PARAM_T20E_AC_ACCESS_ENTRY_REMOVE_UNSAFE_AC_ACCESS_ENTRY_REMOVE_UNSAFE_AC_KEY_VLAN_DOMAIN_ID,
    "vlan_domain",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ac_key.vlan_domain:\r\n"
    "  /*\r\n"
    "  *\r\n"
    "  Outer VID. Set to T20E_LIF_IGNORE_OUTER_VID in order to\r\n"
    "  *\r\n"
    "  ignore outer_VID, then also the inner VID will be\r\n"
    "  *\r\n"
    "  ignored and the AC is set according to vlan_domain only.\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_entry_remove_unsafe inner_vid 0 outer_vid 0\r\n"
    "  vlan_domain 0\r\n"
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
    PARAM_T20E_AC_ACCESS_TBL_CLEAR_UNSAFE_AC_ACCESS_TBL_CLEAR_UNSAFE_ID,
    "ac_access_tbl_clear_unsafe",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_tbl_clear_unsafe\r\n"
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
    PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_ID,
    "ac_access_internal_entry_add_unsafe",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_internal_entry_add_unsafe inner_vid 0\r\n"
    "  outer_vid 0 vlan_domain 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mep_index_base_valid 0 mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0\r\n"
    "  enable_my_mac 0 stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
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
    PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_KEY_INNER_VID_ID,
    "inner_vid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ac_key.inner_vid:\r\n"
    "  uint32 inner_vid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_internal_entry_add_unsafe inner_vid 0\r\n"
    "  outer_vid 0 vlan_domain 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mep_index_base_valid 0 mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0\r\n"
    "  enable_my_mac 0 stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
#endif
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
    PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_KEY_OUTER_VID_ID,
    "outer_vid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ac_key.outer_vid:\r\n"
    "  /*\r\n"
    "  *\r\n"
    "  Inner VID. Should be 'T20E_LIF_IGNORE_INTERNAL_VID' when\r\n"
    "  *\r\n"
    "  the vlan_domain refers to ports that do not support VSI\r\n"
    "  *\r\n"
    "  according to 2 VIDs\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_internal_entry_add_unsafe inner_vid 0\r\n"
    "  outer_vid 0 vlan_domain 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mep_index_base_valid 0 mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0\r\n"
    "  enable_my_mac 0 stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
#endif
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
    PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_KEY_VLAN_DOMAIN_ID,
    "vlan_domain",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ac_key.vlan_domain:\r\n"
    "  /*\r\n"
    "  *\r\n"
    "  Outer VID. Set to T20E_LIF_IGNORE_OUTER_VID in order to\r\n"
    "  *\r\n"
    "  ignore outer_VID, then also the inner VID will be\r\n"
    "  *\r\n"
    "  ignored and the AC is set according to vlan_domain only.\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_internal_entry_add_unsafe inner_vid 0\r\n"
    "  outer_vid 0 vlan_domain 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mep_index_base_valid 0 mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0\r\n"
    "  enable_my_mac 0 stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
#endif
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
    PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_MP_MAX_LEVEL_VALID_ID,
    "mp_max_level_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  payload.mp_max_level_valid:\r\n"
    "  uint32 mp_max_level_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_internal_entry_add_unsafe inner_vid 0\r\n"
    "  outer_vid 0 vlan_domain 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mep_index_base_valid 0 mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0\r\n"
    "  enable_my_mac 0 stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
#endif
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
    PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_MP_MAX_LEVEL_ID,
    "mp_max_level",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  payload.mp_max_level:\r\n"
    "  uint32 mp_max_level\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_internal_entry_add_unsafe inner_vid 0\r\n"
    "  outer_vid 0 vlan_domain 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mep_index_base_valid 0 mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0\r\n"
    "  enable_my_mac 0 stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
#endif
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
    PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_MEP_INDEX_BASE_VALID_ID,
    "mep_index_base_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  payload.mep_index_base_valid:\r\n"
    "  uint32 mep_index_base_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_internal_entry_add_unsafe inner_vid 0\r\n"
    "  outer_vid 0 vlan_domain 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mep_index_base_valid 0 mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0\r\n"
    "  enable_my_mac 0 stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
#endif
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
    PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_MEP_INDEX_BASE_ID,
    "mep_index_base",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  payload.mep_index_base:\r\n"
    "  uint32 mep_index_base\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_internal_entry_add_unsafe inner_vid 0\r\n"
    "  outer_vid 0 vlan_domain 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mep_index_base_valid 0 mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0\r\n"
    "  enable_my_mac 0 stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
#endif
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
    PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_ENABLE_ROUTING_MPLS_ID,
    "enable_routing_mpls",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  payload.enable_routing_mpls:\r\n"
    "  uint32 enable_routing_mpls\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_internal_entry_add_unsafe inner_vid 0\r\n"
    "  outer_vid 0 vlan_domain 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mep_index_base_valid 0 mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0\r\n"
    "  enable_my_mac 0 stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
#endif
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
    PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_MY_MAC_LSB_ID,
    "my_mac_lsb",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  payload.my_mac_lsb:\r\n"
    "  uint32 my_mac_lsb\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_internal_entry_add_unsafe inner_vid 0\r\n"
    "  outer_vid 0 vlan_domain 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mep_index_base_valid 0 mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0\r\n"
    "  enable_my_mac 0 stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
#endif
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
    PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_ENABLE_MY_MAC_ID,
    "enable_my_mac",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  payload.enable_my_mac:\r\n"
    "  uint32 enable_my_mac\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_internal_entry_add_unsafe inner_vid 0\r\n"
    "  outer_vid 0 vlan_domain 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mep_index_base_valid 0 mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0\r\n"
    "  enable_my_mac 0 stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
#endif
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
    PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_STP_TOPOLOGY_ID_ID,
    "stp_topology_id",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  payload.stp_topology_id:\r\n"
    "  uint32 stp_topology_id\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_internal_entry_add_unsafe inner_vid 0\r\n"
    "  outer_vid 0 vlan_domain 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mep_index_base_valid 0 mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0\r\n"
    "  enable_my_mac 0 stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
#endif
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
    PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_AC_OP_CODE_ID,
    "ac_op_code",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  payload.ac_op_code:\r\n"
    "  uint32 ac_op_code\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_internal_entry_add_unsafe inner_vid 0\r\n"
    "  outer_vid 0 vlan_domain 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mep_index_base_valid 0 mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0\r\n"
    "  enable_my_mac 0 stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
#endif
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
    PARAM_T20E_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_AC_ACCESS_INTERNAL_ENTRY_ADD_UNSAFE_PAYLOAD_AC_BASE_ID,
    "ac_base",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  payload.ac_base:\r\n"
    "  uint32 ac_base\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_internal_entry_add_unsafe inner_vid 0\r\n"
    "  outer_vid 0 vlan_domain 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mep_index_base_valid 0 mep_index_base 0 enable_routing_mpls 0 my_mac_lsb 0\r\n"
    "  enable_my_mac 0 stp_topology_id 0 ac_op_code 0 ac_base 0\r\n"
#endif
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
    PARAM_T20E_AC_ACCESS_GET_BLOCK_UNSAFE_AC_ACCESS_GET_BLOCK_UNSAFE_ID,
    "ac_access_get_block_unsafe",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_get_block_unsafe\r\n"
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
    PARAM_T20E_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_ID,
    "ac_access_hw_set_callback_unsafe",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_hw_set_callback_unsafe tbl_ndx 0\r\n"
    "  entry 0 payload 0 verifier 0\r\n"
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
    PARAM_T20E_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_TBL_NDX_ID,
    "tbl_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  tbl_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_hw_set_callback_unsafe tbl_ndx 0\r\n"
    "  entry 0 payload 0 verifier 0\r\n"
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
    PARAM_T20E_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_hw_set_callback_unsafe tbl_ndx 0\r\n"
    "  entry 0 payload 0 verifier 0\r\n"
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
    PARAM_T20E_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_PAYLOAD_ID,
    "payload",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  payload:\r\n"
    "  SOC_SAND_IN\r\n"
    "  T20E_EXACT_MATCH_PAYLOAD\r\n"
    "  payload\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_hw_set_callback_unsafe tbl_ndx 0\r\n"
    "  entry 0 payload 0 verifier 0\r\n"
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
    PARAM_T20E_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_AC_ACCESS_HW_SET_CALLBACK_UNSAFE_VERIFIER_ID,
    "verifier",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  verifier:\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_hw_set_callback_unsafe tbl_ndx 0\r\n"
    "  entry 0 payload 0 verifier 0\r\n"
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
    PARAM_T20E_AC_ACCESS_HW_GET_CALLBACK_UNSAFE_AC_ACCESS_HW_GET_CALLBACK_UNSAFE_ID,
    "ac_access_hw_get_callback_unsafe",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_hw_get_callback_unsafe tbl_ndx 0\r\n"
    "  entry 0\r\n"
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
    PARAM_T20E_AC_ACCESS_HW_GET_CALLBACK_UNSAFE_AC_ACCESS_HW_GET_CALLBACK_UNSAFE_TBL_NDX_ID,
    "tbl_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  tbl_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_hw_get_callback_unsafe tbl_ndx 0\r\n"
    "  entry 0\r\n"
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
    PARAM_T20E_AC_ACCESS_HW_GET_CALLBACK_UNSAFE_AC_ACCESS_HW_GET_CALLBACK_UNSAFE_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp ac_access_hw_get_callback_unsafe tbl_ndx 0\r\n"
    "  entry 0\r\n"
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
    PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_ID,
    "lem_da_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_da_tbl_set inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /* is_dynamic 0 destination 0 asd 0 valid 0\r\n"
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
    PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_ENTRY_KEY_INFO_ILM_INRIF_ID,
    "inrif",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_key.info.ilm.inrif:\r\n"
    "  uint32 inrif\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_da_tbl_set inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /* is_dynamic 0 destination 0 asd 0 valid 0\r\n"
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
    PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_ENTRY_KEY_INFO_ILM_IN_LOCAL_PORT_ID,
    "in_local_port",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_key.info.ilm.in_local_port:\r\n"
    "  /*\r\n"
    "  *\r\n"
    "  The incoming interface the packet associated with. Note:\r\n"
    "  *\r\n"
    "  If by the global setting soc_ppc_frwrd_ilm_glbl_info_set,\r\n"
    "  *\r\n"
    "  the in-RIF is masked, then this value has to be\r\n"
    "  *\r\n"
    "  zero.Always masked in T20E.\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_da_tbl_set inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /* is_dynamic 0 destination 0 asd 0 valid 0\r\n"
#endif
    "",
    "",
    {BIT(7), 0, 0, 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_ENTRY_KEY_INFO_ILM_MAPPED_EXP_ID,
    "mapped_exp",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_key.info.ilm.mapped_exp:\r\n"
    "  /*\r\n"
    "  *\r\n"
    "  The local port the packet enters from. Note: If by the\r\n"
    "  *\r\n"
    "  global setting soc_ppc_frwrd_ilm_glbl_info_set, the port is\r\n"
    "  *\r\n"
    "  masked, then this value has to be zero.\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_da_tbl_set inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /* is_dynamic 0 destination 0 asd 0 valid 0\r\n"
#endif
    "",
    "",
    {BIT(7), 0, 0, 0},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_ENTRY_KEY_INFO_ILM_IN_LABEL_ID,
    "in_label",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_key.info.ilm.in_label:\r\n"
    "  /*\r\n"
    "  *\r\n"
    "  Internal EXP. Relevant only for labels in the ELSP\r\n"
    "  *\r\n"
    "  range. In this case, this is the value after mapping the\r\n"
    "  *\r\n"
    "  header EXP with 'exp_map_tbl table'. Otherwise, this is\r\n"
    "  *\r\n"
    "  ignored (set to zero)\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_da_tbl_set inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /* is_dynamic 0 destination 0 asd 0 valid 0\r\n"
#endif
    "",
    "",
    {BIT(7), 0, 0, 0},
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_ENTRY_KEY_INFO_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_key.info.offset:\r\n"
    "  /*\r\n"
    "  * Used to access to the table as ILM\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_da_tbl_set inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /* is_dynamic 0 destination 0 asd 0 valid 0\r\n"
#endif
    "",
    "",
    {BIT(7), 0, 0, 0},
    7,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_ENTRY_KEY_TYPE_ID,
    "type",
    (PARAM_VAL_RULES *)&T20E_PP_LEM_TBL_KEY_TYPE_rule[0],
    (sizeof(T20E_PP_LEM_TBL_KEY_TYPE_rule) / sizeof(T20E_PP_LEM_TBL_KEY_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_key.type:\r\n"
    "  /*\r\n"
    "  *\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_da_tbl_set inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /* is_dynamic 0 destination 0 asd 0 valid 0\r\n"
#endif
    "",
    "",
    {BIT(7), 0, 0, 0},
    8,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_TBL_DATA_IS_DYNAMIC_ID,
    "is_dynamic",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.is_dynamic:\r\n"
    "  uint32 is_dynamic\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_da_tbl_set inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /* is_dynamic 0 destination 0 asd 0 valid 0\r\n"
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
    PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_TBL_DATA_DESTINATION_ID,
    "destination",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.destination:\r\n"
    "  uint32 destination\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_da_tbl_set inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /* is_dynamic 0 destination 0 asd 0 valid 0\r\n"
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
    PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_TBL_DATA_ASD_ID,
    "asd",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.asd:\r\n"
    "  uint32 asd\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_da_tbl_set inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /* is_dynamic 0 destination 0 asd 0 valid 0\r\n"
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
    PARAM_T20E_PP_LEM_DA_TBL_SET_PP_LEM_DA_TBL_SET_TBL_DATA_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.valid:\r\n"
    "  uint32 valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_da_tbl_set inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /* is_dynamic 0 destination 0 asd 0 valid 0\r\n"
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
    PARAM_T20E_PP_LEM_DA_TBL_GET_PP_LEM_DA_TBL_GET_ID,
    "lem_da_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_da_tbl_get inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /*\r\n"
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
    PARAM_T20E_PP_LEM_DA_TBL_GET_PP_LEM_DA_TBL_GET_ENTRY_KEY_INFO_ILM_INRIF_ID,
    "inrif",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_key.info.ilm.inrif:\r\n"
    "  uint32 inrif\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_da_tbl_get inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /*\r\n"
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
    PARAM_T20E_PP_LEM_DA_TBL_GET_PP_LEM_DA_TBL_GET_ENTRY_KEY_INFO_ILM_IN_LOCAL_PORT_ID,
    "in_local_port",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_key.info.ilm.in_local_port:\r\n"
    "  /*\r\n"
    "  *\r\n"
    "  The incoming interface the packet associated with. Note:\r\n"
    "  *\r\n"
    "  If by the global setting soc_ppc_frwrd_ilm_glbl_info_set,\r\n"
    "  *\r\n"
    "  the in-RIF is masked, then this value has to be\r\n"
    "  *\r\n"
    "  zero.Always masked in T20E.\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_da_tbl_get inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /*\r\n"
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
    PARAM_T20E_PP_LEM_DA_TBL_GET_PP_LEM_DA_TBL_GET_ENTRY_KEY_INFO_ILM_MAPPED_EXP_ID,
    "mapped_exp",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_key.info.ilm.mapped_exp:\r\n"
    "  /*\r\n"
    "  *\r\n"
    "  The local port the packet enters from. Note: If by the\r\n"
    "  *\r\n"
    "  global setting soc_ppc_frwrd_ilm_glbl_info_set, the port is\r\n"
    "  *\r\n"
    "  masked, then this value has to be zero.\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_da_tbl_get inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /*\r\n"
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
    PARAM_T20E_PP_LEM_DA_TBL_GET_PP_LEM_DA_TBL_GET_ENTRY_KEY_INFO_ILM_IN_LABEL_ID,
    "in_label",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_key.info.ilm.in_label:\r\n"
    "  /*\r\n"
    "  *\r\n"
    "  Internal EXP. Relevant only for labels in the ELSP\r\n"
    "  *\r\n"
    "  range. In this case, this is the value after mapping the\r\n"
    "  *\r\n"
    "  header EXP with 'exp_map_tbl table'. Otherwise, this is\r\n"
    "  *\r\n"
    "  ignored (set to zero)\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_da_tbl_get inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /*\r\n"
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
    PARAM_T20E_PP_LEM_DA_TBL_GET_PP_LEM_DA_TBL_GET_ENTRY_KEY_INFO_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_key.info.offset:\r\n"
    "  /*\r\n"
    "  * Used to access to the table as ILM\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_da_tbl_get inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /*\r\n"
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
    PARAM_T20E_PP_LEM_DA_TBL_GET_PP_LEM_DA_TBL_GET_ENTRY_KEY_TYPE_ID,
    "type",
    (PARAM_VAL_RULES *)&T20E_PP_LEM_TBL_KEY_TYPE_rule[0],
    (sizeof(T20E_PP_LEM_TBL_KEY_TYPE_rule) / sizeof(T20E_PP_LEM_TBL_KEY_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_key.type:\r\n"
    "  /*\r\n"
    "  *\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_da_tbl_get inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /*\r\n"
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
    PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_ID,
    "lem_sa_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_sa_tbl_set inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /* is_dynamic 0 destination 0 asd 0 valid 0\r\n"
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
    PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_ENTRY_KEY_INFO_ILM_INRIF_ID,
    "inrif",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_key.info.ilm.inrif:\r\n"
    "  uint32 inrif\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_sa_tbl_set inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /* is_dynamic 0 destination 0 asd 0 valid 0\r\n"
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
    PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_ENTRY_KEY_INFO_ILM_IN_LOCAL_PORT_ID,
    "in_local_port",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_key.info.ilm.in_local_port:\r\n"
    "  /*\r\n"
    "  *\r\n"
    "  The incoming interface the packet associated with. Note:\r\n"
    "  *\r\n"
    "  If by the global setting soc_ppc_frwrd_ilm_glbl_info_set,\r\n"
    "  *\r\n"
    "  the in-RIF is masked, then this value has to be\r\n"
    "  *\r\n"
    "  zero.Always masked in T20E.\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_sa_tbl_set inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /* is_dynamic 0 destination 0 asd 0 valid 0\r\n"
#endif
    "",
    "",
    {BIT(9), 0, 0, 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_ENTRY_KEY_INFO_ILM_MAPPED_EXP_ID,
    "mapped_exp",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_key.info.ilm.mapped_exp:\r\n"
    "  /*\r\n"
    "  *\r\n"
    "  The local port the packet enters from. Note: If by the\r\n"
    "  *\r\n"
    "  global setting soc_ppc_frwrd_ilm_glbl_info_set, the port is\r\n"
    "  *\r\n"
    "  masked, then this value has to be zero.\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_sa_tbl_set inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /* is_dynamic 0 destination 0 asd 0 valid 0\r\n"
#endif
    "",
    "",
    {BIT(9), 0, 0, 0},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_ENTRY_KEY_INFO_ILM_IN_LABEL_ID,
    "in_label",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_key.info.ilm.in_label:\r\n"
    "  /*\r\n"
    "  *\r\n"
    "  Internal EXP. Relevant only for labels in the ELSP\r\n"
    "  *\r\n"
    "  range. In this case, this is the value after mapping the\r\n"
    "  *\r\n"
    "  header EXP with 'exp_map_tbl table'. Otherwise, this is\r\n"
    "  *\r\n"
    "  ignored (set to zero)\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_sa_tbl_set inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /* is_dynamic 0 destination 0 asd 0 valid 0\r\n"
#endif
    "",
    "",
    {BIT(9), 0, 0, 0},
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_ENTRY_KEY_INFO_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_key.info.offset:\r\n"
    "  /*\r\n"
    "  * Used to access to the table as ILM\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_sa_tbl_set inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /* is_dynamic 0 destination 0 asd 0 valid 0\r\n"
#endif
    "",
    "",
    {BIT(9), 0, 0, 0},
    7,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_ENTRY_KEY_TYPE_ID,
    "type",
    (PARAM_VAL_RULES *)&T20E_PP_LEM_TBL_KEY_TYPE_rule[0],
    (sizeof(T20E_PP_LEM_TBL_KEY_TYPE_rule) / sizeof(T20E_PP_LEM_TBL_KEY_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_key.type:\r\n"
    "  /*\r\n"
    "  *\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_sa_tbl_set inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /* is_dynamic 0 destination 0 asd 0 valid 0\r\n"
#endif
    "",
    "",
    {BIT(9), 0, 0, 0},
    8,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_TBL_DATA_IS_DYNAMIC_ID,
    "is_dynamic",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.is_dynamic:\r\n"
    "  uint32 is_dynamic\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_sa_tbl_set inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /* is_dynamic 0 destination 0 asd 0 valid 0\r\n"
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
    PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_TBL_DATA_DESTINATION_ID,
    "destination",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.destination:\r\n"
    "  uint32 destination\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_sa_tbl_set inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /* is_dynamic 0 destination 0 asd 0 valid 0\r\n"
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
    PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_TBL_DATA_ASD_ID,
    "asd",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.asd:\r\n"
    "  uint32 asd\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_sa_tbl_set inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /* is_dynamic 0 destination 0 asd 0 valid 0\r\n"
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
    PARAM_T20E_PP_LEM_SA_TBL_SET_PP_LEM_SA_TBL_SET_TBL_DATA_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.valid:\r\n"
    "  uint32 valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_sa_tbl_set inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /* is_dynamic 0 destination 0 asd 0 valid 0\r\n"
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
    PARAM_T20E_PP_LEM_SA_TBL_GET_PP_LEM_SA_TBL_GET_ID,
    "lem_sa_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_sa_tbl_get inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /*\r\n"
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
    PARAM_T20E_PP_LEM_SA_TBL_GET_PP_LEM_SA_TBL_GET_ENTRY_KEY_INFO_ILM_INRIF_ID,
    "inrif",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_key.info.ilm.inrif:\r\n"
    "  uint32 inrif\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_sa_tbl_get inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /*\r\n"
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
    PARAM_T20E_PP_LEM_SA_TBL_GET_PP_LEM_SA_TBL_GET_ENTRY_KEY_INFO_ILM_IN_LOCAL_PORT_ID,
    "in_local_port",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_key.info.ilm.in_local_port:\r\n"
    "  /*\r\n"
    "  *\r\n"
    "  The incoming interface the packet associated with. Note:\r\n"
    "  *\r\n"
    "  If by the global setting soc_ppc_frwrd_ilm_glbl_info_set,\r\n"
    "  *\r\n"
    "  the in-RIF is masked, then this value has to be\r\n"
    "  *\r\n"
    "  zero.Always masked in T20E.\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_sa_tbl_get inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /*\r\n"
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
    PARAM_T20E_PP_LEM_SA_TBL_GET_PP_LEM_SA_TBL_GET_ENTRY_KEY_INFO_ILM_MAPPED_EXP_ID,
    "mapped_exp",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_key.info.ilm.mapped_exp:\r\n"
    "  /*\r\n"
    "  *\r\n"
    "  The local port the packet enters from. Note: If by the\r\n"
    "  *\r\n"
    "  global setting soc_ppc_frwrd_ilm_glbl_info_set, the port is\r\n"
    "  *\r\n"
    "  masked, then this value has to be zero.\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_sa_tbl_get inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /*\r\n"
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
    PARAM_T20E_PP_LEM_SA_TBL_GET_PP_LEM_SA_TBL_GET_ENTRY_KEY_INFO_ILM_IN_LABEL_ID,
    "in_label",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_key.info.ilm.in_label:\r\n"
    "  /*\r\n"
    "  *\r\n"
    "  Internal EXP. Relevant only for labels in the ELSP\r\n"
    "  *\r\n"
    "  range. In this case, this is the value after mapping the\r\n"
    "  *\r\n"
    "  header EXP with 'exp_map_tbl table'. Otherwise, this is\r\n"
    "  *\r\n"
    "  ignored (set to zero)\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_sa_tbl_get inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /*\r\n"
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
    PARAM_T20E_PP_LEM_SA_TBL_GET_PP_LEM_SA_TBL_GET_ENTRY_KEY_INFO_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_key.info.offset:\r\n"
    "  /*\r\n"
    "  * Used to access to the table as ILM\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_sa_tbl_get inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /*\r\n"
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
    PARAM_T20E_PP_LEM_SA_TBL_GET_PP_LEM_SA_TBL_GET_ENTRY_KEY_TYPE_ID,
    "type",
    (PARAM_VAL_RULES *)&T20E_PP_LEM_TBL_KEY_TYPE_rule[0],
    (sizeof(T20E_PP_LEM_TBL_KEY_TYPE_rule) / sizeof(T20E_PP_LEM_TBL_KEY_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry_key.type:\r\n"
    "  /*\r\n"
    "  *\r\n"
    "  */\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_lem_sa_tbl_get inrif 0 in_local_port 0 mapped_exp 0\r\n"
    "  in_label 0 offset 0 type /*\r\n"
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
    PARAM_T20E_PP_SET_REPS_FOR_TBL_UNSAFE_PP_SET_REPS_FOR_TBL_UNSAFE_ID,
    "set_reps_for_tbl_unsafe",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_set_reps_for_tbl_unsafe nof_reps 0\r\n"
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
    PARAM_T20E_PP_SET_REPS_FOR_TBL_UNSAFE_PP_SET_REPS_FOR_TBL_UNSAFE_NOF_REPS_ID,
    "nof_reps",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  nof_reps:\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_set_reps_for_tbl_unsafe nof_reps 0\r\n"
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
    PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_ID,
    "llvp_classification_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_llvp_classification_tbl_set port_profile_ndx 0\r\n"
    "  out_tag_encap_ndx 0 out_is_prio_ndx 0 in_tag_encap_ndx 0 use_two_vlans 0\r\n"
    "  llvp_inner_comp_index 0 llvp_outer_comp_index 0 tag_edit_profile 0\r\n"
    "  acceptable_frame_type_action 0 incoming_s_tag_exist 0 incoming_tag_exist 0\r\n"
    "  incoming_vid_exist 0\r\n"
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
    PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_PORT_PROFILE_NDX_ID,
    "port_profile_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_profile_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  port_profile_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_llvp_classification_tbl_set port_profile_ndx 0\r\n"
    "  out_tag_encap_ndx 0 out_is_prio_ndx 0 in_tag_encap_ndx 0 use_two_vlans 0\r\n"
    "  llvp_inner_comp_index 0 llvp_outer_comp_index 0 tag_edit_profile 0\r\n"
    "  acceptable_frame_type_action 0 incoming_s_tag_exist 0 incoming_tag_exist 0\r\n"
    "  incoming_vid_exist 0\r\n"
#endif
    "",
    "",
    {BIT(12), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_OUT_TAG_ENCAP_NDX_ID,
    "out_tag_encap_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  out_tag_encap_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  out_tag_encap_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_llvp_classification_tbl_set port_profile_ndx 0\r\n"
    "  out_tag_encap_ndx 0 out_is_prio_ndx 0 in_tag_encap_ndx 0 use_two_vlans 0\r\n"
    "  llvp_inner_comp_index 0 llvp_outer_comp_index 0 tag_edit_profile 0\r\n"
    "  acceptable_frame_type_action 0 incoming_s_tag_exist 0 incoming_tag_exist 0\r\n"
    "  incoming_vid_exist 0\r\n"
#endif
    "",
    "",
    {BIT(12), 0, 0, 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_OUT_IS_PRIO_NDX_ID,
    "out_is_prio_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  out_is_prio_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  out_is_prio_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_llvp_classification_tbl_set port_profile_ndx 0\r\n"
    "  out_tag_encap_ndx 0 out_is_prio_ndx 0 in_tag_encap_ndx 0 use_two_vlans 0\r\n"
    "  llvp_inner_comp_index 0 llvp_outer_comp_index 0 tag_edit_profile 0\r\n"
    "  acceptable_frame_type_action 0 incoming_s_tag_exist 0 incoming_tag_exist 0\r\n"
    "  incoming_vid_exist 0\r\n"
#endif
    "",
    "",
    {BIT(12), 0, 0, 0},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_IN_TAG_ENCAP_NDX_ID,
    "in_tag_encap_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  in_tag_encap_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  in_tag_encap_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_llvp_classification_tbl_set port_profile_ndx 0\r\n"
    "  out_tag_encap_ndx 0 out_is_prio_ndx 0 in_tag_encap_ndx 0 use_two_vlans 0\r\n"
    "  llvp_inner_comp_index 0 llvp_outer_comp_index 0 tag_edit_profile 0\r\n"
    "  acceptable_frame_type_action 0 incoming_s_tag_exist 0 incoming_tag_exist 0\r\n"
    "  incoming_vid_exist 0\r\n"
#endif
    "",
    "",
    {BIT(12), 0, 0, 0},
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_TBL_DATA_USE_TWO_VLANS_ID,
    "use_two_vlans",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.use_two_vlans:\r\n"
    "  uint32 use_two_vlans\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_llvp_classification_tbl_set port_profile_ndx 0\r\n"
    "  out_tag_encap_ndx 0 out_is_prio_ndx 0 in_tag_encap_ndx 0 use_two_vlans 0\r\n"
    "  llvp_inner_comp_index 0 llvp_outer_comp_index 0 tag_edit_profile 0\r\n"
    "  acceptable_frame_type_action 0 incoming_s_tag_exist 0 incoming_tag_exist 0\r\n"
    "  incoming_vid_exist 0\r\n"
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
    PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_TBL_DATA_LLVP_INNER_COMP_INDEX_ID,
    "llvp_inner_comp_index",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.llvp_inner_comp_index:\r\n"
    "  uint32 llvp_inner_comp_index\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_llvp_classification_tbl_set port_profile_ndx 0\r\n"
    "  out_tag_encap_ndx 0 out_is_prio_ndx 0 in_tag_encap_ndx 0 use_two_vlans 0\r\n"
    "  llvp_inner_comp_index 0 llvp_outer_comp_index 0 tag_edit_profile 0\r\n"
    "  acceptable_frame_type_action 0 incoming_s_tag_exist 0 incoming_tag_exist 0\r\n"
    "  incoming_vid_exist 0\r\n"
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
    PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_TBL_DATA_LLVP_OUTER_COMP_INDEX_ID,
    "llvp_outer_comp_index",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.llvp_outer_comp_index:\r\n"
    "  uint32 llvp_outer_comp_index\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_llvp_classification_tbl_set port_profile_ndx 0\r\n"
    "  out_tag_encap_ndx 0 out_is_prio_ndx 0 in_tag_encap_ndx 0 use_two_vlans 0\r\n"
    "  llvp_inner_comp_index 0 llvp_outer_comp_index 0 tag_edit_profile 0\r\n"
    "  acceptable_frame_type_action 0 incoming_s_tag_exist 0 incoming_tag_exist 0\r\n"
    "  incoming_vid_exist 0\r\n"
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
    PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_TBL_DATA_TAG_EDIT_PROFILE_ID,
    "tag_edit_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.tag_edit_profile:\r\n"
    "  uint32 tag_edit_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_llvp_classification_tbl_set port_profile_ndx 0\r\n"
    "  out_tag_encap_ndx 0 out_is_prio_ndx 0 in_tag_encap_ndx 0 use_two_vlans 0\r\n"
    "  llvp_inner_comp_index 0 llvp_outer_comp_index 0 tag_edit_profile 0\r\n"
    "  acceptable_frame_type_action 0 incoming_s_tag_exist 0 incoming_tag_exist 0\r\n"
    "  incoming_vid_exist 0\r\n"
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
    PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_TBL_DATA_ACCEPTABLE_FRAME_TYPE_ACTION_ID,
    "acceptable_frame_type_action",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.acceptable_frame_type_action:\r\n"
    "  uint32 acceptable_frame_type_action\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_llvp_classification_tbl_set port_profile_ndx 0\r\n"
    "  out_tag_encap_ndx 0 out_is_prio_ndx 0 in_tag_encap_ndx 0 use_two_vlans 0\r\n"
    "  llvp_inner_comp_index 0 llvp_outer_comp_index 0 tag_edit_profile 0\r\n"
    "  acceptable_frame_type_action 0 incoming_s_tag_exist 0 incoming_tag_exist 0\r\n"
    "  incoming_vid_exist 0\r\n"
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
    PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_TBL_DATA_INCOMING_S_TAG_EXIST_ID,
    "incoming_s_tag_exist",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.incoming_s_tag_exist:\r\n"
    "  uint32 incoming_s_tag_exist\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_llvp_classification_tbl_set port_profile_ndx 0\r\n"
    "  out_tag_encap_ndx 0 out_is_prio_ndx 0 in_tag_encap_ndx 0 use_two_vlans 0\r\n"
    "  llvp_inner_comp_index 0 llvp_outer_comp_index 0 tag_edit_profile 0\r\n"
    "  acceptable_frame_type_action 0 incoming_s_tag_exist 0 incoming_tag_exist 0\r\n"
    "  incoming_vid_exist 0\r\n"
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
    PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_TBL_DATA_INCOMING_TAG_EXIST_ID,
    "incoming_tag_exist",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.incoming_tag_exist:\r\n"
    "  uint32 incoming_tag_exist\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_llvp_classification_tbl_set port_profile_ndx 0\r\n"
    "  out_tag_encap_ndx 0 out_is_prio_ndx 0 in_tag_encap_ndx 0 use_two_vlans 0\r\n"
    "  llvp_inner_comp_index 0 llvp_outer_comp_index 0 tag_edit_profile 0\r\n"
    "  acceptable_frame_type_action 0 incoming_s_tag_exist 0 incoming_tag_exist 0\r\n"
    "  incoming_vid_exist 0\r\n"
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
    PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_SET_PP_LLVP_CLASSIFICATION_TBL_SET_TBL_DATA_INCOMING_VID_EXIST_ID,
    "incoming_vid_exist",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.incoming_vid_exist:\r\n"
    "  uint32 incoming_vid_exist\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_llvp_classification_tbl_set port_profile_ndx 0\r\n"
    "  out_tag_encap_ndx 0 out_is_prio_ndx 0 in_tag_encap_ndx 0 use_two_vlans 0\r\n"
    "  llvp_inner_comp_index 0 llvp_outer_comp_index 0 tag_edit_profile 0\r\n"
    "  acceptable_frame_type_action 0 incoming_s_tag_exist 0 incoming_tag_exist 0\r\n"
    "  incoming_vid_exist 0\r\n"
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
    PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_GET_PP_LLVP_CLASSIFICATION_TBL_GET_ID,
    "llvp_classification_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_llvp_classification_tbl_get port_profile_ndx 0\r\n"
    "  out_tag_encap_ndx 0 out_is_prio_ndx 0 in_tag_encap_ndx 0\r\n"
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
    PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_GET_PP_LLVP_CLASSIFICATION_TBL_GET_PORT_PROFILE_NDX_ID,
    "port_profile_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_profile_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  port_profile_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_llvp_classification_tbl_get port_profile_ndx 0\r\n"
    "  out_tag_encap_ndx 0 out_is_prio_ndx 0 in_tag_encap_ndx 0\r\n"
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
    PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_GET_PP_LLVP_CLASSIFICATION_TBL_GET_OUT_TAG_ENCAP_NDX_ID,
    "out_tag_encap_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  out_tag_encap_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  out_tag_encap_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_llvp_classification_tbl_get port_profile_ndx 0\r\n"
    "  out_tag_encap_ndx 0 out_is_prio_ndx 0 in_tag_encap_ndx 0\r\n"
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
    PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_GET_PP_LLVP_CLASSIFICATION_TBL_GET_OUT_IS_PRIO_NDX_ID,
    "out_is_prio_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  out_is_prio_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  out_is_prio_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_llvp_classification_tbl_get port_profile_ndx 0\r\n"
    "  out_tag_encap_ndx 0 out_is_prio_ndx 0 in_tag_encap_ndx 0\r\n"
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
    PARAM_T20E_PP_LLVP_CLASSIFICATION_TBL_GET_PP_LLVP_CLASSIFICATION_TBL_GET_IN_TAG_ENCAP_NDX_ID,
    "in_tag_encap_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  in_tag_encap_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  in_tag_encap_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_llvp_classification_tbl_get port_profile_ndx 0\r\n"
    "  out_tag_encap_ndx 0 out_is_prio_ndx 0 in_tag_encap_ndx 0\r\n"
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
    PARAM_T20E_PP_IPV4_SUBNET_CAMI_TBL_SET_PP_IPV4_SUBNET_CAMI_TBL_SET_ID,
    "ipv4_subnet_cami_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ipv4_subnet_cami_tbl_set entry 0\r\n"
    "  ipv4_subnet_bmap 0\r\n"
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
    PARAM_T20E_PP_IPV4_SUBNET_CAMI_TBL_SET_PP_IPV4_SUBNET_CAMI_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ipv4_subnet_cami_tbl_set entry 0\r\n"
    "  ipv4_subnet_bmap 0\r\n"
#endif
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
    PARAM_T20E_PP_IPV4_SUBNET_CAMI_TBL_SET_PP_IPV4_SUBNET_CAMI_TBL_SET_TBL_DATA_IPV4_SUBNET_BMAP_ID,
    "ipv4_subnet_bmap",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ipv4_subnet_bmap:\r\n"
    "  uint32 ipv4_subnet_bmap\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ipv4_subnet_cami_tbl_set entry 0\r\n"
    "  ipv4_subnet_bmap 0\r\n"
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
    PARAM_T20E_PP_IPV4_SUBNET_CAMI_TBL_GET_PP_IPV4_SUBNET_CAMI_TBL_GET_ID,
    "ipv4_subnet_cami_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ipv4_subnet_cami_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_IPV4_SUBNET_CAMI_TBL_GET_PP_IPV4_SUBNET_CAMI_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ipv4_subnet_cami_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_IPV4_SUBNET_TABLE_TBL_SET_PP_IPV4_SUBNET_TABLE_TBL_SET_ID,
    "ipv4_subnet_table_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ipv4_subnet_table_tbl_set entry 0\r\n"
    "  ipv4_subnet_tc_valid 0 ipv4_subnet_tc 0 ipv4_subnet_vid_valid 0\r\n"
    "  ipv4_subnet_vid 0\r\n"
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
    PARAM_T20E_PP_IPV4_SUBNET_TABLE_TBL_SET_PP_IPV4_SUBNET_TABLE_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ipv4_subnet_table_tbl_set entry 0\r\n"
    "  ipv4_subnet_tc_valid 0 ipv4_subnet_tc 0 ipv4_subnet_vid_valid 0\r\n"
    "  ipv4_subnet_vid 0\r\n"
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
    PARAM_T20E_PP_IPV4_SUBNET_TABLE_TBL_SET_PP_IPV4_SUBNET_TABLE_TBL_SET_TBL_DATA_IPV4_SUBNET_TC_VALID_ID,
    "ipv4_subnet_tc_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ipv4_subnet_tc_valid:\r\n"
    "  uint32 ipv4_subnet_tc_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ipv4_subnet_table_tbl_set entry 0\r\n"
    "  ipv4_subnet_tc_valid 0 ipv4_subnet_tc 0 ipv4_subnet_vid_valid 0\r\n"
    "  ipv4_subnet_vid 0\r\n"
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
    PARAM_T20E_PP_IPV4_SUBNET_TABLE_TBL_SET_PP_IPV4_SUBNET_TABLE_TBL_SET_TBL_DATA_IPV4_SUBNET_TC_ID,
    "ipv4_subnet_tc",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ipv4_subnet_tc:\r\n"
    "  uint32 ipv4_subnet_tc\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ipv4_subnet_table_tbl_set entry 0\r\n"
    "  ipv4_subnet_tc_valid 0 ipv4_subnet_tc 0 ipv4_subnet_vid_valid 0\r\n"
    "  ipv4_subnet_vid 0\r\n"
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
    PARAM_T20E_PP_IPV4_SUBNET_TABLE_TBL_SET_PP_IPV4_SUBNET_TABLE_TBL_SET_TBL_DATA_IPV4_SUBNET_VID_VALID_ID,
    "ipv4_subnet_vid_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ipv4_subnet_vid_valid:\r\n"
    "  uint32 ipv4_subnet_vid_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ipv4_subnet_table_tbl_set entry 0\r\n"
    "  ipv4_subnet_tc_valid 0 ipv4_subnet_tc 0 ipv4_subnet_vid_valid 0\r\n"
    "  ipv4_subnet_vid 0\r\n"
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
    PARAM_T20E_PP_IPV4_SUBNET_TABLE_TBL_SET_PP_IPV4_SUBNET_TABLE_TBL_SET_TBL_DATA_IPV4_SUBNET_VID_ID,
    "ipv4_subnet_vid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ipv4_subnet_vid:\r\n"
    "  uint32 ipv4_subnet_vid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ipv4_subnet_table_tbl_set entry 0\r\n"
    "  ipv4_subnet_tc_valid 0 ipv4_subnet_tc 0 ipv4_subnet_vid_valid 0\r\n"
    "  ipv4_subnet_vid 0\r\n"
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
    PARAM_T20E_PP_IPV4_SUBNET_TABLE_TBL_GET_PP_IPV4_SUBNET_TABLE_TBL_GET_ID,
    "ipv4_subnet_table_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ipv4_subnet_table_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_IPV4_SUBNET_TABLE_TBL_GET_PP_IPV4_SUBNET_TABLE_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ipv4_subnet_table_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_PORT_PROTOCOL_CAMI_TBL_SET_PP_PORT_PROTOCOL_CAMI_TBL_SET_ID,
    "port_protocol_cami_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_protocol_cami_tbl_set entry 0\r\n"
    "  ethernet_type_bmap 0\r\n"
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
    PARAM_T20E_PP_PORT_PROTOCOL_CAMI_TBL_SET_PP_PORT_PROTOCOL_CAMI_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_protocol_cami_tbl_set entry 0\r\n"
    "  ethernet_type_bmap 0\r\n"
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
    PARAM_T20E_PP_PORT_PROTOCOL_CAMI_TBL_SET_PP_PORT_PROTOCOL_CAMI_TBL_SET_TBL_DATA_ETHERNET_TYPE_BMAP_ID,
    "ethernet_type_bmap",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ethernet_type_bmap:\r\n"
    "  uint32 ethernet_type_bmap\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_protocol_cami_tbl_set entry 0\r\n"
    "  ethernet_type_bmap 0\r\n"
#endif
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
    PARAM_T20E_PP_PORT_PROTOCOL_CAMI_TBL_GET_PP_PORT_PROTOCOL_CAMI_TBL_GET_ID,
    "port_protocol_cami_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_protocol_cami_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_PORT_PROTOCOL_CAMI_TBL_GET_PP_PORT_PROTOCOL_CAMI_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_protocol_cami_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_PORT_PROTOCOL_TO_VID_TBL_SET_PP_PORT_PROTOCOL_TO_VID_TBL_SET_ID,
    "port_protocol_to_vid_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_protocol_to_vid_tbl_set entry 0\r\n"
    "  traffic_class_valid 0 traffic_class 0 vid_valid 0 vid 0\r\n"
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
    PARAM_T20E_PP_PORT_PROTOCOL_TO_VID_TBL_SET_PP_PORT_PROTOCOL_TO_VID_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_protocol_to_vid_tbl_set entry 0\r\n"
    "  traffic_class_valid 0 traffic_class 0 vid_valid 0 vid 0\r\n"
#endif
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
    PARAM_T20E_PP_PORT_PROTOCOL_TO_VID_TBL_SET_PP_PORT_PROTOCOL_TO_VID_TBL_SET_TBL_DATA_TRAFFIC_CLASS_VALID_ID,
    "traffic_class_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.traffic_class_valid:\r\n"
    "  uint32 traffic_class_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_protocol_to_vid_tbl_set entry 0\r\n"
    "  traffic_class_valid 0 traffic_class 0 vid_valid 0 vid 0\r\n"
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
    PARAM_T20E_PP_PORT_PROTOCOL_TO_VID_TBL_SET_PP_PORT_PROTOCOL_TO_VID_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID,
    "traffic_class",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.traffic_class:\r\n"
    "  uint32 traffic_class\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_protocol_to_vid_tbl_set entry 0\r\n"
    "  traffic_class_valid 0 traffic_class 0 vid_valid 0 vid 0\r\n"
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
    PARAM_T20E_PP_PORT_PROTOCOL_TO_VID_TBL_SET_PP_PORT_PROTOCOL_TO_VID_TBL_SET_TBL_DATA_VID_VALID_ID,
    "vid_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.vid_valid:\r\n"
    "  uint32 vid_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_protocol_to_vid_tbl_set entry 0\r\n"
    "  traffic_class_valid 0 traffic_class 0 vid_valid 0 vid 0\r\n"
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
    PARAM_T20E_PP_PORT_PROTOCOL_TO_VID_TBL_SET_PP_PORT_PROTOCOL_TO_VID_TBL_SET_TBL_DATA_VID_ID,
    "vid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.vid:\r\n"
    "  uint32 vid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_protocol_to_vid_tbl_set entry 0\r\n"
    "  traffic_class_valid 0 traffic_class 0 vid_valid 0 vid 0\r\n"
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
    PARAM_T20E_PP_PORT_PROTOCOL_TO_VID_TBL_GET_PP_PORT_PROTOCOL_TO_VID_TBL_GET_ID,
    "port_protocol_to_vid_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_protocol_to_vid_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_PORT_PROTOCOL_TO_VID_TBL_GET_PP_PORT_PROTOCOL_TO_VID_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_protocol_to_vid_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_VLAN_PORT_MEMBERSHIP_TBL_SET_PP_VLAN_PORT_MEMBERSHIP_TBL_SET_ID,
    "vlan_port_membership_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_vlan_port_membership_tbl_set entry 0\r\n"
    "  port_bmap_membership 0\r\n"
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
    PARAM_T20E_PP_VLAN_PORT_MEMBERSHIP_TBL_SET_PP_VLAN_PORT_MEMBERSHIP_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_vlan_port_membership_tbl_set entry 0\r\n"
    "  port_bmap_membership 0\r\n"
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
    PARAM_T20E_PP_VLAN_PORT_MEMBERSHIP_TBL_SET_PP_VLAN_PORT_MEMBERSHIP_TBL_SET_TBL_DATA_PORT_BMAP_MEMBERSHIP_ID,
    "port_bmap_membership",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.port_bmap_membership:\r\n"
    "  uint32 port_bmap_membership\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_vlan_port_membership_tbl_set entry 0\r\n"
    "  port_bmap_membership 0\r\n"
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
    PARAM_T20E_PP_VLAN_PORT_MEMBERSHIP_TBL_GET_PP_VLAN_PORT_MEMBERSHIP_TBL_GET_ID,
    "vlan_port_membership_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_vlan_port_membership_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_VLAN_PORT_MEMBERSHIP_TBL_GET_PP_VLAN_PORT_MEMBERSHIP_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_vlan_port_membership_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_RESERVED_MC_TBL_SET_PP_RESERVED_MC_TBL_SET_ID,
    "reserved_mc_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_reserved_mc_tbl_set entry 0\r\n"
    "  reserved_mc_trap_code 0\r\n"
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
    PARAM_T20E_PP_RESERVED_MC_TBL_SET_PP_RESERVED_MC_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_reserved_mc_tbl_set entry 0\r\n"
    "  reserved_mc_trap_code 0\r\n"
#endif
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
    PARAM_T20E_PP_RESERVED_MC_TBL_SET_PP_RESERVED_MC_TBL_SET_TBL_DATA_RESERVED_MC_TRAP_CODE_ID,
    "reserved_mc_trap_code",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.reserved_mc_trap_code:\r\n"
    "  uint32 reserved_mc_trap_code\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_reserved_mc_tbl_set entry 0\r\n"
    "  reserved_mc_trap_code 0\r\n"
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
    PARAM_T20E_PP_RESERVED_MC_TBL_GET_PP_RESERVED_MC_TBL_GET_ID,
    "reserved_mc_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_reserved_mc_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_RESERVED_MC_TBL_GET_PP_RESERVED_MC_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_reserved_mc_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_PCP_DECODING_TBL_SET_PP_PCP_DECODING_TBL_SET_ID,
    "pcp_decoding_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pcp_decoding_tbl_set entry 0 drop_eligible\r\n"
    "  0 traffic_class 0\r\n"
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
    PARAM_T20E_PP_PCP_DECODING_TBL_SET_PP_PCP_DECODING_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pcp_decoding_tbl_set entry 0 drop_eligible\r\n"
    "  0 traffic_class 0\r\n"
#endif
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
    PARAM_T20E_PP_PCP_DECODING_TBL_SET_PP_PCP_DECODING_TBL_SET_TBL_DATA_DROP_ELIGIBLE_ID,
    "drop_eligible",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.drop_eligible:\r\n"
    "  uint32 drop_eligible\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pcp_decoding_tbl_set entry 0 drop_eligible\r\n"
    "  0 traffic_class 0\r\n"
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
    PARAM_T20E_PP_PCP_DECODING_TBL_SET_PP_PCP_DECODING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID,
    "traffic_class",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.traffic_class:\r\n"
    "  uint32 traffic_class\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pcp_decoding_tbl_set entry 0 drop_eligible\r\n"
    "  0 traffic_class 0\r\n"
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
    PARAM_T20E_PP_PCP_DECODING_TBL_GET_PP_PCP_DECODING_TBL_GET_ID,
    "pcp_decoding_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pcp_decoding_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_PCP_DECODING_TBL_GET_PP_PCP_DECODING_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pcp_decoding_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_TOS_TO_TC_TBL_SET_PP_TOS_TO_TC_TBL_SET_ID,
    "tos_to_tc_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_tos_to_tc_tbl_set is_ipv4_ndx 0 tbl_ndx 0\r\n"
    "  entry 0 traffic_class_valid 0 traffic_class 0\r\n"
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
    PARAM_T20E_PP_TOS_TO_TC_TBL_SET_PP_TOS_TO_TC_TBL_SET_IS_IPV4_NDX_ID,
    "is_ipv4_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  is_ipv4_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint8\r\n"
    "  is_ipv4_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_tos_to_tc_tbl_set is_ipv4_ndx 0 tbl_ndx 0\r\n"
    "  entry 0 traffic_class_valid 0 traffic_class 0\r\n"
#endif
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
    PARAM_T20E_PP_TOS_TO_TC_TBL_SET_PP_TOS_TO_TC_TBL_SET_TBL_NDX_ID,
    "tbl_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  tbl_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_tos_to_tc_tbl_set is_ipv4_ndx 0 tbl_ndx 0\r\n"
    "  entry 0 traffic_class_valid 0 traffic_class 0\r\n"
#endif
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
    PARAM_T20E_PP_TOS_TO_TC_TBL_SET_PP_TOS_TO_TC_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_tos_to_tc_tbl_set is_ipv4_ndx 0 tbl_ndx 0\r\n"
    "  entry 0 traffic_class_valid 0 traffic_class 0\r\n"
#endif
    "",
    "",
    {BIT(28), 0, 0, 0},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_TOS_TO_TC_TBL_SET_PP_TOS_TO_TC_TBL_SET_TBL_DATA_TRAFFIC_CLASS_VALID_ID,
    "traffic_class_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.traffic_class_valid:\r\n"
    "  uint32 traffic_class_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_tos_to_tc_tbl_set is_ipv4_ndx 0 tbl_ndx 0\r\n"
    "  entry 0 traffic_class_valid 0 traffic_class 0\r\n"
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
    PARAM_T20E_PP_TOS_TO_TC_TBL_SET_PP_TOS_TO_TC_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID,
    "traffic_class",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.traffic_class:\r\n"
    "  uint32 traffic_class\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_tos_to_tc_tbl_set is_ipv4_ndx 0 tbl_ndx 0\r\n"
    "  entry 0 traffic_class_valid 0 traffic_class 0\r\n"
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
    PARAM_T20E_PP_TOS_TO_TC_TBL_GET_PP_TOS_TO_TC_TBL_GET_ID,
    "tos_to_tc_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_tos_to_tc_tbl_get is_ipv4_ndx 0 tbl_ndx 0\r\n"
    "  entry 0\r\n"
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
    PARAM_T20E_PP_TOS_TO_TC_TBL_GET_PP_TOS_TO_TC_TBL_GET_IS_IPV4_NDX_ID,
    "is_ipv4_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  is_ipv4_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint8\r\n"
    "  is_ipv4_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_tos_to_tc_tbl_get is_ipv4_ndx 0 tbl_ndx 0\r\n"
    "  entry 0\r\n"
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
    PARAM_T20E_PP_TOS_TO_TC_TBL_GET_PP_TOS_TO_TC_TBL_GET_TBL_NDX_ID,
    "tbl_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  tbl_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_tos_to_tc_tbl_get is_ipv4_ndx 0 tbl_ndx 0\r\n"
    "  entry 0\r\n"
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
    PARAM_T20E_PP_TOS_TO_TC_TBL_GET_PP_TOS_TO_TC_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_tos_to_tc_tbl_get is_ipv4_ndx 0 tbl_ndx 0\r\n"
    "  entry 0\r\n"
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
    PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_SET_ID,
    "l4_port_ranges_to_traffic_class_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_l4_port_ranges_to_traffic_class_tbl_set\r\n"
    "  entry 0 traffic_class_valid 0 traffic_class 0\r\n"
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
    PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_l4_port_ranges_to_traffic_class_tbl_set\r\n"
    "  entry 0 traffic_class_valid 0 traffic_class 0\r\n"
#endif
    "",
    "",
    {BIT(30), 0, 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_SET_TBL_DATA_TRAFFIC_CLASS_VALID_ID,
    "traffic_class_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.traffic_class_valid:\r\n"
    "  uint32 traffic_class_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_l4_port_ranges_to_traffic_class_tbl_set\r\n"
    "  entry 0 traffic_class_valid 0 traffic_class 0\r\n"
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
    PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID,
    "traffic_class",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.traffic_class:\r\n"
    "  uint32 traffic_class\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_l4_port_ranges_to_traffic_class_tbl_set\r\n"
    "  entry 0 traffic_class_valid 0 traffic_class 0\r\n"
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
    PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_GET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_GET_ID,
    "l4_port_ranges_to_traffic_class_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_l4_port_ranges_to_traffic_class_tbl_get\r\n"
    "  entry 0\r\n"
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
    PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_GET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_l4_port_ranges_to_traffic_class_tbl_get\r\n"
    "  entry 0\r\n"
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
    PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_ID,
    "l4_port_ranges_to_traffic_class_tbl_parsed_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_l4_port_ranges_to_traffic_class_tbl_parsed_set\r\n"
    "  is_tc_ndx 0 src_range_ndx 0 dest_range_ndx 0 traffic_class_valid 0\r\n"
    "  traffic_class 0\r\n"
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
    PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_IS_TC_NDX_ID,
    "is_tc_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  is_tc_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  is_tc_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_l4_port_ranges_to_traffic_class_tbl_parsed_set\r\n"
    "  is_tc_ndx 0 src_range_ndx 0 dest_range_ndx 0 traffic_class_valid 0\r\n"
    "  traffic_class 0\r\n"
#endif
    "",
    "",
    {0, BIT(0), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_SRC_RANGE_NDX_ID,
    "src_range_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  src_range_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  src_range_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_l4_port_ranges_to_traffic_class_tbl_parsed_set\r\n"
    "  is_tc_ndx 0 src_range_ndx 0 dest_range_ndx 0 traffic_class_valid 0\r\n"
    "  traffic_class 0\r\n"
#endif
    "",
    "",
    {0, BIT(0), 0, 0},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_DEST_RANGE_NDX_ID,
    "dest_range_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  dest_range_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  dest_range_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_l4_port_ranges_to_traffic_class_tbl_parsed_set\r\n"
    "  is_tc_ndx 0 src_range_ndx 0 dest_range_ndx 0 traffic_class_valid 0\r\n"
    "  traffic_class 0\r\n"
#endif
    "",
    "",
    {0, BIT(0), 0, 0},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_TBL_DATA_TRAFFIC_CLASS_VALID_ID,
    "traffic_class_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.traffic_class_valid:\r\n"
    "  uint32 traffic_class_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_l4_port_ranges_to_traffic_class_tbl_parsed_set\r\n"
    "  is_tc_ndx 0 src_range_ndx 0 dest_range_ndx 0 traffic_class_valid 0\r\n"
    "  traffic_class 0\r\n"
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
    PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_SET_TBL_DATA_TRAFFIC_CLASS_ID,
    "traffic_class",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.traffic_class:\r\n"
    "  uint32 traffic_class\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_l4_port_ranges_to_traffic_class_tbl_parsed_set\r\n"
    "  is_tc_ndx 0 src_range_ndx 0 dest_range_ndx 0 traffic_class_valid 0\r\n"
    "  traffic_class 0\r\n"
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
    PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_GET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_GET_ID,
    "l4_port_ranges_to_traffic_class_tbl_parsed_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_l4_port_ranges_to_traffic_class_tbl_parsed_get\r\n"
    "  is_tc_ndx 0 src_range_ndx 0 dest_range_ndx 0\r\n"
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
    PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_GET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_GET_IS_TC_NDX_ID,
    "is_tc_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  is_tc_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  is_tc_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_l4_port_ranges_to_traffic_class_tbl_parsed_get\r\n"
    "  is_tc_ndx 0 src_range_ndx 0 dest_range_ndx 0\r\n"
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
    PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_GET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_GET_SRC_RANGE_NDX_ID,
    "src_range_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  src_range_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  src_range_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_l4_port_ranges_to_traffic_class_tbl_parsed_get\r\n"
    "  is_tc_ndx 0 src_range_ndx 0 dest_range_ndx 0\r\n"
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
    PARAM_T20E_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_GET_PP_L4_PORT_RANGES_TO_TRAFFIC_CLASS_TBL_PARSED_GET_DEST_RANGE_NDX_ID,
    "dest_range_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  dest_range_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  dest_range_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_l4_port_ranges_to_traffic_class_tbl_parsed_get\r\n"
    "  is_tc_ndx 0 src_range_ndx 0 dest_range_ndx 0\r\n"
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
    PARAM_T20E_PP_DROP_PRECEDENCE_MAP_TC_TBL_SET_PP_DROP_PRECEDENCE_MAP_TC_TBL_SET_ID,
    "drop_precedence_map_tc_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_drop_precedence_map_tc_tbl_set entry 0\r\n"
    "  drop_precedence 0\r\n"
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
    PARAM_T20E_PP_DROP_PRECEDENCE_MAP_TC_TBL_SET_PP_DROP_PRECEDENCE_MAP_TC_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_drop_precedence_map_tc_tbl_set entry 0\r\n"
    "  drop_precedence 0\r\n"
#endif
    "",
    "",
    {0, BIT(2), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_DROP_PRECEDENCE_MAP_TC_TBL_SET_PP_DROP_PRECEDENCE_MAP_TC_TBL_SET_TBL_DATA_DROP_PRECEDENCE_ID,
    "drop_precedence",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.drop_precedence:\r\n"
    "  uint32 drop_precedence\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_drop_precedence_map_tc_tbl_set entry 0\r\n"
    "  drop_precedence 0\r\n"
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
    PARAM_T20E_PP_DROP_PRECEDENCE_MAP_TC_TBL_GET_PP_DROP_PRECEDENCE_MAP_TC_TBL_GET_ID,
    "drop_precedence_map_tc_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_drop_precedence_map_tc_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_DROP_PRECEDENCE_MAP_TC_TBL_GET_PP_DROP_PRECEDENCE_MAP_TC_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_drop_precedence_map_tc_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_AC_OFFSET_TBL_SET_PP_AC_OFFSET_TBL_SET_ID,
    "ac_offset_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_offset_tbl_set entry 0 ac_offset 0\r\n"
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
    PARAM_T20E_PP_AC_OFFSET_TBL_SET_PP_AC_OFFSET_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_offset_tbl_set entry 0 ac_offset 0\r\n"
#endif
    "",
    "",
    {0, BIT(4), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_OFFSET_TBL_SET_PP_AC_OFFSET_TBL_SET_TBL_DATA_AC_OFFSET_ID,
    "ac_offset",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ac_offset:\r\n"
    "  uint32 ac_offset\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_offset_tbl_set entry 0 ac_offset 0\r\n"
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
    PARAM_T20E_PP_AC_OFFSET_TBL_GET_PP_AC_OFFSET_TBL_GET_ID,
    "ac_offset_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_offset_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_AC_OFFSET_TBL_GET_PP_AC_OFFSET_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_offset_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_COS_PROFILE_DECODING_TBL_SET_PP_COS_PROFILE_DECODING_TBL_SET_ID,
    "cos_profile_decoding_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_cos_profile_decoding_tbl_set entry 0\r\n"
    "  drop_precedence 0 traffic_class 0 map_profile 0 force_or_map 0\r\n"
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
    PARAM_T20E_PP_COS_PROFILE_DECODING_TBL_SET_PP_COS_PROFILE_DECODING_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_cos_profile_decoding_tbl_set entry 0\r\n"
    "  drop_precedence 0 traffic_class 0 map_profile 0 force_or_map 0\r\n"
#endif
    "",
    "",
    {0, BIT(6), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_COS_PROFILE_DECODING_TBL_SET_PP_COS_PROFILE_DECODING_TBL_SET_TBL_DATA_DROP_PRECEDENCE_ID,
    "drop_precedence",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.drop_precedence:\r\n"
    "  uint32 drop_precedence\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_cos_profile_decoding_tbl_set entry 0\r\n"
    "  drop_precedence 0 traffic_class 0 map_profile 0 force_or_map 0\r\n"
#endif
    "",
    "",
    {0, BIT(6), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_COS_PROFILE_DECODING_TBL_SET_PP_COS_PROFILE_DECODING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID,
    "traffic_class",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.traffic_class:\r\n"
    "  uint32 traffic_class\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_cos_profile_decoding_tbl_set entry 0\r\n"
    "  drop_precedence 0 traffic_class 0 map_profile 0 force_or_map 0\r\n"
#endif
    "",
    "",
    {0, BIT(6), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_COS_PROFILE_DECODING_TBL_SET_PP_COS_PROFILE_DECODING_TBL_SET_TBL_DATA_MAP_PROFILE_ID,
    "map_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.map_profile:\r\n"
    "  uint32 map_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_cos_profile_decoding_tbl_set entry 0\r\n"
    "  drop_precedence 0 traffic_class 0 map_profile 0 force_or_map 0\r\n"
#endif
    "",
    "",
    {0, BIT(6), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_COS_PROFILE_DECODING_TBL_SET_PP_COS_PROFILE_DECODING_TBL_SET_TBL_DATA_FORCE_OR_MAP_ID,
    "force_or_map",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.force_or_map:\r\n"
    "  uint32 force_or_map\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_cos_profile_decoding_tbl_set entry 0\r\n"
    "  drop_precedence 0 traffic_class 0 map_profile 0 force_or_map 0\r\n"
#endif
    "",
    "",
    {0, BIT(6), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_COS_PROFILE_DECODING_TBL_GET_PP_COS_PROFILE_DECODING_TBL_GET_ID,
    "cos_profile_decoding_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_cos_profile_decoding_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_COS_PROFILE_DECODING_TBL_GET_PP_COS_PROFILE_DECODING_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_cos_profile_decoding_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_COS_PROFILE_MAPPING_TBL_SET_PP_COS_PROFILE_MAPPING_TBL_SET_ID,
    "cos_profile_mapping_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_cos_profile_mapping_tbl_set entry 0\r\n"
    "  drop_precedence 0 drop_precedence_ow 0 traffic_class 0 traffic_class_ow 0\r\n"
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
    PARAM_T20E_PP_COS_PROFILE_MAPPING_TBL_SET_PP_COS_PROFILE_MAPPING_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_cos_profile_mapping_tbl_set entry 0\r\n"
    "  drop_precedence 0 drop_precedence_ow 0 traffic_class 0 traffic_class_ow 0\r\n"
#endif
    "",
    "",
    {0, BIT(8), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_COS_PROFILE_MAPPING_TBL_SET_PP_COS_PROFILE_MAPPING_TBL_SET_TBL_DATA_DROP_PRECEDENCE_ID,
    "drop_precedence",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.drop_precedence:\r\n"
    "  uint32 drop_precedence\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_cos_profile_mapping_tbl_set entry 0\r\n"
    "  drop_precedence 0 drop_precedence_ow 0 traffic_class 0 traffic_class_ow 0\r\n"
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
    PARAM_T20E_PP_COS_PROFILE_MAPPING_TBL_SET_PP_COS_PROFILE_MAPPING_TBL_SET_TBL_DATA_DROP_PRECEDENCE_OW_ID,
    "drop_precedence_ow",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.drop_precedence_ow:\r\n"
    "  uint32 drop_precedence_ow\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_cos_profile_mapping_tbl_set entry 0\r\n"
    "  drop_precedence 0 drop_precedence_ow 0 traffic_class 0 traffic_class_ow 0\r\n"
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
    PARAM_T20E_PP_COS_PROFILE_MAPPING_TBL_SET_PP_COS_PROFILE_MAPPING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID,
    "traffic_class",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.traffic_class:\r\n"
    "  uint32 traffic_class\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_cos_profile_mapping_tbl_set entry 0\r\n"
    "  drop_precedence 0 drop_precedence_ow 0 traffic_class 0 traffic_class_ow 0\r\n"
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
    PARAM_T20E_PP_COS_PROFILE_MAPPING_TBL_SET_PP_COS_PROFILE_MAPPING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_OW_ID,
    "traffic_class_ow",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.traffic_class_ow:\r\n"
    "  uint32 traffic_class_ow\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_cos_profile_mapping_tbl_set entry 0\r\n"
    "  drop_precedence 0 drop_precedence_ow 0 traffic_class 0 traffic_class_ow 0\r\n"
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
    PARAM_T20E_PP_COS_PROFILE_MAPPING_TBL_GET_PP_COS_PROFILE_MAPPING_TBL_GET_ID,
    "cos_profile_mapping_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_cos_profile_mapping_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_COS_PROFILE_MAPPING_TBL_GET_PP_COS_PROFILE_MAPPING_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_cos_profile_mapping_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_STP_STATE_TBL_SET_PP_STP_STATE_TBL_SET_ID,
    "stp_state_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_stp_state_tbl_set entry 0 stp_state 0\r\n"
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
    PARAM_T20E_PP_STP_STATE_TBL_SET_PP_STP_STATE_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_stp_state_tbl_set entry 0 stp_state 0\r\n"
#endif
    "",
    "",
    {0, BIT(10), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_STP_STATE_TBL_SET_PP_STP_STATE_TBL_SET_TBL_DATA_STP_STATE_ID,
    "stp_state",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.stp_state:\r\n"
    "  uint32 stp_state\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_stp_state_tbl_set entry 0 stp_state 0\r\n"
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
    PARAM_T20E_PP_STP_STATE_TBL_GET_PP_STP_STATE_TBL_GET_ID,
    "stp_state_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_stp_state_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_STP_STATE_TBL_GET_PP_STP_STATE_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_stp_state_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_PCP_DEI_MAP_TBL_SET_PP_PCP_DEI_MAP_TBL_SET_ID,
    "pcp_dei_map_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pcp_dei_map_tbl_set entry 0 dei 0 pcp 0\r\n"
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
    PARAM_T20E_PP_PCP_DEI_MAP_TBL_SET_PP_PCP_DEI_MAP_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pcp_dei_map_tbl_set entry 0 dei 0 pcp 0\r\n"
#endif
    "",
    "",
    {0, BIT(12), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PCP_DEI_MAP_TBL_SET_PP_PCP_DEI_MAP_TBL_SET_TBL_DATA_DEI_ID,
    "dei",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.dei:\r\n"
    "  uint32 dei\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pcp_dei_map_tbl_set entry 0 dei 0 pcp 0\r\n"
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
    PARAM_T20E_PP_PCP_DEI_MAP_TBL_SET_PP_PCP_DEI_MAP_TBL_SET_TBL_DATA_PCP_ID,
    "pcp",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.pcp:\r\n"
    "  uint32 pcp\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pcp_dei_map_tbl_set entry 0 dei 0 pcp 0\r\n"
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
    PARAM_T20E_PP_PCP_DEI_MAP_TBL_GET_PP_PCP_DEI_MAP_TBL_GET_ID,
    "pcp_dei_map_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pcp_dei_map_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, BIT(13), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PCP_DEI_MAP_TBL_GET_PP_PCP_DEI_MAP_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pcp_dei_map_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, BIT(13), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_IVEC_TBL_SET_PP_IVEC_TBL_SET_ID,
    "ivec_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ivec_tbl_set entry 0 command 0\r\n"
#endif
    "",
    "",
    {0, BIT(14), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_IVEC_TBL_SET_PP_IVEC_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ivec_tbl_set entry 0 command 0\r\n"
#endif
    "",
    "",
    {0, BIT(14), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_IVEC_TBL_SET_PP_IVEC_TBL_SET_TBL_DATA_COMMAND_ID,
    "command",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.command:\r\n"
    "  uint32 command\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ivec_tbl_set entry 0 command 0\r\n"
#endif
    "",
    "",
    {0, BIT(14), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_IVEC_TBL_GET_PP_IVEC_TBL_GET_ID,
    "ivec_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ivec_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, BIT(15), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_IVEC_TBL_GET_PP_IVEC_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ivec_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, BIT(15), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_ID,
    "action_profile_mpls_value_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_action_profile_mpls_value_tbl_set entry 0\r\n"
    "  action_bos_snoop 0 action_bos_frwrd 0 action_not_bos_snoop 0\r\n"
    "  action_not_bos_frwrd 0\r\n"
#endif
    "",
    "",
    {0, BIT(16), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_action_profile_mpls_value_tbl_set entry 0\r\n"
    "  action_bos_snoop 0 action_bos_frwrd 0 action_not_bos_snoop 0\r\n"
    "  action_not_bos_frwrd 0\r\n"
#endif
    "",
    "",
    {0, BIT(16), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_TBL_DATA_ACTION_BOS_SNOOP_ID,
    "action_bos_snoop",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.action_bos_snoop:\r\n"
    "  uint32 action_bos_snoop\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_action_profile_mpls_value_tbl_set entry 0\r\n"
    "  action_bos_snoop 0 action_bos_frwrd 0 action_not_bos_snoop 0\r\n"
    "  action_not_bos_frwrd 0\r\n"
#endif
    "",
    "",
    {0, BIT(16), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_TBL_DATA_ACTION_BOS_FRWRD_ID,
    "action_bos_frwrd",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.action_bos_frwrd:\r\n"
    "  uint32 action_bos_frwrd\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_action_profile_mpls_value_tbl_set entry 0\r\n"
    "  action_bos_snoop 0 action_bos_frwrd 0 action_not_bos_snoop 0\r\n"
    "  action_not_bos_frwrd 0\r\n"
#endif
    "",
    "",
    {0, BIT(16), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_TBL_DATA_ACTION_NOT_BOS_SNOOP_ID,
    "action_not_bos_snoop",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.action_not_bos_snoop:\r\n"
    "  uint32 action_not_bos_snoop\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_action_profile_mpls_value_tbl_set entry 0\r\n"
    "  action_bos_snoop 0 action_bos_frwrd 0 action_not_bos_snoop 0\r\n"
    "  action_not_bos_frwrd 0\r\n"
#endif
    "",
    "",
    {0, BIT(16), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_PP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_TBL_DATA_ACTION_NOT_BOS_FRWRD_ID,
    "action_not_bos_frwrd",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.action_not_bos_frwrd:\r\n"
    "  uint32 action_not_bos_frwrd\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_action_profile_mpls_value_tbl_set entry 0\r\n"
    "  action_bos_snoop 0 action_bos_frwrd 0 action_not_bos_snoop 0\r\n"
    "  action_not_bos_frwrd 0\r\n"
#endif
    "",
    "",
    {0, BIT(16), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_GET_PP_ACTION_PROFILE_MPLS_VALUE_TBL_GET_ID,
    "action_profile_mpls_value_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_action_profile_mpls_value_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, BIT(17), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_ACTION_PROFILE_MPLS_VALUE_TBL_GET_PP_ACTION_PROFILE_MPLS_VALUE_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_action_profile_mpls_value_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, BIT(17), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_ID,
    "mpls_cos_profile_decoding_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_cos_profile_decoding_tbl_set entry 0\r\n"
    "  drop_precedence 0 traffic_class 0 map_profile 0 force_or_map 0\r\n"
#endif
    "",
    "",
    {0, BIT(18), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_cos_profile_decoding_tbl_set entry 0\r\n"
    "  drop_precedence 0 traffic_class 0 map_profile 0 force_or_map 0\r\n"
#endif
    "",
    "",
    {0, BIT(18), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_TBL_DATA_DROP_PRECEDENCE_ID,
    "drop_precedence",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.drop_precedence:\r\n"
    "  uint32 drop_precedence\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_cos_profile_decoding_tbl_set entry 0\r\n"
    "  drop_precedence 0 traffic_class 0 map_profile 0 force_or_map 0\r\n"
#endif
    "",
    "",
    {0, BIT(18), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID,
    "traffic_class",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.traffic_class:\r\n"
    "  uint32 traffic_class\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_cos_profile_decoding_tbl_set entry 0\r\n"
    "  drop_precedence 0 traffic_class 0 map_profile 0 force_or_map 0\r\n"
#endif
    "",
    "",
    {0, BIT(18), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_TBL_DATA_MAP_PROFILE_ID,
    "map_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.map_profile:\r\n"
    "  uint32 map_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_cos_profile_decoding_tbl_set entry 0\r\n"
    "  drop_precedence 0 traffic_class 0 map_profile 0 force_or_map 0\r\n"
#endif
    "",
    "",
    {0, BIT(18), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_PP_MPLS_COS_PROFILE_DECODING_TBL_SET_TBL_DATA_FORCE_OR_MAP_ID,
    "force_or_map",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.force_or_map:\r\n"
    "  uint32 force_or_map\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_cos_profile_decoding_tbl_set entry 0\r\n"
    "  drop_precedence 0 traffic_class 0 map_profile 0 force_or_map 0\r\n"
#endif
    "",
    "",
    {0, BIT(18), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_GET_PP_MPLS_COS_PROFILE_DECODING_TBL_GET_ID,
    "mpls_cos_profile_decoding_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_cos_profile_decoding_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, BIT(19), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_COS_PROFILE_DECODING_TBL_GET_PP_MPLS_COS_PROFILE_DECODING_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_cos_profile_decoding_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, BIT(19), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_COS_MAPPING_TBL_SET_PP_MPLS_COS_MAPPING_TBL_SET_ID,
    "mpls_cos_mapping_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_cos_mapping_tbl_set entry 0\r\n"
    "  drop_precedence 0 drop_precedence_ow 0 traffic_class 0 traffic_class_ow 0\r\n"
#endif
    "",
    "",
    {0, BIT(20), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_COS_MAPPING_TBL_SET_PP_MPLS_COS_MAPPING_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_cos_mapping_tbl_set entry 0\r\n"
    "  drop_precedence 0 drop_precedence_ow 0 traffic_class 0 traffic_class_ow 0\r\n"
#endif
    "",
    "",
    {0, BIT(20), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_COS_MAPPING_TBL_SET_PP_MPLS_COS_MAPPING_TBL_SET_TBL_DATA_DROP_PRECEDENCE_ID,
    "drop_precedence",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.drop_precedence:\r\n"
    "  uint32 drop_precedence\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_cos_mapping_tbl_set entry 0\r\n"
    "  drop_precedence 0 drop_precedence_ow 0 traffic_class 0 traffic_class_ow 0\r\n"
#endif
    "",
    "",
    {0, BIT(20), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_COS_MAPPING_TBL_SET_PP_MPLS_COS_MAPPING_TBL_SET_TBL_DATA_DROP_PRECEDENCE_OW_ID,
    "drop_precedence_ow",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.drop_precedence_ow:\r\n"
    "  uint32 drop_precedence_ow\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_cos_mapping_tbl_set entry 0\r\n"
    "  drop_precedence 0 drop_precedence_ow 0 traffic_class 0 traffic_class_ow 0\r\n"
#endif
    "",
    "",
    {0, BIT(20), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_COS_MAPPING_TBL_SET_PP_MPLS_COS_MAPPING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID,
    "traffic_class",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.traffic_class:\r\n"
    "  uint32 traffic_class\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_cos_mapping_tbl_set entry 0\r\n"
    "  drop_precedence 0 drop_precedence_ow 0 traffic_class 0 traffic_class_ow 0\r\n"
#endif
    "",
    "",
    {0, BIT(20), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_COS_MAPPING_TBL_SET_PP_MPLS_COS_MAPPING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_OW_ID,
    "traffic_class_ow",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.traffic_class_ow:\r\n"
    "  uint32 traffic_class_ow\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_cos_mapping_tbl_set entry 0\r\n"
    "  drop_precedence 0 drop_precedence_ow 0 traffic_class 0 traffic_class_ow 0\r\n"
#endif
    "",
    "",
    {0, BIT(20), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_COS_MAPPING_TBL_GET_PP_MPLS_COS_MAPPING_TBL_GET_ID,
    "mpls_cos_mapping_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_cos_mapping_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, BIT(21), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_COS_MAPPING_TBL_GET_PP_MPLS_COS_MAPPING_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_cos_mapping_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, BIT(21), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_ID,
    "mpls_termination_cos_mapping_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_termination_cos_mapping_tbl_set entry\r\n"
    "  0 drop_precedence 0 drop_precedence_ow 0 traffic_class 0 traffic_class_ow 0\r\n"
#endif
    "",
    "",
    {0, BIT(22), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_termination_cos_mapping_tbl_set entry\r\n"
    "  0 drop_precedence 0 drop_precedence_ow 0 traffic_class 0 traffic_class_ow 0\r\n"
#endif
    "",
    "",
    {0, BIT(22), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_TBL_DATA_DROP_PRECEDENCE_ID,
    "drop_precedence",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.drop_precedence:\r\n"
    "  uint32 drop_precedence\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_termination_cos_mapping_tbl_set entry\r\n"
    "  0 drop_precedence 0 drop_precedence_ow 0 traffic_class 0 traffic_class_ow 0\r\n"
#endif
    "",
    "",
    {0, BIT(22), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_TBL_DATA_DROP_PRECEDENCE_OW_ID,
    "drop_precedence_ow",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.drop_precedence_ow:\r\n"
    "  uint32 drop_precedence_ow\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_termination_cos_mapping_tbl_set entry\r\n"
    "  0 drop_precedence 0 drop_precedence_ow 0 traffic_class 0 traffic_class_ow 0\r\n"
#endif
    "",
    "",
    {0, BIT(22), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_ID,
    "traffic_class",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.traffic_class:\r\n"
    "  uint32 traffic_class\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_termination_cos_mapping_tbl_set entry\r\n"
    "  0 drop_precedence 0 drop_precedence_ow 0 traffic_class 0 traffic_class_ow 0\r\n"
#endif
    "",
    "",
    {0, BIT(22), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_PP_MPLS_TERMINATION_COS_MAPPING_TBL_SET_TBL_DATA_TRAFFIC_CLASS_OW_ID,
    "traffic_class_ow",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.traffic_class_ow:\r\n"
    "  uint32 traffic_class_ow\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_termination_cos_mapping_tbl_set entry\r\n"
    "  0 drop_precedence 0 drop_precedence_ow 0 traffic_class 0 traffic_class_ow 0\r\n"
#endif
    "",
    "",
    {0, BIT(22), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_GET_PP_MPLS_TERMINATION_COS_MAPPING_TBL_GET_ID,
    "mpls_termination_cos_mapping_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_termination_cos_mapping_tbl_get entry\r\n"
    "  0\r\n"
#endif
    "",
    "",
    {0, BIT(23), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_TERMINATION_COS_MAPPING_TBL_GET_PP_MPLS_TERMINATION_COS_MAPPING_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_termination_cos_mapping_tbl_get entry\r\n"
    "  0\r\n"
#endif
    "",
    "",
    {0, BIT(23), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_TUNNEL_TBL_SET_PP_MPLS_TUNNEL_TBL_SET_ID,
    "mpls_tunnel_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_tunnel_tbl_set entry 0 stat_tag_valid\r\n"
    "  0 policer_valid 0 counter_valid 0 termination_valid 0\r\n"
#endif
    "",
    "",
    {0, BIT(24), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_TUNNEL_TBL_SET_PP_MPLS_TUNNEL_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_tunnel_tbl_set entry 0 stat_tag_valid\r\n"
    "  0 policer_valid 0 counter_valid 0 termination_valid 0\r\n"
#endif
    "",
    "",
    {0, BIT(24), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_TUNNEL_TBL_SET_PP_MPLS_TUNNEL_TBL_SET_TBL_DATA_STAT_TAG_VALID_ID,
    "stat_tag_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.stat_tag_valid:\r\n"
    "  uint32 stat_tag_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_tunnel_tbl_set entry 0 stat_tag_valid\r\n"
    "  0 policer_valid 0 counter_valid 0 termination_valid 0\r\n"
#endif
    "",
    "",
    {0, BIT(24), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_TUNNEL_TBL_SET_PP_MPLS_TUNNEL_TBL_SET_TBL_DATA_POLICER_VALID_ID,
    "policer_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.policer_valid:\r\n"
    "  uint32 policer_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_tunnel_tbl_set entry 0 stat_tag_valid\r\n"
    "  0 policer_valid 0 counter_valid 0 termination_valid 0\r\n"
#endif
    "",
    "",
    {0, BIT(24), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_TUNNEL_TBL_SET_PP_MPLS_TUNNEL_TBL_SET_TBL_DATA_COUNTER_VALID_ID,
    "counter_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.counter_valid:\r\n"
    "  uint32 counter_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_tunnel_tbl_set entry 0 stat_tag_valid\r\n"
    "  0 policer_valid 0 counter_valid 0 termination_valid 0\r\n"
#endif
    "",
    "",
    {0, BIT(24), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_TUNNEL_TBL_SET_PP_MPLS_TUNNEL_TBL_SET_TBL_DATA_TERMINATION_VALID_ID,
    "termination_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.termination_valid:\r\n"
    "  uint32 termination_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_tunnel_tbl_set entry 0 stat_tag_valid\r\n"
    "  0 policer_valid 0 counter_valid 0 termination_valid 0\r\n"
#endif
    "",
    "",
    {0, BIT(24), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_TUNNEL_TBL_GET_PP_MPLS_TUNNEL_TBL_GET_ID,
    "mpls_tunnel_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_tunnel_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, BIT(25), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MPLS_TUNNEL_TBL_GET_PP_MPLS_TUNNEL_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mpls_tunnel_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, BIT(25), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PROTECTION_INSTANCE_TBL_SET_PP_PROTECTION_INSTANCE_TBL_SET_ID,
    "protection_instance_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_protection_instance_tbl_set entry 0\r\n"
    "  status_bmap 0\r\n"
#endif
    "",
    "",
    {0, BIT(26), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PROTECTION_INSTANCE_TBL_SET_PP_PROTECTION_INSTANCE_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_protection_instance_tbl_set entry 0\r\n"
    "  status_bmap 0\r\n"
#endif
    "",
    "",
    {0, BIT(26), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PROTECTION_INSTANCE_TBL_SET_PP_PROTECTION_INSTANCE_TBL_SET_TBL_DATA_STATUS_BMAP_ID,
    "status_bmap",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.status_bmap:\r\n"
    "  uint32 status_bmap\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_protection_instance_tbl_set entry 0\r\n"
    "  status_bmap 0\r\n"
#endif
    "",
    "",
    {0, BIT(26), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PROTECTION_INSTANCE_TBL_GET_PP_PROTECTION_INSTANCE_TBL_GET_ID,
    "protection_instance_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_protection_instance_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, BIT(27), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PROTECTION_INSTANCE_TBL_GET_PP_PROTECTION_INSTANCE_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_protection_instance_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, BIT(27), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_DESTINATION_STATUS_TBL_SET_PP_DESTINATION_STATUS_TBL_SET_ID,
    "destination_status_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_destination_status_tbl_set entry 0\r\n"
    "  status_bmap 0\r\n"
#endif
    "",
    "",
    {0, BIT(28), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_DESTINATION_STATUS_TBL_SET_PP_DESTINATION_STATUS_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_destination_status_tbl_set entry 0\r\n"
    "  status_bmap 0\r\n"
#endif
    "",
    "",
    {0, BIT(28), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_DESTINATION_STATUS_TBL_SET_PP_DESTINATION_STATUS_TBL_SET_TBL_DATA_STATUS_BMAP_ID,
    "status_bmap",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.status_bmap:\r\n"
    "  uint32 status_bmap\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_destination_status_tbl_set entry 0\r\n"
    "  status_bmap 0\r\n"
#endif
    "",
    "",
    {0, BIT(28), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_DESTINATION_STATUS_TBL_GET_PP_DESTINATION_STATUS_TBL_GET_ID,
    "destination_status_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_destination_status_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, BIT(29), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_DESTINATION_STATUS_TBL_GET_PP_DESTINATION_STATUS_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_destination_status_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, BIT(29), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_ID,
    "port_config_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_SYSTEM_PORT_ID,
    "system_port",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.system_port:\r\n"
    "  uint32 system_port\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_DEFAULT_SNOOP_CMD_ID,
    "default_snoop_cmd",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.default_snoop_cmd:\r\n"
    "  uint32 default_snoop_cmd\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_USE_LL_ID,
    "use_ll",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.use_ll:\r\n"
    "  uint32 use_ll\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_LEARN_AC_ID,
    "learn_ac",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.learn_ac:\r\n"
    "  uint32 learn_ac\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_DROP_PRECEDENCE_PROFILE_ID,
    "drop_precedence_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.drop_precedence_profile:\r\n"
    "  uint32 drop_precedence_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_USE_DEI_ID,
    "use_dei",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.use_dei:\r\n"
    "  uint32 use_dei\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_DEFAULT_TRAFFIC_CLASS_ID,
    "default_traffic_class",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.default_traffic_class:\r\n"
    "  uint32 default_traffic_class\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_TC_PORT_PROTOCOL_ENABLE_ID,
    "tc_port_protocol_enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.tc_port_protocol_enable:\r\n"
    "  uint32 tc_port_protocol_enable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_TC_SUBNET_ENABLE_ID,
    "tc_subnet_enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.tc_subnet_enable:\r\n"
    "  uint32 tc_subnet_enable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_TC_L4_PROTOCOL_ENABLE_ID,
    "tc_l4_protocol_enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.tc_l4_protocol_enable:\r\n"
    "  uint32 tc_l4_protocol_enable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_TC_TOS_INDEX_ID,
    "tc_tos_index",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.tc_tos_index:\r\n"
    "  uint32 tc_tos_index\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_TC_TOS_ENABLE_ID,
    "tc_tos_enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.tc_tos_enable:\r\n"
    "  uint32 tc_tos_enable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_PCP_PROFILE_ID,
    "pcp_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.pcp_profile:\r\n"
    "  uint32 pcp_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_RESERVED_MC_TRAP_PROFILE_ID,
    "reserved_mc_trap_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.reserved_mc_trap_profile:\r\n"
    "  uint32 reserved_mc_trap_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_PROTOCOL_PROFILE_ID,
    "protocol_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.protocol_profile:\r\n"
    "  uint32 protocol_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_DEFAULT_INITIAL_VID_ID,
    "default_initial_vid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.default_initial_vid:\r\n"
    "  uint32 default_initial_vid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_VID_PORT_PROTOCOL_ENABLE_ID,
    "vid_port_protocol_enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.vid_port_protocol_enable:\r\n"
    "  uint32 vid_port_protocol_enable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_VID_IPV4_SUBNET_ENABLE_ID,
    "vid_ipv4_subnet_enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.vid_ipv4_subnet_enable:\r\n"
    "  uint32 vid_ipv4_subnet_enable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_IGNORE_INCOMING_PCP_ID,
    "ignore_incoming_pcp",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ignore_incoming_pcp:\r\n"
    "  uint32 ignore_incoming_pcp\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_IGNORE_INCOMING_VID_ID,
    "ignore_incoming_vid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ignore_incoming_vid:\r\n"
    "  uint32 ignore_incoming_vid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_TPID2_INDX_ID,
    "tpid2_indx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.tpid2_indx:\r\n"
    "  uint32 tpid2_indx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_TPID1_INDX_ID,
    "tpid1_indx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.tpid1_indx:\r\n"
    "  uint32 tpid1_indx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_SET_PP_PORT_CONFIG_TBL_SET_TBL_DATA_VLAN_CLASSIFICATION_PROFILE_ID,
    "vlan_classification_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.vlan_classification_profile:\r\n"
    "  uint32 vlan_classification_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_set entry 0 system_port 0\r\n"
    "  default_snoop_cmd 0 use_ll 0 learn_ac 0 port_profile 0\r\n"
    "  drop_precedence_profile 0 use_dei 0 default_traffic_class 0\r\n"
    "  tc_port_protocol_enable 0 tc_subnet_enable 0 tc_l4_protocol_enable 0\r\n"
    "  tc_tos_index 0 tc_tos_enable 0 pcp_profile 0 reserved_mc_trap_profile 0\r\n"
    "  protocol_profile 0 default_initial_vid 0 vid_port_protocol_enable 0\r\n"
    "  vid_ipv4_subnet_enable 0 ignore_incoming_pcp 0 ignore_incoming_vid 0\r\n"
    "  tpid2_indx 0 tpid1_indx 0 vlan_classification_profile 0\r\n"
#endif
    "",
    "",
    {0, BIT(30), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_GET_PP_PORT_CONFIG_TBL_GET_ID,
    "port_config_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, BIT(31), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PORT_CONFIG_TBL_GET_PP_PORT_CONFIG_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_port_config_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, BIT(31), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_ID,
    "mep_level_profile_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mep_level_profile_tbl_set entry 0 level7 0\r\n"
    "  level6 0 level5 0 level4 0 level3 0 level2 0 level1 0 level0 0\r\n"
#endif
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
    PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mep_level_profile_tbl_set entry 0 level7 0\r\n"
    "  level6 0 level5 0 level4 0 level3 0 level2 0 level1 0 level0 0\r\n"
#endif
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
    PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL7_ID,
    "level7",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.level7:\r\n"
    "  uint32 level7\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mep_level_profile_tbl_set entry 0 level7 0\r\n"
    "  level6 0 level5 0 level4 0 level3 0 level2 0 level1 0 level0 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(0), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL6_ID,
    "level6",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.level6:\r\n"
    "  uint32 level6\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mep_level_profile_tbl_set entry 0 level7 0\r\n"
    "  level6 0 level5 0 level4 0 level3 0 level2 0 level1 0 level0 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(0), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL5_ID,
    "level5",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.level5:\r\n"
    "  uint32 level5\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mep_level_profile_tbl_set entry 0 level7 0\r\n"
    "  level6 0 level5 0 level4 0 level3 0 level2 0 level1 0 level0 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(0), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL4_ID,
    "level4",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.level4:\r\n"
    "  uint32 level4\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mep_level_profile_tbl_set entry 0 level7 0\r\n"
    "  level6 0 level5 0 level4 0 level3 0 level2 0 level1 0 level0 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(0), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL3_ID,
    "level3",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.level3:\r\n"
    "  uint32 level3\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mep_level_profile_tbl_set entry 0 level7 0\r\n"
    "  level6 0 level5 0 level4 0 level3 0 level2 0 level1 0 level0 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(0), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL2_ID,
    "level2",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.level2:\r\n"
    "  uint32 level2\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mep_level_profile_tbl_set entry 0 level7 0\r\n"
    "  level6 0 level5 0 level4 0 level3 0 level2 0 level1 0 level0 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(0), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL1_ID,
    "level1",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.level1:\r\n"
    "  uint32 level1\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mep_level_profile_tbl_set entry 0 level7 0\r\n"
    "  level6 0 level5 0 level4 0 level3 0 level2 0 level1 0 level0 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(0), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_SET_PP_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL0_ID,
    "level0",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.level0:\r\n"
    "  uint32 level0\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mep_level_profile_tbl_set entry 0 level7 0\r\n"
    "  level6 0 level5 0 level4 0 level3 0 level2 0 level1 0 level0 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(0), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_GET_PP_MEP_LEVEL_PROFILE_TBL_GET_ID,
    "mep_level_profile_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mep_level_profile_tbl_get entry 0\r\n"
#endif
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
    PARAM_T20E_PP_MEP_LEVEL_PROFILE_TBL_GET_PP_MEP_LEVEL_PROFILE_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mep_level_profile_tbl_get entry 0\r\n"
#endif
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
    PARAM_T20E_PP_OAM_ACTION_TBL_SET_PP_OAM_ACTION_TBL_SET_ID,
    "action_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_oam_action_tbl_set entry 0\r\n"
    "  oam_action_profile 0\r\n"
#endif
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
    PARAM_T20E_PP_OAM_ACTION_TBL_SET_PP_OAM_ACTION_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_oam_action_tbl_set entry 0\r\n"
    "  oam_action_profile 0\r\n"
#endif
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
    PARAM_T20E_PP_OAM_ACTION_TBL_SET_PP_OAM_ACTION_TBL_SET_TBL_DATA_OAM_ACTION_PROFILE_ID,
    "action_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.oam_action_profile:\r\n"
    "  uint32 oam_action_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_oam_action_tbl_set entry 0\r\n"
    "  oam_action_profile 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(2), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_OAM_ACTION_TBL_GET_PP_OAM_ACTION_TBL_GET_ID,
    "action_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_oam_action_tbl_get entry 0\r\n"
#endif
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
    PARAM_T20E_PP_OAM_ACTION_TBL_GET_PP_OAM_ACTION_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_oam_action_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(3), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_ID,
    "forward_action_profile_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_forward_action_profile_tbl_set entry 0\r\n"
    "  strength 0 dp_ow 0 dp 0 tc_ow 0 tc 0 destination_valid 0 destination 0 drop 0\r\n"
    "  control 0 trap 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(4), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_forward_action_profile_tbl_set entry 0\r\n"
    "  strength 0 dp_ow 0 dp 0 tc_ow 0 tc 0 destination_valid 0 destination 0 drop 0\r\n"
    "  control 0 trap 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(4), 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_STRENGTH_ID,
    "strength",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.strength:\r\n"
    "  uint32 strength\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_forward_action_profile_tbl_set entry 0\r\n"
    "  strength 0 dp_ow 0 dp 0 tc_ow 0 tc 0 destination_valid 0 destination 0 drop 0\r\n"
    "  control 0 trap 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(4), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_DP_OW_ID,
    "dp_ow",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.dp_ow:\r\n"
    "  uint32 dp_ow\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_forward_action_profile_tbl_set entry 0\r\n"
    "  strength 0 dp_ow 0 dp 0 tc_ow 0 tc 0 destination_valid 0 destination 0 drop 0\r\n"
    "  control 0 trap 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(4), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_DP_ID,
    "dp",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.dp:\r\n"
    "  uint32 dp\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_forward_action_profile_tbl_set entry 0\r\n"
    "  strength 0 dp_ow 0 dp 0 tc_ow 0 tc 0 destination_valid 0 destination 0 drop 0\r\n"
    "  control 0 trap 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(4), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_TC_OW_ID,
    "tc_ow",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.tc_ow:\r\n"
    "  uint32 tc_ow\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_forward_action_profile_tbl_set entry 0\r\n"
    "  strength 0 dp_ow 0 dp 0 tc_ow 0 tc 0 destination_valid 0 destination 0 drop 0\r\n"
    "  control 0 trap 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(4), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_TC_ID,
    "tc",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.tc:\r\n"
    "  uint32 tc\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_forward_action_profile_tbl_set entry 0\r\n"
    "  strength 0 dp_ow 0 dp 0 tc_ow 0 tc 0 destination_valid 0 destination 0 drop 0\r\n"
    "  control 0 trap 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(4), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_DESTINATION_VALID_ID,
    "destination_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.destination_valid:\r\n"
    "  uint32 destination_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_forward_action_profile_tbl_set entry 0\r\n"
    "  strength 0 dp_ow 0 dp 0 tc_ow 0 tc 0 destination_valid 0 destination 0 drop 0\r\n"
    "  control 0 trap 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(4), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_DESTINATION_ID,
    "destination",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.destination:\r\n"
    "  uint32 destination\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_forward_action_profile_tbl_set entry 0\r\n"
    "  strength 0 dp_ow 0 dp 0 tc_ow 0 tc 0 destination_valid 0 destination 0 drop 0\r\n"
    "  control 0 trap 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(4), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_DROP_ID,
    "drop",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.drop:\r\n"
    "  uint32 drop\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_forward_action_profile_tbl_set entry 0\r\n"
    "  strength 0 dp_ow 0 dp 0 tc_ow 0 tc 0 destination_valid 0 destination 0 drop 0\r\n"
    "  control 0 trap 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(4), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_CONTROL_ID,
    "control",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.control:\r\n"
    "  uint32 control\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_forward_action_profile_tbl_set entry 0\r\n"
    "  strength 0 dp_ow 0 dp 0 tc_ow 0 tc 0 destination_valid 0 destination 0 drop 0\r\n"
    "  control 0 trap 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(4), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_SET_PP_FORWARD_ACTION_PROFILE_TBL_SET_TBL_DATA_TRAP_ID,
    "trap",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.trap:\r\n"
    "  uint32 trap\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_forward_action_profile_tbl_set entry 0\r\n"
    "  strength 0 dp_ow 0 dp 0 tc_ow 0 tc 0 destination_valid 0 destination 0 drop 0\r\n"
    "  control 0 trap 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(4), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_GET_PP_FORWARD_ACTION_PROFILE_TBL_GET_ID,
    "forward_action_profile_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_forward_action_profile_tbl_get entry 0\r\n"
#endif
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
    PARAM_T20E_PP_FORWARD_ACTION_PROFILE_TBL_GET_PP_FORWARD_ACTION_PROFILE_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_forward_action_profile_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(5), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_SNOOP_ACTION_PROFILE_TBL_SET_PP_SNOOP_ACTION_PROFILE_TBL_SET_ID,
    "snoop_action_profile_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_snoop_action_profile_tbl_set entry 0\r\n"
    "  strength 0\r\n"
#endif
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
    PARAM_T20E_PP_SNOOP_ACTION_PROFILE_TBL_SET_PP_SNOOP_ACTION_PROFILE_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_snoop_action_profile_tbl_set entry 0\r\n"
    "  strength 0\r\n"
#endif
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
    PARAM_T20E_PP_SNOOP_ACTION_PROFILE_TBL_SET_PP_SNOOP_ACTION_PROFILE_TBL_SET_TBL_DATA_STRENGTH_ID,
    "strength",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.strength:\r\n"
    "  uint32 strength\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_snoop_action_profile_tbl_set entry 0\r\n"
    "  strength 0\r\n"
#endif
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
    PARAM_T20E_PP_SNOOP_ACTION_PROFILE_TBL_GET_PP_SNOOP_ACTION_PROFILE_TBL_GET_ID,
    "snoop_action_profile_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_snoop_action_profile_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(7), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_SNOOP_ACTION_PROFILE_TBL_GET_PP_SNOOP_ACTION_PROFILE_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_snoop_action_profile_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(7), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_BURST_SIZE_TBL_SET_PP_BURST_SIZE_TBL_SET_ID,
    "burst_size_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_burst_size_tbl_set entry 0 max_bs_bytes 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(8), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_BURST_SIZE_TBL_SET_PP_BURST_SIZE_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_burst_size_tbl_set entry 0 max_bs_bytes 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(8), 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_BURST_SIZE_TBL_SET_PP_BURST_SIZE_TBL_SET_TBL_DATA_MAX_BS_BYTES_ID,
    "max_bs_bytes",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.max_bs_bytes:\r\n"
    "  uint32 max_bs_bytes\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_burst_size_tbl_set entry 0 max_bs_bytes 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(8), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_BURST_SIZE_TBL_GET_PP_BURST_SIZE_TBL_GET_ID,
    "burst_size_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_burst_size_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(9), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_BURST_SIZE_TBL_GET_PP_BURST_SIZE_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_burst_size_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(9), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MANTISSA_DIVISION_TBL_SET_PP_MANTISSA_DIVISION_TBL_SET_ID,
    "mantissa_division_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mantissa_division_tbl_set entry 0 value 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(10), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MANTISSA_DIVISION_TBL_SET_PP_MANTISSA_DIVISION_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mantissa_division_tbl_set entry 0 value 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(10), 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MANTISSA_DIVISION_TBL_SET_PP_MANTISSA_DIVISION_TBL_SET_TBL_DATA_VALUE_ID,
    "value",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.value:\r\n"
    "  uint32 value\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mantissa_division_tbl_set entry 0 value 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(10), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MANTISSA_DIVISION_TBL_GET_PP_MANTISSA_DIVISION_TBL_GET_ID,
    "mantissa_division_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mantissa_division_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(11), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MANTISSA_DIVISION_TBL_GET_PP_MANTISSA_DIVISION_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_mantissa_division_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(11), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_GLAG_RANGE_TBL_SET_PP_GLAG_RANGE_TBL_SET_ID,
    "glag_range_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_glag_range_tbl_set entry 0 lag_range 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(12), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_GLAG_RANGE_TBL_SET_PP_GLAG_RANGE_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_glag_range_tbl_set entry 0 lag_range 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(12), 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_GLAG_RANGE_TBL_SET_PP_GLAG_RANGE_TBL_SET_TBL_DATA_LAG_RANGE_ID,
    "lag_range",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.lag_range:\r\n"
    "  uint32 lag_range\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_glag_range_tbl_set entry 0 lag_range 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(12), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_GLAG_RANGE_TBL_GET_PP_GLAG_RANGE_TBL_GET_ID,
    "glag_range_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_glag_range_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(13), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_GLAG_RANGE_TBL_GET_PP_GLAG_RANGE_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_glag_range_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(13), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MODULO_TBL_SET_PP_MODULO_TBL_SET_ID,
    "modulo_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_modulo_tbl_set entry 0 lag_offset 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(14), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MODULO_TBL_SET_PP_MODULO_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_modulo_tbl_set entry 0 lag_offset 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(14), 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MODULO_TBL_SET_PP_MODULO_TBL_SET_TBL_DATA_LAG_OFFSET_ID,
    "lag_offset",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.lag_offset:\r\n"
    "  uint32 lag_offset\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_modulo_tbl_set entry 0 lag_offset 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(14), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MODULO_TBL_GET_PP_MODULO_TBL_GET_ID,
    "modulo_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_modulo_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(15), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_MODULO_TBL_GET_PP_MODULO_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_modulo_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(15), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_GLAG_OUT_PORT_TBL_SET_PP_GLAG_OUT_PORT_TBL_SET_ID,
    "glag_out_port_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_glag_out_port_tbl_set entry 0\r\n"
    "  out_system_port 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(16), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_GLAG_OUT_PORT_TBL_SET_PP_GLAG_OUT_PORT_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_glag_out_port_tbl_set entry 0\r\n"
    "  out_system_port 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(16), 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_GLAG_OUT_PORT_TBL_SET_PP_GLAG_OUT_PORT_TBL_SET_TBL_DATA_OUT_SYSTEM_PORT_ID,
    "out_system_port",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.out_system_port:\r\n"
    "  uint32 out_system_port\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_glag_out_port_tbl_set entry 0\r\n"
    "  out_system_port 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(16), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_GLAG_OUT_PORT_TBL_GET_PP_GLAG_OUT_PORT_TBL_GET_ID,
    "glag_out_port_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_glag_out_port_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(17), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_GLAG_OUT_PORT_TBL_GET_PP_GLAG_OUT_PORT_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_glag_out_port_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(17), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_VID_MIRR_CAMI_TBL_SET_PP_VID_MIRR_CAMI_TBL_SET_ID,
    "vid_mirr_cami_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_vid_mirr_cami_tbl_set entry 0\r\n"
    "  vid_mirr_bmap 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(18), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_VID_MIRR_CAMI_TBL_SET_PP_VID_MIRR_CAMI_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_vid_mirr_cami_tbl_set entry 0\r\n"
    "  vid_mirr_bmap 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(18), 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_VID_MIRR_CAMI_TBL_SET_PP_VID_MIRR_CAMI_TBL_SET_TBL_DATA_VID_MIRR_BMAP_ID,
    "vid_mirr_bmap",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.vid_mirr_bmap:\r\n"
    "  uint32 vid_mirr_bmap\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_vid_mirr_cami_tbl_set entry 0\r\n"
    "  vid_mirr_bmap 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(18), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_VID_MIRR_CAMI_TBL_GET_PP_VID_MIRR_CAMI_TBL_GET_ID,
    "vid_mirr_cami_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_vid_mirr_cami_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(19), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_VID_MIRR_CAMI_TBL_GET_PP_VID_MIRR_CAMI_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_vid_mirr_cami_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(19), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_VID_MIRR_PROFILE_TABLE_TBL_SET_PP_VID_MIRR_PROFILE_TABLE_TBL_SET_ID,
    "vid_mirr_profile_table_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_vid_mirr_profile_table_tbl_set entry 0\r\n"
    "  vid_mirr_profile 0\r\n"
#endif
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
    PARAM_T20E_PP_VID_MIRR_PROFILE_TABLE_TBL_SET_PP_VID_MIRR_PROFILE_TABLE_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_vid_mirr_profile_table_tbl_set entry 0\r\n"
    "  vid_mirr_profile 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(20), 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_VID_MIRR_PROFILE_TABLE_TBL_SET_PP_VID_MIRR_PROFILE_TABLE_TBL_SET_TBL_DATA_VID_MIRR_PROFILE_ID,
    "vid_mirr_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.vid_mirr_profile:\r\n"
    "  uint32 vid_mirr_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_vid_mirr_profile_table_tbl_set entry 0\r\n"
    "  vid_mirr_profile 0\r\n"
#endif
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
    PARAM_T20E_PP_VID_MIRR_PROFILE_TABLE_TBL_GET_PP_VID_MIRR_PROFILE_TABLE_TBL_GET_ID,
    "vid_mirr_profile_table_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_vid_mirr_profile_table_tbl_get entry 0\r\n"
#endif
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
    PARAM_T20E_PP_VID_MIRR_PROFILE_TABLE_TBL_GET_PP_VID_MIRR_PROFILE_TABLE_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_vid_mirr_profile_table_tbl_get entry 0\r\n"
#endif
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
    PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_ID,
    "output_port_config1_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_output_port_config1_tbl_set entry 0 mtu 0\r\n"
    "  default_snoop_command 0 default_action_profile 0 llvp_profile 0\r\n"
    "  port_cfg_ctpid_index 0 port_cfg_ctpid_valid 0 port_cfg_stpid_index 0\r\n"
    "  port_cfg_stpid_valid 0 outport_sys_port_id 0 outport_port_is_glag_member 0\r\n"
    "  mtu_check 0 split_horizon_filter_enabled 0 unknown_da_bc_filter_enabled 0\r\n"
    "  unknown_da_mc_filter_enabled 0 unknown_da_uc_filter_enabled 0\r\n"
    "  hair_pin_filtering_enabled 0 pep_acc_frm_types_filtering_enabled 0\r\n"
    "  stp_state_filtering_enabled 0 vsi_filtering_enabled 0 disable_filtering 0\r\n"
    "  disable_learning 0 port_type 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(22), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_output_port_config1_tbl_set entry 0 mtu 0\r\n"
    "  default_snoop_command 0 default_action_profile 0 llvp_profile 0\r\n"
    "  port_cfg_ctpid_index 0 port_cfg_ctpid_valid 0 port_cfg_stpid_index 0\r\n"
    "  port_cfg_stpid_valid 0 outport_sys_port_id 0 outport_port_is_glag_member 0\r\n"
    "  mtu_check 0 split_horizon_filter_enabled 0 unknown_da_bc_filter_enabled 0\r\n"
    "  unknown_da_mc_filter_enabled 0 unknown_da_uc_filter_enabled 0\r\n"
    "  hair_pin_filtering_enabled 0 pep_acc_frm_types_filtering_enabled 0\r\n"
    "  stp_state_filtering_enabled 0 vsi_filtering_enabled 0 disable_filtering 0\r\n"
    "  disable_learning 0 port_type 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(22), 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_MTU_ID,
    "mtu",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mtu:\r\n"
    "  uint32 mtu\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_output_port_config1_tbl_set entry 0 mtu 0\r\n"
    "  default_snoop_command 0 default_action_profile 0 llvp_profile 0\r\n"
    "  port_cfg_ctpid_index 0 port_cfg_ctpid_valid 0 port_cfg_stpid_index 0\r\n"
    "  port_cfg_stpid_valid 0 outport_sys_port_id 0 outport_port_is_glag_member 0\r\n"
    "  mtu_check 0 split_horizon_filter_enabled 0 unknown_da_bc_filter_enabled 0\r\n"
    "  unknown_da_mc_filter_enabled 0 unknown_da_uc_filter_enabled 0\r\n"
    "  hair_pin_filtering_enabled 0 pep_acc_frm_types_filtering_enabled 0\r\n"
    "  stp_state_filtering_enabled 0 vsi_filtering_enabled 0 disable_filtering 0\r\n"
    "  disable_learning 0 port_type 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(22), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_DEFAULT_SNOOP_COMMAND_ID,
    "default_snoop_command",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.default_snoop_command:\r\n"
    "  uint32 default_snoop_command\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_output_port_config1_tbl_set entry 0 mtu 0\r\n"
    "  default_snoop_command 0 default_action_profile 0 llvp_profile 0\r\n"
    "  port_cfg_ctpid_index 0 port_cfg_ctpid_valid 0 port_cfg_stpid_index 0\r\n"
    "  port_cfg_stpid_valid 0 outport_sys_port_id 0 outport_port_is_glag_member 0\r\n"
    "  mtu_check 0 split_horizon_filter_enabled 0 unknown_da_bc_filter_enabled 0\r\n"
    "  unknown_da_mc_filter_enabled 0 unknown_da_uc_filter_enabled 0\r\n"
    "  hair_pin_filtering_enabled 0 pep_acc_frm_types_filtering_enabled 0\r\n"
    "  stp_state_filtering_enabled 0 vsi_filtering_enabled 0 disable_filtering 0\r\n"
    "  disable_learning 0 port_type 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(22), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_DEFAULT_ACTION_PROFILE_ID,
    "default_action_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.default_action_profile:\r\n"
    "  uint32 default_action_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_output_port_config1_tbl_set entry 0 mtu 0\r\n"
    "  default_snoop_command 0 default_action_profile 0 llvp_profile 0\r\n"
    "  port_cfg_ctpid_index 0 port_cfg_ctpid_valid 0 port_cfg_stpid_index 0\r\n"
    "  port_cfg_stpid_valid 0 outport_sys_port_id 0 outport_port_is_glag_member 0\r\n"
    "  mtu_check 0 split_horizon_filter_enabled 0 unknown_da_bc_filter_enabled 0\r\n"
    "  unknown_da_mc_filter_enabled 0 unknown_da_uc_filter_enabled 0\r\n"
    "  hair_pin_filtering_enabled 0 pep_acc_frm_types_filtering_enabled 0\r\n"
    "  stp_state_filtering_enabled 0 vsi_filtering_enabled 0 disable_filtering 0\r\n"
    "  disable_learning 0 port_type 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(22), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_OUTPORT_SYS_PORT_ID_ID,
    "outport_sys_port_id",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.outport_sys_port_id:\r\n"
    "  uint32 outport_sys_port_id\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_output_port_config1_tbl_set entry 0 mtu 0\r\n"
    "  default_snoop_command 0 default_action_profile 0 llvp_profile 0\r\n"
    "  port_cfg_ctpid_index 0 port_cfg_ctpid_valid 0 port_cfg_stpid_index 0\r\n"
    "  port_cfg_stpid_valid 0 outport_sys_port_id 0 outport_port_is_glag_member 0\r\n"
    "  mtu_check 0 split_horizon_filter_enabled 0 unknown_da_bc_filter_enabled 0\r\n"
    "  unknown_da_mc_filter_enabled 0 unknown_da_uc_filter_enabled 0\r\n"
    "  hair_pin_filtering_enabled 0 pep_acc_frm_types_filtering_enabled 0\r\n"
    "  stp_state_filtering_enabled 0 vsi_filtering_enabled 0 disable_filtering 0\r\n"
    "  disable_learning 0 port_type 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(22), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_OUTPORT_PORT_IS_GLAG_MEMBER_ID,
    "outport_port_is_glag_member",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.outport_port_is_glag_member:\r\n"
    "  uint32 outport_port_is_glag_member\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_output_port_config1_tbl_set entry 0 mtu 0\r\n"
    "  default_snoop_command 0 default_action_profile 0 llvp_profile 0\r\n"
    "  port_cfg_ctpid_index 0 port_cfg_ctpid_valid 0 port_cfg_stpid_index 0\r\n"
    "  port_cfg_stpid_valid 0 outport_sys_port_id 0 outport_port_is_glag_member 0\r\n"
    "  mtu_check 0 split_horizon_filter_enabled 0 unknown_da_bc_filter_enabled 0\r\n"
    "  unknown_da_mc_filter_enabled 0 unknown_da_uc_filter_enabled 0\r\n"
    "  hair_pin_filtering_enabled 0 pep_acc_frm_types_filtering_enabled 0\r\n"
    "  stp_state_filtering_enabled 0 vsi_filtering_enabled 0 disable_filtering 0\r\n"
    "  disable_learning 0 port_type 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(22), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_MTU_CHECK_ID,
    "mtu_check",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mtu_check:\r\n"
    "  uint32 mtu_check\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_output_port_config1_tbl_set entry 0 mtu 0\r\n"
    "  default_snoop_command 0 default_action_profile 0 llvp_profile 0\r\n"
    "  port_cfg_ctpid_index 0 port_cfg_ctpid_valid 0 port_cfg_stpid_index 0\r\n"
    "  port_cfg_stpid_valid 0 outport_sys_port_id 0 outport_port_is_glag_member 0\r\n"
    "  mtu_check 0 split_horizon_filter_enabled 0 unknown_da_bc_filter_enabled 0\r\n"
    "  unknown_da_mc_filter_enabled 0 unknown_da_uc_filter_enabled 0\r\n"
    "  hair_pin_filtering_enabled 0 pep_acc_frm_types_filtering_enabled 0\r\n"
    "  stp_state_filtering_enabled 0 vsi_filtering_enabled 0 disable_filtering 0\r\n"
    "  disable_learning 0 port_type 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(22), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_SPLIT_HORIZON_FILTER_ENABLED_ID,
    "split_horizon_filter_enabled",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.split_horizon_filter_enabled:\r\n"
    "  uint32 split_horizon_filter_enabled\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_output_port_config1_tbl_set entry 0 mtu 0\r\n"
    "  default_snoop_command 0 default_action_profile 0 llvp_profile 0\r\n"
    "  port_cfg_ctpid_index 0 port_cfg_ctpid_valid 0 port_cfg_stpid_index 0\r\n"
    "  port_cfg_stpid_valid 0 outport_sys_port_id 0 outport_port_is_glag_member 0\r\n"
    "  mtu_check 0 split_horizon_filter_enabled 0 unknown_da_bc_filter_enabled 0\r\n"
    "  unknown_da_mc_filter_enabled 0 unknown_da_uc_filter_enabled 0\r\n"
    "  hair_pin_filtering_enabled 0 pep_acc_frm_types_filtering_enabled 0\r\n"
    "  stp_state_filtering_enabled 0 vsi_filtering_enabled 0 disable_filtering 0\r\n"
    "  disable_learning 0 port_type 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(22), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_UNKNOWN_DA_BC_FILTER_ENABLED_ID,
    "unknown_da_bc_filter_enabled",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.unknown_da_bc_filter_enabled:\r\n"
    "  uint32 unknown_da_bc_filter_enabled\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_output_port_config1_tbl_set entry 0 mtu 0\r\n"
    "  default_snoop_command 0 default_action_profile 0 llvp_profile 0\r\n"
    "  port_cfg_ctpid_index 0 port_cfg_ctpid_valid 0 port_cfg_stpid_index 0\r\n"
    "  port_cfg_stpid_valid 0 outport_sys_port_id 0 outport_port_is_glag_member 0\r\n"
    "  mtu_check 0 split_horizon_filter_enabled 0 unknown_da_bc_filter_enabled 0\r\n"
    "  unknown_da_mc_filter_enabled 0 unknown_da_uc_filter_enabled 0\r\n"
    "  hair_pin_filtering_enabled 0 pep_acc_frm_types_filtering_enabled 0\r\n"
    "  stp_state_filtering_enabled 0 vsi_filtering_enabled 0 disable_filtering 0\r\n"
    "  disable_learning 0 port_type 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(22), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_UNKNOWN_DA_MC_FILTER_ENABLED_ID,
    "unknown_da_mc_filter_enabled",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.unknown_da_mc_filter_enabled:\r\n"
    "  uint32 unknown_da_mc_filter_enabled\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_output_port_config1_tbl_set entry 0 mtu 0\r\n"
    "  default_snoop_command 0 default_action_profile 0 llvp_profile 0\r\n"
    "  port_cfg_ctpid_index 0 port_cfg_ctpid_valid 0 port_cfg_stpid_index 0\r\n"
    "  port_cfg_stpid_valid 0 outport_sys_port_id 0 outport_port_is_glag_member 0\r\n"
    "  mtu_check 0 split_horizon_filter_enabled 0 unknown_da_bc_filter_enabled 0\r\n"
    "  unknown_da_mc_filter_enabled 0 unknown_da_uc_filter_enabled 0\r\n"
    "  hair_pin_filtering_enabled 0 pep_acc_frm_types_filtering_enabled 0\r\n"
    "  stp_state_filtering_enabled 0 vsi_filtering_enabled 0 disable_filtering 0\r\n"
    "  disable_learning 0 port_type 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(22), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_UNKNOWN_DA_UC_FILTER_ENABLED_ID,
    "unknown_da_uc_filter_enabled",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.unknown_da_uc_filter_enabled:\r\n"
    "  uint32 unknown_da_uc_filter_enabled\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_output_port_config1_tbl_set entry 0 mtu 0\r\n"
    "  default_snoop_command 0 default_action_profile 0 llvp_profile 0\r\n"
    "  port_cfg_ctpid_index 0 port_cfg_ctpid_valid 0 port_cfg_stpid_index 0\r\n"
    "  port_cfg_stpid_valid 0 outport_sys_port_id 0 outport_port_is_glag_member 0\r\n"
    "  mtu_check 0 split_horizon_filter_enabled 0 unknown_da_bc_filter_enabled 0\r\n"
    "  unknown_da_mc_filter_enabled 0 unknown_da_uc_filter_enabled 0\r\n"
    "  hair_pin_filtering_enabled 0 pep_acc_frm_types_filtering_enabled 0\r\n"
    "  stp_state_filtering_enabled 0 vsi_filtering_enabled 0 disable_filtering 0\r\n"
    "  disable_learning 0 port_type 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(22), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_HAIR_PIN_FILTERING_ENABLED_ID,
    "hair_pin_filtering_enabled",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.hair_pin_filtering_enabled:\r\n"
    "  uint32 hair_pin_filtering_enabled\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_output_port_config1_tbl_set entry 0 mtu 0\r\n"
    "  default_snoop_command 0 default_action_profile 0 llvp_profile 0\r\n"
    "  port_cfg_ctpid_index 0 port_cfg_ctpid_valid 0 port_cfg_stpid_index 0\r\n"
    "  port_cfg_stpid_valid 0 outport_sys_port_id 0 outport_port_is_glag_member 0\r\n"
    "  mtu_check 0 split_horizon_filter_enabled 0 unknown_da_bc_filter_enabled 0\r\n"
    "  unknown_da_mc_filter_enabled 0 unknown_da_uc_filter_enabled 0\r\n"
    "  hair_pin_filtering_enabled 0 pep_acc_frm_types_filtering_enabled 0\r\n"
    "  stp_state_filtering_enabled 0 vsi_filtering_enabled 0 disable_filtering 0\r\n"
    "  disable_learning 0 port_type 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(22), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_PEP_ACC_FRM_TYPES_FILTERING_ENABLED_ID,
    "pep_acc_frm_types_filtering_enabled",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.pep_acc_frm_types_filtering_enabled:\r\n"
    "  uint32 pep_acc_frm_types_filtering_enabled\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_output_port_config1_tbl_set entry 0 mtu 0\r\n"
    "  default_snoop_command 0 default_action_profile 0 llvp_profile 0\r\n"
    "  port_cfg_ctpid_index 0 port_cfg_ctpid_valid 0 port_cfg_stpid_index 0\r\n"
    "  port_cfg_stpid_valid 0 outport_sys_port_id 0 outport_port_is_glag_member 0\r\n"
    "  mtu_check 0 split_horizon_filter_enabled 0 unknown_da_bc_filter_enabled 0\r\n"
    "  unknown_da_mc_filter_enabled 0 unknown_da_uc_filter_enabled 0\r\n"
    "  hair_pin_filtering_enabled 0 pep_acc_frm_types_filtering_enabled 0\r\n"
    "  stp_state_filtering_enabled 0 vsi_filtering_enabled 0 disable_filtering 0\r\n"
    "  disable_learning 0 port_type 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(22), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_STP_STATE_FILTERING_ENABLED_ID,
    "stp_state_filtering_enabled",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.stp_state_filtering_enabled:\r\n"
    "  uint32 stp_state_filtering_enabled\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_output_port_config1_tbl_set entry 0 mtu 0\r\n"
    "  default_snoop_command 0 default_action_profile 0 llvp_profile 0\r\n"
    "  port_cfg_ctpid_index 0 port_cfg_ctpid_valid 0 port_cfg_stpid_index 0\r\n"
    "  port_cfg_stpid_valid 0 outport_sys_port_id 0 outport_port_is_glag_member 0\r\n"
    "  mtu_check 0 split_horizon_filter_enabled 0 unknown_da_bc_filter_enabled 0\r\n"
    "  unknown_da_mc_filter_enabled 0 unknown_da_uc_filter_enabled 0\r\n"
    "  hair_pin_filtering_enabled 0 pep_acc_frm_types_filtering_enabled 0\r\n"
    "  stp_state_filtering_enabled 0 vsi_filtering_enabled 0 disable_filtering 0\r\n"
    "  disable_learning 0 port_type 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(22), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_VSI_FILTERING_ENABLED_ID,
    "vsi_filtering_enabled",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.vsi_filtering_enabled:\r\n"
    "  uint32 vsi_filtering_enabled\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_output_port_config1_tbl_set entry 0 mtu 0\r\n"
    "  default_snoop_command 0 default_action_profile 0 llvp_profile 0\r\n"
    "  port_cfg_ctpid_index 0 port_cfg_ctpid_valid 0 port_cfg_stpid_index 0\r\n"
    "  port_cfg_stpid_valid 0 outport_sys_port_id 0 outport_port_is_glag_member 0\r\n"
    "  mtu_check 0 split_horizon_filter_enabled 0 unknown_da_bc_filter_enabled 0\r\n"
    "  unknown_da_mc_filter_enabled 0 unknown_da_uc_filter_enabled 0\r\n"
    "  hair_pin_filtering_enabled 0 pep_acc_frm_types_filtering_enabled 0\r\n"
    "  stp_state_filtering_enabled 0 vsi_filtering_enabled 0 disable_filtering 0\r\n"
    "  disable_learning 0 port_type 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(22), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_DISABLE_FILTERING_ID,
    "disable_filtering",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.disable_filtering:\r\n"
    "  uint32 disable_filtering\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_output_port_config1_tbl_set entry 0 mtu 0\r\n"
    "  default_snoop_command 0 default_action_profile 0 llvp_profile 0\r\n"
    "  port_cfg_ctpid_index 0 port_cfg_ctpid_valid 0 port_cfg_stpid_index 0\r\n"
    "  port_cfg_stpid_valid 0 outport_sys_port_id 0 outport_port_is_glag_member 0\r\n"
    "  mtu_check 0 split_horizon_filter_enabled 0 unknown_da_bc_filter_enabled 0\r\n"
    "  unknown_da_mc_filter_enabled 0 unknown_da_uc_filter_enabled 0\r\n"
    "  hair_pin_filtering_enabled 0 pep_acc_frm_types_filtering_enabled 0\r\n"
    "  stp_state_filtering_enabled 0 vsi_filtering_enabled 0 disable_filtering 0\r\n"
    "  disable_learning 0 port_type 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(22), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_DISABLE_LEARNING_ID,
    "disable_learning",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.disable_learning:\r\n"
    "  uint32 disable_learning\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_output_port_config1_tbl_set entry 0 mtu 0\r\n"
    "  default_snoop_command 0 default_action_profile 0 llvp_profile 0\r\n"
    "  port_cfg_ctpid_index 0 port_cfg_ctpid_valid 0 port_cfg_stpid_index 0\r\n"
    "  port_cfg_stpid_valid 0 outport_sys_port_id 0 outport_port_is_glag_member 0\r\n"
    "  mtu_check 0 split_horizon_filter_enabled 0 unknown_da_bc_filter_enabled 0\r\n"
    "  unknown_da_mc_filter_enabled 0 unknown_da_uc_filter_enabled 0\r\n"
    "  hair_pin_filtering_enabled 0 pep_acc_frm_types_filtering_enabled 0\r\n"
    "  stp_state_filtering_enabled 0 vsi_filtering_enabled 0 disable_filtering 0\r\n"
    "  disable_learning 0 port_type 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(22), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_SET_PP_OUTPUT_PORT_CONFIG1_TBL_SET_TBL_DATA_PORT_TYPE_ID,
    "port_type",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.port_type:\r\n"
    "  uint32 port_type\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_output_port_config1_tbl_set entry 0 mtu 0\r\n"
    "  default_snoop_command 0 default_action_profile 0 llvp_profile 0\r\n"
    "  port_cfg_ctpid_index 0 port_cfg_ctpid_valid 0 port_cfg_stpid_index 0\r\n"
    "  port_cfg_stpid_valid 0 outport_sys_port_id 0 outport_port_is_glag_member 0\r\n"
    "  mtu_check 0 split_horizon_filter_enabled 0 unknown_da_bc_filter_enabled 0\r\n"
    "  unknown_da_mc_filter_enabled 0 unknown_da_uc_filter_enabled 0\r\n"
    "  hair_pin_filtering_enabled 0 pep_acc_frm_types_filtering_enabled 0\r\n"
    "  stp_state_filtering_enabled 0 vsi_filtering_enabled 0 disable_filtering 0\r\n"
    "  disable_learning 0 port_type 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(22), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_GET_PP_OUTPUT_PORT_CONFIG1_TBL_GET_ID,
    "output_port_config1_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_output_port_config1_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(23), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_OUTPUT_PORT_CONFIG1_TBL_GET_PP_OUTPUT_PORT_CONFIG1_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_output_port_config1_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(23), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_ID,
    "ingress_vlan_edit_command_map_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ingress_vlan_edit_command_map_tbl_set\r\n"
    "  entry 0 bytes_to_remove 0 inner_tpid_source 0 inner_vid_source 0\r\n"
    "  inner_pcp_dei_source 0 outer_tpid_source 0 outer_vid_source 0\r\n"
    "  outer_pcp_dei_source 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(24), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ingress_vlan_edit_command_map_tbl_set\r\n"
    "  entry 0 bytes_to_remove 0 inner_tpid_source 0 inner_vid_source 0\r\n"
    "  inner_pcp_dei_source 0 outer_tpid_source 0 outer_vid_source 0\r\n"
    "  outer_pcp_dei_source 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(24), 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_BYTES_TO_REMOVE_ID,
    "bytes_to_remove",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.bytes_to_remove:\r\n"
    "  uint32 bytes_to_remove\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ingress_vlan_edit_command_map_tbl_set\r\n"
    "  entry 0 bytes_to_remove 0 inner_tpid_source 0 inner_vid_source 0\r\n"
    "  inner_pcp_dei_source 0 outer_tpid_source 0 outer_vid_source 0\r\n"
    "  outer_pcp_dei_source 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(24), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_INNER_TPID_SOURCE_ID,
    "inner_tpid_source",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.inner_tpid_source:\r\n"
    "  uint32 inner_tpid_source\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ingress_vlan_edit_command_map_tbl_set\r\n"
    "  entry 0 bytes_to_remove 0 inner_tpid_source 0 inner_vid_source 0\r\n"
    "  inner_pcp_dei_source 0 outer_tpid_source 0 outer_vid_source 0\r\n"
    "  outer_pcp_dei_source 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(24), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_INNER_VID_SOURCE_ID,
    "inner_vid_source",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.inner_vid_source:\r\n"
    "  uint32 inner_vid_source\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ingress_vlan_edit_command_map_tbl_set\r\n"
    "  entry 0 bytes_to_remove 0 inner_tpid_source 0 inner_vid_source 0\r\n"
    "  inner_pcp_dei_source 0 outer_tpid_source 0 outer_vid_source 0\r\n"
    "  outer_pcp_dei_source 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(24), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_INNER_PCP_DEI_SOURCE_ID,
    "inner_pcp_dei_source",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.inner_pcp_dei_source:\r\n"
    "  uint32 inner_pcp_dei_source\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ingress_vlan_edit_command_map_tbl_set\r\n"
    "  entry 0 bytes_to_remove 0 inner_tpid_source 0 inner_vid_source 0\r\n"
    "  inner_pcp_dei_source 0 outer_tpid_source 0 outer_vid_source 0\r\n"
    "  outer_pcp_dei_source 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(24), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_OUTER_TPID_SOURCE_ID,
    "outer_tpid_source",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.outer_tpid_source:\r\n"
    "  uint32 outer_tpid_source\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ingress_vlan_edit_command_map_tbl_set\r\n"
    "  entry 0 bytes_to_remove 0 inner_tpid_source 0 inner_vid_source 0\r\n"
    "  inner_pcp_dei_source 0 outer_tpid_source 0 outer_vid_source 0\r\n"
    "  outer_pcp_dei_source 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(24), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_OUTER_VID_SOURCE_ID,
    "outer_vid_source",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.outer_vid_source:\r\n"
    "  uint32 outer_vid_source\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ingress_vlan_edit_command_map_tbl_set\r\n"
    "  entry 0 bytes_to_remove 0 inner_tpid_source 0 inner_vid_source 0\r\n"
    "  inner_pcp_dei_source 0 outer_tpid_source 0 outer_vid_source 0\r\n"
    "  outer_pcp_dei_source 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(24), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_OUTER_PCP_DEI_SOURCE_ID,
    "outer_pcp_dei_source",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.outer_pcp_dei_source:\r\n"
    "  uint32 outer_pcp_dei_source\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ingress_vlan_edit_command_map_tbl_set\r\n"
    "  entry 0 bytes_to_remove 0 inner_tpid_source 0 inner_vid_source 0\r\n"
    "  inner_pcp_dei_source 0 outer_tpid_source 0 outer_vid_source 0\r\n"
    "  outer_pcp_dei_source 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(24), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_ID,
    "ingress_vlan_edit_command_map_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ingress_vlan_edit_command_map_tbl_get\r\n"
    "  entry 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(25), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_PP_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ingress_vlan_edit_command_map_tbl_get\r\n"
    "  entry 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(25), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_PROFILE1_TBL_SET_PP_EGRESS_MPLS_PROFILE1_TBL_SET_ID,
    "egress_mpls_profile1_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_profile1_tbl_set entry 0 cw 0\r\n"
    "  ttl 0 pipe 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(26), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_PROFILE1_TBL_SET_PP_EGRESS_MPLS_PROFILE1_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_profile1_tbl_set entry 0 cw 0\r\n"
    "  ttl 0 pipe 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(26), 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_PROFILE1_TBL_SET_PP_EGRESS_MPLS_PROFILE1_TBL_SET_TBL_DATA_CW_ID,
    "cw",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.cw:\r\n"
    "  uint32 cw\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_profile1_tbl_set entry 0 cw 0\r\n"
    "  ttl 0 pipe 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(26), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_PROFILE1_TBL_SET_PP_EGRESS_MPLS_PROFILE1_TBL_SET_TBL_DATA_TTL_ID,
    "ttl",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ttl:\r\n"
    "  uint32 ttl\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_profile1_tbl_set entry 0 cw 0\r\n"
    "  ttl 0 pipe 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(26), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_PROFILE1_TBL_SET_PP_EGRESS_MPLS_PROFILE1_TBL_SET_TBL_DATA_PIPE_ID,
    "pipe",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.pipe:\r\n"
    "  uint32 pipe\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_profile1_tbl_set entry 0 cw 0\r\n"
    "  ttl 0 pipe 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(26), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_PROFILE1_TBL_GET_PP_EGRESS_MPLS_PROFILE1_TBL_GET_ID,
    "egress_mpls_profile1_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_profile1_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(27), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_PROFILE1_TBL_GET_PP_EGRESS_MPLS_PROFILE1_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_profile1_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(27), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_PROFILE2_TBL_SET_PP_EGRESS_MPLS_PROFILE2_TBL_SET_ID,
    "egress_mpls_profile2_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_profile2_tbl_set entry 0 exp 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(28), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_PROFILE2_TBL_SET_PP_EGRESS_MPLS_PROFILE2_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_profile2_tbl_set entry 0 exp 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(28), 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_PROFILE2_TBL_SET_PP_EGRESS_MPLS_PROFILE2_TBL_SET_TBL_DATA_EXP_ID,
    "exp",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.exp:\r\n"
    "  uint32 exp\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_profile2_tbl_set entry 0 exp 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(28), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_PROFILE2_TBL_GET_PP_EGRESS_MPLS_PROFILE2_TBL_GET_ID,
    "egress_mpls_profile2_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_profile2_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(29), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_PROFILE2_TBL_GET_PP_EGRESS_MPLS_PROFILE2_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_profile2_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(29), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_ID,
    "egress_encapsulation_link_layer_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_encapsulation_link_layer_tbl_set\r\n"
    "  entry 0 out_pcp_valid 0 out_pcp 0 out_vid_valid 0 out_vid 0\r\n"
    "  out_tpid_index 0 dest_mac_index 0 dest_mac 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(30), 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_encapsulation_link_layer_tbl_set\r\n"
    "  entry 0 out_pcp_valid 0 out_pcp 0 out_vid_valid 0 out_vid 0\r\n"
    "  out_tpid_index 0 dest_mac_index 0 dest_mac 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(30), 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_TBL_DATA_OUT_PCP_VALID_ID,
    "out_pcp_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.out_pcp_valid:\r\n"
    "  uint32 out_pcp_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_encapsulation_link_layer_tbl_set\r\n"
    "  entry 0 out_pcp_valid 0 out_pcp 0 out_vid_valid 0 out_vid 0\r\n"
    "  out_tpid_index 0 dest_mac_index 0 dest_mac 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(30), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_TBL_DATA_OUT_PCP_ID,
    "out_pcp",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.out_pcp:\r\n"
    "  uint32 out_pcp\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_encapsulation_link_layer_tbl_set\r\n"
    "  entry 0 out_pcp_valid 0 out_pcp 0 out_vid_valid 0 out_vid 0\r\n"
    "  out_tpid_index 0 dest_mac_index 0 dest_mac 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(30), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_TBL_DATA_OUT_VID_VALID_ID,
    "out_vid_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.out_vid_valid:\r\n"
    "  uint32 out_vid_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_encapsulation_link_layer_tbl_set\r\n"
    "  entry 0 out_pcp_valid 0 out_pcp 0 out_vid_valid 0 out_vid 0\r\n"
    "  out_tpid_index 0 dest_mac_index 0 dest_mac 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(30), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_TBL_DATA_OUT_VID_ID,
    "out_vid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.out_vid:\r\n"
    "  uint32 out_vid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_encapsulation_link_layer_tbl_set\r\n"
    "  entry 0 out_pcp_valid 0 out_pcp 0 out_vid_valid 0 out_vid 0\r\n"
    "  out_tpid_index 0 dest_mac_index 0 dest_mac 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(30), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_TBL_DATA_OUT_TPID_INDEX_ID,
    "out_tpid_index",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.out_tpid_index:\r\n"
    "  uint32 out_tpid_index\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_encapsulation_link_layer_tbl_set\r\n"
    "  entry 0 out_pcp_valid 0 out_pcp 0 out_vid_valid 0 out_vid 0\r\n"
    "  out_tpid_index 0 dest_mac_index 0 dest_mac 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(30), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_TBL_DATA_DEST_MAC_INDEX_ID,
    "dest_mac_index",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  dest_mac_index:\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_encapsulation_link_layer_tbl_set\r\n"
    "  entry 0 out_pcp_valid 0 out_pcp 0 out_vid_valid 0 out_vid 0\r\n"
    "  out_tpid_index 0 dest_mac_index 0 dest_mac 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(30), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_SET_TBL_DATA_DEST_MAC_ID,
    "dest_mac",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.dest_mac[ prm_dest_mac_index]:\r\n"
    "  uint32 dest_mac[2]\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_encapsulation_link_layer_tbl_set\r\n"
    "  entry 0 out_pcp_valid 0 out_pcp 0 out_vid_valid 0 out_vid 0\r\n"
    "  out_tpid_index 0 dest_mac_index 0 dest_mac 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(30), 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_GET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_GET_ID,
    "egress_encapsulation_link_layer_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_encapsulation_link_layer_tbl_get\r\n"
    "  entry 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_GET_PP_EGRESS_ENCAPSULATION_LINK_LAYER_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_encapsulation_link_layer_tbl_get\r\n"
    "  entry 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_OUT_VSI_TBL_SET_PP_EGRESS_OUT_VSI_TBL_SET_ID,
    "egress_out_vsi_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_out_vsi_tbl_set entry 0 sa_lsb 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_OUT_VSI_TBL_SET_PP_EGRESS_OUT_VSI_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_out_vsi_tbl_set entry 0 sa_lsb 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(0)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_OUT_VSI_TBL_SET_PP_EGRESS_OUT_VSI_TBL_SET_TBL_DATA_SA_LSB_ID,
    "sa_lsb",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.sa_lsb:\r\n"
    "  uint32 sa_lsb\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_out_vsi_tbl_set entry 0 sa_lsb 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_OUT_VSI_TBL_GET_PP_EGRESS_OUT_VSI_TBL_GET_ID,
    "egress_out_vsi_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_out_vsi_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_OUT_VSI_TBL_GET_PP_EGRESS_OUT_VSI_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_out_vsi_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_LLVP_TABLE_TBL_SET_PP_EGRESS_LLVP_TABLE_TBL_SET_ID,
    "egress_llvp_table_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_llvp_table_tbl_set entry 0\r\n"
    "  incoming_tag_format 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(2)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_LLVP_TABLE_TBL_SET_PP_EGRESS_LLVP_TABLE_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_llvp_table_tbl_set entry 0\r\n"
    "  incoming_tag_format 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(2)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_LLVP_TABLE_TBL_SET_PP_EGRESS_LLVP_TABLE_TBL_SET_TBL_DATA_INCOMING_TAG_FORMAT_ID,
    "incoming_tag_format",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.incoming_tag_format:\r\n"
    "  uint32 incoming_tag_format\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_llvp_table_tbl_set entry 0\r\n"
    "  incoming_tag_format 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(2)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_LLVP_TABLE_TBL_GET_PP_EGRESS_LLVP_TABLE_TBL_GET_ID,
    "egress_llvp_table_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_llvp_table_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_LLVP_TABLE_TBL_GET_PP_EGRESS_LLVP_TABLE_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_llvp_table_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(3)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_ID,
    "egress_vlan_edit_command_map_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_command_map_tbl_set\r\n"
    "  incoming_tag_format_ndx 0 out_tag_profile_ndx 0 bytes_to_remove 0\r\n"
    "  inner_pcp_dei_source 0 outer_pcp_dei_source 0 inner_vid_source 0\r\n"
    "  outer_vid_source 0 inner_tpid_type 0 outer_tpid_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(4)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_INCOMING_TAG_FORMAT_NDX_ID,
    "incoming_tag_format_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  incoming_tag_format_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  incoming_tag_format_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_command_map_tbl_set\r\n"
    "  incoming_tag_format_ndx 0 out_tag_profile_ndx 0 bytes_to_remove 0\r\n"
    "  inner_pcp_dei_source 0 outer_pcp_dei_source 0 inner_vid_source 0\r\n"
    "  outer_vid_source 0 inner_tpid_type 0 outer_tpid_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(4)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_OUT_TAG_PROFILE_NDX_ID,
    "out_tag_profile_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  out_tag_profile_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  out_tag_profile_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_command_map_tbl_set\r\n"
    "  incoming_tag_format_ndx 0 out_tag_profile_ndx 0 bytes_to_remove 0\r\n"
    "  inner_pcp_dei_source 0 outer_pcp_dei_source 0 inner_vid_source 0\r\n"
    "  outer_vid_source 0 inner_tpid_type 0 outer_tpid_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(4)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_BYTES_TO_REMOVE_ID,
    "bytes_to_remove",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.bytes_to_remove:\r\n"
    "  uint32 bytes_to_remove\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_command_map_tbl_set\r\n"
    "  incoming_tag_format_ndx 0 out_tag_profile_ndx 0 bytes_to_remove 0\r\n"
    "  inner_pcp_dei_source 0 outer_pcp_dei_source 0 inner_vid_source 0\r\n"
    "  outer_vid_source 0 inner_tpid_type 0 outer_tpid_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_INNER_PCP_DEI_SOURCE_ID,
    "inner_pcp_dei_source",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.inner_pcp_dei_source:\r\n"
    "  uint32 inner_pcp_dei_source\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_command_map_tbl_set\r\n"
    "  incoming_tag_format_ndx 0 out_tag_profile_ndx 0 bytes_to_remove 0\r\n"
    "  inner_pcp_dei_source 0 outer_pcp_dei_source 0 inner_vid_source 0\r\n"
    "  outer_vid_source 0 inner_tpid_type 0 outer_tpid_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_OUTER_PCP_DEI_SOURCE_ID,
    "outer_pcp_dei_source",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.outer_pcp_dei_source:\r\n"
    "  uint32 outer_pcp_dei_source\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_command_map_tbl_set\r\n"
    "  incoming_tag_format_ndx 0 out_tag_profile_ndx 0 bytes_to_remove 0\r\n"
    "  inner_pcp_dei_source 0 outer_pcp_dei_source 0 inner_vid_source 0\r\n"
    "  outer_vid_source 0 inner_tpid_type 0 outer_tpid_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_INNER_VID_SOURCE_ID,
    "inner_vid_source",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.inner_vid_source:\r\n"
    "  uint32 inner_vid_source\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_command_map_tbl_set\r\n"
    "  incoming_tag_format_ndx 0 out_tag_profile_ndx 0 bytes_to_remove 0\r\n"
    "  inner_pcp_dei_source 0 outer_pcp_dei_source 0 inner_vid_source 0\r\n"
    "  outer_vid_source 0 inner_tpid_type 0 outer_tpid_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_OUTER_VID_SOURCE_ID,
    "outer_vid_source",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.outer_vid_source:\r\n"
    "  uint32 outer_vid_source\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_command_map_tbl_set\r\n"
    "  incoming_tag_format_ndx 0 out_tag_profile_ndx 0 bytes_to_remove 0\r\n"
    "  inner_pcp_dei_source 0 outer_pcp_dei_source 0 inner_vid_source 0\r\n"
    "  outer_vid_source 0 inner_tpid_type 0 outer_tpid_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_INNER_TPID_TYPE_ID,
    "inner_tpid_type",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.inner_tpid_type:\r\n"
    "  uint32 inner_tpid_type\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_command_map_tbl_set\r\n"
    "  incoming_tag_format_ndx 0 out_tag_profile_ndx 0 bytes_to_remove 0\r\n"
    "  inner_pcp_dei_source 0 outer_pcp_dei_source 0 inner_vid_source 0\r\n"
    "  outer_vid_source 0 inner_tpid_type 0 outer_tpid_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_TBL_DATA_OUTER_TPID_TYPE_ID,
    "outer_tpid_type",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.outer_tpid_type:\r\n"
    "  uint32 outer_tpid_type\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_command_map_tbl_set\r\n"
    "  incoming_tag_format_ndx 0 out_tag_profile_ndx 0 bytes_to_remove 0\r\n"
    "  inner_pcp_dei_source 0 outer_pcp_dei_source 0 inner_vid_source 0\r\n"
    "  outer_vid_source 0 inner_tpid_type 0 outer_tpid_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_ID,
    "egress_vlan_edit_command_map_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_command_map_tbl_get\r\n"
    "  incoming_tag_format_ndx 0 out_tag_profile_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(5)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_INCOMING_TAG_FORMAT_NDX_ID,
    "incoming_tag_format_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  incoming_tag_format_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  incoming_tag_format_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_command_map_tbl_get\r\n"
    "  incoming_tag_format_ndx 0 out_tag_profile_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(5)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_PP_EGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_OUT_TAG_PROFILE_NDX_ID,
    "out_tag_profile_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  out_tag_profile_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  out_tag_profile_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_command_map_tbl_get\r\n"
    "  incoming_tag_format_ndx 0 out_tag_profile_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(5)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_PCP_DEI_MAP_TBL_SET_PP_EGRESS_PCP_DEI_MAP_TBL_SET_ID,
    "egress_pcp_dei_map_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_pcp_dei_map_tbl_set entry 0 pcp_dei\r\n"
    "  0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(6)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_PCP_DEI_MAP_TBL_SET_PP_EGRESS_PCP_DEI_MAP_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_pcp_dei_map_tbl_set entry 0 pcp_dei\r\n"
    "  0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_PCP_DEI_MAP_TBL_SET_PP_EGRESS_PCP_DEI_MAP_TBL_SET_TBL_DATA_PCP_DEI_ID,
    "pcp_dei",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.pcp_dei:\r\n"
    "  uint32 pcp_dei\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_pcp_dei_map_tbl_set entry 0 pcp_dei\r\n"
    "  0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_PCP_DEI_MAP_TBL_GET_PP_EGRESS_PCP_DEI_MAP_TBL_GET_ID,
    "egress_pcp_dei_map_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_pcp_dei_map_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(7)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_PCP_DEI_MAP_TBL_GET_PP_EGRESS_PCP_DEI_MAP_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_pcp_dei_map_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_TC_DP_TO_UP_TBL_SET_PP_EGRESS_TC_DP_TO_UP_TBL_SET_ID,
    "egress_tc_dp_to_up_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_tc_dp_to_up_tbl_set entry 0\r\n"
    "  user_priority 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(8)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_TC_DP_TO_UP_TBL_SET_PP_EGRESS_TC_DP_TO_UP_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_tc_dp_to_up_tbl_set entry 0\r\n"
    "  user_priority 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(8)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_TC_DP_TO_UP_TBL_SET_PP_EGRESS_TC_DP_TO_UP_TBL_SET_TBL_DATA_USER_PRIORITY_ID,
    "user_priority",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.user_priority:\r\n"
    "  uint32 user_priority\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_tc_dp_to_up_tbl_set entry 0\r\n"
    "  user_priority 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_TC_DP_TO_UP_TBL_GET_PP_EGRESS_TC_DP_TO_UP_TBL_GET_ID,
    "egress_tc_dp_to_up_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_tc_dp_to_up_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(9)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_TC_DP_TO_UP_TBL_GET_PP_EGRESS_TC_DP_TO_UP_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_tc_dp_to_up_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(9)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_SET_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_SET_ID,
    "egress_port_vlan_membership_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_port_vlan_membership_tbl_set entry\r\n"
    "  0 port_vid_untagged 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(10)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_SET_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_port_vlan_membership_tbl_set entry\r\n"
    "  0 port_vid_untagged 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(10)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_SET_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_SET_TBL_DATA_PORT_VID_UNTAGGED_ID,
    "port_vid_untagged",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.port_vid_untagged:\r\n"
    "  uint32 port_vid_untagged\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_port_vlan_membership_tbl_set entry\r\n"
    "  0 port_vid_untagged 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_GET_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_GET_ID,
    "egress_port_vlan_membership_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_port_vlan_membership_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(11)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_GET_PP_EGRESS_PORT_VLAN_MEMBERSHIP_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_port_vlan_membership_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(11)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_STP_STATE_TBL_SET_PP_EGRESS_STP_STATE_TBL_SET_ID,
    "egress_stp_state_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_stp_state_tbl_set topology_id_ndx 0\r\n"
    "  outport_ndx 0 state_state 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(12)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_STP_STATE_TBL_SET_PP_EGRESS_STP_STATE_TBL_SET_TOPOLOGY_ID_NDX_ID,
    "topology_id_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  topology_id_ndx:\r\n"
    "  SOC_SAND_IN   uint32               topology_id_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_stp_state_tbl_set topology_id_ndx 0\r\n"
    "  outport_ndx 0 state_state 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(12)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_STP_STATE_TBL_SET_PP_EGRESS_STP_STATE_TBL_SET_OUTPORT_NDX_ID,
    "outport_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  outport_ndx:\r\n"
    "  SOC_SAND_IN   uint32               outport_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_stp_state_tbl_set topology_id_ndx 0\r\n"
    "  outport_ndx 0 state_state 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(12)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_STP_STATE_TBL_SET_PP_EGRESS_STP_STATE_TBL_SET_TBL_DATA_STATE_STATE_ID,
    "state_state",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.state_state:\r\n"
    "  uint32 state_state\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_stp_state_tbl_set topology_id_ndx 0\r\n"
    "  outport_ndx 0 state_state 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(12)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_STP_STATE_TBL_GET_PP_EGRESS_STP_STATE_TBL_GET_ID,
    "egress_stp_state_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_stp_state_tbl_get topology_id_ndx 0\r\n"
    "  outport_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(13)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_STP_STATE_TBL_GET_PP_EGRESS_STP_STATE_TBL_GET_TOPOLOGY_ID_NDX_ID,
    "topology_id_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  topology_id_ndx:\r\n"
    "  SOC_SAND_IN   uint32               topology_id_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_stp_state_tbl_get topology_id_ndx 0\r\n"
    "  outport_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(13)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_STP_STATE_TBL_GET_PP_EGRESS_STP_STATE_TBL_GET_OUTPORT_NDX_ID,
    "outport_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  outport_ndx:\r\n"
    "  SOC_SAND_IN   uint32               outport_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_stp_state_tbl_get topology_id_ndx 0\r\n"
    "  outport_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(13)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_SET_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_SET_ID,
    "egress_acceptable_frame_types_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_acceptable_frame_types_tbl_set vlan_format_ndx\r\n"
    "  0 pep_acc_profile_ndx 0 acceptable_frame_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(14)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_SET_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_SET_VLAN_FORMAT_NDX_ID,
    "vlan_format_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vlan_format_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32               vlan_format_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_acceptable_frame_types_tbl_set vlan_format_ndx\r\n"
    "  0 pep_acc_profile_ndx 0 acceptable_frame_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(14)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_SET_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_SET_PEP_ACC_PROFILE_NDX_ID,
    "pep_acc_profile_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  pep_acc_profile_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  pep_acc_profile_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_acceptable_frame_types_tbl_set vlan_format_ndx\r\n"
    "  0 pep_acc_profile_ndx 0 acceptable_frame_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(14)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_SET_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_SET_TBL_DATA_ACCEPTABLE_FRAME_TYPE_ID,
    "acceptable_frame_type",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.acceptable_frame_type:\r\n"
    "  uint32 acceptable_frame_type\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_acceptable_frame_types_tbl_set vlan_format_ndx\r\n"
    "  0 pep_acc_profile_ndx 0 acceptable_frame_type 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_GET_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_GET_ID,
    "egress_acceptable_frame_types_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_acceptable_frame_types_tbl_get vlan_format_ndx\r\n"
    "  0 pep_acc_profile_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(15)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_GET_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_GET_VLAN_FORMAT_NDX_ID,
    "vlan_format_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vlan_format_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32               vlan_format_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_acceptable_frame_types_tbl_get vlan_format_ndx\r\n"
    "  0 pep_acc_profile_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(15)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_GET_PP_EGRESS_ACCEPTABLE_FRAME_TYPES_TBL_GET_PEP_ACC_PROFILE_NDX_ID,
    "pep_acc_profile_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  pep_acc_profile_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  pep_acc_profile_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_acceptable_frame_types_tbl_get vlan_format_ndx\r\n"
    "  0 pep_acc_profile_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(15)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_ID,
    "egress_mep_level_profile_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mep_level_profile_tbl_set entry 0\r\n"
    "  level7 0 level6 0 level5 0 level4 0 level3 0 level2 0 level1 0 level0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(16)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mep_level_profile_tbl_set entry 0\r\n"
    "  level7 0 level6 0 level5 0 level4 0 level3 0 level2 0 level1 0 level0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(16)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL7_ID,
    "level7",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.level7:\r\n"
    "  uint32 level7\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mep_level_profile_tbl_set entry 0\r\n"
    "  level7 0 level6 0 level5 0 level4 0 level3 0 level2 0 level1 0 level0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL6_ID,
    "level6",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.level6:\r\n"
    "  uint32 level6\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mep_level_profile_tbl_set entry 0\r\n"
    "  level7 0 level6 0 level5 0 level4 0 level3 0 level2 0 level1 0 level0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL5_ID,
    "level5",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.level5:\r\n"
    "  uint32 level5\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mep_level_profile_tbl_set entry 0\r\n"
    "  level7 0 level6 0 level5 0 level4 0 level3 0 level2 0 level1 0 level0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL4_ID,
    "level4",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.level4:\r\n"
    "  uint32 level4\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mep_level_profile_tbl_set entry 0\r\n"
    "  level7 0 level6 0 level5 0 level4 0 level3 0 level2 0 level1 0 level0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL3_ID,
    "level3",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.level3:\r\n"
    "  uint32 level3\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mep_level_profile_tbl_set entry 0\r\n"
    "  level7 0 level6 0 level5 0 level4 0 level3 0 level2 0 level1 0 level0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL2_ID,
    "level2",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.level2:\r\n"
    "  uint32 level2\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mep_level_profile_tbl_set entry 0\r\n"
    "  level7 0 level6 0 level5 0 level4 0 level3 0 level2 0 level1 0 level0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL1_ID,
    "level1",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.level1:\r\n"
    "  uint32 level1\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mep_level_profile_tbl_set entry 0\r\n"
    "  level7 0 level6 0 level5 0 level4 0 level3 0 level2 0 level1 0 level0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_SET_TBL_DATA_LEVEL0_ID,
    "level0",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.level0:\r\n"
    "  uint32 level0\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mep_level_profile_tbl_set entry 0\r\n"
    "  level7 0 level6 0 level5 0 level4 0 level3 0 level2 0 level1 0 level0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_GET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_GET_ID,
    "egress_mep_level_profile_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mep_level_profile_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(17)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_GET_PP_EGRESS_MEP_LEVEL_PROFILE_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mep_level_profile_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_OAM_ACTION_TBL_SET_PP_EGRESS_OAM_ACTION_TBL_SET_ID,
    "egress_oam_action_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_oam_action_tbl_set entry 0\r\n"
    "  oam_action_profile 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(18)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_OAM_ACTION_TBL_SET_PP_EGRESS_OAM_ACTION_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_oam_action_tbl_set entry 0\r\n"
    "  oam_action_profile 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(18)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_OAM_ACTION_TBL_SET_PP_EGRESS_OAM_ACTION_TBL_SET_TBL_DATA_OAM_ACTION_PROFILE_ID,
    "action_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.oam_action_profile:\r\n"
    "  uint32 oam_action_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_oam_action_tbl_set entry 0\r\n"
    "  oam_action_profile 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_OAM_ACTION_TBL_GET_PP_EGRESS_OAM_ACTION_TBL_GET_ID,
    "egress_oam_action_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_oam_action_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(19)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_OAM_ACTION_TBL_GET_PP_EGRESS_OAM_ACTION_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_oam_action_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(19)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_ID,
    "egress_action_profile_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_action_profile_tbl_set entry 0\r\n"
    "  strength 0 dp_ow 0 dp_value 0 tc_ow 0 tc_value 0 destination 0 drop 0 control\r\n"
    "  0 trap 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(20)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_action_profile_tbl_set entry 0\r\n"
    "  strength 0 dp_ow 0 dp_value 0 tc_ow 0 tc_value 0 destination 0 drop 0 control\r\n"
    "  0 trap 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(20)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_STRENGTH_ID,
    "strength",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.strength:\r\n"
    "  uint32 strength\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_action_profile_tbl_set entry 0\r\n"
    "  strength 0 dp_ow 0 dp_value 0 tc_ow 0 tc_value 0 destination 0 drop 0 control\r\n"
    "  0 trap 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_DP_OW_ID,
    "dp_ow",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.dp_ow:\r\n"
    "  uint32 dp_ow\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_action_profile_tbl_set entry 0\r\n"
    "  strength 0 dp_ow 0 dp_value 0 tc_ow 0 tc_value 0 destination 0 drop 0 control\r\n"
    "  0 trap 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_DP_VALUE_ID,
    "dp_value",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.dp_value:\r\n"
    "  uint32 dp_value\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_action_profile_tbl_set entry 0\r\n"
    "  strength 0 dp_ow 0 dp_value 0 tc_ow 0 tc_value 0 destination 0 drop 0 control\r\n"
    "  0 trap 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_TC_OW_ID,
    "tc_ow",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.tc_ow:\r\n"
    "  uint32 tc_ow\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_action_profile_tbl_set entry 0\r\n"
    "  strength 0 dp_ow 0 dp_value 0 tc_ow 0 tc_value 0 destination 0 drop 0 control\r\n"
    "  0 trap 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_TC_VALUE_ID,
    "tc_value",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.tc_value:\r\n"
    "  uint32 tc_value\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_action_profile_tbl_set entry 0\r\n"
    "  strength 0 dp_ow 0 dp_value 0 tc_ow 0 tc_value 0 destination 0 drop 0 control\r\n"
    "  0 trap 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_DESTINATION_ID,
    "destination",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.destination:\r\n"
    "  uint32 destination\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_action_profile_tbl_set entry 0\r\n"
    "  strength 0 dp_ow 0 dp_value 0 tc_ow 0 tc_value 0 destination 0 drop 0 control\r\n"
    "  0 trap 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_DROP_ID,
    "drop",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.drop:\r\n"
    "  uint32 drop\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_action_profile_tbl_set entry 0\r\n"
    "  strength 0 dp_ow 0 dp_value 0 tc_ow 0 tc_value 0 destination 0 drop 0 control\r\n"
    "  0 trap 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_CONTROL_ID,
    "control",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.control:\r\n"
    "  uint32 control\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_action_profile_tbl_set entry 0\r\n"
    "  strength 0 dp_ow 0 dp_value 0 tc_ow 0 tc_value 0 destination 0 drop 0 control\r\n"
    "  0 trap 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_SET_PP_EGRESS_ACTION_PROFILE_TBL_SET_TBL_DATA_TRAP_ID,
    "trap",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.trap:\r\n"
    "  uint32 trap\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_action_profile_tbl_set entry 0\r\n"
    "  strength 0 dp_ow 0 dp_value 0 tc_ow 0 tc_value 0 destination 0 drop 0 control\r\n"
    "  0 trap 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_GET_PP_EGRESS_ACTION_PROFILE_TBL_GET_ID,
    "egress_action_profile_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_action_profile_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(21)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_ACTION_PROFILE_TBL_GET_PP_EGRESS_ACTION_PROFILE_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_action_profile_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(21)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_SNOOP_PROFILE_TBL_SET_PP_EGRESS_SNOOP_PROFILE_TBL_SET_ID,
    "egress_snoop_profile_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_snoop_profile_tbl_set entry 0\r\n"
    "  strength 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(22)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_SNOOP_PROFILE_TBL_SET_PP_EGRESS_SNOOP_PROFILE_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_snoop_profile_tbl_set entry 0\r\n"
    "  strength 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(22)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_SNOOP_PROFILE_TBL_SET_PP_EGRESS_SNOOP_PROFILE_TBL_SET_TBL_DATA_STRENGTH_ID,
    "strength",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.strength:\r\n"
    "  uint32 strength\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_snoop_profile_tbl_set entry 0\r\n"
    "  strength 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(22)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_SNOOP_PROFILE_TBL_GET_PP_EGRESS_SNOOP_PROFILE_TBL_GET_ID,
    "egress_snoop_profile_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_snoop_profile_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(23)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_SNOOP_PROFILE_TBL_GET_PP_EGRESS_SNOOP_PROFILE_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_snoop_profile_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(23)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FEC_LOW_TBL_SET_PP_FEC_LOW_TBL_SET_ID,
    "fec_low_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_fec_low_tbl_set entry 0 secondary_outlif 0\r\n"
    "  primary_outlif 0 secondary_destination 0 primary_destination 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(24)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FEC_LOW_TBL_SET_PP_FEC_LOW_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_fec_low_tbl_set entry 0 secondary_outlif 0\r\n"
    "  primary_outlif 0 secondary_destination 0 primary_destination 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(24)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FEC_LOW_TBL_SET_PP_FEC_LOW_TBL_SET_TBL_DATA_SECONDARY_OUTLIF_ID,
    "secondary_outlif",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.secondary_outlif:\r\n"
    "  uint32 secondary_outlif\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_fec_low_tbl_set entry 0 secondary_outlif 0\r\n"
    "  primary_outlif 0 secondary_destination 0 primary_destination 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(24)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FEC_LOW_TBL_SET_PP_FEC_LOW_TBL_SET_TBL_DATA_VALID,
    "valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.secondary_outlif:\r\n"
    "  uint32 secondary_outlif\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_fec_low_tbl_set entry 0 secondary_outlif 0\r\n"
    "  primary_outlif 0 secondary_destination 0 primary_destination 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(24)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },  
  {
    PARAM_T20E_PP_FEC_LOW_TBL_SET_PP_FEC_LOW_TBL_SET_TBL_DATA_PRIMARY_OUTLIF_ID,
    "primary_outlif",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.primary_outlif:\r\n"
    "  uint32 primary_outlif\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_fec_low_tbl_set entry 0 secondary_outlif 0\r\n"
    "  primary_outlif 0 secondary_destination 0 primary_destination 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(24)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FEC_LOW_TBL_SET_PP_FEC_LOW_TBL_SET_TBL_DATA_SECONDARY_DESTINATION_ID,
    "secondary_destination",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.secondary_destination:\r\n"
    "  uint32 secondary_destination\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_fec_low_tbl_set entry 0 secondary_outlif 0\r\n"
    "  primary_outlif 0 secondary_destination 0 primary_destination 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(24)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FEC_LOW_TBL_SET_PP_FEC_LOW_TBL_SET_TBL_DATA_PRIMARY_DESTINATION_ID,
    "primary_destination",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.primary_destination:\r\n"
    "  uint32 primary_destination\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_fec_low_tbl_set entry 0 secondary_outlif 0\r\n"
    "  primary_outlif 0 secondary_destination 0 primary_destination 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(24)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FEC_LOW_TBL_GET_PP_FEC_LOW_TBL_GET_ID,
    "fec_low_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_fec_low_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(25)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FEC_LOW_TBL_GET_PP_FEC_LOW_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_fec_low_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(25)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FEC_HIGH_TBL_SET_PP_FEC_HIGH_TBL_SET_ID,
    "fec_high_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_fec_high_tbl_set entry 0 protection_type 0\r\n"
    "  protection_instance 0 secondary_out_rif 0 primary_out_rif 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(26)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FEC_HIGH_TBL_SET_PP_FEC_HIGH_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_fec_high_tbl_set entry 0 protection_type 0\r\n"
    "  protection_instance 0 secondary_out_rif 0 primary_out_rif 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(26)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FEC_HIGH_TBL_SET_PP_FEC_HIGH_TBL_SET_TBL_DATA_PROTECTION_TYPE_ID,
    "protection_type",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.protection_type:\r\n"
    "  uint32 protection_type\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_fec_high_tbl_set entry 0 protection_type 0\r\n"
    "  protection_instance 0 secondary_out_rif 0 primary_out_rif 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FEC_HIGH_TBL_SET_PP_FEC_HIGH_TBL_SET_TBL_DATA_PROTECTION_INSTANCE_ID,
    "protection_instance",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.protection_instance:\r\n"
    "  uint32 protection_instance\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_fec_high_tbl_set entry 0 protection_type 0\r\n"
    "  protection_instance 0 secondary_out_rif 0 primary_out_rif 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FEC_HIGH_TBL_SET_PP_FEC_HIGH_TBL_SET_TBL_DATA_SECONDARY_OUT_RIF_ID,
    "secondary_out_rif",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.secondary_out_rif:\r\n"
    "  uint32 secondary_out_rif\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_fec_high_tbl_set entry 0 protection_type 0\r\n"
    "  protection_instance 0 secondary_out_rif 0 primary_out_rif 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FEC_HIGH_TBL_SET_PP_FEC_HIGH_TBL_SET_TBL_DATA_PRIMARY_OUT_RIF_ID,
    "primary_out_rif",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.primary_out_rif:\r\n"
    "  uint32 primary_out_rif\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_fec_high_tbl_set entry 0 protection_type 0\r\n"
    "  protection_instance 0 secondary_out_rif 0 primary_out_rif 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FEC_HIGH_TBL_GET_PP_FEC_HIGH_TBL_GET_ID,
    "fec_high_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_fec_high_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(27)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_FEC_HIGH_TBL_GET_PP_FEC_HIGH_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_fec_high_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(27)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_ID,
    "egress_mpls_ac_table_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_ac_table_tbl_set entry 0\r\n"
    "  counter_ptr 0 counter_valid 0 next_eep 0 has_cw 0 label 0 mpls_out_vsi 0\r\n"
    "  out_ac 0 out_vsi 0 mpls_command 0 ac_mpls_mvr 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(28)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_ac_table_tbl_set entry 0\r\n"
    "  counter_ptr 0 counter_valid 0 next_eep 0 has_cw 0 label 0 mpls_out_vsi 0\r\n"
    "  out_ac 0 out_vsi 0 mpls_command 0 ac_mpls_mvr 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(28)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_COUNTER_PTR_ID,
    "counter_ptr",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.counter_ptr:\r\n"
    "  uint32 counter_ptr\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_ac_table_tbl_set entry 0\r\n"
    "  counter_ptr 0 counter_valid 0 next_eep 0 has_cw 0 label 0 mpls_out_vsi 0\r\n"
    "  out_ac 0 out_vsi 0 mpls_command 0 ac_mpls_mvr 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_COUNTER_VALID_ID,
    "counter_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.counter_valid:\r\n"
    "  uint32 counter_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_ac_table_tbl_set entry 0\r\n"
    "  counter_ptr 0 counter_valid 0 next_eep 0 has_cw 0 label 0 mpls_out_vsi 0\r\n"
    "  out_ac 0 out_vsi 0 mpls_command 0 ac_mpls_mvr 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_NEXT_EEP_ID,
    "next_eep",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.next_eep:\r\n"
    "  uint32 next_eep\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_ac_table_tbl_set entry 0\r\n"
    "  counter_ptr 0 counter_valid 0 next_eep 0 has_cw 0 label 0 mpls_out_vsi 0\r\n"
    "  out_ac 0 out_vsi 0 mpls_command 0 ac_mpls_mvr 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_HAS_CW_ID,
    "has_cw",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.has_cw:\r\n"
    "  uint32 has_cw\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_ac_table_tbl_set entry 0\r\n"
    "  counter_ptr 0 counter_valid 0 next_eep 0 has_cw 0 label 0 mpls_out_vsi 0\r\n"
    "  out_ac 0 out_vsi 0 mpls_command 0 ac_mpls_mvr 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_LABEL_ID,
    "label",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.label:\r\n"
    "  uint32 label\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_ac_table_tbl_set entry 0\r\n"
    "  counter_ptr 0 counter_valid 0 next_eep 0 has_cw 0 label 0 mpls_out_vsi 0\r\n"
    "  out_ac 0 out_vsi 0 mpls_command 0 ac_mpls_mvr 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_MPLS_OUT_VSI_ID,
    "mpls_out_vsi",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mpls_out_vsi:\r\n"
    "  uint32 mpls_out_vsi\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_ac_table_tbl_set entry 0\r\n"
    "  counter_ptr 0 counter_valid 0 next_eep 0 has_cw 0 label 0 mpls_out_vsi 0\r\n"
    "  out_ac 0 out_vsi 0 mpls_command 0 ac_mpls_mvr 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_OUT_AC_ID,
    "out_ac",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.out_ac:\r\n"
    "  uint32 out_ac\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_ac_table_tbl_set entry 0\r\n"
    "  counter_ptr 0 counter_valid 0 next_eep 0 has_cw 0 label 0 mpls_out_vsi 0\r\n"
    "  out_ac 0 out_vsi 0 mpls_command 0 ac_mpls_mvr 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_OUT_VSI_ID,
    "out_vsi",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.out_vsi:\r\n"
    "  uint32 out_vsi\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_ac_table_tbl_set entry 0\r\n"
    "  counter_ptr 0 counter_valid 0 next_eep 0 has_cw 0 label 0 mpls_out_vsi 0\r\n"
    "  out_ac 0 out_vsi 0 mpls_command 0 ac_mpls_mvr 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_MPLS_COMMAND_ID,
    "mpls_command",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mpls_command:\r\n"
    "  uint32 mpls_command\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_ac_table_tbl_set entry 0\r\n"
    "  counter_ptr 0 counter_valid 0 next_eep 0 has_cw 0 label 0 mpls_out_vsi 0\r\n"
    "  out_ac 0 out_vsi 0 mpls_command 0 ac_mpls_mvr 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_PP_EGRESS_MPLS_AC_TABLE_TBL_SET_TBL_DATA_AC_MPLS_MVR_ID,
    "ac_mpls_mvr",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ac_mpls_mvr:\r\n"
    "  uint32 ac_mpls_mvr\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_ac_table_tbl_set entry 0\r\n"
    "  counter_ptr 0 counter_valid 0 next_eep 0 has_cw 0 label 0 mpls_out_vsi 0\r\n"
    "  out_ac 0 out_vsi 0 mpls_command 0 ac_mpls_mvr 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_GET_PP_EGRESS_MPLS_AC_TABLE_TBL_GET_ID,
    "egress_mpls_ac_table_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_ac_table_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(29)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_AC_TABLE_TBL_GET_PP_EGRESS_MPLS_AC_TABLE_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_ac_table_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(29)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_ID,
    "egress_ctag_translation_table_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_table_tbl_set\r\n"
    "  edit_profile_ndx 0 outport_ndx 0 cvid_ndx 0 counter_ptr 0 counter_valid 0\r\n"
    "  edit_up 0 edit_vid 0 out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mp_index_base_valid 0 mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(30)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_EDIT_PROFILE_NDX_ID,
    "edit_profile_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  edit_profile_ndx:\r\n"
    "  SOC_SAND_IN   uint32               edit_profile_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_table_tbl_set\r\n"
    "  edit_profile_ndx 0 outport_ndx 0 cvid_ndx 0 counter_ptr 0 counter_valid 0\r\n"
    "  edit_up 0 edit_vid 0 out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mp_index_base_valid 0 mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(30)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_OUTPORT_NDX_ID,
    "outport_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  outport_ndx:\r\n"
    "  SOC_SAND_IN   uint32               outport_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_table_tbl_set\r\n"
    "  edit_profile_ndx 0 outport_ndx 0 cvid_ndx 0 counter_ptr 0 counter_valid 0\r\n"
    "  edit_up 0 edit_vid 0 out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mp_index_base_valid 0 mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(30)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_IS_CEP_NDX_ID,
    "is_cep_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  is CEP port:\r\n"
    "  SOC_SAND_IN   uint32               is_cep\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_table_tbl_set\r\n"
    "  edit_profile_ndx 0 outport_ndx 0 cvid_ndx 0 counter_ptr 0 counter_valid 0\r\n"
    "  edit_up 0 edit_vid 0 out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mp_index_base_valid 0 mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(30)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_CVID_NDX_ID,
    "cvid_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  cvid_ndx:\r\n"
    "  SOC_SAND_IN   uint32               cvid_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_table_tbl_set\r\n"
    "  edit_profile_ndx 0 outport_ndx 0 cvid_ndx 0 counter_ptr 0 counter_valid 0\r\n"
    "  edit_up 0 edit_vid 0 out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mp_index_base_valid 0 mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(30)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_COUNTER_PTR_ID,
    "counter_ptr",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.counter_ptr:\r\n"
    "  uint32 counter_ptr\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_table_tbl_set\r\n"
    "  edit_profile_ndx 0 outport_ndx 0 cvid_ndx 0 counter_ptr 0 counter_valid 0\r\n"
    "  edit_up 0 edit_vid 0 out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mp_index_base_valid 0 mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(30)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_COUNTER_VALID_ID,
    "counter_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.counter_valid:\r\n"
    "  uint32 counter_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_table_tbl_set\r\n"
    "  edit_profile_ndx 0 outport_ndx 0 cvid_ndx 0 counter_ptr 0 counter_valid 0\r\n"
    "  edit_up 0 edit_vid 0 out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mp_index_base_valid 0 mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(30)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_EDIT_UP_ID,
    "edit_up",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.edit_up:\r\n"
    "  uint32 edit_up\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_table_tbl_set\r\n"
    "  edit_profile_ndx 0 outport_ndx 0 cvid_ndx 0 counter_ptr 0 counter_valid 0\r\n"
    "  edit_up 0 edit_vid 0 out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mp_index_base_valid 0 mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(30)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_EDIT_VID_ID,
    "edit_vid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.edit_vid:\r\n"
    "  uint32 edit_vid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_table_tbl_set\r\n"
    "  edit_profile_ndx 0 outport_ndx 0 cvid_ndx 0 counter_ptr 0 counter_valid 0\r\n"
    "  edit_up 0 edit_vid 0 out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mp_index_base_valid 0 mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(30)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_OUT_TAG_PROFILE_ID,
    "out_tag_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.out_tag_profile:\r\n"
    "  uint32 out_tag_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_table_tbl_set\r\n"
    "  edit_profile_ndx 0 outport_ndx 0 cvid_ndx 0 counter_ptr 0 counter_valid 0\r\n"
    "  edit_up 0 edit_vid 0 out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mp_index_base_valid 0 mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(30)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_MP_MAX_LEVEL_VALID_ID,
    "mp_max_level_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mp_max_level_valid:\r\n"
    "  uint32 mp_max_level_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_table_tbl_set\r\n"
    "  edit_profile_ndx 0 outport_ndx 0 cvid_ndx 0 counter_ptr 0 counter_valid 0\r\n"
    "  edit_up 0 edit_vid 0 out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mp_index_base_valid 0 mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(30)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_MP_MAX_LEVEL_ID,
    "mp_max_level",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mp_max_level:\r\n"
    "  uint32 mp_max_level\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_table_tbl_set\r\n"
    "  edit_profile_ndx 0 outport_ndx 0 cvid_ndx 0 counter_ptr 0 counter_valid 0\r\n"
    "  edit_up 0 edit_vid 0 out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mp_index_base_valid 0 mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(30)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_MP_INDEX_BASE_VALID_ID,
    "mp_index_base_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mp_index_base_valid:\r\n"
    "  uint32 mp_index_base_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_table_tbl_set\r\n"
    "  edit_profile_ndx 0 outport_ndx 0 cvid_ndx 0 counter_ptr 0 counter_valid 0\r\n"
    "  edit_up 0 edit_vid 0 out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mp_index_base_valid 0 mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(30)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_MP_INDEX_BASE_ID,
    "mp_index_base",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mp_index_base:\r\n"
    "  uint32 mp_index_base\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_table_tbl_set\r\n"
    "  edit_profile_ndx 0 outport_ndx 0 cvid_ndx 0 counter_ptr 0 counter_valid 0\r\n"
    "  edit_up 0 edit_vid 0 out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mp_index_base_valid 0 mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(30)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_STP_TOPOLOGY_ID_ID,
    "stp_topology_id",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.stp_topology_id:\r\n"
    "  uint32 stp_topology_id\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_table_tbl_set\r\n"
    "  edit_profile_ndx 0 outport_ndx 0 cvid_ndx 0 counter_ptr 0 counter_valid 0\r\n"
    "  edit_up 0 edit_vid 0 out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mp_index_base_valid 0 mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(30)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_SET_TBL_DATA_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.valid:\r\n"
    "  uint32 valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_table_tbl_set\r\n"
    "  edit_profile_ndx 0 outport_ndx 0 cvid_ndx 0 counter_ptr 0 counter_valid 0\r\n"
    "  edit_up 0 edit_vid 0 out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0\r\n"
    "  mp_index_base_valid 0 mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(30)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_ID,
    "egress_ctag_translation_table_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_table_tbl_get\r\n"
    "  edit_profile_ndx 0 outport_ndx 0 cvid_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(31)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_EDIT_PROFILE_NDX_ID,
    "edit_profile_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  edit_profile_ndx:\r\n"
    "  SOC_SAND_IN   uint32               edit_profile_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_table_tbl_get\r\n"
    "  edit_profile_ndx 0 outport_ndx 0 cvid_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(31)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_OUTPORT_NDX_ID,
    "outport_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  outport_ndx:\r\n"
    "  SOC_SAND_IN   uint32               outport_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_table_tbl_get\r\n"
    "  edit_profile_ndx 0 outport_ndx 0 cvid_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(31)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_IS_CEP_NDX_ID,
    "is_cep_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  is CEP port:\r\n"
    "  SOC_SAND_IN   uint32               is_cep\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_table_tbl_get\r\n"
    "  edit_profile_ndx 0 outport_ndx 0 cvid_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(31)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_PP_EGRESS_CTAG_TRANSLATION_TABLE_TBL_GET_CVID_NDX_ID,
    "cvid_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  cvid_ndx:\r\n"
    "  SOC_SAND_IN   uint32               cvid_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_table_tbl_get\r\n"
    "  edit_profile_ndx 0 outport_ndx 0 cvid_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, BIT(31)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_ID,
    "egress_ctag_translation_raw_table_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_raw_table_tbl_set\r\n"
    "  entry 0 counter_ptr 0 counter_valid 0 edit_up 0 edit_vid 0\r\n"
    "  out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0 mp_index_base_valid 0\r\n"
    "  mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN   uint32               entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_raw_table_tbl_set\r\n"
    "  entry 0 counter_ptr 0 counter_valid 0 edit_up 0 edit_vid 0\r\n"
    "  out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0 mp_index_base_valid 0\r\n"
    "  mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_ENTRY_IS_CEP_NDX_ID,
    "is_cep_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  is CEP port:\r\n"
    "  SOC_SAND_IN   uint32               is_cep\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_raw_table_tbl_set\r\n"
    "  entry 0 counter_ptr 0 counter_valid 0 edit_up 0 edit_vid 0\r\n"
    "  out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0 mp_index_base_valid 0\r\n"
    "  mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_COUNTER_PTR_ID,
    "counter_ptr",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.counter_ptr:\r\n"
    "  uint32 counter_ptr\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_raw_table_tbl_set\r\n"
    "  entry 0 counter_ptr 0 counter_valid 0 edit_up 0 edit_vid 0\r\n"
    "  out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0 mp_index_base_valid 0\r\n"
    "  mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_COUNTER_VALID_ID,
    "counter_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.counter_valid:\r\n"
    "  uint32 counter_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_raw_table_tbl_set\r\n"
    "  entry 0 counter_ptr 0 counter_valid 0 edit_up 0 edit_vid 0\r\n"
    "  out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0 mp_index_base_valid 0\r\n"
    "  mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_EDIT_UP_ID,
    "edit_up",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.edit_up:\r\n"
    "  uint32 edit_up\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_raw_table_tbl_set\r\n"
    "  entry 0 counter_ptr 0 counter_valid 0 edit_up 0 edit_vid 0\r\n"
    "  out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0 mp_index_base_valid 0\r\n"
    "  mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_EDIT_VID_ID,
    "edit_vid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.edit_vid:\r\n"
    "  uint32 edit_vid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_raw_table_tbl_set\r\n"
    "  entry 0 counter_ptr 0 counter_valid 0 edit_up 0 edit_vid 0\r\n"
    "  out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0 mp_index_base_valid 0\r\n"
    "  mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_OUT_TAG_PROFILE_ID,
    "out_tag_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.out_tag_profile:\r\n"
    "  uint32 out_tag_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_raw_table_tbl_set\r\n"
    "  entry 0 counter_ptr 0 counter_valid 0 edit_up 0 edit_vid 0\r\n"
    "  out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0 mp_index_base_valid 0\r\n"
    "  mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_MP_MAX_LEVEL_VALID_ID,
    "mp_max_level_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mp_max_level_valid:\r\n"
    "  uint32 mp_max_level_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_raw_table_tbl_set\r\n"
    "  entry 0 counter_ptr 0 counter_valid 0 edit_up 0 edit_vid 0\r\n"
    "  out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0 mp_index_base_valid 0\r\n"
    "  mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_MP_MAX_LEVEL_ID,
    "mp_max_level",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mp_max_level:\r\n"
    "  uint32 mp_max_level\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_raw_table_tbl_set\r\n"
    "  entry 0 counter_ptr 0 counter_valid 0 edit_up 0 edit_vid 0\r\n"
    "  out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0 mp_index_base_valid 0\r\n"
    "  mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_MP_INDEX_BASE_VALID_ID,
    "mp_index_base_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mp_index_base_valid:\r\n"
    "  uint32 mp_index_base_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_raw_table_tbl_set\r\n"
    "  entry 0 counter_ptr 0 counter_valid 0 edit_up 0 edit_vid 0\r\n"
    "  out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0 mp_index_base_valid 0\r\n"
    "  mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_MP_INDEX_BASE_ID,
    "mp_index_base",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mp_index_base:\r\n"
    "  uint32 mp_index_base\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_raw_table_tbl_set\r\n"
    "  entry 0 counter_ptr 0 counter_valid 0 edit_up 0 edit_vid 0\r\n"
    "  out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0 mp_index_base_valid 0\r\n"
    "  mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_STP_TOPOLOGY_ID_ID,
    "stp_topology_id",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.stp_topology_id:\r\n"
    "  uint32 stp_topology_id\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_raw_table_tbl_set\r\n"
    "  entry 0 counter_ptr 0 counter_valid 0 edit_up 0 edit_vid 0\r\n"
    "  out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0 mp_index_base_valid 0\r\n"
    "  mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_SET_TBL_DATA_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.valid:\r\n"
    "  uint32 valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_raw_table_tbl_set\r\n"
    "  entry 0 counter_ptr 0 counter_valid 0 edit_up 0 edit_vid 0\r\n"
    "  out_tag_profile 0 mp_max_level_valid 0 mp_max_level 0 mp_index_base_valid 0\r\n"
    "  mp_index_base 0 stp_topology_id 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_GET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_GET_ID,
    "egress_ctag_translation_raw_table_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_raw_table_tbl_get\r\n"
    "  entry 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_GET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN   uint32               entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_raw_table_tbl_get\r\n"
    "  entry 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_GET_PP_EGRESS_CTAG_TRANSLATION_RAW_TABLE_TBL_GET_ENTRY_IS_CEP_NDX_ID,
    "is_cep_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  is CEP port:\r\n"
    "  SOC_SAND_IN   uint32               is_cep\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_ctag_translation_raw_table_tbl_get\r\n"
    "  entry 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_ID,
    "egress_vlan_edit_and_membership_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_set vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0 outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0\r\n"
    "  pep_edit_profile 0 pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0\r\n"
    "  pcp_dei_profile 0 out_tag_profile 0 counter_ptr 0 counter_valid 0\r\n"
    "  mp_max_level 0 mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0\r\n"
    "  valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_VSI_NDX_ID,
    "vsi_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vsi_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  vsi_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_set vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0 outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0\r\n"
    "  pep_edit_profile 0 pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0\r\n"
    "  pcp_dei_profile 0 out_tag_profile 0 counter_ptr 0 counter_valid 0\r\n"
    "  mp_max_level 0 mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0\r\n"
    "  valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PORT_NDX_ID,
    "port_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  port_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_set vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0 outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0\r\n"
    "  pep_edit_profile 0 pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0\r\n"
    "  pcp_dei_profile 0 out_tag_profile 0 counter_ptr 0 counter_valid 0\r\n"
    "  mp_max_level 0 mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0\r\n"
    "  valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_IS_CEP_NDX_ID,
    "is_cep_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  is CEP port:\r\n"
    "  SOC_SAND_IN   uint32               is_cep\r\n"
    "",

#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_set vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0 outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0\r\n"
    "  pep_edit_profile 0 pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0\r\n"
    "  pcp_dei_profile 0 out_tag_profile 0 counter_ptr 0 counter_valid 0\r\n"
    "  mp_max_level 0 mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0\r\n"
    "  valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_ACC_MODE_NDX_ID,
    "acc_mode_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acc_mode_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  acc_mode_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_set vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0 outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0\r\n"
    "  pep_edit_profile 0 pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0\r\n"
    "  pcp_dei_profile 0 out_tag_profile 0 counter_ptr 0 counter_valid 0\r\n"
    "  mp_max_level 0 mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0\r\n"
    "  valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(2)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_OUTER_EDIT_TAG_ID,
    "outer_edit_vid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.outer_edit_tag:\r\n"
    "  uint32 outer_edit_tag\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_set vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0 outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0\r\n"
    "  pep_edit_profile 0 pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0\r\n"
    "  pcp_dei_profile 0 out_tag_profile 0 counter_ptr 0 counter_valid 0\r\n"
    "  mp_max_level 0 mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0\r\n"
    "  valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_STP_TOPOLOGY_ID_ID,
    "stp_topology_id",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.stp_topology_id:\r\n"
    "  uint32 stp_topology_id\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_set vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0 outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0\r\n"
    "  pep_edit_profile 0 pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0\r\n"
    "  pcp_dei_profile 0 out_tag_profile 0 counter_ptr 0 counter_valid 0\r\n"
    "  mp_max_level 0 mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0\r\n"
    "  valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_INNER_EDIT_TAG_ID,
    "inner_edit_vid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.inner_edit_tag:\r\n"
    "  uint32 inner_edit_tag\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_set vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0 outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0\r\n"
    "  pep_edit_profile 0 pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0\r\n"
    "  pcp_dei_profile 0 out_tag_profile 0 counter_ptr 0 counter_valid 0\r\n"
    "  mp_max_level 0 mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0\r\n"
    "  valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_PEP_EDIT_PROFILE_ID,
    "pep_edit_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.pep_edit_profile:\r\n"
    "  uint32 pep_edit_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_set vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0 outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0\r\n"
    "  pep_edit_profile 0 pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0\r\n"
    "  pcp_dei_profile 0 out_tag_profile 0 counter_ptr 0 counter_valid 0\r\n"
    "  mp_max_level 0 mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0\r\n"
    "  valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_PEP_COS_PROFILE_ID,
    "pep_cos_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.pep_cos_profile:\r\n"
    "  uint32 pep_cos_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_set vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0 outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0\r\n"
    "  pep_edit_profile 0 pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0\r\n"
    "  pcp_dei_profile 0 out_tag_profile 0 counter_ptr 0 counter_valid 0\r\n"
    "  mp_max_level 0 mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0\r\n"
    "  valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_PEP_ACC_FRM_TYPES_ID,
    "pep_acc_frm_types",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.pep_acc_frm_types:\r\n"
    "  uint32 pep_acc_frm_types\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_set vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0 outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0\r\n"
    "  pep_edit_profile 0 pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0\r\n"
    "  pcp_dei_profile 0 out_tag_profile 0 counter_ptr 0 counter_valid 0\r\n"
    "  mp_max_level 0 mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0\r\n"
    "  valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_PEP_PCP_ID,
    "pep_pcp",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.pep_pcp:\r\n"
    "  uint32 pep_pcp\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_set vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0 outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0\r\n"
    "  pep_edit_profile 0 pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0\r\n"
    "  pcp_dei_profile 0 out_tag_profile 0 counter_ptr 0 counter_valid 0\r\n"
    "  mp_max_level 0 mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0\r\n"
    "  valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_PEP_PVID_ID,
    "pep_pvid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.pep_pvid:\r\n"
    "  uint32 pep_pvid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_set vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0 outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0\r\n"
    "  pep_edit_profile 0 pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0\r\n"
    "  pcp_dei_profile 0 out_tag_profile 0 counter_ptr 0 counter_valid 0\r\n"
    "  mp_max_level 0 mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0\r\n"
    "  valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_PCP_DEI_PROFILE_ID,
    "pcp_dei_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.pcp_dei_profile:\r\n"
    "  uint32 pcp_dei_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_set vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0 outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0\r\n"
    "  pep_edit_profile 0 pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0\r\n"
    "  pcp_dei_profile 0 out_tag_profile 0 counter_ptr 0 counter_valid 0\r\n"
    "  mp_max_level 0 mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0\r\n"
    "  valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_OUT_TAG_PROFILE_ID,
    "out_tag_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.out_tag_profile:\r\n"
    "  uint32 out_tag_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_set vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0 outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0\r\n"
    "  pep_edit_profile 0 pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0\r\n"
    "  pcp_dei_profile 0 out_tag_profile 0 counter_ptr 0 counter_valid 0\r\n"
    "  mp_max_level 0 mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0\r\n"
    "  valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_COUNTER_PTR_ID,
    "counter_ptr",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.counter_ptr:\r\n"
    "  uint32 counter_ptr\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_set vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0 outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0\r\n"
    "  pep_edit_profile 0 pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0\r\n"
    "  pcp_dei_profile 0 out_tag_profile 0 counter_ptr 0 counter_valid 0\r\n"
    "  mp_max_level 0 mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0\r\n"
    "  valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_COUNTER_VALID_ID,
    "counter_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.counter_valid:\r\n"
    "  uint32 counter_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_set vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0 outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0\r\n"
    "  pep_edit_profile 0 pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0\r\n"
    "  pcp_dei_profile 0 out_tag_profile 0 counter_ptr 0 counter_valid 0\r\n"
    "  mp_max_level 0 mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0\r\n"
    "  valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_MP_MAX_LEVEL_ID,
    "mp_max_level",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mp_max_level:\r\n"
    "  uint32 mp_max_level\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_set vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0 outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0\r\n"
    "  pep_edit_profile 0 pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0\r\n"
    "  pcp_dei_profile 0 out_tag_profile 0 counter_ptr 0 counter_valid 0\r\n"
    "  mp_max_level 0 mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0\r\n"
    "  valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_MP_MAX_LEVEL_VALID_ID,
    "mp_max_level_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mp_max_level_valid:\r\n"
    "  uint32 mp_max_level_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_set vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0 outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0\r\n"
    "  pep_edit_profile 0 pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0\r\n"
    "  pcp_dei_profile 0 out_tag_profile 0 counter_ptr 0 counter_valid 0\r\n"
    "  mp_max_level 0 mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0\r\n"
    "  valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_MP_INDEX_BASE_ID,
    "mp_index_base",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mp_index_base:\r\n"
    "  uint32 mp_index_base\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_set vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0 outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0\r\n"
    "  pep_edit_profile 0 pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0\r\n"
    "  pcp_dei_profile 0 out_tag_profile 0 counter_ptr 0 counter_valid 0\r\n"
    "  mp_max_level 0 mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0\r\n"
    "  valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_MP_INDEX_BASE_VALID_ID,
    "mp_index_base_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mp_index_base_valid:\r\n"
    "  uint32 mp_index_base_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_set vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0 outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0\r\n"
    "  pep_edit_profile 0 pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0\r\n"
    "  pcp_dei_profile 0 out_tag_profile 0 counter_ptr 0 counter_valid 0\r\n"
    "  mp_max_level 0 mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0\r\n"
    "  valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_SET_TBL_DATA_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.valid:\r\n"
    "  uint32 valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_set vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0 outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0\r\n"
    "  pep_edit_profile 0 pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0\r\n"
    "  pcp_dei_profile 0 out_tag_profile 0 counter_ptr 0 counter_valid 0\r\n"
    "  mp_max_level 0 mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0\r\n"
    "  valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_ID,
    "egress_vlan_edit_and_membership_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_get vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_VSI_NDX_ID,
    "vsi_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  vsi_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  vsi_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_get vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_PORT_NDX_ID,
    "port_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  port_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  port_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_get vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_IS_CEP_NDX_ID,
    "is_cep_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  is CEP port:\r\n"
    "  SOC_SAND_IN   uint32               is_cep\r\n"
    "",

#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_get vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_TBL_GET_ACC_MODE_NDX_ID,
    "acc_mode_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  acc_mode_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  acc_mode_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_tbl_get vsi_ndx 0\r\n"
    "  port_ndx 0 acc_mode_ndx 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_ID,
    "egress_vlan_edit_and_membership_raw_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_raw_tbl_set ac_ndx 0\r\n"
    "  outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0 pep_edit_profile 0\r\n"
    "  pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0 pcp_dei_profile 0\r\n"
    "  out_tag_profile 0 counter_ptr 0 counter_valid 0 mp_max_level 0\r\n"
    "  mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_AC_NDX_ID,
    "ac_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ac_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  ac_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_raw_tbl_set ac_ndx 0\r\n"
    "  outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0 pep_edit_profile 0\r\n"
    "  pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0 pcp_dei_profile 0\r\n"
    "  out_tag_profile 0 counter_ptr 0 counter_valid 0 mp_max_level 0\r\n"
    "  mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_IS_CEP_NDX_ID,
      "is_cep_ndx",
      (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
      (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "  is CEP port:\r\n"
      "  SOC_SAND_IN   uint32               is_cep\r\n"
      "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_raw_tbl_set ac_ndx 0\r\n"
    "  outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0 pep_edit_profile 0\r\n"
    "  pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0 pcp_dei_profile 0\r\n"
    "  out_tag_profile 0 counter_ptr 0 counter_valid 0 mp_max_level 0\r\n"
    "  mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_OUTER_EDIT_TAG_ID,
    "outer_edit_tag",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.outer_edit_tag:\r\n"
    "  uint32 outer_edit_tag\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_raw_tbl_set ac_ndx 0\r\n"
    "  outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0 pep_edit_profile 0\r\n"
    "  pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0 pcp_dei_profile 0\r\n"
    "  out_tag_profile 0 counter_ptr 0 counter_valid 0 mp_max_level 0\r\n"
    "  mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_STP_TOPOLOGY_ID_ID,
    "stp_topology_id",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.stp_topology_id:\r\n"
    "  uint32 stp_topology_id\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_raw_tbl_set ac_ndx 0\r\n"
    "  outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0 pep_edit_profile 0\r\n"
    "  pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0 pcp_dei_profile 0\r\n"
    "  out_tag_profile 0 counter_ptr 0 counter_valid 0 mp_max_level 0\r\n"
    "  mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_INNER_EDIT_TAG_ID,
    "inner_edit_tag",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.inner_edit_tag:\r\n"
    "  uint32 inner_edit_tag\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_raw_tbl_set ac_ndx 0\r\n"
    "  outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0 pep_edit_profile 0\r\n"
    "  pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0 pcp_dei_profile 0\r\n"
    "  out_tag_profile 0 counter_ptr 0 counter_valid 0 mp_max_level 0\r\n"
    "  mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_PEP_EDIT_PROFILE_ID,
    "pep_edit_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.pep_edit_profile:\r\n"
    "  uint32 pep_edit_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_raw_tbl_set ac_ndx 0\r\n"
    "  outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0 pep_edit_profile 0\r\n"
    "  pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0 pcp_dei_profile 0\r\n"
    "  out_tag_profile 0 counter_ptr 0 counter_valid 0 mp_max_level 0\r\n"
    "  mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_PEP_COS_PROFILE_ID,
    "pep_cos_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.pep_cos_profile:\r\n"
    "  uint32 pep_cos_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_raw_tbl_set ac_ndx 0\r\n"
    "  outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0 pep_edit_profile 0\r\n"
    "  pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0 pcp_dei_profile 0\r\n"
    "  out_tag_profile 0 counter_ptr 0 counter_valid 0 mp_max_level 0\r\n"
    "  mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_PEP_ACC_FRM_TYPES_ID,
    "pep_acc_frm_types",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.pep_acc_frm_types:\r\n"
    "  uint32 pep_acc_frm_types\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_raw_tbl_set ac_ndx 0\r\n"
    "  outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0 pep_edit_profile 0\r\n"
    "  pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0 pcp_dei_profile 0\r\n"
    "  out_tag_profile 0 counter_ptr 0 counter_valid 0 mp_max_level 0\r\n"
    "  mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_PEP_PCP_ID,
    "pep_pcp",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.pep_pcp:\r\n"
    "  uint32 pep_pcp\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_raw_tbl_set ac_ndx 0\r\n"
    "  outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0 pep_edit_profile 0\r\n"
    "  pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0 pcp_dei_profile 0\r\n"
    "  out_tag_profile 0 counter_ptr 0 counter_valid 0 mp_max_level 0\r\n"
    "  mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_PEP_PVID_ID,
    "pep_pvid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.pep_pvid:\r\n"
    "  uint32 pep_pvid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_raw_tbl_set ac_ndx 0\r\n"
    "  outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0 pep_edit_profile 0\r\n"
    "  pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0 pcp_dei_profile 0\r\n"
    "  out_tag_profile 0 counter_ptr 0 counter_valid 0 mp_max_level 0\r\n"
    "  mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_PCP_DEI_PROFILE_ID,
    "pcp_dei_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.pcp_dei_profile:\r\n"
    "  uint32 pcp_dei_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_raw_tbl_set ac_ndx 0\r\n"
    "  outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0 pep_edit_profile 0\r\n"
    "  pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0 pcp_dei_profile 0\r\n"
    "  out_tag_profile 0 counter_ptr 0 counter_valid 0 mp_max_level 0\r\n"
    "  mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_OUT_TAG_PROFILE_ID,
    "out_tag_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.out_tag_profile:\r\n"
    "  uint32 out_tag_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_raw_tbl_set ac_ndx 0\r\n"
    "  outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0 pep_edit_profile 0\r\n"
    "  pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0 pcp_dei_profile 0\r\n"
    "  out_tag_profile 0 counter_ptr 0 counter_valid 0 mp_max_level 0\r\n"
    "  mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_COUNTER_PTR_ID,
    "counter_ptr",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.counter_ptr:\r\n"
    "  uint32 counter_ptr\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_raw_tbl_set ac_ndx 0\r\n"
    "  outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0 pep_edit_profile 0\r\n"
    "  pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0 pcp_dei_profile 0\r\n"
    "  out_tag_profile 0 counter_ptr 0 counter_valid 0 mp_max_level 0\r\n"
    "  mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_COUNTER_VALID_ID,
    "counter_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.counter_valid:\r\n"
    "  uint32 counter_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_raw_tbl_set ac_ndx 0\r\n"
    "  outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0 pep_edit_profile 0\r\n"
    "  pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0 pcp_dei_profile 0\r\n"
    "  out_tag_profile 0 counter_ptr 0 counter_valid 0 mp_max_level 0\r\n"
    "  mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_MP_MAX_LEVEL_ID,
    "mp_max_level",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mp_max_level:\r\n"
    "  uint32 mp_max_level\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_raw_tbl_set ac_ndx 0\r\n"
    "  outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0 pep_edit_profile 0\r\n"
    "  pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0 pcp_dei_profile 0\r\n"
    "  out_tag_profile 0 counter_ptr 0 counter_valid 0 mp_max_level 0\r\n"
    "  mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_MP_MAX_LEVEL_VALID_ID,
    "mp_max_level_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mp_max_level_valid:\r\n"
    "  uint32 mp_max_level_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_raw_tbl_set ac_ndx 0\r\n"
    "  outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0 pep_edit_profile 0\r\n"
    "  pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0 pcp_dei_profile 0\r\n"
    "  out_tag_profile 0 counter_ptr 0 counter_valid 0 mp_max_level 0\r\n"
    "  mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_MP_INDEX_BASE_ID,
    "mp_index_base",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mp_index_base:\r\n"
    "  uint32 mp_index_base\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_raw_tbl_set ac_ndx 0\r\n"
    "  outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0 pep_edit_profile 0\r\n"
    "  pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0 pcp_dei_profile 0\r\n"
    "  out_tag_profile 0 counter_ptr 0 counter_valid 0 mp_max_level 0\r\n"
    "  mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_MP_INDEX_BASE_VALID_ID,
    "mp_index_base_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mp_index_base_valid:\r\n"
    "  uint32 mp_index_base_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_raw_tbl_set ac_ndx 0\r\n"
    "  outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0 pep_edit_profile 0\r\n"
    "  pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0 pcp_dei_profile 0\r\n"
    "  out_tag_profile 0 counter_ptr 0 counter_valid 0 mp_max_level 0\r\n"
    "  mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_SET_TBL_DATA_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.valid:\r\n"
    "  uint32 valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_raw_tbl_set ac_ndx 0\r\n"
    "  outer_edit_tag 0 stp_topology_id 0 inner_edit_tag 0 pep_edit_profile 0\r\n"
    "  pep_cos_profile 0 pep_acc_frm_types 0 pep_pcp 0 pep_pvid 0 pcp_dei_profile 0\r\n"
    "  out_tag_profile 0 counter_ptr 0 counter_valid 0 mp_max_level 0\r\n"
    "  mp_max_level_valid 0 mp_index_base 0 mp_index_base_valid 0 valid 0\r\n"
#endif
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
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_GET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_GET_ID,
    "egress_vlan_edit_and_membership_raw_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_raw_tbl_get ac_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(5)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_GET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_GET_AC_NDX_ID,
    "ac_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ac_ndx:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  ac_ndx\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_raw_tbl_get ac_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(5)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_GET_PP_EGRESS_VLAN_EDIT_AND_MEMBERSHIP_RAW_TBL_GET_IS_CEP_NDX_ID,
    "is_cep_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  is CEP port:\r\n"
    "  SOC_SAND_IN   uint32               is_cep\r\n"
    "",

#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_vlan_edit_and_membership_raw_tbl_get ac_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(5)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_LOW_TBL_SET_PP_AC_LOW_TBL_SET_ID,
    "ac_low_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_low_tbl_set entry 0 vlan_edit_tag 0\r\n"
    "  learn_destination 0 unknown_frwd_dst 0 asd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(6)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_LOW_TBL_SET_PP_AC_LOW_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_low_tbl_set entry 0 vlan_edit_tag 0\r\n"
    "  learn_destination 0 unknown_frwd_dst 0 asd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_LOW_TBL_SET_PP_AC_LOW_TBL_SET_TBL_DATA_VLAN_EDIT_TAG_ID,
    "vlan_edit_vid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.vlan_edit_tag:\r\n"
    "  uint32 vlan_edit_tag\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_low_tbl_set entry 0 vlan_edit_tag 0\r\n"
    "  learn_destination 0 unknown_frwd_dst 0 asd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_LOW_TBL_SET_PP_AC_LOW_TBL_SET_TBL_DATA_VLAN_EDIT_TAG_PCP_ID,
    "vlan_edit_pcp_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.vlan_edit_tag:\r\n"
    "  uint32 vlan_edit_tag\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_low_tbl_set entry 0 vlan_edit_tag 0\r\n"
    "  learn_destination 0 unknown_frwd_dst 0 asd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_LOW_TBL_SET_PP_AC_LOW_TBL_SET_TBL_DATA_LEARN_DESTINATION_ID,
    "learn_destination",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.learn_destination:\r\n"
    "  uint32 learn_destination\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_low_tbl_set entry 0 vlan_edit_tag 0\r\n"
    "  learn_destination 0 unknown_frwd_dst 0 asd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_LOW_TBL_SET_PP_AC_LOW_TBL_SET_TBL_DATA_UNKNOWN_FRWD_DEST_ID,
    "unknown_frwd_dst",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.unknown_frwd_dst:\r\n"
    "  uint32 unknown_frwd_dst\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_low_tbl_set entry 0 vlan_edit_tag 0\r\n"
    "  learn_destination 0 unknown_frwd_dst 0 asd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_LOW_TBL_SET_PP_AC_LOW_TBL_SET_TBL_DATA_ASD_ID,
      "unknown_frwd_asd",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.asd:\r\n"
    "  uint32 asd\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_low_tbl_set entry 0 vlan_edit_tag 0\r\n"
    "  learn_destination 0 unknown_frwd_dst 0 asd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_LOW_TBL_GET_PP_AC_LOW_TBL_GET_ID,
    "ac_low_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_low_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(7)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_LOW_TBL_GET_PP_AC_LOW_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_low_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_ID,
    "ac_high_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_high_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0\r\n"
    "  link_layer_termination 0 learn_enable 0 vlan_edit_profile 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(8)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_high_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0\r\n"
    "  link_layer_termination 0 learn_enable 0 vlan_edit_profile 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(8)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.valid:\r\n"
    "  uint32 valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_high_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0\r\n"
    "  link_layer_termination 0 learn_enable 0 vlan_edit_profile 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_COUNTER_OR_STAT_PTR_ID,
    "counter_or_stat_ptr",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.counter_or_stat_ptr:\r\n"
    "  uint32 counter_or_stat_ptr\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_high_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0\r\n"
    "  link_layer_termination 0 learn_enable 0 vlan_edit_profile 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_POLICER_PTR_ID,
    "policer_ptr",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.policer_ptr:\r\n"
    "  uint32 policer_ptr\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_high_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0\r\n"
    "  link_layer_termination 0 learn_enable 0 vlan_edit_profile 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_STATISTICS_TAG_ENABLE_ID,
    "statistics_tag_enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.statistics_tag_enable:\r\n"
    "  uint32 statistics_tag_enable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_high_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0\r\n"
    "  link_layer_termination 0 learn_enable 0 vlan_edit_profile 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_COUNTERS_ENABLE_ID,
    "counters_enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.counters_enable:\r\n"
    "  uint32 counters_enable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_high_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0\r\n"
    "  link_layer_termination 0 learn_enable 0 vlan_edit_profile 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_POLICER_ENABLE_ID,
    "policer_enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.policer_enable:\r\n"
    "  uint32 policer_enable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_high_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0\r\n"
    "  link_layer_termination 0 learn_enable 0 vlan_edit_profile 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_VSI_ID,
    "vsi",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.vsi:\r\n"
    "  uint32 vsi\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_high_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0\r\n"
    "  link_layer_termination 0 learn_enable 0 vlan_edit_profile 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_COS_PROFILE_ID,
    "cos_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.cos_profile:\r\n"
    "  uint32 cos_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_high_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0\r\n"
    "  link_layer_termination 0 learn_enable 0 vlan_edit_profile 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_MEF_L2_CPSF_TUNNELING_ID,
    "mef_l2_cpsf_tunneling",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mef_l2_cpsf_tunneling:\r\n"
    "  uint32 mef_l2_cpsf_tunneling\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_high_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0\r\n"
    "  link_layer_termination 0 learn_enable 0 vlan_edit_profile 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_LINK_LAYER_TERMINATION_ID,
    "link_layer_termination",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.link_layer_termination:\r\n"
    "  uint32 link_layer_termination\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_high_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0\r\n"
    "  link_layer_termination 0 learn_enable 0 vlan_edit_profile 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_LEARN_ENABLE_ID,
    "learn_enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.learn_enable:\r\n"
    "  uint32 learn_enable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_high_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0\r\n"
    "  link_layer_termination 0 learn_enable 0 vlan_edit_profile 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_HIGH_TBL_SET_PP_AC_HIGH_TBL_SET_TBL_DATA_VLAN_EDIT_PROFILE_ID,
    "vlan_edit_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.vlan_edit_profile:\r\n"
    "  uint32 vlan_edit_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_high_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0\r\n"
    "  link_layer_termination 0 learn_enable 0 vlan_edit_profile 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_HIGH_TBL_GET_PP_AC_HIGH_TBL_GET_ID,
    "ac_high_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_high_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(9)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_HIGH_TBL_GET_PP_AC_HIGH_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_high_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(9)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_ID,
    "ac_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0 learn_enable 0\r\n"
    "  vlan_edit_profile 0 vlan_edit_tag 0 learn_destination 0 unknown_frwd_dst 0\r\n"
    "  eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(10)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0 learn_enable 0\r\n"
    "  vlan_edit_profile 0 vlan_edit_tag 0 learn_destination 0 unknown_frwd_dst 0\r\n"
    "  eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(10)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.valid:\r\n"
    "  uint32 valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0 learn_enable 0\r\n"
    "  vlan_edit_profile 0 vlan_edit_tag 0 learn_destination 0 unknown_frwd_dst 0\r\n"
    "  eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_COUNTER_OR_STAT_PTR_ID,
    "counter_or_stat_ptr",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.counter_or_stat_ptr:\r\n"
    "  uint32 counter_or_stat_ptr\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0 learn_enable 0\r\n"
    "  vlan_edit_profile 0 vlan_edit_tag 0 learn_destination 0 unknown_frwd_dst 0\r\n"
    "  eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_POLICER_PTR_ID,
    "policer_ptr",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.policer_ptr:\r\n"
    "  uint32 policer_ptr\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0 learn_enable 0\r\n"
    "  vlan_edit_profile 0 vlan_edit_tag 0 learn_destination 0 unknown_frwd_dst 0\r\n"
    "  eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_STATISTICS_TAG_ENABLE_ID,
    "statistics_tag_enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.statistics_tag_enable:\r\n"
    "  uint32 statistics_tag_enable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0 learn_enable 0\r\n"
    "  vlan_edit_profile 0 vlan_edit_tag 0 learn_destination 0 unknown_frwd_dst 0\r\n"
    "  eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_COUNTERS_ENABLE_ID,
    "counters_enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.counters_enable:\r\n"
    "  uint32 counters_enable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0 learn_enable 0\r\n"
    "  vlan_edit_profile 0 vlan_edit_tag 0 learn_destination 0 unknown_frwd_dst 0\r\n"
    "  eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_POLICER_ENABLE_ID,
    "policer_enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.policer_enable:\r\n"
    "  uint32 policer_enable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0 learn_enable 0\r\n"
    "  vlan_edit_profile 0 vlan_edit_tag 0 learn_destination 0 unknown_frwd_dst 0\r\n"
    "  eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_VSI_ID,
    "vsi",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.vsi:\r\n"
    "  uint32 vsi\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0 learn_enable 0\r\n"
    "  vlan_edit_profile 0 vlan_edit_tag 0 learn_destination 0 unknown_frwd_dst 0\r\n"
    "  eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_COS_PROFILE_ID,
    "cos_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.cos_profile:\r\n"
    "  uint32 cos_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0 learn_enable 0\r\n"
    "  vlan_edit_profile 0 vlan_edit_tag 0 learn_destination 0 unknown_frwd_dst 0\r\n"
    "  eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_LINK_LAYER_TERMINATION_ID,
    "link_layer_termination",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.link_layer_termination:\r\n"
    "  uint32 link_layer_termination\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 link_layer_termination 0 learn_enable 0\r\n"
    "  vlan_edit_profile 0 vlan_edit_tag 0 learn_destination 0 unknown_frwd_dst 0\r\n"
    "  eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_MEF_L2_CPSF_TUNNELING_ID,
    "mef_l2_cpsf_tunneling",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mef_l2_cpsf_tunneling:\r\n"
    "  uint32 mef_l2_cpsf_tunneling\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0 learn_enable 0\r\n"
    "  vlan_edit_profile 0 vlan_edit_tag 0 learn_destination 0 unknown_frwd_dst 0\r\n"
    "  eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_LEARN_ENABLE_ID,
    "learn_enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.learn_enable:\r\n"
    "  uint32 learn_enable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0 learn_enable 0\r\n"
    "  vlan_edit_profile 0 vlan_edit_tag 0 learn_destination 0 unknown_frwd_dst 0\r\n"
    "  eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_VLAN_EDIT_PROFILE_ID,
    "vlan_edit_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.vlan_edit_profile:\r\n"
    "  uint32 vlan_edit_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0 learn_enable 0\r\n"
    "  vlan_edit_profile 0 vlan_edit_tag 0 learn_destination 0 unknown_frwd_dst 0\r\n"
    "  eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_VLAN_EDIT_TAG_ID,
    "vlan_edit_vid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.vlan_edit_tag:\r\n"
    "  uint32 vlan_edit_tag\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0 learn_enable 0\r\n"
    "  vlan_edit_profile 0 vlan_edit_tag 0 learn_destination 0 unknown_frwd_dst 0\r\n"
    "  eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_VLAN_EDIT_TAG_ID,
    "vlan_edit_pcp_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.vlan_edit_tag:\r\n"
    "  uint32 vlan_edit_tag\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0 learn_enable 0\r\n"
    "  vlan_edit_profile 0 vlan_edit_tag 0 learn_destination 0 unknown_frwd_dst 0\r\n"
    "  eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_LEARN_DESTINATION_ID,
    "learn_destination",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.learn_destination:\r\n"
    "  uint32 learn_destination\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0 learn_enable 0\r\n"
    "  vlan_edit_profile 0 vlan_edit_tag 0 learn_destination 0 unknown_frwd_dst 0\r\n"
    "  eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_UNKNOWN_FRWD_DEST_ID,
    "unknown_frwd_dst",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.unknown_frwd_dst:\r\n"
    "  uint32 unknown_frwd_dst\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0 learn_enable 0\r\n"
    "  vlan_edit_profile 0 vlan_edit_tag 0 learn_destination 0 unknown_frwd_dst 0\r\n"
    "  eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_TBL_SET_PP_AC_TBL_SET_TBL_DATA_EEI_ID,
    "unknown_frwd_asd",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.eei:\r\n"
    "  uint32 eei\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_tbl_set entry 0 valid 0\r\n"
    "  counter_or_stat_ptr 0 policer_ptr 0 statistics_tag_enable 0 counters_enable 0\r\n"
    "  policer_enable 0 vsi 0 cos_profile 0 mef_l2_cpsf_tunneling 0 learn_enable 0\r\n"
    "  vlan_edit_profile 0 vlan_edit_tag 0 learn_destination 0 unknown_frwd_dst 0\r\n"
    "  eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AC_TBL_GET_PP_AC_TBL_GET_ID,
    "ac_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_tbl_get entry 0\r\n"
#endif
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
    PARAM_T20E_PP_AC_TBL_GET_PP_AC_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_ac_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(11)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_ID,
    "pwe_high_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_high_tbl_set entry 0 valid 0\r\n"
    "  next_protocol 0 counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0\r\n"
    "  counter_enable 0 policer_enable 0 orientation_is_hub 0 tpid_profile 0\r\n"
    "  cos_profile 0 learn_enable 0 asd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(12)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_high_tbl_set entry 0 valid 0\r\n"
    "  next_protocol 0 counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0\r\n"
    "  counter_enable 0 policer_enable 0 orientation_is_hub 0 tpid_profile 0\r\n"
    "  cos_profile 0 learn_enable 0 asd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(12)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.valid:\r\n"
    "  uint32 valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_high_tbl_set entry 0 valid 0\r\n"
    "  next_protocol 0 counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0\r\n"
    "  counter_enable 0 policer_enable 0 orientation_is_hub 0 tpid_profile 0\r\n"
    "  cos_profile 0 learn_enable 0 asd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(12)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_NEXT_PROTOCOL_ID,
    "next_protocol",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.next_protocol:\r\n"
    "  uint32 next_protocol\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_high_tbl_set entry 0 valid 0\r\n"
    "  next_protocol 0 counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0\r\n"
    "  counter_enable 0 policer_enable 0 orientation_is_hub 0 tpid_profile 0\r\n"
    "  cos_profile 0 learn_enable 0 asd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(12)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_COUNTER_PTR_STAT_TAG_ID,
    "counter_ptr_stat_tag",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.counter_ptr_stat_tag:\r\n"
    "  uint32 counter_ptr_stat_tag\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_high_tbl_set entry 0 valid 0\r\n"
    "  next_protocol 0 counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0\r\n"
    "  counter_enable 0 policer_enable 0 orientation_is_hub 0 tpid_profile 0\r\n"
    "  cos_profile 0 learn_enable 0 asd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(12)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_POINT_TO_POINT_ID,
    "point_to_point",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.point_to_point:\r\n"
    "  uint32 point_to_point\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_high_tbl_set entry 0 valid 0\r\n"
    "  next_protocol 0 point_to_point 0 policer_ptr 0 stat_tag_enable 0\r\n"
    "  counter_enable 0 policer_enable 0 orientation_is_hub 0 tpid_profile 0\r\n"
    "  cos_profile 0 learn_enable 0 asd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(12)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_POLICER_PTR_ID,
    "policer_ptr",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.policer_ptr:\r\n"
    "  uint32 policer_ptr\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_high_tbl_set entry 0 valid 0\r\n"
    "  next_protocol 0 counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0\r\n"
    "  counter_enable 0 policer_enable 0 orientation_is_hub 0 tpid_profile 0\r\n"
    "  cos_profile 0 learn_enable 0 asd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(12)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_STAT_TAG_ENABLE_ID,
    "stat_tag_enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.stat_tag_enable:\r\n"
    "  uint32 stat_tag_enable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_high_tbl_set entry 0 valid 0\r\n"
    "  next_protocol 0 counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0\r\n"
    "  counter_enable 0 policer_enable 0 orientation_is_hub 0 tpid_profile 0\r\n"
    "  cos_profile 0 learn_enable 0 asd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(12)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_COUNTER_ENABLE_ID,
    "counter_enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.counter_enable:\r\n"
    "  uint32 counter_enable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_high_tbl_set entry 0 valid 0\r\n"
    "  next_protocol 0 counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0\r\n"
    "  counter_enable 0 policer_enable 0 orientation_is_hub 0 tpid_profile 0\r\n"
    "  cos_profile 0 learn_enable 0 asd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(12)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_POLICER_ENABLE_ID,
    "policer_enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.policer_enable:\r\n"
    "  uint32 policer_enable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_high_tbl_set entry 0 valid 0\r\n"
    "  next_protocol 0 counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0\r\n"
    "  counter_enable 0 policer_enable 0 orientation_is_hub 0 tpid_profile 0\r\n"
    "  cos_profile 0 learn_enable 0 asd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(12)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_ORIENTATION_IS_HUB_ID,
    "orientation_is_hub",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.orientation_is_hub:\r\n"
    "  uint32 orientation_is_hub\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_high_tbl_set entry 0 valid 0\r\n"
    "  next_protocol 0 counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0\r\n"
    "  counter_enable 0 policer_enable 0 orientation_is_hub 0 tpid_profile 0\r\n"
    "  cos_profile 0 learn_enable 0 asd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(12)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_TPID_PROFILE_ID,
    "tpid_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.tpid_profile:\r\n"
    "  uint32 tpid_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_high_tbl_set entry 0 valid 0\r\n"
    "  next_protocol 0 counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0\r\n"
    "  counter_enable 0 policer_enable 0 orientation_is_hub 0 tpid_profile 0\r\n"
    "  cos_profile 0 learn_enable 0 asd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(12)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_COS_PROFILE_ID,
    "cos_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.cos_profile:\r\n"
    "  uint32 cos_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_high_tbl_set entry 0 valid 0\r\n"
    "  next_protocol 0 counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0\r\n"
    "  counter_enable 0 policer_enable 0 orientation_is_hub 0 tpid_profile 0\r\n"
    "  cos_profile 0 learn_enable 0 asd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(12)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_LEARN_ENABLE_ID,
    "learn_enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.learn_enable:\r\n"
    "  uint32 learn_enable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_high_tbl_set entry 0 valid 0\r\n"
    "  next_protocol 0 counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0\r\n"
    "  counter_enable 0 policer_enable 0 orientation_is_hub 0 tpid_profile 0\r\n"
    "  cos_profile 0 learn_enable 0 asd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(12)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_HIGH_TBL_SET_PP_PWE_HIGH_TBL_SET_TBL_DATA_ASD_ID,
    "asd",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.asd:\r\n"
    "  uint32 asd\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_high_tbl_set entry 0 valid 0\r\n"
    "  next_protocol 0 counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0\r\n"
    "  counter_enable 0 policer_enable 0 orientation_is_hub 0 tpid_profile 0\r\n"
    "  cos_profile 0 learn_enable 0 asd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(12)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_HIGH_TBL_GET_PP_PWE_HIGH_TBL_GET_ID,
    "pwe_high_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_high_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(13)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_HIGH_TBL_GET_PP_PWE_HIGH_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_high_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(13)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_LOW_TBL_SET_PP_PWE_LOW_TBL_SET_ID,
    "pwe_low_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_low_tbl_set entry 0 vsi 0 reverse_fec\r\n"
    "  0 unknown_frwd_dst 0 eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(14)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_LOW_TBL_SET_PP_PWE_LOW_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_low_tbl_set entry 0 vsi 0 reverse_fec\r\n"
    "  0 unknown_frwd_dst 0 eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(14)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_LOW_TBL_SET_PP_PWE_LOW_TBL_SET_TBL_DATA_VSI_ID,
    "vsi",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.vsi:\r\n"
    "  uint32 vsi\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_low_tbl_set entry 0 vsi 0 reverse_fec\r\n"
    "  0 unknown_frwd_dst 0 eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_LOW_TBL_SET_PP_PWE_LOW_TBL_SET_TBL_DATA_REVERSE_FEC_ID,
    "reverse_fec",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.reverse_fec:\r\n"
    "  uint32 reverse_fec\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_low_tbl_set entry 0 vsi 0 reverse_fec\r\n"
    "  0 unknown_frwd_dst 0 eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_LOW_TBL_SET_PP_PWE_LOW_TBL_SET_TBL_DATA_UNKNOWN_FRWD_DEST_ID,
    "unknown_frwd_dst",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.unknown_frwd_dst:\r\n"
    "  uint32 unknown_frwd_dst\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_low_tbl_set entry 0 vsi 0 reverse_fec\r\n"
    "  0 unknown_frwd_dst 0 eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_LOW_TBL_SET_PP_PWE_LOW_TBL_SET_TBL_DATA_EEI_ID,
    "eei",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.eei:\r\n"
    "  uint32 eei\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_low_tbl_set entry 0 vsi 0 reverse_fec\r\n"
    "  0 unknown_frwd_dst 0 eei 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_LOW_TBL_GET_PP_PWE_LOW_TBL_GET_ID,
    "pwe_low_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_low_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(15)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_LOW_TBL_GET_PP_PWE_LOW_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_low_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(15)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_ID,
    "pwe_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_tbl_set entry 0 valid 0\r\n"
    "  counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0 counter_enable 0\r\n"
    "  policer_enable 0 orientation_is_hub 0 reverse_vc 0 reverse_fec 0 tpid_profile\r\n"
    "  0 learn_enable 0 cos_profile 0 unknown_frwd_dst 0 vsi 0 next_protocol 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(16)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_tbl_set entry 0 valid 0\r\n"
    "  counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0 counter_enable 0\r\n"
    "  policer_enable 0 orientation_is_hub 0 reverse_vc 0 reverse_fec 0 tpid_profile\r\n"
    "  0 learn_enable 0 cos_profile 0 unknown_frwd_dst 0 vsi 0 next_protocol 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(16)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.valid:\r\n"
    "  uint32 valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_tbl_set entry 0 valid 0\r\n"
    "  counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0 counter_enable 0\r\n"
    "  policer_enable 0 orientation_is_hub 0 reverse_vc 0 reverse_fec 0 tpid_profile\r\n"
    "  0 learn_enable 0 cos_profile 0 unknown_frwd_dst 0 vsi 0 next_protocol 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_COUNTER_PTR_STAT_TAG_ID,
    "counter_ptr_stat_tag",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.counter_ptr_stat_tag:\r\n"
    "  uint32 counter_ptr_stat_tag\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_tbl_set entry 0 valid 0\r\n"
    "  counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0 counter_enable 0\r\n"
    "  policer_enable 0 orientation_is_hub 0 reverse_vc 0 reverse_fec 0 tpid_profile\r\n"
    "  0 learn_enable 0 cos_profile 0 unknown_frwd_dst 0 vsi 0 next_protocol 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_POLICER_PTR_ID,
    "policer_ptr",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.policer_ptr:\r\n"
    "  uint32 policer_ptr\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_tbl_set entry 0 valid 0\r\n"
    "  counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0 counter_enable 0\r\n"
    "  policer_enable 0 orientation_is_hub 0 reverse_vc 0 reverse_fec 0 tpid_profile\r\n"
    "  0 learn_enable 0 cos_profile 0 unknown_frwd_dst 0 vsi 0 next_protocol 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_STAT_TAG_ENABLE_ID,
    "stat_tag_enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.stat_tag_enable:\r\n"
    "  uint32 stat_tag_enable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_tbl_set entry 0 valid 0\r\n"
    "  counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0 counter_enable 0\r\n"
    "  policer_enable 0 orientation_is_hub 0 reverse_vc 0 reverse_fec 0 tpid_profile\r\n"
    "  0 learn_enable 0 cos_profile 0 unknown_frwd_dst 0 vsi 0 next_protocol 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_COUNTER_ENABLE_ID,
    "counter_enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.counter_enable:\r\n"
    "  uint32 counter_enable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_tbl_set entry 0 valid 0\r\n"
    "  counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0 counter_enable 0\r\n"
    "  policer_enable 0 orientation_is_hub 0 reverse_vc 0 reverse_fec 0 tpid_profile\r\n"
    "  0 learn_enable 0 cos_profile 0 unknown_frwd_dst 0 vsi 0 next_protocol 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_POLICER_ENABLE_ID,
    "policer_enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.policer_enable:\r\n"
    "  uint32 policer_enable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_tbl_set entry 0 valid 0\r\n"
    "  counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0 counter_enable 0\r\n"
    "  policer_enable 0 orientation_is_hub 0 reverse_vc 0 reverse_fec 0 tpid_profile\r\n"
    "  0 learn_enable 0 cos_profile 0 unknown_frwd_dst 0 vsi 0 next_protocol 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_ORIENTATION_IS_HUB_ID,
    "orientation_is_hub",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.orientation_is_hub:\r\n"
    "  uint32 orientation_is_hub\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_tbl_set entry 0 valid 0\r\n"
    "  counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0 counter_enable 0\r\n"
    "  policer_enable 0 orientation_is_hub 0 reverse_vc 0 reverse_fec 0 tpid_profile\r\n"
    "  0 learn_enable 0 cos_profile 0 unknown_frwd_dst 0 vsi 0 next_protocol 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_REVERSE_VC_ID,
    "reverse_vc",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.reverse_vc:\r\n"
    "  uint32 reverse_vc\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_tbl_set entry 0 valid 0\r\n"
    "  counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0 counter_enable 0\r\n"
    "  policer_enable 0 orientation_is_hub 0 reverse_vc 0 reverse_fec 0 tpid_profile\r\n"
    "  0 learn_enable 0 cos_profile 0 unknown_frwd_dst 0 vsi 0 next_protocol 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_REVERSE_FEC_ID,
    "reverse_fec",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.reverse_fec:\r\n"
    "  uint32 reverse_fec\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_tbl_set entry 0 valid 0\r\n"
    "  counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0 counter_enable 0\r\n"
    "  policer_enable 0 orientation_is_hub 0 reverse_vc 0 reverse_fec 0 tpid_profile\r\n"
    "  0 learn_enable 0 cos_profile 0 unknown_frwd_dst 0 vsi 0 next_protocol 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_TPID_PROFILE_ID,
    "tpid_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.tpid_profile:\r\n"
    "  uint32 tpid_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_tbl_set entry 0 valid 0\r\n"
    "  counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0 counter_enable 0\r\n"
    "  policer_enable 0 orientation_is_hub 0 reverse_vc 0 reverse_fec 0 tpid_profile\r\n"
    "  0 learn_enable 0 cos_profile 0 unknown_frwd_dst 0 vsi 0 next_protocol 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_LEARN_ENABLE_ID,
    "learn_enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.learn_enable:\r\n"
    "  uint32 learn_enable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_tbl_set entry 0 valid 0\r\n"
    "  counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0 counter_enable 0\r\n"
    "  policer_enable 0 orientation_is_hub 0 reverse_vc 0 reverse_fec 0 tpid_profile\r\n"
    "  0 learn_enable 0 cos_profile 0 unknown_frwd_dst 0 vsi 0 next_protocol 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_COS_PROFILE_ID,
    "cos_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.cos_profile:\r\n"
    "  uint32 cos_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_tbl_set entry 0 valid 0\r\n"
    "  counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0 counter_enable 0\r\n"
    "  policer_enable 0 orientation_is_hub 0 reverse_vc 0 reverse_fec 0 tpid_profile\r\n"
    "  0 learn_enable 0 cos_profile 0 unknown_frwd_dst 0 vsi 0 next_protocol 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_UNKNOWN_FRWD_DEST_ID,
    "unknown_frwd_dst",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.unknown_frwd_dst:\r\n"
    "  uint32 unknown_frwd_dst\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_tbl_set entry 0 valid 0\r\n"
    "  counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0 counter_enable 0\r\n"
    "  policer_enable 0 orientation_is_hub 0 reverse_vc 0 reverse_fec 0 tpid_profile\r\n"
    "  0 learn_enable 0 cos_profile 0 unknown_frwd_dst 0 vsi 0 next_protocol 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_UNKNOWN_FRWD_ASD_ID,
    "unknown_frwd_asd",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.unknown_frwd_asd:\r\n"
    "  uint32 unknown_frwd_asd\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_tbl_set entry 0 valid 0\r\n"
    "  counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0 counter_enable 0\r\n"
    "  policer_enable 0 orientation_is_hub 0 reverse_vc 0 reverse_fec 0 tpid_profile\r\n"
    "  0 learn_enable 0 cos_profile 0 unknown_frwd_asd 0 vsi 0 next_protocol 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_VSI_ID,
    "vsi",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.vsi:\r\n"
    "  uint32 vsi\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_tbl_set entry 0 valid 0\r\n"
    "  counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0 counter_enable 0\r\n"
    "  policer_enable 0 orientation_is_hub 0 reverse_vc 0 reverse_fec 0 tpid_profile\r\n"
    "  0 learn_enable 0 cos_profile 0 unknown_frwd_dst 0 vsi 0 next_protocol 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_TBL_SET_PP_PWE_TBL_SET_TBL_DATA_NEXT_PROTOCOL_ID,
    "next_protocol",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.next_protocol:\r\n"
    "  uint32 next_protocol\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_tbl_set entry 0 valid 0\r\n"
    "  counter_ptr_stat_tag 0 policer_ptr 0 stat_tag_enable 0 counter_enable 0\r\n"
    "  policer_enable 0 orientation_is_hub 0 reverse_vc 0 reverse_fec 0 tpid_profile\r\n"
    "  0 learn_enable 0 cos_profile 0 unknown_frwd_dst 0 vsi 0 next_protocol 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_TBL_GET_PP_PWE_TBL_GET_ID,
    "pwe_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(17)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PWE_TBL_GET_PP_PWE_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_pwe_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_ID,
    "aging_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_aging_tbl_set entry 0 age_status_15 0\r\n"
    "  age_status_14 0 age_status_13 0 age_status_12 0 age_status_11 0 age_status_10\r\n"
    "  0 age_status_9 0 age_status_8 0 age_status_7 0 age_status_6 0 age_status_5 0\r\n"
    "  age_status_4 0 age_status_3 0 age_status_2 0 age_status_1 0 age_status_0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(18)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_aging_tbl_set entry 0 age_status_15 0\r\n"
    "  age_status_14 0 age_status_13 0 age_status_12 0 age_status_11 0 age_status_10\r\n"
    "  0 age_status_9 0 age_status_8 0 age_status_7 0 age_status_6 0 age_status_5 0\r\n"
    "  age_status_4 0 age_status_3 0 age_status_2 0 age_status_1 0 age_status_0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(18)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_15_ID,
    "age_status_15",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.age_status_15:\r\n"
    "  uint32 age_status_15\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_aging_tbl_set entry 0 age_status_15 0\r\n"
    "  age_status_14 0 age_status_13 0 age_status_12 0 age_status_11 0 age_status_10\r\n"
    "  0 age_status_9 0 age_status_8 0 age_status_7 0 age_status_6 0 age_status_5 0\r\n"
    "  age_status_4 0 age_status_3 0 age_status_2 0 age_status_1 0 age_status_0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_14_ID,
    "age_status_14",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.age_status_14:\r\n"
    "  uint32 age_status_14\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_aging_tbl_set entry 0 age_status_15 0\r\n"
    "  age_status_14 0 age_status_13 0 age_status_12 0 age_status_11 0 age_status_10\r\n"
    "  0 age_status_9 0 age_status_8 0 age_status_7 0 age_status_6 0 age_status_5 0\r\n"
    "  age_status_4 0 age_status_3 0 age_status_2 0 age_status_1 0 age_status_0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_13_ID,
    "age_status_13",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.age_status_13:\r\n"
    "  uint32 age_status_13\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_aging_tbl_set entry 0 age_status_15 0\r\n"
    "  age_status_14 0 age_status_13 0 age_status_12 0 age_status_11 0 age_status_10\r\n"
    "  0 age_status_9 0 age_status_8 0 age_status_7 0 age_status_6 0 age_status_5 0\r\n"
    "  age_status_4 0 age_status_3 0 age_status_2 0 age_status_1 0 age_status_0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_12_ID,
    "age_status_12",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.age_status_12:\r\n"
    "  uint32 age_status_12\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_aging_tbl_set entry 0 age_status_15 0\r\n"
    "  age_status_14 0 age_status_13 0 age_status_12 0 age_status_11 0 age_status_10\r\n"
    "  0 age_status_9 0 age_status_8 0 age_status_7 0 age_status_6 0 age_status_5 0\r\n"
    "  age_status_4 0 age_status_3 0 age_status_2 0 age_status_1 0 age_status_0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_11_ID,
    "age_status_11",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.age_status_11:\r\n"
    "  uint32 age_status_11\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_aging_tbl_set entry 0 age_status_15 0\r\n"
    "  age_status_14 0 age_status_13 0 age_status_12 0 age_status_11 0 age_status_10\r\n"
    "  0 age_status_9 0 age_status_8 0 age_status_7 0 age_status_6 0 age_status_5 0\r\n"
    "  age_status_4 0 age_status_3 0 age_status_2 0 age_status_1 0 age_status_0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_10_ID,
    "age_status_10",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.age_status_10:\r\n"
    "  uint32 age_status_10\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_aging_tbl_set entry 0 age_status_15 0\r\n"
    "  age_status_14 0 age_status_13 0 age_status_12 0 age_status_11 0 age_status_10\r\n"
    "  0 age_status_9 0 age_status_8 0 age_status_7 0 age_status_6 0 age_status_5 0\r\n"
    "  age_status_4 0 age_status_3 0 age_status_2 0 age_status_1 0 age_status_0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_9_ID,
    "age_status_9",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.age_status_9:\r\n"
    "  uint32 age_status_9\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_aging_tbl_set entry 0 age_status_15 0\r\n"
    "  age_status_14 0 age_status_13 0 age_status_12 0 age_status_11 0 age_status_10\r\n"
    "  0 age_status_9 0 age_status_8 0 age_status_7 0 age_status_6 0 age_status_5 0\r\n"
    "  age_status_4 0 age_status_3 0 age_status_2 0 age_status_1 0 age_status_0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_8_ID,
    "age_status_8",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.age_status_8:\r\n"
    "  uint32 age_status_8\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_aging_tbl_set entry 0 age_status_15 0\r\n"
    "  age_status_14 0 age_status_13 0 age_status_12 0 age_status_11 0 age_status_10\r\n"
    "  0 age_status_9 0 age_status_8 0 age_status_7 0 age_status_6 0 age_status_5 0\r\n"
    "  age_status_4 0 age_status_3 0 age_status_2 0 age_status_1 0 age_status_0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_7_ID,
    "age_status_7",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.age_status_7:\r\n"
    "  uint32 age_status_7\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_aging_tbl_set entry 0 age_status_15 0\r\n"
    "  age_status_14 0 age_status_13 0 age_status_12 0 age_status_11 0 age_status_10\r\n"
    "  0 age_status_9 0 age_status_8 0 age_status_7 0 age_status_6 0 age_status_5 0\r\n"
    "  age_status_4 0 age_status_3 0 age_status_2 0 age_status_1 0 age_status_0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_6_ID,
    "age_status_6",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.age_status_6:\r\n"
    "  uint32 age_status_6\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_aging_tbl_set entry 0 age_status_15 0\r\n"
    "  age_status_14 0 age_status_13 0 age_status_12 0 age_status_11 0 age_status_10\r\n"
    "  0 age_status_9 0 age_status_8 0 age_status_7 0 age_status_6 0 age_status_5 0\r\n"
    "  age_status_4 0 age_status_3 0 age_status_2 0 age_status_1 0 age_status_0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_5_ID,
    "age_status_5",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.age_status_5:\r\n"
    "  uint32 age_status_5\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_aging_tbl_set entry 0 age_status_15 0\r\n"
    "  age_status_14 0 age_status_13 0 age_status_12 0 age_status_11 0 age_status_10\r\n"
    "  0 age_status_9 0 age_status_8 0 age_status_7 0 age_status_6 0 age_status_5 0\r\n"
    "  age_status_4 0 age_status_3 0 age_status_2 0 age_status_1 0 age_status_0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_4_ID,
    "age_status_4",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.age_status_4:\r\n"
    "  uint32 age_status_4\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_aging_tbl_set entry 0 age_status_15 0\r\n"
    "  age_status_14 0 age_status_13 0 age_status_12 0 age_status_11 0 age_status_10\r\n"
    "  0 age_status_9 0 age_status_8 0 age_status_7 0 age_status_6 0 age_status_5 0\r\n"
    "  age_status_4 0 age_status_3 0 age_status_2 0 age_status_1 0 age_status_0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_3_ID,
    "age_status_3",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.age_status_3:\r\n"
    "  uint32 age_status_3\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_aging_tbl_set entry 0 age_status_15 0\r\n"
    "  age_status_14 0 age_status_13 0 age_status_12 0 age_status_11 0 age_status_10\r\n"
    "  0 age_status_9 0 age_status_8 0 age_status_7 0 age_status_6 0 age_status_5 0\r\n"
    "  age_status_4 0 age_status_3 0 age_status_2 0 age_status_1 0 age_status_0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_2_ID,
    "age_status_2",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.age_status_2:\r\n"
    "  uint32 age_status_2\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_aging_tbl_set entry 0 age_status_15 0\r\n"
    "  age_status_14 0 age_status_13 0 age_status_12 0 age_status_11 0 age_status_10\r\n"
    "  0 age_status_9 0 age_status_8 0 age_status_7 0 age_status_6 0 age_status_5 0\r\n"
    "  age_status_4 0 age_status_3 0 age_status_2 0 age_status_1 0 age_status_0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_1_ID,
    "age_status_1",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.age_status_1:\r\n"
    "  uint32 age_status_1\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_aging_tbl_set entry 0 age_status_15 0\r\n"
    "  age_status_14 0 age_status_13 0 age_status_12 0 age_status_11 0 age_status_10\r\n"
    "  0 age_status_9 0 age_status_8 0 age_status_7 0 age_status_6 0 age_status_5 0\r\n"
    "  age_status_4 0 age_status_3 0 age_status_2 0 age_status_1 0 age_status_0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AGING_TBL_SET_PP_AGING_TBL_SET_TBL_DATA_AGE_STATUS_0_ID,
    "age_status_0",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.age_status_0:\r\n"
    "  uint32 age_status_0\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_aging_tbl_set entry 0 age_status_15 0\r\n"
    "  age_status_14 0 age_status_13 0 age_status_12 0 age_status_11 0 age_status_10\r\n"
    "  0 age_status_9 0 age_status_8 0 age_status_7 0 age_status_6 0 age_status_5 0\r\n"
    "  age_status_4 0 age_status_3 0 age_status_2 0 age_status_1 0 age_status_0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AGING_TBL_GET_PP_AGING_TBL_GET_ID,
    "aging_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_aging_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(19)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_AGING_TBL_GET_PP_AGING_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_aging_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(19)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_CNT_INP_TBL_SET_PP_CNT_INP_TBL_SET_ID,
    "cnt_inp_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_cnt_inp_tbl_set entry 0 packets 0\r\n"
    "  octets_index 0 octets 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(20)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_CNT_INP_TBL_SET_PP_CNT_INP_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_cnt_inp_tbl_set entry 0 packets 0\r\n"
    "  octets_index 0 octets 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(20)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_CNT_INP_TBL_SET_PP_CNT_INP_TBL_SET_TBL_DATA_PACKETS_ID,
    "packets",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.packets:\r\n"
    "  uint32 packets\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_cnt_inp_tbl_set entry 0 packets 0\r\n"
    "  octets_index 0 octets 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_CNT_INP_TBL_SET_PP_CNT_INP_TBL_SET_TBL_DATA_OCTETS_INDEX_ID,
    "octets_index",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  octets_index:\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_cnt_inp_tbl_set entry 0 packets 0\r\n"
    "  octets_index 0 octets 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_CNT_INP_TBL_SET_PP_CNT_INP_TBL_SET_TBL_DATA_OCTETS_ID,
    "octets",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.octets[ prm_octets_index]:\r\n"
    "  uint32 octets[2]\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_cnt_inp_tbl_set entry 0 packets 0\r\n"
    "  octets_index 0 octets 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_CNT_INP_TBL_GET_PP_CNT_INP_TBL_GET_ID,
    "cnt_inp_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_cnt_inp_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(21)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_CNT_INP_TBL_GET_PP_CNT_INP_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_cnt_inp_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(21)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_COUNTERS_TBL_SET_PP_EGRESS_COUNTERS_TBL_SET_ID,
    "egress_counters_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_counters_tbl_set entry 0\r\n"
    "  counter1_overflow 0 counter1 0 counter0_overflow 0 counter0_index 0 counter0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(22)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_COUNTERS_TBL_SET_PP_EGRESS_COUNTERS_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_counters_tbl_set entry 0\r\n"
    "  counter1_overflow 0 counter1 0 counter0_overflow 0 counter0_index 0 counter0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(22)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_COUNTERS_TBL_SET_PP_EGRESS_COUNTERS_TBL_SET_TBL_DATA_COUNTER0_ID,
    "octet0",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.counter0[ prm_counter0_index]:\r\n"
    "  uint32 counter0[2]\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_counters_tbl_set entry 0\r\n"
    "  counter1_overflow 0 counter1 0 counter0_overflow 0 counter0_index 0 counter0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(22)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_COUNTERS_TBL_SET_PP_EGRESS_COUNTERS_TBL_SET_TBL_DATA_COUNTER1_ID,
    "octet1",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.counter0[ prm_counter0_index]:\r\n"
    "  uint32 counter0[2]\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_counters_tbl_set entry 0\r\n"
    "  counter1_overflow 0 counter1 0 counter0_overflow 0 counter0_index 0 counter0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(22)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_COUNTERS_TBL_SET_PP_EGRESS_COUNTERS_TBL_SET_TBL_DATA_PACKETS_ID,
    "packets",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.counter0[ prm_counter0_index]:\r\n"
    "  uint32 counter0[2]\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_counters_tbl_set entry 0\r\n"
    "  counter1_overflow 0 counter1 0 counter0_overflow 0 counter0_index 0 counter0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(22)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_COUNTERS_TBL_GET_PP_EGRESS_COUNTERS_TBL_GET_ID,
    "egress_counters_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_counters_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(23)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_COUNTERS_TBL_GET_PP_EGRESS_COUNTERS_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_counters_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(23)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PLC_TBL_SET_PP_PLC_TBL_SET_ID,
    "plc_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_plc_tbl_set entry 0 info_index 0 info 0\r\n"
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
    PARAM_T20E_PP_PLC_TBL_SET_PP_PLC_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_plc_tbl_set entry 0 info_index 0 info 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(24)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_PLC_TBL_SET_PP_PLC_TBL_SET_TBL_DATA_INFO_INDEX_ID,
    "info_index",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info_index:\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_plc_tbl_set entry 0 info_index 0 info 0\r\n"
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
    PARAM_T20E_PP_PLC_TBL_SET_PP_PLC_TBL_SET_TBL_DATA_INFO_ID,
    "info",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.info[ prm_info_index]:\r\n"
    "  uint32 info[3]\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_plc_tbl_set entry 0 info_index 0 info 0\r\n"
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
    PARAM_T20E_PP_PLC_TBL_GET_PP_PLC_TBL_GET_ID,
    "plc_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_plc_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_PLC_TBL_GET_PP_PLC_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_plc_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_ID,
    "egress_mpls_tunnel_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_tunnel_tbl_set entry 0\r\n"
    "  next_eep 0 out_vsi 0 mpls2_label 0 mpls2_command 0 mpls1_label 0\r\n"
    "  mpls1_command 0 orientation 0\r\n"
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
    PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_tunnel_tbl_set entry 0\r\n"
    "  next_eep 0 out_vsi 0 mpls2_label 0 mpls2_command 0 mpls1_label 0\r\n"
    "  mpls1_command 0 orientation 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(26)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_TBL_DATA_NEXT_EEP_ID,
    "next_eep",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.next_eep:\r\n"
    "  uint32 next_eep\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_tunnel_tbl_set entry 0\r\n"
    "  next_eep 0 out_vsi 0 mpls2_label 0 mpls2_command 0 mpls1_label 0\r\n"
    "  mpls1_command 0 orientation 0\r\n"
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
    PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_TBL_DATA_OUT_VSI_ID,
    "out_vsi",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.out_vsi:\r\n"
    "  uint32 out_vsi\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_tunnel_tbl_set entry 0\r\n"
    "  next_eep 0 out_vsi 0 mpls2_label 0 mpls2_command 0 mpls1_label 0\r\n"
    "  mpls1_command 0 orientation 0\r\n"
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
    PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_TBL_DATA_MPLS2_LABEL_ID,
    "mpls2_label",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mpls2_label:\r\n"
    "  uint32 mpls2_label\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_tunnel_tbl_set entry 0\r\n"
    "  next_eep 0 out_vsi 0 mpls2_label 0 mpls2_command 0 mpls1_label 0\r\n"
    "  mpls1_command 0 orientation 0\r\n"
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
    PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_TBL_DATA_MPLS2_COMMAND_ID,
    "mpls2_command",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mpls2_command:\r\n"
    "  uint32 mpls2_command\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_tunnel_tbl_set entry 0\r\n"
    "  next_eep 0 out_vsi 0 mpls2_label 0 mpls2_command 0 mpls1_label 0\r\n"
    "  mpls1_command 0 orientation 0\r\n"
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
    PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_TBL_DATA_MPLS1_LABEL_ID,
    "mpls1_label",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mpls1_label:\r\n"
    "  uint32 mpls1_label\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_tunnel_tbl_set entry 0\r\n"
    "  next_eep 0 out_vsi 0 mpls2_label 0 mpls2_command 0 mpls1_label 0\r\n"
    "  mpls1_command 0 orientation 0\r\n"
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
    PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_TBL_DATA_MPLS1_COMMAND_ID,
    "mpls1_command",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mpls1_command:\r\n"
    "  uint32 mpls1_command\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_tunnel_tbl_set entry 0\r\n"
    "  next_eep 0 out_vsi 0 mpls2_label 0 mpls2_command 0 mpls1_label 0\r\n"
    "  mpls1_command 0 orientation 0\r\n"
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
    PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_SET_PP_EGRESS_MPLS_TUNNEL_TBL_SET_TBL_DATA_ORIENTATION_ID,
    "orientation",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.orientation:\r\n"
    "  uint32 orientation\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_tunnel_tbl_set entry 0\r\n"
    "  next_eep 0 out_vsi 0 mpls2_label 0 mpls2_command 0 mpls1_label 0\r\n"
    "  mpls1_command 0 orientation 0\r\n"
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
    PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_GET_PP_EGRESS_MPLS_TUNNEL_TBL_GET_ID,
    "egress_mpls_tunnel_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_tunnel_tbl_get entry 0\r\n"
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
    PARAM_T20E_PP_EGRESS_MPLS_TUNNEL_TBL_GET_PP_EGRESS_MPLS_TUNNEL_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc pp pp_egress_mpls_tunnel_tbl_get entry 0\r\n"
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
    PARAM_T20E_AC_ACCESS_ENTRY_GET_UNSAFE_AC_ACCESS_ENTRY_GET_UNSAFE_ID,
    "ac_access_entry_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc t20e_acc_pp entry_get inner_vid 0 outer_vid 0\r\n"
    "  vlan_domain 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(30)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_AC_ACCESS_ENTRY_GET_UNSAFE_AC_ACCESS_ENTRY_GET_UNSAFE_AC_KEYS_INNER_VID_ID,
    "inner_vid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ac_keys.inner_vid:\r\n"
    "  uint32 inner_vid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc t20e_acc_pp entry_get inner_vid 0 outer_vid 0\r\n"
    "  vlan_domain 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(30)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_AC_ACCESS_ENTRY_GET_UNSAFE_AC_ACCESS_ENTRY_GET_UNSAFE_AC_KEYS_OUTER_VID_ID,
    "outer_vid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ac_keys.outer_vid:\r\n"
    "  Inner VID. Should be 'T20E_LIF_IGNORE_INTERNAL_VID' when\r\n"
    "  the vlan_domain refers to ports that do not support VSI\r\n"
    "  according to 2 VIDs\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc t20e_acc_pp entry_get inner_vid 0 outer_vid 0\r\n"
    "  vlan_domain 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(30)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_AC_ACCESS_ENTRY_GET_UNSAFE_AC_ACCESS_ENTRY_GET_UNSAFE_AC_KEYS_VLAN_DOMAIN_ID,
    "vlan_domain",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ac_keys.vlan_domain:\r\n"
    "  Outer VID. Set to T20E_LIF_IGNORE_OUTER_VID in order to\r\n"
    "  ignore outer_VID, then also the inner VID will be\r\n"
    "  ignored and the AC is set according to vlan_domain only.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc t20e_acc_pp entry_get inner_vid 0 outer_vid 0\r\n"
    "  vlan_domain 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(30)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_T20E_ACC_PP_ID,
    "pp",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(8)|BIT(9)|BIT(10)|BIT(11)|BIT(12)|BIT(13)|BIT(14)|BIT(15)|BIT(16)|BIT(17)|BIT(18)|BIT(19)|BIT(20)|BIT(21)|BIT(22)|BIT(23)|BIT(24)|BIT(25)|BIT(26)|BIT(27)|BIT(28)|BIT(29)|BIT(30)|BIT(31), BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(8)|BIT(9)|BIT(10)|BIT(11)|BIT(12)|BIT(13)|BIT(14)|BIT(15)|BIT(16)|BIT(17)|BIT(18)|BIT(19)|BIT(20)|BIT(21)|BIT(22)|BIT(23)|BIT(24)|BIT(25)|BIT(26)|BIT(27)|BIT(28)|BIT(29)|BIT(30)|BIT(31), BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(8)|BIT(9)|BIT(10)|BIT(11)|BIT(12)|BIT(13)|BIT(14)|BIT(15)|BIT(16)|BIT(17)|BIT(18)|BIT(19)|BIT(20)|BIT(21)|BIT(22)|BIT(23)|BIT(24)|BIT(25)|BIT(26)|BIT(27)|BIT(28)|BIT(29)|BIT(30)|BIT(31), BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(8)|BIT(9)|BIT(10)|BIT(11)|BIT(12)|BIT(13)|BIT(14)|BIT(15)|BIT(16)|BIT(17)|BIT(18)|BIT(19)|BIT(20)|BIT(21)|BIT(22)|BIT(23)|BIT(24)|BIT(25)|BIT(26)|BIT(27)|BIT(28)|BIT(29)|BIT(30)|BIT(31), BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(8)|BIT(9)|BIT(10)|BIT(11)|BIT(12)|BIT(13)|BIT(14)|BIT(15)|BIT(16)|BIT(17)|BIT(18)|BIT(19)|BIT(20)|BIT(21)|BIT(22)|BIT(23)|BIT(24)|BIT(25)|BIT(26)|BIT(27),BIT(30)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } pp*/
#ifdef UI_T20E_ACC_MACT/* { mact*/
  {
    PARAM_T20E_MACT_SET_REPS_FOR_TBL_UNSAFE_MACT_SET_REPS_FOR_TBL_UNSAFE_ID,
    "set_reps_for_tbl_unsafe",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_set_reps_for_tbl_unsafe nof_reps 0\r\n"
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
    PARAM_T20E_MACT_SET_REPS_FOR_TBL_UNSAFE_MACT_SET_REPS_FOR_TBL_UNSAFE_NOF_REPS_ID,
    "nof_reps",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  nof_reps:\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_set_reps_for_tbl_unsafe nof_reps 0\r\n"
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
    PARAM_T20E_PP_LEM_MAC_DA_TBL_SET_T20E_PP_LEM_MAC_DA_TBL_SET_ID,
    "mac_da_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc t20e_acc_mact mac_da_tbl_set fid 0 mac 0 is_dynamic 0\r\n"
    "  destination 0 asd 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(6)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_LEM_MAC_DA_TBL_SET_T20E_PP_LEM_MAC_DA_TBL_SET_MAC_KEY_FID_ID,
    "fid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mac_key.fid:\r\n"
    "  uint32 fid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc t20e_acc_mact mac_da_tbl_set fid 0 mac 0 is_dynamic 0\r\n"
    "  destination 0 asd 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_LEM_MAC_DA_TBL_SET_T20E_PP_LEM_MAC_DA_TBL_SET_MAC_KEY_MAC_ID,
    "mac",
    (PARAM_VAL_RULES *)&t20e_acc_mac_addr_vals[0],
    (sizeof(t20e_acc_mac_addr_vals) / sizeof(t20e_acc_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mac_key.mac:\r\n"
    "  /* Filtering ID. Range: Soc_petra: 0 - 4095. T20E: 0 - 64K*/\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc t20e_acc_mact mac_da_tbl_set fid 0 mac 0 is_dynamic 0\r\n"
    "  destination 0 asd 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(6)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_LEM_MAC_DA_TBL_SET_T20E_PP_LEM_MAC_DA_TBL_SET_MAC_VAL_IS_DYNAMIC_ID,
    "is_dynamic",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mac_val.is_dynamic:\r\n"
    "  uint32 is_dynamic\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc t20e_acc_mact mac_da_tbl_set fid 0 mac 0 is_dynamic 0\r\n"
    "  destination 0 asd 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_LEM_MAC_DA_TBL_SET_T20E_PP_LEM_MAC_DA_TBL_SET_MAC_VAL_DESTINATION_ID,
    "destination",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mac_val.destination:\r\n"
    "  uint32 destination\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc t20e_acc_mact mac_da_tbl_set fid 0 mac 0 is_dynamic 0\r\n"
    "  destination 0 asd 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_LEM_MAC_DA_TBL_SET_T20E_PP_LEM_MAC_DA_TBL_SET_MAC_VAL_ASD_ID,
    "asd",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mac_val.asd:\r\n"
    "  uint32 asd\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc t20e_acc_mact mac_da_tbl_set fid 0 mac 0 is_dynamic 0\r\n"
    "  destination 0 asd 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_LEM_MAC_DA_TBL_SET_T20E_PP_LEM_MAC_DA_TBL_SET_MAC_VAL_VALID_ID,
    "valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mac_val.valid:\r\n"
    "  uint32 valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc t20e_acc_mact mac_da_tbl_set fid 0 mac 0 is_dynamic 0\r\n"
    "  destination 0 asd 0 valid 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_LEM_MAC_DA_TBL_GET_T20E_PP_LEM_MAC_DA_TBL_GET_ID,
    "mac_da_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc t20e_acc_mact mac_da_tbl_get fid 0 mac 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(7)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_LEM_MAC_DA_TBL_GET_T20E_PP_LEM_MAC_DA_TBL_GET_MAC_KEY_FID_ID,
    "fid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mac_key.fid:\r\n"
    "  uint32 fid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc t20e_acc_mact mac_da_tbl_get fid 0 mac 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_PP_LEM_MAC_DA_TBL_GET_T20E_PP_LEM_MAC_DA_TBL_GET_MAC_KEY_MAC_ID,
    "mac",
    (PARAM_VAL_RULES *)&t20e_acc_mac_addr_vals[0],
    (sizeof(t20e_acc_mac_addr_vals) / sizeof(t20e_acc_mac_addr_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mac_key.mac:\r\n"
    "  /* Filtering ID. Range: Soc_petra: 0 - 4095. T20E: 0 - 64K*/\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc t20e_acc_mact mac_da_tbl_get fid 0 mac 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_ID,
    "fid_counter_profile_db_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_fid_counter_profile_db_tbl_set entry 0\r\n"
    "  limit_en 0 interrupt_en 0 limit 0\r\n"
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
    PARAM_T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_fid_counter_profile_db_tbl_set entry 0\r\n"
    "  limit_en 0 interrupt_en 0 limit 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(29)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_TBL_DATA_LIMIT_EN_ID,
    "limit_en",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.limit_en:\r\n"
    "  uint32 limit_en\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_fid_counter_profile_db_tbl_set entry 0\r\n"
    "  limit_en 0 interrupt_en 0 limit 0\r\n"
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
    PARAM_T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_TBL_DATA_INTERRUPT_EN_ID,
    "interrupt_en",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.interrupt_en:\r\n"
    "  uint32 interrupt_en\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_fid_counter_profile_db_tbl_set entry 0\r\n"
    "  limit_en 0 interrupt_en 0 limit 0\r\n"
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
    PARAM_T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_TBL_DATA_LIMIT_ID,
    "limit",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.limit:\r\n"
    "  uint32 limit\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_fid_counter_profile_db_tbl_set entry 0\r\n"
    "  limit_en 0 interrupt_en 0 limit 0\r\n"
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
    PARAM_T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_GET_MACT_FID_COUNTER_PROFILE_DB_TBL_GET_ID,
    "fid_counter_profile_db_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_fid_counter_profile_db_tbl_get entry 0\r\n"
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
    PARAM_T20E_MACT_FID_COUNTER_PROFILE_DB_TBL_GET_MACT_FID_COUNTER_PROFILE_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_fid_counter_profile_db_tbl_get entry 0\r\n"
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
    PARAM_T20E_MACT_FID_COUNTER_DB_TBL_SET_MACT_FID_COUNTER_DB_TBL_SET_ID,
    "fid_counter_db_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_fid_counter_db_tbl_set entry 0\r\n"
    "  profile_pointer 0 entry_count 0\r\n"
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
    PARAM_T20E_MACT_FID_COUNTER_DB_TBL_SET_MACT_FID_COUNTER_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_fid_counter_db_tbl_set entry 0\r\n"
    "  profile_pointer 0 entry_count 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, BIT(31)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MACT_FID_COUNTER_DB_TBL_SET_MACT_FID_COUNTER_DB_TBL_SET_TBL_DATA_PROFILE_POINTER_ID,
    "profile_pointer",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.profile_pointer:\r\n"
    "  uint32 profile_pointer\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_fid_counter_db_tbl_set entry 0\r\n"
    "  profile_pointer 0 entry_count 0\r\n"
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
    PARAM_T20E_MACT_FID_COUNTER_DB_TBL_SET_MACT_FID_COUNTER_DB_TBL_SET_TBL_DATA_ENTRY_COUNT_ID,
    "entry_count",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.entry_count:\r\n"
    "  uint32 entry_count\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_fid_counter_db_tbl_set entry 0\r\n"
    "  profile_pointer 0 entry_count 0\r\n"
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
    PARAM_T20E_MACT_FID_COUNTER_DB_TBL_GET_MACT_FID_COUNTER_DB_TBL_GET_ID,
    "fid_counter_db_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_fid_counter_db_tbl_get entry 0\r\n"
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
    PARAM_T20E_MACT_FID_COUNTER_DB_TBL_GET_MACT_FID_COUNTER_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_fid_counter_db_tbl_get entry 0\r\n"
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
    PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_ID,
    "flush_db_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_flush_db_tbl_set entry 0\r\n"
    "  action_transplant_payload_mask_index 0 action_transplant_payload_mask 0\r\n"
    "  action_transplant_payload_data_index 0 action_transplant_payload_data 0\r\n"
    "  action_drop 0 compare_payload_mask_index 0 compare_payload_mask 0\r\n"
    "  compare_payload_data_index 0 compare_payload_data 0 compare_key_18_msbs_mask\r\n"
    "  0 compare_key_18_msbs_data 0 compare_valid 0\r\n"
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
    PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_flush_db_tbl_set entry 0\r\n"
    "  action_transplant_payload_mask_index 0 action_transplant_payload_mask 0\r\n"
    "  action_transplant_payload_data_index 0 action_transplant_payload_data 0\r\n"
    "  action_drop 0 compare_payload_mask_index 0 compare_payload_mask 0\r\n"
    "  compare_payload_data_index 0 compare_payload_data 0 compare_key_18_msbs_mask\r\n"
    "  0 compare_key_18_msbs_data 0 compare_valid 0\r\n"
#endif
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
    PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_ACTION_TRANSPLANT_PAYLOAD_MASK_INDEX_ID,
    "action_transplant_payload_mask_index",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action_transplant_payload_mask_index:\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_flush_db_tbl_set entry 0\r\n"
    "  action_transplant_payload_mask_index 0 action_transplant_payload_mask 0\r\n"
    "  action_transplant_payload_data_index 0 action_transplant_payload_data 0\r\n"
    "  action_drop 0 compare_payload_mask_index 0 compare_payload_mask 0\r\n"
    "  compare_payload_data_index 0 compare_payload_data 0 compare_key_18_msbs_mask\r\n"
    "  0 compare_key_18_msbs_data 0 compare_valid 0\r\n"
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
    PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_ACTION_TRANSPLANT_PAYLOAD_MASK_ID,
    "action_transplant_payload_mask",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.action_transplant_payload_mask[\r\n"
    "  prm_action_transplant_payload_mask_index]:\r\n"
    "  uint32 action_transplant_payload_mask[2]\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_flush_db_tbl_set entry 0\r\n"
    "  action_transplant_payload_mask_index 0 action_transplant_payload_mask 0\r\n"
    "  action_transplant_payload_data_index 0 action_transplant_payload_data 0\r\n"
    "  action_drop 0 compare_payload_mask_index 0 compare_payload_mask 0\r\n"
    "  compare_payload_data_index 0 compare_payload_data 0 compare_key_18_msbs_mask\r\n"
    "  0 compare_key_18_msbs_data 0 compare_valid 0\r\n"
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
    PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_ACTION_TRANSPLANT_PAYLOAD_DATA_INDEX_ID,
    "action_transplant_payload_data_index",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  action_transplant_payload_data_index:\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_flush_db_tbl_set entry 0\r\n"
    "  action_transplant_payload_mask_index 0 action_transplant_payload_mask 0\r\n"
    "  action_transplant_payload_data_index 0 action_transplant_payload_data 0\r\n"
    "  action_drop 0 compare_payload_mask_index 0 compare_payload_mask 0\r\n"
    "  compare_payload_data_index 0 compare_payload_data 0 compare_key_18_msbs_mask\r\n"
    "  0 compare_key_18_msbs_data 0 compare_valid 0\r\n"
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
    PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_ACTION_TRANSPLANT_PAYLOAD_DATA_ID,
    "action_transplant_payload_data",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.action_transplant_payload_data[\r\n"
    "  prm_action_transplant_payload_data_index]:\r\n"
    "  uint32 action_transplant_payload_data[2]\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_flush_db_tbl_set entry 0\r\n"
    "  action_transplant_payload_mask_index 0 action_transplant_payload_mask 0\r\n"
    "  action_transplant_payload_data_index 0 action_transplant_payload_data 0\r\n"
    "  action_drop 0 compare_payload_mask_index 0 compare_payload_mask 0\r\n"
    "  compare_payload_data_index 0 compare_payload_data 0 compare_key_18_msbs_mask\r\n"
    "  0 compare_key_18_msbs_data 0 compare_valid 0\r\n"
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
    PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_ACTION_DROP_ID,
    "action_drop",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.action_drop:\r\n"
    "  uint32 action_drop\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_flush_db_tbl_set entry 0\r\n"
    "  action_transplant_payload_mask_index 0 action_transplant_payload_mask 0\r\n"
    "  action_transplant_payload_data_index 0 action_transplant_payload_data 0\r\n"
    "  action_drop 0 compare_payload_mask_index 0 compare_payload_mask 0\r\n"
    "  compare_payload_data_index 0 compare_payload_data 0 compare_key_18_msbs_mask\r\n"
    "  0 compare_key_18_msbs_data 0 compare_valid 0\r\n"
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
    PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_COMPARE_PAYLOAD_MASK_INDEX_ID,
    "compare_payload_mask_index",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  compare_payload_mask_index:\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_flush_db_tbl_set entry 0\r\n"
    "  action_transplant_payload_mask_index 0 action_transplant_payload_mask 0\r\n"
    "  action_transplant_payload_data_index 0 action_transplant_payload_data 0\r\n"
    "  action_drop 0 compare_payload_mask_index 0 compare_payload_mask 0\r\n"
    "  compare_payload_data_index 0 compare_payload_data 0 compare_key_18_msbs_mask\r\n"
    "  0 compare_key_18_msbs_data 0 compare_valid 0\r\n"
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
    PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_COMPARE_PAYLOAD_MASK_ID,
    "compare_payload_mask",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.compare_payload_mask[ prm_compare_payload_mask_index]:\r\n"
    "  uint32 compare_payload_mask[2]\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_flush_db_tbl_set entry 0\r\n"
    "  action_transplant_payload_mask_index 0 action_transplant_payload_mask 0\r\n"
    "  action_transplant_payload_data_index 0 action_transplant_payload_data 0\r\n"
    "  action_drop 0 compare_payload_mask_index 0 compare_payload_mask 0\r\n"
    "  compare_payload_data_index 0 compare_payload_data 0 compare_key_18_msbs_mask\r\n"
    "  0 compare_key_18_msbs_data 0 compare_valid 0\r\n"
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
    PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_COMPARE_PAYLOAD_DATA_INDEX_ID,
    "compare_payload_data_index",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  compare_payload_data_index:\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_flush_db_tbl_set entry 0\r\n"
    "  action_transplant_payload_mask_index 0 action_transplant_payload_mask 0\r\n"
    "  action_transplant_payload_data_index 0 action_transplant_payload_data 0\r\n"
    "  action_drop 0 compare_payload_mask_index 0 compare_payload_mask 0\r\n"
    "  compare_payload_data_index 0 compare_payload_data 0 compare_key_18_msbs_mask\r\n"
    "  0 compare_key_18_msbs_data 0 compare_valid 0\r\n"
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
    PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_COMPARE_PAYLOAD_DATA_ID,
    "compare_payload_data",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.compare_payload_data[ prm_compare_payload_data_index]:\r\n"
    "  uint32 compare_payload_data[2]\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_flush_db_tbl_set entry 0\r\n"
    "  action_transplant_payload_mask_index 0 action_transplant_payload_mask 0\r\n"
    "  action_transplant_payload_data_index 0 action_transplant_payload_data 0\r\n"
    "  action_drop 0 compare_payload_mask_index 0 compare_payload_mask 0\r\n"
    "  compare_payload_data_index 0 compare_payload_data 0 compare_key_18_msbs_mask\r\n"
    "  0 compare_key_18_msbs_data 0 compare_valid 0\r\n"
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
    PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_COMPARE_KEY_18_MSBS_MASK_ID,
    "compare_key_18_msbs_mask",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.compare_key_18_msbs_mask:\r\n"
    "  uint32 compare_key_18_msbs_mask\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_flush_db_tbl_set entry 0\r\n"
    "  action_transplant_payload_mask_index 0 action_transplant_payload_mask 0\r\n"
    "  action_transplant_payload_data_index 0 action_transplant_payload_data 0\r\n"
    "  action_drop 0 compare_payload_mask_index 0 compare_payload_mask 0\r\n"
    "  compare_payload_data_index 0 compare_payload_data 0 compare_key_18_msbs_mask\r\n"
    "  0 compare_key_18_msbs_data 0 compare_valid 0\r\n"
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
    PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_COMPARE_KEY_18_MSBS_DATA_ID,
    "compare_key_18_msbs_data",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.compare_key_18_msbs_data:\r\n"
    "  uint32 compare_key_18_msbs_data\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_flush_db_tbl_set entry 0\r\n"
    "  action_transplant_payload_mask_index 0 action_transplant_payload_mask 0\r\n"
    "  action_transplant_payload_data_index 0 action_transplant_payload_data 0\r\n"
    "  action_drop 0 compare_payload_mask_index 0 compare_payload_mask 0\r\n"
    "  compare_payload_data_index 0 compare_payload_data 0 compare_key_18_msbs_mask\r\n"
    "  0 compare_key_18_msbs_data 0 compare_valid 0\r\n"
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
    PARAM_T20E_MACT_FLUSH_DB_TBL_SET_MACT_FLUSH_DB_TBL_SET_TBL_DATA_COMPARE_VALID_ID,
    "compare_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.compare_valid:\r\n"
    "  uint32 compare_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_flush_db_tbl_set entry 0\r\n"
    "  action_transplant_payload_mask_index 0 action_transplant_payload_mask 0\r\n"
    "  action_transplant_payload_data_index 0 action_transplant_payload_data 0\r\n"
    "  action_drop 0 compare_payload_mask_index 0 compare_payload_mask 0\r\n"
    "  compare_payload_data_index 0 compare_payload_data 0 compare_key_18_msbs_mask\r\n"
    "  0 compare_key_18_msbs_data 0 compare_valid 0\r\n"
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
    PARAM_T20E_MACT_FLUSH_DB_TBL_GET_MACT_FLUSH_DB_TBL_GET_ID,
    "flush_db_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_flush_db_tbl_get entry 0\r\n"
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
    PARAM_T20E_MACT_FLUSH_DB_TBL_GET_MACT_FLUSH_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mact mact_flush_db_tbl_get entry 0\r\n"
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
    PARAM_T20E_ACC_MACT_ID,
    "mact",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, 0, 0, BIT(28)|BIT(29)|BIT(30)|BIT(31), BIT(0), 0, BIT(6)|BIT(7)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } mact*/
#ifdef UI_T20E_ACC_OAM/* { oam*/
  {
    PARAM_T20E_OAMP_SET_REPS_FOR_TBL_UNSAFE_OAMP_SET_REPS_FOR_TBL_UNSAFE_ID,
    "set_reps_for_tbl_unsafe",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_set_reps_for_tbl_unsafe nof_reps 0\r\n"
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
    PARAM_T20E_OAMP_SET_REPS_FOR_TBL_UNSAFE_OAMP_SET_REPS_FOR_TBL_UNSAFE_NOF_REPS_ID,
    "nof_reps",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  nof_reps:\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_set_reps_for_tbl_unsafe nof_reps 0\r\n"
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
    PARAM_T20E_OAMP_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_ID,
    "mos_db_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mos_db_tbl_set entry 0 start 0 ffd_rate\r\n"
    "  0 fdi 0 bdi 0 is_ffd 0 cv_ffd_enable 0\r\n"
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
    PARAM_T20E_OAMP_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mos_db_tbl_set entry 0 start 0 ffd_rate\r\n"
    "  0 fdi 0 bdi 0 is_ffd 0 cv_ffd_enable 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(4)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_TBL_DATA_START_ID,
    "start",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.start:\r\n"
    "  uint32 start\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mos_db_tbl_set entry 0 start 0 ffd_rate\r\n"
    "  0 fdi 0 bdi 0 is_ffd 0 cv_ffd_enable 0\r\n"
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
    PARAM_T20E_OAMP_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_TBL_DATA_FFD_RATE_ID,
    "ffd_rate",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ffd_rate:\r\n"
    "  uint32 ffd_rate\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mos_db_tbl_set entry 0 start 0 ffd_rate\r\n"
    "  0 fdi 0 bdi 0 is_ffd 0 cv_ffd_enable 0\r\n"
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
    PARAM_T20E_OAMP_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_TBL_DATA_FDI_ID,
    "fdi",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.fdi:\r\n"
    "  uint32 fdi\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mos_db_tbl_set entry 0 start 0 ffd_rate\r\n"
    "  0 fdi 0 bdi 0 is_ffd 0 cv_ffd_enable 0\r\n"
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
    PARAM_T20E_OAMP_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_TBL_DATA_BDI_ID,
    "bdi",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.bdi:\r\n"
    "  uint32 bdi\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mos_db_tbl_set entry 0 start 0 ffd_rate\r\n"
    "  0 fdi 0 bdi 0 is_ffd 0 cv_ffd_enable 0\r\n"
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
    PARAM_T20E_OAMP_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_TBL_DATA_CV0_FFD1_ID,
    "is_ffd",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.is_ffd:\r\n"
    "  uint32 is_ffd\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mos_db_tbl_set entry 0 start 0 ffd_rate\r\n"
    "  0 fdi 0 bdi 0 is_ffd 0 cv_ffd_enable 0\r\n"
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
    PARAM_T20E_OAMP_MOS_DB_TBL_SET_OAMP_MOS_DB_TBL_SET_TBL_DATA_CV_FFD_ENABLE_ID,
    "cv_ffd_enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.cv_ffd_enable:\r\n"
    "  uint32 cv_ffd_enable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mos_db_tbl_set entry 0 start 0 ffd_rate\r\n"
    "  0 fdi 0 bdi 0 is_ffd 0 cv_ffd_enable 0\r\n"
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
    PARAM_T20E_OAMP_MOS_DB_TBL_GET_OAMP_MOS_DB_TBL_GET_ID,
    "mos_db_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mos_db_tbl_get entry 0\r\n"
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
    PARAM_T20E_OAMP_MOS_DB_TBL_GET_OAMP_MOS_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mos_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(5)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_ID,
    "mor_db_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
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
    PARAM_T20E_OAMP_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_MOT_DB_INDEX_ID,
    "mot_db_index",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mot_db_index:\r\n"
    "  uint32 mot_db_index\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_LAST_FFDCV_RX_TIME_VALID_ID,
    "last_ffdcv_rx_time_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.last_ffdcv_rx_time_valid:\r\n"
    "  uint32 last_ffdcv_rx_time_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_BDI_STATE_ID,
    "bdi_state",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.bdi_state:\r\n"
    "  uint32 bdi_state\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_LAST_BDI_RECEIVE_TIME_ID,
    "last_bdi_receive_time",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.last_bdi_receive_time:\r\n"
    "  uint32 last_bdi_receive_time\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_FDI_STATE_ID,
    "fdi_state",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.fdi_state:\r\n"
    "  uint32 fdi_state\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_LAST_FDI_RECEIVE_TIME_ID,
    "last_fdi_receive_time",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.last_fdi_receive_time:\r\n"
    "  uint32 last_fdi_receive_time\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_D_LOCV_TRANSITION_COUNT_ID,
    "d_locv_transition_count",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.d_locv_transition_count:\r\n"
    "  uint32 d_locv_transition_count\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_D_LOCV_STATE_ID,
    "d_locv_state",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.d_locv_state:\r\n"
    "  uint32 d_locv_state\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_LAST_FFDCV_RX_TIME_ID,
    "last_ffdcv_rx_time",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.last_ffdcv_rx_time:\r\n"
    "  uint32 last_ffdcv_rx_time\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_FFD_RX_RATE_ID,
    "ffd_rx_rate",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ffd_rx_rate:\r\n"
    "  uint32 ffd_rx_rate\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOR_DB_TBL_SET_OAMP_MOR_DB_TBL_SET_TBL_DATA_CV0_FFD1_ID,
    "is_ffd",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.is_ffd:\r\n"
    "  uint32 is_ffd\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mor_db_tbl_set entry 0 mot_db_index 0\r\n"
    "  last_ffdcv_rx_time_valid 0 bdi_state 0 last_bdi_receive_time 0 fdi_state 0\r\n"
    "  last_fdi_receive_time 0 d_locv_transition_count 0 d_locv_state 0\r\n"
    "  last_ffdcv_rx_time 0 ffd_rx_rate 0 is_ffd 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOR_DB_TBL_GET_OAMP_MOR_DB_TBL_GET_ID,
    "mor_db_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mor_db_tbl_get entry 0\r\n"
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
    PARAM_T20E_OAMP_MOR_DB_TBL_GET_OAMP_MOR_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mor_db_tbl_get entry 0\r\n"
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
    PARAM_T20E_OAMP_SYS_PORT_TBL_SET_OAMP_SYS_PORT_TBL_SET_ID,
    "sys_port_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_sys_port_tbl_set entry 0 sys_port 0\r\n"
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
    PARAM_T20E_OAMP_SYS_PORT_TBL_SET_OAMP_SYS_PORT_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_sys_port_tbl_set entry 0 sys_port 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(8)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_SYS_PORT_TBL_SET_OAMP_SYS_PORT_TBL_SET_TBL_DATA_SYS_PORT_ID,
    "sys_port",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.sys_port:\r\n"
    "  uint32 sys_port\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_sys_port_tbl_set entry 0 sys_port 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_SYS_PORT_TBL_GET_OAMP_SYS_PORT_TBL_GET_ID,
    "sys_port_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_sys_port_tbl_get entry 0\r\n"
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
    PARAM_T20E_OAMP_SYS_PORT_TBL_GET_OAMP_SYS_PORT_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_sys_port_tbl_get entry 0\r\n"
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
    PARAM_T20E_OAMP_LSR_ID_TBL_SET_OAMP_LSR_ID_TBL_SET_ID,
    "lsr_id_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_lsr_id_tbl_set entry 0 lsr_id_index 0\r\n"
    "  lsr_id 0\r\n"
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
    PARAM_T20E_OAMP_LSR_ID_TBL_SET_OAMP_LSR_ID_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_lsr_id_tbl_set entry 0 lsr_id_index 0\r\n"
    "  lsr_id 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(10)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_LSR_ID_TBL_SET_OAMP_LSR_ID_TBL_SET_TBL_DATA_LSR_ID_INDEX_ID,
    "lsr_id_index",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lsr_id_index:\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_lsr_id_tbl_set entry 0 lsr_id_index 0\r\n"
    "  lsr_id 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_LSR_ID_TBL_SET_OAMP_LSR_ID_TBL_SET_TBL_DATA_LSR_ID_ID,
    "lsr_id",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.lsr_id[ prm_lsr_id_index]:\r\n"
    "  uint32 lsr_id[4]\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_lsr_id_tbl_set entry 0 lsr_id_index 0\r\n"
    "  lsr_id 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_LSR_ID_TBL_GET_OAMP_LSR_ID_TBL_GET_ID,
    "lsr_id_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_lsr_id_tbl_get entry 0\r\n"
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
    PARAM_T20E_OAMP_LSR_ID_TBL_GET_OAMP_LSR_ID_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_lsr_id_tbl_get entry 0\r\n"
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
    PARAM_T20E_OAMP_DEFECT_TYPE_TBL_SET_OAMP_DEFECT_TYPE_TBL_SET_ID,
    "defect_type_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_defect_type_tbl_set entry 0 defect_type\r\n"
    "  0\r\n"
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
    PARAM_T20E_OAMP_DEFECT_TYPE_TBL_SET_OAMP_DEFECT_TYPE_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_defect_type_tbl_set entry 0 defect_type\r\n"
    "  0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(12)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_DEFECT_TYPE_TBL_SET_OAMP_DEFECT_TYPE_TBL_SET_TBL_DATA_DEFECT_TYPE_ID,
    "defect_type",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.defect_type:\r\n"
    "  uint32 defect_type\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_defect_type_tbl_set entry 0 defect_type\r\n"
    "  0\r\n"
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
    PARAM_T20E_OAMP_DEFECT_TYPE_TBL_GET_OAMP_DEFECT_TYPE_TBL_GET_ID,
    "defect_type_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_defect_type_tbl_get entry 0\r\n"
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
    PARAM_T20E_OAMP_DEFECT_TYPE_TBL_GET_OAMP_DEFECT_TYPE_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_defect_type_tbl_get entry 0\r\n"
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
    PARAM_T20E_OAMP_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_ID,
    "mep_db_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mep_db_tbl_set entry 0 up1down0 0 port\r\n"
    "  0 dm_db_index 0 dm_enable 0 ccm_tx_start 0 maid 0 ccm_interval 0\r\n"
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
    PARAM_T20E_OAMP_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mep_db_tbl_set entry 0 up1down0 0 port\r\n"
    "  0 dm_db_index 0 dm_enable 0 ccm_tx_start 0 maid 0 ccm_interval 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(14)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_UP1DOWN0_ID,
    "up1down0",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.up1down0:\r\n"
    "  uint32 up1down0\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mep_db_tbl_set entry 0 up1down0 0 port\r\n"
    "  0 dm_db_index 0 dm_enable 0 ccm_tx_start 0 maid 0 ccm_interval 0\r\n"
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
    PARAM_T20E_OAMP_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_PORT_ID,
    "port",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.port:\r\n"
    "  uint32 port\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mep_db_tbl_set entry 0 up1down0 0 port\r\n"
    "  0 dm_db_index 0 dm_enable 0 ccm_tx_start 0 maid 0 ccm_interval 0\r\n"
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
    PARAM_T20E_OAMP_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_DM_DB_INDEX_ID,
    "dm_db_index",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.dm_db_index:\r\n"
    "  uint32 dm_db_index\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mep_db_tbl_set entry 0 up1down0 0 port\r\n"
    "  0 dm_db_index 0 dm_enable 0 ccm_tx_start 0 maid 0 ccm_interval 0\r\n"
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
    PARAM_T20E_OAMP_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_DM_ENABLE_ID,
    "dm_enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.dm_enable:\r\n"
    "  uint32 dm_enable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mep_db_tbl_set entry 0 up1down0 0 port\r\n"
    "  0 dm_db_index 0 dm_enable 0 ccm_tx_start 0 maid 0 ccm_interval 0\r\n"
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
    PARAM_T20E_OAMP_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_CCM_TX_START_ID,
    "ccm_tx_start",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ccm_tx_start:\r\n"
    "  uint32 ccm_tx_start\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mep_db_tbl_set entry 0 up1down0 0 port\r\n"
    "  0 dm_db_index 0 dm_enable 0 ccm_tx_start 0 maid 0 ccm_interval 0\r\n"
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
    PARAM_T20E_OAMP_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_MAID_ID,
    "maid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.maid:\r\n"
    "  uint32 maid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mep_db_tbl_set entry 0 up1down0 0 port\r\n"
    "  0 dm_db_index 0 dm_enable 0 ccm_tx_start 0 maid 0 ccm_interval 0\r\n"
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
    PARAM_T20E_OAMP_MEP_DB_TBL_SET_OAMP_MEP_DB_TBL_SET_TBL_DATA_CCM_INTERVAL_ID,
    "ccm_interval",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ccm_interval:\r\n"
    "  uint32 ccm_interval\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mep_db_tbl_set entry 0 up1down0 0 port\r\n"
    "  0 dm_db_index 0 dm_enable 0 ccm_tx_start 0 maid 0 ccm_interval 0\r\n"
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
    PARAM_T20E_OAMP_MEP_DB_TBL_GET_OAMP_MEP_DB_TBL_GET_ID,
    "mep_db_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mep_db_tbl_get entry 0\r\n"
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
    PARAM_T20E_OAMP_MEP_DB_TBL_GET_OAMP_MEP_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mep_db_tbl_get entry 0\r\n"
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
    PARAM_T20E_OAMP_MEP_INDEX_DB_TBL_SET_OAMP_MEP_INDEX_DB_TBL_SET_ID,
    "mep_index_db_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mep_index_db_tbl_set entry 0\r\n"
    "  accelerated 0 mep_db_ptr 0\r\n"
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
    PARAM_T20E_OAMP_MEP_INDEX_DB_TBL_SET_OAMP_MEP_INDEX_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mep_index_db_tbl_set entry 0\r\n"
    "  accelerated 0 mep_db_ptr 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(16)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MEP_INDEX_DB_TBL_SET_OAMP_MEP_INDEX_DB_TBL_SET_TBL_DATA_ACCELERATED_ID,
    "accelerated",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.accelerated:\r\n"
    "  uint32 accelerated\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mep_index_db_tbl_set entry 0\r\n"
    "  accelerated 0 mep_db_ptr 0\r\n"
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
    PARAM_T20E_OAMP_MEP_INDEX_DB_TBL_SET_OAMP_MEP_INDEX_DB_TBL_SET_TBL_DATA_MEP_DB_PTR_ID,
    "mep_db_ptr",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mep_db_ptr:\r\n"
    "  uint32 mep_db_ptr\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mep_index_db_tbl_set entry 0\r\n"
    "  accelerated 0 mep_db_ptr 0\r\n"
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
    PARAM_T20E_OAMP_MEP_INDEX_DB_TBL_GET_OAMP_MEP_INDEX_DB_TBL_GET_ID,
    "mep_index_db_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mep_index_db_tbl_get entry 0\r\n"
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
    PARAM_T20E_OAMP_MEP_INDEX_DB_TBL_GET_OAMP_MEP_INDEX_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mep_index_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(17)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_ID,
    "rmep_db_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_rmep_db_tbl_set entry 0 rcvd_rdi 0\r\n"
    "  ccmdefect 0 ccminterval 0 last_ccmtime_valid 0 last_ccmtime 0\r\n"
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
    PARAM_T20E_OAMP_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_rmep_db_tbl_set entry 0 rcvd_rdi 0\r\n"
    "  ccmdefect 0 ccminterval 0 last_ccmtime_valid 0 last_ccmtime 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(18)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_TBL_DATA_RCVD_RDI_ID,
    "rcvd_rdi",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.rcvd_rdi:\r\n"
    "  uint32 rcvd_rdi\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_rmep_db_tbl_set entry 0 rcvd_rdi 0\r\n"
    "  ccmdefect 0 ccminterval 0 last_ccmtime_valid 0 last_ccmtime 0\r\n"
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
    PARAM_T20E_OAMP_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_TBL_DATA_CCMDEFECT_ID,
    "ccmdefect",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ccmdefect:\r\n"
    "  uint32 ccmdefect\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_rmep_db_tbl_set entry 0 rcvd_rdi 0\r\n"
    "  ccmdefect 0 ccminterval 0 last_ccmtime_valid 0 last_ccmtime 0\r\n"
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
    PARAM_T20E_OAMP_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_TBL_DATA_CCMINTERVAL_ID,
    "ccminterval",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ccminterval:\r\n"
    "  uint32 ccminterval\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_rmep_db_tbl_set entry 0 rcvd_rdi 0\r\n"
    "  ccmdefect 0 ccminterval 0 last_ccmtime_valid 0 last_ccmtime 0\r\n"
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
    PARAM_T20E_OAMP_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_TBL_DATA_LAST_CCMTIME_VALID_ID,
    "last_ccmtime_valid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.last_ccmtime_valid:\r\n"
    "  uint32 last_ccmtime_valid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_rmep_db_tbl_set entry 0 rcvd_rdi 0\r\n"
    "  ccmdefect 0 ccminterval 0 last_ccmtime_valid 0 last_ccmtime 0\r\n"
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
    PARAM_T20E_OAMP_RMEP_DB_TBL_SET_OAMP_RMEP_DB_TBL_SET_TBL_DATA_LAST_CCMTIME_ID,
    "last_ccmtime",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.last_ccmtime:\r\n"
    "  uint32 last_ccmtime\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_rmep_db_tbl_set entry 0 rcvd_rdi 0\r\n"
    "  ccmdefect 0 ccminterval 0 last_ccmtime_valid 0 last_ccmtime 0\r\n"
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
    PARAM_T20E_OAMP_RMEP_DB_TBL_GET_OAMP_RMEP_DB_TBL_GET_ID,
    "rmep_db_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_rmep_db_tbl_get entry 0\r\n"
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
    PARAM_T20E_OAMP_RMEP_DB_TBL_GET_OAMP_RMEP_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_rmep_db_tbl_get entry 0\r\n"
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
    PARAM_T20E_OAMP_RMEP_HASH_0_DB_TBL_SET_OAMP_RMEP_HASH_0_DB_TBL_SET_ID,
    "rmep_hash_0_db_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_rmep_hash_0_db_tbl_set entry 0\r\n"
    "  valid_hash0 0 verifier0 0 rmep_db_ptr0 0\r\n"
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
    PARAM_T20E_OAMP_RMEP_HASH_0_DB_TBL_SET_OAMP_RMEP_HASH_0_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_rmep_hash_0_db_tbl_set entry 0\r\n"
    "  valid_hash0 0 verifier0 0 rmep_db_ptr0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(20)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_RMEP_HASH_0_DB_TBL_SET_OAMP_RMEP_HASH_0_DB_TBL_SET_TBL_DATA_VALID_HASH0_ID,
    "valid_hash0",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.valid_hash0:\r\n"
    "  uint32 valid_hash0\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_rmep_hash_0_db_tbl_set entry 0\r\n"
    "  valid_hash0 0 verifier0 0 rmep_db_ptr0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_RMEP_HASH_0_DB_TBL_SET_OAMP_RMEP_HASH_0_DB_TBL_SET_TBL_DATA_VERIFIER0_ID,
    "verifier0",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.verifier0:\r\n"
    "  uint32 verifier0\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_rmep_hash_0_db_tbl_set entry 0\r\n"
    "  valid_hash0 0 verifier0 0 rmep_db_ptr0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_RMEP_HASH_0_DB_TBL_SET_OAMP_RMEP_HASH_0_DB_TBL_SET_TBL_DATA_RMEP_DB_PTR0_ID,
    "rmep_db_ptr0",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.rmep_db_ptr0:\r\n"
    "  uint32 rmep_db_ptr0\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_rmep_hash_0_db_tbl_set entry 0\r\n"
    "  valid_hash0 0 verifier0 0 rmep_db_ptr0 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_RMEP_HASH_0_DB_TBL_GET_OAMP_RMEP_HASH_0_DB_TBL_GET_ID,
    "rmep_hash_0_db_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_rmep_hash_0_db_tbl_get entry 0\r\n"
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
    PARAM_T20E_OAMP_RMEP_HASH_0_DB_TBL_GET_OAMP_RMEP_HASH_0_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_rmep_hash_0_db_tbl_get entry 0\r\n"
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
    PARAM_T20E_OAMP_RMEP_HASH_1_DB_TBL_SET_OAMP_RMEP_HASH_1_DB_TBL_SET_ID,
    "rmep_hash_1_db_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_rmep_hash_1_db_tbl_set entry 0\r\n"
    "  valid_hash1 0 verifier1 0 rmep_db_ptr1 0\r\n"
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
    PARAM_T20E_OAMP_RMEP_HASH_1_DB_TBL_SET_OAMP_RMEP_HASH_1_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_rmep_hash_1_db_tbl_set entry 0\r\n"
    "  valid_hash1 0 verifier1 0 rmep_db_ptr1 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(22)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_RMEP_HASH_1_DB_TBL_SET_OAMP_RMEP_HASH_1_DB_TBL_SET_TBL_DATA_VALID_HASH1_ID,
    "valid_hash1",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.valid_hash1:\r\n"
    "  uint32 valid_hash1\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_rmep_hash_1_db_tbl_set entry 0\r\n"
    "  valid_hash1 0 verifier1 0 rmep_db_ptr1 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(22)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_RMEP_HASH_1_DB_TBL_SET_OAMP_RMEP_HASH_1_DB_TBL_SET_TBL_DATA_VERIFIER1_ID,
    "verifier1",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.verifier1:\r\n"
    "  uint32 verifier1\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_rmep_hash_1_db_tbl_set entry 0\r\n"
    "  valid_hash1 0 verifier1 0 rmep_db_ptr1 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(22)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_RMEP_HASH_1_DB_TBL_SET_OAMP_RMEP_HASH_1_DB_TBL_SET_TBL_DATA_RMEP_DB_PTR1_ID,
    "rmep_db_ptr1",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.rmep_db_ptr1:\r\n"
    "  uint32 rmep_db_ptr1\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_rmep_hash_1_db_tbl_set entry 0\r\n"
    "  valid_hash1 0 verifier1 0 rmep_db_ptr1 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(22)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_RMEP_HASH_1_DB_TBL_GET_OAMP_RMEP_HASH_1_DB_TBL_GET_ID,
    "rmep_hash_1_db_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_rmep_hash_1_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(23)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_RMEP_HASH_1_DB_TBL_GET_OAMP_RMEP_HASH_1_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_rmep_hash_1_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(23)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_ID,
    "lmdb_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_lmdb_tbl_set entry 0 tx_fcf_peer_p 0\r\n"
    "  tx_fcf_peer_c 0 rx_fcb_p 0 rx_fcb_c 0 rx_fcb_peer_p 0 rx_fcb_peer_c 0\r\n"
    "  tx_fcf_p 0 tx_fcf_c 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(24)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_lmdb_tbl_set entry 0 tx_fcf_peer_p 0\r\n"
    "  tx_fcf_peer_c 0 rx_fcb_p 0 rx_fcb_c 0 rx_fcb_peer_p 0 rx_fcb_peer_c 0\r\n"
    "  tx_fcf_p 0 tx_fcf_c 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(24)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_TX_FCF_PEER_P_ID,
    "tx_fcf_peer_p",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.tx_fcf_peer_p:\r\n"
    "  uint32 tx_fcf_peer_p\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_lmdb_tbl_set entry 0 tx_fcf_peer_p 0\r\n"
    "  tx_fcf_peer_c 0 rx_fcb_p 0 rx_fcb_c 0 rx_fcb_peer_p 0 rx_fcb_peer_c 0\r\n"
    "  tx_fcf_p 0 tx_fcf_c 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(24)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_TX_FCF_PEER_C_ID,
    "tx_fcf_peer_c",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.tx_fcf_peer_c:\r\n"
    "  uint32 tx_fcf_peer_c\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_lmdb_tbl_set entry 0 tx_fcf_peer_p 0\r\n"
    "  tx_fcf_peer_c 0 rx_fcb_p 0 rx_fcb_c 0 rx_fcb_peer_p 0 rx_fcb_peer_c 0\r\n"
    "  tx_fcf_p 0 tx_fcf_c 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(24)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_RX_FCB_P_ID,
    "rx_fcb_p",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.rx_fcb_p:\r\n"
    "  uint32 rx_fcb_p\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_lmdb_tbl_set entry 0 tx_fcf_peer_p 0\r\n"
    "  tx_fcf_peer_c 0 rx_fcb_p 0 rx_fcb_c 0 rx_fcb_peer_p 0 rx_fcb_peer_c 0\r\n"
    "  tx_fcf_p 0 tx_fcf_c 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(24)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_RX_FCB_C_ID,
    "rx_fcb_c",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.rx_fcb_c:\r\n"
    "  uint32 rx_fcb_c\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_lmdb_tbl_set entry 0 tx_fcf_peer_p 0\r\n"
    "  tx_fcf_peer_c 0 rx_fcb_p 0 rx_fcb_c 0 rx_fcb_peer_p 0 rx_fcb_peer_c 0\r\n"
    "  tx_fcf_p 0 tx_fcf_c 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(24)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_RX_FCB_PEER_P_ID,
    "rx_fcb_peer_p",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.rx_fcb_peer_p:\r\n"
    "  uint32 rx_fcb_peer_p\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_lmdb_tbl_set entry 0 tx_fcf_peer_p 0\r\n"
    "  tx_fcf_peer_c 0 rx_fcb_p 0 rx_fcb_c 0 rx_fcb_peer_p 0 rx_fcb_peer_c 0\r\n"
    "  tx_fcf_p 0 tx_fcf_c 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(24)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_RX_FCB_PEER_C_ID,
    "rx_fcb_peer_c",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.rx_fcb_peer_c:\r\n"
    "  uint32 rx_fcb_peer_c\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_lmdb_tbl_set entry 0 tx_fcf_peer_p 0\r\n"
    "  tx_fcf_peer_c 0 rx_fcb_p 0 rx_fcb_c 0 rx_fcb_peer_p 0 rx_fcb_peer_c 0\r\n"
    "  tx_fcf_p 0 tx_fcf_c 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(24)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_TX_FCF_P_ID,
    "tx_fcf_p",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.tx_fcf_p:\r\n"
    "  uint32 tx_fcf_p\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_lmdb_tbl_set entry 0 tx_fcf_peer_p 0\r\n"
    "  tx_fcf_peer_c 0 rx_fcb_p 0 rx_fcb_c 0 rx_fcb_peer_p 0 rx_fcb_peer_c 0\r\n"
    "  tx_fcf_p 0 tx_fcf_c 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(24)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_LMDB_TBL_SET_OAMP_LMDB_TBL_SET_TBL_DATA_TX_FCF_C_ID,
    "tx_fcf_c",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.tx_fcf_c:\r\n"
    "  uint32 tx_fcf_c\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_lmdb_tbl_set entry 0 tx_fcf_peer_p 0\r\n"
    "  tx_fcf_peer_c 0 rx_fcb_p 0 rx_fcb_c 0 rx_fcb_peer_p 0 rx_fcb_peer_c 0\r\n"
    "  tx_fcf_p 0 tx_fcf_c 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(24)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_LMDB_TBL_GET_OAMP_LMDB_TBL_GET_ID,
    "lmdb_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_lmdb_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(25)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_LMDB_TBL_GET_OAMP_LMDB_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_lmdb_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(25)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_ID,
    "mot_db_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mot_db_tbl_set entry 0 defect_location\r\n"
    "  0 defect_type_ptr 0 ttl_profile 0 lsp_id_lsb 0 lsr_id_ptr 0 eep 0 cos 0\r\n"
    "  sysport 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(26)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mot_db_tbl_set entry 0 defect_location\r\n"
    "  0 defect_type_ptr 0 ttl_profile 0 lsp_id_lsb 0 lsr_id_ptr 0 eep 0 cos 0\r\n"
    "  sysport 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(26)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_DEFECT_LOCATION_ID,
    "defect_location",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.defect_location:\r\n"
    "  uint32 defect_location\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mot_db_tbl_set entry 0 defect_location\r\n"
    "  0 defect_type_ptr 0 ttl_profile 0 lsp_id_lsb 0 lsr_id_ptr 0 eep 0 cos 0\r\n"
    "  sysport 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_DEFECT_TYPE_PTR_ID,
    "defect_type_ptr",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.defect_type_ptr:\r\n"
    "  uint32 defect_type_ptr\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mot_db_tbl_set entry 0 defect_location\r\n"
    "  0 defect_type_ptr 0 ttl_profile 0 lsp_id_lsb 0 lsr_id_ptr 0 eep 0 cos 0\r\n"
    "  sysport 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_TTL_PROFILE_ID,
    "ttl_profile",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.ttl_profile:\r\n"
    "  uint32 ttl_profile\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mot_db_tbl_set entry 0 defect_location\r\n"
    "  0 defect_type_ptr 0 ttl_profile 0 lsp_id_lsb 0 lsr_id_ptr 0 eep 0 cos 0\r\n"
    "  sysport 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_LSP_ID_LSB_ID,
    "lsp_id_lsb",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.lsp_id_lsb:\r\n"
    "  uint32 lsp_id_lsb\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mot_db_tbl_set entry 0 defect_location\r\n"
    "  0 defect_type_ptr 0 ttl_profile 0 lsp_id_lsb 0 lsr_id_ptr 0 eep 0 cos 0\r\n"
    "  sysport 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_LSR_ID_PTR_ID,
    "lsr_id_ptr",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.lsr_id_ptr:\r\n"
    "  uint32 lsr_id_ptr\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mot_db_tbl_set entry 0 defect_location\r\n"
    "  0 defect_type_ptr 0 ttl_profile 0 lsp_id_lsb 0 lsr_id_ptr 0 eep 0 cos 0\r\n"
    "  sysport 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_EEP_ID,
    "eep",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.eep:\r\n"
    "  uint32 eep\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mot_db_tbl_set entry 0 defect_location\r\n"
    "  0 defect_type_ptr 0 ttl_profile 0 lsp_id_lsb 0 lsr_id_ptr 0 eep 0 cos 0\r\n"
    "  sysport 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_CO_S_ID,
    "cos",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.cos:\r\n"
    "  uint32 cos\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mot_db_tbl_set entry 0 defect_location\r\n"
    "  0 defect_type_ptr 0 ttl_profile 0 lsp_id_lsb 0 lsr_id_ptr 0 eep 0 cos 0\r\n"
    "  sysport 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOT_DB_TBL_SET_OAMP_MOT_DB_TBL_SET_TBL_DATA_SYSPORT_ID,
    "sysport",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.sysport:\r\n"
    "  uint32 sysport\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mot_db_tbl_set entry 0 defect_location\r\n"
    "  0 defect_type_ptr 0 ttl_profile 0 lsp_id_lsb 0 lsr_id_ptr 0 eep 0 cos 0\r\n"
    "  sysport 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOT_DB_TBL_GET_OAMP_MOT_DB_TBL_GET_ID,
    "mot_db_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mot_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(27)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_MOT_DB_TBL_GET_OAMP_MOT_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_mot_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(27)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_ID,
    "ext_mep_db_tbl_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_ext_mep_db_tbl_set entry 0 lmenable 0\r\n"
    "  priority 0 vid 0 md_level 0 mepid 0 rdi_indicator 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(28)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_ext_mep_db_tbl_set entry 0 lmenable 0\r\n"
    "  priority 0 vid 0 md_level 0 mepid 0 rdi_indicator 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(28)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_TBL_DATA_LMENABLE_ID,
    "lmenable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.lmenable:\r\n"
    "  uint32 lmenable\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_ext_mep_db_tbl_set entry 0 lmenable 0\r\n"
    "  priority 0 vid 0 md_level 0 mepid 0 rdi_indicator 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_TBL_DATA_PRIORITY_ID,
    "priority",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.priority:\r\n"
    "  uint32 priority\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_ext_mep_db_tbl_set entry 0 lmenable 0\r\n"
    "  priority 0 vid 0 md_level 0 mepid 0 rdi_indicator 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_TBL_DATA_VID_ID,
    "vid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.vid:\r\n"
    "  uint32 vid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_ext_mep_db_tbl_set entry 0 lmenable 0\r\n"
    "  priority 0 vid 0 md_level 0 mepid 0 rdi_indicator 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_TBL_DATA_MD_LEVEL_ID,
    "md_level",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.md_level:\r\n"
    "  uint32 md_level\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_ext_mep_db_tbl_set entry 0 lmenable 0\r\n"
    "  priority 0 vid 0 md_level 0 mepid 0 rdi_indicator 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_TBL_DATA_MEPID_ID,
    "mepid",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.mepid:\r\n"
    "  uint32 mepid\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_ext_mep_db_tbl_set entry 0 lmenable 0\r\n"
    "  priority 0 vid 0 md_level 0 mepid 0 rdi_indicator 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_EXT_MEP_DB_TBL_SET_OAMP_EXT_MEP_DB_TBL_SET_TBL_DATA_RDI_INDICATOR_ID,
    "rdi_indicator",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  tbl_data.rdi_indicator:\r\n"
    "  uint32 rdi_indicator\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_ext_mep_db_tbl_set entry 0 lmenable 0\r\n"
    "  priority 0 vid 0 md_level 0 mepid 0 rdi_indicator 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(28)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_EXT_MEP_DB_TBL_GET_OAMP_EXT_MEP_DB_TBL_GET_ID,
    "ext_mep_db_tbl_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_ext_mep_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(29)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_OAMP_EXT_MEP_DB_TBL_GET_OAMP_EXT_MEP_DB_TBL_GET_ENTRY_OFFSET_NDX_ID,
    "entry",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entry:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  entry\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc oam oamp_ext_mep_db_tbl_get entry 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, BIT(29)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_ACC_OAM_ID,
    "oam",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, 0, 0, 0, BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(8)|BIT(9)|BIT(10)|BIT(11)|BIT(12)|BIT(13)|BIT(14)|BIT(15)|BIT(16)|BIT(17)|BIT(18)|BIT(19)|BIT(20)|BIT(21)|BIT(22)|BIT(23)|BIT(24)|BIT(25)|BIT(26)|BIT(27)|BIT(28)|BIT(29)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } oam*/

#ifdef UI_T20E_REG_ACCESS/* { reg_access*/
  {
    PARAM_T20E_READ_FLD_READ_FLD_ID,
    "read_fld",
    (PARAM_VAL_RULES *)&t20e_addr_and_data[0],
    (sizeof(t20e_addr_and_data) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Read a field from t20e register\r\n"
    "",
    "Examples:\r\n"
    "  t20e_acc reg_access read_fld 0x100 lsb 0 msb 0 \r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_READ_FLD_READ_FLD_FIELD_LSB_ID,
    "lsb",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  field.lsb:\r\n"
    "  Field Least Significant Bit in the register.\r\n"
    "",
#ifdef UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc reg_access read_fld lsb 0 msb 0 step 0 base 0 instance_idx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_READ_FLD_READ_FLD_FIELD_MSB_ID,
    "msb",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  field.msb:\r\n"
    "  Field Most Significant Bit in the register.\r\n"
    "",
#ifdef UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc reg_access read_fld lsb 0 msb 0 step 0 base 0 instance_idx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_WRITE_FLD_WRITE_FLD_ID,
    "write_fld",
    (PARAM_VAL_RULES *)&t20e_addr_and_data[0],
    (sizeof(t20e_addr_and_data) / sizeof(t20e_addr_and_data[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Write a field from t20e register\r\n"
    "",
    "Examples:\r\n"
    "  t20e_acc reg_access write_fld  0x100 lsb 0 msb 0 val 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_WRITE_FLD_WRITE_FLD_FIELD_LSB_ID,
    "lsb",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  field.lsb:\r\n"
    "  Field Least Significant Bit in the register.\r\n"
    "",
#ifdef UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc reg_access write_fld lsb 0 msb 0 step 0 base 0 instance_idx 0 val 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_WRITE_FLD_WRITE_FLD_FIELD_MSB_ID,
    "msb",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  field.msb:\r\n"
    "  Field Most Significant Bit in the register.\r\n"
    "",
#ifdef UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc reg_access write_fld lsb 0 msb 0 step 0 base 0 instance_idx 0 val 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_WRITE_FLD_WRITE_FLD_VAL_ID,
    "val",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  val:\r\n"
    "  field value to write.\r\n"
    "",
#ifdef UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc reg_access write_fld lsb 0 msb 0 step 0 base 0 instance_idx 0 val 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(1)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_READ_REG_READ_REG_ID,
    "read_reg",
    (PARAM_VAL_RULES *)&t20e_addr_and_data[0],
    (sizeof(t20e_addr_and_data) / sizeof(t20e_addr_and_data[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Read a register from t20e register\r\n"
    "",
    "Examples:\r\n"
    "  t20e_acc reg_access 0x3400\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(2)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_WRITE_REG_WRITE_REG_ID,
    "write_reg",
    (PARAM_VAL_RULES *)&t20e_addr_and_data[0],
    (sizeof(t20e_addr_and_data) / sizeof(t20e_addr_and_data[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Write a register from t20e register\r\n"
    "",
    "Examples:\r\n"
    "  t20e_acc reg_access write_reg 0x3400 val 0x100\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_WRITE_REG_WRITE_REG_VAL_ID,
    "val",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  val:\r\n"
    "  register value to write.\r\n"
    "",
#ifdef UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc reg_access write_reg step 0 base 0 instance_idx 0 val 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(3)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_IWRITE_ID,
    "iwrite",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Write Data (MSB) to indirect Addr\r\n"
    "",
    "Examples:\r\n"
    "  t20e_acc general iwrite 0x10045 0x4 0x4 0x2\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(4)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_IREAD_ID,
    "iread",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Read data (MSB) from indirect addr\r\n"
    "",
    "Examples:\r\n"
    "  t20e_acc general iread 0x10045\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(5)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_IRW_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&t20e_addr_and_data[0],
    (sizeof(t20e_addr_and_data) / sizeof(t20e_addr_and_data[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Read data (MSB) from indirect addr\r\n"
    "",
    "Examples:\r\n"
    "  t20e_acc general iread 0x10045\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(4)|BIT(5)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_IACC_MODULE_ID_ID,
    "module_id",
    (PARAM_VAL_RULES *)&T20E_ACC_MODULE_ID_rule[0],
    (sizeof(T20E_ACC_MODULE_ID_rule) / sizeof(T20E_ACC_MODULE_ID_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  t20e block (module) to access. Range: 0 - 20\r\n"
    "OLP:   0   \r\n"
    "IRE:   1   \r\n"
    "IDR:   2   \r\n"
    "IRR:   3   \r\n"
    "IHP:   4   \r\n"
    "QDR:   5   \r\n"
    "IPS:   6   \r\n"
    "IPT:   7   \r\n"
    "DPI_A: 8   \r\n"
    "DPI_B: 9   \r\n"
    "DPI_C: 10  \r\n"
    "DPI_D: 11  \r\n"
    "DPI_E: 12  \r\n"
    "DPI_F: 13  \r\n"
    "RTP:   14  \r\n"
    "EGQ:   15  \r\n"
    "SCH:   16  \r\n"
    "CFC:   17  \r\n"
    "EPNI:  18  \r\n"
    "IQM:   19  \r\n"
    "MMU:   20  \r\n"
    "",
    "Examples:\r\n"
    "  t20e_acc general iread 0x10045 size 1 module_id 4\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(4)|BIT(5)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_SIZE_ID,
    "size",
    (PARAM_VAL_RULES *)&T20E_ACC_SIZE_ID_rule[0],
    (sizeof(T20E_ACC_SIZE_ID_rule) / sizeof(T20E_ACC_SIZE_ID_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  table size 1-3\r\n"
    "",
    "Examples:\r\n"
    "  t20e_acc general iread 0x10045 size 1\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(4)|BIT(5)},
    5,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_ACC_IWRITE_VAL_ID,
    "val",
    (PARAM_VAL_RULES *)&T20E_ACC_IWRITE_VAL_rule[0],
    (sizeof(T20E_ACC_IWRITE_VAL_rule) / sizeof(T20E_ACC_IWRITE_VAL_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  val:\r\n"
    "  register value to write.\r\n"
    "",
#ifdef UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc reg_access write_reg step 0 base 0 instance_idx 0 val 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(4)},
    6,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_DO_TWICE_ID,
    "do_twice",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Perform twice and wait in milliseconds\r\n"
    "",
    "Examples:\r\n"
    "  t20e_acc general iread 0x10045 size 1 do_twice 1000\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(0)|BIT(4)|BIT(5)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_ACC_REG_ACCESS_ID,
    "access",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } reg_access*/
#ifdef UI_T20E_HW_IF/* { hw_if*/
  {
    PARAM_T20E_HW_IF_CPU_INFO_SET_CPU_INFO_SET_ID,
    "cpu_info_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the CPU including Number of clocks between two CPU accesses.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if cpu_info_set acc_per_clk 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(0)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_CPU_INFO_SET_CPU_INFO_SET_CPU_INFO_ACC_PER_CLK_ID,
    "acc_per_clk",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  cpu_info.acc_per_clk:\r\n"
    "  Number of system clocks between two CPU accesses, controls the allocated\r\n"
    "  bandwidth to CPU\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if cpu_info_set acc_per_clk 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_CPU_INFO_GET_CPU_INFO_GET_ID,
    "cpu_info_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the CPU including Number of clocks between two CPU accesses.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if cpu_info_get\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(1)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_DRAM_INFO_SET_DRAM_INFO_SET_ID,
    "dram_info_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the DRAM including size of DRAM\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if dram_info_set dram_size_mbit 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(2)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_DRAM_INFO_SET_DRAM_INFO_SET_DRAM_INFO_DRAM_SIZE_MBIT_ID,
    "dram_size_mbit",
    (PARAM_VAL_RULES *)&T20E_HW_IF_DRAM_SIZE_MBIT_rule[0],
    (sizeof(T20E_HW_IF_DRAM_SIZE_MBIT_rule) / sizeof(T20E_HW_IF_DRAM_SIZE_MBIT_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  dram_info.dram_size_mbit:\r\n"
    "  Total QDR SRAM memory size Units: Mbits. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if dram_info_set dram_size_mbit 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(2)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_DRAM_INFO_GET_DRAM_INFO_GET_ID,
    "dram_info_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the DRAM including size of DRAM\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if dram_info_get\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(3)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_QDR_INFO_SET_QDR_INFO_SET_ID,
    "qdr_info_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the QDR configuration including size of QDR\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if qdr_info_set qdr_size_mbit 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(4)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_QDR_INFO_SET_QDR_INFO_SET_QDR_INFO_QDR_SIZE_MBIT_ID,
    "qdr_size_mbit",
    (PARAM_VAL_RULES *)&T20E_HW_IF_QDR_SIZE_MBIT_rule[0],
    (sizeof(T20E_HW_IF_QDR_SIZE_MBIT_rule) / sizeof(T20E_HW_IF_QDR_SIZE_MBIT_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  qdr_info.qdr_size_mbit:\r\n"
    "  Total QDR SRAM memory size Units: Mbits. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if qdr_info_set qdr_size_mbit 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(4)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_QDR_INFO_GET_QDR_INFO_GET_ID,
    "qdr_info_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the QDR configuration including size of QDR\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if qdr_info_get\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(5)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_ID,
    "xaui_info_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the XAUI configuration.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if xaui_info_set xaui_ndx 0 tx_vodctrl 0 tx_preemp_2t 0\r\n"
    "  tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0\r\n"
    "  enable_flow_ctrl 0 enable_if 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(6)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_NDX_ID,
    "xaui_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  xaui_ndx:\r\n"
    "  XAUI ID. Range 0 - 1.  \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if xaui_info_set xaui_ndx 0 tx_vodctrl 0 tx_preemp_2t 0\r\n"
    "  tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0\r\n"
    "  enable_flow_ctrl 0 enable_if 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(6)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_SERDES_INFO_TX_VODCTRL_ID,
    "tx_vodctrl",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  xaui_info.serdes_info.tx_vodctrl:\r\n"
    "  Voltage Output Differential Control. Range 0 - 7. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if xaui_info_set xaui_ndx 0 tx_vodctrl 0 tx_preemp_2t 0\r\n"
    "  tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0\r\n"
    "  enable_flow_ctrl 0 enable_if 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_SERDES_INFO_TX_PREEMP_2T_ID,
    "tx_preemp_2t",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  xaui_info.serdes_info.tx_preemp_2t:\r\n"
    "  2nd Post tap Pre-emphasis Control. Range 0 - 15. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if xaui_info_set xaui_ndx 0 tx_vodctrl 0 tx_preemp_2t 0\r\n"
    "  tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0\r\n"
    "  enable_flow_ctrl 0 enable_if 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_SERDES_INFO_TX_PREEMP_1T_ID,
    "tx_preemp_1t",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  xaui_info.serdes_info.tx_preemp_1t:\r\n"
    "  1st Post tap Pre-emphasis Control. Range 0 - 15. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if xaui_info_set xaui_ndx 0 tx_vodctrl 0 tx_preemp_2t 0\r\n"
    "  tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0\r\n"
    "  enable_flow_ctrl 0 enable_if 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_SERDES_INFO_TX_PREEMP_0T_ID,
    "tx_preemp_0t",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  xaui_info.serdes_info.tx_preemp_0t:\r\n"
    "  Pre tap Pre-emphasis Control. Range 0 - 15. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if xaui_info_set xaui_ndx 0 tx_vodctrl 0 tx_preemp_2t 0\r\n"
    "  tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0\r\n"
    "  enable_flow_ctrl 0 enable_if 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_SERDES_INFO_RX_EQ_DC_GAIN_ID,
    "rx_eq_dc_gain",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  xaui_info.serdes_info.rx_eq_dc_gain:\r\n"
    "  Equalizer DC Gain Control. Range 0 - 3. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if xaui_info_set xaui_ndx 0 tx_vodctrl 0 tx_preemp_2t 0\r\n"
    "  tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0\r\n"
    "  enable_flow_ctrl 0 enable_if 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_SERDES_INFO_RX_EQ_CTRL_ID,
    "rx_eq_ctrl",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  xaui_info.serdes_info.rx_eq_ctrl:\r\n"
    "  Equalization Control. Range 0 - 15.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if xaui_info_set xaui_ndx 0 tx_vodctrl 0 tx_preemp_2t 0\r\n"
    "  tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0\r\n"
    "  enable_flow_ctrl 0 enable_if 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_ENABLE_BCT_ID,
    "enable_bct",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  xaui_info.enable_bct:\r\n"
    "  Enable/disable BCT (Burst Control Tag). When Set to TRUE the XAUI\r\n"
    "  receive/transmit BCT from/to Soc_petra\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if xaui_info_set xaui_ndx 0 tx_vodctrl 0 tx_preemp_2t 0\r\n"
    "  tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0\r\n"
    "  enable_flow_ctrl 0 enable_if 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_ENABLE_FLOW_CTRL_ID,
    "enable_flow_ctrl",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  xaui_info.enable_flow_ctrl:\r\n"
    "  Enable/disable Ingress Flow Control.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if xaui_info_set xaui_ndx 0 tx_vodctrl 0 tx_preemp_2t 0\r\n"
    "  tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0\r\n"
    "  enable_flow_ctrl 0 enable_if 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_XAUI_INFO_SET_XAUI_INFO_SET_XAUI_INFO_ENABLE_IF_ID,
    "enable_if",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  xaui_info.enable_if:\r\n"
    "  Enable/disable the interface.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if xaui_info_set xaui_ndx 0 tx_vodctrl 0 tx_preemp_2t 0\r\n"
    "  tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0\r\n"
    "  enable_flow_ctrl 0 enable_if 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(6)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_XAUI_INFO_GET_XAUI_INFO_GET_ID,
    "xaui_info_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the XAUI configuration.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if xaui_info_get xaui_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(7)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_XAUI_INFO_GET_XAUI_INFO_GET_XAUI_NDX_ID,
    "xaui_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  xaui_ndx:\r\n"
    "  XAUI ID. Range 0 - 1.  \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if xaui_info_get xaui_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(7)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_ID,
    "spaui_info_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the SPAUI configuration.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if spaui_info_set spaui_ndx 0 tx_vodctrl 0 tx_preemp_2t 0\r\n"
    "  tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(8)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_SPAUI_NDX_ID,
    "spaui_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  spaui_ndx:\r\n"
    "  SPAUI ID. Range: 0 - 1.  \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if spaui_info_set spaui_ndx 0 tx_vodctrl 0 tx_preemp_2t 0\r\n"
    "  tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(8)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_SPAUI_INFO_SERDES_INFO_TX_VODCTRL_ID,
    "tx_vodctrl",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  spaui_info.serdes_info.tx_vodctrl:\r\n"
    "  Voltage Output Differential Control. Range 0 - 7. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if spaui_info_set spaui_ndx 0 tx_vodctrl 0 tx_preemp_2t 0\r\n"
    "  tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_SPAUI_INFO_SERDES_INFO_TX_PREEMP_2T_ID,
    "tx_preemp_2t",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  spaui_info.serdes_info.tx_preemp_2t:\r\n"
    "  2nd Post tap Pre-emphasis Control. Range 0 - 15. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if spaui_info_set spaui_ndx 0 tx_vodctrl 0 tx_preemp_2t 0\r\n"
    "  tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_SPAUI_INFO_SERDES_INFO_TX_PREEMP_1T_ID,
    "tx_preemp_1t",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  spaui_info.serdes_info.tx_preemp_1t:\r\n"
    "  1st Post tap Pre-emphasis Control. Range 0 - 15. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if spaui_info_set spaui_ndx 0 tx_vodctrl 0 tx_preemp_2t 0\r\n"
    "  tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_SPAUI_INFO_SERDES_INFO_TX_PREEMP_0T_ID,
    "tx_preemp_0t",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  spaui_info.serdes_info.tx_preemp_0t:\r\n"
    "  Pre tap Pre-emphasis Control. Range 0 - 15. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if spaui_info_set spaui_ndx 0 tx_vodctrl 0 tx_preemp_2t 0\r\n"
    "  tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_SPAUI_INFO_SERDES_INFO_RX_EQ_DC_GAIN_ID,
    "rx_eq_dc_gain",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  spaui_info.serdes_info.rx_eq_dc_gain:\r\n"
    "  Equalizer DC Gain Control. Range 0 - 3. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if spaui_info_set spaui_ndx 0 tx_vodctrl 0 tx_preemp_2t 0\r\n"
    "  tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_SPAUI_INFO_SERDES_INFO_RX_EQ_CTRL_ID,
    "rx_eq_ctrl",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  spaui_info.serdes_info.rx_eq_ctrl:\r\n"
    "  Equalization Control. Range 0 - 15.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if spaui_info_set spaui_ndx 0 tx_vodctrl 0 tx_preemp_2t 0\r\n"
    "  tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_SPAUI_INFO_SET_SPAUI_INFO_SET_SPAUI_INFO_ENABLE_BCT_ID,
    "enable_bct",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  spaui_info.enable_bct:\r\n"
    "  Enable/disable BCT (Burst Control Tag). When Set to TRUE the SPAUI\r\n"
    "  receive/transmit BCT from/to Soc_petra\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if spaui_info_set spaui_ndx 0 tx_vodctrl 0 tx_preemp_2t 0\r\n"
    "  tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(8)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_SPAUI_INFO_GET_SPAUI_INFO_GET_ID,
    "spaui_info_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the SPAUI configuration.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if spaui_info_get spaui_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(9)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_HW_IF_SPAUI_INFO_GET_SPAUI_INFO_GET_SPAUI_NDX_ID,
    "spaui_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  spaui_ndx:\r\n"
    "  SPAUI ID. Range: 0 - 1.  \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc hw_if spaui_info_get spaui_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(9)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_ACC_HW_IF_ID,
    "hw_if",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(8)|BIT(9)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } hw_if*/
#ifdef UI_T20E_MGMT/* { mgmt*/
  {
    PARAM_T20E_REGISTER_DEVICE_REGISTER_DEVICE_ID,
    "register_device",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This procedure registers a new device to be taken care of by this device\r\n"
    "  driver. Physical device must be accessible by CPU when this call is made.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt register_device base_address 0 reset_device_ptr 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(10)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_REGISTER_DEVICE_REGISTER_DEVICE_BASE_ADDRESS_ID,
    "base_address",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  base_address:\r\n"
    "  Base address of direct access memory assigned for device's registers. This\r\n"
    "  parameter needs to be specified even if physical access to device is not by\r\n"
    "  direct access memory since all logic, within driver, up to actual physical\r\n"
    "  access, assumes 'virtual' direct access memory. Memory block assigned by this\r\n"
    "  pointer must not overlap other memory blocks in user's system and certainly\r\n"
    "  not memory blocks assigned to other T20E devices using this procedure.  \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt register_device base_address 0 reset_device_ptr 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_REGISTER_DEVICE_REGISTER_DEVICE_RESET_DEVICE_PTR_ID,
    "reset_device_ptr",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  reset_device_ptr:\r\n"
    "  BSP-function for device reset. Refer to 'SOC_SAND_RESET_DEVICE_FUNC_PTR'\r\n"
    "  definition.  \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt register_device base_address 0 reset_device_ptr 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(10)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_UNREGISTER_DEVICE_UNREGISTER_DEVICE_ID,
    "unregister_device",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Undo t20e_unregister_device().\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt unregister_device\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(11)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_ID,
    "operation_mode_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set T20E device operation mode. This defines configurations such as enable\r\n"
    "  support policing/trapping or support for certain headers etc.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt operation_mode_set enable_eep_for_uc_packets 0\r\n"
    "  enable_cud_for_uc_packets 0 enable_cfm_trapping 0 enable_policing 0\r\n"
    "  device_rev_id 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(12)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OP_MODE_ENABLE_EEP_FOR_UC_PACKETS_ID,
    "enable_eep_for_uc_packets",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  op_mode.enable_eep_for_uc_packets:\r\n"
    "  Enable EEP (Egress Editing Pointer) for Unicast Packets in the system Headers\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt operation_mode_set enable_eep_for_uc_packets 0\r\n"
    "  enable_cud_for_uc_packets 0 enable_cfm_trapping 0 enable_policing 0\r\n"
    "  device_rev_id 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(12)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OP_MODE_ENABLE_CUD_FOR_UC_PACKETS_ID,
    "enable_cud_for_uc_packets",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  op_mode.enable_cud_for_uc_packets:\r\n"
    "  Enable CUD (Copy Unique Data) for Unicast Packets in the system Headers\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt operation_mode_set enable_eep_for_uc_packets 0\r\n"
    "  enable_cud_for_uc_packets 0 enable_cfm_trapping 0 enable_policing 0\r\n"
    "  device_rev_id 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(12)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OP_MODE_ENABLE_CFM_TRAPPING_ID,
    "enable_cfm_trapping",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  op_mode.enable_cfm_trapping:\r\n"
    "  Enable CFM Trapping\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt operation_mode_set enable_eep_for_uc_packets 0\r\n"
    "  enable_cud_for_uc_packets 0 enable_cfm_trapping 0 enable_policing 0\r\n"
    "  device_rev_id 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(12)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OP_MODE_ENABLE_POLICING_ID,
    "enable_policing",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  op_mode.enable_policing:\r\n"
    "  Enable Policing\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt operation_mode_set enable_eep_for_uc_packets 0\r\n"
    "  enable_cud_for_uc_packets 0 enable_cfm_trapping 0 enable_policing 0\r\n"
    "  device_rev_id 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(12)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OP_MODE_DEVICE_REV_ID_ID,
    "device_rev_id",
    (PARAM_VAL_RULES *)&T20E_MGMT_REVISION_ID_rule[0],
    (sizeof(T20E_MGMT_REVISION_ID_rule) / sizeof(T20E_MGMT_REVISION_ID_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  op_mode.device_rev_id:\r\n"
    "  The device revision ID.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt operation_mode_set enable_eep_for_uc_packets 0\r\n"
    "  enable_cud_for_uc_packets 0 enable_cfm_trapping 0 enable_policing 0\r\n"
    "  device_rev_id 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(12)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_OPERATION_MODE_GET_OPERATION_MODE_GET_ID,
    "operation_mode_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set T20E device operation mode. This defines configurations such as enable\r\n"
    "  support policing/trapping or support for certain headers etc.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt operation_mode_get\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(13)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_ID,
    "init_sequence_phase1",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Initialize the device, including:1.  Disable Traffic. 2.  Initialize the device\r\n"
    "  tables and registers to default values. 3.  Initialize board-specific hardware\r\n"
    "  interfaces according to configurable information including CPU, DRAM, QDR,\r\n"
    "  NIFs. 4.  Perform basic device initialization. The configuration can be\r\n"
    "  enabled/disabled as passed in 'enable_config'.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase1 mask 0 spaui_index 0 tx_vodctrl 0\r\n"
    "  tx_preemp_2t 0 tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0\r\n"
    "  enable_bct 0 xaui_index 0 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\r\n"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0 enable_flow_ctrl 0\r\n"
    "  enable_if 0 qdr_size_mbit 0 dram_size_mbit 0 acc_per_clk 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(14)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_MASK_ID,
    "mask",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  phase1_info.mask:\r\n"
    "  Mask to select which fields of the hw_adjust to configure. See\r\n"
    "  T20E_MGMT_PHASE1_HW_ADJUST_SELECT.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase1 mask 0 spaui_index 0 tx_vodctrl 0\r\n"
    "  tx_preemp_2t 0 tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0\r\n"
    "  enable_bct 0 xaui_index 0 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\r\n"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0 enable_flow_ctrl 0\r\n"
    "  enable_if 0 qdr_size_mbit 0 dram_size_mbit 0 acc_per_clk 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_SPAUI_INDEX_ID,
    "spaui_index",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  spaui_index:\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase1 mask 0 spaui_index 0 tx_vodctrl 0\r\n"
    "  tx_preemp_2t 0 tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0\r\n"
    "  enable_bct 0 xaui_index 0 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\r\n"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0 enable_flow_ctrl 0\r\n"
    "  enable_if 0 qdr_size_mbit 0 dram_size_mbit 0 acc_per_clk 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_SPAUI_SERDES_INFO_TX_VODCTRL_ID,
    "tx_vodctrl",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  phase1_info.hw_adjust.nif.spaui[ prm_spaui_index].serdes_info.tx_vodctrl:\r\n"
    "  Voltage Output Differential Control. Range 0 - 7. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase1 mask 0 spaui_index 0 tx_vodctrl 0\r\n"
    "  tx_preemp_2t 0 tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0\r\n"
    "  enable_bct 0 xaui_index 0 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\r\n"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0 enable_flow_ctrl 0\r\n"
    "  enable_if 0 qdr_size_mbit 0 dram_size_mbit 0 acc_per_clk 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_SPAUI_SERDES_INFO_TX_PREEMP_2T_ID,
    "tx_preemp_2t",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  phase1_info.hw_adjust.nif.spaui[ prm_spaui_index].serdes_info.tx_preemp_2t:\r\n"
    "  2nd Post tap Pre-emphasis Control. Range 0 - 15. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase1 mask 0 spaui_index 0 tx_vodctrl 0\r\n"
    "  tx_preemp_2t 0 tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0\r\n"
    "  enable_bct 0 xaui_index 0 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\r\n"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0 enable_flow_ctrl 0\r\n"
    "  enable_if 0 qdr_size_mbit 0 dram_size_mbit 0 acc_per_clk 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_SPAUI_SERDES_INFO_TX_PREEMP_1T_ID,
    "tx_preemp_1t",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  phase1_info.hw_adjust.nif.spaui[ prm_spaui_index].serdes_info.tx_preemp_1t:\r\n"
    "  1st Post tap Pre-emphasis Control. Range 0 - 15. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase1 mask 0 spaui_index 0 tx_vodctrl 0\r\n"
    "  tx_preemp_2t 0 tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0\r\n"
    "  enable_bct 0 xaui_index 0 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\r\n"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0 enable_flow_ctrl 0\r\n"
    "  enable_if 0 qdr_size_mbit 0 dram_size_mbit 0 acc_per_clk 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_SPAUI_SERDES_INFO_TX_PREEMP_0T_ID,
    "tx_preemp_0t",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  phase1_info.hw_adjust.nif.spaui[ prm_spaui_index].serdes_info.tx_preemp_0t:\r\n"
    "  Pre tap Pre-emphasis Control. Range 0 - 15. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase1 mask 0 spaui_index 0 tx_vodctrl 0\r\n"
    "  tx_preemp_2t 0 tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0\r\n"
    "  enable_bct 0 xaui_index 0 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\r\n"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0 enable_flow_ctrl 0\r\n"
    "  enable_if 0 qdr_size_mbit 0 dram_size_mbit 0 acc_per_clk 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_SPAUI_SERDES_INFO_RX_EQ_DC_GAIN_ID,
    "rx_eq_dc_gain",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  phase1_info.hw_adjust.nif.spaui[ prm_spaui_index].serdes_info.rx_eq_dc_gain:\r\n"
    "  Equalizer DC Gain Control. Range 0 - 3. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase1 mask 0 spaui_index 0 tx_vodctrl 0\r\n"
    "  tx_preemp_2t 0 tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0\r\n"
    "  enable_bct 0 xaui_index 0 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\r\n"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0 enable_flow_ctrl 0\r\n"
    "  enable_if 0 qdr_size_mbit 0 dram_size_mbit 0 acc_per_clk 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_SPAUI_SERDES_INFO_RX_EQ_CTRL_ID,
    "rx_eq_ctrl",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  phase1_info.hw_adjust.nif.spaui[ prm_spaui_index].serdes_info.rx_eq_ctrl:\r\n"
    "  Equalization Control. Range 0 - 15.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase1 mask 0 spaui_index 0 tx_vodctrl 0\r\n"
    "  tx_preemp_2t 0 tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0\r\n"
    "  enable_bct 0 xaui_index 0 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\r\n"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0 enable_flow_ctrl 0\r\n"
    "  enable_if 0 qdr_size_mbit 0 dram_size_mbit 0 acc_per_clk 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_SPAUI_ENABLE_BCT_ID,
    "enable_bct",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  phase1_info.hw_adjust.nif.spaui[ prm_spaui_index].enable_bct:\r\n"
    "  Enable/disable BCT (Burst Control Tag). When Set to TRUE the SPAUI\r\n"
    "  receive/transmit BCT from/to Soc_petra\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase1 mask 0 spaui_index 0 tx_vodctrl 0\r\n"
    "  tx_preemp_2t 0 tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0\r\n"
    "  enable_bct 0 xaui_index 0 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\r\n"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0 enable_flow_ctrl 0\r\n"
    "  enable_if 0 qdr_size_mbit 0 dram_size_mbit 0 acc_per_clk 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_INDEX_ID,
    "xaui_index",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  xaui_index:\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase1 mask 0 spaui_index 0 tx_vodctrl 0\r\n"
    "  tx_preemp_2t 0 tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0\r\n"
    "  enable_bct 0 xaui_index 0 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\r\n"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0 enable_flow_ctrl 0\r\n"
    "  enable_if 0 qdr_size_mbit 0 dram_size_mbit 0 acc_per_clk 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_SERDES_INFO_TX_VODCTRL_ID,
    "tx_vodctrl",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  phase1_info.hw_adjust.nif.xaui[ prm_xaui_index].serdes_info.tx_vodctrl:\r\n"
    "  Voltage Output Differential Control. Range 0 - 7. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase1 mask 0 spaui_index 0 tx_vodctrl 0\r\n"
    "  tx_preemp_2t 0 tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0\r\n"
    "  enable_bct 0 xaui_index 0 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\r\n"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0 enable_flow_ctrl 0\r\n"
    "  enable_if 0 qdr_size_mbit 0 dram_size_mbit 0 acc_per_clk 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_SERDES_INFO_TX_PREEMP_2T_ID,
    "tx_preemp_2t",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  phase1_info.hw_adjust.nif.xaui[ prm_xaui_index].serdes_info.tx_preemp_2t:\r\n"
    "  2nd Post tap Pre-emphasis Control. Range 0 - 15. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase1 mask 0 spaui_index 0 tx_vodctrl 0\r\n"
    "  tx_preemp_2t 0 tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0\r\n"
    "  enable_bct 0 xaui_index 0 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\r\n"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0 enable_flow_ctrl 0\r\n"
    "  enable_if 0 qdr_size_mbit 0 dram_size_mbit 0 acc_per_clk 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_SERDES_INFO_TX_PREEMP_1T_ID,
    "tx_preemp_1t",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  phase1_info.hw_adjust.nif.xaui[ prm_xaui_index].serdes_info.tx_preemp_1t:\r\n"
    "  1st Post tap Pre-emphasis Control. Range 0 - 15. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase1 mask 0 spaui_index 0 tx_vodctrl 0\r\n"
    "  tx_preemp_2t 0 tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0\r\n"
    "  enable_bct 0 xaui_index 0 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\r\n"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0 enable_flow_ctrl 0\r\n"
    "  enable_if 0 qdr_size_mbit 0 dram_size_mbit 0 acc_per_clk 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_SERDES_INFO_TX_PREEMP_0T_ID,
    "tx_preemp_0t",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  phase1_info.hw_adjust.nif.xaui[ prm_xaui_index].serdes_info.tx_preemp_0t:\r\n"
    "  Pre tap Pre-emphasis Control. Range 0 - 15. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase1 mask 0 spaui_index 0 tx_vodctrl 0\r\n"
    "  tx_preemp_2t 0 tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0\r\n"
    "  enable_bct 0 xaui_index 0 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\r\n"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0 enable_flow_ctrl 0\r\n"
    "  enable_if 0 qdr_size_mbit 0 dram_size_mbit 0 acc_per_clk 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_SERDES_INFO_RX_EQ_DC_GAIN_ID,
    "rx_eq_dc_gain",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  phase1_info.hw_adjust.nif.xaui[ prm_xaui_index].serdes_info.rx_eq_dc_gain:\r\n"
    "  Equalizer DC Gain Control. Range 0 - 3. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase1 mask 0 spaui_index 0 tx_vodctrl 0\r\n"
    "  tx_preemp_2t 0 tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0\r\n"
    "  enable_bct 0 xaui_index 0 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\r\n"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0 enable_flow_ctrl 0\r\n"
    "  enable_if 0 qdr_size_mbit 0 dram_size_mbit 0 acc_per_clk 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_SERDES_INFO_RX_EQ_CTRL_ID,
    "rx_eq_ctrl",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  phase1_info.hw_adjust.nif.xaui[ prm_xaui_index].serdes_info.rx_eq_ctrl:\r\n"
    "  Equalization Control. Range 0 - 15.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase1 mask 0 spaui_index 0 tx_vodctrl 0\r\n"
    "  tx_preemp_2t 0 tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0\r\n"
    "  enable_bct 0 xaui_index 0 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\r\n"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0 enable_flow_ctrl 0\r\n"
    "  enable_if 0 qdr_size_mbit 0 dram_size_mbit 0 acc_per_clk 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_ENABLE_BCT_ID,
    "enable_bct",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  phase1_info.hw_adjust.nif.xaui[ prm_xaui_index].enable_bct:\r\n"
    "  Enable/disable BCT (Burst Control Tag). When Set to TRUE the XAUI\r\n"
    "  receive/transmit BCT from/to Soc_petra\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase1 mask 0 spaui_index 0 tx_vodctrl 0\r\n"
    "  tx_preemp_2t 0 tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0\r\n"
    "  enable_bct 0 xaui_index 0 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\r\n"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0 enable_flow_ctrl 0\r\n"
    "  enable_if 0 qdr_size_mbit 0 dram_size_mbit 0 acc_per_clk 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_ENABLE_FLOW_CTRL_ID,
    "enable_flow_ctrl",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  phase1_info.hw_adjust.nif.xaui[ prm_xaui_index].enable_flow_ctrl:\r\n"
    "  Enable/disable Ingress Flow Control.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase1 mask 0 spaui_index 0 tx_vodctrl 0\r\n"
    "  tx_preemp_2t 0 tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0\r\n"
    "  enable_bct 0 xaui_index 0 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\r\n"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0 enable_flow_ctrl 0\r\n"
    "  enable_if 0 qdr_size_mbit 0 dram_size_mbit 0 acc_per_clk 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_NIF_XAUI_ENABLE_IF_ID,
    "enable_if",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  phase1_info.hw_adjust.nif.xaui[ prm_xaui_index].enable_if:\r\n"
    "  Enable/disable the interface.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase1 mask 0 spaui_index 0 tx_vodctrl 0\r\n"
    "  tx_preemp_2t 0 tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0\r\n"
    "  enable_bct 0 xaui_index 0 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\r\n"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0 enable_flow_ctrl 0\r\n"
    "  enable_if 0 qdr_size_mbit 0 dram_size_mbit 0 acc_per_clk 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_QDR_QDR_SIZE_MBIT_ID,
    "qdr_size_mbit",
    (PARAM_VAL_RULES *)&T20E_HW_IF_QDR_SIZE_MBIT_rule[0],
    (sizeof(T20E_HW_IF_QDR_SIZE_MBIT_rule) / sizeof(T20E_HW_IF_QDR_SIZE_MBIT_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  phase1_info.hw_adjust.qdr.qdr_size_mbit:\r\n"
    "  Total QDR SRAM memory size Units: Mbits. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase1 mask 0 spaui_index 0 tx_vodctrl 0\r\n"
    "  tx_preemp_2t 0 tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0\r\n"
    "  enable_bct 0 xaui_index 0 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\r\n"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0 enable_flow_ctrl 0\r\n"
    "  enable_if 0 qdr_size_mbit 0 dram_size_mbit 0 acc_per_clk 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_DRAM_DRAM_SIZE_MBIT_ID,
    "dram_size_mbit",
    (PARAM_VAL_RULES *)&T20E_HW_IF_DRAM_SIZE_MBIT_rule[0],
    (sizeof(T20E_HW_IF_DRAM_SIZE_MBIT_rule) / sizeof(T20E_HW_IF_DRAM_SIZE_MBIT_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  phase1_info.hw_adjust.dram.dram_size_mbit:\r\n"
    "  Total QDR SRAM memory size Units: Mbits. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase1 mask 0 spaui_index 0 tx_vodctrl 0\r\n"
    "  tx_preemp_2t 0 tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0\r\n"
    "  enable_bct 0 xaui_index 0 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\r\n"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0 enable_flow_ctrl 0\r\n"
    "  enable_if 0 qdr_size_mbit 0 dram_size_mbit 0 acc_per_clk 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_PHASE1_INFO_HW_ADJUST_CPU_ACC_PER_CLK_ID,
    "acc_per_clk",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  phase1_info.hw_adjust.cpu.acc_per_clk:\r\n"
    "  Number of system clocks between two CPU accesses, controls the allocated\r\n"
    "  bandwidth to CPU\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase1 mask 0 spaui_index 0 tx_vodctrl 0\r\n"
    "  tx_preemp_2t 0 tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0\r\n"
    "  enable_bct 0 xaui_index 0 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\r\n"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0 enable_flow_ctrl 0\r\n"
    "  enable_if 0 qdr_size_mbit 0 dram_size_mbit 0 acc_per_clk 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_SILENT_ID,
    "silent",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  silent:\r\n"
    "  If TRUE, progress printing will be suppressed.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase1 mask 0 spaui_index 0 tx_vodctrl 0\r\n"
    "  tx_preemp_2t 0 tx_preemp_1t 0 tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0\r\n"
    "  enable_bct 0 xaui_index 0 tx_vodctrl 0 tx_preemp_2t 0 tx_preemp_1t 0\r\n"
    "  tx_preemp_0t 0 rx_eq_dc_gain 0 rx_eq_ctrl 0 enable_bct 0 enable_flow_ctrl 0\r\n"
    "  enable_if 0 qdr_size_mbit 0 dram_size_mbit 0 acc_per_clk 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(14)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE2_INIT_SEQUENCE_PHASE2_ID,
    "init_sequence_phase2",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Configurations needed after out of reset.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase2 tbd 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(15)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE2_INIT_SEQUENCE_PHASE2_PHASE2_INFO_TBD_ID,
    "tbd",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  phase2_info.tbd:\r\n"
    "  TBD, currently empty.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase2 tbd 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(15)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_INIT_SEQUENCE_PHASE2_INIT_SEQUENCE_PHASE2_SILENT_ID,
    "silent",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  silent:\r\n"
    "  If TRUE, progress printing will be suppressed.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt init_sequence_phase2 tbd 0 silent 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(15)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_ENABLE_TRAFFIC_SET_ENABLE_TRAFFIC_SET_ID,
    "enable_traffic_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Enable / Disable the device from receiving and transmitting traffic.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt enable_traffic_set enable 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(16)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_ENABLE_TRAFFIC_SET_ENABLE_TRAFFIC_SET_ENABLE_ID,
    "enable",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  enable:\r\n"
    "  Enable / disable traffic.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt enable_traffic_set enable 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(16)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_ENABLE_TRAFFIC_GET_ENABLE_TRAFFIC_GET_ID,
    "enable_traffic_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Enable / Disable the device from receiving and transmitting traffic.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt enable_traffic_get\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(17)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_PCKT_SIZE_RANGE_SET_PCKT_SIZE_RANGE_SET_ID,
    "pckt_size_range_set",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the boundaries (minimal/maximal allowed size) for the expected packets.\r\n"
    "  The limitation can be performed based on the packet size before or after the\r\n"
    "  ingress editing (external and internal configuration mode, accordingly).\r\n"
    "  Packets outside the specified range are dropped.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt pckt_size_range_set max 0 min 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(18)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_PCKT_SIZE_RANGE_SET_PCKT_SIZE_RANGE_SET_SIZE_RANGE_MAX_ID,
    "max",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  size_range.max:\r\n"
    "  Maximal allowed packet size, in bytes. a A frame, exceeding the specified\r\n"
    "  octets is received, it is truncated and a length error indication is\r\n"
    "  given.The length includes the headers and FCS field, but not the preamble.\r\n"
    "  Range 64-9022. (bytes)\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt pckt_size_range_set max 0 min 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_PCKT_SIZE_RANGE_SET_PCKT_SIZE_RANGE_SET_SIZE_RANGE_MIN_ID,
    "min",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  size_range.min:\r\n"
    "  Minimal allowed packet size, in bytes. Packets below this value will be\r\n"
    "  dropped. Must be 64. Range 64-64.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt pckt_size_range_set max 0 min 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(18)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_MGMT_PCKT_SIZE_RANGE_GET_PCKT_SIZE_RANGE_GET_ID,
    "pckt_size_range_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the boundaries (minimal/maximal allowed size) for the expected packets.\r\n"
    "  The limitation can be performed based on the packet size before or after the\r\n"
    "  ingress editing (external and internal configuration mode, accordingly).\r\n"
    "  Packets outside the specified range are dropped.\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc mgmt pckt_size_range_get\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(19)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_ACC_MGMT_ID,
    "mgmt",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(10)|BIT(11)|BIT(12)|BIT(13)|BIT(14)|BIT(15)|BIT(16)|BIT(17)|BIT(18)|BIT(19)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } mgmt*/
#ifdef UI_T20E_DIAGNOSTICS/* { diagnostics*/
  {
    PARAM_T20E_DIAG_ERRORS_GET_DIAG_ERRORS_GET_ID,
    "errors_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get errors and indications resulted by packet processing. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc diagnostics errors_get error_select_ndx 1\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(20)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_DIAG_ERRORS_GET_DIAG_ERRORS_GET_ERROR_SELECT_NDX_ID,
    "error_select_ndx",
    (PARAM_VAL_RULES *)&T20E_DIAG_ERR_SELECT_rule[0],
    (sizeof(T20E_DIAG_ERR_SELECT_rule) / sizeof(T20E_DIAG_ERR_SELECT_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  error_select_ndx:\r\n"
    "  select category of errors to get, may be (ingress/egress/pp/mact etc..\r\n"
    "  erros), use T20E_DIAG_ERR_SELECT_ALL to get all errors.  \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc diagnostics errors_get error_select_ndx 1\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(20)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_DIAG_COUNTER_GET_DIAG_COUNTER_GET_ID,
    "counter_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get the value of a specific counter. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc diagnostics counter_get counter_type_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(21)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_DIAG_COUNTER_GET_DIAG_COUNTER_GET_COUNTER_TYPE_NDX_ID,
    "counter_type_ndx",
    (PARAM_VAL_RULES *)&T20E_DIAG_CNT_TYPE_rule[0],
    (sizeof(T20E_DIAG_CNT_TYPE_rule) / sizeof(T20E_DIAG_CNT_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  counter_type_ndx:\r\n"
    "  Counter type.  \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc diagnostics counter_get counter_type_ndx 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(21)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_DIAG_ALL_COUNTERS_GET_DIAG_ALL_COUNTERS_GET_ID,
    "all_counters_get",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get the value of all Diagnostic counters. \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc diagnostics all_counters_get\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(22)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_DIAG_ALL_MIB_COUNTERS_PRINT_DIAG_ALL_COUNTERS_PRINT_ID,
    "all_mib_counters_print",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Print MIB counters\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc diagnostics mib_counters_print\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(24)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_DIAG_ALL_MIB_COUNTERS_PRINT_DIAG_ALL_COUNTERS_PRINT_NIF_NDX_ID,
    "nif_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Print MIB counters\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc diagnostics mib_counters_print\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(25)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_DIAG_MIB_COUNTER_PRINT_DIAG_ALL_COUNTERS_PRINT_ID,
    "mib_counter_print",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Print MIB counters\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc diagnostics mib_counters_print\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(25)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_DIAG_MIB_COUNTER_PRINT_DIAG_ALL_COUNTERS_PRINT_NIF_NDX_ID,
    "nif_ndx",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Print MIB counters\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc diagnostics mib_counters_print\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(25)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_DIAG_MIB_COUNTER_PRINT_DIAG_ALL_COUNTERS_PRINT_COUNTER_TYPE_NDX_ID,
    "counter_type",
    (PARAM_VAL_RULES *)&T20E_DIAG_MIB_COUNTER_TYPE_rule[0],
    (sizeof(T20E_DIAG_MIB_COUNTER_TYPE_rule) / sizeof(T20E_DIAG_MIB_COUNTER_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Print MIB counters\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc diagnostics mib_counters_print\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(25)},
    4,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_DIAG_ALL_COUNTERS_PRINT_DIAG_ALL_COUNTERS_PRINT_ID,
    "all_counters_print",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Print all counters\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc diagnostics all_counters_print print_zero 0 print_long 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(23)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_DIAG_ALL_COUNTERS_PRINT_DIAG_ALL_COUNTERS_PRINT_COUNTERS_INDEX_ID,
    "print_zero",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  counters_index:\r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc diagnostics all_counters_print print_zero 0 print_long 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(23)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_DIAG_ALL_COUNTERS_PRINT_DIAG_ALL_COUNTERS_PRINT_COUNTERS_ID,
    "print_long",
    (PARAM_VAL_RULES *)&t20e_acc_free_vals[0],
    (sizeof(t20e_acc_free_vals) / sizeof(t20e_acc_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  counters[ prm_counters_index]:\r\n"
    "  T20E_DIAG_NOF_COUNTERS]-     Include the counters values  \r\n"
    "",
#if UI_T20E_ACC_ADD_EXAMPLE
    "Examples:\r\n"
    "  t20e_acc diagnostics all_counters_print print_zero 0 print_long 0\r\n"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(23)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_T20E_ACC_DIAGNOSTICS_ID,
    "diagnostics",
    (PARAM_VAL_RULES *)&t20e_acc_empty_vals[0],
    (sizeof(t20e_acc_empty_vals) / sizeof(t20e_acc_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, 0, BIT(20)|BIT(21)|BIT(23)|BIT(24)|BIT(25)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } diagnostics*/

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

/* } __UI_ROM_DEFI_T20E_ACC_INCLUDED__*/
#endif
