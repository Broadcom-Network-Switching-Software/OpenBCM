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
#include <shared/bitop.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>


#include <sal/appl/sal.h>


#include <soc/debug.h>
#include <soc/error.h>
#include <soc/iproc.h>
#include <soc/mem.h>
#include <soc/mcm/memregs.h>


#include <soc/dpp/drv.h>
#include <soc/sand/sand_mem.h>
#include <soc/dpp/multicast_imp.h>
#include <soc/dpp/PPC/ppc_api_lif.h>
#include <soc/sand/sand_mem.h>


#include <soc/dpp/JER/jer_tbls.h>
#include <soc/dpp/JER/jer_fabric.h>

 
#include <soc/dpp/ARAD/arad_init.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_ingress_packet_queuing.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_mirror.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap_access.h>





#define JER_MBMP_SET_EXCLUDED(_mem)      SHR_BITSET(jer_excluded_mems_bmap_p, _mem)
#define JER_MBMP_IS_EXCLUDED(_mem)       SHR_BITGET( jer_excluded_mems_bmap_p, _mem)



static soc_mem_t jer_tbls_88375_excluded_mem_list[] = {
    EDB_EEDB_BANKm,
    EDB_EEDB_TOP_BANKm,
    IRR_MCDBm,
    IRR_QUEUE_IS_OCB_COMMITTEDm,
    EDB_LINK_LAYER_OR_ARP_NEW_FORMATm,
    EDB_PROTECTION_ENTRYm,
    EDB_TRILL_FORMAT_FULL_ENTRYm,
    EDB_TRILL_FORMAT_HALF_ENTRYm,
    ILKN_PMH_PORT_0_CPU_ACCESSm,
    ILKN_PMH_PORT_1_CPU_ACCESSm,
    ILKN_PML_PORT_0_CPU_ACCESSm,
    ILKN_PML_PORT_1_CPU_ACCESSm,
    SCH_SCHEDULER_INITm,
    SER_ACC_TYPE_MAPm,
    SER_MEMORYm,
    SER_RESULT_0m,
    SER_RESULT_1m,
    SER_RESULT_DATA_0m,
    SER_RESULT_DATA_1m,
    SER_RESULT_EXPECTED_0m,
    SER_RESULT_EXPECTED_1m,
    EGQ_VSI_MEMBERSHIPm,
    KAPS_TCMm,
    EDB_MAP_OUTLIF_TO_DSPm, 
    BRDC_FSRD_FSRD_WL_EXT_MEMm,
    FSRD_FSRD_WL_EXT_MEMm,
    XLPORT_WC_UCMEM_DATAm,
    CLPORT_WC_UCMEM_DATAm,

    
    INVALIDm
};

static soc_mem_t *jer_tbls_88675_excluded_mem_list = jer_tbls_88375_excluded_mem_list;



 
uint32 soc_jer_sch_tbls_init(int unit)
{
    uint32 table_entry[128] = {0};
    int core;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);

    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "init_tbls_to_zero", !(
#ifdef PLISIM
        SAL_BOOT_PLISIM || 
#endif
        SOC_DPP_CONFIG(unit)->emulation_system))) {
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_FC_MAP_FCMm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_CH_NIF_RATES_CONFIGURATION_CNRCm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_ONE_PORT_NIF_CONFIGURATION_OPNCm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_CH_NIF_CALENDAR_CONFIGURATION_CNCCm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_FLOW_SUB_FLOW_FSFm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_FLOW_DESCRIPTOR_MEMORY_STATIC_FDMSm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_FLOW_TO_FIP_MAPPING_FFMm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry));
    }

    
    soc_mem_field32_set(unit, SCH_FLOW_TO_FIP_MAPPING_FFMm, table_entry, DEVICE_NUMBERf, SOC_TMC_MAX_FAP_ID);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, SCH_FLOW_TO_FIP_MAPPING_FFMm, table_entry));
    soc_mem_field32_set(unit, SCH_FLOW_TO_FIP_MAPPING_FFMm, table_entry, DEVICE_NUMBERf, 0);

    
    soc_mem_field32_set(unit, SCH_SCHEDULER_INITm, table_entry, SCH_INITf, 0x1);
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SCHEDULER_INITm(unit, SCH_BLOCK(unit, core), 0x0, table_entry));
    }
    soc_mem_field32_set(unit, SCH_SCHEDULER_INITm, table_entry, SCH_INITf, 0x0);

    soc_mem_field32_set(unit, SCH_PS_8P_RATES_PSRm, table_entry, PS_8P_RATES_PSRf, 128);
    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_PS_8P_RATES_PSRm, SCH_BLOCK(unit, SOC_CORE_ALL), table_entry));
    soc_mem_field32_set(unit, SCH_PS_8P_RATES_PSRm, table_entry, PS_8P_RATES_PSRf, 0);

    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_0m, SCH_BLOCK(unit, core), table_entry));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_1m, SCH_BLOCK(unit, core), table_entry));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_2m, SCH_BLOCK(unit, core), table_entry));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_3m, SCH_BLOCK(unit, core), table_entry));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_4m, SCH_BLOCK(unit, core), table_entry));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_5m, SCH_BLOCK(unit, core), table_entry));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_6m, SCH_BLOCK(unit, core), table_entry));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_7m, SCH_BLOCK(unit, core), table_entry));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_8m, SCH_BLOCK(unit, core), table_entry));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_9m, SCH_BLOCK(unit, core), table_entry));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_10m, SCH_BLOCK(unit, core), table_entry));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_11m, SCH_BLOCK(unit, core), table_entry));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_12m, SCH_BLOCK(unit, core), table_entry));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_13m, SCH_BLOCK(unit, core), table_entry));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_14m, SCH_BLOCK(unit, core), table_entry));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR_CAL_15m, SCH_BLOCK(unit, core), table_entry));
    }

    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry, WFQ_PG_0_WEIGHTf, 0x1);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry, WFQ_PG_1_WEIGHTf, 0x2);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry, WFQ_PG_2_WEIGHTf, 0x4);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry, WFQ_PG_3_WEIGHTf, 0x8);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry, WFQ_PG_4_WEIGHTf, 0x10);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry, WFQ_PG_5_WEIGHTf, 0x20);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry, WFQ_PG_6_WEIGHTf, 0x40);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry, WFQ_PG_7_WEIGHTf, 0x80);
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, SCH_BLOCK(unit, core), table_entry));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



uint32 soc_jer_irr_tbls_init(int unit)
{
    uint32 m[2]= {0};
    SOCDNX_INIT_FUNC_DEFS;
    
    if (SOC_DPP_CONFIG(unit)->arad->init.dram.nof_drams) {
        soc_mem_field32_set(unit, IRR_QUEUE_IS_OCB_COMMITTEDm, m, COMMITMENTf, 0x0); 
    } else {
        soc_mem_field32_set(unit, IRR_QUEUE_IS_OCB_COMMITTEDm, m, COMMITMENTf, 0xffffffff); 
    }
    
    if (SOC_DPP_CONFIG(unit)->emulation_system) { 
        SOCDNX_IF_ERR_EXIT(sand_fill_partial_table_with_entry(unit, IRR_QUEUE_IS_OCB_COMMITTEDm, 0, 0, MEM_BLOCK_ALL, 0, 10000, m));
    } else {
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IRR_QUEUE_IS_OCB_COMMITTEDm, MEM_BLOCK_ALL, m));
    }

    
    sal_memset(m, 0, sizeof(m)); 
    soc_mem_field32_set(unit, IRR_DESTINATION_TABLEm, m, QUEUE_NUMBERf, 0x1ffff); 
    soc_mem_field32_set(unit, IRR_DESTINATION_TABLEm, m, TC_PROFILEf, 0); 
    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IRR_DESTINATION_TABLEm, MEM_BLOCK_ANY, m)); 
    sal_memset(m, 0, sizeof(m));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32 soc_jer_mrps_tbls_init (int unit){
	int core_id;
	uint32 table_entry[4] = {0};
	SOCDNX_INIT_FUNC_DEFS;

	for(core_id = 0; core_id < SOC_DPP_CONFIG(unit)->meter.nof_meter_cores; core_id++){
		SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, MRPS_MCDA_PRFCFG_0m, MRPS_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, MRPS_MCDB_PRFCFG_0m, MRPS_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, MRPS_MCDA_PRFCFG_1m, MRPS_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, MRPS_MCDB_PRFCFG_1m, MRPS_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, MRPS_MCDA_PRFSELm, MRPS_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, MRPS_MCDB_PRFSELm, MRPS_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, MRPS_MCDA_HEADER_APPEND_SIZE_PTR_MAPm, MRPS_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, MRPS_MCDB_HEADER_APPEND_SIZE_PTR_MAPm, MRPS_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, MRPS_MCDA_IN_PP_PORT_MAPm, MRPS_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, MRPS_MCDB_IN_PP_PORT_MAPm, MRPS_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, MRPS_MCDA_IN_PP_PORT_MAP_AND_MTR_PRF_MAPm, MRPS_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, MRPS_MCDB_IN_PP_PORT_MAP_AND_MTR_PRF_MAPm, MRPS_BLOCK(unit, core_id), table_entry));
	}

