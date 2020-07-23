#include <soc/mcm/memregs.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#if defined(BCM_88650_A0)
/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_CHIP


#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/ARAD/arad_chip_regs.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/mem.h>
#include <soc/dpp/ARAD/arad_chip_regs.h>

#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/hwstate/hw_log.h>
#endif 




#define FIX_MEM_ORDER_E(v,m) (((m)->flags & SOC_MEM_FLAG_BE) ? BYTES2WORDS((m)->bytes)-1-(v) : (v))














ARAD_FAST_REG_CALL_INFO     g_fast_reg_info_table[BCM_MAX_NUM_UNITS][ARAD_FAST_REG_TYPE_LAST];
ARAD_FAST_FIELD_CALL_INFO   g_fast_field_info_table[BCM_MAX_NUM_UNITS][ARAD_FAST_FIELD_TYPE_LAST];
ARAD_FAST_MEMORY_CALL_INFO  g_fast_mem_info_table[BCM_MAX_NUM_UNITS][ARAD_FAST_MEM_TYPE_LAST];





extern void _soc_reg_debug(int unit, int access_width, char *op_str, uint32 addr, uint32 data_hi, uint32 data_lo);
extern void _soc_reg_above_64_debug(int unit, char *op_str, soc_block_t block, uint32 addr, soc_reg_above_64_val_t data);

