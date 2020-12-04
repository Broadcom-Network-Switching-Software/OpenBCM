 /*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        field_reg_mem.c
 * Purpose:     Field Processor module routines specific to BCM56960 registers
 *              and memories
 */

#include <shared/bsl.h>
#include <soc/defs.h>
#if defined(BCM_FIELD_SUPPORT)
#include <soc/drv.h>
#include <bcm/error.h>
#include <bcm/field.h>
#include <bcm_int/esw/field.h>

/*
 * Function: _bcm_field_reg_instance_get
 *
 * Purpose:
 *     To get the hardware register for the pipe instance mentioned.
 *
 * Parameters:
 *     unit - (IN)BCM device number
 *     reg  - (IN)Register name
 *     inst - (IN)Pipe instance
 *     reg_out - (OUT)Register instance
 *
 * Returns:
 *     BCM_E_XXX
 *
 * Notes:
 * This function takes the register name and pipe number as input and 
 * converts to the right register instance for the pipe given.
 * Number of instances depends on number of pipes supported on the device.
 * For any register access for a pipe we need to call this way of getting the 
 * register instance and add required case in this function.
 */

int
_bcm_field_reg_instance_get(int unit, soc_reg_t reg, int inst, soc_reg_t *reg_out)
{

   switch(reg) {

       case EFP_CLASSID_B_SELECTOR_KEY_TYPEr:
            _FP_2PIPE_REG_IDENTIFIER_GET(*reg_out, EFP_CLASSID_B_SELECTOR_KEY_TYPE,
                                         PIPE, inst);
            break;
       case VFP_KEY_CONTROL_1r:
            _FP_8PIPE_REG_IDENTIFIER_GET(*reg_out, VFP_KEY_CONTROL_1, PIPE, inst);
            break;
       case VFP_KEY_CONTROL_2r:
            _FP_8PIPE_REG_IDENTIFIER_GET(*reg_out, VFP_KEY_CONTROL_2, PIPE, inst);
            break;
       case EFP_CLASSID_SELECTORr:
            _FP_8PIPE_REG_IDENTIFIER_GET(*reg_out, EFP_CLASSID_SELECTOR, PIPE, inst);
            break;
       case EFP_CLASSID_SELECTOR_Ar:
            _FP_2PIPE_REG_IDENTIFIER_GET(*reg_out, EFP_CLASSID_SELECTOR_A,
                                         PIPE, inst);
            break;
       case EFP_KEY4_DVP_SELECTORr:
            _FP_4PIPE_REG_IDENTIFIER_GET(*reg_out, EFP_KEY4_DVP_SELECTOR,
                                         PIPE, inst);
            break;
       case EFP_KEY4_MDL_SELECTORr:
            _FP_8PIPE_REG_IDENTIFIER_GET(*reg_out, EFP_KEY4_MDL_SELECTOR,
                                    PIPE, inst);
            break;
       case EFP_KEY8_DVP_SELECTORr:
            _FP_4PIPE_REG_IDENTIFIER_GET(*reg_out, EFP_KEY8_DVP_SELECTOR,
                                         PIPE, inst);
            break;
       case VFP_SLICE_MAPr:
            _FP_8PIPE_REG_IDENTIFIER_GET(*reg_out, VFP_SLICE_MAP, PIPE, inst);
            break;
       case EFP_SLICE_MAPr:
            _FP_8PIPE_REG_IDENTIFIER_GET(*reg_out, EFP_SLICE_MAP, PIPE, inst);
            break;
       case EFP_SLICE_CONTROLr:
            _FP_8PIPE_REG_IDENTIFIER_GET(*reg_out, EFP_SLICE_CONTROL, PIPE, inst);
            break;
       case VFP_SLICE_CONTROLr:
            _FP_8PIPE_REG_IDENTIFIER_GET(*reg_out, VFP_SLICE_CONTROL, PIPE, inst);
            break;
       case IFP_CONFIGr:
            _FP_8PIPE_REG_IDENTIFIER_GET(*reg_out, IFP_CONFIG, PIPE, inst);
            break;
       case AUX_ARB_CONTROLr:
            _FP_8PIPE_REG_IDENTIFIER_GET(*reg_out, AUX_ARB_CONTROL, PIPE, inst);
            break;
       case IFP_LOGICAL_TABLE_SELECT_CONFIGr:
            _FP_8PIPE_REG_IDENTIFIER_GET(*reg_out, IFP_LOGICAL_TABLE_SELECT_CONFIG,
                                         PIPE, inst);
            break;
       case EXACT_MATCH_LOGICAL_TABLE_SELECT_CONFIGr:
            _FP_8PIPE_REG_IDENTIFIER_GET(*reg_out, EXACT_MATCH_LOGICAL_TABLE_SELECT_CONFIG,
                                         PIPE, inst);
            break;
       case IFP_LOGICAL_TABLE_CONFIGr:
            _FP_8PIPE_REG_IDENTIFIER_GET(*reg_out, IFP_LOGICAL_TABLE_CONFIG,
                                         PIPE, inst);
            break;
       case L4_SRC_PORT_MAPr:
            _FP_4PIPE_REG_IDENTIFIER_GET(*reg_out, L4_SRC_PORT_MAP,
                                         PIPE, inst);
            break;
       case ETHERTYPE_MAPr:
            _FP_8PIPE_REG_IDENTIFIER_GET(*reg_out, ETHERTYPE_MAP,
                                         PIPE, inst);
            break;
       case L4_DST_PORT_MAPr:
            _FP_4PIPE_REG_IDENTIFIER_GET(*reg_out, L4_DST_PORT_MAP,
                                         PIPE, inst);
            break;
       case EFP_DEST_PORT_SELECTORr:
            _FP_2PIPE_REG_IDENTIFIER_GET(*reg_out, EFP_DEST_PORT_SELECTOR,
                                         PIPE, inst);
            break;
       case EFP_KEY4_L3_CLASSID_SELECTORr:
            _FP_8PIPE_REG_IDENTIFIER_GET(*reg_out, EFP_KEY4_L3_CLASSID_SELECTOR,
                                         PIPE, inst);
            break;
       case EFP_KEY8_L3_CLASSID_SELECTORr:
            _FP_8PIPE_REG_IDENTIFIER_GET(*reg_out, EFP_KEY8_L3_CLASSID_SELECTOR,
                                         PIPE, inst);
            break;
       default:
            return BCM_E_INTERNAL;
   }
   return BCM_E_NONE;
}