exit:
    SOCDNX_FUNC_RETURN;

}

uint32 soc_jer_mrpsEm_tbls_init (int unit){
	int core_id;
	uint32 table_entry[4] = {0};
	SOCDNX_INIT_FUNC_DEFS;

	for(core_id = 0; core_id < SOC_DPP_CONFIG(unit)->meter.nof_meter_cores; core_id++){
		SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, MTRPS_EM_MCDA_PRFCFG_0m, MTRPS_EM_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, MTRPS_EM_MCDB_PRFCFG_0m, MTRPS_EM_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, MTRPS_EM_MCDA_PRFCFG_1m, MTRPS_EM_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, MTRPS_EM_MCDB_PRFCFG_1m, MTRPS_EM_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, MTRPS_EM_MCDA_PRFSELm, MTRPS_EM_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, MTRPS_EM_MCDB_PRFSELm, MTRPS_EM_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, MTRPS_EM_MCDA_HEADER_APPEND_SIZE_PTR_MAPm, MTRPS_EM_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, MTRPS_EM_MCDB_HEADER_APPEND_SIZE_PTR_MAPm, MTRPS_EM_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, MTRPS_EM_MCDA_IN_PP_PORT_MAPm, MTRPS_EM_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, MTRPS_EM_MCDB_IN_PP_PORT_MAPm, MTRPS_EM_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, MTRPS_EM_MCDA_IN_PP_PORT_MAP_AND_MTR_PRF_MAPm, MTRPS_EM_BLOCK(unit, core_id), table_entry));
		SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, MTRPS_EM_MCDB_IN_PP_PORT_MAP_AND_MTR_PRF_MAPm, MTRPS_EM_BLOCK(unit, core_id), table_entry));
	}

exit:
    SOCDNX_FUNC_RETURN;

}

uint32  soc_jer_idr_tbls_init (int unit){
	uint32 table_entry = 0;
	SOCDNX_INIT_FUNC_DEFS;

	SOCDNX_IF_ERR_EXIT(sand_fill_partial_table_with_entry(unit, IDR_ETHERNET_METER_CONFIGm,0,1, MEM_BLOCK_ANY,0,1279, &table_entry));
    
    table_entry = 1;
	SOCDNX_IF_ERR_EXIT(sand_fill_partial_table_with_entry(unit, IDR_DRAM_BUFFER_TYPEm, 0, 0, MEM_BLOCK_ANY, 48, 63, &table_entry));

exit:
    SOCDNX_FUNC_RETURN;

}

uint32 soc_jer_ire_tbls_init(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;


    SOCDNX_FUNC_RETURN;
}

uint32 soc_jer_ihp_tbls_init(int unit)
{
    int core_id;
    uint32 table_default_vtt[1] = {0xb80};
    uint32 table_default_vsi[1] = {0x4000070};
    uint32 table_default_pinfo[3] = {0x0, 0x1ee00000, 0x0};
    uint32 table_default_tos_2_cos[1] = {0x0};
    SOCDNX_INIT_FUNC_DEFS;

    for(core_id = 0; core_id < SOC_DPP_CONFIG(unit)->meter.nof_meter_cores; core_id++){
        SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, IHP_VTT_LLVPm, IHP_BLOCK(unit, core_id), table_default_vtt));
        SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, IHP_VSI_LOW_CFG_2m, IHP_BLOCK(unit, core_id), table_default_vsi));
        SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, IHP_PINFO_LLRm, IHP_BLOCK(unit, core_id), table_default_pinfo));
        SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, IHP_TOS_2_COSm, IHP_BLOCK(unit, core_id), table_default_tos_2_cos));
    }  
exit:
    SOCDNX_FUNC_RETURN;
}

uint32 soc_jer_ihb_tbls_init(int unit)
{
    uint32 res;
    uint32 vsi_low_cfg[1] = {0};
	uint32 table_entry[4] = {0};
    uint32 table_default_path_selection[1] = {0x0};

    SOCDNX_INIT_FUNC_DEFS;

    
    soc_mem_field32_set(unit, IHP_VSI_LOW_CFG_2m, vsi_low_cfg, FID_CLASSf, 7);
    res = arad_fill_table_with_entry(unit, IHP_VSI_LOW_CFG_2m, MEM_BLOCK_ANY, vsi_low_cfg);
    SOCDNX_SAND_IF_ERR_EXIT(res);

	
	SOCDNX_IF_ERR_EXIT(WRITE_IHB_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, SOC_CORE_ALL, 1));
    SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, IHB_CPU_TRAP_CODE_CTRm, IHB_BLOCK(unit, SOC_CORE_ALL), table_entry));

      
    if (SOC_IS_JERICHO_PLUS_A0(unit)) {
    	SOCDNX_IF_ERR_EXIT(WRITE_IHB_INDIRECT_WR_MASKr(unit,SOC_CORE_ALL, 0));
    	SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, IHB_FEC_PATH_SELECTm, IHB_BLOCK(unit, SOC_CORE_ALL), table_default_path_selection));
    	SOCDNX_IF_ERR_EXIT(WRITE_IHB_INDIRECT_WR_MASKr(unit,SOC_CORE_ALL, 0xffffffff));
    }    

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 soc_jer_iqm_tbls_init( int unit)
{
    uint32 table_default[SOC_MAX_MEM_WORDS] = {0};
    uint32 table_pcp_default[SOC_MAX_MEM_WORDS] = {0xaaaaaaaa};
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);

    
    
    if (SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_pcp_enable) {
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IQM_PACK_MODEm, IQM_BLOCK(unit, SOC_CORE_ALL), table_pcp_default)) ;
    } else {
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IQM_PACK_MODEm, IQM_BLOCK(unit, SOC_CORE_ALL), table_default)) ;
    }
    soc_mem_field32_set(unit, IQM_PQWQm, table_default, PQ_WEIGHTf, 2);
    soc_mem_field32_set(unit, IQM_PQWQm, table_default, PQ_AVRG_ENf, 1); 
    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IQM_PQWQm, IQM_BLOCK(unit, SOC_CORE_ALL),  table_default)); 
    sal_memset(table_default, 0, sizeof(table_default));

exit:
    SOCDNX_FUNC_RETURN;
}



uint32 soc_jer_iqmt_tbls_init(int unit)
{
    uint32 table_entry = 0;
    soc_reg_above_64_val_t table_default;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);


    
    SOCDNX_IF_ERR_EXIT(WRITE_IQMT_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));

    
    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IQMT_ING_RPT_PCMm, IQMT_BLOCK(unit), &table_entry));

    
    if (!SAL_BOOT_PLISIM && SOC_MEM_IS_VALID(unit, IQMT_PDM_0m)) {
        SOC_REG_ABOVE_64_CLEAR(table_default);
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IQMT_PDM_0m, IQMT_BLOCK(unit), table_default));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IQMT_PDM_1m, IQMT_BLOCK(unit), table_default));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IQMT_PDM_2m, IQMT_BLOCK(unit), table_default));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IQMT_PDM_3m, IQMT_BLOCK(unit), table_default));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IQMT_PDM_4m, IQMT_BLOCK(unit), table_default));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, IQMT_PDM_5m, IQMT_BLOCK(unit), table_default));
    }

    SOCDNX_IF_ERR_EXIT(WRITE_IQMT_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 0));

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_ips_tbls_init(int unit)
{
    uint32 table_entry[2] = {ARAD_IPQ_INVALID_FLOW_QUARTET};
    uint8 silent = 0;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);

    
    SOCDNX_SAND_IF_ERR_EXIT(arad_mgmt_ips_tbls_init(unit, silent));

    if (!SOC_IS_QUX(unit)) {
         
        soc_mem_field32_set(unit, IPS_FMSBYPm, table_entry, FMS_BYP_BMPf, 0x0000); 
        SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, IPS_FMSBYPm, table_entry));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 soc_jer_fcr_tbls_init(int unit)
{
    uint32 table_entry[1] = {0};

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);
    
    soc_mem_field32_set(unit, FCR_EFMS_SOURCE_PIPEm, table_entry, DATAf, 0xff); 
    SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, FCR_EFMS_SOURCE_PIPEm, FCR_BLOCK(unit), table_entry));
    soc_mem_field32_set(unit, FCR_EFMS_SOURCE_PIPEm, table_entry, DATAf, 0);

exit:
    SOCDNX_FUNC_RETURN;
}