void
    arad_fast_reg_value_init(int unit)
{


    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_MACT_CPU_REQUEST_REQUEST].reg                 = PPDB_B_LARGE_EM_CPU_REQUEST_REQUESTr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_MACT_CPU_REQUEST_REQUEST].index               = 0;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_MACT_REPLY_FIFO_REPLY_FIFO_ENTRY_COUNT].reg   = PPDB_B_LARGE_EM_REPLY_FIFO_REPLY_FIFO_ENTRY_COUNTr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_MACT_REPLY_FIFO_REPLY_FIFO_ENTRY_COUNT].index = 0;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_MACT_REPLY].reg                               = PPDB_B_LARGE_EM_REPLYr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_MACT_REPLY].index                             = 0;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_MACT_CPU_REQUEST_TRIGGER].reg                 = SOC_IS_JERICHO(unit)? PPDB_B_LARGE_EM_CPU_REQUEST_TRIGGERr : IHP_MACT_CPU_REQUEST_TRIGGERr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_MACT_CPU_REQUEST_TRIGGER].index               = 0;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_ISA_INTERRUPT_REGISTER].reg                   = SOC_IS_QAX(unit) ?    PPDB_A_ISEM_INTERRUPT_REGISTER_ONEr :
                                                                                                  SOC_IS_JERICHO(unit)? IHB_ISEM_INTERRUPT_REGISTER_ONEr :
                                                                                                                        IHP_ISA_INTERRUPT_REGISTER_ONEr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_ISA_INTERRUPT_REGISTER].index                 = 0;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_ISB_INTERRUPT_REGISTER].reg                   = SOC_IS_QAX(unit) ?    PPDB_B_ISEM_INTERRUPT_REGISTER_ONEr :
                                                                                                  SOC_IS_JERICHO(unit)? IHB_ISEM_INTERRUPT_REGISTER_ONEr:
                                                                                                                        IHP_ISB_INTERRUPT_REGISTER_ONEr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_ISB_INTERRUPT_REGISTER].index                 = 0;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_ISA_MANAGEMENT_UNIT_FAILURE].reg              = SOC_IS_QAX(unit) ?    PPDB_A_ISEM_MANAGEMENT_UNIT_FAILUREr :
                                                                                                  SOC_IS_JERICHO(unit)? IHB_ISEM_MANAGEMENT_UNIT_FAILUREr:
                                                                                                                        IHP_ISA_MANAGEMENT_UNIT_FAILUREr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_ISA_MANAGEMENT_UNIT_FAILURE].index            = 0;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_ISB_MANAGEMENT_UNIT_FAILURE].reg              = SOC_IS_QAX(unit) ?    PPDB_B_ISEM_MANAGEMENT_UNIT_FAILUREr :
                                                                                                  SOC_IS_JERICHO(unit)? IHB_ISEM_MANAGEMENT_UNIT_FAILUREr:
                                                                                                                        IHP_ISB_MANAGEMENT_UNIT_FAILUREr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_ISB_MANAGEMENT_UNIT_FAILURE].index            = 0;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_OAMP_IPV4_SRC_ADDR_SELECT].reg  = OAMP_IPV4_SRC_ADDR_SELECTr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_OAMP_IPV4_SRC_ADDR_SELECT].index  = 0;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_OAMP_IPV4_TOS_TTL_SELECT].reg  = OAMP_IPV4_TOS_TTL_SELECTr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_OAMP_IPV4_TOS_TTL_SELECT].index  = 0;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_OAMP_BFD_TX_RATE].reg  = OAMP_BFD_TX_RATEr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_OAMP_BFD_TX_RATE].index  = 0;
    
    g_fast_reg_info_table[unit][ARAD_FAST_REG_OAMP_BFD_REQ_INTERVAL_POINTER].reg  = OAMP_BFD_REQ_INTERVAL_POINTERr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_OAMP_BFD_REQ_INTERVAL_POINTER].index  = 0; 

    g_fast_reg_info_table[unit][ARAD_FAST_REG_OAMP_MPLS_PWE_PROFILE].reg  = OAMP_MPLS_PWE_PROFILEr ;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_OAMP_MPLS_PWE_PROFILE].index  = 0; 

    g_fast_reg_info_table[unit][ARAD_FAST_REG_OAMP_BFD_TX_IPV4_MULTI_HOP].reg  = !SOC_IS_QAX(unit) ? OAMP_BFD_TX_IPV4_MULTI_HOPr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_OAMP_BFD_TX_IPV4_MULTI_HOP].index  = 0; 

    g_fast_reg_info_table[unit][ARAD_FAST_REG_OAMP_PR_2_FW_DTC].reg  = OAMP_PR_2_FW_DTCr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_OAMP_PR_2_FW_DTC].index  = 0; 

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_WORD].reg                   = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_RESPONSE_WORDr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_WORD].index                 = 0;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_WORD].reg                    = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_COMMAND_WORDr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_WORD].index                  = 0;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_0].reg                  = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_COMMAND_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_0].index                = 0;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_1].reg                  = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_COMMAND_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_1].index                = 1;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_2].reg                  = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_COMMAND_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_2].index                = 2;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_3].reg                  = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_COMMAND_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_3].index                = 3;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_4].reg                  = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_COMMAND_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_4].index                = 4;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_5].reg                  = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_COMMAND_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_5].index                = 5;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_6].reg                  = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_COMMAND_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_6].index                = 6;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_7].reg                  = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_COMMAND_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_7].index                = 7;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_8].reg                  = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_COMMAND_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_8].index                = 8;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_9].reg                  = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_COMMAND_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_9].index                = 9;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_10].reg                 = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_COMMAND_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_10].index               = 10;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_11].reg                 = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_COMMAND_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_11].index               = 11;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_12].reg                 = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_COMMAND_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_12].index               = 12;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_13].reg                 = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_COMMAND_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_13].index               = 13;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_14].reg                 = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_COMMAND_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_COMMAND_DATA_14].index               = 14;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_0].reg                 = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_RESPONSE_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_0].index               = 0;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_1].reg                 = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_RESPONSE_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_1].index               = 1;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_2].reg                 = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_RESPONSE_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_2].index               = 2;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_3].reg                 = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_RESPONSE_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_3].index               = 3;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_4].reg                 = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_RESPONSE_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_4].index               = 4;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_5].reg                 = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_RESPONSE_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_5].index               = 5;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_6].reg                 = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_RESPONSE_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_6].index               = 6;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_7].reg                 = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_RESPONSE_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_7].index               = 7;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_8].reg                 = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_RESPONSE_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_8].index               = 8;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_9].reg                 = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_RESPONSE_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_9].index               = 9;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_10].reg                = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_RESPONSE_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_10].index              = 10;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_11].reg                = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_RESPONSE_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_11].index              = 11;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_12].reg                = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_RESPONSE_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_12].index              = 12;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_13].reg                = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_RESPONSE_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_13].index              = 13;

    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_14].reg                = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_RESPONSE_DATAr : INVALIDr;
    g_fast_reg_info_table[unit][ARAD_FAST_REG_IHP_KAPS_IBC_RESPONSE_DATA_14].index              = 14;
}

