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


#define SOC_QAX_SRAM_NOF_SRAM_BUFFERS (32768)
#define SOC_QAX_SRAM_NOF_DROP_PROFILES (4)

#define SOC_QAX_SRAM_DROP_PRIORITY_HIGH (0x0)
#define SOC_QAX_SRAM_DROP_PRIORITY_MID  (0x2)
#define SOC_QAX_SRAM_DROP_PRIORITY_LOW  (0x3)

#define SOC_QAX_SRAM_DROP_KEY_PROFILE_SHIFT                 (0)
#define SOC_QAX_SRAM_DROP_KEY_PROFILE_SIZE                  (2)
#define SOC_QAX_SRAM_DROP_KEY_PROFILE_MASK                  ((1<<SOC_QAX_SRAM_DROP_KEY_PROFILE_SIZE)-1)
#define SOC_QAX_SRAM_DROP_KEY_TC_SHIFT                      (SOC_QAX_SRAM_DROP_KEY_PROFILE_SHIFT + SOC_QAX_SRAM_DROP_KEY_PROFILE_SIZE)
#define SOC_QAX_SRAM_DROP_KEY_TC_SIZE                       (3)
#define SOC_QAX_SRAM_DROP_KEY_TC_MASK                       ((1<<SOC_QAX_SRAM_DROP_KEY_TC_SIZE)-1)
#define SOC_QAX_SRAM_DROP_KEY_DP_SHIFT                      (SOC_QAX_SRAM_DROP_KEY_TC_SHIFT + SOC_QAX_SRAM_DROP_KEY_TC_SIZE)
#define SOC_QAX_SRAM_DROP_KEY_DP_SIZE                       (2)
#define SOC_QAX_SRAM_DROP_KEY_DP_MASK                       ((1<<SOC_QAX_SRAM_DROP_KEY_DP_SIZE)-1)

#define SOC_QAX_SRAM_DROP_KEY_SET(profile, tc, dp) \
        (((profile & SOC_QAX_SRAM_DROP_KEY_PROFILE_MASK) << SOC_QAX_SRAM_DROP_KEY_PROFILE_SHIFT) | \
        ((tc & SOC_QAX_SRAM_DROP_KEY_TC_MASK) << SOC_QAX_SRAM_DROP_KEY_TC_SHIFT) | \
        ((dp & SOC_QAX_SRAM_DROP_KEY_DP_MASK) << SOC_QAX_SRAM_DROP_KEY_DP_SHIFT))


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