soc_error_t
  soc_jer_ipt_tbls_init(
      SOC_SAND_IN  int                 unit
    )
{
    int i;

    uint32 fabric_priority;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);

    
    for(i=0;i<SOC_JER_FABRIC_PRIORITY_NDX_NOF ; i++) {
        uint32 is_tdm, tc;
        
        is_tdm = (i & SOC_JER_FABRIC_PRIORITY_NDX_IS_TDM_MASK) >> SOC_JER_FABRIC_PRIORITY_NDX_IS_TDM_OFFSET;
        tc = (i & SOC_JER_FABRIC_PRIORITY_NDX_TC_MASK) >> SOC_JER_FABRIC_PRIORITY_NDX_TC_OFFSET;

        if(is_tdm) {
            fabric_priority = 3;
        } else {
            
            
            
            fabric_priority = tc/3;
        }

        SOCDNX_IF_ERR_EXIT(WRITE_IPT_PRIORITY_BITS_MAPPING_2_FDTm(unit, MEM_BLOCK_ALL, i, &fabric_priority));
        SOCDNX_IF_ERR_EXIT(WRITE_IPT_TDM_BIT_MAPPING_2_FDTm(unit, MEM_BLOCK_ALL, i, &is_tdm));
    }

    exit:
      SOCDNX_FUNC_RETURN;
}



uint32 soc_jer_fdt_tbls_init(int unit)
{
    uint32 table_default[SOC_MAX_MEM_WORDS] = {0};
    uint32 pcp_config_data = 2;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);

    if ((SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_EGR_CORE_MESH_MODE) == 0 && 
         SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_pcp_enable) {
        
        SHR_BITCOPY_RANGE(table_default, 6, &pcp_config_data, 0, SOC_JER_FABRIC_PCP_LENGTH);
    } else {
        
    }

    
    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, FDT_IPT_MESH_MCm, MEM_BLOCK_ALL, table_default));

exit:
    SOCDNX_FUNC_RETURN;
}



uint32 soc_jer_egq_tbls_init(int unit)
{
    soc_reg_above_64_val_t data;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(data);
    soc_mem_field32_set(unit, EGQ_PCTm, data, CGM_PORT_PROFILEf, ARAD_EGR_PORT_THRESH_TYPE_15);
    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, EGQ_PCTm, MEM_BLOCK_ALL, data));

    
    SOC_REG_ABOVE_64_ALLONES(data);
    SOCDNX_IF_ERR_EXIT(arad_fill_table_with_entry(unit, EGQ_VSI_MEMBERSHIPm, MEM_BLOCK_ALL, data));

    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, EDB_MAP_OUTLIF_TO_DSPm, MEM_BLOCK_ALL, data)); 

    SOC_REG_ABOVE_64_CLEAR(data);
    soc_mem_field32_set(unit, EGQ_DSP_PTR_MAPm, data, OUT_TM_PORTf, ARAD_EGR_INVALID_BASE_Q_PAIR);
    SOCDNX_IF_ERR_EXIT(arad_fill_table_with_entry(unit, EGQ_DSP_PTR_MAPm, MEM_BLOCK_ALL, data));

    SOC_REG_ABOVE_64_CLEAR(data);
    soc_mem_field32_set(unit, EGQ_PER_IFC_CFGm, data, OTM_PORT_NUMBERf, ARAD_EGR_INVALID_BASE_Q_PAIR);
    SOCDNX_IF_ERR_EXIT(arad_fill_table_with_entry(unit, EGQ_PER_IFC_CFGm, MEM_BLOCK_ALL, data));

exit:
    SOCDNX_FUNC_RETURN;
}



uint32 soc_jer_epni_tbls_init(int unit)
{
    uint32 res = 0;
    uint32 tx_tag_table_entry[9] = {0};
    uint32 table_default[3] = {0x10040100, 0x04010040, 0x4010};
    uint32 mtu_table_entry[4] = {0, 0, 0xF8000000, 0x1FF}; 
    int core_id;
    uint32 table_default_data[2] = {0,0};
    uint32 table_default_remark[1] = {0xfac688};
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, EPNI_MIRROR_PROFILE_TABLEm, MEM_BLOCK_ALL, table_default));

    for(core_id = 0; core_id < SOC_DPP_CONFIG(unit)->meter.nof_meter_cores; core_id++){
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, EPNI_REMARK_IPV6_TO_EXPm, EPNI_BLOCK(unit, core_id), table_default_remark));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, EPNI_PP_COUNTER_TABLEm, EPNI_BLOCK(unit, core_id), table_default_data));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, EPNI_DSCP_EXP_TO_PCP_DEIm, EPNI_BLOCK(unit, core_id), table_default_data));
    }


    res = soc_sand_bitstream_fill(tx_tag_table_entry, 8);
    SOCDNX_IF_ERR_EXIT(res);

    res = arad_fill_table_with_entry(unit, EPNI_TX_TAG_TABLEm, MEM_BLOCK_ANY, &tx_tag_table_entry);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    res = arad_pp_eg_mirror_init_unsafe(unit);
    SOCDNX_SAND_IF_ERR_EXIT(res);

    if (SOC_MEM_IS_VALID(unit,EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm)) {
        res = arad_fill_table_with_entry(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, MEM_BLOCK_ANY, &mtu_table_entry); 
        SOCDNX_SAND_IF_ERR_EXIT(res);    
    }


exit:
    SOCDNX_FUNC_RETURN;
}

uint32 soc_jer_oamp_tbls_init(int unit)
{
    soc_reg_above_64_val_t table_default;
    uint32 write_val = 1;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(table_default);


    
    SOCDNX_IF_ERR_EXIT(WRITE_OAMP_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit , write_val));
    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, OAMP_DM_TRIGERm, MEM_BLOCK_ALL, table_default));
    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, OAMP_SAT_RX_FLOW_PARAMSm , MEM_BLOCK_ALL, table_default));
    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, OAMP_SAT_RX_FLOW_STATSm , MEM_BLOCK_ALL, table_default));
    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, OAMP_SAT_TXm , MEM_BLOCK_ALL, table_default));
    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, OAMP_SAT_TX_EVC_PARAMS_ENTRY_1m , MEM_BLOCK_ALL, table_default));
    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, OAMP_SAT_TX_EVC_PARAMS_ENTRY_2m , MEM_BLOCK_ALL, table_default));
    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, OAMP_SAT_TX_GEN_PARAMSm , MEM_BLOCK_ALL, table_default));

    write_val = 0;
    SOCDNX_IF_ERR_EXIT(WRITE_OAMP_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit , write_val));

    

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 soc_jer_ppdb_tbls_init(int unit)
{
    soc_reg_above_64_val_t table_default;
    uint32 table_entry[128] = {0};
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(table_default);

    
    SOCDNX_IF_ERR_EXIT(arad_fill_table_with_entry(unit, PPDB_A_OEMA_KEYT_PLDT_Hm, MEM_BLOCK_ANY, table_default));
    SOCDNX_IF_ERR_EXIT(arad_fill_table_with_entry(unit, PPDB_A_TCAM_ENTRY_PARITYm, MEM_BLOCK_ANY, table_default));
    SOCDNX_IF_ERR_EXIT(arad_fill_table_with_entry(unit, PPDB_A_TCAM_ENTRY_PARITY_SMALL_12m, MEM_BLOCK_ANY, table_default));
    SOCDNX_IF_ERR_EXIT(arad_fill_table_with_entry(unit, PPDB_A_TCAM_ENTRY_PARITY_SMALL_13m, MEM_BLOCK_ANY, table_default));
    SOCDNX_IF_ERR_EXIT(arad_fill_table_with_entry(unit, PPDB_A_TCAM_ENTRY_PARITY_SMALL_14m, MEM_BLOCK_ANY, table_default));
    SOCDNX_IF_ERR_EXIT(arad_fill_table_with_entry(unit, PPDB_A_TCAM_ENTRY_PARITY_SMALL_15m, MEM_BLOCK_ANY, table_default));
    
    soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, table_entry, TCAM_CPU_CMD_WRf, 0x3);
    soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, table_entry, TCAM_CPU_CMD_VALIDf, 0x0);
    SOCDNX_IF_ERR_EXIT(arad_fill_table_with_entry(unit, PPDB_A_TCAM_BANK_COMMANDm, MEM_BLOCK_ANY, table_entry));

    soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, table_entry, TCAM_CPU_CMD_WRf, 0x0);
    soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, table_entry, TCAM_CPU_CMD_VALIDf, 0x0);
    SOCDNX_IF_ERR_EXIT(arad_fill_table_with_entry(unit, PPDB_A_TCAM_BANK_COMMANDm, MEM_BLOCK_ANY, table_entry));

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 soc_jer_xlp_tbls_init(int unit)
{
    uint32 reset_value[SAND_MAX_U32S_IN_MEM_ENTRY] = {0};
    soc_jer_pll_config_t   *pll;
    soc_mem_info_t         *soc_mem_info;
    int copyno, maxblk, minblk;

    SOCDNX_INIT_FUNC_DEFS;

    pll = &SOC_DPP_CONFIG(unit)->jer->pll;
    soc_mem_info=&SOC_MEM_INFO(unit, XLPORT_WC_UCMEM_DATAm);
    maxblk=soc_mem_info->maxblock;
    minblk=soc_mem_info->minblock;

   if ((!(pll->ref_clk_pml_in[0] == soc_dcmn_init_serdes_ref_clock_disable || pll->ref_clk_pml_out[0] == soc_dcmn_init_serdes_ref_clock_disable)) && \
       (!(pll->ref_clk_pml_in[1] == soc_dcmn_init_serdes_ref_clock_disable || pll->ref_clk_pml_out[1] == soc_dcmn_init_serdes_ref_clock_disable))) {
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, XLPORT_WC_UCMEM_DATAm, MEM_BLOCK_ALL, reset_value));

    } else if (!(pll->ref_clk_pml_in[0] == soc_dcmn_init_serdes_ref_clock_disable || pll->ref_clk_pml_out[0] == soc_dcmn_init_serdes_ref_clock_disable)) {
        for (copyno=minblk; copyno<(minblk+6); copyno++) {
             SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, XLPORT_WC_UCMEM_DATAm, copyno, reset_value));
        }

    } else if (!(pll->ref_clk_pml_in[1] == soc_dcmn_init_serdes_ref_clock_disable || pll->ref_clk_pml_out[1] == soc_dcmn_init_serdes_ref_clock_disable)) {
        for (copyno=(minblk+6); copyno<=maxblk; copyno++) {
             SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, XLPORT_WC_UCMEM_DATAm, copyno, reset_value));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 soc_jer_clp_tbls_init(int unit)
{
    uint32 reset_value[SAND_MAX_U32S_IN_MEM_ENTRY] = {0};
    soc_jer_pll_config_t        *pll;

    SOCDNX_INIT_FUNC_DEFS;

    pll = &SOC_DPP_CONFIG(unit)->jer->pll;

   if (!(pll->ref_clk_pmh_in == soc_dcmn_init_serdes_ref_clock_disable || pll->ref_clk_pmh_out == soc_dcmn_init_serdes_ref_clock_disable)) {
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, CLPORT_WC_UCMEM_DATAm, MEM_BLOCK_ALL, reset_value));

    }

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_jer_excluded_tbls_list_set(int unit) 
{
    SHR_BITDCL *jer_excluded_mems_bmap_p = NULL;
    int mem_iter = 0;
    soc_mem_t* excluded_list;

    SOCDNX_INIT_FUNC_DEFS;

    
    jer_excluded_mems_bmap_p = SOC_DPP_CONFIG(unit)->jer->excluded_mems.excluded_mems_bmap;

    
    if (SOC_IS_QMX(unit)) {
        excluded_list = jer_tbls_88375_excluded_mem_list;
    } else if(SOC_IS_JERICHO(unit)){ 
        excluded_list = jer_tbls_88675_excluded_mem_list;
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Unknown Device Type\n")));
    }

    
    sal_memset(jer_excluded_mems_bmap_p, 0, SHR_BITALLOCSIZE(NUM_SOC_MEM));

    
    mem_iter = 0;
    while (excluded_list[mem_iter] != INVALIDm) {
        JER_MBMP_SET_EXCLUDED(excluded_list[mem_iter]);
        ++mem_iter;
    }

exit:
    SOCDNX_FUNC_RETURN;     
}