void
    arad_fast_field_value_init(int unit)
{
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_QUALIFIER].reg   = PPDB_B_LARGE_EM_CPU_REQUEST_REQUESTr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_QUALIFIER].field = LARGE_EM_REQ_QUALIFIERf;    

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_SELF].reg   = PPDB_B_LARGE_EM_CPU_REQUEST_REQUESTr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_SELF].field = LARGE_EM_REQ_SELFf;    

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_AGE_PAYLOAD].reg   = PPDB_B_LARGE_EM_CPU_REQUEST_REQUESTr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_AGE_PAYLOAD].field = LARGE_EM_REQ_AGE_PAYLOADf;    

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_PAYLOAD_IS_DYNAMIC].reg   = PPDB_B_LARGE_EM_CPU_REQUEST_REQUESTr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_PAYLOAD_IS_DYNAMIC].field = LARGE_EM_REQ_PAYLOAD_IS_DYNAMICf;    

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_PAYLOAD].reg   = PPDB_B_LARGE_EM_CPU_REQUEST_REQUESTr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_PAYLOAD].field = LARGE_EM_REQ_PAYLOADf;    

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_STAMP].reg   = PPDB_B_LARGE_EM_CPU_REQUEST_REQUESTr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_STAMP].field = LARGE_EM_REQ_STAMPf;    

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_COMMAND].reg   = PPDB_B_LARGE_EM_CPU_REQUEST_REQUESTr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_COMMAND].field = LARGE_EM_REQ_COMMANDf;    

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_PART_OF_LAG].reg   = PPDB_B_LARGE_EM_CPU_REQUEST_REQUESTr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_PART_OF_LAG].field = LARGE_EM_REQ_PART_OF_LAGf;    

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_MFF_IS_KEY].reg   = PPDB_B_LARGE_EM_CPU_REQUEST_REQUESTr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_MFF_IS_KEY].field = LARGE_EM_REQ_MFF_IS_KEYf;    

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_MFF_KEY].reg   = PPDB_B_LARGE_EM_CPU_REQUEST_REQUESTr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_MFF_KEY].field = LARGE_EM_REQ_MFF_KEYf;    

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_REASON].reg   = PPDB_B_LARGE_EM_CPU_REQUEST_REQUESTr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_REASON].field = LARGE_EM_REQ_REASONf;    

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_SUCCESS].reg   = PPDB_B_LARGE_EM_CPU_REQUEST_REQUESTr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_MACT_CPU_REQUEST_REQUEST_MACT_REQ_SUCCESS].field = LARGE_EM_REQ_SUCCESSf;    

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_ISA_INTERRUPT_REGISTER_ONE_ISA_MANAGEMENT_COMPLETED].reg   = SOC_IS_QAX(unit) ?    PPDB_A_ISEM_INTERRUPT_REGISTER_ONEr :
                                                                                                                   SOC_IS_JERICHO(unit)? IHB_ISEM_INTERRUPT_REGISTER_ONEr :
                                                                                                                                         IHP_ISA_INTERRUPT_REGISTER_ONEr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_ISA_INTERRUPT_REGISTER_ONE_ISA_MANAGEMENT_COMPLETED].field = SOC_IS_JERICHO(unit)? ISEM_MANAGEMENT_COMPLETEDf: ISA_MANAGEMENT_COMPLETEDf;    

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_ISB_INTERRUPT_REGISTER_ONE_ISB_MANAGEMENT_COMPLETED].reg   = SOC_IS_QAX(unit) ?    PPDB_B_ISEM_INTERRUPT_REGISTER_ONEr :
                                                                                                                   SOC_IS_JERICHO(unit)? IHB_ISEM_INTERRUPT_REGISTER_ONEr:
                                                                                                                                         IHP_ISB_INTERRUPT_REGISTER_ONEr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_ISB_INTERRUPT_REGISTER_ONE_ISB_MANAGEMENT_COMPLETED].field = SOC_IS_JERICHO(unit)? ISEM_MANAGEMENT_COMPLETEDf: ISB_MANAGEMENT_COMPLETEDf;    

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_ISA_MANAGEMENT_UNIT_FAILURE_ISA_MNGMNT_UNIT_FAILURE_VALID].reg   = SOC_IS_QAX(unit) ?    PPDB_A_ISEM_MANAGEMENT_UNIT_FAILUREr :
                                                                                                                         SOC_IS_JERICHO(unit)? IHB_ISEM_MANAGEMENT_UNIT_FAILUREr:
                                                                                                                         IHP_ISA_MANAGEMENT_UNIT_FAILUREr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_ISA_MANAGEMENT_UNIT_FAILURE_ISA_MNGMNT_UNIT_FAILURE_VALID].field = SOC_IS_JERICHO(unit)? ISEM_MNGMNT_UNIT_FAILURE_VALIDf: ISA_MNGMNT_UNIT_FAILURE_VALIDf;

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_ISB_MANAGEMENT_UNIT_FAILURE_ISB_MNGMNT_UNIT_FAILURE_VALID].reg   = SOC_IS_QAX(unit) ?    PPDB_B_ISEM_MANAGEMENT_UNIT_FAILUREr :
                                                                                                                         SOC_IS_JERICHO(unit)? IHB_ISEM_MANAGEMENT_UNIT_FAILUREr:
                                                                                                                         IHP_ISB_MANAGEMENT_UNIT_FAILUREr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_ISB_MANAGEMENT_UNIT_FAILURE_ISB_MNGMNT_UNIT_FAILURE_VALID].field = SOC_IS_JERICHO(unit)? ISEM_MNGMNT_UNIT_FAILURE_VALIDf: ISB_MNGMNT_UNIT_FAILURE_VALIDf;

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_ISA_MANAGEMENT_UNIT_FAILURE_ISA_MNGMNT_UNIT_FAILURE_REASON].reg   = SOC_IS_QAX(unit) ?    PPDB_A_ISEM_MANAGEMENT_UNIT_FAILUREr :
                                                                                                                          SOC_IS_JERICHO(unit)? IHB_ISEM_MANAGEMENT_UNIT_FAILUREr:
                                                                                                                          IHP_ISA_MANAGEMENT_UNIT_FAILUREr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_ISA_MANAGEMENT_UNIT_FAILURE_ISA_MNGMNT_UNIT_FAILURE_REASON].field = SOC_IS_JERICHO(unit)? ISEM_MNGMNT_UNIT_FAILURE_REASONf: ISA_MNGMNT_UNIT_FAILURE_REASONf;

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_ISB_MANAGEMENT_UNIT_FAILURE_ISB_MNGMNT_UNIT_FAILURE_REASON].reg   = SOC_IS_QAX(unit) ?    PPDB_B_ISEM_MANAGEMENT_UNIT_FAILUREr :
                                                                                                                          SOC_IS_JERICHO(unit)? IHB_ISEM_MANAGEMENT_UNIT_FAILUREr:
                                                                                                                          IHP_ISB_MANAGEMENT_UNIT_FAILUREr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_ISB_MANAGEMENT_UNIT_FAILURE_ISB_MNGMNT_UNIT_FAILURE_REASON].field = SOC_IS_JERICHO(unit)? ISEM_MNGMNT_UNIT_FAILURE_REASONf: ISB_MNGMNT_UNIT_FAILURE_REASONf;

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_KAPS_IBC_COMMAND_WORD_BLKID].reg   = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_COMMAND_WORDr : INVALIDr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_KAPS_IBC_COMMAND_WORD_BLKID].field = BLKIDf;

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_KAPS_IBC_COMMAND_WORD_CMD].reg   = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_COMMAND_WORDr : INVALIDr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_KAPS_IBC_COMMAND_WORD_CMD].field = CMDf;

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_KAPS_IBC_COMMAND_WORD_FUNC].reg   = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_COMMAND_WORDr : INVALIDr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_KAPS_IBC_COMMAND_WORD_FUNC].field = FUNCf;

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_KAPS_IBC_COMMAND_WORD_OFFSET].reg   = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_COMMAND_WORDr : INVALIDr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_KAPS_IBC_COMMAND_WORD_OFFSET].field = OFFSETf;

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_KAPS_IBC_RESPONSE_WORD_BLKID].reg   = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_RESPONSE_WORDr : INVALIDr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_KAPS_IBC_RESPONSE_WORD_BLKID].field = BLKIDf;

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_KAPS_IBC_RESPONSE_WORD_RSP].reg   = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_RESPONSE_WORDr : INVALIDr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_KAPS_IBC_RESPONSE_WORD_RSP].field = RSPf;

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_KAPS_IBC_RESPONSE_WORD_FUNC].reg   = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_RESPONSE_WORDr : INVALIDr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_KAPS_IBC_RESPONSE_WORD_FUNC].field = FUNCf;

    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_KAPS_IBC_RESPONSE_WORD_STATUS].reg   = SOC_IS_JERICHO_AND_BELOW(unit)   && SOC_IS_JERICHO(unit) ? KAPS_IBC_RESPONSE_WORDr : INVALIDr;
    g_fast_field_info_table[unit][ARAD_FAST_FIELD_IHP_KAPS_IBC_RESPONSE_WORD_STATUS].field = STATUSf;
}

