/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __UI_ROM_DEFI_FE600_API_INCLUDED__
/* { */
#define __UI_ROM_DEFI_FE600_API_INCLUDED__
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <appl/dpp/UserInterface/ui_pure_defi_fe600_api.h>
#include <soc/dpp/SOC_SAND_FE600/fe600_api_mgmt.h>
#include <soc/dpp/SOC_SAND_FE600/fe600_api_status.h>
#include <soc/dpp/SOC_SAND_FE600/fe600_api_links.h>
#include <soc/dpp/SOC_SAND_FE600/fe600_api_links_topology.h>
#include <soc/dpp/SOC_SAND_FE600/fe600_api_serdes.h>
#include <soc/dpp/SOC_SAND_FE600/fe600_api_serdes_utils.h>
#include <soc/dpp/SOC_SAND_FE600/fe600_api_cell.h>
 /*
  * ENUM RULES
  * {
  */
/********************************************************/
/********************************************************/
EXTERN CONST
   PARAM_VAL_RULES
     Fe600_addr_and_data[]
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
        4,
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
     FE600_CELL_LINKS_NUMBER_rule[]
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
        FE600_CELL_NOF_LINKS_IN_PATH_LINKS,
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
     FE600_CELL_CPU2CPU_DATA_IN_rule[]
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
        SOC_SAND_DATA_CELL_PAYLOAD_IN_UINT32S,
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
      FE600_SAND_DEVICE_ENTITY_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "FE1",
    {
      {
        SOC_SAND_FE1_ENTITY,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SAND_DEVICE_ENTITY.SOC_SAND_FE1_ENTITY:\r\n"
        "  FE1 Entity.\r\n"
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
    "FE2",
    {
      {
        SOC_SAND_FE2_ENTITY,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SAND_DEVICE_ENTITY.SOC_SAND_FE2_ENTITY:\r\n"
        "  FE2 Entity.\r\n"
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
    "FE3",
    {
      {
        SOC_SAND_FE3_ENTITY,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SAND_DEVICE_ENTITY.SOC_SAND_FE3_ENTITY:\r\n"
        "  FE3 Entity.\r\n"
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
      "FOP",
    {
      {
        SOC_SAND_FOP_ENTITY,
          /*
          * Casting added here just to keep the stupid compiler silent.
          */
          (long)""
          "  FE600_SAND_DEVICE_ENTITY.SOC_SAND_FOP_ENTITY:\r\n"
          "  FIP Entity.\r\n"
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
     FE600_DIAG_SOFT_ERROR_PATTERN_rule[] 
#ifdef INIT 
   = 
{ 
  { 
    VAL_SYMBOL, 
    "one", 
    { 
      { 
        FE600_DIAG_SOFT_ERROR_PATTERN_ONE, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  FE600_DIAG_SOFT_ERROR_PATTERN.FE600_DIAG_SOFT_ERROR_PATTERN_ONE:\n\r"
        "  Fill the data to write by 1111... (Bits). \n\r"
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
    "zero", 
    { 
      { 
        FE600_DIAG_SOFT_ERROR_PATTERN_ZERO, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  FE600_DIAG_SOFT_ERROR_PATTERN.FE600_DIAG_SOFT_ERROR_PATTERN_ZERO:\n\r"
        "  Fill the data to write by 000... (Bits). \n\r"
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
    "diff1", 
    { 
      { 
        FE600_DIAG_SOFT_ERROR_PATTERN_DIFF1, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  FE600_DIAG_SOFT_ERROR_PATTERN.FE600_DIAG_SOFT_ERROR_PATTERN_DIFF1:\n\r"
        "  Fill the data to write by 1010101... (Bits). \n\r"
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
    "diff2", 
    { 
      { 
        FE600_DIAG_SOFT_ERROR_PATTERN_DIFF2, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  FE600_DIAG_SOFT_ERROR_PATTERN.FE600_DIAG_SOFT_ERROR_PATTERN_DIFF2:\n\r"
        "  Fill the data to write by 0101010... (Bits). \n\r"
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
     FE600_DIAG_SOFT_COUNT_TYPE_rule[] 
#ifdef INIT 
   = 
{ 
  { 
    VAL_SYMBOL, 
    "fast", 
    { 
      { 
        FE600_DIAG_SOFT_COUNT_TYPE_FAST, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  FE600_DIAG_SOFT_COUNT_TYPE.FE600_DIAG_SOFT_COUNT_TYPE_FAST:\n\r"
        "  The error count is done per SMS (but not per SONE).\n\r"
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
    "complete", 
    { 
      { 
        FE600_DIAG_SOFT_COUNT_TYPE_COMPLETE, 
          /* 
           * Casting added here just to keep the compiler silent. 
           */ 
        (long)"" 
        "  FE600_DIAG_SOFT_COUNT_TYPE.FE600_DIAG_SOFT_COUNT_TYPE_COMPLETE:\n\r"
        "  The error count is done per SMS, and per SONE.\n\r"
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
/********************************************************/

EXTERN CONST
   PARAM_VAL_RULES
      FE600_FE600_SRD_CNT_SRC_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "MAC",
    {
      {
        FE600_SRD_CNT_SRC_CRC_MAC,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_CNT_SRC.FE600_SRD_CNT_SRC_MAC:\r\n"
        "  Counting source as MAC, no FEC enable.\r\n"
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
    "PRBS",
    {
      {
        FE600_SRD_CNT_SRC_PRBS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_CNT_SRC.FE600_SRD_CNT_SRC_PRBS:\r\n"
        "  Counting source as PRBS, the prbs_ parameters must be set.\r\n"
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
    "BER",
    {
      {
        FE600_SRD_CNT_SRC_FEC_BER,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_CNT_SRC.FE600_SRD_CNT_SRC_BER:\r\n"
        "  BER - FEC enable.\r\n"
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
    "CER",
    {
      {
        FE600_SRD_CNT_SRC_FEC_CER,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_CNT_SRC.FE600_SRD_CNT_SRC_CER:\r\n"
        "  CER - FEC enable.\r\n"
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
/********************************************************/

EXTERN CONST
   PARAM_VAL_RULES
      FE600_FE600_SRD_TRAFFIC_SRC_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "prbs",
    {
      {
        FE600_SRD_TRAFFIC_SRC_PRBS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_TRAFFIC_SRC.FE600_SRD_TRAFFIC_SRC_PRBS:\r\n"
        "  PRBS enable.\r\n"
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
    "ext",
    {
      {
        FE600_SRD_TRAFFIC_SRC_EXT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_TRAFFIC_SRC.FE600_SRD_TRAFFIC_SRC_ExT:\r\n"
        "  External traffic (no PRBS, nor snake) is already generated.\r\n"
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
/********************************************************/

EXTERN CONST
   PARAM_VAL_RULES
     FE600_LINK_STATE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "on",
    {
      {
        FE600_LINK_STATE_ON,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_LINK_STATE.FE600_LINK_STATE_ON:\r\n"
        "  Link is turned on (normal operation).\r\n"
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
    "off",
    {
      {
        FE600_LINK_STATE_OFF,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_LINK_STATE.FE600_LINK_STATE_OFF:\r\n"
        "  Link is turned off.\r\n"
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
      FE600_SAND_DEVICE_ENTITY2_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "FE1",
    {
      {
        SOC_SAND_FE1_ENTITY,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SAND_DEVICE_ENTITY.SOC_SAND_FE1_ENTITY:\r\n"
        "  FE1 Entity.\r\n"
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
    "FE2",
    {
      {
        SOC_SAND_FE2_ENTITY,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SAND_DEVICE_ENTITY.SOC_SAND_FE2_ENTITY:\r\n"
        "  FE2 Entity.\r\n"
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
    "FE3",
    {
      {
        SOC_SAND_FE3_ENTITY,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SAND_DEVICE_ENTITY.SOC_SAND_FE3_ENTITY:\r\n"
        "  FE3 Entity.\r\n"
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

/********************************************************/

EXTERN CONST
   PARAM_VAL_RULES
     FE600_FABRIC_MODE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "single_stage",
    {
      {
        FE600_FABRIC_MODE_SINGLE_STAGE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_FABRIC_MODE.FE600_FABRIC_MODE_SINGLE_STAGE:\r\n"
        "  FE in single stage system\r\n"
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
    "mult_stage_fe2",
    {
      {
        FE600_FABRIC_MODE_MULT_STAGE_FE2,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_FABRIC_MODE.FE600_FABRIC_MODE_MULT_STAGE_FE2:\r\n"
        "  FE in multistage system, operates as FE2\r\n"
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
    "mult_stage_fe13",
    {
      {
        FE600_FABRIC_MODE_MULT_STAGE_FE13,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_FABRIC_MODE.FE600_FABRIC_MODE_MULT_STAGE_FE13:\r\n"
        "  FE in multistage system, operates as FE13\r\n"
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
    "repeater",
    {
      {
        FE600_FABRIC_MODE_REPEATER,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_FABRIC_MODE.FE600_FABRIC_MODE_REPEATER:\r\n"
        "  Repeater Mode\r\n"
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
    "nof_fabric_modes",
    {
      {
        FE600_NOF_FABRIC_MODES,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_FABRIC_MODE.FE600_NOF_FABRIC_MODES:\r\n"
        "  Number of Modes\r\n"
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
     FE600_LOAD_BALANCE_METHOD_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "lb_mtd_normal",
    {
      {
        FE600_LB_MTD_NORMAL,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_LOAD_BALANCE_METHOD.FE600_LB_MTD_NORMAL:\r\n"
        "  Normal Mode. Destination FAP is unreachable only if all the links to that FAP\r\n"
        "  are invalid\r\n"
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
    "lb_mtd_unreachable",
    {
      {
        FE600_LB_MTD_UNREACHABLE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_LOAD_BALANCE_METHOD.FE600_LB_MTD_UNREACHABLE:\r\n"
        "  Destination Unreachable mode. Destination FAP is unreachable when a single\r\n"
        "  FAP towards it is invalid\r\n"
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
    "lb_mtd_load_balance",
    {
      {
        FE600_LB_MTD_LOAD_BALANCE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_LOAD_BALANCE_METHOD.FE600_LB_MTD_LOAD_BALANCE:\r\n"
        "  Balance Input links mode. Destination FAP is partially reachable, according\r\n"
        "  to the percentage of the valid links towards it.This is the recommended mode.\r\n"
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
    "nof_balance_methods",
    {
      {
        FE600_LOAD_NOF_BALANCE_METHODS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_LOAD_BALANCE_METHOD.FE600_LOAD_NOF_BALANCE_METHODS:\r\n"
        "  Number of load balance methodologies\r\n"
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
     FE600_TOPOLOGY_STATE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "issolated",
    {
      {
        FE600_TOPOLOGY_STATE_ISSOLATED,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_TOPOLOGY_STATE.FE600_TOPOLOGY_STATE_ISSOLATED:\r\n"
        "  FE in single stage system\r\n"
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
    "active",
    {
      {
        FE600_TOPOLOGY_STATE_ACTIVE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_TOPOLOGY_STATE.FE600_TOPOLOGY_STATE_ACTIVE:\r\n"
        "  FE in multistage system, operates as FE2\r\n"
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
    "nof_states",
    {
      {
        FE600_TOPOLOGY_NOF_STATES,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_TOPOLOGY_STATE.FE600_TOPOLOGY_NOF_STATES:\r\n"
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
     FE600_TOPOLOGY_STAT_REACH_PRINT_FLAV_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "all",
    {
      {
        FE600_TOPOLOGY_STAT_REACH_PRINT_FLAV_ALL,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_TOPOLOGY_STAT_REACH_PRINT_FLAV.FE600_TOPOLOGY_STAT_REACH_PRINT_FLAV_ALL:\r\n"
        "  Only the active links are printed; the faulty links input is ignored. \r\n"
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
    "unused",
    {
      {
        FE600_TOPOLOGY_STAT_REACH_PRINT_FLAV_UNUSED,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_TOPOLOGY_STAT_REACH_PRINT_FLAV.FE600_TOPOLOGY_STAT_REACH_PRINT_FLAV_UNU\r\n"
        "  ED:\r\n"
        "  For each link and destination, prints if the route is active / faulty /\r\n"
        "  non-exist\r\n"
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
     FE600_GCI_PRIOR_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "low",
    {
      {
        FE600_GCI_PRIOR_LOW,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_GCI_PRIOR.FE600_GCI_PRIOR_LOW:\r\n"
        "  GCI low congestion level\r\n"
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
    "med",
    {
      {
        FE600_GCI_PRIOR_MED,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_GCI_PRIOR.FE600_GCI_PRIOR_MED:\r\n"
        "  GCI medium congestion level\r\n"
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
    "high",
    {
      {
        FE600_GCI_PRIOR_HIGH,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_GCI_PRIOR.FE600_GCI_PRIOR_HIGH:\r\n"
        "  GCI high congestion level\r\n"
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
    "nof_priors",
    {
      {
        FE600_GCI_NOF_PRIORS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_GCI_PRIOR.FE600_GCI_NOF_PRIORS:\r\n"
        "  GCI number of congestion levels\r\n"
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
     FE600_LINK_ERR_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "crc",
    {
      {
        FE600_LINK_ERR_TYPE_CRC,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_LINK_ERR_TYPE.FE600_LINK_ERR_TYPE_CRC:\r\n"
        "  crc_error Flag: Indication of whether a cell with CRC error has been detected.\r\n"
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
    "size",
    {
      {
        FE600_LINK_ERR_TYPE_SIZE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_LINK_ERR_TYPE.FE600_LINK_ERR_TYPE_SIZE:\r\n"
        "  Wrong cell size is detected on MAC receive\r\n"
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
    "misalign",
    {
      {
        FE600_LINK_ERR_TYPE_MISALIGN,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_LINK_ERR_TYPE.FE600_LINK_ERR_TYPE_MISALIGN:\r\n"
        "  misalignment_error Flag: Indication of whether a cell with misalignment error\r\n"
        "  (comma) has been detected.\r\n"
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
    "code_group",
    {
      {
        FE600_LINK_ERR_TYPE_CODE_GROUP,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_LINK_ERR_TYPE.FE600_LINK_ERR_TYPE_CODE_GROUP:\r\n"
        "  code_group_error Flag: Indication of whether a cell with code-group\r\n"
        "  combination error (comma) has been detected.\r\n"
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
     FE600_LINK_INDICATE_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "sig_lock",
    {
      {
        FE600_LINK_INDICATE_TYPE_SIG_LOCK,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_LINK_INDICATE_TYPE.FE600_LINK_INDICATE_TYPE_SIG_LOCK:\r\n"
        "  sig_lock Flag: Indication of whether valid signal has been locked. \r\n"
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
    "accept_cell",
    {
      {
        FE600_LINK_INDICATE_TYPE_ACCEPT_CELL,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_LINK_INDICATE_TYPE.FE600_LINK_INDICATE_TYPE_ACCEPT_CELL:\r\n"
        "  accepting_cells Flag: Indication of whether this link is down or not.\r\n"
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
    "intrnl_fixed",
    {
      {
        FE600_LINK_INDICATE_INTRNL_FIXED,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_LINK_INDICATE_TYPE.FE600_LINK_INDICATE_INTRNL_FIXED:\r\n"
        "  Internally fixed error: Error was detected, but fixed by the FEC mechanism\r\n"
        "  (Always FALSE when FEC is disabled)\r\n"
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
     FE600_LANE_NDX_rule[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FE600_NOF_LINKS-1, 0,
        2,
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
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
        FE600_NOF_LINKS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_NOF_LINKS (96):\r\n"
        "  Acting on all lanes\r\n"
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

EXTERN CONST
   PARAM_VAL_RULES
     FE600_STAT_COUNTER_FLAVOR_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "main_road",
    {
      {
        FE600_STAT_COUNTER_FLAVOR_MAIN_ROAD,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_STAT_COUNTER_FLAVOR.FE600_STAT_COUNTER_FLAVOR_MAIN_ROAD:\r\n"
        "  Main road: Counters that present the packet walk-through\r\n"
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
    "errors",
    {
      {
        FE600_STAT_COUNTER_FLAVOR_ERRORS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_STAT_COUNTER_FLAVOR.FE600_STAT_COUNTER_FLAVOR_ERRORS:\r\n"
        "  Errors: Logical presentation of errors\r\n"
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
    "all",
    {
      {
        FE600_STAT_COUNTER_FLAVOR_ALL,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_STAT_COUNTER_FLAVOR.FE600_STAT_COUNTER_FLAVOR_ALL:\r\n"
        "  All: Raw presentation of all the counters and interrupts\r\n"
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
     FE600_CONNECTION_DIRECTION_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "rx",
    {
      {
        FE600_CONNECTION_DIRECTION_RX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_CONNECTION_DIRECTION.FE600_CONNECTION_DIRECTION_RX:\r\n"
        "  Connection direction - receive (RX).\r\n"
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
    "tx",
    {
      {
        FE600_CONNECTION_DIRECTION_TX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_CONNECTION_DIRECTION.FE600_CONNECTION_DIRECTION_TX:\r\n"
        "  Connection direction - transmit (TX).\r\n"
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
    "both",
    {
      {
        FE600_CONNECTION_DIRECTION_BOTH,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_CONNECTION_DIRECTION.FE600_CONNECTION_DIRECTION_BOTH:\r\n"
        "  Connection direction - both receive and transmit (RX/TX).\r\n"
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
     FE600_SRD_STAR_ID_rule[]
#ifdef INIT
=
{
  {
    VAL_NUMERIC,
      SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
          FE600_SRD_NOF_STAR_IDS - 1,
          0,
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
/********************************************************/

EXTERN CONST
   PARAM_VAL_RULES
     FE600_SRD_DATA_RATE_rule[]
#ifdef INIT
   =
{
    {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        6250000, 99,
        1,
          /*
           * Pointer to a function to call after symbolic value
           * has been accepted. Casting should be 'VAL_PROC_PTR'.
           * Casting to 'long' added here just to keep the stupid
           * compiler silent.
           */
        (long)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
    "mbps_1000_00",
    {
      {
        FE600_SRD_DATA_RATE_1000_00,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_DATA_RATE.FE600_SRD_DATA_RATE_1000_00:\r\n"
        "  SerDes Data Rate: 1000.00 Mbps\r\n"
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
    "mbps_1041_67",
    {
      {
        FE600_SRD_DATA_RATE_1041_67,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_DATA_RATE.FE600_SRD_DATA_RATE_1041_67:\r\n"
        "  SerDes Data Rate: 1041.67 Mbps\r\n"
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
    "mbps_1171_88",
    {
      {
        FE600_SRD_DATA_RATE_1171_88,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_DATA_RATE.FE600_SRD_DATA_RATE_1171_88:\r\n"
        "  SerDes Data Rate: 1171.88 Mbps\r\n"
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
    "mbps_1250_00",
    {
      {
        FE600_SRD_DATA_RATE_1250_00,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_DATA_RATE.FE600_SRD_DATA_RATE_1250_00:\r\n"
        "  SerDes Data Rate: 1250.00 Mbps\r\n"
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
    "mbps_1302_03",
    {
      {
        FE600_SRD_DATA_RATE_1302_03,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_DATA_RATE.FE600_SRD_DATA_RATE_1302_03:\r\n"
        "  SerDes Data Rate: 1302.03 Mbps\r\n"
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
    "mbps_1333_33",
    {
      {
        FE600_SRD_DATA_RATE_1333_33,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_DATA_RATE.FE600_SRD_DATA_RATE_1333_33:\r\n"
        "  SerDes Data Rate: 1333.33 Mbps\r\n"
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
    "mbps_1562_50",
    {
      {
        FE600_SRD_DATA_RATE_1562_50,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_DATA_RATE.FE600_SRD_DATA_RATE_1562_50:\r\n"
        "  SerDes Data Rate: 1562.50 Mbps\r\n"
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
    "mbps_2343_75",
    {
      {
        FE600_SRD_DATA_RATE_2343_75,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_DATA_RATE.FE600_SRD_DATA_RATE_2343_75:\r\n"
        "  SerDes Data Rate: 2343.75 Mbps\r\n"
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
    "mbps_2500_00",
    {
      {
        FE600_SRD_DATA_RATE_2500_00,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_DATA_RATE.FE600_SRD_DATA_RATE_2500_00:\r\n"
        "  SerDes Data Rate: 2500.00 Mbps\r\n"
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
    "mbps_2604_16",
    {
      {
        FE600_SRD_DATA_RATE_2604_16,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_DATA_RATE.FE600_SRD_DATA_RATE_2604_16:\r\n"
        "  SerDes Data Rate: 2604.16 Mbps\r\n"
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
    "mbps_2666_67",
    {
      {
        FE600_SRD_DATA_RATE_2666_67,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_DATA_RATE.FE600_SRD_DATA_RATE_2666_67:\r\n"
        "  SerDes Data Rate: 2666.67 Mbps\r\n"
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
    "mbps_3000_00",
    {
      {
        FE600_SRD_DATA_RATE_3000_00,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_DATA_RATE.FE600_SRD_DATA_RATE_3000_00:\r\n"
        "  SerDes Data Rate: 3000.00 Mbps\r\n"
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
    "mbps_3125_00",
    {
      {
        FE600_SRD_DATA_RATE_3125_00,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_DATA_RATE.FE600_SRD_DATA_RATE_3125_00:\r\n"
        "  SerDes Data Rate: 3125.00 Mbps\r\n"
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
    "mbps_3125_00_FDR",
    {
      {
        FE600_SRD_DATA_RATE_3125_00_FDR,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_DATA_RATE.FE600_SRD_DATA_RATE_3125_00_FDR:\r\n"
        "  SerDes Data Rate: 3125.00 Mbps, using FDR divisor setting\r\n"
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
    "mbps_3750_00",
    {
      {
        FE600_SRD_DATA_RATE_3750_00,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_DATA_RATE.FE600_SRD_DATA_RATE_3750_00:\r\n"
        "  SerDes Data Rate: 3750.00 Mbps\r\n"
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
    "mbps_4000_00",
    {
      {
        FE600_SRD_DATA_RATE_4000_00,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_DATA_RATE.FE600_SRD_DATA_RATE_4000_00:\r\n"
        "  SerDes Data Rate: 4000.00 Mbps\r\n"
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
    "mbps_4687_50",
    {
      {
        FE600_SRD_DATA_RATE_4687_50,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_DATA_RATE.FE600_SRD_DATA_RATE_4687_50:\r\n"
        "  SerDes Data Rate: 4687.50 Mbps\r\n"
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
    "mbps_5000_00",
    {
      {
        FE600_SRD_DATA_RATE_5000_00,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_DATA_RATE.FE600_SRD_DATA_RATE_5000_00:\r\n"
        "  SerDes Data Rate: 5000.00 Mbps\r\n"
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
    "mbps_5208_33",
    {
      {
        FE600_SRD_DATA_RATE_5208_33,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_DATA_RATE.FE600_SRD_DATA_RATE_5208_33:\r\n"
        "  SerDes Data Rate: 5208.33 Mbps\r\n"
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
    "mbps_5333_33",
    {
      {
        FE600_SRD_DATA_RATE_5333_33,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_DATA_RATE.FE600_SRD_DATA_RATE_5333_33:\r\n"
        "  SerDes Data Rate: 5333.33 Mbps\r\n"
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
    "mbps_5833_33",
    {
      {
        FE600_SRD_DATA_RATE_5833_33,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_DATA_RATE.FE600_SRD_DATA_RATE_5833_33:\r\n"
        "  SerDes Data Rate: 5833.33 Mbps\r\n"
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
    "mbps_6000_00",
    {
      {
        FE600_SRD_DATA_RATE_6000_00,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_DATA_RATE.FE600_SRD_DATA_RATE_6000_00:\r\n"
        "  SerDes Data Rate: 6000.00 Mbps\r\n"
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
    "mbps_6250_00",
    {
      {
        FE600_SRD_DATA_RATE_6250_00,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_DATA_RATE.FE600_SRD_DATA_RATE_6250_00:\r\n"
        "  SerDes Data Rate: 6250.00 Mbps\r\n"
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
    "mbps_5468_75",
    {
      {
        FE600_SRD_DATA_RATE_5468_75,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_DATA_RATE.FE600_SRD_DATA_RATE_5468_75:\r\n"
        "  SerDes Data Rate: 5468.75 Mbps\r\n"
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
     FE600_SRD_MEDIA_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "chip2chip",
    {
      {
        FE600_SRD_MEDIA_TYPE_CHIP2CHIP,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_MEDIA_TYPE.FE600_SRD_MEDIA_TYPE_CHIP2CHIP:\r\n"
        "  The 2 communicating chips lay on the same board, therefore very minor Loss is\r\n"
        "  expected.\r\n"
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
    "short_backplane",
    {
      {
        FE600_SRD_MEDIA_TYPE_SHORT_BACKPLANE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_MEDIA_TYPE.FE600_SRD_MEDIA_TYPE_SHORT_BACKPLANE:\r\n"
        "  The 2 communicating chips lay on a short back-plane or connected through a\r\n"
        "  connector.\r\n"
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
    "long_backplane",
    {
      {
        FE600_SRD_MEDIA_TYPE_LONG_BACKPLANE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_MEDIA_TYPE.FE600_SRD_MEDIA_TYPE_LONG_BACKPLANE:\r\n"
        "  The 2 communicating chips lay on a long back-plane, this derives a relatively\r\n"
        "  high Loss.\r\n"
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
     FE600_SRD_TX_PHYS_CONF_MODE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "internal",
    {
      {
        FE600_SRD_TX_PHYS_CONF_MODE_INTERNAL,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_TX_PHYS_CONF_MODE.FE600_SRD_TX_PHYS_CONF_MODE_INTERNAL:\r\n"
        "  Transmitter physical parameters are set according to an internal\r\n"
        "  representation.\r\n"
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
    "explicit",
    {
      {
        FE600_SRD_TX_PHYS_CONF_MODE_EXPLICIT,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_TX_PHYS_CONF_MODE.FE600_SRD_TX_PHYS_CONF_MODE_EXPLICIT:\r\n"
        "  Transmitter physical parameters are set using VODD/VODP values. When reading\r\n"
        "  physical parameters (using the _get API with this configuration mode),\r\n"
        "  PRE/POST-Emphasis values are also returned, in percents. \r\n"
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
    "atten",
    {
      {
        FE600_SRD_TX_PHYS_CONF_MODE_ATTEN,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_TX_PHYS_CONF_MODE.FE600_SRD_TX_PHYS_CONF_MODE_ATTEN:\r\n"
        "  Transmitter physical parameters are calculated and set, based on the lane\r\n"
        "  attenuation. \r\n"
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
    "media_type",
    {
      {
        FE600_SRD_TX_PHYS_CONF_MODE_MEDIA_TYPE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_TX_PHYS_CONF_MODE.FE600_SRD_TX_PHYS_CONF_MODE_MEDIA_TYPE:\r\n"
        "  Transmitter physical parameters are calculated and set, based on the Media\r\n"
        "  Type.\r\n"
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
     FE600_SRD_POWER_STATE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "down",
    {
      {
        FE600_SRD_POWER_STATE_DOWN,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_POWER_STATE.FE600_SRD_POWER_STATE_DOWN:\r\n"
        "  Power state: down.\r\n"
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
    "up",
    {
      {
        FE600_SRD_POWER_STATE_UP,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_POWER_STATE.FE600_SRD_POWER_STATE_UP:\r\n"
        "  Power state: up (normal operation).\r\n"
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
    "up_and_relock",
    {
      {
        FE600_SRD_POWER_STATE_UP_AND_RELOCK,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_POWER_STATE.FE600_SRD_POWER_STATE_UP_AND_RELOCK:\r\n"
        "  Power state: up. Validate SerDes, relock if needed\r\n"
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
     FE600_SRD_AEQ_MODE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "blind",
    {
      {
        FE600_SRD_AEQ_MODE_BLIND,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_AEQ_MODE.FE600_SRD_AEQ_MODE_BLIND:\r\n"
        "  Auto-equalization mode: blind. \r\n"
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
    "steady_state",
    {
      {
        FE600_SRD_AEQ_MODE_STEADY_STATE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_AEQ_MODE.FE600_SRD_AEQ_MODE_STEADY_STATE:\r\n"
        "  Auto-equalization mode: steady-state.\r\n"
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
     FE600_SRD_ENTITY_TYPE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "lane",
    {
      {
        FE600_SRD_ENTITY_TYPE_LANE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_ENTITY_TYPE.FE600_SRD_ENTITY_TYPE_LANE:\r\n"
        "  SerDes Lane. Range : 0 - 95.\r\n"
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
    "cmu",
    {
      {
        FE600_SRD_ENTITY_TYPE_CMU,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_ENTITY_TYPE.FE600_SRD_ENTITY_TYPE_CMU:\r\n"
        "  Per-SerDes-quartet control unit. Note : SerDes Quartet, also referred to as\r\n"
        "  SerDes Macrocell, contains four SerDes Lanes. \r\n"
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
    "ipu",
    {
      {
        FE600_SRD_ENTITY_TYPE_IPU,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_ENTITY_TYPE.FE600_SRD_ENTITY_TYPE_IPU:\r\n"
        "  Per-SerDes-star control unit (8051). Note:\r\n"
        "  SerDes Star, also referred to as SerDes STAR, is a unit containing 3 SerDes\r\n"
        "  Quartets. \r\n"
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
     FE600_SRD_LANE_ELEMENT_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "rx",
    {
      {
        FE600_SRD_LANE_ELEMENT_RX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_LANE_ELEMENT.FE600_SRD_LANE_ELEMENT_RX:\r\n"
        "  RXLow-speed control\r\n"
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
    "tx",
    {
      {
        FE600_SRD_LANE_ELEMENT_TX,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_LANE_ELEMENT.FE600_SRD_LANE_ELEMENT_TX:\r\n"
        "  TXLow-speed control.\r\n"
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
    "prbs",
    {
      {
        FE600_SRD_LANE_ELEMENT_PRBS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_LANE_ELEMENT.FE600_SRD_LANE_ELEMENT_PRBS:\r\n"
        "  PRBS generator/checker.\r\n"
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
    "rx_los",
    {
      {
        FE600_SRD_LANE_ELEMENT_RX_LOS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_LANE_ELEMENT.FE600_SRD_LANE_ELEMENT_RX_LOS:\r\n"
        "  RX LOS detection control registers.\r\n"
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
    "vcdl",
    {
      {
        FE600_SRD_LANE_ELEMENT_VCDL,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_LANE_ELEMENT.FE600_SRD_LANE_ELEMENT_VCDL:\r\n"
        "  VCDL TRIM (4-bit data).\r\n"
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
    "rx_fe",
    {
      {
        FE600_SRD_LANE_ELEMENT_RX_FE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_LANE_ELEMENT.FE600_SRD_LANE_ELEMENT_RX_FE:\r\n"
        "  RX front-end trim (4-bit data).\r\n"
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
    "rx_hs",
    {
      {
        FE600_SRD_LANE_ELEMENT_RX_HS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_LANE_ELEMENT.FE600_SRD_LANE_ELEMENT_RX_HS:\r\n"
        "  RX high-speed control.\r\n"
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
    "rx_sm",
    {
      {
        FE600_SRD_LANE_ELEMENT_RX_SM,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_LANE_ELEMENT.FE600_SRD_LANE_ELEMENT_RX_SM:\r\n"
        "  RX state machine.\r\n"
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
    "preamp",
    {
      {
        FE600_SRD_LANE_ELEMENT_PREAMP,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_LANE_ELEMENT.FE600_SRD_LANE_ELEMENT_PREAMP:\r\n"
        "  Preamp control.\r\n"
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
    "tx_hs",
    {
      {
        FE600_SRD_LANE_ELEMENT_TX_HS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_LANE_ELEMENT.FE600_SRD_LANE_ELEMENT_TX_HS:\r\n"
        "  TX high-speed control.\r\n"
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
     FE600_SRD_CMU_ELEMENT_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "control",
    {
      {
        FE600_SRD_CMU_ELEMENT_CONTROL,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_CMU_ELEMENT.FE600_SRD_CMU_ELEMENT_CONTROL:\r\n"
        "  CMU Control Register Set\r\n"
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
    "arbitration",
    {
      {
        FE600_SRD_CMU_ELEMENT_ARBITRATION,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_CMU_ELEMENT.FE600_SRD_CMU_ELEMENT_ARBITRATION:\r\n"
        "  CMU Interface Arbitration Register Set.\r\n"
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
     FE600_SRD_IPU_ELEMENT_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "control",
    {
      {
        FE600_SRD_IPU_ELEMENT_CONTROL,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_IPU_ELEMENT.FE600_SRD_IPU_ELEMENT_CONTROL:\r\n"
        "  IPU Control Register Set\r\n"
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
    "arbitration",
    {
      {
        FE600_SRD_IPU_ELEMENT_ARBITRATION,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_IPU_ELEMENT.FE600_SRD_IPU_ELEMENT_ARBITRATION:\r\n"
        "  IPU Interface Arbitration Register Set.\r\n"
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
     FE600_SRD_LANE_EQ_MODE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "off",
    {
      {
        FE600_SRD_LANE_EQ_MODE_OFF,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_LANE_EQ_MODE.FE600_SRD_LANE_EQ_MODE_OFF:\r\n"
        "  Adaptive equalizer is off.\r\n"
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
    "blind",
    {
      {
        FE600_SRD_LANE_EQ_MODE_BLIND,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_LANE_EQ_MODE.FE600_SRD_LANE_EQ_MODE_BLIND:\r\n"
        "  Run the algorithm with no previously known parameters about the signal.Blind\r\n"
        "  mode will optimize all preamplifier and DFEsettings. Blind mode is often used\r\n"
        "  to determine the optimum settings, which are then used as preset values.\r\n"
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
    "preset",
    {
      {
        FE600_SRD_LANE_EQ_MODE_PRESET,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_LANE_EQ_MODE.FE600_SRD_LANE_EQ_MODE_PRESET:\r\n"
        "  Preset EQ mode is for setting and aiding the receiver to operate at its\r\n"
        "  near-optimal AEQ settings.\r\n"
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
    "ss",
    {
      {
        FE600_SRD_LANE_EQ_MODE_SS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_LANE_EQ_MODE.FE600_SRD_LANE_EQ_MODE_SS:\r\n"
        "  Steady-State mode. The steadystate EQ mode is used for further optimizing\r\n"
        "  preset EQ parameters (fine adjustment) and for adjusting AEQparameters in\r\n"
        "  response to long-term communication channel characteristics variation.\r\n"
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
     FE600_SRD_LANE_LOOPBACK_MODE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "none",
    {
      {
        FE600_SRD_LANE_LOOPBACK_NONE,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_LANE_LOOPBACK_MODE.FE600_SRD_LANE_LOOPBACK_NONE:\r\n"
        "  Normal (No loopback).\r\n"
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
    "nsilb",
    {
      {
        FE600_SRD_LANE_LOOPBACK_NSILB,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_LANE_LOOPBACK_MODE.FE600_SRD_LANE_LOOPBACK_NSILB:\r\n"
        "  Serial Tx->Rx Loopback, without Driver and PrAmp.\r\n"
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
    "npilb",
    {
      {
        FE600_SRD_LANE_LOOPBACK_NPILB,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_LANE_LOOPBACK_MODE.FE600_SRD_LANE_LOOPBACK_NPILB:\r\n"
        "  Parallel Remote Loopback.\r\n"
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
    "ipilb",
    {
      {
        FE600_SRD_LANE_LOOPBACK_IPILB,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_LANE_LOOPBACK_MODE.FE600_SRD_LANE_LOOPBACK_IPILB:\r\n"
        "  Parallel Internal Loopback.\r\n"
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
     FE600_SRD_PRBS_MODE_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "poly_7",
    {
      {
        FE600_SRD_PRBS_MODE_POLY_7,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_PRBS_MODE.FE600_SRD_PRBS_MODE_POLY_7:\r\n"
        "  PRBS pattern mode: X^7 polynomial.\r\n"
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
    "poly_15",
    {
      {
        FE600_SRD_PRBS_MODE_POLY_15,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_PRBS_MODE.FE600_SRD_PRBS_MODE_POLY_15:\r\n"
        "  PRBS pattern mode: X^15 polynomial.\r\n"
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
    "poly_23",
    {
      {
        FE600_SRD_PRBS_MODE_POLY_23,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_PRBS_MODE.FE600_SRD_PRBS_MODE_POLY_23:\r\n"
        "  PRBS pattern mode: X^23 polynomial.\r\n"
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
    "poly_31",
    {
      {
        FE600_SRD_PRBS_MODE_POLY_31,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_PRBS_MODE.FE600_SRD_PRBS_MODE_POLY_31:\r\n"
        "  PRBS pattern mode: X^31 polynomial.\r\n"
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
    "poly_23_swap_polarity",
    {
      {
        FE600_SRD_PRBS_MODE_POLY_23_SWAP_POLARITY,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_PRBS_MODE.FE600_SRD_PRBS_MODE_POLY_23_SWAP_POLARITY:\r\n"
        "  PRBS pattern mode: X^23 polynomial, with swapped polarity. \r\n"
        "  This mode is needed when working with older devices: SOC_SAND_FAP21V; SOC_SAND_FAP20V; SOC_SAND_FE600.\r\n"
        "  When used, the SerDes lane polarity is swapped before running PRBS,\r\n"
        "  and restored afterwards.\r\n"
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
     FE600_SRD_PRBS_SIGNAL_STAT_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "locked_no_loss",
    {
      {
        FE600_SRD_PRBS_SIGNAL_STAT_LOCKED_NO_LOSS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_PRBS_SIGNAL_STAT.FE600_SRD_PRBS_SIGNAL_STAT_LOCKED_NO_LOSS:\r\n"
        "  PRBS signal status, receiver - locked. No signal loss wath detected. \r\n"
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
    "locked_after_loss",
    {
      {
        FE600_SRD_PRBS_SIGNAL_STAT_LOCKED_AFTER_LOSS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_PRBS_SIGNAL_STAT.FE600_SRD_PRBS_SIGNAL_STAT_LOCKED_AFTER_LOSS:\r\n"
        "  PRBS signal status, receiver - currently locked, but signal loss was detected.\r\n"
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
    "not_locked",
    {
      {
        FE600_SRD_PRBS_SIGNAL_STAT_NOT_LOCKED,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_PRBS_SIGNAL_STAT.FE600_SRD_PRBS_SIGNAL_STAT_NOT_LOCKED:\r\n"
        "  PRBS signal status, receiver - not locked.\r\n"
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
     FE600_SRD_RATE_DIVISOR_rule[]
#ifdef INIT
   =
{
  {
    VAL_SYMBOL,
    "full",
    {
      {
        FE600_SRD_RATE_DIVISOR_FULL,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_RATE_DIVISOR.FE600_SRD_RATE_DIVISOR_FULL:\r\n"
        "  Full Data-rate - as it is.\r\n"
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
    "half",
    {
      {
        FE600_SRD_RATE_DIVISOR_HALF,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_RATE_DIVISOR.FE600_SRD_RATE_DIVISOR_HALF:\r\n"
        "  Half Data-rate - divide full data rate by 2.\r\n"
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
    "quarter",
    {
      {
        FE600_SRD_RATE_DIVISOR_QUARTER,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_RATE_DIVISOR.FE600_SRD_RATE_DIVISOR_QUARTER:\r\n"
        "  Quarter Data-rate - divide full data rate by 4.\r\n"
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
    "nof_rate_divisors",
    {
      {
        FE600_SRD_NOF_RATE_DIVISORS,
          /*
           * Casting added here just to keep the stupid compiler silent.
           */
        (long)""
        "  FE600_SRD_RATE_DIVISOR.FE600_SRD_NOF_RATE_DIVISORS:\r\n"
        "  Total number of SerDes rate divisors.\r\n"
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
/********************************************************/
EXTERN CONST
   PARAM_VAL_RULES
     fe600_api_free_vals[]
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
     fe600_api_serdes_vals[]
#ifdef INIT
   =
{
  {
    VAL_NUMERIC,SYMB_NAME_NUMERIC,
    {
      {
        HAS_MIN_VALUE | HAS_MAX_VALUE,
        FE600_NOF_LINKS-1, 0,
        /*
           * Number of times this value can be repeated (i.e.,
           * value is an array). Must be at least 1!
           */
        FE600_SRD_NOF_LANES,
        (VAL_PROC_PTR)NULL
      }
    }
  },
  {
    VAL_SYMBOL,
      "all",
    {
      {
        FE600_NOF_LINKS,
          /*
          * Casting added here just to keep the stupid compiler silent.
          */
          (long)""
          "  FE600_NOF_LINKS (96):\r\n"
          "  Acting on all lanes\r\n"
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

EXTERN CONST
   PARAM_VAL_RULES
     fe600_api_empty_vals[]
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
     Fe600_api_params[]
#ifdef INIT
   =
{
#ifdef UI_FE600_DEFAULT_SECTION/* { default_section*/
#endif /* } default_section*/
#ifdef UI_FE600_GENERAL/* { general*/
  {
    PARAM_FE600_READ_ID,
    "read",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Read Addr\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api general read 0x45\r\n"
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
    PARAM_FE600_WRITE_ID,
    "write",
    (PARAM_VAL_RULES *)&Fe600_addr_and_data[0],
    (sizeof(Fe600_addr_and_data) / sizeof(Fe600_addr_and_data[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Write to Addr\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api general write 0x45 0x2\r\n"
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
    PARAM_FE600_IWRITE_ID,
    "iwrite",
    (PARAM_VAL_RULES *)&Fe600_addr_and_data[0],
    (sizeof(Fe600_addr_and_data) / sizeof(Fe600_addr_and_data[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Write Data (MSB) to indirect Addr\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api general iwrite 0x10045 0x4 0x4 0x2\r\n"
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
    PARAM_FE600_IREAD_ID,
    "iread",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Read data (MSB) from indirect addr\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api general iread 0x10045\r\n"
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
    PARAM_FE600_SIZE_ID,
    "size",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  table size 1-3\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api general iread 0x10045 size 1\r\n"
    "",
    "",
    {0, 0, 0, BIT(3) | BIT(2)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_DO_TWICE_ID,
    "do_twice",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Perform twice and wait in milliseconds\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api general iread 0x10045 size 1 do_twice 1000\r\n"
    "",
    "",
    {0, 0, 0, BIT(3) | BIT(2) | BIT(1) | BIT(0)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_API_GENERAL_ID,
    "general",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, 0, BIT(0) | BIT(1) | BIT(2) | BIT(3)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } general*/
#ifdef UI_FE600_MGMT/* { mgmt*/
  {
    PARAM_FE600_REGISTER_DEVICE_REGISTER_DEVICE_ID,
    "register_device",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This procedure registers a new device to be taken\r\n"
    "  care of by the CPU.\r\n"
    "  Physical device must be accessible by CPU when this call is made.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt register_device base_address 0 reset_device_ptr 0\r\n"
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
    PARAM_FE600_REGISTER_DEVICE_REGISTER_DEVICE_BASE_ADDRESS_ID,
    "base_address",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  base_address:\r\n"
    "  Application device handle.\r\n"
    "  Usually the base address of direct access memory assigned\r\n"
    "  for device's registers.\r\n"
    "  This parameter needs to be specified even if\r\n"
    "  physical access to device is not by direct\r\n"
    "  access memory since all logic, within driver,\r\n"
    "  up to actual physical access, assumes 'virtual'\r\n"
    "  direct access memory.\r\n"
    "  Memory block assigned by this pointer must not\r\n"
    "  overlap other memory blocks in user's system\r\n"
    "  and certainly not memory blocks assigned to other\r\n"
    "  SOC_SAND_FE600 devices using this procedure.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt register_device base_address 0 reset_device_ptr 0\r\n"
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
    PARAM_FE600_REGISTER_DEVICE_REGISTER_DEVICE_RESET_DEVICE_PTR_ID,
    "reset_device_ptr",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  reset_device_ptr:\r\n"
    "  BSP-function for device reset.\r\n"
    "  Refer to 'SOC_SAND_RESET_DEVICE_FUNC_PTR' definition.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt register_device base_address 0 reset_device_ptr 0\r\n"
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
    PARAM_FE600_UNREGISTER_DEVICE_UNREGISTER_DEVICE_ID,
    "unregister_device",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Undo fe600_register_device()\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt unregister_device\r\n"
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
    PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_ID,
    "operation_mode_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set supported operation mode.\r\n"
    "  The operation mode setting updates the device with the above\r\n"
    "  information, and also updates the driver with the above fundamental\r\n"
    "  settings. The driver later interprets other API calls according to\r\n"
    "  the above information, to enable uniform API regardless the\r\n"
    "  operation mode.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt operation_mode_set enable_16k_multicast_groups 0\r\n"
    "  is_64_links_mode 0 is_TDM_only 0 direct_multicast_mode 0 load_balance_mthd 0\r\n"
    "  support_inband_configuration 0 is_variable_cell_size 0 fabric_mode 0\r\n"
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
    PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OPERATION_MODE_ENABLE_16K_MULTICAST_GROUPS_ID,
    "enable_16k_multicast_groups",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  operation_mode.enable_16k_multicast_groups:\r\n"
    "  If set, the SOC_SAND_FE600 device supports 16K multicast groups.Otherwise, 8K\r\n"
    "  multicast groups.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt operation_mode_set enable_16k_multicast_groups 0\r\n"
    "  is_64_links_mode 0 is_TDM_only 0 direct_multicast_mode 0 load_balance_mthd 0\r\n"
    "  support_inband_configuration 0 is_variable_cell_size 0 fabric_mode 0\r\n"
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
    PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OPERATION_MODE_IS_64_LINKS_MODE_ID,
    "is_64_links_mode",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  operation_mode.is_64_links_mode:\r\n"
    "  When the device works with 64 links, the device activates an internal\r\n"
    "  optimization.In this case, links 16-23; 40-47; 64-71 and 88-95 are not\r\n"
    "  functional\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt operation_mode_set enable_16k_multicast_groups 0\r\n"
    "  is_64_links_mode 0 is_TDM_only 0 direct_multicast_mode 0 load_balance_mthd 0\r\n"
    "  support_inband_configuration 0 is_variable_cell_size 0 fabric_mode 0\r\n"
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
    PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OPERATION_MODE_IS_TDM_ONLY_ID,
    "is_TDM_only",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  operation_mode.is_TDM_only:\r\n"
    "  When the device is TDM only, the links are utilized in a better way.If not,\r\n"
    "  one can still set the TDM-only per bit\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt operation_mode_set enable_16k_multicast_groups 0\r\n"
    "  is_64_links_mode 0 is_TDM_only 0 direct_multicast_mode 0 load_balance_mthd 0\r\n"
    "  support_inband_configuration 0 is_variable_cell_size 0 fabric_mode 0\r\n"
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
    PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OPERATION_MODE_DIRECT_MULTICAST_MODE_ID,
    "direct_multicast_mode",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  operation_mode.direct_multicast_mode:\r\n"
    "  When TRUE, the multicast groups are represented by bitmap of faps.Otherwise,\r\n"
    "  the multicast groups are represented by bitmap of links.Direct multicast is\r\n"
    "  valid in single-stage systems with highest FAP-ID lower than 96.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt operation_mode_set enable_16k_multicast_groups 0\r\n"
    "  is_64_links_mode 0 is_TDM_only 0 direct_multicast_mode 0 load_balance_mthd 0\r\n"
    "  support_inband_configuration 0 is_variable_cell_size 0 fabric_mode 0\r\n"
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
    PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OPERATION_MODE_LOAD_BALANCE_MTHD_ID,
    "load_balance_mthd",
    (PARAM_VAL_RULES *)&FE600_LOAD_BALANCE_METHOD_rule[0],
    (sizeof(FE600_LOAD_BALANCE_METHOD_rule) / sizeof(FE600_LOAD_BALANCE_METHOD_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  operation_mode.load_balance_mthd:\r\n"
    "  Load-Balance Method\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt operation_mode_set enable_16k_multicast_groups 0\r\n"
    "  is_64_links_mode 0 is_TDM_only 0 direct_multicast_mode 0 load_balance_mthd 0\r\n"
    "  support_inband_configuration 0 is_variable_cell_size 0 fabric_mode 0\r\n"
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
    PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OPERATION_MODE_SUPPORT_INBAND_CONFIGURATION_ID,
    "support_inband_configuration",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  operation_mode.support_inband_configuration:\r\n"
    "  If TRUE, the SOC_SAND_FE600 supports configuration by remote source-routed cell\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt operation_mode_set enable_16k_multicast_groups 0\r\n"
    "  is_64_links_mode 0 is_TDM_only 0 direct_multicast_mode 0 load_balance_mthd 0\r\n"
    "  support_inband_configuration 0 is_variable_cell_size 0 fabric_mode 0\r\n"
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
    PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OPERATION_MODE_IS_VARIABLE_CELL_SIZE_ID,
    "is_variable_cell_size",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  operation_mode.is_variable_cell_size:\r\n"
    "  Fixed / variable cell size.TRUE for variable cell size.The SOC_SAND_FE600 supports the\r\n"
    "  mode of Fix Sized Cells (FSC) in order to interoperate with the existing SOC_SAND\r\n"
    "  devices (FAP10/20V, FAP11/21V, SOC_SAND_FAP10M, SOC_SAND_FAP20M and SOC_SAND_FE200).\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt operation_mode_set enable_16k_multicast_groups 0\r\n"
    "  is_64_links_mode 0 is_TDM_only 0 direct_multicast_mode 0 load_balance_mthd 0\r\n"
    "  support_inband_configuration 0 is_variable_cell_size 0 fabric_mode 0\r\n"
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
    PARAM_FE600_MGMT_OPERATION_MODE_SET_OPERATION_MODE_SET_OPERATION_MODE_FABRIC_MODE_ID,
    "fabric_mode",
    (PARAM_VAL_RULES *)&FE600_FABRIC_MODE_rule[0],
    (sizeof(FE600_FABRIC_MODE_rule) / sizeof(FE600_FABRIC_MODE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  operation_mode.fabric_mode:\r\n"
    "  Single stage / Multi stage FE2 / Multi stage FE13 / Repeater\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt operation_mode_set enable_16k_multicast_groups 0\r\n"
    "  is_64_links_mode 0 is_TDM_only 0 direct_multicast_mode 0 load_balance_mthd 0\r\n"
    "  support_inband_configuration 0 is_variable_cell_size 0 fabric_mode 0\r\n"
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
    PARAM_FE600_MGMT_OPERATION_MODE_GET_OPERATION_MODE_GET_ID,
    "operation_mode_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set supported operation mode.\r\n"
    "  The operation mode setting updates the device with the above\r\n"
    "  information, and also updates the driver with the above fundamental\r\n"
    "  settings. The driver later interprets other API calls according to\r\n"
    "  the above information, to enable uniform API regardless the\r\n"
    "  operation mode.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt operation_mode_get\r\n"
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
    PARAM_FE600_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_ID,
    "init_sequence_phase1",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Initialize the device, including:1.  Prevent all the control\r\n"
    "  cells2.  Initialize the device memories.3.  Update the device memories\r\n"
    "  initialization according to the hw_ajust\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt init_sequence_phase1 lane_conf_index 0 is_swap_polarity 0\r\n"
    "  power_state_conf 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  conf_mode 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0 enable 0 silent 0\r\n"
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
    PARAM_FE600_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_AJUST_SERDES_INFO_CONF_LANE_CONF_INDEX_ID,
    "lane_conf_index",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_conf_index:\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt init_sequence_phase1 lane_conf_index 0 is_swap_polarity 0\r\n"
    "  power_state_conf 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  conf_mode 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0 enable 0 silent 0\r\n"
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
    PARAM_FE600_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_AJUST_SERDES_INFO_CONF_LANE_CONF_IS_SWAP_POLARITY_ID,
    "is_swap_polarity",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  hw_ajust.serdes_info.conf.lane_conf[ prm_lane_conf_index].is_swap_polarity:\r\n"
    "  If TRUE, polarity is swapped.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt init_sequence_phase1 lane_conf_index 0 is_swap_polarity 0\r\n"
    "  power_state_conf 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  conf_mode 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0 enable 0 silent 0\r\n"
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
    PARAM_FE600_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_AJUST_SERDES_INFO_CONF_LANE_CONF_POWER_STATE_CONF_ID,
    "power_state_conf",
    (PARAM_VAL_RULES *)&FE600_SRD_POWER_STATE_rule[0],
    (sizeof(FE600_SRD_POWER_STATE_rule) / sizeof(FE600_SRD_POWER_STATE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  hw_ajust.serdes_info.conf.lane_conf[ prm_lane_conf_index].power_state_conf:\r\n"
    "  Per-SerDes Lane power state configuration parameters. Note: if enabled, the\r\n"
    "  configuration is set for both direction (receive and transmit). To set\r\n"
    "  different configuration per-direction - either set FALSE here and configure\r\n"
    "  using dedicated API, or override one of the directions using dedicated API. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt init_sequence_phase1 lane_conf_index 0 is_swap_polarity 0\r\n"
    "  power_state_conf 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  conf_mode 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0 enable 0 silent 0\r\n"
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
    PARAM_FE600_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_AJUST_SERDES_INFO_CONF_LANE_CONF_TX_PHYS_CONF_MEDIA_TYPE_ID,
    "media_type",
    (PARAM_VAL_RULES *)&FE600_SRD_MEDIA_TYPE_rule[0],
    (sizeof(FE600_SRD_MEDIA_TYPE_rule) / sizeof(FE600_SRD_MEDIA_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  hw_ajust.serdes_info.conf.lane_conf[\r\n"
    "  prm_lane_conf_index].tx_phys_conf.media_type:\r\n"
    "  SerDes connecting media type. TX physical parameters are derived from the\r\n"
    "  selected media type. Used only if the conf_mode is media_type - ignored\r\n"
    "  otherwise. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt init_sequence_phase1 lane_conf_index 0 is_swap_polarity 0\r\n"
    "  power_state_conf 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  conf_mode 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0 enable 0 silent 0\r\n"
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
    PARAM_FE600_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_AJUST_SERDES_INFO_CONF_LANE_CONF_TX_PHYS_CONF_EXPLCT_POST_EMPHASIS_ID,
    "post_emphasis",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  hw_ajust.serdes_info.conf.lane_conf[\r\n"
    "  prm_lane_conf_index].tx_phys_conf.explct.post_emphasis:\r\n"
    "  Transmitter post-emphasis. Units: 1/10[dB]Range: 0 - 120. Example: 120 means\r\n"
    "  12.0 db, 55 means 5.5 dB etc.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt init_sequence_phase1 lane_conf_index 0 is_swap_polarity 0\r\n"
    "  power_state_conf 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  conf_mode 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0 enable 0 silent 0\r\n"
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
    PARAM_FE600_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_AJUST_SERDES_INFO_CONF_LANE_CONF_TX_PHYS_CONF_EXPLCT_PRE_EMPHASIS_ID,
    "pre_emphasis",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  hw_ajust.serdes_info.conf.lane_conf[\r\n"
    "  prm_lane_conf_index].tx_phys_conf.explct.pre_emphasis:\r\n"
    "  Transmitter pre-emphasis. Units: 1/10[dB]Range: 0 - 30. Example: 30 means 3.0\r\n"
    "  db, 15 means 1.5 dB etc.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt init_sequence_phase1 lane_conf_index 0 is_swap_polarity 0\r\n"
    "  power_state_conf 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  conf_mode 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0 enable 0 silent 0\r\n"
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
    PARAM_FE600_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_AJUST_SERDES_INFO_CONF_LANE_CONF_TX_PHYS_CONF_EXPLCT_AMPLITUDE_ID,
    "amplitude",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  hw_ajust.serdes_info.conf.lane_conf[\r\n"
    "  prm_lane_conf_index].tx_phys_conf.explct.amplitude:\r\n"
    "  The amplitude of the transmitter in mV. Range: 200-1750mV.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt init_sequence_phase1 lane_conf_index 0 is_swap_polarity 0\r\n"
    "  power_state_conf 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  conf_mode 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0 enable 0 silent 0\r\n"
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
    PARAM_FE600_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_AJUST_SERDES_INFO_CONF_LANE_CONF_TX_PHYS_CONF_CONF_MODE_ID,
    "conf_mode",
    (PARAM_VAL_RULES *)&FE600_SRD_TX_PHYS_CONF_MODE_rule[0],
    (sizeof(FE600_SRD_TX_PHYS_CONF_MODE_rule) / sizeof(FE600_SRD_TX_PHYS_CONF_MODE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  hw_ajust.serdes_info.conf.lane_conf[\r\n"
    "  prm_lane_conf_index].tx_phys_conf.conf_mode:\r\n"
    "  Transmitter physical parameters configuration mode - by media-type, or\r\n"
    "  explicitly.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt init_sequence_phase1 lane_conf_index 0 is_swap_polarity 0\r\n"
    "  power_state_conf 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  conf_mode 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0 enable 0 silent 0\r\n"
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
    PARAM_FE600_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_AJUST_SERDES_INFO_CONF_LANE_CONF_RATE_CONF_ID,
    "rate_conf",
    (PARAM_VAL_RULES *)&FE600_SRD_DATA_RATE_rule[0],
    (sizeof(FE600_SRD_DATA_RATE_rule) / sizeof(FE600_SRD_DATA_RATE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  hw_ajust.serdes_info.conf.lane_conf[ prm_lane_conf_index].rate_conf:\r\n"
    "  Per-SerDes Lane rate configuration parameters. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt init_sequence_phase1 lane_conf_index 0 is_swap_polarity 0\r\n"
    "  power_state_conf 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  conf_mode 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0 enable 0 silent 0\r\n"
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
    PARAM_FE600_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_AJUST_SERDES_INFO_CONF_LANE_CONF_ENABLE_ID,
    "enable",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  hw_ajust.serdes_info.conf.lane_conf[ prm_lane_conf_index].enable:\r\n"
    "  If TRUE, the lane configuration is applied. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt init_sequence_phase1 lane_conf_index 0 is_swap_polarity 0\r\n"
    "  power_state_conf 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  conf_mode 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0 enable 0 silent 0\r\n"
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
    PARAM_FE600_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_AJUST_SERDES_INFO_CONF_STAR_CONF_INDEX_ID,
    "star_conf_index",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  star_conf_index:\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt init_sequence_phase1 lane_conf_index 0 is_swap_polarity 0\r\n"
    "  power_state_conf 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  conf_mode 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0 enable 0 silent 0\r\n"
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
    PARAM_FE600_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_AJUST_SERDES_INFO_CONF_STAR_CONF_CONF_QRTT_INDEX_ID,
    "qrtt_index",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  qrtt_index:\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt init_sequence_phase1 lane_conf_index 0 is_swap_polarity 0\r\n"
    "  power_state_conf 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  conf_mode 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0 enable 0 silent 0\r\n"
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
    PARAM_FE600_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_AJUST_SERDES_INFO_CONF_STAR_CONF_CONF_QRTT_MAX_EXPECTED_LANE_RATE_ID,
    "max_expected_lane_rate",
    (PARAM_VAL_RULES *)&FE600_SRD_DATA_RATE_rule[0],
    (sizeof(FE600_SRD_DATA_RATE_rule) / sizeof(FE600_SRD_DATA_RATE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  hw_ajust.serdes_info.conf.star_conf[ prm_star_conf_index].conf.qrtt[\r\n"
    "  prm_qrtt_index].max_expected_lane_rate:\r\n"
    "  The maximal expected rate for any lane in the quartet. Range: 312.5 - 6250\r\n"
    "  Mbps. Must be identical for all lanes per-quartet.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt init_sequence_phase1 lane_conf_index 0 is_swap_polarity 0\r\n"
    "  power_state_conf 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  conf_mode 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0 enable 0 silent 0\r\n"
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
    PARAM_FE600_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_AJUST_SERDES_INFO_CONF_STAR_CONF_CONF_QRTT_IS_ACTIVE_ID,
    "is_active",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  hw_ajust.serdes_info.conf.star_conf[ prm_star_conf_index].conf.qrtt[\r\n"
    "  prm_qrtt_index].is_active:\r\n"
    "  If TRUE, the specified quartet CMU in the SerDes star is activated. Notes: 1.\r\n"
    "  If any CMU is expected to be used at some stage in the future, it must be\r\n"
    "  activated on init (the ACTIVE CMU structure). 2. Not activating a CMU (if not\r\n"
    "  expected to be used at any point) improves the device power consumption.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt init_sequence_phase1 lane_conf_index 0 is_swap_polarity 0\r\n"
    "  power_state_conf 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  conf_mode 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0 enable 0 silent 0\r\n"
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
    PARAM_FE600_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_AJUST_SERDES_INFO_CONF_STAR_CONF_ENABLE_ID,
    "enable",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  hw_ajust.serdes_info.conf.star_conf[ prm_star_conf_index].enable:\r\n"
    "  If TRUE, the STAR configuration is applied (STAR initialization sequence).\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt init_sequence_phase1 lane_conf_index 0 is_swap_polarity 0\r\n"
    "  power_state_conf 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  conf_mode 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0 enable 0 silent 0\r\n"
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
    PARAM_FE600_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_AJUST_SERDES_INFO_ENABLE_ID,
    "enable",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  hw_ajust.serdes_info.enable:\r\n"
    "  Enable/disable SerDes configuration\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt init_sequence_phase1 lane_conf_index 0 is_swap_polarity 0\r\n"
    "  power_state_conf 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  conf_mode 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0 enable 0 silent 0\r\n"
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
    PARAM_FE600_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_SILENT_ID,
    "silent",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  silent:\r\n"
    "  TRUE - Print progress messages.\r\n"
    "  FALSE - Do not print progress messages.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt init_sequence_phase1 lane_conf_index 0 is_swap_polarity 0\r\n"
    "  power_state_conf 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  conf_mode 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0 enable 0 silent 0\r\n"
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
    PARAM_FE600_MGMT_INIT_SEQUENCE_PHASE2_INIT_SEQUENCE_PHASE2_ID,
    "init_sequence_phase2",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Enable the device to receive and transmit control cells.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt init_sequence_phase2\r\n"
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
    PARAM_FE600_MGMT_SYSTEM_FE_ID_SET_SYSTEM_FE_ID_SET_ID,
    "system_fe_id_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the fabric system ID of the device (Unique in the system).\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt system_fe_id_set fabric_id 0\r\n"
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
    PARAM_FE600_MGMT_SYSTEM_FE_ID_SET_SYSTEM_FE_ID_SET_FABRIC_ID_ID,
    "fabric_id",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  fabric_id:\r\n"
    "  the system ID of the device (Unique in the system).\r\n"
    "  Range: 0 - 2047.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt system_fe_id_set fabric_id 0\r\n"
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
    PARAM_FE600_MGMT_SYSTEM_FE_ID_GET_SYSTEM_FE_ID_GET_ID,
    "system_fe_id_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the fabric system ID of the device (Unique in the system).\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt system_fe_id_get\r\n"
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
    PARAM_FE600_MGMT_ALL_CTRL_CELLS_ENABLE_ALL_CTRL_CELLS_ENABLE_ID,
    "all_ctrl_cells_enable",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Enable / Disable the device from receiving and transmitting\r\n"
    "  control cells.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt all_ctrl_cells_enable is_enabled 0\r\n"
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
    PARAM_FE600_MGMT_ALL_CTRL_CELLS_ENABLE_ALL_CTRL_CELLS_ENABLE_IS_ENABLED_ID,
    "is_enabled",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  is_enabled:\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint8\r\n"
    "  is_enabled\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt all_ctrl_cells_enable is_enabled 0\r\n"
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
    PARAM_FE600_MGMT_ENABLE_TRAFFIC_SET_ENABLE_TRAFFIC_SET_ID,
    "enable_traffic_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Enables / Disables the device from receiving and transmitting\r\n"
    "  traffic.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt enable_traffic_set is_enabled 0\r\n"
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
    PARAM_FE600_MGMT_ENABLE_TRAFFIC_SET_ENABLE_TRAFFIC_SET_IS_ENABLED_ID,
    "is_enabled",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  is_enabled:\r\n"
    "  TRUE - enables traffic.\r\n"
    "  FALSE - disables traffic.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt enable_traffic_set is_enabled 0\r\n"
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
    PARAM_FE600_MGMT_ENABLE_TRAFFIC_GET_ENABLE_TRAFFIC_GET_ID,
    "enable_traffic_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Enables / Disables the device from receiving and transmitting\r\n"
    "  traffic.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt enable_traffic_get\r\n"
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
    PARAM_FE600_SSR_ID,
    "ssr_reset_restore_and_test",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  SSR reset and restore test. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api mgmt enable_traffic_get\r\n"
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
    PARAM_FE600_API_MGMT_ID,
    "mgmt",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(8)|BIT(9)|BIT(10)|BIT(11)|BIT(12), 0, 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } mgmt*/
#ifdef UI_FE600_LINKS_TOPOLOGY/* { topology*/
  {
    PARAM_FE600_TOPOLOGY_CONST_CONNECTIVITY_SET_TOPOLOGY_CONST_CONNECTIVITY_SET_ID,
    "const_connectivity_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the constant topology of the system.\r\n"
    "  For each destination FAP ID, defines which links access it.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology const_connectivity_set far_unit_index\r\n"
    "  0 far_unit 0\r\n"
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
    PARAM_FE600_TOPOLOGY_CONST_CONNECTIVITY_SET_TOPOLOGY_CONST_CONNECTIVITY_SET_SYS_TOPOLOGY_FAR_DEVICE_ID_INDEX_ID,
    "far_unit_index",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  far_unit_index:\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology const_connectivity_set far_unit_index\r\n"
    "  0 far_unit 0\r\n"
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
    PARAM_FE600_TOPOLOGY_CONST_CONNECTIVITY_SET_TOPOLOGY_CONST_CONNECTIVITY_SET_SYS_TOPOLOGY_FAR_DEVICE_ID_ID,
    "far_unit",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_topology.far_unit[ prm_far_unit_index]:\r\n"
    "  For each link, the system device ID of the far device (FAP / FE1).\r\n"
    "  FE600_INVALID_FAR_DEVICE Indicates that the link is not connected to a\r\n"
    "  valid far device\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology const_connectivity_set far_unit_index\r\n"
    "  0 far_unit 0\r\n"
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
    PARAM_FE600_TOPOLOGY_CONST_CONNECTIVITY_GET_TOPOLOGY_CONST_CONNECTIVITY_GET_ID,
    "const_connectivity_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the constant topology of the system.\r\n"
    "  For each destination FAP ID, defines which links access it.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology const_connectivity_get\r\n"
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
    PARAM_FE600_TOPOLOGY_CONST_CONNECTIVITY_UPDATE_TOPOLOGY_CONST_CONNECTIVITY_UPDATE_ID,
    "const_connectivity_update",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Update the reachability information of a single FAP.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology const_connectivity_update\r\n"
    "  far_unit_index 0 far_unit 0\r\n"
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
    PARAM_FE600_TOPOLOGY_CONST_CONNECTIVITY_UPDATE_TOPOLOGY_CONST_CONNECTIVITY_UPDATE_SYS_TOPOLOGY_FAR_DEVICE_ID_INDEX_ID,
    "far_unit_index",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  far_unit_index:\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology const_connectivity_update\r\n"
    "  far_unit_index 0 far_unit 0\r\n"
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
    PARAM_FE600_TOPOLOGY_CONST_CONNECTIVITY_UPDATE_TOPOLOGY_CONST_CONNECTIVITY_UPDATE_SYS_TOPOLOGY_FAR_DEVICE_ID_ID,
    "far_unit",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  sys_topology.far_unit[ prm_far_unit_index]:\r\n"
    "  For each link, the system device ID of the far device (FAP / FE1).\r\n"
    "  FE600_INVALID_FAR_DEVICE Indicates that the link is not connected to a\r\n"
    "  valid far device\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology const_connectivity_update\r\n"
    "  far_unit_index 0 far_unit 0\r\n"
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
    PARAM_FE600_TOPOLOGY_SET_CONST_TOPOLOGY_AS_CURRENT_TOPOLOGY_SET_CONST_TOPOLOGY_AS_CURRENT_ID,
    "set_const_as_current",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the const topology, according to the current topology in the\r\n"
    "  device.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology set_const_as_current\r\n"
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
    PARAM_FE600_TOPOLOGY_REPEATER_SET_TOPOLOGY_REPEATER_SET_ID,
    "repeater_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the repeater topology.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology repeater_set destination_links_index 0\r\n"
    "  destination_links 0\r\n"
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
    PARAM_FE600_TOPOLOGY_REPEATER_SET_TOPOLOGY_REPEATER_SET_DESTINATION_LINKS_INDEX_ID,
    "destination_links_index",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  destination_links_index:\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology repeater_set destination_links_index 0\r\n"
    "  destination_links 0\r\n"
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
    PARAM_FE600_TOPOLOGY_REPEATER_SET_TOPOLOGY_REPEATER_SET_DESTINATION_LINKS_ID,
    "destination_links",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  destination_links[ prm_destination_links_index]:\r\n"
    "  Entry I in the array set the destination link for source link I.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology repeater_set destination_links_index 0\r\n"
    "  destination_links 0\r\n"
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
    PARAM_FE600_TOPOLOGY_REPEATER_GET_TOPOLOGY_REPEATER_GET_ID,
    "repeater_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the repeater topology.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology repeater_get\r\n"
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
    PARAM_FE600_TOPOLOGY_GRACEFUL_SHUTDOWN_TOPOLOGY_GRACEFUL_SHUTDOWN_ID,
    "graceful_shutdown",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Isolate the FE device from the system.\r\n"
    "  Once the device is Isolated, the device might be reset or updated\r\n"
    "  without affecting the system \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology graceful_shutdown state 0\r\n"
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
    PARAM_FE600_TOPOLOGY_GRACEFUL_SHUTDOWN_TOPOLOGY_GRACEFUL_SHUTDOWN_STATE_ID,
    "state",
    (PARAM_VAL_RULES *)&FE600_TOPOLOGY_STATE_rule[0],
    (sizeof(FE600_TOPOLOGY_STATE_rule) / sizeof(FE600_TOPOLOGY_STATE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  state:\r\n"
    "  Isolated: The FE is not seen by the other devices.\r\n"
    "  Active:\r\n"
    "  The device is back in the system, and used to send traffic\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology graceful_shutdown state 0\r\n"
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
    PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_GET_TOPOLOGY_STATUS_REACHABILITY_MAP_GET_ID,
    "status_reachability_map_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Retrieve the reachability map from the device. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology status_reachability_map_get highest_fap_id 0\r\n"
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
    PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_GET_TOPOLOGY_STATUS_REACHABILITY_MAP_GET_HIGHEST_FAP_ID_ID,
    "highest_fap_id",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  highest_fap_id:\r\n"
    "  The highest FAP ID in the system.\r\n"
    "  Also used as the number of entries pointed by\r\n"
    "  p_reach_map - 1.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology status_reachability_map_get highest_fap_id 0\r\n"
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
    PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_ID,
    "status_reachability_map_print",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Print the reachability map from the device. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology status_reachability_map_print flavor 0\r\n"
    "  highest_fap_id 0 first_fap_id 0 last_fap_id 0 bm_links_index 0 bm_links 0\r\n"
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
    PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_FLAVOR_ID,
    "flavor",
    (PARAM_VAL_RULES *)&FE600_TOPOLOGY_STAT_REACH_PRINT_FLAV_rule[0],
    (sizeof(FE600_TOPOLOGY_STAT_REACH_PRINT_FLAV_rule) / sizeof(FE600_TOPOLOGY_STAT_REACH_PRINT_FLAV_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flavor:\r\n"
    "  SOC_SAND_IN FE600_TOPOLOGY_STAT_REACH_PRINT_FLAV flavor\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology status_reachability_map_print flavor 0\r\n"
    "  highest_fap_id 0 first_fap_id 0 last_fap_id 0 bm_links_index 0 bm_links 0\r\n"
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
    PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_HIGHEST_FAP_ID_ID,
    "highest_fap_id",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  highest_fap_id:\r\n"
    "  The highest FAP ID in the system.\r\n"
    "  Also used as the number of entries pointed by\r\n"
    "  p_reach_map - 1.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology status_reachability_map_print flavor 0\r\n"
    "  highest_fap_id 0 first_fap_id 0 last_fap_id 0 bm_links_index 0 bm_links 0\r\n"
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
    PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_FIRST_FAP_ID_ID,
    "first_fap_id",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  first_fap_id:\r\n"
    "  The first FAP ID to be printed.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology status_reachability_map_print flavor 0\r\n"
    "  highest_fap_id 0 first_fap_id 0 last_fap_id 0 bm_links_index 0 bm_links 0\r\n"
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
    PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_LAST_FAP_ID_ID,
    "last_fap_id",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  last_fap_id:\r\n"
    "  The last FAP ID to be printed.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology status_reachability_map_print flavor 0\r\n"
    "  highest_fap_id 0 first_fap_id 0 last_fap_id 0 bm_links_index 0 bm_links 0\r\n"
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
    PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_P_REACH_MAP_BM_LINKS_INDEX_ID,
    "bm_links_index",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  bm_links_index:\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology status_reachability_map_print flavor 0\r\n"
    "  highest_fap_id 0 first_fap_id 0 last_fap_id 0 bm_links_index 0 bm_links 0\r\n"
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
    PARAM_FE600_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_TOPOLOGY_STATUS_REACHABILITY_MAP_PRINT_P_REACH_MAP_BM_LINKS_ID,
    "bm_links",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  p_reach_map.bm_links[ prm_bm_links_index]:\r\n"
    "  Bit Map. For Bit X in bm_links[I]:If Asserted: Destination FAP dest_fap_ndx\r\n"
    "  is reachable through link X + I*32.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology status_reachability_map_print flavor 0\r\n"
    "  highest_fap_id 0 first_fap_id 0 last_fap_id 0 bm_links_index 0 bm_links 0\r\n"
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
    PARAM_FE600_TOPOLOGY_STATUS_CMP_WITH_CURR_CONNECTIVITY_GET_TOPOLOGY_STATUS_CMP_WITH_CURR_CONNECTIVITY_GET_ID,
    "status_cmp_with_curr_connectivity_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Compare the declared topology with the actual reachability table\r\n"
    "  from the device. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology status_cmp_with_curr_connectivity_get\r\n"
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
    PARAM_FE600_TOPOLOGY_STATUS_CONNECTIVITY_GET_TOPOLOGY_STATUS_CONNECTIVITY_GET_ID,
    "status_connectivity_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Retrieve the connectivity map from the device. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api topology status_connectivity_get\r\n"
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
    PARAM_FE600_API_LINKS_TOPOLOGY_ID,
    "topology",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(13)|BIT(14)|BIT(15)|BIT(16)|BIT(17)|BIT(18)|BIT(19)|BIT(20)|BIT(21)|BIT(22)|BIT(23), 0, 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } topology*/
#ifdef UI_FE600_LINKS/* { links*/
  {
    PARAM_FE600_FABRIC_SRD_QRTT_RESET_SRD_QRTT_RESET_ID,
    "srd_qrtt_reset",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This procedure handles the SerDes configuration of a SerDes quartet attached\r\n"
    "  to the fabric interface. It is called during fabric initialization sequence.\r\n"
    "  Also, in the case that a SerDes quartet is reset (e.g. in order to change the\r\n"
    "  SerDes rate), this API must be called to first shut down all the relevant\r\n"
    "  links (is_oor == FALSE), and then, after serdes quartet reset, with is_oor ==\r\n"
    "  TRUE. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api fabric srd_qrtt_reset srd_qrtt_ndx 0 is_oor 0\r\n"
    "",
    "",
    {0, 0, BIT(25)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_FABRIC_SRD_QRTT_RESET_SRD_QRTT_RESET_SRD_QRTT_NDX_ID,
    "srd_qrtt_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  srd_qrtt_ndx:\r\n"
    "  The SerDes quartet index. Range: 3 and 7 according to Combo configuration, 8\r\n"
    "  - 14.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api fabric srd_qrtt_reset srd_qrtt_ndx 0 is_oor 0\r\n"
    "",
    "",
    {0, 0, BIT(25)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_FABRIC_SRD_QRTT_RESET_SRD_QRTT_RESET_IS_OOR_ID,
    "is_oor",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  is_oor:\r\n"
    "  Is Out-Of-Reset. If FALSE, means In-Reset. Performs the following:\r\n"
    "  If TRUE, configures the SerDes quartet to fabric-compatable configuration. If\r\n"
    "  FALSE, turns down the appropriate links in the Fabric. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api fabric srd_qrtt_reset srd_qrtt_ndx 0 is_oor 0\r\n"
    "",
    "",
    {0, 0, BIT(25)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_FE600_LINKS_RESET_SET_RESET_SET_ID,
    "link_on_off_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Enables / disables a links\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links reset_set link_ndx 0 is_enabled 0\r\n"
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
    PARAM_FE600_LINKS_RESET_SET_RESET_SET_LINK_NDX_ID,
    "link_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  link_ndx:\r\n"
    "  The Link ID\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links reset_set link_ndx 0 is_enabled 0\r\n"
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
    PARAM_FE600_LINKS_RESET_SET_RESET_SET_IS_ENABLED_ID,
    "serdes_also",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  serdes_also:\r\n"
    "  TRUE -\r\n"
    "  Link is operational.\r\n"
    "  FALSE - Link is in reset.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links reset_set link_ndx 0 is_enabled 0\r\n"
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
    PARAM_FE600_LINK_ON_OFF_SET_LINK_ON_OFF_SET_INFO_ON_OFF_ID,
    "on_off",
    (PARAM_VAL_RULES *)&FE600_LINK_STATE_rule[0],
    (sizeof(FE600_LINK_STATE_rule) / sizeof(FE600_LINK_STATE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.on_off:\r\n"
    "  Fabric link state - normal operation or down.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api fabric link_on_off_set link_ndx 0 serdes_also 0 on_off 0\r\n"
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
    PARAM_FE600_LINKS_RESET_GET_RESET_GET_ID,
    "link_on_off_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Enables / disables a links\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links reset_get link_ndx 0\r\n"
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
    PARAM_FE600_LINKS_RESET_GET_RESET_GET_LINK_NDX_ID,
    "link_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  link_ndx:\r\n"
    "  The Link ID\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links reset_get link_ndx 0\r\n"
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
    PARAM_FE600_LINKS_IS_TDM_ONLY_SET_IS_TDM_ONLY_SET_ID,
    "is_tdm_only_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Enables / Disables acceptance of NON-TDM traffic on the link\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links is_tdm_only_set link_ndx 0 is_tdm_only 0\r\n"
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
    PARAM_FE600_LINKS_IS_TDM_ONLY_SET_IS_TDM_ONLY_SET_LINK_NDX_ID,
    "link_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  link_ndx:\r\n"
    "  The Link ID\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links is_tdm_only_set link_ndx 0 is_tdm_only 0\r\n"
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
    PARAM_FE600_LINKS_IS_TDM_ONLY_SET_IS_TDM_ONLY_SET_IS_TDM_ONLY_ID,
    "is_tdm_only",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  is_tdm_only:\r\n"
    "  TRUE -\r\n"
    "  TDM only.\r\n"
    "  FALSE - TDM and TDM traffic is accepted.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links is_tdm_only_set link_ndx 0 is_tdm_only 0\r\n"
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
    PARAM_FE600_LINKS_IS_TDM_ONLY_GET_IS_TDM_ONLY_GET_ID,
    "is_tdm_only_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Enables / Disables acceptance of NON-TDM traffic on the link\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links is_tdm_only_get link_ndx 0\r\n"
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
    PARAM_FE600_LINKS_IS_TDM_ONLY_GET_IS_TDM_ONLY_GET_LINK_NDX_ID,
    "link_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  link_ndx:\r\n"
    "  The Link ID\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links is_tdm_only_get link_ndx 0\r\n"
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
    PARAM_FE600_LINKS_LINK_LEVEL_FC_ENABLE_SET_LINK_LEVEL_FC_ENABLE_SET_ID,
    "link_level_fc_enable_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Enables / Disables Link Level flow control on the link\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links link_level_fc_enable_set link_ndx 0 is_enabled 0\r\n"
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
    PARAM_FE600_LINKS_LINK_LEVEL_FC_ENABLE_SET_LINK_LEVEL_FC_ENABLE_SET_LINK_NDX_ID,
    "link_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  link_ndx:\r\n"
    "  The Link ID\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links link_level_fc_enable_set link_ndx 0 is_enabled 0\r\n"
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
    PARAM_FE600_LINKS_LINK_LEVEL_FC_ENABLE_SET_LINK_LEVEL_FC_ENABLE_SET_IS_ENABLED_ID,
    "is_enabled",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  is_enabled:\r\n"
    "  TRUE -\r\n"
    "  Enabled\r\n"
    "  FALSE - Disabled\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links link_level_fc_enable_set link_ndx 0 is_enabled 0\r\n"
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
    PARAM_FE600_LINKS_LINK_LEVEL_FC_ENABLE_GET_LINK_LEVEL_FC_ENABLE_GET_ID,
    "link_level_fc_enable_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Enables / Disables Link Level flow control on the link\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links link_level_fc_enable_get link_ndx 0\r\n"
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
    PARAM_FE600_LINKS_LINK_LEVEL_FC_ENABLE_GET_LINK_LEVEL_FC_ENABLE_GET_LINK_NDX_ID,
    "link_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  link_ndx:\r\n"
    "  The Link ID\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links link_level_fc_enable_get link_ndx 0\r\n"
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
    PARAM_FE600_LINKS_FEC_ENABLE_SET_FEC_ENABLE_SET_ID,
    "fec_enable_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Enables / Disables Forward Error Correction\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links fec_enable_set link_ndx 0 is_enabled 0\r\n"
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
    PARAM_FE600_LINKS_FEC_ENABLE_SET_FEC_ENABLE_SET_LINK_NDX_ID,
    "link_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  link_ndx:\r\n"
    "  The Link ID\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links fec_enable_set link_ndx 0 is_enabled 0\r\n"
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
    PARAM_FE600_LINKS_FEC_ENABLE_SET_FEC_ENABLE_SET_IS_ENABLED_ID,
    "is_enabled",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  is_enabled:\r\n"
    "  TRUE -\r\n"
    "  Enabled\r\n"
    "  FALSE - Disabled\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links fec_enable_set link_ndx 0 is_enabled 0\r\n"
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
    PARAM_FE600_LINKS_FEC_ENABLE_GET_FEC_ENABLE_GET_ID,
    "fec_enable_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Enables / Disables Forward Error Correction\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links fec_enable_get link_ndx 0\r\n"
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
    PARAM_FE600_LINKS_FEC_ENABLE_GET_FEC_ENABLE_GET_LINK_NDX_ID,
    "link_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  link_ndx:\r\n"
    "  The Link ID\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links fec_enable_get link_ndx 0\r\n"
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
    PARAM_FE600_LINKS_TYPE_SET_TYPE_SET_ID,
    "type_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Enables / Disables Forward Error Correction\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links type_set link_ndx 0 link_type 0\r\n"
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
    PARAM_FE600_LINKS_TYPE_SET_TYPE_SET_LINK_NDX_ID,
    "link_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  link_ndx:\r\n"
    "  The Link ID\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links type_set link_ndx 0 link_type 0\r\n"
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
    PARAM_FE600_LINKS_TYPE_SET_TYPE_SET_LINK_TYPE_ID,
    "link_type",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  link_type:\r\n"
    "  Link Type. Range 0-1\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links type_set link_ndx 0 link_type 0\r\n"
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
    PARAM_FE600_LINKS_TYPE_GET_TYPE_GET_ID,
    "type_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Enables / Disables Forward Error Correction\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links type_get link_ndx 0\r\n"
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
    PARAM_FE600_LINKS_TYPE_GET_TYPE_GET_LINK_NDX_ID,
    "link_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  link_ndx:\r\n"
    "  The Link ID\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links type_get link_ndx 0\r\n"
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
    PARAM_FE600_LINKS_PER_TYPE_PARAMS_SET_PER_TYPE_PARAMS_SET_ID,
    "per_type_params_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Enables / Disables Forward Error Correction\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links per_type_params_set link_type_ndx 0 gci_fc_th_index 0\r\n"
    "  gci_fc_th 0 rci_fc_th 0 drop_non_tdm_th 0 almost_full_th 0\r\n"
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
    PARAM_FE600_LINKS_PER_TYPE_PARAMS_SET_PER_TYPE_PARAMS_SET_LINK_TYPE_NDX_ID,
    "link_type_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  link_type_ndx:\r\n"
    "  The link type index to be configured\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links per_type_params_set link_type_ndx 0 gci_fc_th_index 0\r\n"
    "  gci_fc_th 0 rci_fc_th 0 drop_non_tdm_th 0 almost_full_th 0\r\n"
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
    PARAM_FE600_LINKS_PER_TYPE_PARAMS_SET_PER_TYPE_PARAMS_SET_TYPE_PARAMS_GCI_FC_TH_INDEX_ID,
    "gci_fc_th_index",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  gci_fc_th_index:\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links per_type_params_set link_type_ndx 0 gci_fc_th_index 0\r\n"
    "  gci_fc_th 0 rci_fc_th 0 drop_non_tdm_th 0 almost_full_th 0\r\n"
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
    PARAM_FE600_LINKS_PER_TYPE_PARAMS_SET_PER_TYPE_PARAMS_SET_TYPE_PARAMS_GCI_FC_TH_ID,
    "gci_fc_th",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  type_params.gci_fc_th[ prm_gci_fc_th_index]:\r\n"
    "  The thresholds above which GCI flow control indication is issued.Three\r\n"
    "  thresholds for generation of Low / Med / High congestion level.Entry 0 in\r\n"
    "  the array stands for the low priority.Resolution: Number of cells in the\r\n"
    "  Queue. Range 0-255\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links per_type_params_set link_type_ndx 0 gci_fc_th_index 0\r\n"
    "  gci_fc_th 0 rci_fc_th 0 drop_non_tdm_th 0 almost_full_th 0\r\n"
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
    PARAM_FE600_LINKS_PER_TYPE_PARAMS_SET_PER_TYPE_PARAMS_SET_TYPE_PARAMS_RCI_FC_TH_ID,
    "rci_fc_th",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  type_params.rci_fc_th:\r\n"
    "  The threshold above which RCI flow control indication is issuedResolution:\r\n"
    "  Number of cells in the Queue. Range 0-255\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links per_type_params_set link_type_ndx 0 gci_fc_th_index 0\r\n"
    "  gci_fc_th 0 rci_fc_th 0 drop_non_tdm_th 0 almost_full_th 0\r\n"
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
    PARAM_FE600_LINKS_PER_TYPE_PARAMS_SET_PER_TYPE_PARAMS_SET_TYPE_PARAMS_DROP_NON_TDM_TH_ID,
    "drop_non_tdm_th",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  type_params.drop_non_tdm_th:\r\n"
    "  The threshold above which non TDM traffic is dropped.Resolution: Number of\r\n"
    "  cells in the Queue. Range 0-511\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links per_type_params_set link_type_ndx 0 gci_fc_th_index 0\r\n"
    "  gci_fc_th 0 rci_fc_th 0 drop_non_tdm_th 0 almost_full_th 0\r\n"
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
    PARAM_FE600_LINKS_PER_TYPE_PARAMS_SET_PER_TYPE_PARAMS_SET_TYPE_PARAMS_ALMOST_FULL_TH_ID,
    "almost_full_th",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  type_params.almost_full_th:\r\n"
    "  The threshold above which traffic is dropped.Resolution: Number of cells in\r\n"
    "  the Queue. Range 0-511\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links per_type_params_set link_type_ndx 0 gci_fc_th_index 0\r\n"
    "  gci_fc_th 0 rci_fc_th 0 drop_non_tdm_th 0 almost_full_th 0\r\n"
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
    PARAM_FE600_LINKS_PER_TYPE_PARAMS_GET_PER_TYPE_PARAMS_GET_ID,
    "per_type_params_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Enables / Disables Forward Error Correction\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links per_type_params_get link_type_ndx 0\r\n"
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
    PARAM_FE600_LINKS_PER_TYPE_PARAMS_GET_PER_TYPE_PARAMS_GET_LINK_TYPE_NDX_ID,
    "link_type_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  link_type_ndx:\r\n"
    "  The link type index to be configured\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links per_type_params_get link_type_ndx 0\r\n"
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
    PARAM_FE600_LINKS_STATUS_GET_STATUS_GET_ID,
    "status_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get Real-time lower level indications and errors of the links\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links status_get\r\n"
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
    PARAN_FE600_LINKS_ACCEPT_CELLS_GET_ID,
    "accept_cells_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Check if a specific link is accepting cells\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links accept_cells_get link_ndx 0\r\n"
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
    PARAN_FE600_LINKS_ACCEPT_CELLS_GET_LINK_NDX_ID,    
    "link_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  link_ndx:\r\n"
    "  The Link ID\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api links accept_cells_get link_ndx 0\r\n"
    "",
    "",
    {0, 0, 0, BIT(5)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  { 
    PARAM_FE600_LINKS_Q_MAX_OCCUPANCY_GET_Q_MAX_OCCUPANCY_GET_ID, 
    "q_max_occupancy_get", 
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0], 
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Get the location (i.e., the links) whose FIFO has a maximum occupancy.\n\r"
    "",
#if UI_FE600_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  fe600_api links q_max_occupancy_get is_dch_enabled 0 is_dcl_enabled 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(100), 
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_FE600_LINKS_Q_MAX_OCCUPANCY_GET_Q_MAX_OCCUPANCY_GET_Q_LOCATION_IS_DCH_ENABLED_ID, 
    "is_dch_enabled", 
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0], 
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  q_location.is_dch_enabled:\n\r"
    "  If True, then the cell queue sizes in the DCH are compared. Otherwise, these\n\r"
    "  queues are ignored.\n\r"
    "",
#if UI_FE600_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  fe600_api links q_max_occupancy_get is_dch_enabled 0 is_dcl_enabled 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(100), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_FE600_LINKS_Q_MAX_OCCUPANCY_GET_Q_MAX_OCCUPANCY_GET_Q_LOCATION_IS_DCL_ENABLED_ID, 
    "is_dcl_enabled", 
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0], 
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  q_location.is_dcl_enabled:\n\r"
    "  If True, then the cell queue sizes in the DCL are compared. Otherwise, these\n\r"
    "  queues are ignored.\n\r"
    "",
#if UI_FE600_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  fe600_api links q_max_occupancy_get is_dch_enabled 0 is_dcl_enabled 0\n\r"
#endif 
    "",
    "",
    BIT_UI_16(100), 
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  {
    PARAM_FE600_API_LINKS_ID,
    "links",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(24)|BIT(25)|BIT(26)|BIT(27)|BIT(28)|BIT(29)|BIT(30)|BIT(31), BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4), BIT(25), BIT(4)|BIT(5)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } links*/
#ifdef UI_FE600_MULTICAST/* { multicast*/
  {
    PARAM_FE600_MULTICAST_CONGESTION_INFO_SET_CONGESTION_INFO_SET_ID,
    "congestion_info_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set Multicast table mode.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast congestion_info_set mci1_th 0 mci0_th 0\r\n"
    "  enable_gci_generation 0 enable_mci_generation 0 enable_pass 0\r\n"
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
    PARAM_FE600_MULTICAST_CONGESTION_INFO_SET_CONGESTION_INFO_SET_INFO_MCI1_TH_ID,
    "mci1_th",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.mci1_th:\r\n"
    "  Above this threshold, if enable_mci_generation the device generates\r\n"
    "  high-level multicast congestion, and if enable_gci_generation the device\r\n"
    "  set the GCI indication to 2.Range: 0-64\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast congestion_info_set mci1_th 0 mci0_th 0\r\n"
    "  enable_gci_generation 0 enable_mci_generation 0 enable_pass 0\r\n"
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
    PARAM_FE600_MULTICAST_CONGESTION_INFO_SET_CONGESTION_INFO_SET_INFO_MCI0_TH_ID,
    "mci0_th",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.mci0_th:\r\n"
    "  Above this threshold, if enable_mci_generation the device generates\r\n"
    "  low-level multicast congestion, and if enable_gci_generation the device set\r\n"
    "  the GCI indication to 1.Range: 0-64\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast congestion_info_set mci1_th 0 mci0_th 0\r\n"
    "  enable_gci_generation 0 enable_mci_generation 0 enable_pass 0\r\n"
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
    PARAM_FE600_MULTICAST_CONGESTION_INFO_SET_CONGESTION_INFO_SET_INFO_ENABLE_GCI_GENERATION_ID,
    "enable_gci_generation",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.enable_gci_generation:\r\n"
    "  When TRUE, the multicast thresholds generate Global Congestion Indication as\r\n"
    "  well\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast congestion_info_set mci1_th 0 mci0_th 0\r\n"
    "  enable_gci_generation 0 enable_mci_generation 0 enable_pass 0\r\n"
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
    PARAM_FE600_MULTICAST_CONGESTION_INFO_SET_CONGESTION_INFO_SET_INFO_ENABLE_MCI_GENERATION_ID,
    "enable_mci_generation",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.enable_mci_generation:\r\n"
    "  When TRUE, the device generate MCI indications according to congestion level\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast congestion_info_set mci1_th 0 mci0_th 0\r\n"
    "  enable_gci_generation 0 enable_mci_generation 0 enable_pass 0\r\n"
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
    PARAM_FE600_MULTICAST_CONGESTION_INFO_SET_CONGESTION_INFO_SET_INFO_ENABLE_PASS_ID,
    "enable_pass",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.enable_pass:\r\n"
    "  Enable passing of remote MCI indication. When FALSE, the indication bits are\r\n"
    "  negated.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast congestion_info_set mci1_th 0 mci0_th 0\r\n"
    "  enable_gci_generation 0 enable_mci_generation 0 enable_pass 0\r\n"
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
    PARAM_FE600_MULTICAST_CONGESTION_INFO_GET_CONGESTION_INFO_GET_ID,
    "congestion_info_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set Multicast table mode.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast congestion_info_get\r\n"
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
    PARAM_FE600_MULTICAST_LOW_PRIO_SET_LOW_PRIO_SET_ID,
    "low_prio_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set range of multicast Ids, with lower priority than the rest of\r\n"
    "  the multicast traffic.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast low_prio_set low_prio_mult_th_down 0 low_prio_mult_th_up\r\n"
    "  0 last_mid 0 first_mid 0 enable 0\r\n"
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
    PARAM_FE600_MULTICAST_LOW_PRIO_SET_LOW_PRIO_SET_INFO_LOW_PRIO_MULT_TH_DOWN_ID,
    "low_prio_mult_th_down",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.low_prio_mult_th_down:\r\n"
    "  Define the threshold for stop dropping low priority multicast.When lower than\r\n"
    "  low_prio_mult_th_up hysteresis is enabled.Range: 0-512\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast low_prio_set low_prio_mult_th_down 0 low_prio_mult_th_up\r\n"
    "  0 last_mid 0 first_mid 0 enable 0\r\n"
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
    PARAM_FE600_MULTICAST_LOW_PRIO_SET_LOW_PRIO_SET_INFO_LOW_PRIO_MULT_TH_UP_ID,
    "low_prio_mult_th_up",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.low_prio_mult_th_up:\r\n"
    "  Define the threshold for dropping low priority multicast.Range: 0-512\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast low_prio_set low_prio_mult_th_down 0 low_prio_mult_th_up\r\n"
    "  0 last_mid 0 first_mid 0 enable 0\r\n"
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
    PARAM_FE600_MULTICAST_LOW_PRIO_SET_LOW_PRIO_SET_INFO_LAST_MID_ID,
    "last_mid",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.last_mid:\r\n"
    "  Last multicast group ID with lower priority. Range 0-(16K-1)\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast low_prio_set low_prio_mult_th_down 0 low_prio_mult_th_up\r\n"
    "  0 last_mid 0 first_mid 0 enable 0\r\n"
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
    PARAM_FE600_MULTICAST_LOW_PRIO_SET_LOW_PRIO_SET_INFO_FIRST_MID_ID,
    "first_mid",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.first_mid:\r\n"
    "  First multicast group ID with lower priority. Range 0-(16K-1)\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast low_prio_set low_prio_mult_th_down 0 low_prio_mult_th_up\r\n"
    "  0 last_mid 0 first_mid 0 enable 0\r\n"
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
    PARAM_FE600_MULTICAST_LOW_PRIO_SET_LOW_PRIO_SET_INFO_ENABLE_ID,
    "enable",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.enable:\r\n"
    "  Enable Low priority multicast\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast low_prio_set low_prio_mult_th_down 0 low_prio_mult_th_up\r\n"
    "  0 last_mid 0 first_mid 0 enable 0\r\n"
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
    PARAM_FE600_MULTICAST_LOW_PRIO_GET_LOW_PRIO_GET_ID,
    "low_prio_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set range of multicast Ids, with lower priority than the rest of\r\n"
    "  the multicast traffic.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast low_prio_get\r\n"
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
    PARAM_FE600_MULTICAST_GROUP_DIRECT_SET_GROUP_DIRECT_SET_ID,
    "group_direct_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set Multicast group.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast group_direct_set multicast_id_ndx 0 faps_index 0 faps 0\r\n"
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
    PARAM_FE600_MULTICAST_GROUP_DIRECT_SET_GROUP_DIRECT_SET_MULTICAST_ID_NDX_ID,
    "multicast_id_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  multicast_id_ndx:\r\n"
    "  The multicast ID to be configured\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast group_direct_set multicast_id_ndx 0 faps_index 0 faps 0\r\n"
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
    PARAM_FE600_MULTICAST_GROUP_DIRECT_SET_GROUP_DIRECT_SET_FAPS_INDEX_ID,
    "faps_index",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  faps_index:\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast group_direct_set multicast_id_ndx 0 faps_index 0 faps 0\r\n"
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
    PARAM_FE600_MULTICAST_GROUP_DIRECT_SET_GROUP_DIRECT_SET_FAPS_ID,
    "faps",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  faps[ prm_faps_index]:\r\n"
    "  FE600_FAPS_BM_SIZE]\r\n"
    "  -\r\n"
    "  FAPs Bit Map. For Bit X in faps[I]:\r\n"
    "  If Asserted: Destination FAP X + (32*I) is in the multicast\r\n"
    "  topology \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast group_direct_set multicast_id_ndx 0 faps_index 0 faps 0\r\n"
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
    PARAM_FE600_MULTICAST_GROUP_DIRECT_GET_GROUP_DIRECT_GET_ID,
    "group_direct_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set Multicast group.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast group_direct_get multicast_id_ndx 0\r\n"
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
    PARAM_FE600_MULTICAST_GROUP_DIRECT_GET_GROUP_DIRECT_GET_MULTICAST_ID_NDX_ID,
    "multicast_id_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  multicast_id_ndx:\r\n"
    "  The multicast ID to be configured\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast group_direct_get multicast_id_ndx 0\r\n"
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
    PARAM_FE600_MULTICAST_GROUP_INDIRECT_SET_GROUP_INDIRECT_SET_ID,
    "group_indirect_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set Multicast group.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast group_indirect_set multicast_id_ndx 0 links_index 0 links\r\n"
    "  0\r\n"
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
    PARAM_FE600_MULTICAST_GROUP_INDIRECT_SET_GROUP_INDIRECT_SET_MULTICAST_ID_NDX_ID,
    "multicast_id_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  multicast_id_ndx:\r\n"
    "  The multicast ID to be configured\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast group_indirect_set multicast_id_ndx 0 links_index 0 links\r\n"
    "  0\r\n"
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
    PARAM_FE600_MULTICAST_GROUP_INDIRECT_SET_GROUP_INDIRECT_SET_LINKS_INDEX_ID,
    "ids_index",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ids_index:\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast group_indirect_set multicast_id_ndx 0 ids_index 0 ids\r\n"
    "  0\r\n"
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
    PARAM_FE600_MULTICAST_GROUP_INDIRECT_SET_GROUP_INDIRECT_SET_LINKS_ID,
    "ids",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  ids[ prm_ids_index]:\r\n"
    "  FE600_LINKS_BM_SIZE]\r\n"
    "  -\r\n"
    "  Links Bit Map. For Bit X in faps[I]:\r\n"
    "  If Asserted: id X + (32*I) is in the multicast topology \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast group_indirect_set multicast_id_ndx 0 ids_index 0 ids\r\n"
    "  0\r\n"
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
    PARAM_FE600_MULTICAST_GROUP_INDIRECT_GET_GROUP_INDIRECT_GET_ID,
    "group_indirect_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set Multicast group.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast group_indirect_get multicast_id_ndx 0\r\n"
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
    PARAM_FE600_MULTICAST_GROUP_INDIRECT_GET_GROUP_INDIRECT_GET_MULTICAST_ID_NDX_ID,
    "multicast_id_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  multicast_id_ndx:\r\n"
    "  The multicast ID to be configured\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api multicast group_indirect_get multicast_id_ndx 0\r\n"
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
    PARAM_FE600_API_MULTICAST_ID,
    "multicast",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(5)|BIT(6)|BIT(7)|BIT(8)|BIT(9)|BIT(10)|BIT(11)|BIT(12), 0, 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } multicast*/
#ifdef UI_FE600_STATUS/* { status*/
  {
    PARAM_FE600_STATUS_COUNTERS_AND_INTERRUPTS_GET_COUNTERS_AND_INTERRUPTS_GET_ID,
    "counters_and_interrupts_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Get the values of counters and interrupts according to flavor.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api status counters_and_interrupts_get\r\n"
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
    PARAM_FE600_STATUS_COUNTERS_AND_INTERRUPTS_PRINT_COUNTERS_AND_INTERRUPTS_PRINT_FLAVOR_ID,
    "flavor",
    (PARAM_VAL_RULES *)&FE600_STAT_COUNTER_FLAVOR_rule[0],
    (sizeof(FE600_STAT_COUNTER_FLAVOR_rule) / sizeof(FE600_STAT_COUNTER_FLAVOR_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  flavor:\r\n"
    "  Sub-Set of the counters and interrupts: All; Errors or Main Road.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api status counters_and_interrupts_print flavor 0\r\n"
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
    PARAM_FE600_STAT_REGS_DUMP_STAT_REGS_DUMP_ID,
    "stat_regs_dump",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function prints all registers in the range start-end.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api status stat_regs_dump start 0 end 0\r\n"
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
    PARAM_FE600_STAT_REGS_DUMP_STAT_REGS_DUMP_START_ID,
    "start",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  start:\r\n"
    "  First register in the range. Range: 0 - 0x3500.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api status stat_regs_dump start 0 end 0\r\n"
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
    PARAM_FE600_STAT_REGS_DUMP_STAT_REGS_DUMP_END_ID,
    "end",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  end:\r\n"
    "  Last register in the range. Range: 0 - 0x3500.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api status stat_regs_dump start 0 end 0\r\n"
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
    PARAM_FE600_STAT_TBLS_DUMP_STAT_TBLS_DUMP_ID,
    "stat_tbls_dump",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function prints all devices tables.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api status stat_tbls_dump print_zero 0\r\n"
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
    PARAM_FE600_STAT_TBLS_DUMP_STAT_TBLS_DUMP_PRINT_ZERO_ID,
    "print_zero",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  print_zero:\r\n"
    "  TRUE - Print entries filled with 0.\r\n"
    "  FALSE - Do not print entries filled with 0.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api status stat_tbls_dump print_zero 0\r\n"
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
    PARAM_FE600_STAT_ECI_ACCESS_TST_DBG_ECI_ACCESS_TST_ID, 
    "eci_access_tst", 
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0], 
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  This function tests the ECI access to Soc_petra. \n\r"
    "",
#if UI_FE600_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  fe600_api debug eci_access_tst nof_k_iters 0 use_dflt_tst_reg 0\n\r"
    "  reg1_addr_longs 0 reg2_addr_longs 0\n\r"
#endif 
    "",
    "",
    {0, 0, BIT(31)},
    2, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_FE600_STAT_ECI_ACCESS_TST_DBG_ECI_ACCESS_TST_NOF_K_ITERS_ID, 
    "nof_k_iters", 
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0], 
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  nof_k_iters:\n\r"
    "  Number of iterations, in 1000 units. For example, setting '100' runs 100000\n\r"
    "  iterations.\n\r"
    "",
#if UI_FE600_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  fe600_api debug eci_access_tst nof_k_iters 0 use_dflt_tst_reg 0\n\r"
    "  reg1_addr_longs 0 reg2_addr_longs 0\n\r"
#endif 
    "",
    "",
    {0, 0, BIT(31)},
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_FE600_STAT_ECI_ACCESS_TST_DBG_ECI_ACCESS_TST_USE_DFLT_TST_REG_ID, 
    "use_dflt_tst_reg", 
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0], 
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  use_dflt_tst_reg:\n\r"
    "  If TRUE, the default test register is used. This register inverts the data\n\r"
    "  written to it. If this option is selected, the following register addresses\n\r"
    "  are not required.\n\r"
    "",
#if UI_FE600_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  fe600_api debug eci_access_tst nof_k_iters 0 use_dflt_tst_reg 0\n\r"
    "  reg1_addr_longs 0 reg2_addr_longs 0\n\r"
#endif 
    "",
    "",
    {0, 0, BIT(31)},
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_FE600_STAT_ECI_ACCESS_TST_DBG_ECI_ACCESS_TST_REG1_ADDR_LONGS_ID, 
    "reg1_addr_longs", 
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0], 
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  reg1_addr_longs:\n\r"
    "  Optional (only if use_dflt_tst_reg is FALSE). If not using the default test\n\r"
    "  register, the data is written to intermittently to the two registers\n\r"
    "  supplied. The register address is in longs (i.e. the same way it is described\n\r"
    "  in the Data Sheet.)\n\r"
    "",
#if UI_FE600_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  fe600_api debug eci_access_tst nof_k_iters 0 use_dflt_tst_reg 0\n\r"
    "  reg1_addr_longs 0 reg2_addr_longs 0\n\r"
#endif 
    "",
    "",
    {0, 0, BIT(31)},
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_FE600_STAT_ECI_ACCESS_TST_DBG_ECI_ACCESS_TST_REG2_ADDR_LONGS_ID, 
    "reg2_addr_longs", 
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0], 
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  reg2_addr_longs:\n\r"
    "  Optional (only if use_dflt_tst_reg is FALSE) If not using the default test\n\r"
    "  register, the data is written to intermittently to the two registers\n\r"
    "  supplied. The register address is in longs (i.e. the same way it is described\n\r"
    "  in the Data Sheet.)\n\r"
    "",
#if UI_FE600_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  fe600_api debug eci_access_tst nof_k_iters 0 use_dflt_tst_reg 0\n\r"
    "  reg1_addr_longs 0 reg2_addr_longs 0\n\r"
#endif 
    "",
    "",
    {0, 0, BIT(31)},
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  }, 
  { 
    PARAM_FE600_DIAG_SOFT_ERROR_TEST_START_DIAG_SOFT_ERROR_TEST_START_ID, 
    "soft_error_test_start", 
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0], 
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Start the soft-error test analyzing the memory validity.\n\r"
    "",
#if UI_FE600_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  fe600_api diagnostics diag_soft_error_test_start sms 0 pattern 0\n\r"
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
    PARAM_FE600_DIAG_SOFT_ERROR_TEST_START_DIAG_SOFT_ERROR_TEST_START_INFO_PATTERN_ID, 
    "pattern", 
    (PARAM_VAL_RULES *)&FE600_DIAG_SOFT_ERROR_PATTERN_rule[0], 
    (sizeof(FE600_DIAG_SOFT_ERROR_PATTERN_rule) / sizeof(FE600_DIAG_SOFT_ERROR_PATTERN_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.pattern:\n\r"
    "  Pattern of the data written into the memories.\n\r"
    "",
#if UI_FE600_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  fe600_api diagnostics diag_soft_error_test_start sms 0 pattern 0\n\r"
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
    PARAM_FE600_DIAG_SOFT_ERROR_TEST_START_DIAG_SOFT_ERROR_TEST_START_INFO_SMS_ID, 
    "sms", 
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0], 
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.sms:\n\r"
    "  SMS index memory to write the pattern in. By default, write in each SMS\n\r"
    "  (FE600_DIAG_SMS_ALL value). Range: 1 - 42.\n\r"
    "",
#if UI_FE600_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  fe600_api diagnostics diag_soft_error_test_start sms 0 pattern 0\n\r"
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
    PARAM_FE600_DIAG_SOFT_ERROR_TEST_RESULT_GET_DIAG_SOFT_ERROR_TEST_RESULT_GET_ID, 
    "soft_error_test_result_get", 
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0], 
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Start the soft-error test analyzing the memory validity.\n\r"
    "",
#if UI_FE600_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  fe600_api diagnostics diag_soft_error_test_result_get sms 0 pattern 0\n\r"
    "  count_type 0\n\r"
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
    PARAM_FE600_DIAG_SOFT_ERROR_TEST_RESULT_GET_DIAG_SOFT_ERROR_TEST_RESULT_GET_INFO_PATTERN_ID, 
    "pattern", 
    (PARAM_VAL_RULES *)&FE600_DIAG_SOFT_ERROR_PATTERN_rule[0], 
    (sizeof(FE600_DIAG_SOFT_ERROR_PATTERN_rule) / sizeof(FE600_DIAG_SOFT_ERROR_PATTERN_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.pattern:\n\r"
    "  Pattern of the data written into the memories.\n\r"
    "",
#if UI_FE600_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  fe600_api diagnostics diag_soft_error_test_result_get sms 0 pattern 0\n\r"
    "  count_type 0\n\r"
#endif 
    "",
    "",
    {0, 0, 0, BIT(7)},
    3, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_FE600_DIAG_SOFT_ERROR_TEST_RESULT_GET_DIAG_SOFT_ERROR_TEST_RESULT_GET_INFO_SMS_ID, 
    "sms", 
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0], 
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.sms:\n\r"
    "  SMS index memory to write the pattern in. By default, write in each SMS\n\r"
    "  (FE600_DIAG_SMS_ALL value). Range: 1 - 29.\n\r"
    "",
#if UI_FE600_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  fe600_api diagnostics diag_soft_error_test_result_get sms 0 pattern 0\n\r"
    "  count_type 0\n\r"
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
    PARAM_FE600_DIAG_SOFT_ERROR_TEST_RESULT_GET_DIAG_SOFT_ERROR_TEST_RESULT_GET_COUNT_TYPE_ID, 
    "count_type", 
    (PARAM_VAL_RULES *)&FE600_DIAG_SOFT_COUNT_TYPE_rule[0], 
    (sizeof(FE600_DIAG_SOFT_COUNT_TYPE_rule) / sizeof(FE600_DIAG_SOFT_COUNT_TYPE_rule[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  count_type:\n\r"
    "  Count type of the errors.\n\r"
    "",
#if UI_FE600_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  fe600_api diagnostics diag_soft_error_test_result_get sms 0 pattern 0\n\r"
    "  count_type 0\n\r"
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
    PARAM_FE600_DIAG_MBIST_RUN_ID, 
    "mbist", 
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0], 
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1, 
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  Start the soft-error test analyzing the memory validity.\n\r"
    "",
#if UI_FE600_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  fe600_api diagnostics mbist sms 0 \n\r"
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
    PARAM_FE600_DIAG_MBIST_RUN_INFO_SMS_ID, 
    "sms", 
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0], 
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.sms:\n\r"
    "  SMS index memory to write the pattern in. By default, write in each SMS\n\r"
    "  (FE600_DIAG_SMS_ALL value). Range: 1 - 29.\n\r"
    "",
#if UI_FE600_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  fe600_api diagnostics mbist sms 0 \n\r"
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
    PARAM_FE600_API_STATUS_ID,
    "status",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(13)|BIT(14)|BIT(15)|BIT(16), BIT(31), BIT(6)|BIT(7)|BIT(8)},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } status*/
#ifdef UI_FE600_SERDES/* { serdes*/
  {
    PARAM_FE600_SRD_RATE_SET_SRD_RATE_SET_ID,
    "rate_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set SerDes lane rate.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_rate_set lane_ndx 0 rate 0\r\n"
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
    PARAM_FE600_SRD_RATE_SET_SRD_RATE_SET_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&FE600_LANE_NDX_rule[0],
    (sizeof(FE600_LANE_NDX_rule) / sizeof(FE600_LANE_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_ndx:(96 means all the lanes)\r\n"
    "  SerDes lane index. Range: 0 - 95.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_rate_set lane_ndx 0 rate 0\r\n"
    "",
    "",
    {0, BIT(17), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_SRD_RATE_SET_SRD_RATE_SET_RATE_ID,
    "rate",
    (PARAM_VAL_RULES *)&FE600_SRD_DATA_RATE_rule[0],
    (sizeof(FE600_SRD_DATA_RATE_rule) / sizeof(FE600_SRD_DATA_RATE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  rate:\r\n"
    "  The rate to configure for the specified lane. Range: 312.5 - 6250 Mbps. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_rate_set lane_ndx 0 rate 0\r\n"
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
    PARAM_FE600_SRD_RATE_GET_SRD_RATE_GET_ID,
    "rate_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set SerDes lane rate.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_rate_get lane_ndx 0\r\n"
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
    PARAM_FE600_SRD_RATE_GET_SRD_RATE_GET_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&FE600_LANE_NDX_rule[0],
    (sizeof(FE600_LANE_NDX_rule) / sizeof(FE600_LANE_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_ndx:(96 means all the lanes)\r\n"
    "  SerDes lane index. Range: 0 - 95.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_rate_get lane_ndx 0\r\n"
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
    PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_EXPLICIT_SRD_TX_PHYS_PARAMS_SET_ID,
    "tx_phys_params_set_explicit",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set SerDes Physical Parameters configuration, on the transmitter side. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_tx_phys_params_set lane_ndx 0 conf_mode_ndx 0 media_type\r\n"
    "  0 post_emphasis 0 pre_emphasis 0 amplitude 0 post 0 pre 0 main 0 amp 0\r\n"
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
    PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_EXPLICIT_SRD_TX_PHYS_PARAMS_SET_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&FE600_LANE_NDX_rule[0],
    (sizeof(FE600_LANE_NDX_rule) / sizeof(FE600_LANE_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_ndx:\r\n"
    "  The index of the SerDes lane to configure. Range: 0-59.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_tx_phys_params_set lane_ndx 0 conf_mode_ndx 0 media_type\r\n"
    "  0 post_emphasis 0 pre_emphasis 0 amplitude 0 post 0 pre 0 main 0 amp 0\r\n"
    "",
    "",
    {0, BIT(19), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_EXPLCT_POST_ID,
    "post",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.conf.explct.post:\r\n"
    "  Transmitter post-emphasis, in percents.\r\n"
    "  Range: 0 - 200. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_tx_phys_params_set lane_ndx 0 conf_mode_ndx 0 media_type\r\n"
    "  0 atten 0 swing 0 post 0 pre 0 post 0 pre 0 main 0 amp 0\r\n"
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
    PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_EXPLCT_PRE_ID,
    "pre",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.conf.explct.pre:\r\n"
    "  Transmitter pre-emphasis, in percents. Applicable for SerDes rates above\r\n"
    "  3.125Gbps only. Range: 0 - 200. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_tx_phys_params_set lane_ndx 0 conf_mode_ndx 0 media_type\r\n"
    "  0 atten 0 swing 0 post 0 pre 0 post 0 pre 0 main 0 amp 0\r\n"
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
    PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_EXPLCT_SWING_ID,
    "swing",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.conf.explct.swing:\r\n"
    "  Amplitude swing. Units: mV. Range: 100 - 1500. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_tx_phys_params_set lane_ndx 0 conf_mode_ndx 0 media_type\r\n"
    "  0 atten 0 swing 0 post 0 pre 0 post 0 pre 0 main 0 amp 0\r\n"
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
    PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_ATTEN_SRD_TX_PHYS_PARAMS_SET_ID,
    "tx_phys_params_set_attenuation",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set SerDes Physical Parameters configuration, on the transmitter side. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes tx_phys_params_set_attenuation lane_ndx 0 atten 105\r\n"
    "",
    "",
    {0, 0, BIT(22)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_ATTEN_SRD_TX_PHYS_PARAMS_SET_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&FE600_LANE_NDX_rule[0],
    (sizeof(FE600_LANE_NDX_rule) / sizeof(FE600_LANE_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_ndx:\r\n"
    "  The index of the SerDes lane to configure. Range: 0-59.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes tx_phys_params_set_attenuation lane_ndx 0 atten 105\r\n"
    "",
    "",
    {0, 0, BIT(22)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_ATTEN_ID,
    "atten",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  hw_adjust.serdes.conf.lane_conf[ prm_lane_conf_index].tx_phys_conf.conf.atten:\r\n"
    "  Applicable for SerDes rates below or equal to 3.125Gbps only. Estimated media\r\n"
    "  attenuation, end to end. Units: Decibel(Db). Resolution:\r\n"
    "  0.1Db (e.g atten=67 for 6.7Db). Range: 0..120. (0.0-12.0Db).\r\n"
    "  Used only if the conf_mode is 'ATTEN' - ignored otherwise.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes tx_phys_params_set_attenuation lane_ndx 0 atten 105\r\n"
    "",
    "",
    {0, 0, BIT(22)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_INTERNAL_SRD_TX_PHYS_PARAMS_SET_ID,
    "tx_phys_params_set_internal",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set SerDes Physical Parameters configuration, on the transmitter side. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_tx_phys_params_set lane_ndx 0 conf_mode_ndx 0 media_type\r\n"
    "  0 post_emphasis 0 pre_emphasis 0 amplitude 0 post 0 pre 0 main 0 amp 0\r\n"
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
    PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_INTERNAL_SRD_TX_PHYS_PARAMS_SET_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&FE600_LANE_NDX_rule[0],
    (sizeof(FE600_LANE_NDX_rule) / sizeof(FE600_LANE_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_ndx:\r\n"
    "  The index of the SerDes lane to configure. Range: 0-59.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_tx_phys_params_set lane_ndx 0 conf_mode_ndx 0 media_type\r\n"
    "  0 post_emphasis 0 pre_emphasis 0 amplitude 0 post 0 pre 0 main 0 amp 0\r\n"
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
    PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_INTERNAL_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_INTERN_POST_ID,
    "post",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.conf.intern.post:\r\n"
    "  Transmitter post-emphasis value - internal representation. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_tx_phys_params_set lane_ndx 0 conf_mode_ndx 0 media_type\r\n"
    "  0 post_emphasis 0 pre_emphasis 0 amplitude 0 post 0 pre 0 main 0 amp 0\r\n"
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
    PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_INTERNAL_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_INTERN_PRE_ID,
    "pre",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.conf.intern.pre:\r\n"
    "  Transmitter pre-emphasis value - internal representation.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_tx_phys_params_set lane_ndx 0 conf_mode_ndx 0 media_type\r\n"
    "  0 post_emphasis 0 pre_emphasis 0 amplitude 0 post 0 pre 0 main 0 amp 0\r\n"
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
    PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_INTERNAL_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_INTERN_MAIN_ID,
    "main",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.conf.intern.main:\r\n"
    "  Transmitter main value - internal representation.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_tx_phys_params_set lane_ndx 0 conf_mode_ndx 0 media_type\r\n"
    "  0 post_emphasis 0 pre_emphasis 0 amplitude 0 post 0 pre 0 main 0 amp 0\r\n"
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
    PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_INTERNAL_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_INTERN_AMP_ID,
    "amp",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.conf.intern.amp:\r\n"
    "  Transmitter amplitude value- internal representation.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_tx_phys_params_set lane_ndx 0 conf_mode_ndx 0 media_type\r\n"
    "  0 post_emphasis 0 pre_emphasis 0 amplitude 0 post 0 pre 0 main 0 amp 0\r\n"
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
    PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_MEDIA_TYPE_SRD_TX_PHYS_PARAMS_SET_ID,
    "tx_phys_params_set_media_type",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set SerDes Physical Parameters configuration, on the transmitter side. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_tx_phys_params_set lane_ndx 0 conf_mode_ndx 0 media_type\r\n"
    "  0 post_emphasis 0 pre_emphasis 0 amplitude 0 post 0 pre 0 main 0 amp 0\r\n"
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
    PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_MEDIA_TYPE_SRD_TX_PHYS_PARAMS_SET_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&FE600_LANE_NDX_rule[0],
    (sizeof(FE600_LANE_NDX_rule) / sizeof(FE600_LANE_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_ndx:\r\n"
    "  The index of the SerDes lane to configure. Range: 0-59.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_tx_phys_params_set lane_ndx 0 conf_mode_ndx 0 media_type\r\n"
    "  0 post_emphasis 0 pre_emphasis 0 amplitude 0 post 0 pre 0 main 0 amp 0\r\n"
    "",
    "",
    {0, 0, BIT(15), 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_SRD_TX_PHYS_PARAMS_SET_MEDIA_TYPE_SRD_TX_PHYS_PARAMS_SET_INFO_CONF_MEDIA_TYPE_ID,
    "media_type",
    (PARAM_VAL_RULES *)&FE600_SRD_MEDIA_TYPE_rule[0],
    (sizeof(FE600_SRD_MEDIA_TYPE_rule) / sizeof(FE600_SRD_MEDIA_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.conf.media_type:\r\n"
    "  SerDes connecting media type. TX physical parameters are derived from the\r\n"
    "  selected media type. Used only if the conf_mode is media_type - ignored\r\n"
    "  otherwise. \r\n"
    "  info.conf.media_type:\r\n"
    "  TX physical parameters are derived from the selected media type. Used only if\r\n"
    "  the conf_mode is 'MEDIA_TYPE' - ignored otherwise.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_tx_phys_params_set lane_ndx 0 conf_mode_ndx 0 media_type\r\n"
    "  0 post_emphasis 0 pre_emphasis 0 amplitude 0 post 0 pre 0 main 0 amp 0\r\n"
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
    PARAM_FE600_SRD_TX_PHYS_PARAMS_GET_SRD_TX_PHYS_PARAMS_GET_ID,
    "tx_phys_params_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set SerDes Physical Parameters configuration, on the transmitter side. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_tx_phys_params_get lane_ndx 0 conf_mode_ndx 0\r\n"
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
    PARAM_FE600_SRD_TX_PHYS_PARAMS_GET_SRD_TX_PHYS_PARAMS_GET_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&FE600_LANE_NDX_rule[0],
    (sizeof(FE600_LANE_NDX_rule) / sizeof(FE600_LANE_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_ndx:(96 means all the lanes)\r\n"
    "  The index of the SerDes lane to configure. Range: 0-95.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_tx_phys_params_get lane_ndx 0 conf_mode_ndx 0\r\n"
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
    PARAM_FE600_SRD_TX_PHYS_PARAMS_GET_SRD_TX_PHYS_PARAMS_GET_CONF_MODE_NDX_ID,
    "conf_mode_ndx",
    (PARAM_VAL_RULES *)&FE600_SRD_TX_PHYS_CONF_MODE_rule[0],
    (sizeof(FE600_SRD_TX_PHYS_CONF_MODE_rule) / sizeof(FE600_SRD_TX_PHYS_CONF_MODE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  conf_mode_ndx:\r\n"
    "  The index of the transmitter physical parameters configuration mode.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_tx_phys_params_get lane_ndx 0 conf_mode_ndx 0\r\n"
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
    PARAM_FE600_SRD_LANE_POLARITY_SET_SRD_LANE_POLARITY_SET_ID,
    "lane_polarity_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set, per direction (RX/TX/Both) whether to swap-polarity on a certain lane. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_lane_polarity_set lane_ndx 0 direction_ndx 0\r\n"
    "  is_swap_polarity 0\r\n"
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
    PARAM_FE600_SRD_LANE_POLARITY_SET_SRD_LANE_POLARITY_SET_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&FE600_LANE_NDX_rule[0],
    (sizeof(FE600_LANE_NDX_rule) / sizeof(FE600_LANE_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_ndx:(96 means all the lanes)\r\n"
    "  The index of the SerDes lane to configure. Range: 0-95.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_lane_polarity_set lane_ndx 0 direction_ndx 0\r\n"
    "  is_swap_polarity 0\r\n"
    "",
    "",
    {0, BIT(21), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_SRD_LANE_POLARITY_SET_SRD_LANE_POLARITY_SET_DIRECTION_NDX_ID,
    "direction_ndx",
    (PARAM_VAL_RULES *)&FE600_CONNECTION_DIRECTION_rule[0],
    (sizeof(FE600_CONNECTION_DIRECTION_rule) / sizeof(FE600_CONNECTION_DIRECTION_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  direction_ndx (0:Rx, 1:Tx, 2 Both):\r\n"
    "  Direction identifier: Transmission (Tx), Reception (Rx) or Both.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_lane_polarity_set lane_ndx 0 direction_ndx 0\r\n"
    "  is_swap_polarity 0\r\n"
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
    PARAM_FE600_SRD_LANE_POLARITY_SET_SRD_LANE_POLARITY_SET_IS_SWAP_POLARITY_ID,
    "is_swap_polarity",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  is_swap_polarity:\r\n"
    "  If TRUE, polarity is swapped.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_lane_polarity_set lane_ndx 0 direction_ndx 0\r\n"
    "  is_swap_polarity 0\r\n"
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
    PARAM_FE600_SRD_LANE_POLARITY_GET_SRD_LANE_POLARITY_GET_ID,
    "lane_polarity_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set, per direction (RX/TX/Both) whether to swap-polarity on a certain lane. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_lane_polarity_get lane_ndx 0 direction_ndx 0\r\n"
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
    PARAM_FE600_SRD_LANE_POLARITY_GET_SRD_LANE_POLARITY_GET_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&FE600_LANE_NDX_rule[0],
    (sizeof(FE600_LANE_NDX_rule) / sizeof(FE600_LANE_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_ndx:(96 means all the lanes)\r\n"
    "  The index of the SerDes lane to configure. Range: 0-95.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_lane_polarity_get lane_ndx 0 direction_ndx 0\r\n"
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
    PARAM_FE600_SRD_LANE_POWER_STATE_SET_SRD_LANE_POWER_STATE_SET_ID,
    "lane_power_state_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the state of the lane - power-up/power-down/reset.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_lane_power_state_set lane_ndx 0 direction_ndx 0 state 0\r\n"
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
    PARAM_FE600_SRD_LANE_POWER_STATE_SET_SRD_LANE_POWER_STATE_SET_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&FE600_LANE_NDX_rule[0],
    (sizeof(FE600_LANE_NDX_rule) / sizeof(FE600_LANE_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_ndx:(96 means all the lanes)\r\n"
    "  The index of the SerDes lane to configure. Range: 0-95.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_lane_power_state_set lane_ndx 0 direction_ndx 0 state 0\r\n"
    "",
    "",
    {0, BIT(23), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_SRD_LANE_POWER_STATE_SET_SRD_LANE_POWER_STATE_SET_DIRECTION_NDX_ID,
    "direction_ndx",
    (PARAM_VAL_RULES *)&FE600_CONNECTION_DIRECTION_rule[0],
    (sizeof(FE600_CONNECTION_DIRECTION_rule) / sizeof(FE600_CONNECTION_DIRECTION_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  direction_ndx (0:Rx, 1:Tx, 2 Both):\r\n"
    "  Direction identifier: Transmission (Tx), Reception (Rx) or Both.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_lane_power_state_set lane_ndx 0 direction_ndx 0 state 0\r\n"
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
    PARAM_FE600_SRD_LANE_POWER_STATE_SET_SRD_LANE_POWER_STATE_SET_STATE_ID,
    "state",
    (PARAM_VAL_RULES *)&FE600_SRD_POWER_STATE_rule[0],
    (sizeof(FE600_SRD_POWER_STATE_rule) / sizeof(FE600_SRD_POWER_STATE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  state:\r\n"
    "  Lane power-state.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_lane_power_state_set lane_ndx 0 direction_ndx 0 state 0\r\n"
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
    PARAM_FE600_SRD_LANE_POWER_STATE_GET_SRD_LANE_POWER_STATE_GET_ID,
    "lane_power_state_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set the state of the lane - power-up/power-down/reset.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_lane_power_state_get lane_ndx 0 direction_ndx 0\r\n"
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
    PARAM_FE600_SRD_LANE_POWER_STATE_GET_SRD_LANE_POWER_STATE_GET_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&FE600_LANE_NDX_rule[0],
    (sizeof(FE600_LANE_NDX_rule) / sizeof(FE600_LANE_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_ndx:(96 means all the lanes)\r\n"
    "  The index of the SerDes lane to configure. Range: 0-95.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_lane_power_state_get lane_ndx 0 direction_ndx 0\r\n"
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
    PARAM_FE600_SRD_STAR_SET_SRD_STAR_SET_ID,
    "star_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Performs a SerDes star initialization sequence.\r\n"
    "  This sequence includes in-out of reset sequence for the star IPU, and the\r\n"
    "  CMU-s of the specified SerDes quartets.\r\n"
    "  It also includes CMU trimming and power-up validation.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_star_set star_ndx 0 qrtt_index 0 max_expected_lane_rate\r\n"
    "  0 is_active 0\r\n"
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
    PARAM_FE600_SRD_STAR_SET_SRD_STAR_SET_STAR_NDX_ID,
    "star_ndx",
    (PARAM_VAL_RULES *)&FE600_SRD_STAR_ID_rule[0],
    (sizeof(FE600_SRD_STAR_ID_rule) / sizeof(FE600_SRD_STAR_ID_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  star_ndx:\r\n"
    "  The Index of the star containing the IPU to initialize.\r\n"
    "  Range: 0 - 7.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_star_set star_ndx 0 qrtt_index 0 max_expected_lane_rate\r\n"
    "  0 is_active 0\r\n"
    "",
    "",
    {0, BIT(25), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_SRD_STAR_SET_SRD_STAR_SET_INFO_QRTT_INDEX_ID,
    "qrtt_index",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  qrtt_index:\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_star_set star_ndx 0 qrtt_index 0 max_expected_lane_rate\r\n"
    "  0 is_active 0\r\n"
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
    PARAM_FE600_SRD_STAR_SET_SRD_STAR_SET_INFO_QRTT_MAX_EXPECTED_LANE_RATE_ID,
    "max_expected_lane_rate",
    (PARAM_VAL_RULES *)&FE600_SRD_DATA_RATE_rule[0],
    (sizeof(FE600_SRD_DATA_RATE_rule) / sizeof(FE600_SRD_DATA_RATE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.qrtt[ prm_qrtt_index].max_expected_lane_rate:\r\n"
    "  The maximal expected rate for any lane in the quartet. Range: 312.5 - 6250\r\n"
    "  Mbps. Must be identical for all lanes per-quartet.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_star_set star_ndx 0 qrtt_index 0 max_expected_lane_rate\r\n"
    "  0 is_active 0\r\n"
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
    PARAM_FE600_SRD_STAR_SET_SRD_STAR_SET_INFO_QRTT_IS_ACTIVE_ID,
    "is_active",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.qrtt[ prm_qrtt_index].is_active:\r\n"
    "  If TRUE, the specified quartet CMU in the SerDes star is activated. Notes: 1.\r\n"
    "  If any CMU is expected to be used at some stage in the future, it must be\r\n"
    "  activated on init (the ACTIVE CMU structure). 2. Not activating a CMU (if not\r\n"
    "  expected to be used at any point) improves the device power consumption.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_star_set star_ndx 0 qrtt_index 0 max_expected_lane_rate\r\n"
    "  0 is_active 0\r\n"
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
    PARAM_FE600_SRD_STAR_GET_SRD_STAR_GET_ID,
    "star_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Performs a SerDes star initialization sequence.\r\n"
    "  This sequence includes in-out of reset sequence for the star IPU, and the\r\n"
    "  CMU-s of the specified SerDes quartets.\r\n"
    "  It also includes CMU trimming and power-up validation.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_star_get star_ndx 0\r\n"
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
    PARAM_FE600_SRD_STAR_GET_SRD_STAR_GET_STAR_NDX_ID,
    "star_ndx",
    (PARAM_VAL_RULES *)&FE600_SRD_STAR_ID_rule[0],
    (sizeof(FE600_SRD_STAR_ID_rule) / sizeof(FE600_SRD_STAR_ID_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  star_ndx:\r\n"
    "  The Index of the star containing the IPU to initialize.\r\n"
    "  Range: 0 - 7.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_star_get star_ndx 0\r\n"
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
    PARAM_FE600_SRD_QRTT_SET_SRD_QRTT_SET_ID,
    "qrtt_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Performs a SerDes Quartet reset/initialization sequence. The maximal\r\n"
    "  supported rate of the SerDes quartet is set as part of this initialization.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_qrtt_set qrtt_ndx 0 max_expected_lane_rate 0 is_active 0\r\n"
    "",
    "",
    {0, 0, BIT(23)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_SRD_QRTT_SET_SRD_QRTT_SET_QRTT_NDX_ID,
    "qrtt_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  qrtt_ndx:\r\n"
    "  The Index of the SerDes quartet to initialize.\r\n"
    "  Range: 0 - 15.  \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_qrtt_set qrtt_ndx 0 max_expected_lane_rate 0 is_active 0\r\n"
    "",
    "",
    {0, 0, BIT(23)},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_SRD_QRTT_SET_SRD_QRTT_SET_INFO_MAX_EXPECTED_LANE_RATE_ID,
    "max_expected_lane_rate",
    (PARAM_VAL_RULES *)&FE600_SRD_DATA_RATE_rule[0],
    (sizeof(FE600_SRD_DATA_RATE_rule) / sizeof(FE600_SRD_DATA_RATE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.max_expected_lane_rate:\r\n"
    "  The maximal expected rate for any lane in the quartet. Range: 1000 -\r\n"
    "  6250.Units: Mbps. Must be identical for all lanes per-quartet.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_qrtt_set qrtt_ndx 0 max_expected_lane_rate 0 is_active 0\r\n"
    "",
    "",
    {0, 0, BIT(23)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_SRD_QRTT_SET_SRD_QRTT_SET_INFO_IS_ACTIVE_ID,
    "is_active",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.is_active:\r\n"
    "  If TRUE, the specified quartet CMU in the SerDes star is activated. Notes: 1.\r\n"
    "  If any CMU is expected to be used at some stage in the future, it must be\r\n"
    "  activated on init (the ACTIVE CMU structure). 2. Not activating a CMU (if not\r\n"
    "  expected to be used at any point) improves the device power consumption.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_qrtt_set qrtt_ndx 0 max_expected_lane_rate 0 is_active 0\r\n"
    "",
    "",
    {0, 0, BIT(23)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_SRD_QRTT_GET_SRD_QRTT_GET_ID,
    "qrtt_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Performs a SerDes Quartet reset/initialization sequence. The maximal\r\n"
    "  supported rate of the SerDes quartet is set as part of this initialization.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_qrtt_get qrtt_ndx 0\r\n"
    "",
    "",
    {0, 0, BIT(24)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_SRD_QRTT_GET_SRD_QRTT_GET_QRTT_NDX_ID,
    "qrtt_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  qrtt_ndx:\r\n"
    "  The Index of the SerDes quartet to initialize.\r\n"
    "  Range: 0 - 15.  \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_qrtt_get qrtt_ndx 0\r\n"
    "",
    "",
    {0, 0, BIT(24)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },

  {
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_ID,
    "all_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set SerDes parameters, for the selected SerDes. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  post 0 pre 0 main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_INDEX_ID,
    "lane_conf_index",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_conf_index:\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  post 0 pre 0 main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_IS_SWAP_POLARITY_RX_ID,
    "is_swap_polarity_rx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.lane_conf[ prm_lane_conf_index].is_swap_polarity_rx:\r\n"
    "  If TRUE, polarity is swapped (RX).\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  post 0 pre 0 main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_IS_SWAP_POLARITY_TX_ID,
    "is_swap_polarity_tx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.lane_conf[ prm_lane_conf_index].is_swap_polarity_tx:\r\n"
    "  If TRUE, polarity is swapped (TX).\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  post 0 pre 0 main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_POWER_STATE_CONF_ID,
    "power_state_conf",
    (PARAM_VAL_RULES *)&FE600_SRD_POWER_STATE_rule[0],
    (sizeof(FE600_SRD_POWER_STATE_rule) / sizeof(FE600_SRD_POWER_STATE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.lane_conf[ prm_lane_conf_index].power_state_conf:\r\n"
    "  Per-SerDes Lane power state configuration parameters. Note: if enabled, the\r\n"
    "  configuration is set for both direction (receive and transmit). To set\r\n"
    "  different configuration per-direction - either set FALSE here and configure\r\n"
    "  using dedicated API, or override one of the directions using dedicated API. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  post 0 pre 0 main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_RX_PHYS_CONF_INTERN_G1CNT_ID,
    "g1cnt",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.lane_conf[ prm_lane_conf_index].rx_phys_conf.intern.g1cnt:\r\n"
    "  Receiver g1cnt value- internal representation.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  post 0 pre 0 main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_RX_PHYS_CONF_INTERN_TLTH_ID,
    "tlth",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.lane_conf[ prm_lane_conf_index].rx_phys_conf.intern.tlth:\r\n"
    "  Receiver tlth value- internal representation.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  post 0 pre 0 main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_RX_PHYS_CONF_INTERN_DFELTH_ID,
    "dfelth",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.lane_conf[ prm_lane_conf_index].rx_phys_conf.intern.dfelth:\r\n"
    "  Receiver dfelth value- internal representation.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  post 0 pre 0 main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_RX_PHYS_CONF_INTERN_Z1CNT_ID,
    "z1cnt",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.lane_conf[ prm_lane_conf_index].rx_phys_conf.intern.z1cnt:\r\n"
    "  Receiver z1cnt value- internal representation.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  post 0 pre 0 main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_RX_PHYS_CONF_INTERN_ZCNT_ID,
    "zcnt",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.lane_conf[ prm_lane_conf_index].rx_phys_conf.intern.zcnt:\r\n"
    "  Receiver zcnt value- internal representation.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  post 0 pre 0 main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_MODE_ID,
    "tx_phys_conf_mode",
    (PARAM_VAL_RULES *)&FE600_SRD_TX_PHYS_CONF_MODE_rule[0],
    (sizeof(FE600_SRD_TX_PHYS_CONF_MODE_rule) / sizeof(FE600_SRD_TX_PHYS_CONF_MODE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.lane_conf[ prm_lane_conf_index].tx_phys_conf_mode:\r\n"
    "  Per-SerDes Lane Transmitter physical parameters configuration mode - by\r\n"
    "  media-type, explicit or internal.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  post 0 pre 0 main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_MEDIA_TYPE_ID,
    "media_type",
    (PARAM_VAL_RULES *)&FE600_SRD_MEDIA_TYPE_rule[0],
    (sizeof(FE600_SRD_MEDIA_TYPE_rule) / sizeof(FE600_SRD_MEDIA_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.lane_conf[ prm_lane_conf_index].tx_phys_conf.conf.media_type:\r\n"
    "  TX physical parameters are derived from the selected media type. Used only if\r\n"
    "  the conf_mode is 'MEDIA_TYPE' - ignored otherwise.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  post 0 pre 0 main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_ATTEN_ID,
    "atten",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  hw_adjust.serdes.conf.lane_conf[ prm_lane_conf_index].tx_phys_conf.conf.atten:\r\n"
    "  Applicable for SerDes rates below or equal to 3.125Gbps only. Estimated media\r\n"
    "  attenuation, end to end. Units: Decibel(Db). Resolution:\r\n"
    "  0.1Db (e.g atten=67 for 6.7Db). Range: 0..120. (0.0-12.0Db).\r\n"
    "  Used only if the conf_mode is 'ATTEN' - ignored otherwise.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 atten 0 swing 0 post 0 pre 0 post 0 pre 0\r\n"
    "  main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_EXPLCT_SWING_ID,
    "swing",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.lane_conf[ prm_lane_conf_index].tx_phys_conf.conf.explct.swing:\r\n"
    "  Amplitude swing. Units: mV. Range: 100 - 1500. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 atten 0 swing 0 post 0 pre 0 post 0 pre 0\r\n"
    "  main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_EXPLCT_POST_ID,
    "post",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.lane_conf[ prm_lane_conf_index].tx_phys_conf.conf.explct.post:\r\n"
    "  Transmitter post-emphasis, in percents.\r\n"
    "  Range: 0 - 200. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 atten 0 swing 0 post 0 pre 0 post 0 pre 0\r\n"
    "  main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_EXPLCT_PRE_ID,
    "pre",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.lane_conf[ prm_lane_conf_index].tx_phys_conf.conf.explct.pre:\r\n"
    "  Transmitter pre-emphasis, in percents. Applicable for SerDes rates above\r\n"
    "  3.125Gbps only. Range: 0 - 200. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 atten 0 swing 0 post 0 pre 0 post 0 pre 0\r\n"
    "  main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_INTERN_POST_ID,
    "post",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.lane_conf[ prm_lane_conf_index].tx_phys_conf.conf.intern.post:\r\n"
    "  Transmitter post-emphasis value - internal representation. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  post 0 pre 0 main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_INTERN_PRE_ID,
    "pre",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.lane_conf[ prm_lane_conf_index].tx_phys_conf.conf.intern.pre:\r\n"
    "  Transmitter pre-emphasis value - internal representation.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  post 0 pre 0 main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_INTERN_MAIN_ID,
    "main",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.lane_conf[ prm_lane_conf_index].tx_phys_conf.conf.intern.main:\r\n"
    "  Transmitter main value - internal representation.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  post 0 pre 0 main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_TX_PHYS_CONF_CONF_INTERN_AMP_ID,
    "amp",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.lane_conf[ prm_lane_conf_index].tx_phys_conf.conf.intern.amp:\r\n"
    "  Transmitter amplitude value- internal representation.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  post 0 pre 0 main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_RATE_CONF_ID,
    "rate_conf",
    (PARAM_VAL_RULES *)&FE600_SRD_DATA_RATE_rule[0],
    (sizeof(FE600_SRD_DATA_RATE_rule) / sizeof(FE600_SRD_DATA_RATE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.lane_conf[ prm_lane_conf_index].rate_conf:\r\n"
    "  Per-SerDes Lane rate configuration parameters. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  post 0 pre 0 main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_LANE_CONF_ENABLE_ID,
    "enable",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.lane_conf[ prm_lane_conf_index].enable:\r\n"
    "  If TRUE, the lane configuration is applied. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  post 0 pre 0 main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_STAR_CONF_INDEX_ID,
    "star_conf_index",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  star_conf_index:\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  post 0 pre 0 main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_STAR_CONF_CONF_QRTT_INDEX_ID,
    "qrtt_index",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  qrtt_index:\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  post 0 pre 0 main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_STAR_CONF_CONF_QRTT_MAX_EXPECTED_LANE_RATE_ID,
    "max_expected_lane_rate",
    (PARAM_VAL_RULES *)&FE600_SRD_DATA_RATE_rule[0],
    (sizeof(FE600_SRD_DATA_RATE_rule) / sizeof(FE600_SRD_DATA_RATE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.star_conf[ prm_star_conf_index].conf.qrtt[\r\n"
    "  prm_qrtt_index].max_expected_lane_rate:\r\n"
    "  The maximal expected rate for any lane in the quartet. Range: 312.5 - 6250\r\n"
    "  Mbps. Must be identical for all lanes per-quartet.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  post 0 pre 0 main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_STAR_CONF_CONF_QRTT_IS_ACTIVE_ID,
    "is_active",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.star_conf[ prm_star_conf_index].conf.qrtt[ prm_qrtt_index].is_active:\r\n"
    "  If TRUE, the specified quartet CMU in the SerDes star is activated. Notes: 1.\r\n"
    "  If any CMU is expected to be used at some stage in the future, it must be\r\n"
    "  activated on init (the ACTIVE CMU structure). 2. Not activating a CMU (if not\r\n"
    "  expected to be used at any point) improves the device power consumption.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  post 0 pre 0 main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_SET_SRD_ALL_SET_INFO_STAR_CONF_ENABLE_ID,
    "enable",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.star_conf[ prm_star_conf_index].enable:\r\n"
    "  If TRUE, the STAR configuration is applied (STAR initialization sequence).\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_set lane_conf_index 0 is_swap_polarity_rx 0\r\n"
    "  is_swap_polarity_tx 0 power_state_conf 0 g1cnt 0 tlth 0 dfelth 0 z1cnt 0 zcnt\r\n"
    "  0 tx_phys_conf_mode 0 media_type 0 post_emphasis 0 pre_emphasis 0 amplitude 0\r\n"
    "  post 0 pre 0 main 0 amp 0 rate_conf 0 enable 0 star_conf_index 0 qrtt_index 0\r\n"
    "  max_expected_lane_rate 0 is_active 0 enable 0\r\n"
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
    PARAM_FE600_SRD_ALL_GET_SRD_ALL_GET_ID,
    "all_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Set SerDes parameters, for the selected SerDes. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_all_get\r\n"
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
    PARAM_FE600_SRD_AUTO_EQUALIZE_SRD_AUTO_EQUALIZE_ID,
    "auto_equalize",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Perform auto-equalization process. This process targets to achive optimal\r\n"
    "  receiver configuration, resulting in maximal eye-opening. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_auto_equalize lane_ndx 0 mode 0\r\n"
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
    PARAM_FE600_SRD_AUTO_EQUALIZE_SRD_AUTO_EQUALIZE_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&FE600_LANE_NDX_rule[0],
    (sizeof(FE600_LANE_NDX_rule) / sizeof(FE600_LANE_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_ndx:(96 means all the lanes)\r\n"
    "  SerDes lane index. Range: 0 - 95.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_auto_equalize lane_ndx 0 mode 0\r\n"
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
    PARAM_FE600_SRD_AUTO_EQUALIZE_SRD_AUTO_EQUALIZE_MODE_ID,
    "mode",
    (PARAM_VAL_RULES *)&FE600_SRD_AEQ_MODE_rule[0],
    (sizeof(FE600_SRD_AEQ_MODE_rule) / sizeof(FE600_SRD_AEQ_MODE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mode:\r\n"
    "  SerDes Auto-equalization mode.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_auto_equalize lane_ndx 0 mode 0\r\n"
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
    PARAM_FE600_SRD_RX_PHYS_PARAMS_SET_SRD_RX_PHYS_PARAMS_SET_ID,
    "rx_phys_params_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Receiver physical parameters. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_rx_phys_params_set lane_ndx 0 g1cnt 0 tlth 0 dfelth 0\r\n"
    "  z1cnt 0 zcnt 0\r\n"
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
    PARAM_FE600_SRD_RX_PHYS_PARAMS_SET_SRD_RX_PHYS_PARAMS_SET_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&FE600_LANE_NDX_rule[0],
    (sizeof(FE600_LANE_NDX_rule) / sizeof(FE600_LANE_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_ndx:(96 means all the lanes)\r\n"
    "  lane index. Range: 0 - 95.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_rx_phys_params_set lane_ndx 0 g1cnt 0 tlth 0 dfelth 0\r\n"
    "  z1cnt 0 zcnt 0\r\n"
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
    PARAM_FE600_SRD_RX_PHYS_PARAMS_SET_SRD_RX_PHYS_PARAMS_SET_INFO_G1CNT_ID,
    "g1cnt",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.g1cnt:\r\n"
    "  RX -equalizer parameter. Valid after Auto\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_rx_phys_params_set lane_ndx 0 g1cnt 0 tlth 0 dfelth 0\r\n"
    "  z1cnt 0 zcnt 0\r\n"
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
    PARAM_FE600_SRD_RX_PHYS_PARAMS_SET_SRD_RX_PHYS_PARAMS_SET_INFO_TLTH_ID,
    "tlth",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.tlth:\r\n"
    "  RX -equalizer parameter. Valid after Auto\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_rx_phys_params_set lane_ndx 0 g1cnt 0 tlth 0 dfelth 0\r\n"
    "  z1cnt 0 zcnt 0\r\n"
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
    PARAM_FE600_SRD_RX_PHYS_PARAMS_SET_SRD_RX_PHYS_PARAMS_SET_INFO_DFELTH_ID,
    "dfelth",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.dfelth:\r\n"
    "  RX -equalizer parameter. Valid after Auto\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_rx_phys_params_set lane_ndx 0 g1cnt 0 tlth 0 dfelth 0\r\n"
    "  z1cnt 0 zcnt 0\r\n"
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
    PARAM_FE600_SRD_RX_PHYS_PARAMS_SET_SRD_RX_PHYS_PARAMS_SET_INFO_Z1CNT_ID,
    "z1cnt",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.z1cnt:\r\n"
    "  RX -equalizer parameter. Valid after Auto\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_rx_phys_params_set lane_ndx 0 g1cnt 0 tlth 0 dfelth 0\r\n"
    "  z1cnt 0 zcnt 0\r\n"
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
    PARAM_FE600_SRD_RX_PHYS_PARAMS_SET_SRD_RX_PHYS_PARAMS_SET_INFO_ZCNT_ID,
    "zcnt",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.zcnt:\r\n"
    "  RX -equalizer parameter. Valid after Auto-equalization is run.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_rx_phys_params_set lane_ndx 0 g1cnt 0 tlth 0 dfelth 0\r\n"
    "  z1cnt 0 zcnt 0\r\n"
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
    PARAM_FE600_SRD_RX_PHYS_PARAMS_GET_SRD_RX_PHYS_PARAMS_GET_ID,
    "rx_phys_params_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Receiver physical parameters. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_rx_phys_params_get lane_ndx 0\r\n"
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
    PARAM_FE600_SRD_RX_PHYS_PARAMS_GET_SRD_RX_PHYS_PARAMS_GET_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&FE600_LANE_NDX_rule[0],
    (sizeof(FE600_LANE_NDX_rule) / sizeof(FE600_LANE_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_ndx:(96 means all the lanes)\r\n"
    "  lane index. Range: 0 - 95.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes srd_rx_phys_params_get lane_ndx 0\r\n"
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
    PARAM_FE600_API_SERDES_ID,
    "serdes",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, BIT(17)|BIT(18)|BIT(19)|BIT(20)|BIT(21)|BIT(22)|BIT(23)|BIT(24)|BIT(25)|BIT(26)|BIT(27)|BIT(28)|BIT(29)|BIT(30)|BIT(31), BIT(14)|BIT(15)|BIT(22)|BIT(23)|BIT(24), 0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } serdes*/
#ifdef UI_FE600_CELL /* { cell*/
  {
    PARAM_FE600_API_CELLS_ID,
    "cell",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {0, 0, BIT(16)|BIT(17)|BIT(18)|BIT(19)|BIT(20)|BIT(21)|BIT(28)|BIT(29)|BIT(30), 0},
    1,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_READ_DIRECT_ID,
    "read_direct",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell read_direct destination_entity_type 2 fip_to_fe1_link_number 1 fe1_to_fe2_link_number 3 fe2_to_fe3_link_number 4 fe3_to_fip_link_number 5 offset 6 \r\n"
    "",
    "",
    {0, 0, BIT(16), 0},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_WRITE_DIRECT_ID,
    "write_direct",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell write_direct destination_entity_type 2 fip_to_fe1_link_number 1 fe1_to_fe2_link_number 3 fe2_to_fe3_link_number 4 fe3_to_fip_link_number 5 offset 6 data_to_write 7\r\n"
    "",
    "",
    {0, 0, BIT(17), 0},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_READ_INDIRECT_ID,
    "read_indirect",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell read_indirect destination_entity_type 2 fip_to_fe1_link_number 0 fe1_to_fe2_link_number 0 fe2_to_fe3_link_number 0 fe3_to_fip_link_number 0 offset 0x8 data_to_write 0x7\r\n"
    "",
    "",
    {0, 0, BIT(18), 0},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_WRITE_INDIRECT_ID,
    "write_indirect",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell write_indirect destination_entity_type 2 fip_to_fe1_link_number 1 fe1_to_fe2_link_number 3 fe2_to_fe3_link_number 4 fe3_to_fip_link_number 5 offset 6 data_to_write 7\r\n"
    "",
    "",
    {0, 0, BIT(19), 0},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_CPU2CPU_ID,
    "cpu_to_cpu_write",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell cpu2cpu destination_entity_type 2 fip_to_fe1_link_number 0 fe1_to_fe2_link_number 0 fe2_to_fe3_link_number 0 fe3_to_fip_link_number 0 offset 0x8 data_to_write 0x7\r\n"
    "",
    "",
    {0, 0, BIT(20), 0},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_CPU2CPU_READ_ID,
    "cpu_to_cpu_read",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell cpu2cpu destination_entity_type 2 fip_to_fe1_link_number 0 fe1_to_fe2_link_number 0 fe2_to_fe3_link_number 0 fe3_to_fip_link_number 0 offset 0x8 data_to_write 0x7\r\n"
    "",
    "",
    {0, 0, BIT(21), 0},
    2,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_READ_DIRECT_SRC_ENTITY_TYPE_ID,
    "source_entity_type",
    (PARAM_VAL_RULES *)&FE600_SAND_DEVICE_ENTITY2_rule[0],
    (sizeof(FE600_SAND_DEVICE_ENTITY2_rule) / sizeof(FE600_SAND_DEVICE_ENTITY2_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell write_direct destination_entity_type 2 fip_to_fe1_link_number 1 fe1_to_fe2_link_number 3 fe2_to_fe3_link_number 4 fe3_to_fip_link_number 5 offset 6 data_to_write 7\r\n"
    "",
    "",
    {0, 0, BIT(16), 0},
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_READ_DIRECT_DEST_ENTITY_TYPE_ID,
    "destination_entity_type",
    (PARAM_VAL_RULES *)&FE600_SAND_DEVICE_ENTITY_rule[0],
    (sizeof(FE600_SAND_DEVICE_ENTITY_rule) / sizeof(FE600_SAND_DEVICE_ENTITY_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell read_direct destination_entity_type 2 fip_to_fe1_link_number 1 fe1_to_fe2_link_number 3 fe2_to_fe3_link_number 4 fe3_to_fip_link_number 5 offset 6 \r\n"
    "",
    "",
    {0, 0, BIT(16), 0},
    4,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_READ_DIRECT_LINK_FIP_TO_FE_ID,
    "fip_fe1_fe2_fe3_output_links",
    (PARAM_VAL_RULES *)&FE600_CELL_LINKS_NUMBER_rule[0],
    (sizeof(FE600_CELL_LINKS_NUMBER_rule) / sizeof(FE600_CELL_LINKS_NUMBER_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell read_direct destination_entity_type 2 fip_to_fe1_link_number 1 fe1_to_fe2_link_number 3 fe2_to_fe3_link_number 4 fe3_to_fip_link_number 5 offset 6 \r\n"
    "",
    "",
    {0, 0, BIT(16), 0},
    5,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_READ_DIRECT_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell read_direct destination_entity_type 2 fip_to_fe1_link_number 1 fe1_to_fe2_link_number 3 fe2_to_fe3_link_number 4 fe3_to_fip_link_number 5 offset 6 \r\n"
    "",
    "",
    {0, 0, BIT(16), 0},
    6,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_WRITE_DIRECT_SRC_ENTITY_TYPE_ID,
    "source_entity_type",
    (PARAM_VAL_RULES *)&FE600_SAND_DEVICE_ENTITY2_rule[0],
    (sizeof(FE600_SAND_DEVICE_ENTITY2_rule) / sizeof(FE600_SAND_DEVICE_ENTITY2_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell write_direct destination_entity_type 2 fip_to_fe1_link_number 1 fe1_to_fe2_link_number 3 fe2_to_fe3_link_number 4 fe3_to_fip_link_number 5 offset 6 data_to_write 7\r\n"
    "",
    "",
    {0, 0, BIT(17), 0},
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_WRITE_DIRECT_DEST_ENTITY_TYPE_ID,
    "destination_entity_type",
    (PARAM_VAL_RULES *)&FE600_SAND_DEVICE_ENTITY_rule[0],
    (sizeof(FE600_SAND_DEVICE_ENTITY_rule) / sizeof(FE600_SAND_DEVICE_ENTITY_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell write_direct destination_entity_type 2 fip_to_fe1_link_number 1 fe1_to_fe2_link_number 3 fe2_to_fe3_link_number 4 fe3_to_fip_link_number 5 offset 6 data_to_write 7\r\n"
    "",
    "",
    {0, 0, BIT(17), 0},
    4,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_WRITE_DIRECT_LINK_FIP_TO_FE_ID,
    "fip_fe1_fe2_fe3_output_links",
    (PARAM_VAL_RULES *)&FE600_CELL_LINKS_NUMBER_rule[0],
    (sizeof(FE600_CELL_LINKS_NUMBER_rule) / sizeof(FE600_CELL_LINKS_NUMBER_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell write_direct destination_entity_type 2 fip_to_fe1_link_number 1 fe1_to_fe2_link_number 3 fe2_to_fe3_link_number 4 fe3_to_fip_link_number 5 offset 6 data_to_write 7\r\n"
    "",
    "",
    {0, 0, BIT(17), 0},
    5,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_WRITE_DIRECT_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell write_direct destination_entity_type 2 fip_to_fe1_link_number 1 fe1_to_fe2_link_number 3 fe2_to_fe3_link_number 4 fe3_to_fip_link_number 5 offset 6 data_to_write 7\r\n"
    "",
    "",
    {0, 0, BIT(17), 0},
    6,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_WRITE_DIRECT_DATA_IN_ID,
    "data_to_write",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell write_direct destination_entity_type 2 fip_to_fe1_link_number 1 fe1_to_fe2_link_number 3 fe2_to_fe3_link_number 4 fe3_to_fip_link_number 5 offset 6 data_to_write 7\r\n"
    "",
    "",
    {0, 0, BIT(17), 0},
    7,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_READ_INDIRECT_SRC_ENTITY_TYPE_ID,
    "source_entity_type",
    (PARAM_VAL_RULES *)&FE600_SAND_DEVICE_ENTITY2_rule[0],
    (sizeof(FE600_SAND_DEVICE_ENTITY2_rule) / sizeof(FE600_SAND_DEVICE_ENTITY2_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell write_direct destination_entity_type 2 fip_to_fe1_link_number 1 fe1_to_fe2_link_number 3 fe2_to_fe3_link_number 4 fe3_to_fip_link_number 5 offset 6 data_to_write 7\r\n"
    "",
    "",
    {0, 0, BIT(18), 0},
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_READ_INDIRECT_DEST_ENTITY_TYPE_ID,
    "destination_entity_type",
    (PARAM_VAL_RULES *)&FE600_SAND_DEVICE_ENTITY_rule[0],
    (sizeof(FE600_SAND_DEVICE_ENTITY_rule) / sizeof(FE600_SAND_DEVICE_ENTITY_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell read_indirect destination_entity_type 2 fip_to_fe1_link_number 0 fe1_to_fe2_link_number 0 fe2_to_fe3_link_number 0 fe3_to_fip_link_number 0 offset 0x8 data_to_write 0x7\r\n"
    "",
    "",
    {0, 0, BIT(18), 0},
    4,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_READ_INDIRECT_LINK_FIP_TO_FE_ID,
      "fip_fe1_fe2_fe3_output_links",
    (PARAM_VAL_RULES *)&FE600_CELL_LINKS_NUMBER_rule[0],
    (sizeof(FE600_CELL_LINKS_NUMBER_rule) / sizeof(FE600_CELL_LINKS_NUMBER_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell read_indirect destination_entity_type 2 fip_to_fe1_link_number 0 fe1_to_fe2_link_number 0 fe2_to_fe3_link_number 0 fe3_to_fip_link_number 0 offset 0x8 data_to_write 0x7\r\n"
    "",
    "",
    {0, 0, BIT(18), 0},
    5,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_READ_INDIRECT_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell read_indirect destination_entity_type 2 fip_to_fe1_link_number 0 fe1_to_fe2_link_number 0 fe2_to_fe3_link_number 0 fe3_to_fip_link_number 0 offset 0x8 data_to_write 0x7\r\n"
    "",
    "",
    {0, 0, BIT(18), 0},
    6,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_WRITE_INDIRECT_SRC_ENTITY_TYPE_ID,
    "source_entity_type",
    (PARAM_VAL_RULES *)&FE600_SAND_DEVICE_ENTITY2_rule[0],
    (sizeof(FE600_SAND_DEVICE_ENTITY2_rule) / sizeof(FE600_SAND_DEVICE_ENTITY2_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell write_direct destination_entity_type 2 fip_to_fe1_link_number 1 fe1_to_fe2_link_number 3 fe2_to_fe3_link_number 4 fe3_to_fip_link_number 5 offset 6 data_to_write 7\r\n"
    "",
    "",
    {0, 0, BIT(19), 0},
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_WRITE_INDIRECT_DEST_ENTITY_TYPE_ID,
    "destination_entity_type",
    (PARAM_VAL_RULES *)&FE600_SAND_DEVICE_ENTITY_rule[0],
    (sizeof(FE600_SAND_DEVICE_ENTITY_rule) / sizeof(FE600_SAND_DEVICE_ENTITY_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell write_indirect destination_entity_type 2 fip_to_fe1_link_number 1 fe1_to_fe2_link_number 3 fe2_to_fe3_link_number 4 fe3_to_fip_link_number 5 offset 6 data_to_write 7\r\n"
    "",
    "",
    {0, 0, BIT(19), 0},
    4,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_WRITE_INDIRECT_LINK_FIP_TO_FE_ID,
    "fip_fe1_fe2_fe3_output_links",
    (PARAM_VAL_RULES *)&FE600_CELL_LINKS_NUMBER_rule[0],
    (sizeof(FE600_CELL_LINKS_NUMBER_rule) / sizeof(FE600_CELL_LINKS_NUMBER_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell write_indirect destination_entity_type 2 fip_to_fe1_link_number 1 fe1_to_fe2_link_number 3 fe2_to_fe3_link_number 4 fe3_to_fip_link_number 5 offset 6 data_to_write 7\r\n"
    "",
    "",
    {0, 0, BIT(19), 0},
    5,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_WRITE_INDIRECT_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell write_indirect destination_entity_type 2 fip_to_fe1_link_number 1 fe1_to_fe2_link_number 3 fe2_to_fe3_link_number 4 fe3_to_fip_link_number 5 offset 6 data_to_write 7\r\n"
    "",
    "",
    {0, 0, BIT(19), 0},
    6,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_WRITE_INDIRECT_DATA_IN_ID,
    "data_to_write",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell write_indirect destination_entity_type 2 fip_to_fe1_link_number 1 fe1_to_fe2_link_number 3 fe2_to_fe3_link_number 4 fe3_to_fip_link_number 5 offset 6 data_to_write 7\r\n"
    "",
    "",
    {0, 0, BIT(19), 0},
    7,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_CPU2CPU_SRC_ENTITY_TYPE_ID,
    "source_entity_type",
    (PARAM_VAL_RULES *)&FE600_SAND_DEVICE_ENTITY2_rule[0],
    (sizeof(FE600_SAND_DEVICE_ENTITY2_rule) / sizeof(FE600_SAND_DEVICE_ENTITY2_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell write_direct destination_entity_type 2 fip_to_fe1_link_number 1 fe1_to_fe2_link_number 3 fe2_to_fe3_link_number 4 fe3_to_fip_link_number 5 offset 6 data_to_write 7\r\n"
    "",
    "",
    {0, 0, BIT(20), 0},
    3,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_CPU2CPU_DEST_ENTITY_TYPE_ID,
    "destination_entity_type",
    (PARAM_VAL_RULES *)&FE600_SAND_DEVICE_ENTITY_rule[0],
    (sizeof(FE600_SAND_DEVICE_ENTITY_rule) / sizeof(FE600_SAND_DEVICE_ENTITY_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell read_indirect destination_entity_type 2 fip_to_fe1_link_number 0 fe1_to_fe2_link_number 0 fe2_to_fe3_link_number 0 fe3_to_fip_link_number 0 offset 0x8 data_to_write 0x7\r\n"
    "",
    "",
    {0, 0, BIT(20), 0},
    4,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_CPU2CPU_LINK_FIP_TO_FE_ID,
    "fip_fe1_fe2_fe3_output_links",
    (PARAM_VAL_RULES *)&FE600_CELL_LINKS_NUMBER_rule[0],
    (sizeof(FE600_CELL_LINKS_NUMBER_rule) / sizeof(FE600_CELL_LINKS_NUMBER_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell read_indirect destination_entity_type 2 fip_to_fe1_link_number 0 fe1_to_fe2_link_number 0 fe2_to_fe3_link_number 0 fe3_to_fip_link_number 0 offset 0x8 data_to_write 0x7\r\n"
    "",
    "",
    {0, 0, BIT(20), 0},
    5,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_CPU2CPU_DATA_IN_ID,
    "data_in",
    (PARAM_VAL_RULES *)&FE600_CELL_CPU2CPU_DATA_IN_rule[0],
    (sizeof(FE600_CELL_CPU2CPU_DATA_IN_rule) / sizeof(FE600_CELL_CPU2CPU_DATA_IN_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "Examples:\r\n"
    "  fe600_api cell read_indirect destination_entity_type 2 fip_to_fe1_link_number 0 fe1_to_fe2_link_number 0 fe2_to_fe3_link_number 0 fe3_to_fip_link_number 0 offset 0x8 data_to_write 0x7\r\n"
    "",
    "",
    {0, 0, BIT(20), 0},
    LAST_ORDINAL,
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_CELL_SNAKE_TEST_INIT_SNAKE_TEST_INIT_ID,
    "snake_test_init",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Initialization function for a diagnostic tool to measure the maximum rate\r\n"
    "  through an internal snake on the SOC_SAND_FE600 links.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api cell snake_test_init is_link_enabled_index 0 is_link_enabled 0\r\n"
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
    PARAM_FE600_CELL_SNAKE_TEST_INIT_SNAKE_TEST_INIT_ENABLED_LINKS_ENABLED_LINK_BITMAP_INDEX_ID,
    "is_link_enabled_index",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  is_link_enabled_index:\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api cell snake_test_init is_link_enabled_index 0 is_link_enabled 0\r\n"
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
    PARAM_FE600_CELL_SNAKE_TEST_INIT_SNAKE_TEST_INIT_ENABLED_LINKS_ENABLED_LINK_BITMAP_ID,
    "is_link_enabled",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  enabled_links.is_link_enabled[ prm_is_link_enabled_index]:\r\n"
    "  The bitmap of the eligible links.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api cell snake_test_init is_link_enabled_index 0 is_link_enabled 0\r\n"
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
    PARAM_FE600_CELL_SNAKE_TEST_INIT_SNAKE_TEST_INIT_IS_LOOPBACK_EXT_ID,
    "is_loopback_ext",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  is_loopback_ext:\r\n"
    "  If True, the loopback on the SerDeses must be external.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api cell snake_test_init is_link_enabled_index 0 is_link_enabled\r\n"
    "  0 is_loopback_ext 0\r\n"
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
    PARAM_FE600_CELL_SNAKE_TEST_RATE_GET_SNAKE_TEST_RATE_GET_ID,
    "snake_test_rate_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Measure the rate after an internal snake on the SOC_SAND_FE600 links.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api cell snake_test_rate_get\r\n"
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
    PARAM_FE600_CELL_SNAKE_TEST_STOP_SNAKE_TEST_STOP_ID,
    "snake_test_stop",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Cancel the configuration set for the rate evaluation diagnostic tool.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api cell snake_test_stop\r\n"
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
#endif/* { cell*/

#ifdef UI_FE600_SERDES_UTILS/* { serdes_utils*/
  {
    PARAM_FE600_SRD_REG_WRITE_SRD_REG_WRITE_ID,
    "reg_write",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Write 8 bit value to the SerDes.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_reg_write entity 0 entity_ndx 0 offset 0 element 0\r\n"
    "  val 0\r\n"
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
    PARAM_FE600_SRD_REG_WRITE_SRD_REG_WRITE_ENTITY_ID,
    "entity",
    (PARAM_VAL_RULES *)&FE600_SRD_ENTITY_TYPE_rule[0],
    (sizeof(FE600_SRD_ENTITY_TYPE_rule) / sizeof(FE600_SRD_ENTITY_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entity:\r\n"
    "  the entity to access.\r\n"
    "  Can be a SerDes lane, a CMU or an IPU.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_reg_write entity 0 entity_ndx 0 offset 0 element 0\r\n"
    "  val 0\r\n"
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
    PARAM_FE600_SRD_REG_WRITE_SRD_REG_WRITE_ENTITY_NDX_ID,
    "entity_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entity_ndx:\r\n"
    "  the entity index.\r\n"
    "  If SerDes Lane - the SerDes lane number.\r\n"
    "  Range: 0-95.\r\n"
    "  If SerDes CMU - the SerDes Quertet index.\r\n"
    "  Range: 0 - 23.\r\n"
    "  If SerDes IPU - the SerDes Star index (refer to FE600_SRD_STAR_ID).\r\n"
    "  Range: 0 - 7.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_reg_write entity 0 entity_ndx 0 offset 0 element 0\r\n"
    "  val 0\r\n"
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
    PARAM_FE600_SRD_REG_WRITE_SRD_REG_WRITE_REG_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  reg.offset:\r\n"
    "  Register Offset. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_reg_write entity 0 entity_ndx 0 offset 0 element 0\r\n"
    "  val 0\r\n"
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
    PARAM_FE600_SRD_REG_WRITE_SRD_REG_WRITE_REG_ELEMENT_ID,
    "element",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  reg.element:\r\n"
    "  Fe600 SerDes Element Type. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_reg_write entity 0 entity_ndx 0 offset 0 element 0\r\n"
    "  val 0\r\n"
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
    PARAM_FE600_SRD_REG_WRITE_SRD_REG_WRITE_VAL_ID,
    "val",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  val:\r\n"
    "  The data to write to the register.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_reg_write entity 0 entity_ndx 0 offset 0 element 0\r\n"
    "  val 0\r\n"
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
    PARAM_FE600_SRD_REG_READ_SRD_REG_READ_ID,
    "reg_read",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Read 8 bit value from the SerDes.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_reg_read entity 0 entity_ndx 0 offset 0 element 0\r\n"
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
    PARAM_FE600_SRD_REG_READ_SRD_REG_READ_ENTITY_ID,
    "entity",
    (PARAM_VAL_RULES *)&FE600_SRD_ENTITY_TYPE_rule[0],
    (sizeof(FE600_SRD_ENTITY_TYPE_rule) / sizeof(FE600_SRD_ENTITY_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entity:\r\n"
    "  the entity to access.\r\n"
    "  Can be a SerDes lane, a CMU or an IPU.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_reg_read entity 0 entity_ndx 0 offset 0 element 0\r\n"
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
    PARAM_FE600_SRD_REG_READ_SRD_REG_READ_ENTITY_NDX_ID,
    "entity_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entity_ndx:\r\n"
    "  the entity index.\r\n"
    "  If SerDes Lane - the SerDes lane number.\r\n"
    "  Range: 0-95.\r\n"
    "  If SerDes CMU - the SerDes Quertet index.\r\n"
    "  Range: 0 - 23.\r\n"
    "  If SerDes IPU - the SerDes Star index (refer to FE600_SRD_STAR_ID).\r\n"
    "  Range: 0 - 7.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_reg_read entity 0 entity_ndx 0 offset 0 element 0\r\n"
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
    PARAM_FE600_SRD_REG_READ_SRD_REG_READ_REG_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  reg.offset:\r\n"
    "  Register Offset. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_reg_read entity 0 entity_ndx 0 offset 0 element 0\r\n"
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
    PARAM_FE600_SRD_REG_READ_SRD_REG_READ_REG_ELEMENT_ID,
    "element",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  reg.element:\r\n"
    "  Fe600 SerDes Element Type. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_reg_read entity 0 entity_ndx 0 offset 0 element 0\r\n"
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
    PARAM_FE600_SRD_FLD_WRITE_SRD_FLD_WRITE_ID,
    "fld_write",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Write a field (up to 8 bit) value to the SerDes.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_fld_write entity 0 entity_ndx 0 lsb 0 msb 0 offset\r\n"
    "  0 element 0 val 0\r\n"
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
    PARAM_FE600_SRD_FLD_WRITE_SRD_FLD_WRITE_ENTITY_ID,
    "entity",
    (PARAM_VAL_RULES *)&FE600_SRD_ENTITY_TYPE_rule[0],
    (sizeof(FE600_SRD_ENTITY_TYPE_rule) / sizeof(FE600_SRD_ENTITY_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entity:\r\n"
    "  the entity to access.\r\n"
    "  Can be a SerDes lane, a CMU or an IPU.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_fld_write entity 0 entity_ndx 0 lsb 0 msb 0 offset\r\n"
    "  0 element 0 val 0\r\n"
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
    PARAM_FE600_SRD_FLD_WRITE_SRD_FLD_WRITE_ENTITY_NDX_ID,
    "entity_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entity_ndx:\r\n"
    "  the entity index.\r\n"
    "  If SerDes Lane - the SerDes lane number.\r\n"
    "  Range: 0-95.\r\n"
    "  If SerDes CMU - the SerDes Quertet index.\r\n"
    "  Range: 0 - 23.\r\n"
    "  If SerDes IPU - the SerDes Star index (refer to FE600_SRD_STAR_ID).\r\n"
    "  Range: 0 - 7.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_fld_write entity 0 entity_ndx 0 lsb 0 msb 0 offset\r\n"
    "  0 element 0 val 0\r\n"
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
    PARAM_FE600_SRD_FLD_WRITE_SRD_FLD_WRITE_FLD_LSB_ID,
    "lsb",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  fld.lsb:\r\n"
    "  Field Least Significant Bit in the register.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_fld_write entity 0 entity_ndx 0 lsb 0 msb 0 offset\r\n"
    "  0 element 0 val 0\r\n"
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
    PARAM_FE600_SRD_FLD_WRITE_SRD_FLD_WRITE_FLD_MSB_ID,
    "msb",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  fld.msb:\r\n"
    "  Field Most Significant Bit in the register.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_fld_write entity 0 entity_ndx 0 lsb 0 msb 0 offset\r\n"
    "  0 element 0 val 0\r\n"
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
    PARAM_FE600_SRD_FLD_WRITE_SRD_FLD_WRITE_FLD_ADDR_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  fld.addr.offset:\r\n"
    "  Register Offset. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_fld_write entity 0 entity_ndx 0 lsb 0 msb 0 offset\r\n"
    "  0 element 0 val 0\r\n"
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
    PARAM_FE600_SRD_FLD_WRITE_SRD_FLD_WRITE_FLD_ADDR_ELEMENT_ID,
    "element",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  fld.addr.element:\r\n"
    "  Fe600 SerDes Element Type. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_fld_write entity 0 entity_ndx 0 lsb 0 msb 0 offset\r\n"
    "  0 element 0 val 0\r\n"
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
    PARAM_FE600_SRD_FLD_WRITE_SRD_FLD_WRITE_VAL_ID,
    "val",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  val:\r\n"
    "  The data to write to the field.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_fld_write entity 0 entity_ndx 0 lsb 0 msb 0 offset\r\n"
    "  0 element 0 val 0\r\n"
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
    PARAM_FE600_SRD_FLD_READ_SRD_FLD_READ_ID,
    "fld_read",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Read 8 bit value from the SerDes.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_fld_read entity 0 entity_ndx 0 lsb 0 msb 0 offset\r\n"
    "  0 element 0\r\n"
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
    PARAM_FE600_SRD_FLD_READ_SRD_FLD_READ_ENTITY_ID,
    "entity",
    (PARAM_VAL_RULES *)&FE600_SRD_ENTITY_TYPE_rule[0],
    (sizeof(FE600_SRD_ENTITY_TYPE_rule) / sizeof(FE600_SRD_ENTITY_TYPE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entity:\r\n"
    "  the entity to access.\r\n"
    "  Can be a SerDes lane, a CMU or an IPU.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_fld_read entity 0 entity_ndx 0 lsb 0 msb 0 offset\r\n"
    "  0 element 0\r\n"
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
    PARAM_FE600_SRD_FLD_READ_SRD_FLD_READ_ENTITY_NDX_ID,
    "entity_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  entity_ndx:\r\n"
    "  the entity index.\r\n"
    "  If SerDes Lane - the SerDes lane number.\r\n"
    "  Range: 0-95.\r\n"
    "  If SerDes CMU - the SerDes Quertet index.\r\n"
    "  Range: 0 - 23.\r\n"
    "  If SerDes IPU - the SerDes Star index (refer to FE600_SRD_STAR_ID).\r\n"
    "  Range: 0 - 7.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_fld_read entity 0 entity_ndx 0 lsb 0 msb 0 offset\r\n"
    "  0 element 0\r\n"
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
    PARAM_FE600_SRD_FLD_READ_SRD_FLD_READ_FLD_LSB_ID,
    "lsb",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  fld.lsb:\r\n"
    "  Field Least Significant Bit in the register.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_fld_read entity 0 entity_ndx 0 lsb 0 msb 0 offset\r\n"
    "  0 element 0\r\n"
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
    PARAM_FE600_SRD_FLD_READ_SRD_FLD_READ_FLD_MSB_ID,
    "msb",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  fld.msb:\r\n"
    "  Field Most Significant Bit in the register.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_fld_read entity 0 entity_ndx 0 lsb 0 msb 0 offset\r\n"
    "  0 element 0\r\n"
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
    PARAM_FE600_SRD_FLD_READ_SRD_FLD_READ_FLD_ADDR_OFFSET_ID,
    "offset",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  fld.addr.offset:\r\n"
    "  Register Offset. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_fld_read entity 0 entity_ndx 0 lsb 0 msb 0 offset\r\n"
    "  0 element 0\r\n"
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
    PARAM_FE600_SRD_FLD_READ_SRD_FLD_READ_FLD_ADDR_ELEMENT_ID,
    "element",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  fld.addr.element:\r\n"
    "  Fe600 SerDes Element Type. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_fld_read entity 0 entity_ndx 0 lsb 0 msb 0 offset\r\n"
    "  0 element 0\r\n"
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
    PARAM_FE600_SRD_LINK_RX_EYE_MONITOR_SRD_LINK_RX_EYE_MONITOR_ID,
    "link_rx_eye_monitor",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function sets the equalizer mode and retrieves the eye-sample.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_link_rx_eye_monitor lane_ndx 0 equalizer_mode 0\r\n"
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
    PARAM_FE600_SRD_LINK_RX_EYE_MONITOR_SRD_LINK_RX_EYE_MONITOR_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&FE600_LANE_NDX_rule[0],
    (sizeof(FE600_LANE_NDX_rule) / sizeof(FE600_LANE_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_ndx:(96 means all the lanes)\r\n"
    "  The SerDes number of which we want to set.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_link_rx_eye_monitor lane_ndx 0 equalizer_mode 0\r\n"
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
    PARAM_FE600_SRD_LINK_RX_EYE_MONITOR_SRD_LINK_RX_EYE_MONITOR_EQUALIZER_MODE_ID,
    "equalizer_mode",
    (PARAM_VAL_RULES *)&FE600_SRD_LANE_EQ_MODE_rule[0],
    (sizeof(FE600_SRD_LANE_EQ_MODE_rule) / sizeof(FE600_SRD_LANE_EQ_MODE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  equalizer_mode:\r\n"
    "  This mode indicates whether and how to run an automatic algorithm to read the\r\n"
    "  Eye-Sample. (modes: off/normal/preset).\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_link_rx_eye_monitor lane_ndx 0 equalizer_mode 0\r\n"
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
    PARAM_FE600_SRD_LANE_LOOPBACK_MODE_SET_SRD_LANE_LOOPBACK_MODE_SET_ID,
    "lane_loopback_mode_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function sets the loopback mode of a specified lane.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_lane_loopback_mode_set lane_ndx 0 loopback_mode 0\r\n"
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
    PARAM_FE600_SRD_LANE_LOOPBACK_MODE_SET_SRD_LANE_LOOPBACK_MODE_SET_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&FE600_LANE_NDX_rule[0],
    (sizeof(FE600_LANE_NDX_rule) / sizeof(FE600_LANE_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_ndx:(96 means all the lanes)\r\n"
    "  The SerDes lane number of which we want to set.(0..95).\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_lane_loopback_mode_set lane_ndx 0 loopback_mode 0\r\n"
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
    PARAM_FE600_SRD_LANE_LOOPBACK_MODE_SET_SRD_LANE_LOOPBACK_MODE_SET_LOOPBACK_MODE_ID,
    "loopback_mode",
    (PARAM_VAL_RULES *)&FE600_SRD_LANE_LOOPBACK_MODE_rule[0],
    (sizeof(FE600_SRD_LANE_LOOPBACK_MODE_rule) / sizeof(FE600_SRD_LANE_LOOPBACK_MODE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  loopback_mode:\r\n"
    "  There are 5 possible loopback modes.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_lane_loopback_mode_set lane_ndx 0 loopback_mode 0\r\n"
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
    PARAM_FE600_SRD_LANE_LOOPBACK_MODE_GET_SRD_LANE_LOOPBACK_MODE_GET_ID,
    "lane_loopback_mode_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function sets the loopback mode of a specified lane.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_lane_loopback_mode_get lane_ndx 0\r\n"
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
    PARAM_FE600_SRD_LANE_LOOPBACK_MODE_GET_SRD_LANE_LOOPBACK_MODE_GET_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&FE600_LANE_NDX_rule[0],
    (sizeof(FE600_LANE_NDX_rule) / sizeof(FE600_LANE_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_ndx:(96 means all the lanes)\r\n"
    "  The SerDes lane number of which we want to set.(0..95).\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_lane_loopback_mode_get lane_ndx 0\r\n"
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
    PARAM_FE600_SRD_PRBS_MODE_SET_SRD_PRBS_MODE_SET_ID,
    "prbs_mode_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Sets the PRBS pattern.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_prbs_mode_set lane_ndx 0 direction_ndx 0 mode 0\r\n"
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
    PARAM_FE600_SRD_PRBS_MODE_SET_SRD_PRBS_MODE_SET_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&FE600_LANE_NDX_rule[0],
    (sizeof(FE600_LANE_NDX_rule) / sizeof(FE600_LANE_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_ndx:(96 means all the lanes)\r\n"
    "  SerDes lane index. Range: 0-95.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_prbs_mode_set lane_ndx 0 direction_ndx 0 mode 0\r\n"
    "",
    "",
    {0, 0, BIT(7), 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_SRD_PRBS_MODE_SET_SRD_PRBS_MODE_SET_DIRECTION_NDX_ID,
    "direction_ndx",
    (PARAM_VAL_RULES *)&FE600_CONNECTION_DIRECTION_rule[0],
    (sizeof(FE600_CONNECTION_DIRECTION_rule) / sizeof(FE600_CONNECTION_DIRECTION_rule[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  direction_ndx (0:Rx, 1:Tx, 2 Both):\r\n"
    "  PRBS direction: TX - for PRBS generation, RX - for PRBS reception, or BOTH.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_prbs_mode_set lane_ndx 0 direction_ndx 0 mode 0\r\n"
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
    PARAM_FE600_SRD_PRBS_MODE_SET_SRD_PRBS_MODE_SET_MODE_ID,
    "mode",
    (PARAM_VAL_RULES *)&FE600_SRD_PRBS_MODE_rule[0],
    (sizeof(FE600_SRD_PRBS_MODE_rule) / sizeof(FE600_SRD_PRBS_MODE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mode:\r\n"
    "  SerDes PRBS pattern mode.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_prbs_mode_set lane_ndx 0 direction_ndx 0 mode 0\r\n"
    "",
    "",
    {0, 0, BIT(7), 0, 0, 0, 0, 0,0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_SRD_PRBS_MODE_GET_SRD_PRBS_MODE_GET_ID,
    "prbs_mode_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Sets the PRBS pattern.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_prbs_mode_get lane_ndx 0 direction_ndx 0\r\n"
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
    PARAM_FE600_SRD_PRBS_MODE_GET_SRD_PRBS_MODE_GET_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&FE600_LANE_NDX_rule[0],
    (sizeof(FE600_LANE_NDX_rule) / sizeof(FE600_LANE_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_ndx:(96 means all the lanes)\r\n"
    "  SerDes lane index. Range: 0-95.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_prbs_mode_get lane_ndx 0 direction_ndx 0\r\n"
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
    PARAM_FE600_SRD_PRBS_START_SRD_PRBS_START_ID,
    "prbs_start",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Start PRBS - generation (TX), reception (RX) or both.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_prbs_start lane_ndx 0 direction_ndx 0\r\n"
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
    PARAM_FE600_SRD_PRBS_START_SRD_PRBS_START_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&FE600_LANE_NDX_rule[0],
    (sizeof(FE600_LANE_NDX_rule) / sizeof(FE600_LANE_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_ndx:(96 means all the lanes)\r\n"
    "  SerDes lane index. Range: 0-95.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_prbs_start lane_ndx 0 direction_ndx 0\r\n"
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
    PARAM_FE600_SRD_PRBS_START_SRD_PRBS_START_DIRECTION_NDX_ID,
    "direction_ndx",
    (PARAM_VAL_RULES *)&FE600_CONNECTION_DIRECTION_rule[0],
    (sizeof(FE600_CONNECTION_DIRECTION_rule) / sizeof(FE600_CONNECTION_DIRECTION_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  direction_ndx (0:Rx, 1:Tx, 2 Both):\r\n"
    "  PRBS direction: TX - for PRBS generation, RX - for PRBS reception, or BOTH. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_prbs_start lane_ndx 0 direction_ndx 0\r\n"
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
    PARAM_FE600_SRD_PRBS_STOP_SRD_PRBS_STOP_ID,
    "prbs_stop",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Stop PRBS - generation (TX), reception (RX) or both.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_prbs_stop lane_ndx 0 direction_ndx 0\r\n"
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
    PARAM_FE600_SRD_PRBS_STOP_SRD_PRBS_STOP_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&FE600_LANE_NDX_rule[0],
    (sizeof(FE600_LANE_NDX_rule) / sizeof(FE600_LANE_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_ndx:(96 means all the lanes)\r\n"
    "  SerDes lane index. Range: 0-95.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_prbs_stop lane_ndx 0 direction_ndx 0\r\n"
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
    PARAM_FE600_SRD_PRBS_STOP_SRD_PRBS_STOP_DIRECTION_NDX_ID,
    "direction_ndx",
    (PARAM_VAL_RULES *)&FE600_CONNECTION_DIRECTION_rule[0],
    (sizeof(FE600_CONNECTION_DIRECTION_rule) / sizeof(FE600_CONNECTION_DIRECTION_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  direction_ndx (0:Rx, 1:Tx, 2 Both):\r\n"
    "  PRBS direction: TX - for PRBS generation, RX - for PRBS reception, or BOTH. \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_prbs_stop lane_ndx 0 direction_ndx 0\r\n"
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
    PARAM_FE600_SRD_PRBS_GET_AND_CLEAR_STAT_SRD_PRBS_GET_AND_CLEAR_STAT_ID,
    "prbs_get_and_clear_stat",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Read the status on the receiver side. All indications are cleared on read.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_prbs_get_and_clear_stat lane_ndx 0\r\n"
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
    PARAM_FE600_SRD_PRBS_GET_AND_CLEAR_STAT_SRD_PRBS_GET_AND_CLEAR_STAT_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&FE600_LANE_NDX_rule[0],
    (sizeof(FE600_LANE_NDX_rule) / sizeof(FE600_LANE_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_ndx:(96 means all the lanes)\r\n"
    "  SerDes lane index. Range: 0-95.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_prbs_get_and_clear_stat lane_ndx 0\r\n"
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
    PARAM_FE600_SRD_LANE_STATUS_GET_SRD_LANE_STATUS_GET_ID,
    "lane_status_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  This function is used as a diagnostics tool per lane that gives an indication\r\n"
    "  about the lane and CMUs status. It indicates per lane - loss of signal,\r\n"
    "  frequency lock is lost. And per CMU - PLL lock is lost.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_lane_status_get lane_ndx 0\r\n"
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
    PARAM_FE600_SRD_LANE_STATUS_GET_SRD_LANE_STATUS_GET_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&FE600_LANE_NDX_rule[0],
    (sizeof(FE600_LANE_NDX_rule) / sizeof(FE600_LANE_NDX_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_ndx:(96 means all the lanes)\r\n"
    "  SOC_SAND_IN\r\n"
    "  uint32\r\n"
    "  lane_ndx\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_lane_status_get lane_ndx 0\r\n"
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
    PARAM_FE600_SRD_QRTT_STATUS_GET_SRD_QRTT_STATUS_GET_ID,
    "qrtt_status_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Prints SerDes diagnostics, per SerDes quartet \r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_qrtt_status_get qrtt_id 0\r\n"
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
    PARAM_FE600_SRD_QRTT_STATUS_GET_SRD_QRTT_STATUS_GET_QRTT_ID_ID,
    "qrtt_id",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  qrtt_id:\r\n"
    "  quartet index. Range: 0 - 15.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_qrtt_status_get qrtt_id 0\r\n"
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
    PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_ID,
    "eye_scan_run",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Runs the eye scan, in order to map between serdes rx parameters\r\n"
    "  and amount of crc errors.\r\n"
    "  Eye scan sequence is as following:\r\n"
    "  For each tlth * dfelth pair, run prbs for a given time, and collect\r\n"
    "  results to a matrix, per lane. The matrix is used to find optimal\r\n"
    "  rx parameters. \r\n"
    "",
#if UI_FE600_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_eye_scan_run resolution 4 duration_min_sec 1 use_link_crc_no_prbs 0\r\n"
    "  lane_ndx 0"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(0), 0, 0},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
 {
     PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_TRAFFIC_SRC_ID,
      "traffic_src",
      (PARAM_VAL_RULES *)&FE600_FE600_SRD_TRAFFIC_SRC_rule[0],
    (sizeof(FE600_FE600_SRD_TRAFFIC_SRC_rule) / sizeof(FE600_FE600_SRD_TRAFFIC_SRC_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.traffic source:\r\n"
    "  Traffic source is optionally prbs or snake.\r\n"
    "",
#if UI_FE600_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_eye_scan_run resolution 0 duration_min_sec 0\r\n"
    "  lane_ndx 0"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(0), 0, 0},
    3,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_RESOLUTION_ID,
    "resolution",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.resolution:\r\n"
    "  Scan density (e.g value of 2 scans every 2nd point in matrix). Range: 1 - 10.\r\n"
    "",
#if UI_FE600_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_eye_scan_run resolution 0 duration_min_sec 0\r\n"
    "  lane_ndx 0"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(0), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
   {
     PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_CRC_ID,
      "counting_src",
      (PARAM_VAL_RULES *)&FE600_FE600_SRD_CNT_SRC_rule[0],
    (sizeof(FE600_FE600_SRD_CNT_SRC_rule) / sizeof(FE600_FE600_SRD_CNT_SRC_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.counting source:\r\n"
    "  Counting source if not PRBS.\r\n"
    "",
#if UI_FE600_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_eye_scan_run resolution 0 duration_min_sec 0\r\n"
    "  lane_ndx 0"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(0), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_PRBS_DURATION_MIN_SEC_ID,
    "duration_min_sec",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.duration_min_sec:\r\n"
    "  Minimal delay to run PRBS on each point of scan. Range: 100 - 60000.\r\n"
    "",
#if UI_FE600_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_eye_scan_run resolution 4 duration_min_sec 1 use_link_crc_no_prbs 0\r\n"
    "  lane_ndx 0"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(0), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&fe600_api_serdes_vals[0],
    (sizeof(fe600_api_serdes_vals) / sizeof(fe600_api_serdes_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.lane_ndx[ prm_lane_ndx_index]:\r\n"
    "  Array of lane indices to run eye scan on.\r\n"
    "",
#if UI_FE600_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_eye_scan_run resolution 4 duration_min_sec 1 use_link_crc_no_prbs 0\r\n"
    "  lane_ndx 0"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(0), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_MODE_ID,
    "prbs_mode",
    (PARAM_VAL_RULES *)&FE600_SRD_PRBS_MODE_rule[0],
    (sizeof(FE600_SRD_PRBS_MODE_rule) / sizeof(FE600_SRD_PRBS_MODE_rule[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  mode:\r\n"
    "  SerDes PRBS pattern mode.\r\n"
    "",
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_prbs_mode_set lane_ndx 0 direction_ndx 0 mode 0\r\n"
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(0),0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_TX_PRBS_ID,
      "prbs_is_tx_enabled",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  info.is_tx_prbs_enabled:\r\n"
    "  Range: 0 - 1.\r\n"
    "",
#if UI_FE600_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_eye_scan_run resolution 0 duration_min_sec 0\r\n"
    "  lane_ndx 0"
#endif
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(0), 0, 0},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  { 
    PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_RANGE_DFELTH_MAX_ID, 
    "dfelth_max", 
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0], 
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.range.dfelth_max:\n\r"
    "  Value of parameter in optimum point. Range: 0 - 63.\n\r"
    "",
#if UI_FE600_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  fe600_api serdes_utils srd_eye_scan_run use_link_crc_no_prbs 0 dfelth_max 0\n\r"
    "  dfelth_min 0 tlth_max 0 tlth_min 0 prbs_mode 0\n\r"
#endif 
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(0), 0, 0},
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_RANGE_DFELTH_MIN_ID, 
    "dfelth_min", 
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0], 
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.range.dfelth_min:\n\r"
    "  Value of parameter in optimum point. Range: 0 - 63.\n\r"
    "",
#if UI_FE600_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  fe600_api serdes_utils srd_eye_scan_run use_link_crc_no_prbs 0 dfelth_max 0\n\r"
    "  dfelth_min 0 tlth_max 0 tlth_min 0 prbs_mode 0\n\r"
#endif 
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(0), 0, 0},
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_RANGE_TLTH_MAX_ID, 
    "tlth_max", 
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0], 
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.range.tlth_max:\n\r"
    "  Value of parameter in optimum point. Range: 0 - 48.\n\r"
    "",
#if UI_FE600_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  fe600_api serdes_utils srd_eye_scan_run use_link_crc_no_prbs 0 dfelth_max 0\n\r"
    "  dfelth_min 0 tlth_max 0 tlth_min 0 prbs_mode 0\n\r"
#endif 
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(0), 0, 0},
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  { 
    PARAM_FE600_SRD_EYE_SCAN_RUN_SRD_EYE_SCAN_RUN_INFO_RANGE_TLTH_MIN_ID, 
    "tlth_min", 
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0], 
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1, 
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0, 
    0,0,0,0,0,0,0, 
    "  info.range.tlth_min:\n\r"
    "  Value of parameter in optimum point. Range: 0 - 48.\n\r"
    "",
#if UI_FE600_API_ADD_EXAMPLE 
    "Examples:\n\r" 
    "  fe600_api serdes_utils srd_eye_scan_run use_link_crc_no_prbs 0 dfelth_max 0\n\r"
    "  dfelth_min 0 tlth_max 0 tlth_min 0 prbs_mode 0\n\r"
#endif 
    "",
    "",
    {0, 0, 0, 0, 0, 0, 0, BIT(0), 0, 0},
    LAST_ORDINAL, 
      /* 
       * Pointer to a function to call after symbolic parameter 
       * has been accepted. 
       */ 
    (VAL_PROC_PTR)NULL 
  },  
  {
    PARAM_FE600_SRD_SCIF_ENABLE_SET_SRD_SCIF_ENABLE_SET_ID,
    "scif_enable_set",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Enables/disables the SCIF interface.\r\n"
    "  The SCIF is a utility serial interface that can be used to access the SerDes\r\n"
    "  for debug configuration and diagnostics. It is not required for normal\r\n"
    "  operation, but may be used to access the SerDes by external tools.\r\n"
    "",
#ifdef UI_FE600_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_scif_enable_set is_enabled 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(26)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_SRD_SCIF_ENABLE_SET_SRD_SCIF_ENABLE_SET_IS_ENABLED_ID,
    "is_enabled",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  is_enabled:\r\n"
    "  If TRUE, the SCIF interface is enabled for use.\r\n"
    "",
#ifdef UI_FE600_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_scif_enable_set is_enabled 0\r\n"
#endif
    "",
    "",
    {0, 0, BIT(26)},
    LAST_ORDINAL,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_SRD_SCIF_ENABLE_GET_SRD_SCIF_ENABLE_GET_ID,
    "scif_enable_get",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Enables/disables the SCIF interface.\r\n"
    "  The SCIF is a utility serial interface that can be used to access the SerDes\r\n"
    "  for debug configuration and diagnostics. It is not required for normal\r\n"
    "  operation, but may be used to access the SerDes by external tools.\r\n"
    "",
#ifdef UI_FE600_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_scif_enable_get\r\n"
#endif
    "",
    "",
    {0, 0, BIT(27)},
    2,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
  {
    PARAM_FE600_SRD_RELOCK_SRD_RELOCK_ID,
    "relock",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  Performs SerDes CDR relock\r\n"
    "",
#if UI_FE600_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_relock lane_ndx 0\r\n"
#endif
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
    PARAM_FE600_SRD_RELOCK_SRD_RELOCK_LANE_NDX_ID,
    "lane_ndx",
    (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
    (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
    HAS_NO_DEFAULT | MAY_NOT_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "  lane_ndx:\r\n"
    "  SerDes lane index. Range: 0 - 59. \r\n"
    "",
#if UI_FE600_API_ADD_EXAMPLE
    "Examples:\r\n"
    "  fe600_api serdes_utils srd_relock lane_ndx 0\r\n"
#endif
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
    PARAM_FE600_API_SERDES_UTILS_ID,
    "serdes_utils",
    (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
    (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
    HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
    0,0,0,0,0,0,0,
    "",
    "",
    "",
    {BIT(0), 0, BIT(0)|BIT(1)|BIT(2)|BIT(3)|BIT(4)|BIT(5)|BIT(6)|BIT(7)|BIT(8)|BIT(9)|BIT(10)|BIT(11)|BIT(12)|BIT(13)|BIT(26)|BIT(27), 0,0,0,0,BIT(0),0},
    1,
      /*
       * Pointer to a function to call after symbolic parameter
       * has been accepted.
       */
    (VAL_PROC_PTR)NULL
  },
#endif /* } serdes_utils*/
#ifdef UI_FE600_UTILS
    {
      PARAM_FE600_API_UTILS_ID,
        "utils",
        (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
        (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
        HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
        0,0,0,0,0,0,0,
        "",
        "",
        "",
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, BIT(18) | BIT(19)},
      1,
      /*
      * Pointer to a function to call after symbolic parameter
      * has been accepted.
      */
      (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FE600_UTILS_SET_DEFAULT_DEVICE_ID,
      "set_unit",
      (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
      (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
      HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
      0,0,0,0,0,0,0,
      "  Sets device id to be used by all API-s \r\n"
      "",
#ifdef UI_FE600_API_ADD_EXAMPLE
      "Examples:\r\n"
      "  fe600_api utils set_unit id 0\r\n"
#endif
      "",
      "",
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, BIT(18)},
    2,
    /*
    * Pointer to a function to call after symbolic parameter
    * has been accepted.
    */
    (VAL_PROC_PTR)NULL
    },
    {
      PARAM_FE600_UTILS_GET_DEFAULT_DEVICE_ID,
        "get_unit",
        (PARAM_VAL_RULES *)&fe600_api_empty_vals[0],
        (sizeof(fe600_api_empty_vals) / sizeof(fe600_api_empty_vals[0])) - 1,
        HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
        0,0,0,0,0,0,0,
        "  Gets device id used by all API-s \r\n"
        "",
#ifdef UI_FE600_API_ADD_EXAMPLE
        "Examples:\r\n"
        "  FE600_api utils get_unit\r\n"
#endif
        "",
        "",
      {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, BIT(19)},
      2,
      /*
      * Pointer to a function to call after symbolic parameter
      * has been accepted.
      */
      (VAL_PROC_PTR)NULL
      },
      {
        PARAM_FE600_UTILS_SET_DEFAULT_DEVICE_ID_DEVICE_ID_ID,
          "id",
          (PARAM_VAL_RULES *)&fe600_api_free_vals[0],
          (sizeof(fe600_api_free_vals) / sizeof(fe600_api_free_vals[0])) - 1,
          HAS_NO_DEFAULT | MUST_APPEAR,(PARAM_VAL *)0,
          0,0,0,0,0,0,0,
          "  Device id. Range: 0 - 2047 \r\n"
          "",
#ifdef UI_FE600_API_ADD_EXAMPLE
          "Examples:\r\n"
          "  FE600_api utils set_unit id 0\r\n"
#endif
          "",
          "",
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, BIT(18)},
        LAST_ORDINAL,
        /*
        * Pointer to a function to call after symbolic parameter
        * has been accepted.
        */
        (VAL_PROC_PTR)NULL
      },
#endif
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

/* } __UI_ROM_DEFI_FE600_API_INCLUDED__*/
#endif