int soc_jer_static_tbls_reset (int unit)
{
    SHR_BITDCL *jer_excluded_mems_bmap_p = NULL;
    int mem_iter = 0;
    uint32 reset_value[128] = {0};

    SOCDNX_INIT_FUNC_DEFS;


    
    
    SOCDNX_IF_ERR_EXIT(soc_jer_excluded_tbls_list_set(unit));

    
    jer_excluded_mems_bmap_p = SOC_DPP_CONFIG(unit)->jer->excluded_mems.excluded_mems_bmap;

    
    for (mem_iter = 0; mem_iter < NUM_SOC_MEM; mem_iter++) 
    {
        if (SOC_MEM_IS_VALID(unit, mem_iter) && 
            
            (soc_mem_flags(unit, mem_iter) & (SOC_MEM_FLAG_SIGNAL | SOC_MEM_FLAG_READONLY)) == 0 &&

            
            !(SOC_MEM_IS_ALIAS(unit, mem_iter)) &&
            !JER_MBMP_IS_EXCLUDED(mem_iter)) { 

            
            LOG_VERBOSE(BSL_LS_SOC_INIT,(BSL_META_U(unit,"Reseting static memory # %d - %s\n"),mem_iter, SOC_MEM_NAME(unit, mem_iter)));

            
            SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, mem_iter, MEM_BLOCK_ALL, reset_value));
        }
    }

    

    
    SOCDNX_IF_ERR_EXIT(WRITE_FDA_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));

    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, FDA_FDA_MCm, MEM_BLOCK_ALL, reset_value));

    if (!SOC_IS_FLAIR(unit)) {
        SOCDNX_IF_ERR_EXIT(WRITE_PPDB_A_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
        SOCDNX_IF_ERR_EXIT(WRITE_PPDB_B_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
        SOCDNX_IF_ERR_EXIT(WRITE_EDB_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));

        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, PPDB_B_LARGE_EM_AGING_CONFIGURATION_TABLEm, MEM_BLOCK_ALL, reset_value));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, PPDB_B_LARGE_EM_FID_COUNTER_PROFILE_DBm, MEM_BLOCK_ALL, reset_value));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATIONm, MEM_BLOCK_ALL, reset_value));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_24m, MEM_BLOCK_ALL, reset_value));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_25m, MEM_BLOCK_ALL, reset_value));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_26m, MEM_BLOCK_ALL, reset_value));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_27m, MEM_BLOCK_ALL, reset_value));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_28m, MEM_BLOCK_ALL, reset_value));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_29m, MEM_BLOCK_ALL, reset_value));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_30m, MEM_BLOCK_ALL, reset_value));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_31m, MEM_BLOCK_ALL, reset_value));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, EDB_EEDB_BANKm, MEM_BLOCK_ALL, reset_value));
        SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, EDB_EEDB_TOP_BANKm, MEM_BLOCK_ALL, reset_value));
    }

    exit:
        SOCDNX_FUNC_RETURN;     
}

STATIC 
int
soc_jer_enable_dynamic_access(int unit, soc_regaddrinfo_t *ainfo, void *data)
{
    uint32 value = *(uint32 *)data;
    int blk;
    int inst=0;

    SOCDNX_INIT_FUNC_DEFS;
    if (!SOC_REG_IS_VALID(unit,ainfo->reg) || 
        sal_strstr(SOC_REG_NAME(unit,ainfo->reg),"BRDC")!=NULL  ||
        !SOC_REG_FIELD_VALID(unit,ainfo->reg,ENABLE_DYNAMIC_MEMORY_ACCESSf)) {
        SOC_EXIT;
    }
    SOC_BLOCK_ITER(unit, blk, SOC_REG_FIRST_BLK_TYPE(SOC_REG_INFO(unit, ainfo->reg).block)) {
        SOCDNX_IF_ERR_EXIT(soc_reg_field32_modify(unit, ainfo->reg, inst,ENABLE_DYNAMIC_MEMORY_ACCESSf, value));
        inst++;
    }



    exit:
        SOCDNX_FUNC_RETURN;     
}

STATIC
 int
soc_jer_mem_reset(int unit, soc_mem_t mem, void* reset_value)
{
    SOCDNX_INIT_FUNC_DEFS;
    if (!soc_mem_is_signal(unit,mem) || soc_mem_is_readonly(unit,mem)) {
        SOC_EXIT;
    }
    switch (mem) {
 
            
        case IQM_PAKCET_DESCRIPTOR_MEMORY_A_DYNAMICm:
        case IQM_PAKCET_DESCRIPTOR_MEMORY_B_DYNAMICm:
        case IQM_PAKCET_DESCRIPTOR_MEMORY_ECC_DYNAMICm:
        
        case MMU_DRAM_ADDRESS_SPACEm:
        case OCB_OCB_ADDRESS_SPACEm:



        SOC_EXIT;


    }
    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, mem, MEM_BLOCK_ALL, reset_value)); 

    exit:
        SOCDNX_FUNC_RETURN;     
}

int
soc_jer_dynamic_tbls_reset (int unit)
{
    soc_reg_above_64_val_t reg_value;
    uint32 value=1;
    uint32 reset_value[128] = {0};
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_reg_iterate(unit, soc_jer_enable_dynamic_access, &value)) ;
    SOCDNX_IF_ERR_EXIT(soc_mem_iterate(unit,soc_jer_mem_reset, reset_value));
    
    value=0;
    SOCDNX_IF_ERR_EXIT(soc_reg_iterate(unit, soc_jer_enable_dynamic_access, &value)) ;

    

    SOC_REG_ABOVE_64_ALLONES(reg_value);  
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_value));
    SOC_REG_ABOVE_64_CLEAR(reg_value);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_value));

     
    soc_reg_above_64_field32_set(unit, ECI_BLOCKS_SOFT_INITr, reg_value, BLOCKS_SOFT_INIT_1f, 1); 
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_value));
    SOC_REG_ABOVE_64_CLEAR(reg_value);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_SOFT_INITr(unit, reg_value));


    exit:
        SOCDNX_FUNC_RETURN;     
}

