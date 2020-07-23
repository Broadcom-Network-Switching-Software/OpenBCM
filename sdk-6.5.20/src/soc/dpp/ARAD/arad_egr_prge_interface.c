/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $
*/


#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_EGRESS





#include <shared/bsl.h>

#include <soc/dcmn/error.h>
#include <soc/dpp/drv.h>

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_stack.h>

#include <soc/dpp/ARAD/arad_egr_prge_mgmt.h>
#include <soc/dpp/ARAD/arad_egr_prog_editor.h>
#include <soc/dpp/ARAD/arad_egr_prge_interface.h>

#include <soc/dpp/ARAD/arad_sw_db.h>
















typedef struct {
    uint8 code;
    uint8 src;
    int base;
} ARAD_EGR_PRGE_INTERFACE_SIZE_OFFSET_INFO;

typedef struct {
    predefined_data_e   field_id;
    uint8               src_select;
    uint8               offset_src;
    int                 offset_base;
} ARAD_EGR_PRGE_FIELD_PROPERTY;





STATIC CONST ARAD_EGR_PRGE_FIELD_PROPERTY prge_field_properties[PRGE_PREDEFINED_FIELDS_NOF] = {

    {PrgePktFieldFheiSize_UseR2TM    , ARAD_EGR_PROG_EDITOR_DATA_SRC_PKT, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_REG_2    , 1           }, 
    {PrgePktFieldFwdHdrOffset_UseR2TM, ARAD_EGR_PROG_EDITOR_DATA_SRC_PKT, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_REG_2    , 4           }, 


    {PrgeDataFieldOutLifProfile1     , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_192, 27          },
    {PrgeDataFieldOutLifProfile2     , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_192, 25          },
    {PrgeDataFieldOutLifProfile3     , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_192, 23          },
    {PrgeDataFieldOutLifProfile4     , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_192, 21          },
    {PrgeDataFieldOutLifMsb          , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_192, 19          },
    {PrgeDataFieldEesData3           , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_192, 18          },
    {PrgeDataFieldR0                 , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_128, 59          },
    {PrgeDataFieldR1                 , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_128, 55          },
    {PrgeDataFieldR2                 , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_128, 51          },
    {PrgeDataFieldR3                 , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_128, 47          },
    {PrgeDataFieldSrcSysPort         , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_128, 43          },
    {PrgeDataFieldDstSysPort         , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_128, 39          },
    {PrgeDataFieldNwkHdrSize         , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_128, 35          },
    {PrgeDataFieldSysHdrSize         , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_128, 33          },
    {PrgeDataFieldEesData1           , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_128, 31          },
    {PrgeDataFieldEesData2           , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_128, 17          },
    {PrgeDataFieldProgVar            , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_128, 3           },
    {PrgeDataFieldOutTmPort          , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 115         },
    {PrgeDataFieldOutPpPort          , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 113         },
    {PrgeDataFieldOamCounterValue    , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 111         },
    {PrgeDataFieldOamExtId           , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 103         },
    {PrgeDataFieldOamExtSubType      , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 98          },
    {PrgeDataFieldTod1588            , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 97          },
    {PrgeDataFieldTodNtp             , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 81          },
    {PrgeDataFieldVsiVar             , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 65          },
    {PrgeDataFieldAceVar             , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 59          },
    {PrgeDataFieldOutLif             , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 57          },
    {PrgeDataFieldTmPortVar          , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 53          },
    {PrgeDataFieldPpPortVar          , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 45          },
    {PrgeDataFieldOamId              , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 37          },
    {PrgeDataFieldOamSubType         , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 33          },
    {PrgeDataFieldDataMemoryEntry    , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 32          }

};