int soc_qax_sram_drop_threshold_set(
        int unit,
        int priority,
        uint32 value)
{
    uint64 reg64_val;
    soc_field_t field;
    SOCDNX_INIT_FUNC_DEFS;

    if (value >= SOC_QAX_SRAM_NOF_SRAM_BUFFERS)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Illegal threshold value %d"), value));
    }

    switch(priority)
    {
        case 0:
            field = PACKET_REJECT_TH_3f;
            break;
        case 1:
            field = PACKET_REJECT_TH_2f;
            break;
        case 2:
            field = PACKET_REJECT_TH_1f;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Illegal priority %d"), priority));
    }

    SOCDNX_IF_ERR_EXIT(READ_SPB_PACKET_REJECT_THr(unit, SOC_CORE_ALL, &reg64_val));
    soc_reg64_field32_set(unit, SPB_PACKET_REJECT_THr, &reg64_val, field, value);
    SOCDNX_IF_ERR_EXIT(WRITE_SPB_PACKET_REJECT_THr(unit, SOC_CORE_ALL, reg64_val));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_sram_drop_threshold_get(
        int unit,
        int priority,
        uint32 *value)
{
    uint64 reg64_val;
    soc_field_t field;
    SOCDNX_INIT_FUNC_DEFS;

    switch(priority)
    {
        case 0:
            field = PACKET_REJECT_TH_3f;
            break;
        case 1:
            field = PACKET_REJECT_TH_2f;
            break;
        case 2:
            field = PACKET_REJECT_TH_1f;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Illegal priority %d"), priority));
    }

    SOCDNX_IF_ERR_EXIT(READ_SPB_PACKET_REJECT_THr(unit, SOC_CORE_ALL, &reg64_val));
    *value = soc_reg64_field32_get(unit, SPB_PACKET_REJECT_THr, reg64_val, field);

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_sram_drop_reassembly_context_profile_map_set(
    int unit,
    int core,
    uint32 reassembly_context,
    uint32 profile_id)
{
    uint32 data;
    SOCDNX_INIT_FUNC_DEFS;

    if (profile_id >= SOC_QAX_SRAM_NOF_DROP_PROFILES)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Illegal profile id %d"), profile_id));
    }

    SOCDNX_IF_ERR_EXIT(READ_SPB_CONTEXT_PROFILE_MAPm(unit, SPB_BLOCK(unit, core), reassembly_context, &data));
    soc_mem_field32_set(unit, SPB_CONTEXT_PROFILE_MAPm, &data, CONTEXT_PROFILEf, profile_id);
    SOCDNX_IF_ERR_EXIT(WRITE_SPB_CONTEXT_PROFILE_MAPm(unit, SPB_BLOCK(unit, core), reassembly_context, &data));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_sram_drop_reassembly_context_profile_map_get(
    int unit,
    int core,
    uint32 reassembly_context,
    uint32 *profile_id)
{
    uint32 data;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_SPB_CONTEXT_PROFILE_MAPm(unit, SPB_BLOCK(unit, core), reassembly_context, &data));
    *profile_id = soc_mem_field32_get(unit, SPB_CONTEXT_PROFILE_MAPm, &data, CONTEXT_PROFILEf);

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_sram_drop_profile_priority_map_set(
    int unit,
    uint32 profile_id,
    uint32 tc,
    uint32 dp,
    uint32 priority)
{
    uint32 data;
    uint32 key;
    SOCDNX_INIT_FUNC_DEFS;

    if (profile_id >= SOC_QAX_SRAM_NOF_DROP_PROFILES)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Illegal profile id %d"), profile_id));
    }

    switch(priority)
    {
        case 0:
            data = SOC_QAX_SRAM_DROP_PRIORITY_HIGH;
            break;
        case 1:
            data = SOC_QAX_SRAM_DROP_PRIORITY_MID;
            break;
        case 2:
            data = SOC_QAX_SRAM_DROP_PRIORITY_LOW;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Illegal priority %d"), priority));
    }

    key = SOC_QAX_SRAM_DROP_KEY_SET(profile_id, tc, dp);
    SOCDNX_IF_ERR_EXIT(WRITE_SPB_PACKET_REJECT_CFGm(unit, SPB_BLOCK(unit, BCM_CORE_ALL), key, &data));

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_sram_drop_profile_priority_map_get(
    int unit,
    uint32 profile_id,
    uint32 tc,
    uint32 dp,
    uint32 *priority)
{
    uint32 data;
    uint32 key;
    SOCDNX_INIT_FUNC_DEFS;

    if (profile_id >= SOC_QAX_SRAM_NOF_DROP_PROFILES)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Illegal profile id %d"), profile_id));
    }

    key = SOC_QAX_SRAM_DROP_KEY_SET(profile_id, tc, dp);
    SOCDNX_IF_ERR_EXIT(READ_SPB_PACKET_REJECT_CFGm(unit, SPB_BLOCK(unit, BCM_CORE_ALL), key, &data));

    switch(data)
    {
        case SOC_QAX_SRAM_DROP_PRIORITY_HIGH:
            *priority = 0;
            break;
        case SOC_QAX_SRAM_DROP_PRIORITY_MID:
            *priority = 1;
            break;
        case SOC_QAX_SRAM_DROP_PRIORITY_LOW:
            *priority = 2;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Illegal priority configuration %d"), data));
    }

exit:
    SOCDNX_FUNC_RETURN;
}