uint32
  jer_sch_drm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_DRM_TBL_DATA* SCH_drm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_DRM_TBL_ENTRY_SIZE];
  soc_mem_t mem ;
  soc_field_t field ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_DRM_TBL_GET_UNSAFE);
  err = soc_sand_os_memset(&(data[0]),0x0,sizeof(data)) ;
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);
  err = soc_sand_os_memset(SCH_drm_tbl_data,0x0,sizeof(ARAD_SCH_DRM_TBL_DATA));
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);
  
  mem = (soc_mem_t)SCH_DEVICE_RATE_MEMORY_DRMm ;
  field = (soc_field_t)DEVICE_RATEf ;
  err = soc_mem_read(unit,mem,SCH_BLOCK(unit, core),entry_offset,data) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);
  SCH_drm_tbl_data->device_rate = soc_mem_field32_get(unit,mem,data,field);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_sch_drm_tbl_get_unsafe()",0,0);
}

uint32
  jer_sch_drm_tbl_set_unsafe(
    SOC_SAND_IN   int                   unit,
    SOC_SAND_IN   int                   core,
    SOC_SAND_IN   uint32                entry_offset,
    SOC_SAND_IN   ARAD_SCH_DRM_TBL_DATA *SCH_drm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_DRM_TBL_ENTRY_SIZE];
  soc_mem_t mem ;
  soc_field_t field ;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_DRM_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(&(data[0]),0x0,sizeof(data)) ;
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);
  
  mem = (soc_mem_t)SCH_DEVICE_RATE_MEMORY_DRMm ;
  field = (soc_field_t)DEVICE_RATEf ;
  
  soc_mem_field32_set(unit,mem,data,field,SCH_drm_tbl_data->device_rate);
  err = soc_mem_write(unit,mem,SCH_BLOCK(unit, core),entry_offset,data) ;
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in jer_sch_drm_tbl_set_unsafe()",0,0);
}

uint32
  jer_sch_shds_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32              offset,
    SOC_SAND_OUT  ARAD_SCH_SHDS_TBL_DATA* SCH_shds_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_SHDS_TBL_ENTRY_SIZE];
  uint32
    entry_offset = offset;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SHDS_TBL_GET_UNSAFE);

  if (SOC_IS_QUX(unit)) {
      entry_offset -= QUX_SCH_SHDS_OFFSET;
  }
  else if (SOC_IS_QAX(unit)) {
      entry_offset -= QAX_SCH_SHDS_OFFSET;
  }
  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          SCH_shds_tbl_data,
          0x0,
          sizeof(ARAD_SCH_SHDS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_mem_read(
          unit,
          SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
          SCH_BLOCK(unit,core),
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  SCH_shds_tbl_data->peak_rate_man_even   = soc_mem_field32_get(
    unit,
    SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data,
    PEAK_RATE_MAN_EVENf            );
  SCH_shds_tbl_data->peak_rate_exp_even   = soc_mem_field32_get(
    unit,
    SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data,
    PEAK_RATE_EXP_EVENf            );
  SCH_shds_tbl_data->max_burst_even   = soc_mem_field32_get(
    unit,
    SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data,
    MAX_BURST_EVENf            );
  SCH_shds_tbl_data->slow_rate2_sel_even   = soc_mem_field32_get(
    unit,
    SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data,
    SLOW_RATE_2_SEL_EVENf            );
  SCH_shds_tbl_data->peak_rate_man_odd   = soc_mem_field32_get(
    unit,
    SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data,
    PEAK_RATE_MAN_ODDf            );
  SCH_shds_tbl_data->peak_rate_exp_odd   = soc_mem_field32_get(
    unit,
    SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data,
    PEAK_RATE_EXP_ODDf            );
  SCH_shds_tbl_data->max_burst_odd   = soc_mem_field32_get(
    unit,
    SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data,
    MAX_BURST_ODDf            );
  SCH_shds_tbl_data->slow_rate2_sel_odd   = soc_mem_field32_get(
    unit,
    SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data,
    SLOW_RATE_2_SEL_ODDf            );
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_shds_tbl_get_unsafe()",0,0);
}

uint32
  jer_sch_shds_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   int             core,
    SOC_SAND_IN   uint32          offset,
    SOC_SAND_IN   ARAD_SCH_SHDS_TBL_DATA* SCH_shds_tbl_data
  )
{
  uint32
    err;
  uint32
    data_shds[ARAD_SCH_SHDS_TBL_ENTRY_SIZE];
  uint32
    entry_offset = offset;
  uint32 orig_val; 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SHDS_TBL_SET_UNSAFE);

  if (SOC_IS_QUX(unit)) {
      entry_offset -= QUX_SCH_SHDS_OFFSET;
  }
  else if (SOC_IS_QAX(unit)) {
      entry_offset -= QAX_SCH_SHDS_OFFSET;
  }
  err = soc_sand_os_memset(
          &(data_shds[0]),
          0x0,
          sizeof(data_shds)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);
  soc_mem_field32_set(
    unit,SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data_shds,PEAK_RATE_MAN_EVENf,
    SCH_shds_tbl_data->peak_rate_man_even );
  soc_mem_field32_set(
    unit,SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data_shds,PEAK_RATE_EXP_EVENf,
    SCH_shds_tbl_data->peak_rate_exp_even );
  soc_mem_field32_set(
    unit,SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data_shds,MAX_BURST_EVENf,
    SCH_shds_tbl_data->max_burst_even );
  soc_mem_field32_set(
    unit,SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data_shds,SLOW_RATE_2_SEL_EVENf,
    SCH_shds_tbl_data->slow_rate2_sel_even );
  soc_mem_field32_set(
    unit,SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data_shds,PEAK_RATE_MAN_ODDf,
    SCH_shds_tbl_data->peak_rate_man_odd );
  soc_mem_field32_set(
    unit,SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data_shds,PEAK_RATE_EXP_ODDf,
    SCH_shds_tbl_data->peak_rate_exp_odd );
  soc_mem_field32_set(
    unit,SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data_shds,MAX_BURST_ODDf,
    SCH_shds_tbl_data->max_burst_odd );
  soc_mem_field32_set(
    unit,SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
    data_shds,SLOW_RATE_2_SEL_ODDf,
    SCH_shds_tbl_data->slow_rate2_sel_odd );
  err = soc_mem_write(
          unit,SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,
          SCH_BLOCK(unit,core),entry_offset,
          data_shds);
  {
    uint32  val32 ;
    uint32 slow_max_bucket_width ;
    soc_field_info_t *soc_field_info_p ;
    
    uint32 max_val_max_burst ;
    
    uint16 max_burst_field_len ;
    
    uint32
      data_tmc[JERICHO_SCH_TOKEN_MEMORY_CONTROLLER_TMC_TBL_ENTRY_SIZE] ;

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(err,10,exit,ARAD_REG_ACCESS_ERR,READ_SCH_SHAPER_CONFIGURATION_REGISTER_1r(unit, core, &val32)) ;
    slow_max_bucket_width = soc_reg_field_get(unit, SCH_SHAPER_CONFIGURATION_REGISTER_1r, val32, SLOW_MAX_BUCKET_WIDTHf) ;
    
    SOC_SAND_ERR_IF_ABOVE_MAX(slow_max_bucket_width, 3, ARAD_FLD_OUT_OF_RANGE, 20, exit) ;
    
    soc_field_info_p = soc_mem_fieldinfo_get(unit,SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC_SHDSm,MAX_BURST_EVENf) ;
    SOC_SAND_CHECK_NULL_PTR(soc_field_info_p, 30, exit) ;
    max_burst_field_len = soc_field_info_p->len ;
    max_val_max_burst = (1 << max_burst_field_len) - 1 ;
    
    if (slow_max_bucket_width >= 2)
    {
      max_val_max_burst = (1 << (max_burst_field_len - slow_max_bucket_width + 1)) - 1 ;
    }
    

    if (SCH_shds_tbl_data->max_burst_update_even)
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(SCH_shds_tbl_data->max_burst_even, max_val_max_burst,ARAD_FLD_OUT_OF_RANGE,40,exit) ;
    }
    if (SCH_shds_tbl_data->max_burst_update_odd)
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(SCH_shds_tbl_data->max_burst_odd, max_val_max_burst,ARAD_FLD_OUT_OF_RANGE,60,exit) ;
    }

    if (SCH_shds_tbl_data->max_burst_update_even)
    {
      err = soc_reg32_get(unit, SCH_ENABLE_DYNAMIC_MEMORY_ACCESSr, REG_PORT_ANY, 0, &orig_val);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 48, exit);
      err = soc_reg32_set(unit, SCH_ENABLE_DYNAMIC_MEMORY_ACCESSr, REG_PORT_ANY, 0, 1);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 49, exit);
      err =
        soc_sand_os_memset(&(data_tmc[0]),0x0,sizeof(data_tmc));
      soc_mem_field32_set(
        unit,SCH_TOKEN_MEMORY_CONTROLLER_TMCm,
        data_tmc,TOKEN_COUNTf,SCH_shds_tbl_data->max_burst_even );
      soc_mem_field32_set(
        unit,SCH_TOKEN_MEMORY_CONTROLLER_TMCm,
        data_tmc,SLOW_STATUSf,1 );
      err =
        soc_mem_write(
          unit,SCH_TOKEN_MEMORY_CONTROLLER_TMCm,
          SCH_BLOCK(unit,core),entry_offset,data_tmc);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 50, exit);
      
      err = soc_reg32_set(unit, SCH_ENABLE_DYNAMIC_MEMORY_ACCESSr, REG_PORT_ANY, 0, orig_val);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 51, exit);
    }
    if (SCH_shds_tbl_data->max_burst_update_odd)
    {
       err = soc_reg32_get(unit, SCH_ENABLE_DYNAMIC_MEMORY_ACCESSr, REG_PORT_ANY, 0, &orig_val);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 68, exit);
      err = soc_reg32_set(unit, SCH_ENABLE_DYNAMIC_MEMORY_ACCESSr, REG_PORT_ANY, 0, 1);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 69, exit);
      err =
        soc_sand_os_memset(&(data_tmc[0]),0x0,sizeof(data_tmc));
      soc_mem_field32_set(
        unit,SCH_TOKEN_MEMORY_CONTROLLER_TMC_MSBm,
        data_tmc,TOKEN_COUNTf,
        SCH_shds_tbl_data->max_burst_odd );
      soc_mem_field32_set(
        unit,SCH_TOKEN_MEMORY_CONTROLLER_TMC_MSBm,
        data_tmc,SLOW_STATUSf,1 );
      err =
        soc_mem_write(
          unit,SCH_TOKEN_MEMORY_CONTROLLER_TMC_MSBm,
          SCH_BLOCK(unit,core),entry_offset,data_tmc);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 70, exit);
      
      err = soc_reg32_set(unit, SCH_ENABLE_DYNAMIC_MEMORY_ACCESSr, REG_PORT_ANY, 0, orig_val);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 71, exit);
    }
  }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 80, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_shds_tbl_set_unsafe()",0,0);
}