STATIC CONST ARAD_EGR_PRGE_FIELD_PROPERTY prge_field_properties_jer_plus[PRGE_PREDEFINED_FIELDS_NOF] = {

    {PrgePktFieldFheiSize_UseR2TM    , ARAD_EGR_PROG_EDITOR_DATA_SRC_PKT, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_REG_2    , 1           }, 
    {PrgePktFieldFwdHdrOffset_UseR2TM, ARAD_EGR_PROG_EDITOR_DATA_SRC_PKT, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_REG_2    , 4           }, 


    {PrgeDataFieldOutLifProfile1     , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_192, 22          },
    {PrgeDataFieldOutLifProfile2     , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_192, 20          },
    {PrgeDataFieldOutLifProfile3     , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_192, 18          },
    {PrgeDataFieldOutLifProfile4     , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_192, 16          },
    {PrgeDataFieldOutLifMsb          , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_192, 14          },
    {PrgeDataFieldOutLifProfile5     , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_192, 13          },
    {PrgeDataFieldOutLifProfile6     , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_192, 11          },
    {PrgeDataFieldOutLifProfile7     , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_192, 9          },
    {PrgeDataFieldNativeLLSize       , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_192, 7          },
    {PrgeDataFieldCud2               , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_192, 5          },
    {PrgeDataFieldLBKey              , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_192, 0           },
    {PrgeDataFieldR0                 , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_128, 59          },
    {PrgeDataFieldR1                 , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_128, 55          },
    {PrgeDataFieldR2                 , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_128, 51          },
    {PrgeDataFieldR3                 , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_128, 47          },
    {PrgeDataFieldSrcSysPort         , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_128, 43          },
    {PrgeDataFieldDstSysPort         , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_128, 39          },
    {PrgeDataFieldNwkHdrSize         , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_128, 35          },
    {PrgeDataFieldSysHdrSize         , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_128, 33          },
    {PrgeDataFieldEesData1           , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_128, 31          },
    {PrgeDataFieldEesData2           , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_128, 17          },
    {PrgeDataFieldProgVar            , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_128, 3           },
    {PrgeDataFieldOutTmPort          , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 115         },
    {PrgeDataFieldOutPpPort          , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 113         },
    {PrgeDataFieldOamCounterValue    , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 111         },
    {PrgeDataFieldOamExtId           , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 103         },
    {PrgeDataFieldOamExtSubType      , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 98          },
    {PrgeDataFieldTod1588            , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 97          },
    {PrgeDataFieldTodNtp             , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 81          },
    {PrgeDataFieldVsiVar             , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 65          },
    {PrgeDataFieldAceVar             , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 59          },
    {PrgeDataFieldOutLif             , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 57          },
    {PrgeDataFieldTmPortVar          , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 53          },
    {PrgeDataFieldPpPortVar          , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 45          },
    {PrgeDataFieldOamId              , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 37          },
    {PrgeDataFieldOamSubType         , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 33          },
    {PrgeDataFieldDataMemoryEntry    , ARAD_EGR_PROG_EDITOR_DATA_SRC_RES, ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0  , 32          }

};

