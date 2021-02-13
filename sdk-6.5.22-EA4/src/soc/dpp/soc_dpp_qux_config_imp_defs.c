/*
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 * This file is auto-generated by autoCoder
 * DO NOT EDIT THIS FILE!
 */

#include <shared/bsl.h>
#include <soc/defs.h>
#ifdef BCM_QUX_SUPPORT

#include <soc/dpp/dpp_config_imp_defs.h>
#include <soc/dpp/soc_dpp_qux_config_imp_defs.h>

#ifdef _ERR_MSG_MODULE_NAME
    #error "_ERR_MSG_MODULE_NAME redefined"
#endif 

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT

CONST soc_dpp_implementation_defines_t soc_dpp_implementation_defines_qux= {
    SOC_DPP_NOF_CHANNELIZED_INTERFACES_QUX,
    SOC_DPP_NOF_SPECIAL_CHANNELIZED_INTERFACES_QUX,
    SOC_DPP_NOF_SPECIAL_NON_CHANNELIZED_INTERFACES_QUX,
    SOC_DPP_NOF_NON_CHANNELIZED_INTERFACES_QUX,
    SOC_DPP_NOF_CORE_INTERFACES_QUX,
    SOC_DPP_NOF_FEC_BANKS_QUX,
    SOC_DPP_IHP_STP_TABLE_NOF_LONGS_QUX,
    SOC_DPP_IHB_PINFO_PMF_NOF_LONGS_QUX,
    SOC_DPP_IHB_PMF_PROGRAM_SELECTION_CAM_NOF_LONGS_QUX,
    SOC_DPP_IHB_FEC_ECMP_NOF_LONGS_QUX,
    SOC_DPP_EGQ_PPCT_NOF_LONGS_QUX,
    SOC_DPP_EPNI_PRGE_PROGRAM_SELECTION_CAM_NOF_LONGS_QUX,
    SOC_DPP_IHP_LIF_ACCESSED_NOF_LINES_QUX,
    SOC_DPP_IHP_FLP_PROGRAM_SELECTION_CAM_NOF_LONGS_QUX,
    SOC_DPP_IHP_FLP_LOOKUPS_NOF_LONGS_QUX,
    SOC_DPP_IHB_PTC_INFO_NOF_LONGS_QUX,
    SOC_DPP_MACT_LIF_BASE_LENGTH_QUX,
    SOC_DPP_PPDB_LARGE_EM_FLUSH_DB_NOF_LONGS_QUX,
    SOC_DPP_MESH_TOPOLOGY_REGISTER_CONFIG_4_VAL_QUX,
    SOC_DPP_EGR_DELETE_FIFO_ALMOST_FULL_MC_LOW_PRIO_QUX,
    SOC_DPP_EGR_DELETE_FIFO_ALMOST_FULL_MC_QUX,
    SOC_DPP_EGR_DELETE_FIFO_ALMOST_FULL_ALL_QUX,
    SOC_DPP_EGR_DELETE_FIFO_THRESHOLD_MAX_QUX,
    SOC_DPP_OUTLIF_PROFILE_NOF_BITS_QUX,
    SOC_DPP_DSP_EVENT_ROUTE_ENTRY_NOF_BITS_QUX,
    SOC_DPP_MIRROR_SNOOP_DESTINATION_QUEUE_ENCODING_QUX,
    SOC_DPP_MIRROR_SNOOP_DESTINATION_MULTICAST_ENCODING_QUX,
    SOC_DPP_MIRROR_SNOOP_DESTINATION_SYS_PHY_PORT_ENCODING_QUX,
    SOC_DPP_MIRROR_SNOOP_DESTINATION_LAG_ENCODING_QUX,
    SOC_DPP_MDIO_INT_DIVIDEND_DEFAULT_QUX,
    SOC_DPP_MDIO_INT_FREQ_DEFAULT_QUX,
    SOC_DPP_MDIO_INT_OUT_DELAY_DEFAULT_QUX,
    SOC_DPP_CMD_IPT_SNP_MIR_CMD_MAP_SNOOP_OFFSET_QUX,
    SOC_DPP_MIRROR_SNOOP_DEST_TM_TABLE_OFFSET_QUX,
    SOC_DPP_FLP_INSTRUCTIONS_NOF_QUX,
    SOC_DPP_PP_EG_MIRROR_PROFILE_TABLE_BITS_PER_PROFILE_QUX,
    SOC_DPP_MIRROR_PROBABILITY_BITS_QUX,
    SOC_DPP_OAM_ID_NOF_BIT_QUX,
    SOC_DPP_ILKN_CTXT_MAP_START_QUX,
    SOC_DPP_NIF_CTXT_MAP_START_QUX,
    SOC_DPP_RCY_CTXT_MAP_START_CORE_0_QUX,
    SOC_DPP_RCY_CTXT_MAP_START_CORE_1_QUX,
    SOC_DPP_CPU_CTXT_MAP_START_QUX,
    SOC_DPP_SAT_CTXT_MAP_START_QUX,
    SOC_DPP_RCY_0_INTERFACE_QUX,
    SOC_DPP_RCY_1_INTERFACE_QUX,
    SOC_DPP_CPU_INTERFACE_QUX,
    SOC_DPP_SAT_INTERFACE_QUX,
    SOC_DPP_EGR_IF_MIN_QUX,
    SOC_DPP_EGR_IF_CPU_QUX,
    SOC_DPP_EGR_IF_OLP_QUX,
    SOC_DPP_EGR_IF_OAMP_QUX,
    SOC_DPP_EGR_IF_RCY_QUX,
    SOC_DPP_EGR_IF_SAT_QUX,
    SOC_DPP_EGR_IF_IPSEC_QUX,
    SOC_DPP_EGR_IF_MAX_QUX,
    SOC_DPP_EGR_TOTAL_PD_QUX,
    SOC_DPP_EGR_TOTAL_DB_QUX,
    SOC_DPP_EGR_TOTAL_UC_PD_QUX,
    SOC_DPP_EGR_TOTAL_UC_DB_QUX,
    SOC_DPP_EGR_TOTAL_MC_PD_QUX,
    SOC_DPP_EGR_TOTAL_MC_DB_QUX,
    SOC_DPP_EGR_TOTAL_UC_FC_PD_QUX,
    SOC_DPP_EGR_TOTAL_UC_FC_DB_QUX,
    SOC_DPP_RCI_SEVERITY_FACTOR_1_QUX,
    SOC_DPP_RCI_SEVERITY_FACTOR_2_QUX,
    SOC_DPP_RCI_SEVERITY_FACTOR_3_QUX,
    SOC_DPP_RCI_THRESHOLD_NOF_CONGESTED_LINKS_QUX,
    SOC_DPP_KAPS_DMA_LINE_WIDTH_QUX,
    SOC_DPP_KAPS_DMA_ZONE_WIDTH_QUX,
};


int soc_dpp_qux_implementation_defines_init(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;

    if ( soc_dpp_implementation_defines[unit] == NULL ) {
        soc_dpp_implementation_defines[unit] = (soc_dpp_implementation_defines_t *) sal_alloc(sizeof(soc_dpp_implementation_defines_t),"Defines struct");
        if ( soc_dpp_implementation_defines[unit] == NULL ) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Failed to allocate memory for configuration defines struct")));
        }
    }
        sal_memcpy(soc_dpp_implementation_defines[unit], &soc_dpp_implementation_defines_qux, sizeof(soc_dpp_implementation_defines_t));

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_dpp_qux_implementation_defines_deinit(int unit)
{
    if ( soc_dpp_implementation_defines[unit] != NULL ) {
        sal_free(soc_dpp_implementation_defines[unit]);
        soc_dpp_implementation_defines[unit] = NULL;
    }

    return SOC_E_NONE;
}





#endif 