void jer_mark_memory(
    const int       unit,
    const soc_mem_t mem        
  )
{
    
    soc_mem_t mem_alias = mem;
    SOC_MEM_ALIAS_TO_ORIG(unit, mem_alias);
    SHR_BITSET(SOC_DPP_CONFIG(unit)->jer->excluded_mems.excluded_mems_bmap, mem_alias);
}


int jer_fill_and_mark_memory(
    const int       unit,
    const soc_mem_t mem,        
    const void      *entry_data 
  )
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, mem, MEM_BLOCK_ALL, entry_data));
    jer_mark_memory(unit, mem);

exit:
    SOCDNX_FUNC_RETURN;
}






static soc_mem_t jerplus_tbls_excluded_mem_list[] = {
    IRR_MCDBm,
    ILKN_PMH_PORT_0_CPU_ACCESSm, 
    ILKN_PMH_PORT_1_CPU_ACCESSm,
    ILKN_PML_PORT_0_CPU_ACCESSm,
    ILKN_PML_PORT_1_CPU_ACCESSm,
    SCH_SCHEDULER_INITm,        
#ifndef JERPLUS_REDSOLVE_LATER
    OAMP_FLOW_STAT_ACCUM_ENTRY_34m,
#endif 
    SER_ACC_TYPE_MAPm,
    SER_MEMORYm,
    SER_RESULT_DATA_1m,
    SER_RESULT_DATA_0m,
    SER_RESULT_EXPECTED_1m,
    SER_RESULT_EXPECTED_0m,
    SER_RESULT_1m,
    SER_RESULT_0m,
    BRDC_FSRD_FSRD_WL_EXT_MEMm,
    FSRD_FSRD_WL_EXT_MEMm,

    
    IQM_PAKCET_DESCRIPTOR_MEMORY_A_DYNAMICm,
    IQM_PAKCET_DESCRIPTOR_MEMORY_B_DYNAMICm,
    IQM_PAKCET_DESCRIPTOR_MEMORY_ECC_DYNAMICm,
    
    MMU_DRAM_ADDRESS_SPACEm,
    OCB_OCB_ADDRESS_SPACEm,

    
    CLPORT_WC_UCMEM_DATAm,
    XLPORT_WC_UCMEM_DATAm,
    
    KAPS_RPB_TCAM_CPU_COMMANDm,
    KAPS_RPB_TCAM_CPU_CMDm,
    KAPS_RPB_TCAM_CPU_REPLYm,
    
    INVALIDm
};


STATIC soc_reg_t jer_plus_dynamic_mem_enable_regs[] = {
    BRDC_FSRD_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CFC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CRPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EDB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EGQ_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    EPNI_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FCR_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FDA_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FDT_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IDR_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IHB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IHP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ILKN_PMH_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ILKN_PML_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPST_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPT_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IQMT_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IRE_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IRR_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    MMU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    MRPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    MTRPS_EM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    OAMP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    OCB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    OLP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    PPDB_A_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    PPDB_B_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    RTP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    SCH_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    KAPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    KAPS_BBS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    INVALIDr
};


STATIC soc_reg_t jer_plus_dynamic_mem_disable_regs[] = {
    BRDC_FSRD_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CFC_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    CRPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ECI_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    
    
    FCR_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    FDT_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IDR_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    
    
    ILKN_PMH_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    ILKN_PML_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPST_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IPT_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    
    IQM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IRE_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    IRR_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    MMU_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    MRPS_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    MTRPS_EM_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    
    OCB_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    
    
    RTP_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    SCH_ENABLE_DYNAMIC_MEMORY_ACCESSr,
    
    
    INVALIDr
};



static int soc_jerplus_ihp_tbls_init(int unit)
{    
#ifdef JERICHO_PLUS_INIT_EGRESSTO_INGRESS_MIRROR_PROFILE_MAP
    int i;
    uint32 table_entry[SAND_MAX_U32S_IN_MEM_ENTRY] = {0};
    uint32 value[1];
#endif
    uint32 table_default_vtt[1] = {0xb80};
    uint32 table_default_vsi[1] = {0x4000070};
    uint32 table_default_pinfo[3] = {0x0, 0x1ee00000, 0x0};
    uint32 table_default_path_selection[1] = {0x0};
    SOCDNX_INIT_FUNC_DEFS;

#ifdef JERICHO_PLUS_INIT_EGRESSTO_INGRESS_MIRROR_PROFILE_MAP
LOG_ERROR(BSL_LS_SOC_DRAM,(BSL_META_U(unit, "mem %s 0\n"), SOC_MEM_NAME(unit, IHP_RECYCLE_COMMANDm)));
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, IHP_RECYCLE_COMMANDm, table_entry));
LOG_ERROR(BSL_LS_SOC_DRAM,(BSL_META_U(unit, "mem %s 1\n"), SOC_MEM_NAME(unit, IHP_RECYCLE_COMMANDm)));

    
    for (i = 0; i <= DPP_MIRROR_ACTION_NDX_MAX; ++i) {
        value[0] = 0; 
        soc_mem_field32_set(unit, IHP_RECYCLE_COMMANDm, value, MIRROR_PROFILEf, i);
        if (i > 0) {       
            soc_mem_field32_set(unit, IHP_RECYCLE_COMMANDm, value, FORWARD_STRENGTHf, 7); 
            soc_mem_field32_set(unit, IHP_RECYCLE_COMMANDm, value, CPU_TRAP_CODEf, SOC_PPC_TRAP_CODE_INTERNAL_LLR_ACCEPTABLE_FRAME_TYPE0);   
            SOCDNX_IF_ERR_EXIT(WRITE_IHP_RECYCLE_COMMANDm(unit, MEM_BLOCK_ALL, i, value));
        }
    }
#endif 

    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, IHP_VTT_LLVPm, table_default_vtt));
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, IHP_VSI_LOW_CFG_2m, table_default_vsi));
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, IHP_PINFO_LLRm, table_default_pinfo));
    
    
      
    SOCDNX_IF_ERR_EXIT(WRITE_IHP_INDIRECT_WR_MASKr(unit,SOC_CORE_ALL, 0));
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, IHP_VTT_PATH_SELECTm, table_default_path_selection));
    SOCDNX_IF_ERR_EXIT(WRITE_IHP_INDIRECT_WR_MASKr(unit,SOC_CORE_ALL, 0xffffffff));   