void
    arad_fast_mem_value_init(int unit)
{
    g_fast_mem_info_table[unit][ARAD_FAST_MEM_TYPE_IHP_PP_PORT_INFO_PP_PORT_OUTER_HEADER_START].mem   = IHP_PP_PORT_INFOm;
    g_fast_mem_info_table[unit][ARAD_FAST_MEM_TYPE_IHP_PP_PORT_INFO_PP_PORT_OUTER_HEADER_START].field = PP_PORT_OUTER_HEADER_STARTf;
}


void
    arad_fast_reg_access_init(int unit)
{
    int     addr, block;
    uint8   at;
    int     i;
    int port = REG_PORT_ANY;

    arad_fast_reg_value_init(unit);

    for (i = 0; i < ARAD_FAST_REG_TYPE_LAST; i++) {
        if (g_fast_reg_info_table[unit][i].reg != INVALIDr) {
            port = REG_PORT_ANY;

            if(SOC_IS_JERICHO(unit))
            {
                if((i == ARAD_FAST_REG_IHP_ISA_INTERRUPT_REGISTER) || (i == ARAD_FAST_REG_IHP_ISA_MANAGEMENT_UNIT_FAILURE)){
                    port = 0;
                }
                if((i == ARAD_FAST_REG_IHP_ISB_INTERRUPT_REGISTER) || (i == ARAD_FAST_REG_IHP_ISB_MANAGEMENT_UNIT_FAILURE)){
                    port = 1;
                }
            }
            addr = soc_reg_addr_get(unit, g_fast_reg_info_table[unit][i].reg, port, g_fast_reg_info_table[unit][i].index, SOC_REG_ADDR_OPTION_WRITE, &block, &at);
            g_fast_reg_info_table[unit][i].at         = at;
            g_fast_reg_info_table[unit][i].reg_addr   = addr;
            g_fast_reg_info_table[unit][i].block      = block;
        }
    }    
}