/*
 * Function: _bcm_field_mem_instance_get
 *
 * Purpose:
 *     To get the hardware memory for the pipe instance mentioned.
 *
 * Parameters:
 *     unit - (IN)BCM device number
 *     mem  - (IN)Memory name
 *     inst - (IN)Pipe instance
 *     mem_out - (OUT)Memory instance
 *
 * Returns:
 *     BCM_E_XXX
 *
 * Notes:
 * This function takes the memory name and pipe number as input and 
 * converts to the right memory instance for the pipe given.
 * Number of instances depends on number of pipes supported on the device.
 * For any memory access for a pipe we need to call this way of getting the 
 * memory instance and add required case in this function.
 */

int
_bcm_field_mem_instance_get(int unit, soc_mem_t mem, int inst, soc_mem_t *mem_out)
{
   switch(mem) {
       case VFP_TCAMm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, VFP_TCAM, PIPE, inst);
            break;
       case VFP_POLICY_TABLEm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, VFP_POLICY_TABLE, PIPE, inst);
            break;
       case EFP_TCAMm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, EFP_TCAM, PIPE, inst);
            break;
       case EFP_POLICY_TABLEm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, EFP_POLICY_TABLE, PIPE, inst);
            break;
       case IFP_TCAM_WIDEm:
            _FP_4PIPE_MEM_IDENTIFIER_GET(*mem_out, IFP_TCAM_WIDE, PIPE, inst);
            break;
       case IFP_TCAMm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, IFP_TCAM, PIPE, inst);
            break;
       case IFP_POLICY_TABLE_WIDEm:
            _FP_2PIPE_MEM_IDENTIFIER_GET(*mem_out, IFP_POLICY_TABLE_WIDE, PIPE, inst);
            break;
       case IFP_POLICY_TABLEm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, IFP_POLICY_TABLE, PIPE, inst);
            break;
       case EXACT_MATCH_4m:
            _FP_4PIPE_MEM_IDENTIFIER_GET(*mem_out, EXACT_MATCH_4, PIPE, inst);
            break;
       case EXACT_MATCH_2m:
            _FP_4PIPE_MEM_IDENTIFIER_GET(*mem_out, EXACT_MATCH_2, PIPE, inst);
            break;
       case IFP_KEY_GEN_PROGRAM_PROFILEm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, IFP_KEY_GEN_PROGRAM_PROFILE, PIPE, inst);
            break;
       case IFP_KEY_GEN_PROGRAM_PROFILE2m:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, IFP_KEY_GEN_PROGRAM_PROFILE2, PIPE, inst);
            break;
       case EXACT_MATCH_KEY_GEN_PROGRAM_PROFILEm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, EXACT_MATCH_KEY_GEN_PROGRAM_PROFILE, PIPE,
                                         inst);
            break;
       case EXACT_MATCH_KEY_GEN_MASKm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, EXACT_MATCH_KEY_GEN_MASK, PIPE, inst);
            break;
       case IFP_RANGE_CHECKm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, IFP_RANGE_CHECK, PIPE, inst);
            break;
       case IFP_LOGICAL_TABLE_SELECTm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, IFP_LOGICAL_TABLE_SELECT, PIPE, inst);
            break;
       case IFP_LOGICAL_TABLE_SELECT_TCAM_ONLYm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, IFP_LOGICAL_TABLE_SELECT_TCAM_ONLY, PIPE,
                                         inst);
            break;
       case IFP_LOGICAL_TABLE_SELECT_DATA_ONLYm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, IFP_LOGICAL_TABLE_SELECT_DATA_ONLY, PIPE,
                                         inst);
            break;
       case EXACT_MATCH_LOGICAL_TABLE_SELECTm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, EXACT_MATCH_LOGICAL_TABLE_SELECT, PIPE,
                                         inst);
            break;
       case EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLYm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLY,
                                         PIPE, inst);
            break;
       case EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLYm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLY,
                                         PIPE, inst);
            break;
       case IFP_METER_TABLEm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, IFP_METER_TABLE, PIPE, inst);
            break;
       case EFP_METER_TABLEm:
            _FP_4PIPE_MEM_IDENTIFIER_GET(*mem_out, EFP_METER_TABLE, PIPE, inst);
            break;
       case EFP_COUNTER_TABLEm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, EFP_COUNTER_TABLE, PIPE, inst);
            break;
       case IFP_LOGICAL_TABLE_ACTION_PRIORITYm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, IFP_LOGICAL_TABLE_ACTION_PRIORITY, PIPE,
                                         inst);
            break;
       case TTL_FNm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, TTL_FN, PIPE, inst);
            break;
       case TOS_FNm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, TOS_FN, PIPE, inst);
            break;
       case TCP_FNm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, TCP_FN, PIPE, inst);
            break;
       case IP_PROTO_MAPm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, IP_PROTO_MAP, PIPE, inst);
            break;
       case SRC_COMPRESSIONm:
            _FP_4PIPE_MEM_IDENTIFIER_GET(*mem_out, SRC_COMPRESSION, PIPE, inst);
            break;
       case DST_COMPRESSIONm:
            _FP_4PIPE_MEM_IDENTIFIER_GET(*mem_out, DST_COMPRESSION, PIPE, inst);
            break;
       case EXACT_MATCH_DEFAULT_POLICYm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, EXACT_MATCH_DEFAULT_POLICY, PIPE, inst);
            break;
       case EXACT_MATCH_ACTION_PROFILEm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, EXACT_MATCH_ACTION_PROFILE, PIPE, inst);
            break;
       case EXACT_MATCH_QOS_ACTIONS_PROFILEm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, EXACT_MATCH_QOS_ACTIONS_PROFILE, PIPE,
                                         inst);
            break;
       case EXACT_MATCH_KEY_ATTRIBUTESm:
            _FP_2PIPE_MEM_IDENTIFIER_GET(*mem_out, EXACT_MATCH_KEY_ATTRIBUTES, PIPE,
                                         inst);
            break;
       case IFP_POLICY_ACTION_PROFILEm:
            _FP_2PIPE_MEM_IDENTIFIER_GET(*mem_out, IFP_POLICY_ACTION_PROFILE, PIPE,
                                         inst);
            break;
       case IFP_RANGE_CHECK_MASK_Am:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, IFP_RANGE_CHECK_MASK_A, PIPE, inst);
            break;
       case IFP_RANGE_CHECK_MASK_Bm:
            _FP_8PIPE_MEM_IDENTIFIER_GET(*mem_out, IFP_RANGE_CHECK_MASK_B, PIPE, inst);
            break;
       default:
            return BCM_E_INTERNAL;
   }

   return BCM_E_NONE;

}
#endif /* BCM_FIELD_SUPPORT */