exit:
    SOCDNX_FUNC_RETURN;
}

static uint32  soc_jerplus_idr_tbls_init (int unit){
    uint32 table_entry = 0;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(sand_fill_partial_table_with_entry(unit, IDR_DRAM_BUFFER_TYPEm, 0, 0, MEM_BLOCK_ANY, 0, 47, &table_entry));
    table_entry = 1;
    SOCDNX_IF_ERR_EXIT(sand_fill_partial_table_with_entry(unit, IDR_DRAM_BUFFER_TYPEm, 0, 0, MEM_BLOCK_ANY, 48, 63, &table_entry));
    jer_mark_memory(unit, IDR_DRAM_BUFFER_TYPEm);

exit:
    SOCDNX_FUNC_RETURN;

}

static uint32 soc_jerplus_irr_tbls_init(int unit)
{
    uint32 m[2]= {0};
    SOCDNX_INIT_FUNC_DEFS;
    
    if (!SOC_DPP_CONFIG(unit)->arad->init.dram.nof_drams) {
        soc_mem_field32_set(unit, IRR_QUEUE_IS_OCB_COMMITTEDm, m, COMMITMENTf, 0xffffffff); 
        if (SOC_DPP_CONFIG(unit)->emulation_system) { 
            SOCDNX_IF_ERR_EXIT(sand_fill_partial_table_with_entry(unit, IRR_QUEUE_IS_OCB_COMMITTEDm, 0, 0, MEM_BLOCK_ALL, 0, 10000, m));
            SHR_BITSET(SOC_DPP_CONFIG(unit)->jer->excluded_mems.excluded_mems_bmap, IRR_QUEUE_IS_OCB_COMMITTEDm);
        } else {
            SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, IRR_QUEUE_IS_OCB_COMMITTEDm, m));
        }
    }

    
    m[0] = 0; 
    soc_mem_field32_set(unit, IRR_DESTINATION_TABLEm, m, QUEUE_NUMBERf, 0x1ffff); 
    soc_mem_field32_set(unit, IRR_DESTINATION_TABLEm, m, TC_PROFILEf, 0); 
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, IRR_DESTINATION_TABLEm, m)); 

exit:
    SOCDNX_FUNC_RETURN;
}



static int soc_jerplus_ipt_tbls_init(int unit)
{
    int i;
    uint32 fabric_priority, is_tdm, tc;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);

    
    for (i = 0; i < SOC_JER_FABRIC_PRIORITY_NDX_NOF; i++) {
        
        is_tdm = (i & SOC_JER_FABRIC_PRIORITY_NDX_IS_TDM_MASK) >> SOC_JER_FABRIC_PRIORITY_NDX_IS_TDM_OFFSET;
        tc = (i & SOC_JER_FABRIC_PRIORITY_NDX_TC_MASK) >> SOC_JER_FABRIC_PRIORITY_NDX_TC_OFFSET;

        if (is_tdm) {
            fabric_priority = 3;
        } else { 
            
            
            
            fabric_priority = tc / 3;
        }

        SOCDNX_IF_ERR_EXIT(WRITE_IPT_PRIORITY_BITS_MAPPING_2_FDTm(unit, MEM_BLOCK_ALL, i, &fabric_priority));
        SOCDNX_IF_ERR_EXIT(WRITE_IPT_TDM_BIT_MAPPING_2_FDTm(unit, MEM_BLOCK_ALL, i, &is_tdm));
    }
    SHR_BITSET(SOC_DPP_CONFIG(unit)->jer->excluded_mems.excluded_mems_bmap, IPT_PRIORITY_BITS_MAPPING_2_FDTm);
    SHR_BITSET(SOC_DPP_CONFIG(unit)->jer->excluded_mems.excluded_mems_bmap, IPT_TDM_BIT_MAPPING_2_FDTm);

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_jerplus_fdt_tbls_init(int unit)
{
    uint32 table_default[5] = {0};
    uint32 pcp_config_data = 2;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);

    
    if ((SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_EGR_CORE_MESH_MODE) == 0 && 
         SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_pcp_enable) {
        
        SHR_BITCOPY_RANGE(table_default, 6, &pcp_config_data, 0, SOC_JER_FABRIC_PCP_LENGTH);
    }

    if(!SOC_IS_QUX(unit)) {
        
        SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, FDT_IPT_MESH_MCm, table_default));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

static int soc_jerplus_fcr_tbls_init(int unit)
{
    uint32 table_entry[1] = {0};

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);
    
    soc_mem_field32_set(unit, FCR_EFMS_SOURCE_PIPEm, table_entry, DATAf, 0xff); 
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, FCR_EFMS_SOURCE_PIPEm, table_entry));

exit:
    SOCDNX_FUNC_RETURN;
}



static int soc_jerplus_iqm_tbls_init(int unit)
{
    uint32 table_default[1] = {0};
    uint32 table_pcp_default[2] = {0xaaaaaaaa};
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);

    SOCDNX_IF_ERR_EXIT(WRITE_IQMT_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));

    
    
    if (SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_pcp_enable) {
        SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, IQM_PACK_MODEm, table_pcp_default)) ;
    }
    soc_mem_field32_set(unit, IQM_PQWQm, table_default, PQ_WEIGHTf, 2);
    soc_mem_field32_set(unit, IQM_PQWQm, table_default, PQ_AVRG_ENf, 1); 
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, IQM_PQWQm, table_default)); 
    sal_memset(table_default, 0, sizeof(table_default));

exit:
    SOCDNX_FUNC_RETURN;
}

 
static int soc_jerplus_sch_tbls_init(int unit)
{
    uint32 table_entry[128] = {0};
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);
 
    
    soc_mem_field32_set(unit, SCH_FLOW_TO_FIP_MAPPING_FFMm, table_entry, DEVICE_NUMBERf, SOC_TMC_MAX_FAP_ID);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, SCH_FLOW_TO_FIP_MAPPING_FFMm, table_entry));
    soc_mem_field32_set(unit, SCH_FLOW_TO_FIP_MAPPING_FFMm, table_entry, DEVICE_NUMBERf, 0);

    soc_mem_field32_set(unit, SCH_PS_8P_RATES_PSRm, table_entry, PS_8P_RATES_PSRf, 128);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, SCH_PS_8P_RATES_PSRm, table_entry));
    soc_mem_field32_set(unit, SCH_PS_8P_RATES_PSRm, table_entry, PS_8P_RATES_PSRf, 0);

    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry, WFQ_PG_0_WEIGHTf, 0x1);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry, WFQ_PG_1_WEIGHTf, 0x2);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry, WFQ_PG_2_WEIGHTf, 0x4);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry, WFQ_PG_3_WEIGHTf, 0x8);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry, WFQ_PG_4_WEIGHTf, 0x10);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry, WFQ_PG_5_WEIGHTf, 0x20);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry, WFQ_PG_6_WEIGHTf, 0x40);
    soc_mem_field32_set(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry, WFQ_PG_7_WEIGHTf, 0x80);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, SCH_PORT_SCHEDULER_WEIGHTS_PSWm, table_entry));   

exit:
    SOCDNX_FUNC_RETURN;
}

static int soc_jerplus_egq_tbls_init(int unit)
{
    soc_reg_above_64_val_t data;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(data);
    soc_mem_field32_set(unit, EGQ_PCTm, data, CGM_PORT_PROFILEf, ARAD_EGR_PORT_THRESH_TYPE_15);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, EGQ_PCTm, data));

    
    SOC_REG_ABOVE_64_ALLONES(data);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, EGQ_VSI_MEMBERSHIPm, data));

    SOC_REG_ABOVE_64_CLEAR(data);
    soc_mem_field32_set(unit, EGQ_DSP_PTR_MAPm, data, OUT_TM_PORTf, ARAD_EGR_INVALID_BASE_Q_PAIR);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, EGQ_DSP_PTR_MAPm, data));

    SOC_REG_ABOVE_64_CLEAR(data);
    soc_mem_field32_set(unit, EGQ_PER_IFC_CFGm, data, OTM_PORT_NUMBERf, ARAD_EGR_INVALID_BASE_Q_PAIR);
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, EGQ_PER_IFC_CFGm, data));

    SOC_REG_ABOVE_64_CLEAR(data);
    soc_mem_field32_set(unit, EGQ_PP_PPCTm, data, MTUf, 0x3fff);    
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, EGQ_PP_PPCTm, data));

    if (SOC_DPP_CONFIG(unit)->emulation_system) {
        
        SOC_REG_ABOVE_64_CLEAR(data);
        SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, EGQ_IVEC_TABLEm, data));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