void
    arad_fast_field_access_init(int unit)
{
    int     i;
    soc_field_info_t *finfop;

    arad_fast_field_value_init(unit);

    for (i = 0; i < ARAD_FAST_FIELD_TYPE_LAST; i++) {
        if (g_fast_field_info_table[unit][i].reg != INVALIDr) {

            SOC_FIND_FIELD(g_fast_field_info_table[unit][i].field,SOC_REG_INFO(unit, g_fast_field_info_table[unit][i].reg).fields,SOC_REG_INFO(unit, g_fast_field_info_table[unit][i].reg).nFields,finfop);
            g_fast_field_info_table[unit][i].bp  = finfop->bp;
            g_fast_field_info_table[unit][i].len = finfop->len;
        }
    }        
}


void
    arad_fast_memory_access_init(int unit)
{
    soc_field_info_t    *fieldinfo;
    soc_mem_info_t      *meminfo;
    int                 i;

    arad_fast_mem_value_init(unit);

    for (i = 0; i < ARAD_FAST_MEM_TYPE_LAST; i++) {
        meminfo = &SOC_MEM_INFO(unit, g_fast_mem_info_table[unit][i].mem);
        SOC_FIND_FIELD(g_fast_mem_info_table[unit][i].field, meminfo->fields, meminfo->nFields, fieldinfo);

        if (NULL == fieldinfo) {
            assert(SOC_MEM_IS_VALID(unit, g_fast_mem_info_table[unit][i].mem));
        } else {
            g_fast_mem_info_table[unit][i].bp      = fieldinfo->bp;
            g_fast_mem_info_table[unit][i].len     = fieldinfo->len;
            g_fast_mem_info_table[unit][i].flags   = fieldinfo->flags;
            g_fast_mem_info_table[unit][i].meminfo = meminfo;
        }
    }
}


void
    arad_fast_regs_and_fields_access_init(int unit)
{
    arad_fast_reg_access_init(unit);
    arad_fast_field_access_init(unit);
    
}