STATIC uint8 arad_egr_prge_interface_field_available_by_device[SOC_MAX_NUM_DEVICES][PRGE_PREDEFINED_FIELDS_NOF];
STATIC CONST predefined_data_e arad_egr_prge_interface_field_available_arad[] = {
    
    PrgePktFieldFheiSize_UseR2TM,
    PrgePktFieldFwdHdrOffset_UseR2TM,
    
    PrgeDataFieldR0,
    PrgeDataFieldR1,
    PrgeDataFieldR2,
    PrgeDataFieldR3,
    PrgeDataFieldSrcSysPort,
    PrgeDataFieldDstSysPort,
    PrgeDataFieldNwkHdrSize,
    PrgeDataFieldSysHdrSize,
    PrgeDataFieldEesData1,
    PrgeDataFieldEesData2,
    PrgeDataFieldProgVar,
    PrgeDataFieldOutTmPort,
    PrgeDataFieldOutPpPort,
    PrgeDataFieldOamCounterValue,
    PrgeDataFieldOamExtId,
    PrgeDataFieldOamExtSubType,
    PrgeDataFieldTod1588,
    PrgeDataFieldTodNtp,
    PrgeDataFieldVsiVar,
    PrgeDataFieldAceVar,
    PrgeDataFieldOutLif,
    PrgeDataFieldTmPortVar,
    PrgeDataFieldPpPortVar,
    PrgeDataFieldOamId,
    PrgeDataFieldOamSubType,
    PrgeDataFieldDataMemoryEntry
};
STATIC CONST predefined_data_e arad_egr_prge_interface_field_available_jer[] = {
    
    PrgePktFieldFheiSize_UseR2TM,
    PrgePktFieldFwdHdrOffset_UseR2TM,
    
    PrgeDataFieldOutLifProfile1,
    PrgeDataFieldOutLifProfile2,
    PrgeDataFieldOutLifProfile3,
    PrgeDataFieldOutLifProfile4,
    PrgeDataFieldOutLifMsb,
    
    PrgeDataFieldEesData3,
    
    PrgeDataFieldR0,
    PrgeDataFieldR1,
    PrgeDataFieldR2,
    PrgeDataFieldR3,
    PrgeDataFieldSrcSysPort,
    PrgeDataFieldDstSysPort,
    PrgeDataFieldNwkHdrSize,
    PrgeDataFieldSysHdrSize,
    PrgeDataFieldEesData1,
    PrgeDataFieldEesData2,
    PrgeDataFieldProgVar,
    PrgeDataFieldOutTmPort,
    PrgeDataFieldOutPpPort,
    PrgeDataFieldOamCounterValue,
    PrgeDataFieldOamExtId,
    PrgeDataFieldOamExtSubType,
    PrgeDataFieldTod1588,
    PrgeDataFieldTodNtp,
    PrgeDataFieldVsiVar,
    PrgeDataFieldAceVar,
    PrgeDataFieldOutLif,
    PrgeDataFieldTmPortVar,
    PrgeDataFieldPpPortVar,
    PrgeDataFieldOamId,
    PrgeDataFieldOamSubType,
    PrgeDataFieldDataMemoryEntry
};
STATIC CONST predefined_data_e arad_egr_prge_interface_field_available_qax[] = {
    
    PrgePktFieldFheiSize_UseR2TM,
    PrgePktFieldFwdHdrOffset_UseR2TM,

    
    
    PrgeDataFieldOutLifProfile5,
    PrgeDataFieldOutLifProfile6,
    PrgeDataFieldOutLifProfile7,
    PrgeDataFieldNativeLLSize,
    PrgeDataFieldCud2,
    PrgeDataFieldLBKey,
    
    PrgeDataFieldOutLifProfile1,
    PrgeDataFieldOutLifProfile2,
    PrgeDataFieldOutLifProfile3,
    PrgeDataFieldOutLifProfile4,
    PrgeDataFieldOutLifMsb,
    
    PrgeDataFieldR0,
    PrgeDataFieldR1,
    PrgeDataFieldR2,
    PrgeDataFieldR3,
    PrgeDataFieldSrcSysPort,
    PrgeDataFieldDstSysPort,
    PrgeDataFieldNwkHdrSize,
    PrgeDataFieldSysHdrSize,
    PrgeDataFieldEesData1,
    PrgeDataFieldEesData2,
    PrgeDataFieldProgVar,
    PrgeDataFieldOutTmPort,
    PrgeDataFieldOutPpPort,
    PrgeDataFieldOamCounterValue,
    PrgeDataFieldOamExtId,
    PrgeDataFieldOamExtSubType,
    PrgeDataFieldTod1588,
    PrgeDataFieldTodNtp,
    PrgeDataFieldVsiVar,
    PrgeDataFieldAceVar,
    PrgeDataFieldOutLif,
    PrgeDataFieldTmPortVar,
    PrgeDataFieldPpPortVar,
    PrgeDataFieldOamId,
    PrgeDataFieldOamSubType,
    PrgeDataFieldDataMemoryEntry
};






int 
    arad_egr_prge_valid_short_val(uint32 val)
{
    if ((val & 0xFFFF0000) && ((val & 0xFFFF0000) != 0xFFFF0000)) {
        
        return 0;
    }
    
    return 1;
}



void
  ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION_clear(
     ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION * instr
  ) {
    sal_memset(instr, 0, sizeof(ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION));
}