static int soc_jerplus_epni_tbls_init(int unit)
{
    uint32 table_entry[SAND_MAX_U32S_IN_MEM_ENTRY] = {0};
    uint32 tx_tag_table_entry[9] = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff, 0};
    uint32 table_default[3] = {0x10040100, 0x04010040, 0x4010};
    int dpp, dp, entry_index, pcp_dei, value;
    
    uint32 out_lif_null_entry = ARAD_PP_EG_ENCAP_EEDB_INDEX_TO_OUTLIF(unit, SOC_PPC_LIF_NULL_LOCAL_OUTLIF_ID); 


    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, SOC_CORE_ALL, 1)); 

    
    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, EPNI_MIRROR_PROFILE_TABLEm, table_default));

    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, EPNI_TX_TAG_TABLEm, &tx_tag_table_entry));

    if (SOC_DPP_CONFIG(unit)->emulation_system) {
        
        SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, EPNI_IVEC_TABLEm, table_entry));
    }

    soc_mem_field32_set(unit, EPNI_PACKETPROCESSING_PORT_CONFIGURATION_TABLEm, table_entry, MTUf, 0x3fff);
    
    soc_mem_field32_set(unit, EPNI_PACKETPROCESSING_PORT_CONFIGURATION_TABLEm, table_entry, DEFAULT_SEM_RESULTf, out_lif_null_entry);

    SOCDNX_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, EPNI_PACKETPROCESSING_PORT_CONFIGURATION_TABLEm, table_entry));

    SOCDNX_SAND_IF_ERR_EXIT(jer_fill_and_mark_memory(unit, EPNI_PCP_DEI_DP_MAPPING_TABLEm, table_entry));
    
    
    for (dpp = 0; dpp <=ARAD_PP_DP_PROFILE_NDX_MAX; ++dpp) {
        for (dp = 0; dp <= ARAD_PP_DP_MAX_VAL; ++dp) {
            for (pcp_dei =0; pcp_dei <= ARAD_PP_PCP_DEI_MAX_VAL; ++pcp_dei){
                value =0;
                entry_index = (dpp << 6) + (dp << 4) + pcp_dei; 
                soc_mem_field32_set(unit, EPNI_PCP_DEI_DP_MAPPING_TABLEm, &value, PUSH_PCP_DEIf, pcp_dei);
                SOCDNX_IF_ERR_EXIT(WRITE_EPNI_PCP_DEI_DP_MAPPING_TABLEm(unit, MEM_BLOCK_ALL, entry_index, &value));
            } 
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

static uint32 soc_jerplus_kaps_tbls_init(int unit)
{
    uint32 table_default_path_selection[1] = {0x0};
    soc_reg_above_64_val_t data;
    SOCDNX_INIT_FUNC_DEFS;
    SOC_REG_ABOVE_64_ALLONES(data);
    
    if (SOC_IS_JERICHO_PLUS_AND_BELOW(unit)) {
        SOCDNX_IF_ERR_EXIT(WRITE_KAPS_INDIRECT_WR_MASKr(unit, data));

        SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, KAPS_TCAM_HIT_INDICATIONm, KAPS_BLOCK(unit, 0), table_default_path_selection));
        SOCDNX_SAND_IF_ERR_EXIT(arad_fill_table_with_entry(unit, KAPS_RPB_TCAM_HIT_INDICATIONm, KAPS_BLOCK(unit, 0), table_default_path_selection));
        SOC_REG_ABOVE_64_CLEAR(data);
        SOCDNX_IF_ERR_EXIT(WRITE_KAPS_INDIRECT_WR_MASKr(unit, data));
    }

exit:
    SOCDNX_FUNC_RETURN;
}



void soc_jer_tbls_zero_init(int unit, soc_mem_t *mem_exclude_list)
{
    SHR_BITDCL *jerplus_excluded_mems_bmap_p = SOC_DPP_CONFIG(unit)->jer->excluded_mems.excluded_mems_bmap;
    soc_mem_t* excluded_list = mem_exclude_list; 

    
    sal_memset(jerplus_excluded_mems_bmap_p, 0, SHR_BITALLOCSIZE(NUM_SOC_MEM));

    
    for (; *excluded_list != INVALIDm; ++excluded_list)  { 
        SHR_BITSET(jerplus_excluded_mems_bmap_p, *excluded_list); 
    }
}


int soc_jer_tbls_zero(int unit)
{
    
    SHR_BITDCL *jerplus_excluded_mems_bmap_p = SOC_DPP_CONFIG(unit)->jer->excluded_mems.excluded_mems_bmap;
    int mem_iter = 0;
    uint32 entry0[128] = {0};
    SOCDNX_INIT_FUNC_DEFS;
        
    
    if (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "init_tbls_to_zero", !(
#ifdef PLISIM
            SAL_BOOT_PLISIM || 
#endif
            SOC_DPP_CONFIG(unit)->emulation_system))) {

        
        for (mem_iter = 0; mem_iter < NUM_SOC_MEM; mem_iter++) { 
            if (SOC_MEM_IS_VALID(unit, mem_iter) && 
                (soc_mem_flags(unit, mem_iter) & SOC_MEM_FLAG_READONLY) == 0 && 
    
                
                !(SOC_MEM_IS_ALIAS(unit, mem_iter)) &&
                !SHR_BITGET(jerplus_excluded_mems_bmap_p, mem_iter)) { 
    
                
                LOG_VERBOSE(BSL_LS_SOC_INIT,(BSL_META_U(unit,"Reseting static memory # %d - %s\n"),mem_iter, SOC_MEM_NAME(unit, mem_iter)));
    
                SOCDNX_IF_ERR_EXIT(sand_fill_table_with_entry(unit, mem_iter, MEM_BLOCK_ALL, entry0));
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jerplus_tbls_init(int unit)
{
    
    uint32 sch_init_val = 1;
    SOCDNX_INIT_FUNC_DEFS;

    
    soc_jer_tbls_zero_init(unit, jerplus_tbls_excluded_mem_list);
        
    SOCDNX_IF_ERR_EXIT(arad_tbl_access_init_unsafe(unit)); 
    SOCDNX_IF_ERR_EXIT(soc_sand_control_dynamic_mem_writes(unit, jer_plus_dynamic_mem_enable_regs, 1)); 

    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, ILKN_PML_ENABLE_DYNAMIC_MEMORY_ACCESSr, 1, 0, 1));
    

    SOCDNX_IF_ERR_EXIT(soc_jerplus_sch_tbls_init(unit)); 
    SOCDNX_IF_ERR_EXIT(soc_jerplus_irr_tbls_init(unit)); 

    

    SOCDNX_IF_ERR_EXIT(soc_jerplus_ihp_tbls_init(unit)); 
    SOCDNX_IF_ERR_EXIT(soc_jerplus_iqm_tbls_init(unit)); 

    SOCDNX_IF_ERR_EXIT(soc_jer_ips_tbls_init(unit)); 
    SOCDNX_IF_ERR_EXIT(soc_jerplus_fcr_tbls_init(unit)); 
    SOCDNX_IF_ERR_EXIT(soc_jerplus_ipt_tbls_init(unit)); 

    SOCDNX_IF_ERR_EXIT(soc_jerplus_fdt_tbls_init(unit)); 

    SOCDNX_IF_ERR_EXIT(soc_jerplus_egq_tbls_init(unit)); 
    SOCDNX_IF_ERR_EXIT(soc_jerplus_epni_tbls_init(unit)); 

    SOCDNX_IF_ERR_EXIT(soc_jerplus_kaps_tbls_init(unit)); 

    
    SOCDNX_IF_ERR_EXIT(soc_jer_mrps_tbls_init(unit));

    SOCDNX_IF_ERR_EXIT(soc_jer_mrpsEm_tbls_init(unit)); 

    SOCDNX_IF_ERR_EXIT(soc_jerplus_idr_tbls_init(unit));
    
    
    
    SOCDNX_IF_ERR_EXIT(soc_jer_ppdb_tbls_init(unit));

    if (SOC_DPP_CONFIG(unit)->emulation_system <= 1) { 
        SOCDNX_IF_ERR_EXIT(dpp_mult_rplct_tbl_entry_unoccupied_set_all(unit));
    }
    SOCDNX_IF_ERR_EXIT(dpp_mcds_multicast_init2(unit));

    
    SOCDNX_IF_ERR_EXIT(soc_jer_tbls_zero(unit));
    SOCDNX_IF_ERR_EXIT(soc_sand_control_dynamic_mem_writes(unit, jer_plus_dynamic_mem_disable_regs, 0)); 
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, ILKN_PML_ENABLE_DYNAMIC_MEMORY_ACCESSr, 1, 0, 0));
    
    SOCDNX_IF_ERR_EXIT(WRITE_SCH_SCHEDULER_INITm(unit, SCH_BLOCK(unit, SOC_CORE_ALL), 0, &sch_init_val));

    SOCDNX_SAND_IF_ERR_EXIT(arad_pp_eg_mirror_init_unsafe(unit)); 

exit:
    SOCDNX_FUNC_RETURN;
}