int
arad_fast_reg_get(int unit, soc_reg_t reg, int acc_type, int addr, int block, soc_reg_above_64_val_t data)
{
    uint64 data64;
    uint32 data32;
    uint32 rc;
    int reg_size;
    
    SOC_REG_ABOVE_64_CLEAR(data);

#ifdef CRASH_RECOVERY_SUPPORT

    
    if (SOC_IS_DONE_INIT(unit))
    {
        if (BCM_UNIT_DO_HW_READ_WRITE(unit))
        {
            if(Hw_Log_List[unit].Access_cb.fast_reg_get)
            {
                return Hw_Log_List[unit].Access_cb.fast_reg_get(unit, reg, acc_type, addr, block, data);
            }
        }
    }
#endif 


    if (SOC_REG_IS_ABOVE_64(unit, reg)) 
    {
        reg_size = SOC_REG_ABOVE_64_INFO(unit, reg).size;
        if (!soc_feature(unit, soc_feature_new_sbus_format)) {
            block = ((addr >> SOC_BLOCK_BP) & 0xf) | (((addr >> SOC_BLOCK_MSB_BP) & 0x3) << 4);
        }
        rc = soc_direct_reg_get(unit, block, addr, reg_size, data);
#ifdef BROADCOM_DEBUG
        if (bsl_check(bslLayerSoc, bslSourceReg, bslSeverityNormal, unit)) {
            _soc_reg_above_64_debug(unit, "fast_read", block, addr, data);
        }
#endif 

        return rc;
    } 
    else if (SOC_REG_IS_64(unit, reg)) {
        COMPILER_64_SET(data64, data[1], data[0]);
        rc = _soc_reg64_get(unit, block, acc_type, addr, (uint64*)(&data64));        
        data[0] = COMPILER_64_LO(data64);
        data[1] = COMPILER_64_HI(data64);
#ifdef BROADCOM_DEBUG
        if (bsl_check(bslLayerSoc, bslSourceReg, bslSeverityNormal, unit)) {
            _soc_reg_debug(unit, 64, "fast_read", addr, 0, *data);
        }
#endif 

        return rc;
    } 
    else {                
        rc = _soc_reg32_get(unit, block, acc_type, addr, &data32);
        data[0] = data32;
#ifdef BROADCOM_DEBUG
        if (bsl_check(bslLayerSoc, bslSourceReg, bslSeverityNormal, unit)) {
            _soc_reg_debug(unit, 32, "fast_read", addr, 0, *data);
        }
#endif 
        return rc;
    }
}

int
    arad_fast_reg_set(int unit, soc_reg_t reg, int acc_type, int addr, int block, soc_reg_above_64_val_t data)
{
    int reg_size;
    uint64 data64;

#ifdef CRASH_RECOVERY_SUPPORT

    if (SOC_IS_DONE_INIT(unit))
    {
        if (BCM_UNIT_DO_HW_READ_WRITE(unit))
        {
            if(Hw_Log_List[unit].Access_cb.fast_reg_set)
            {
                return Hw_Log_List[unit].Access_cb.fast_reg_set(unit, reg, acc_type, addr, block, data);
            }
        }
    }
#endif 
    
    if (SOC_REG_IS_ABOVE_64(unit, reg)) 
    {
        reg_size = SOC_REG_ABOVE_64_INFO(unit, reg).size;
#ifdef BROADCOM_DEBUG
        if (bsl_check(bslLayerSoc, bslSourceReg, bslSeverityNormal, unit)) {
            _soc_reg_above_64_debug(unit, "fast_write", block, addr, data);
        }
#endif 

        return soc_direct_reg_set(unit, block, addr, reg_size, data);
    } 
    else if (SOC_REG_IS_64(unit, reg)) {
        COMPILER_64_SET(data64, data[1], data[0]);
#ifdef BROADCOM_DEBUG
        if (bsl_check(bslLayerSoc, bslSourceReg, bslSeverityNormal, unit)) {
            _soc_reg_debug(unit, 64, "fast_write", addr, 0, *data);
        }
#endif 

        return _soc_reg64_set(unit, block, acc_type, addr, data64);
    } 
    else {
        COMPILER_64_SET(data64, 0, data[0]);
#ifdef BROADCOM_DEBUG
        if (bsl_check(bslLayerSoc, bslSourceReg, bslSeverityNormal, unit)) {
            _soc_reg_debug(unit, 32, "fast_write", addr, 0, *data);
        }
#endif 

        return _soc_reg32_set(unit, block, acc_type, addr, COMPILER_64_LO(data64));
    }

}