void
  arad_egr_prge_interface_field_available_by_device_init(
     int unit
  ) {
    predefined_data_e CONST *available_fields_arr;
    int available_fields_nof;
    int i;

    
    for (i = 0; i < PRGE_PREDEFINED_FIELDS_NOF; i++) {
        arad_egr_prge_interface_field_available_by_device[unit][i] = 0;
    }

    
    if (SOC_IS_JERICHO_PLUS(unit)) {
        available_fields_arr = arad_egr_prge_interface_field_available_qax;
        available_fields_nof = sizeof(arad_egr_prge_interface_field_available_qax)/sizeof(predefined_data_e);
    }
    else if (SOC_IS_JERICHO(unit)) {
        available_fields_arr = arad_egr_prge_interface_field_available_jer;
        available_fields_nof = sizeof(arad_egr_prge_interface_field_available_jer)/sizeof(predefined_data_e);
    }
    else { 
        available_fields_arr = arad_egr_prge_interface_field_available_arad;
        available_fields_nof = sizeof(arad_egr_prge_interface_field_available_arad)/sizeof(predefined_data_e);
    }

    
    for (i = 0; i < available_fields_nof; i++) {
        arad_egr_prge_interface_field_available_by_device[unit][available_fields_arr[i]] = 1;
    }

}

uint32
  arad_egr_prge_interface_ce_offset_parse(
     int     unit,
     ARAD_EGR_PRGE_INTERFACE_SIZE_OFFSET_INFO  *offset_info,
     ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  ) {
    int
        offset;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(instr);
    SOC_SAND_CHECK_NULL_INPUT(offset_info);

    switch (offset_info->code) {
    case ARAD_EGR_PROG_EDITOR_OFFSET_CODE_REG:
        instr->ce_interface_instruction.offset_src = offset_info->src;
        instr->ce_interface_instruction.offset_base = ((uint8)offset_info->base);
        break;
    case ARAD_EGR_PROG_EDITOR_OFFSET_CODE_CONST:
        
        offset = offset_info->base;
        if (offset > (192 + 127)) { 
            SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE error - Offset too large")));
        }
        else if (offset >= 192) {
            instr->ce_interface_instruction.offset_src = ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_192;
            instr->ce_interface_instruction.offset_base = ((uint8)(offset - 192));
        }
        else if (offset >= 128) {
            instr->ce_interface_instruction.offset_src = ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_128;
            instr->ce_interface_instruction.offset_base = ((uint8)(offset - 128));
        }
        else {
            instr->ce_interface_instruction.offset_src = ARAD_EGR_PROG_EDITOR_OFFSET_SRC_CONST_0;
            instr->ce_interface_instruction.offset_base = ((uint8)offset);
        }
        break;
    default:
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE Interface internal error - Wrong offset type")));
    }

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_interface_ce_offset_parse()",0,0);
}


uint32
  arad_egr_prge_interface_read_data_instr(
     int     unit,
     uint32  data_src,
     uint32  offset_code,
     uint32  offset_src,
     int     offset_base,
     ARAD_EGR_PROG_EDITOR_LFEM lfem,
     ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  ) {

    ARAD_EGR_PRGE_INTERFACE_SIZE_OFFSET_INFO
        offset_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(instr);

    
    instr->ce_interface_instruction.src_select = data_src;

    
    offset_info.code = offset_code;
    offset_info.src = offset_src;
    offset_info.base = offset_base;
    SOC_SAND_CHECK_FUNC_RESULT(arad_egr_prge_interface_ce_offset_parse(unit, &offset_info, instr), 380, exit);

    
    instr->ce_interface_instruction.size_base = 0;
    instr->ce_interface_instruction.size_src = ARAD_EGR_PROG_EDITOR_SIZE_SRC_CONST_0;

    
    instr->ce_interface_instruction.lfem = lfem;

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_interface_read_data_instr()",0,0);
}


uint32
  arad_egr_prge_interface_ce_field_verify(
     int                                  unit,
     predefined_data_e                    field
  ) {

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (!arad_egr_prge_interface_field_available_by_device[unit][field]) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE Interface error - Field not available for the device")));
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_interface_ce_field_verify()",field,0);
}

