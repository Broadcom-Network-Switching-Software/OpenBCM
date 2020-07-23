/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT


#include <shared/bsl.h>


#include <sal/appl/sal.h>


#include <soc/error.h>
#include <soc/dcmn/error.h>
#include <soc/sand/sand_mem.h>


#include <soc/dpp/QAX/qax_sram.h>
#include <soc/dpp/QAX/qax_link_bonding.h>
#include <soc/dpp/drv.h>

int soc_qax_sram_conf_set(int unit)
{

    uint32 mem_entry[SOC_MAX_MEM_WORDS] = {0};
    uint32 reg32_val;
    uint64 reg64_val;

    SOCDNX_INIT_FUNC_DEFS;

    
    COMPILER_64_ZERO(reg64_val);
    SOCDNX_IF_ERR_EXIT( READ_SPB_STATIC_CONFIGURATIONr(unit, SOC_CORE_ALL, &reg64_val));
    soc_reg64_field32_set(unit, SPB_STATIC_CONFIGURATIONr, &reg64_val, DDC_2_NUM_OF_BUFF_THf, 4);
    soc_reg64_field32_set(unit, SPB_STATIC_CONFIGURATIONr, &reg64_val, DEL_NUM_OF_BUFF_AF_THf, 0x7d0);
    soc_reg64_field32_set(unit, SPB_STATIC_CONFIGURATIONr, &reg64_val, DEL_NUM_OF_PKT_AF_THf, 0x64);
    soc_reg64_field32_set(unit, SPB_STATIC_CONFIGURATIONr, &reg64_val, PTC_PDQ_RDY_TH_WORDf, 8);
    soc_reg64_field32_set(unit, SPB_STATIC_CONFIGURATIONr, &reg64_val, STOP_IRE_WHEN_FBC_EMPTYf, 1);
    soc_reg64_field32_set(unit, SPB_STATIC_CONFIGURATIONr, &reg64_val, STOP_IRE_THf, 5);
    SOCDNX_IF_ERR_EXIT( WRITE_SPB_STATIC_CONFIGURATIONr(unit, SOC_CORE_ALL, reg64_val));

    
    sal_memset(mem_entry, 0, sizeof(mem_entry));
    soc_mem_field32_set(unit, SPB_CONTEXT_MRUm, mem_entry, MAX_SIZEf, 0x2fff);
    soc_mem_field32_set(unit, SPB_CONTEXT_MRUm, mem_entry, MAX_ORG_SIZEf, 0x2fff);
    soc_mem_field32_set(unit, SPB_CONTEXT_MRUm, mem_entry, MIN_ORG_SIZEf, 0x20);
    soc_mem_field32_set(unit, SPB_CONTEXT_MRUm, mem_entry, MIN_SIZEf, 0x20);
    SOCDNX_IF_ERR_EXIT( sand_fill_table_with_entry( unit, SPB_CONTEXT_MRUm, MEM_BLOCK_ALL, mem_entry));

    
    SOCDNX_IF_ERR_EXIT( READ_SPB_DYNAMIC_CONFIGURATIONr(unit, SOC_CORE_ALL,&reg32_val));
    soc_reg_field_set(unit, SPB_DYNAMIC_CONFIGURATIONr, &reg32_val, SOC_IS_QUX(unit)?MAX_BUFFERS_THRESHOLDf:FIELD_1_6f, 0x2e);
    soc_reg_field_set(unit, SPB_DYNAMIC_CONFIGURATIONr, &reg32_val, FBC_INITf, 1);
    SOCDNX_IF_ERR_EXIT( WRITE_SPB_DYNAMIC_CONFIGURATIONr(unit, SOC_CORE_ALL,reg32_val));
    SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, SPB_DYNAMIC_CONFIGURATIONr, REG_PORT_ANY, FBC_INITf, 0));

    
#ifdef BCM_QAX_SUPPORT
    if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->link_bonding_enable) {
        SOCDNX_IF_ERR_EXIT(qax_lb_ing_init(unit));
    }
#endif

    
    SOCDNX_IF_ERR_EXIT( READ_SPB_PACKET_REJECT_THr(unit, SOC_CORE_ALL, &reg64_val));
    soc_reg64_field32_set(unit, SPB_PACKET_REJECT_THr, &reg64_val, PACKET_REJECT_TH_1f, 300);
    soc_reg64_field32_set(unit, SPB_PACKET_REJECT_THr, &reg64_val, PACKET_REJECT_TH_2f, 200);
    soc_reg64_field32_set(unit, SPB_PACKET_REJECT_THr, &reg64_val, PACKET_REJECT_TH_3f, 100);
    SOCDNX_IF_ERR_EXIT( WRITE_SPB_PACKET_REJECT_THr(unit, SOC_CORE_ALL, reg64_val));


exit:
    SOCDNX_FUNC_RETURN;
}