void
    arad_fast_mem_field_set(int bp, int len,uint32 flags, soc_mem_info_t *meminfo, uint32 *entbuf, uint32 *fldbuf)
{
    int  i, wp;
    uint32  mask;

    if (flags & SOCF_LE) {
        wp = bp / 32;
        bp = bp & (32 - 1);
        i = 0;

        for (; len > 0; len -= 32) {
            if (bp) {
                if (len < 32) {
                    mask = (1 << len) - 1;
                } else {
                    mask = -1;
                }

                entbuf[FIX_MEM_ORDER_E(wp, meminfo)] &= ~(mask << bp);
                entbuf[FIX_MEM_ORDER_E(wp++, meminfo)] |= fldbuf[i] << bp;
                if (len > (32 - bp)) {
                    entbuf[FIX_MEM_ORDER_E(wp, meminfo)] &= ~(mask >> (32 - bp));
                    entbuf[FIX_MEM_ORDER_E(wp, meminfo)] |=
                        fldbuf[i] >> (32 - bp) & ((1 << bp) - 1);
                }
            } else {
                if (len < 32) {
                    mask = (1 << len) - 1;
                    entbuf[FIX_MEM_ORDER_E(wp, meminfo)] &= ~mask;
                    entbuf[FIX_MEM_ORDER_E(wp++, meminfo)] |= fldbuf[i] << bp;
                } else {
                    entbuf[FIX_MEM_ORDER_E(wp++, meminfo)] = fldbuf[i];
                }
            }

            i++;
        }
    } else {                           
        while (len > 0) {
            len--;
            entbuf[FIX_MEM_ORDER_E(bp / 32, meminfo)] &= ~(1 << (bp & (32-1)));
            entbuf[FIX_MEM_ORDER_E(bp / 32, meminfo)] |=
                (fldbuf[len / 32] >> (len & (32-1)) & 1) << (bp & (32-1));
            bp++;
        }
    }
}


uint32*
    arad_fast_mem_field_get(int bp, int len, uint32 flags, soc_mem_info_t *meminfo, const uint32 *entbuf, uint32 *fldbuf)
{
    int                 i, wp;

    if (len == 1) {     
        wp = bp / 32;
        bp = bp & (32 - 1);
        if (entbuf[FIX_MEM_ORDER_E(wp, meminfo)] & (1<<bp)) {
            fldbuf[0] = 1;
        } else {
            fldbuf[0] = 0;
        }
        return fldbuf;
    }

    if (flags & SOCF_LE) {
        wp = bp / 32;
        bp = bp & (32 - 1);
        i = 0;

        for (; len > 0; len -= 32) {
            if (bp) {
                fldbuf[i] =
                    entbuf[FIX_MEM_ORDER_E(wp++, meminfo)] >> bp &
                    ((1 << (32 - bp)) - 1);
                if ( len > (32 - bp) ) {
                    fldbuf[i] |= entbuf[FIX_MEM_ORDER_E(wp, meminfo)] <<
                        (32 - bp);
                }
            } else {
                fldbuf[i] = entbuf[FIX_MEM_ORDER_E(wp++, meminfo)];
            }

            if (len < 32) {
                fldbuf[i] &= ((1 << len) - 1);
            }

            i++;
        }
    } else {
        i = (len - 1) / 32;

        while (len > 0) {
            assert(i >= 0);

            fldbuf[i] = 0;

            do {
                fldbuf[i] =
                    (fldbuf[i] << 1) |
                    ((entbuf[FIX_MEM_ORDER_E(bp / 32, meminfo)] >>
                      (bp & (32 - 1))) & 1);
                len--;
                bp++;
            } while (len & (32 - 1));

            i--;
        }
    }

    return fldbuf;
}



uint32
  arad_polling(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN sal_usecs_t time_out,
    SOC_SAND_IN int32    min_polls,
    SOC_SAND_IN soc_reg_t   reg,
    SOC_SAND_IN int32    port,
    SOC_SAND_IN int32    index,
    SOC_SAND_IN soc_field_t field,
    SOC_SAND_IN uint32    expected_value
  )
{
  uint32
    res = SOC_SAND_OK;
  soc_reg_above_64_val_t
    reg_above_64,
    field_above_64;
  uint64
    reg_64;
  uint32
    reg_32;
  soc_timeout_t
    to;
  uint32
    result = ~expected_value;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_POLLING);