uint32
  arad_egr_prge_interface_ce_field_parse(
     int                                  unit,
     predefined_data_e                    field,
     int                                  offset_from_field_lsb,
     ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  ) {

    int i;
    CONST ARAD_EGR_PRGE_FIELD_PROPERTY *prge_field_properties_by_device = SOC_IS_JERICHO_PLUS(unit) ?
        prge_field_properties_jer_plus :
        prge_field_properties ;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(instr);

    
    SOC_SAND_CHECK_FUNC_RESULT(arad_egr_prge_interface_ce_field_verify(unit, field), 381, exit);

    
    for (i = 0; i < PRGE_PREDEFINED_FIELDS_NOF; i++) {
        if (prge_field_properties_by_device[i].field_id == field) {
            instr->ce_interface_instruction.src_select = prge_field_properties_by_device[i].src_select;
            instr->ce_interface_instruction.offset_src = prge_field_properties_by_device[i].offset_src;
            instr->ce_interface_instruction.offset_base = (prge_field_properties_by_device[i].offset_base + offset_from_field_lsb) & 0xFF;
            SOC_SAND_EXIT_NO_ERROR;
        }
    }

    SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE Interface internal error - Field not defined")));
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_interface_ce_field_parse()",field,0);
}


uint32
  arad_egr_prge_interface_read_field_instr(
     int                                  unit,
     predefined_data_e                    field,
     int                                  offset,
     ARAD_EGR_PROG_EDITOR_LFEM            lfem,
     ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  ) {

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(instr);

    
    SOC_SAND_CHECK_FUNC_RESULT(arad_egr_prge_interface_ce_field_parse(unit, field, offset, instr), 380, exit);

    
    instr->ce_interface_instruction.size_base = 0;
    instr->ce_interface_instruction.size_src = ARAD_EGR_PROG_EDITOR_SIZE_SRC_CONST_0;

    
    instr->ce_interface_instruction.lfem = lfem;

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_interface_read_field_instr()",0,0);
}

uint32
  arad_egr_prge_interface_ce_size_parse(
     int                                  unit,
     ARAD_EGR_PRGE_INTERFACE_SIZE_OFFSET_INFO  *size_info,
     ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  ) {

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(instr);
    SOC_SAND_CHECK_NULL_INPUT(size_info);

    if ((size_info->base > 15) || (size_info->base < -16)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE error - Size too large")));
    }

    instr->ce_interface_instruction.size_base = ((uint8)size_info->base)&0x1F;

    switch (size_info->code) {
    case ARAD_EGR_PROG_EDITOR_SIZE_CODE_REG:
        instr->ce_interface_instruction.size_src = size_info->src;
        break;
    case ARAD_EGR_PROG_EDITOR_OFFSET_CODE_CONST:
        instr->ce_interface_instruction.size_src = ARAD_EGR_PROG_EDITOR_SIZE_SRC_CONST_0;
        break;
    default:
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE Interface internal error - Wrong offset type")));
    }

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_interface_ce_size_parse()",0,0);
}


uint32
  arad_egr_prge_interface_copy_data_instr(
     int                                  unit,
     uint32                               data_src,
     uint32                               offset_code,
     uint32                               offset_src,
     int                                  offset_base,
     uint32                               size_code,
     uint32                               size_src,
     int                                  size_base,
     ARAD_EGR_PROG_EDITOR_LFEM            lfem,
     ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  ) {

    ARAD_EGR_PRGE_INTERFACE_SIZE_OFFSET_INFO
        size_offset_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(instr);

    
    instr->ce_interface_instruction.src_select = data_src;

    
    size_offset_info.code = offset_code;
    size_offset_info.src = offset_src;
    size_offset_info.base = offset_base;
    SOC_SAND_CHECK_FUNC_RESULT(arad_egr_prge_interface_ce_offset_parse(unit, &size_offset_info, instr), 380, exit);

    
    size_offset_info.code = size_code;
    size_offset_info.src = size_src;
    size_offset_info.base = size_base;
    SOC_SAND_CHECK_FUNC_RESULT(arad_egr_prge_interface_ce_size_parse(unit, &size_offset_info, instr), 380, exit);

    
    instr->ce_interface_instruction.lfem = lfem;

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_interface_copy_data_instr()",0,0);
}


uint32
  arad_egr_prge_interface_copy_field_instr(
     int     unit,
     predefined_data_e                    field,
     int                                  offset,
     uint32                               size_code,
     uint32                               size_src,
     int                                  size_base,
     ARAD_EGR_PROG_EDITOR_LFEM            lfem,
     ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  ) {

    ARAD_EGR_PRGE_INTERFACE_SIZE_OFFSET_INFO
        size_info;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(instr);

    
    SOC_SAND_CHECK_FUNC_RESULT(arad_egr_prge_interface_ce_field_parse(unit, field, offset, instr), 380, exit);

    
    size_info.code = size_code;
    size_info.src = size_src;
    size_info.base = size_base;
    SOC_SAND_CHECK_FUNC_RESULT(arad_egr_prge_interface_ce_size_parse(unit, &size_info, instr), 380, exit);

    
    instr->ce_interface_instruction.lfem = lfem;

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_interface_copy_field_instr()",0,0);
}


uint32
  arad_egr_prge_interface_ce_nop(
     int     unit,
     ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  ) {

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(instr);

    instr->ce_interface_instruction.src_select = _ARAD_EGR_PRGE_MGMT_CE_DATA_SRC_NOP;

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_interface_ce_nop()",0,0);
}

uint32
  arad_egr_prge_interface_alu_op_parse(
    int      unit,
    uint32   op_num,
    uint32   op_type,
    uint32   op_val,
    ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  ) {

    uint8
        *p_op;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(instr);

    switch (op_num) {
    case 1: 
        p_op = &(instr->alu_interface_instruction.op1);
        break;
    case 2: 
        p_op = &(instr->alu_interface_instruction.op2);
        break;
    case 3: 
        p_op = &(instr->alu_interface_instruction.op3);
        break;
    default:
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE Interface internal error - Wrong op number")));
    }

    switch (op_type) {
    case ARAD_EGR_PROG_EDITOR_OP_TYPE_REG:
        *p_op = op_val;
        break;
    case ARAD_EGR_PROG_EDITOR_OP_TYPE_CONST:
        if (!arad_egr_prge_valid_short_val(op_val)) {
            SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE error - alu val not valid - bound to 16 bits")));
        }
        *p_op = ARAD_PP_PRGE_ENG_OP_VALUE;
        instr->alu_interface_instruction.op_value = op_val;
        break;
    case ARAD_EGR_PROG_EDITOR_OP_TYPE_FEM:
        if (op_num == 3) {
            SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE Interface internal error - op3 can not be LFEM")));
        }
        *p_op = ARAD_PP_PRGE_ENG_OP_FEM;
        break;
    case ARAD_EGR_PROG_EDITOR_OP_TYPE_ALU:
        if (op_num != 3) {
            SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE Interface internal error - only op3 can select ALU result")));
        }
        *p_op = ARAD_PP_PRGE_ENG_OP_ALU;
        break;
    default:
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE Interface internal error - Wrong op type")));
    }
    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_interface_alu_op_parse()",0,0);
}


uint32
  arad_egr_prge_interface_alu_arith_instr(
    int      unit,
    uint32   op1_type,
    uint32   op1_val,
    uint32   op2_type,
    uint32   op2_val,
    uint32   alu_action,
    uint32   dst_type,
    uint32   dst_reg,
    ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  ) {
    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(instr);

    if (dst_type == ARAD_EGR_PROG_EDITOR_OP_TYPE_DATA_ADDR) {
        res = arad_egr_prge_mgmt_current_program_load_only_set(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 390, exit);
    }
    else if (dst_type != ARAD_EGR_PROG_EDITOR_OP_TYPE_REG) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE error - can't assign to that operand")));
    }

    SOC_SAND_CHECK_FUNC_RESULT(arad_egr_prge_interface_alu_op_parse(unit, 1, op1_type, op1_val, instr), 390, exit);
    SOC_SAND_CHECK_FUNC_RESULT(arad_egr_prge_interface_alu_op_parse(unit, 2, op2_type, op2_val, instr), 390, exit);

    instr->alu_interface_instruction.alu_action = alu_action;
    instr->alu_interface_instruction.cmp_action = ARAD_EGR_PROG_EDITOR_ALU_CMP_ALL;
    instr->alu_interface_instruction.op3 = ARAD_PP_PRGE_ENG_OP_ALU;
    instr->alu_interface_instruction.dst_select = dst_reg;

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_interface_add_instr()",0,0);
}