#ifdef CRASH_RECOVERY_SUPPORT
  
  if (SOC_IS_DONE_INIT(unit))
  {
      if (BCM_UNIT_DO_HW_READ_WRITE(unit))
      {
          if(Hw_Log_List[unit].Access_cb.polling)
          {
              return Hw_Log_List[unit].Access_cb.polling(
                   unit,
                   time_out,
                   min_polls,
                   reg,
                   port,
                   index,
                   field,
                   expected_value );
          }
      }
  }
#endif 

  soc_timeout_init(&to, time_out, min_polls);
  for(;;)
  {
    if(SOC_REG_IS_64(unit, reg)) {
      COMPILER_64_ZERO(reg_64);
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 12, exit, soc_reg64_get(unit, reg, port, index, &reg_64));
      result = soc_reg64_field32_get(unit, reg, reg_64, field);
    }
    else if(SOC_REG_IS_ABOVE_64(unit, reg)) {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 14, exit, soc_reg_above_64_get(unit, reg, port, index, reg_above_64));
      soc_reg_above_64_field_get(unit, reg, reg_above_64, field, field_above_64);
      SHR_BITCOPY_RANGE(&result, 0, field_above_64, 0, 32);
    }
    else {
        
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_reg32_get(unit, reg, port, index, &reg_32));
        result = soc_reg_field_get(unit, reg, reg_32, field);
    }
    
#ifdef PLISIM
    if (SAL_BOOT_PLISIM) {
        result = expected_value;
    }
#endif
    if(result == expected_value) {
        break;
    }
    if (soc_timeout_check(&to)) {
      LOG_ERROR(BSL_LS_SOC_REG,(BSL_META("Error polling register: %d field: %d \n"), reg, field));
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 20, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_polling()", time_out, min_polls);
}


uint32
  soc_dpp_polling(
    int         unit,
    sal_usecs_t time_out,
    int32       min_polls,
    soc_reg_t   reg,
    int32       port,
    int32       index,
    soc_field_t field,
    uint32      expected_value
  )
{
  int    soc_sand_rv = SOC_E_NONE;

  SOCDNX_INIT_FUNC_DEFS;

  if (!SOC_REG_IS_VALID(unit,reg) || !SOC_REG_FIELD_VALID(unit,reg,field)) {
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Cant poll reg %s: \n."),SOC_REG_NAME(unit,reg)));
  }
  soc_sand_rv = arad_polling(
        unit,
        time_out,
        min_polls,
        reg,
        port,
        index,
        field,
        expected_value
      );
      SOCDNX_IF_ERR_EXIT(handle_sand_result(soc_sand_rv));

exit:
  SOCDNX_FUNC_RETURN;
}


uint32
  arad_mem_polling(
    SOC_SAND_IN int   unit,
    SOC_SAND_IN int   core_block,
    SOC_SAND_IN sal_usecs_t time_out,
    SOC_SAND_IN int32    min_polls,
    SOC_SAND_IN soc_mem_t mem,
    SOC_SAND_IN uint32  buff_off,
    SOC_SAND_IN uint32   array_type,
    SOC_SAND_IN soc_field_t field,
    SOC_SAND_IN uint32    expected_value
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32 *dma_buf = NULL;
  uint32 *buf_ptr;
  soc_timeout_t
    to;
  uint32
    result = ~expected_value;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  ARAD_ALLOC(dma_buf, uint32, array_type*soc_mem_entry_bytes(unit, mem)/sizeof(uint32), "arad_mem_polling.dma_buf");

  soc_timeout_init(&to, time_out, min_polls);
  for(;;)
  {
    if(SOC_MEM_IS_ARRAY(unit, mem)) {
        res = soc_mem_array_read_range(unit, mem, 0, core_block, 0, array_type, dma_buf);       
        SOC_SAND_IF_ERR_EXIT(res);
        buf_ptr = dma_buf + buff_off;
        result = soc_mem_field32_get(unit, mem, buf_ptr, field); 
    }
    else {
        goto exit;
    }
    
#ifdef PLISIM
    if (SAL_BOOT_PLISIM) {
        result = expected_value;
    }
#endif
    if(result == expected_value) {
        break;
    }
    if (soc_timeout_check(&to)) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 20, exit);
    }
  }

exit:
  ARAD_FREE(dma_buf);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_polling()", time_out, min_polls);
}


#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif 