uint32
  arad_egr_prge_interface_add_instr(
    int      unit,
    uint32   op1_type,
    uint32   op1_val,
    uint32   op2_type,
    uint32   op2_val,
    uint32   dst_type,
    uint32   dst_reg,
    ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  ) {
    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(instr);

    res = arad_egr_prge_interface_alu_arith_instr(unit,
                                                  op1_type,
                                                  op1_val,
                                                  op2_type,
                                                  op2_val,
                                                  ARAD_PP_EG_PROG_ALU_ADD,
                                                  dst_type,
                                                  dst_reg,
                                                  instr);
    SOC_SAND_CHECK_FUNC_RESULT(res, 390, exit);

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_interface_add_instr()",0,0);
}


uint32
  arad_egr_prge_interface_sub_instr(
    int      unit,
    uint32   op1_type,
    uint32   op1_val,
    uint32   op2_type,
    uint32   op2_val,
    uint32   dst_type,
    uint32   dst_reg,
    ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  ) {
    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(instr);

    res = arad_egr_prge_interface_alu_arith_instr(unit,
                                                  op1_type,
                                                  op1_val,
                                                  op2_type,
                                                  op2_val,
                                                  ARAD_PP_EG_PROG_ALU_SUB,
                                                  dst_type,
                                                  dst_reg,
                                                  instr);
    SOC_SAND_CHECK_FUNC_RESULT(res, 390, exit);

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_interface_sub_instr()",0,0);
}


uint32
  arad_egr_prge_interface_set_instr(
    int      unit,
    uint32   val,
    uint32   dst_type,
    uint32   dst_reg,
    ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  ) {

    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(instr);

    if (dst_type == ARAD_EGR_PROG_EDITOR_OP_TYPE_DATA_ADDR) {
        res = arad_egr_prge_mgmt_current_program_load_only_set(unit);
        SOC_SAND_CHECK_FUNC_RESULT(res, 390, exit);
    }
    else if (dst_type != ARAD_EGR_PROG_EDITOR_OP_TYPE_REG) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE error - can't assign to that operand")));
    }

    SOC_SAND_CHECK_FUNC_RESULT(arad_egr_prge_interface_alu_op_parse(unit, 3, ARAD_EGR_PROG_EDITOR_OP_TYPE_CONST, val, instr), 390, exit);

    instr->alu_interface_instruction.cmp_action = ARAD_EGR_PROG_EDITOR_ALU_CMP_ALL;
    instr->alu_interface_instruction.dst_select = dst_reg;

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_interface_set_instr()",0,0);
}


uint32
  arad_egr_prge_interface_set_if_instr(
    int      unit,
    uint32   op1_type,
    uint32   op1_val,
    uint32   cmp_type,
    uint32   op2_type,
    uint32   op2_val,
    uint32   res_type,
    uint32   res_val,
    uint32   __ignored,
    uint32   dst_reg,
    uint32   alu_action,
    ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  ) {
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(instr);

    SOC_SAND_CHECK_FUNC_RESULT(arad_egr_prge_interface_alu_op_parse(unit, 1, op1_type, op1_val, instr), 390, exit);
    SOC_SAND_CHECK_FUNC_RESULT(arad_egr_prge_interface_alu_op_parse(unit, 2, op2_type, op2_val, instr), 390, exit);
    SOC_SAND_CHECK_FUNC_RESULT(arad_egr_prge_interface_alu_op_parse(unit, 3, res_type, res_val, instr), 390, exit);

    instr->alu_interface_instruction.alu_action = alu_action;
    instr->alu_interface_instruction.cmp_action = cmp_type;
    instr->alu_interface_instruction.dst_select = dst_reg;

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_interface_set_if_instr()",0,0);
}


uint32
  arad_egr_prge_interface_jump_if_instr(
    int      unit,
    uint32   op1_type,
    uint32   op1_val,
    uint32   cmp_type,
    uint32   op2_type,
    uint32   op2_val,
    ARAD_EGR_PROG_EDITOR_BRANCH branch,
    ARAD_EGR_PROG_EDITOR_JUMP_POINT jump_point,
    ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  ) {
    uint32
        res;
    uint32
        branch_pointer;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(instr);

    if ((op1_type == ARAD_EGR_PROG_EDITOR_OP_TYPE_CONST)
        || (op2_type == ARAD_EGR_PROG_EDITOR_OP_TYPE_CONST)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE error - can't compare to const and jump to branch")));
    }

    
    res = arad_egr_prog_editor_branch_pointer_allocate(unit, branch, &branch_pointer);
    SOC_SAND_CHECK_FUNC_RESULT(res, 390, exit);

    
    res = arad_egr_prge_mgmt_program_jump_point_add(unit, jump_point);
    SOC_SAND_CHECK_FUNC_RESULT(res, 390, exit);

    
    SOC_SAND_CHECK_FUNC_RESULT(arad_egr_prge_interface_alu_op_parse(unit, 1, op1_type, op1_val, instr), 390, exit);
    SOC_SAND_CHECK_FUNC_RESULT(arad_egr_prge_interface_alu_op_parse(unit, 2, op2_type, op2_val, instr), 390, exit);
    instr->alu_interface_instruction.alu_action = ARAD_PP_EG_PROG_ALU_SUB;
    instr->alu_interface_instruction.cmp_action = cmp_type;

    
    instr->alu_interface_instruction.op_value = branch_pointer; 
    instr->alu_interface_instruction.op3 = ARAD_PP_PRGE_ENG_OP_VALUE;
    instr->alu_interface_instruction.dst_select = ARAD_PP_PRGE_ENG_OP_SIZE_ADDR;

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_interface_set_if_instr()",0,0);
}


uint32
  arad_egr_prge_interface_jump_instr(
    int      unit,
    uint32   addr_op_type,
    uint32   addr_op_val,
    ARAD_EGR_PROG_EDITOR_JUMP_POINT jump_point,
    ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  ){
    uint32
        res;

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(instr);

    if ((addr_op_type == ARAD_EGR_PROG_EDITOR_OP_TYPE_CONST)
        || (addr_op_type == ARAD_EGR_PROG_EDITOR_OP_TYPE_FEM)) {
        SOC_SAND_SET_ERROR_MSG((_BSL_SOCDNX_SAND_MSG("PRGE error - can't jump to constant branch address without comparrison, nor directly to FEM")));
    }

    
    res = arad_egr_prge_mgmt_program_jump_point_add(unit, jump_point);
    SOC_SAND_IF_ERR_EXIT(res);

    

    SOC_SAND_CHECK_FUNC_RESULT(arad_egr_prge_interface_alu_op_parse(unit, 1, addr_op_type, addr_op_val, instr), 390, exit);
    
    SOC_SAND_CHECK_FUNC_RESULT(arad_egr_prge_interface_alu_op_parse(unit, 2, ARAD_EGR_PROG_EDITOR_OP_TYPE_CONST, 0, instr), 390, exit);
    instr->alu_interface_instruction.alu_action = ARAD_PP_EG_PROG_ALU_ADD;

    instr->alu_interface_instruction.cmp_action = ARAD_EGR_PROG_EDITOR_ALU_CMP_ALL;
    instr->alu_interface_instruction.op3 = ARAD_PP_PRGE_ENG_OP_ALU;

    instr->alu_interface_instruction.dst_select = ARAD_PP_PRGE_ENG_OP_SIZE_ADDR;

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_interface_jump_instr()",0,0);
}


uint32
  arad_egr_prge_interface_alu_nop(
     int     unit,
     ARAD_EGR_PRGE_MGMT_INTERFACE_INSTRUCTION  *instr
  ) {
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    SOC_SAND_CHECK_NULL_INPUT(instr);

    instr->alu_interface_instruction.cmp_action = ARAD_EGR_PROG_EDITOR_ALU_CMP_NONE;

    SOC_SAND_EXIT_NO_ERROR;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egr_prge_interface_alu_nop()",0,0);
}




#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

#undef _ERR_MSG_MODULE_NAME

