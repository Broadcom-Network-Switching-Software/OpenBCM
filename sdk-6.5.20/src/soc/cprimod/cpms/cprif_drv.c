/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bsl.h>
#include <soc/error.h>
#include <soc/esw/port.h>
#include <soc/cprimod/cprimod.h>
#include <soc/cprimod/cprimod_internal.h>
#include <soc/cprimod/cprimod_dispatch.h>
#include <soc/cprimod/cprif_drv.h>
#include <soc/cprimod/cprif_drv_private.h>
#include "../../../../libs/phymod/chip/falcon_dpll/tier1/falcon_dpll_cfg_seq.h"
#include "../../../../libs/phymod/chip/falcon_dpll/tier1/falcon2_monterey_interface.h"


#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef CPRIMOD_CPRI_FALCON_SUPPORT

#define CPRIF_DRV_NUMBER_OF_MSG 1920
#define CPRIF_DRV_N_MG_IN_MSTR_FRM 21
#define CPRIF_DRV_RSVD4_MSG_SIZE 19

#define CPRIF_DRV_MEM_PREP phy_port = SOC_INFO(unit).port_l2p_mapping[port]; \
        port_core_index = ((phy_port-1)%4); \
        local_mem = mem_select[port_core_index]; \
        num_words = soc_mem_entry_words(unit, local_mem); \
        mem_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, ""); \
        SOC_NULL_CHECK(mem_data_ptr); \
        sal_memset(mem_data_ptr, 0, sizeof(uint32) * num_words);

int cprif_pm_version_get(int unit, soc_port_t port, cprif_pm_version_t  *pm_version)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    if (!SOC_REG_IS_VALID(unit, CLPORT_PM_VERSION_IDr)) {
        pm_version->pm_type         = 0xFFFF;
        pm_version->tech_process    = 0xFFFF;
        pm_version->rev_number      = 0xFFFF;
        pm_version->rev_letter      = 0xFFFF;
    } else {
        _SOC_IF_ERR_EXIT(READ_CLPORT_PM_VERSION_IDr(unit, port, &reg_val));
        pm_version->pm_type         = soc_reg_field_get(unit, CLPORT_PM_VERSION_IDr, reg_val,PM_TYPEf );
        pm_version->tech_process    = soc_reg_field_get(unit, CLPORT_PM_VERSION_IDr, reg_val,TECH_PROCESSf );
        pm_version->rev_number      = soc_reg_field_get(unit, CLPORT_PM_VERSION_IDr, reg_val,REV_NUMBERf );
        pm_version->rev_letter      = soc_reg_field_get(unit, CLPORT_PM_VERSION_IDr, reg_val,REV_LETTERf );
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_bfa_bfp_table_entry_t_init( cprif_bfa_bfp_table_entry_t* cprimod_bfa_bfp_table_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprimod_bfa_bfp_table_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprimod_bfa_bfp_table_entry NULL parameter"));
    }
    sal_memset(cprimod_bfa_bfp_table_entry, 0, sizeof(cprif_bfa_bfp_table_entry_t));
    cprimod_bfa_bfp_table_entry->axc_id = 0;
    cprimod_bfa_bfp_table_entry->rsrv_bit_cnt = 0;
    cprimod_bfa_bfp_table_entry->data_bit_cnt = 0;

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_container_map_entry_t_init( cprif_cpri_container_map_entry_t* cprif_cpri_container_map_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_cpri_container_map_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_cpri_container_map_entry NULL parameter"));
    }
    sal_memset(cprif_cpri_container_map_entry, 0, sizeof(cprif_cpri_container_map_entry_t));
    cprif_cpri_container_map_entry->map_method = 0;
    cprif_cpri_container_map_entry->axc_id = 0;
    cprif_cpri_container_map_entry->stuffing_at_end = 0;
    cprif_cpri_container_map_entry->stuff_cnt = 0;
    cprif_cpri_container_map_entry->cblk_cnt = 0;
    cprif_cpri_container_map_entry->naxc_cnt = 0;
    cprif_cpri_container_map_entry->Nv = 0;
    cprif_cpri_container_map_entry->Na = 0;
    cprif_cpri_container_map_entry->rfrm_sync = 0;
    cprif_cpri_container_map_entry->hfrm_sync = 0;
    cprif_cpri_container_map_entry->bfn_offset = 0;
    cprif_cpri_container_map_entry->hfn_offset = 0;
    cprif_cpri_container_map_entry->bfrm_offset = 0;


exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_container_map_entry_t_init( cprif_rsvd4_container_map_entry_t* cprif_rsvd4_container_map_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_rsvd4_container_map_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_rsvd4_container_map_entry NULL parameter"));
    }
    sal_memset(cprif_rsvd4_container_map_entry, 0, sizeof(cprif_rsvd4_container_map_entry_t));
    cprif_rsvd4_container_map_entry->map_method = 0;
    cprif_rsvd4_container_map_entry->axc_id = 0;
    cprif_rsvd4_container_map_entry->stuffing_at_end = 0;
    cprif_rsvd4_container_map_entry->stuff_cnt = 0;
    cprif_rsvd4_container_map_entry->cblk_cnt = 0;
    cprif_rsvd4_container_map_entry->rfrm_offset = 0;
    cprif_rsvd4_container_map_entry->bfn_offset = 0;
    cprif_rsvd4_container_map_entry->msg_ts_mode = 0;
    cprif_rsvd4_container_map_entry->rfrm_sync = 0;
    cprif_rsvd4_container_map_entry->hfrm_sync = 0;
    cprif_rsvd4_container_map_entry->msg_ts_cnt = 0;
    cprif_rsvd4_container_map_entry->msg_ts_sync = 0;
    cprif_rsvd4_container_map_entry->use_ts_dbm = 0;
    cprif_rsvd4_container_map_entry->ts_dbm_prof_num = 0;
    cprif_rsvd4_container_map_entry->num_active_slots = 0;
    cprif_rsvd4_container_map_entry->msg_addr = 0;
    cprif_rsvd4_container_map_entry->msg_type = 0;
    cprif_rsvd4_container_map_entry->msg_ts_offset = 0;


exit:
    SOC_FUNC_RETURN;

}

int cprif_iq_buffer_config_t_init( cprif_iq_buffer_config_t* cprif_iq_buffer_config_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_iq_buffer_config_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_iq_buffer_config_entry NULL parameter"));
    }
    sal_memset(cprif_iq_buffer_config_entry, 0, sizeof(cprif_iq_buffer_config_t));
    cprif_iq_buffer_config_entry->payload_size      = 0;
    cprif_iq_buffer_config_entry->last_packet_num  = 0;
    cprif_iq_buffer_config_entry->last_payload_size = 0;

exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_decap_data_entry_t_init( cprif_encap_decap_data_entry_t* cprif_encap_decap_data_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_encap_decap_data_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_encap_decap_data_entry NULL parameter"));
    }
    sal_memset(cprif_encap_decap_data_entry, 0, sizeof(cprif_encap_decap_data_entry_t));
    cprif_encap_decap_data_entry->sample_size         = 0;
    cprif_encap_decap_data_entry->out_sample_size     = 0;
    cprif_encap_decap_data_entry->valid               = 0;
    cprif_encap_decap_data_entry->mux_enable          = 0;
    cprif_encap_decap_data_entry->queue_size          = 0;
    cprif_encap_decap_data_entry->queue_offset        = 0;
    cprif_encap_decap_data_entry->sign_ext_enable     = 0;
    cprif_encap_decap_data_entry->sign_ext_type       = CPRIF_DATA_SIGN_EXT_15_TO_16;
    cprif_encap_decap_data_entry->bit_reversal        = 0;
    cprif_encap_decap_data_entry->tx_cycle_size       = 0;

exit:
    SOC_FUNC_RETURN;
}

int cprif_rsvd4_encap_decap_data_entry_t_init( cprif_rsvd4_encap_decap_data_entry_t* cprif_rsvd4_encap_decap_data_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_rsvd4_encap_decap_data_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_rsvd4_encap_decap_data_entry NULL parameter"));
    }
    sal_memset(cprif_rsvd4_encap_decap_data_entry, 0, sizeof(cprif_rsvd4_encap_decap_data_entry_t));
    cprif_rsvd4_encap_decap_data_entry->gsm_pad_size            = 0;
    cprif_rsvd4_encap_decap_data_entry->gsm_extra_pad_size      = 0;
    cprif_rsvd4_encap_decap_data_entry->gsm_pad_enable          = 0;
    cprif_rsvd4_encap_decap_data_entry->gsm_control_location    = 0;
    cprif_rsvd4_encap_decap_data_entry->valid                   = 0;
    cprif_rsvd4_encap_decap_data_entry->queue_size              = 0;
    cprif_rsvd4_encap_decap_data_entry->queue_offset            = 0;
    cprif_rsvd4_encap_decap_data_entry->tx_cycle_size           = 0;

exit:
    SOC_FUNC_RETURN;
}

int cprif_encap_header_entry_t_init( cprif_encap_header_entry_t* cprif_encap_header_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_encap_header_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_encap_header_entry NULL parameter"));
    }
    sal_memset(cprif_encap_header_entry, 0, sizeof(cprif_encap_header_entry_t));
    cprif_encap_header_entry->header_type           = 0;
    cprif_encap_header_entry->ver                   = 0;
    cprif_encap_header_entry->pkttype               = 0;
    cprif_encap_header_entry->flow_id               = 0;
    cprif_encap_header_entry->ordering_info_index   = 0;
    cprif_encap_header_entry->mac_da_index          = 0;
    cprif_encap_header_entry->mac_sa_index          = 0;
    cprif_encap_header_entry->vlan_type             = 0;
    cprif_encap_header_entry->vlan_id_0_index       = 0;
    cprif_encap_header_entry->vlan_id_1_index       = 0;
    cprif_encap_header_entry->vlan_0_priority       = 0;
    cprif_encap_header_entry->vlan_1_priority       = 0;
    cprif_encap_header_entry->ether_type_index      = 0;
    cprif_encap_header_entry->use_tagid_for_vlan    = 0;
    cprif_encap_header_entry->use_tagid_for_flowid  = 0;
    cprif_encap_header_entry->use_opcode            = 0;
    cprif_encap_header_entry->roe_opcode            = 0;
    cprif_encap_header_entry->tsn_bitmap            = 0;
    cprif_encap_header_entry->gsm_q_cnt_disable     = 0;

exit:
    SOC_FUNC_RETURN;
}

int cprif_encap_ordering_info_entry_t_init( cprif_encap_ordering_info_entry_t* cprif_encap_ordering_info_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_encap_ordering_info_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_encap_ordering_info_entry NULL parameter"));
    }
    sal_memset(cprif_encap_ordering_info_entry, 0, sizeof(cprif_encap_ordering_info_entry_t));
    cprif_encap_ordering_info_entry->type       = 0;
    cprif_encap_ordering_info_entry->p_size     = 0;
    cprif_encap_ordering_info_entry->q_size     = 0;
    cprif_encap_ordering_info_entry->max        = 0;
    cprif_encap_ordering_info_entry->increment  = 0;
    cprif_encap_ordering_info_entry->pcnt_prop  = 0;
    cprif_encap_ordering_info_entry->qcnt_prop  = 0;

exit:
    SOC_FUNC_RETURN;
}

int cprif_decap_ordering_info_entry_t_init( cprif_decap_ordering_info_entry_t* cprif_decap_ordering_info_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_decap_ordering_info_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_decap_ordering_info_entry NULL parameter"));
    }
    sal_memset(cprif_decap_ordering_info_entry, 0, sizeof(cprif_decap_ordering_info_entry_t));

    cprif_decap_ordering_info_entry->type           = 0;
    cprif_decap_ordering_info_entry->p_size         = 0x20;
    cprif_decap_ordering_info_entry->q_size         = 0;
    cprif_decap_ordering_info_entry->max            = 0xFFFFFFFF;
    cprif_decap_ordering_info_entry->increment      = 1;
    cprif_decap_ordering_info_entry->pcnt_inc_p2    = 1;
    cprif_decap_ordering_info_entry->qcnt_inc_p2    = 0;
    cprif_decap_ordering_info_entry->pcnt_extended  = 0;
    cprif_decap_ordering_info_entry->pcnt_pkt_count = 0;
    cprif_decap_ordering_info_entry->modulo_2       = 1;
    cprif_decap_ordering_info_entry->bias           = 1;
    cprif_decap_ordering_info_entry->gsm_tsn_bitmap = 0x55;

exit:
    SOC_FUNC_RETURN;
}


int cprif_header_compare_entry_t_init( cprif_header_compare_entry_t* cprif_header_compare_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_header_compare_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_header_compare_entry NULL parameter"));
    }
    sal_memset(cprif_header_compare_entry, 0, sizeof(cprif_header_compare_entry_t));

    cprif_header_compare_entry->valid           = 0;
    cprif_header_compare_entry->match_data      = 0;
    cprif_header_compare_entry->mask            = 0;
    cprif_header_compare_entry->flow_id         = 0xFF;
    cprif_header_compare_entry->ctrl_flow       = 0;

exit:
    SOC_FUNC_RETURN;
}


int cprif_modulo_rule_entry_t_init( cprif_modulo_rule_entry_t* cprif_modulo_rule_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_modulo_rule_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_modulo_rule_entry NULL parameter"));
    }
    sal_memset(cprif_modulo_rule_entry, 0, sizeof(cprif_modulo_rule_entry_t));

    cprif_modulo_rule_entry->active = 0;
    cprif_modulo_rule_entry->modulo = 0;
    cprif_modulo_rule_entry->modulo_index = 0;
    cprif_modulo_rule_entry->dbm_enable = 0xFF;
    cprif_modulo_rule_entry->flow_id = 0xFF;
    cprif_modulo_rule_entry->ctrl_flow = 0;


exit:
    SOC_FUNC_RETURN;
}


int cprif_rsvd4_dbm_rule_entry_t_init( cprif_rsvd4_dbm_rule_entry_t* cprif_rsvd4_dbm_rule_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_rsvd4_dbm_rule_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_rsvd4_dbm_rule_entry NULL parameter"));
    }
    sal_memset(cprif_rsvd4_dbm_rule_entry, 0, sizeof(cprif_rsvd4_dbm_rule_entry_t));

    cprif_rsvd4_dbm_rule_entry->bm1_mult = 0;
    cprif_rsvd4_dbm_rule_entry->bm1[0]= 0;
    cprif_rsvd4_dbm_rule_entry->bm1[1]= 0;
    cprif_rsvd4_dbm_rule_entry->bm1[2]= 0;
    cprif_rsvd4_dbm_rule_entry->bm1_size = 0;
    cprif_rsvd4_dbm_rule_entry->bm2[0]= 0;
    cprif_rsvd4_dbm_rule_entry->bm2[1]= 0;
    cprif_rsvd4_dbm_rule_entry->bm2_size = 0;
    cprif_rsvd4_dbm_rule_entry->num_slots = 0;
    cprif_rsvd4_dbm_rule_entry->pos_index = 0;
    cprif_rsvd4_dbm_rule_entry->pos_entries = 0;

exit:
    SOC_FUNC_RETURN;
}


int cprif_rsvd4_secondary_dbm_rule_entry_t_init( cprif_rsvd4_secondary_dbm_rule_entry_t* cprif_rsvd4_secondary_dbm_rule_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_rsvd4_secondary_dbm_rule_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_rsvd4_secondary_dbm_rule_entry NULL parameter"));
    }
    sal_memset(cprif_rsvd4_secondary_dbm_rule_entry, 0, sizeof(cprif_rsvd4_secondary_dbm_rule_entry_t));

    cprif_rsvd4_secondary_dbm_rule_entry->bm1_mult = 0;
    cprif_rsvd4_secondary_dbm_rule_entry->bm1 = 0;
    cprif_rsvd4_secondary_dbm_rule_entry->bm1_size = 0;
    cprif_rsvd4_secondary_dbm_rule_entry->bm2= 0;
    cprif_rsvd4_secondary_dbm_rule_entry->bm2_size = 0;
    cprif_rsvd4_secondary_dbm_rule_entry->num_slots = 0;

exit:
    SOC_FUNC_RETURN;
}

int cprif_rsvd4_dbm_pos_table_entry_t_init( cprif_rsvd4_dbm_pos_table_entry_t* cprif_rsvd4_dbm_pos_table_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (cprif_rsvd4_dbm_pos_table_entry == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("cprif_rsvd4_dbm_pos_table_entry NULL parameter"));
    }
    sal_memset(cprif_rsvd4_dbm_pos_table_entry, 0, sizeof(cprif_rsvd4_dbm_pos_table_entry_t));

    cprif_rsvd4_dbm_pos_table_entry->valid      = 0;
    cprif_rsvd4_dbm_pos_table_entry->flow_id    = 0;
    cprif_rsvd4_dbm_pos_table_entry->ctrl_flow  = 0;
    cprif_rsvd4_dbm_pos_table_entry->flow_index = 0;

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_basic_frame_parser_active_table_set(int unit, int port, uint8 table)
{
    uint32 reg_val;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_RXFRM_BFRM_CTRLr(unit, port, &reg_val));
    field_buf = table;
    soc_reg_field_set(unit, CPRI_RXFRM_BFRM_CTRLr, &reg_val,
                      BFRM_MAP_TAB_IDf, field_buf);
    _SOC_IF_ERR_EXIT(WRITE_CPRI_RXFRM_BFRM_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_basic_frame_parser_active_table_get(int unit, int port, uint8* table)
{
    uint32 reg_val;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_RXFRM_BFRM_CTRLr(unit, port, &reg_val));
    field_buf =  soc_reg_field_get(unit, CPRI_RXFRM_BFRM_CTRLr, reg_val, BFRM_MAP_TAB_IDf);
    *table = field_buf;

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_basic_frame_parser_table_num_entries_set(int unit, int port, uint8 table, uint32 num_entries)
{
    uint32 reg_val;
    uint32 active_table;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_RXFRM_BFRM_CTRLr(unit, port, &reg_val));

    active_table =  soc_reg_field_get(unit, CPRI_RXFRM_BFRM_CTRLr, reg_val, BFRM_MAP_TAB_IDf);

    /*
     * Check to see if it is trying to change number of entries to active table.
     */
    if (active_table == table) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,(_SOC_MSG("cannot change number of entries of active table.")));
    }

    field_buf = num_entries;
    if (table == 0) {
        soc_reg_field_set(unit, CPRI_RXFRM_BFRM_CTRLr, &reg_val,
                          NUM_VLD_ENTRIES_TAB0f, field_buf);
    } else {
        soc_reg_field_set(unit, CPRI_RXFRM_BFRM_CTRLr, &reg_val,
                          NUM_VLD_ENTRIES_TAB1f, field_buf);
    }

    _SOC_IF_ERR_EXIT(WRITE_CPRI_RXFRM_BFRM_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_basic_frame_parser_table_num_entries_get(int unit, int port, uint8 table, uint32* num_entries)
{
    uint32 reg_val;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    if (num_entries == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("num_entries NULL parameter"));
    }

    _SOC_IF_ERR_EXIT(READ_CPRI_RXFRM_BFRM_CTRLr(unit, port, &reg_val));
    if (table == 0) {
        field_buf = soc_reg_field_get(unit, CPRI_RXFRM_BFRM_CTRLr, reg_val,NUM_VLD_ENTRIES_TAB0f);
    } else {
        field_buf = soc_reg_field_get(unit, CPRI_RXFRM_BFRM_CTRLr, reg_val,NUM_VLD_ENTRIES_TAB1f);
    }

    *num_entries = field_buf;

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_rx_framer_agnostic_mode_set (int unit, int port, uint32 axc_id,
                                           int enable,
                                           cprimod_agnostic_mode_type_t mode,
                                           int restore_rsvd4)
{
    uint32 reg_val;
    uint32 field_buf;
    uint32 tunnel_mode = FALSE;
    uint32 rx_agnostic_mode = FALSE;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));

    if (CPRIF_CLPORT_IS_A0(pm_version)) {
        if ((mode == cprimod_agnostic_mode_rsvd4) ||
            (mode == cprimod_agnostic_mode_tunnel)) {

            LOG_CLI(("This feature  is not supported in A0.\n"));
            return SOC_E_UNAVAIL;
        }
    }

    _SOC_IF_ERR_EXIT(READ_CPRI_RXFRM_BFRM_CTRLr(unit, port, &reg_val));

    if (enable) {
        switch (mode) {
            case cprimod_agnostic_mode_cpri:
                field_buf = axc_id;
                soc_reg_field_set(unit, CPRI_RXFRM_BFRM_CTRLr, &reg_val,
                                  FWD_CW_TO_ENCAP_AXCC_IDf, field_buf);

                field_buf = TRUE ;
                soc_reg_field_set(unit, CPRI_RXFRM_BFRM_CTRLr, &reg_val,
                                  FWD_CW_TO_ENCAPf, field_buf);

                rx_agnostic_mode    = FALSE ;
                tunnel_mode         = FALSE;

                break;

            case cprimod_agnostic_mode_rsvd4:

                field_buf = FALSE ;
                soc_reg_field_set(unit, CPRI_RXFRM_BFRM_CTRLr, &reg_val,
                                  FWD_CW_TO_ENCAPf, field_buf);

                field_buf = FALSE ;
                soc_reg_field_set(unit, CPRI_RXFRM_BFRM_CTRLr, &reg_val,
                                  BRCM_RESERVED_CPM_306_0f, field_buf);

                rx_agnostic_mode    = TRUE;
                tunnel_mode         = FALSE;

                break;

            case cprimod_agnostic_mode_tunnel:

                field_buf = FALSE ;
                soc_reg_field_set(unit, CPRI_RXFRM_BFRM_CTRLr, &reg_val,
                                  FWD_CW_TO_ENCAPf, field_buf);

                field_buf = FALSE;
                soc_reg_field_set(unit, CPRI_RXFRM_BFRM_CTRLr, &reg_val,
                              BRCM_RESERVED_CPM_306_0f, field_buf);

                rx_agnostic_mode    = FALSE ;
                tunnel_mode         = TRUE;

                break;

            default:
                break;
        }

    } else { /* Disable */
        field_buf = FALSE ;
        soc_reg_field_set(unit, CPRI_RXFRM_BFRM_CTRLr, &reg_val,
                          FWD_CW_TO_ENCAPf, field_buf);

        if ((mode == cprimod_agnostic_mode_rsvd4)||
            (restore_rsvd4)) {
            field_buf = TRUE;
            soc_reg_field_set(unit, CPRI_RXFRM_BFRM_CTRLr, &reg_val,
                              BRCM_RESERVED_CPM_306_0f, field_buf);
        }

        rx_agnostic_mode    = FALSE ;
        tunnel_mode         = FALSE;
    }

    if (CPRIF_CLPORT_IS_B0(pm_version)) {
        soc_reg_field_set(unit, CPRI_RXFRM_BFRM_CTRLr, &reg_val,
                          RXFRM_TUNNEL_MODEf, tunnel_mode);
        soc_reg_field_set(unit, CPRI_RXFRM_BFRM_CTRLr, &reg_val,
                          RXFRM_AGNOSTIC_MODE_ENf, rx_agnostic_mode);
    }
    _SOC_IF_ERR_EXIT(WRITE_CPRI_RXFRM_BFRM_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_rx_framer_agnostic_mode_get(int unit, int port,uint32* axc_id, int* enable, cprimod_agnostic_mode_type_t* mode)
{
    uint32 reg_val;
    uint32 field_buf;
    int tunnel_mode = 0;
    int rsvd4_agnostic = 0;
    int fwd_cw =0;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));


    _SOC_IF_ERR_EXIT(READ_CPRI_RXFRM_BFRM_CTRLr(unit, port, &reg_val));

    if (CPRIF_CLPORT_IS_B0(pm_version)) {
        field_buf = soc_reg_field_get(unit, CPRI_RXFRM_BFRM_CTRLr, reg_val,
                                      RXFRM_TUNNEL_MODEf);
        tunnel_mode = field_buf ? 1:0;

        field_buf = soc_reg_field_get(unit, CPRI_RXFRM_BFRM_CTRLr, reg_val,
                                      RXFRM_AGNOSTIC_MODE_ENf);
        rsvd4_agnostic = field_buf ? 1:0;
    }

    field_buf = soc_reg_field_get(unit, CPRI_RXFRM_BFRM_CTRLr, reg_val,
                                  FWD_CW_TO_ENCAPf);
    fwd_cw  = field_buf ? 1:0;

    *axc_id = 0;

    if (tunnel_mode) {
        *mode   =  cprimod_agnostic_mode_tunnel;
        *enable = TRUE;
    } else if (rsvd4_agnostic) {
        *mode   =  cprimod_agnostic_mode_rsvd4;
        *enable = TRUE;
    } else if (fwd_cw) {
        *mode   =  cprimod_agnostic_mode_cpri;
        *enable = TRUE;
        *axc_id = soc_reg_field_get(unit, CPRI_RXFRM_BFRM_CTRLr, reg_val,
                                      FWD_CW_TO_ENCAP_AXCC_IDf);
    } else {
        *enable = FALSE;
    }

exit:
    SOC_FUNC_RETURN;

}
int cprif_drv_tx_framer_rsvd4_agnostic_mode_set(int unit, int port, int enable)
{
    uint64 data;
    uint64 write_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_2r(unit, port, &data));

    /*
     *  In RSVD4 agnostic, the framer is not run in RSVD4 mode.
     */

    if(enable) {
        COMPILER_64_SET(write_val, 0, 0);
    } else {
        COMPILER_64_SET(write_val, 0, 1);
    }
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_2r, &data,
                        BRCM_RESERVED_CPM_2_0f, write_val);

    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_2r(unit, port, data));

    SOC_FUNC_RETURN;
}

int cprif_drv_basic_frame_parser_cw_to_encap_config_set(int unit, int port, uint32 axc_id, int fwd_cw_to_encap)
{
    uint32 reg_val;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_RXFRM_BFRM_CTRLr(unit, port, &reg_val));

   field_buf = axc_id;
    soc_reg_field_set(unit, CPRI_RXFRM_BFRM_CTRLr, &reg_val,
                      FWD_CW_TO_ENCAP_AXCC_IDf, field_buf);

    field_buf = fwd_cw_to_encap ? 1:0 ;
    soc_reg_field_set(unit, CPRI_RXFRM_BFRM_CTRLr, &reg_val,
                      FWD_CW_TO_ENCAPf, field_buf);
    _SOC_IF_ERR_EXIT(WRITE_CPRI_RXFRM_BFRM_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_basic_frame_assembly_active_table_set(int unit, int port, uint8 table)
{
    uint32 reg_val;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;


    _SOC_IF_ERR_EXIT(READ_CPRI_TXFRM_BFA_CTRLr(unit, port, &reg_val));
    field_buf = table;
    soc_reg_field_set(unit, CPRI_TXFRM_BFA_CTRLr, &reg_val, BFA_MAP_TAB_IDf, field_buf);

    _SOC_IF_ERR_EXIT(WRITE_CPRI_TXFRM_BFA_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;

}


int cprif_drv_basic_frame_assembly_active_table_get(int unit, int port, uint8* table)
{
    uint32 reg_val;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_TXFRM_BFA_CTRLr(unit, port, &reg_val));
    field_buf = soc_reg_field_get(unit, CPRI_TXFRM_BFA_CTRLr, reg_val, BFA_MAP_TAB_IDf);

    *table = field_buf;

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_basic_frame_assembly_table_num_entries_set(int unit, int port, uint8 table, uint32 num_entries)
{
    uint32 reg_val;
    uint32 field_buf;
    uint32 active_table;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_TXFRM_BFA_CTRLr(unit, port, &reg_val));

    active_table =  soc_reg_field_get(unit, CPRI_TXFRM_BFA_CTRLr, reg_val, BFA_MAP_TAB_IDf);
    /*
     * Check to see if it is trying to change number of entries to active table.
     */

    if (active_table == table) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM,(_SOC_MSG("cannot change number of entries of active table.")));
    }

    field_buf = num_entries;
    if (table == 0) {
        soc_reg_field_set(unit, CPRI_TXFRM_BFA_CTRLr, &reg_val,
                          BFA_NUM_VLD_ENTRIES_TAB0f, field_buf);
    } else {
        soc_reg_field_set(unit, CPRI_TXFRM_BFA_CTRLr, &reg_val,
                          BFA_NUM_VLD_ENTRIES_TAB1f, field_buf);
    }

    _SOC_IF_ERR_EXIT(WRITE_CPRI_TXFRM_BFA_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_basic_frame_assembly_table_num_entries_get(int unit, int port, uint8 table, uint32* num_entries)
{
    uint32 reg_val;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_TXFRM_BFA_CTRLr(unit, port, &reg_val));

    if (table == 0) {
        field_buf = soc_reg_field_get(unit, CPRI_TXFRM_BFA_CTRLr, reg_val,NUM_VLD_ENTRIES_TAB0f);
    } else {
        field_buf = soc_reg_field_get(unit, CPRI_TXFRM_BFA_CTRLr, reg_val,NUM_VLD_ENTRIES_TAB1f);
    }
    *num_entries = field_buf;
exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_basic_frame_entry_set(int unit, int port,
                                     uint8 table,
                                     cprimod_direction_t dir,
                                     uint8 index,
                                     const cprif_bfa_bfp_table_entry_t *entry)
{
    uint32 *wr_data_ptr;
    soc_mem_t local_mem;
    int blk;
    int phy_port;
    int port_core_index;
    uint32 field_buf;
    uint32 num_words;


    soc_mem_t tx_mem_select[] = { CPRI_BFA_TAB0_CFG_0m,   /* port_0, table 0 */
                                  CPRI_BFA_TAB0_CFG_1m,   /* port_1, table 0 */
                                  CPRI_BFA_TAB0_CFG_2m,   /* port_2, table 0 */
                                  CPRI_BFA_TAB0_CFG_3m,   /* port_3, table 0 */
                                  CPRI_BFA_TAB1_CFG_0m,   /* port_0, table 1 */
                                  CPRI_BFA_TAB1_CFG_1m,   /* port_1, table 1 */
                                  CPRI_BFA_TAB1_CFG_2m,   /* port_2, table 1 */
                                  CPRI_BFA_TAB1_CFG_3m }; /* port_3, table 1 */
    soc_mem_t rx_mem_select[] = { CPRI_BFP_TAB0_CFG_0m,   /* port_0, table 0 */
                                  CPRI_BFP_TAB0_CFG_1m,   /* port_1, table 0 */
                                  CPRI_BFP_TAB0_CFG_2m,   /* port_2, table 0 */
                                  CPRI_BFP_TAB0_CFG_3m,   /* port_3, table 0 */
                                  CPRI_BFP_TAB1_CFG_0m,   /* port_0, table 1 */
                                  CPRI_BFP_TAB1_CFG_1m,   /* port_1, table 1 */
                                  CPRI_BFP_TAB1_CFG_2m,   /* port_2, table 1 */
                                  CPRI_BFP_TAB1_CFG_3m }; /* port_3, table 1 */

    SOC_INIT_FUNC_DEFS;
    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (dir == CPRIMOD_DIR_TX) {
        local_mem = tx_mem_select[4*table + port_core_index];
    } else {
        local_mem = rx_mem_select[4*table + port_core_index];
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "cprimod");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    field_buf = entry->axc_id;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, AXCC_IDf, &field_buf);

    /*
     * Convert the bits into count as basic frame tables store the count.
     */
    field_buf = entry->rsrv_bit_cnt/2;    /* NB: this is 'cnt', not 'bits' */
    soc_mem_field_set(unit, local_mem, wr_data_ptr, RSRV_CNTf, &field_buf);

    /*
     * Convert the bits into count.
     */
    field_buf = entry->data_bit_cnt/2;    /* NB: this is 'cnt', not 'bits' */
    soc_mem_field_set(unit, local_mem, wr_data_ptr, DATA_CNTf, &field_buf);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));
exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}



int cprif_drv_basic_frame_entry_get(int unit, int port,
                                     uint8 table,
                                     cprimod_direction_t dir,
                                     uint8 index,
                                     cprif_bfa_bfp_table_entry_t *entry)
{

    soc_mem_t local_mem;
    int blk;
    int phy_port;
    int port_core_index;
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;

    soc_mem_t tx_mem_select[] = { CPRI_BFA_TAB0_CFG_0m,   /* port_0, table 0 */
                                  CPRI_BFA_TAB0_CFG_1m,   /* port_1, table 0 */
                                  CPRI_BFA_TAB0_CFG_2m,   /* port_2, table 0 */
                                  CPRI_BFA_TAB0_CFG_3m,   /* port_3, table 0 */
                                  CPRI_BFA_TAB1_CFG_0m,   /* port_0, table 1 */
                                  CPRI_BFA_TAB1_CFG_1m,   /* port_1, table 1 */
                                  CPRI_BFA_TAB1_CFG_2m,   /* port_2, table 1 */
                                  CPRI_BFA_TAB1_CFG_3m }; /* port_3, table 1 */

    soc_mem_t rx_mem_select[] = { CPRI_BFP_TAB0_CFG_0m,   /* port_0, table 0 */
                                  CPRI_BFP_TAB0_CFG_1m,   /* port_1, table 0 */
                                  CPRI_BFP_TAB0_CFG_2m,   /* port_2, table 0 */
                                  CPRI_BFP_TAB0_CFG_3m,   /* port_3, table 0 */
                                  CPRI_BFP_TAB1_CFG_0m,   /* port_0, table 1 */
                                  CPRI_BFP_TAB1_CFG_1m,   /* port_1, table 1 */
                                  CPRI_BFP_TAB1_CFG_2m,   /* port_2, table 1 */
                                  CPRI_BFP_TAB1_CFG_3m }; /* port_3, table 1 */
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (dir == CPRIMOD_DIR_TX) {
        local_mem = tx_mem_select[4*table + port_core_index];
    } else {
        local_mem = rx_mem_select[4*table + port_core_index];
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));
    soc_mem_field_get(unit, local_mem, rd_data_ptr, AXCC_IDf, &field_buf);
    entry->axc_id = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, RSRV_CNTf, &field_buf);

    /*
     * Convert the count into bits as basic frame table store the count.
     */
    entry->rsrv_bit_cnt = field_buf*2;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, DATA_CNTf, &field_buf);

    /*
     * Convert the count into bits.
     */
    entry->data_bit_cnt = field_buf*2;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;

}


int cprif_drv_basic_frame_usage_entry_add(int unit, int port,
                                           uint8 table,
                                           cprimod_direction_t dir,
                                           cprimod_basic_frame_usage_entry_t new_entry,
                                           cprimod_basic_frame_usage_entry_t *user_table,
                                           int num_entries_in_table,
                                           int *num_entries )
{
    int table_index;
    cprif_bfa_bfp_table_entry_t table_entry;
    uint16 current_bit_location;
    int table_end;
    int entry_added=0;
    int user_table_index;
    current_bit_location = 0;
    user_table_index = 0;
    table_end = FALSE;
    entry_added = FALSE;

    for (table_index=0; (table_index<CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY) &&(!table_end) ; table_index++) {
        SOC_IF_ERROR_RETURN
            (cprif_drv_basic_frame_entry_get(unit, port,
                                           table, dir,
                                           table_index,
                                           &table_entry));

        /*
         * reserve bits need to be count first.
         */

        if (table_entry.rsrv_bit_cnt != 0) {
            current_bit_location += table_entry.rsrv_bit_cnt;
        }

        /* if data count is non-zero, it is an active entry.*/
        if (table_entry.data_bit_cnt != 0) {

            if ((new_entry.start_bit <  current_bit_location) &&
                (!entry_added)) {
                if (user_table_index < num_entries_in_table) {
                    user_table[user_table_index].start_bit = new_entry.start_bit;
                    user_table[user_table_index].num_bits  = new_entry.num_bits;
                    user_table[user_table_index].axc_id    = new_entry.axc_id;
                    user_table_index++;
                }
                entry_added = TRUE;
            }

            /*
             * Add existing entry.
             */

            if (user_table_index < num_entries_in_table) {
                user_table[user_table_index].start_bit = current_bit_location;
                user_table[user_table_index].num_bits  = table_entry.data_bit_cnt;
                user_table[user_table_index].axc_id    = table_entry.axc_id;
                current_bit_location += table_entry.data_bit_cnt;
                user_table_index++;
            }
        }

        /*
         * if both count are 0, then it is end of the table.
         */
        if ((table_entry.data_bit_cnt == 0) &&
            (table_entry.rsrv_bit_cnt == 0)) {
            table_end = TRUE;
        }
    } /* for loop table_index */

    if (!entry_added) {
        user_table[user_table_index].start_bit = new_entry.start_bit;
        user_table[user_table_index].num_bits = new_entry.num_bits;
        user_table[user_table_index].axc_id    = new_entry.axc_id;
        user_table_index++;
        entry_added = TRUE;
    }
    *num_entries = user_table_index;

    return SOC_E_NONE;
}

int cprif_drv_basic_frame_usage_entry_delete(int unit, int port,
                                              uint8 table,
                                              cprimod_direction_t dir,
                                              uint8 axc_id,
                                              cprimod_basic_frame_usage_entry_t *user_table,
                                              int num_entries_in_table,
                                              int *num_entries)
{
    int table_index;
    cprif_bfa_bfp_table_entry_t table_entry;
    uint16 current_bit_location;
    int table_end;
    int user_table_index;

    current_bit_location = 0;
    user_table_index = 0;
    table_end = FALSE;

    for (table_index=0; (table_index < CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY) &&(!table_end) ; table_index++) {
        SOC_IF_ERROR_RETURN
            (cprif_drv_basic_frame_entry_get(unit, port,
                                           table, dir,
                                           table_index,
                                           &table_entry));

        if (table_entry.rsrv_bit_cnt != 0) {
            current_bit_location += table_entry.rsrv_bit_cnt;
        }

        /* if data count is non-zero, it is an active entry.*/
        if (table_entry.data_bit_cnt != 0) {
            if ((table_entry.axc_id != axc_id)&&
               (user_table_index < num_entries_in_table)) {
                /*
                 * If the AxC id is not matched, then added to the user table.
                 */
                user_table[user_table_index].start_bit = current_bit_location;
                user_table[user_table_index].num_bits  = table_entry.data_bit_cnt;
                user_table[user_table_index].axc_id    = table_entry.axc_id;
                user_table_index++;
            }
            /*
             *  Need to count bit position regardless of deleting entry or not.
             */
            current_bit_location += table_entry.data_bit_cnt;
        }

        /* if both count are 0, then it is end of the table. */
        if ((table_entry.data_bit_cnt == 0) &&
            (table_entry.rsrv_bit_cnt == 0)) {
            table_end = TRUE;
        }
    } /* for loop table_index */

    *num_entries = user_table_index;

    return SOC_E_NONE;
}

int cprif_drv_basic_frame_usage_entries_get(int unit, int port,
                                             uint8 table,
                                             cprimod_direction_t dir,
                                             uint8 axc_id,
                                             cprimod_basic_frame_usage_entry_t *user_table,
                                             int num_entries_in_table,
                                             int *num_entries )
{
    int table_index;
    cprif_bfa_bfp_table_entry_t table_entry;
    uint16 current_bit_location;
    int table_end;
    int user_table_index;

    current_bit_location = 0;
    user_table_index = 0;
    table_end = FALSE;

    for (table_index=0; (table_index<CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY) &&(!table_end) ; table_index++) {
        SOC_IF_ERROR_RETURN
            (cprif_drv_basic_frame_entry_get(unit, port,
                                           table, dir,
                                           table_index,
                                           &table_entry));

        if (table_entry.rsrv_bit_cnt != 0) {
            current_bit_location += table_entry.rsrv_bit_cnt;
        }

        /* if data count is non-zero, it is an active entry.*/
        if (table_entry.data_bit_cnt != 0) {
            /* If the AxC is match or looking for all entries  and still have table space. */
            if (((table_entry.axc_id == axc_id)||
                (CPRIMOD_AXC_ID_ALL == axc_id)) &&
                (user_table_index < num_entries_in_table)) {
                /*
                 * If the AxC id is matching, then added to the user table.
                 */
                user_table[user_table_index].start_bit = current_bit_location;
                user_table[user_table_index].num_bits  = table_entry.data_bit_cnt;
                user_table[user_table_index].axc_id    = table_entry.axc_id;
                user_table_index++;
            }
            current_bit_location += table_entry.data_bit_cnt;
        }


        /* if both count are 0, then it is end of the table. */
        if ((table_entry.data_bit_cnt == 0) &&
            (table_entry.rsrv_bit_cnt == 0)) {
            table_end = TRUE;
        }
    } /* for loop table_index */

    *num_entries = user_table_index;

    return SOC_E_NONE;
}

int cprif_drv_basic_frame_usage_table_to_hw_table_set(int unit, int port,
                                     uint8 table,
                                     cprimod_direction_t dir,
                                     int basic_frm_len,
                                     cprimod_basic_frame_usage_entry_t *user_table,
                                     int *num_entries ) /* num_entries in out */
{

    uint16 current_bit_location = 0;
    uint16 reserve_bit_cnt;
    uint16 data_bit_cnt;
    uint16 current_index = 0;
    cprif_bfa_bfp_table_entry_t table_entry;
    int table_index;
    SOC_INIT_FUNC_DEFS;

    cprif_bfa_bfp_table_entry_t_init(&table_entry);
    /*
     * Check for Overlap first.
     */

    if (*num_entries > 1) {
        for (table_index=0; (table_index < ((*num_entries)-1)) ; table_index++) {
            if ((user_table[table_index].start_bit+user_table[table_index].num_bits) >
                 user_table[table_index+1].start_bit) {
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Overlapping AxC Entries"));
            }
           if (((user_table[table_index].start_bit+user_table[table_index].num_bits) > basic_frm_len)||
               ((user_table[table_index+1].start_bit+user_table[table_index+1].num_bits) > basic_frm_len)) {
                _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Using More Bits than available for the speed.."));
            }
        }
    }

    /*
     * Clear the whole table first before programming anything
     * to get rid of stale entries.
     */
    cprif_bfa_bfp_table_entry_t_init( &table_entry);

    for (table_index=0; table_index < CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY; table_index++) {
        _SOC_IF_ERR_EXIT
            (cprif_drv_basic_frame_entry_set(unit, port,
                                             table, dir,
                                             table_index,
                                             &table_entry));
    }

    current_index = 0;
    current_bit_location = 0;

    for (table_index=0; table_index < *num_entries; table_index++) {

        if (current_bit_location < user_table[table_index].start_bit) {
            /*
             * if current bit position is less than start bit, need to add rsrv bits.
             */
            reserve_bit_cnt = user_table[table_index].start_bit - current_bit_location ;
            current_bit_location += reserve_bit_cnt;
            table_entry.axc_id = user_table[table_index].axc_id;
            table_entry.data_bit_cnt = 0;

            /* fill reserve bits first. */
            while (reserve_bit_cnt) {
                if (reserve_bit_cnt > CPRIMOD_BASIC_FRAME_MAX_BITS_PER_ENTRY) {
                    table_entry.rsrv_bit_cnt = CPRIMOD_BASIC_FRAME_MAX_BITS_PER_ENTRY ;
                } else {
                    table_entry.rsrv_bit_cnt = reserve_bit_cnt;
                }

                reserve_bit_cnt -= table_entry.rsrv_bit_cnt;
                /* Do not write the last entry, will combine with data below. */
                if (reserve_bit_cnt != 0) {
                    _SOC_IF_ERR_EXIT
                        (cprif_drv_basic_frame_entry_set(unit, port,
                                                         table, dir,
                                                         current_index,
                                                         &table_entry));
                    current_index++;
                }
            }
        }
        /*
         * Fill data bits.
         */
        data_bit_cnt = user_table[table_index].num_bits ;
        current_bit_location += data_bit_cnt;
        table_entry.axc_id = user_table[table_index].axc_id;
        if (data_bit_cnt == 0) {
            _SOC_IF_ERR_EXIT
                    (cprif_drv_basic_frame_entry_set(unit, port,
                                                     table, dir,
                                                     current_index,
                                                     &table_entry));
            current_index++;
        } else {
            while (data_bit_cnt) {
                if (data_bit_cnt > CPRIMOD_BASIC_FRAME_MAX_BITS_PER_ENTRY) {
                    table_entry.data_bit_cnt = CPRIMOD_BASIC_FRAME_MAX_BITS_PER_ENTRY;
                } else {
                    table_entry.data_bit_cnt = data_bit_cnt;
                }

                data_bit_cnt -= table_entry.data_bit_cnt;
                _SOC_IF_ERR_EXIT
                        (cprif_drv_basic_frame_entry_set(unit, port,
                                                         table, dir,
                                                         current_index,
                                                         &table_entry));
                table_entry.rsrv_bit_cnt = 0;
                current_index++;
            }
        }
    }
    /* Fill the remaning bits with reserved */
    if(current_bit_location < basic_frm_len) {
        reserve_bit_cnt = basic_frm_len-current_bit_location;
        table_entry.data_bit_cnt = 0;
        while (reserve_bit_cnt) {
            if (reserve_bit_cnt > CPRIMOD_BASIC_FRAME_MAX_BITS_PER_ENTRY) {
                table_entry.rsrv_bit_cnt = CPRIMOD_BASIC_FRAME_MAX_BITS_PER_ENTRY ;
            } else {
                table_entry.rsrv_bit_cnt = reserve_bit_cnt;
            }

            reserve_bit_cnt -= table_entry.rsrv_bit_cnt;
            _SOC_IF_ERR_EXIT
                (cprif_drv_basic_frame_entry_set(unit, port,
                                                 table, dir,
                                                 current_index,
                                                 &table_entry));
            current_index++;

        }
    }
    *num_entries = current_index;

exit:
    SOC_FUNC_RETURN;
}


#define CPRIF_DIAG_OUT(str)  LOG_CLI(str);

int _cprif_print_usage_table (cprimod_basic_frame_usage_entry_t* usage_table,
                                int num_entries )
{
    int index;
    cprimod_basic_frame_usage_entry_t* entry;

    CPRIF_DIAG_OUT(("    +-------------------------------------------+\n"));
    CPRIF_DIAG_OUT(("    | AxC ID  |  START_BIT  |  NUM BITS         |\n"));
    CPRIF_DIAG_OUT(("    +-------------------------------------------+\n"));

    for (index = 0; index < num_entries ; index++) {
        entry = &usage_table[index];
        CPRIF_DIAG_OUT(("    |   %3d   |     %3d     |      %3d          |\n", entry->axc_id,entry->start_bit, entry->num_bits));
        CPRIF_DIAG_OUT(("    +-------------------------------------------+\n"));
    }
    return SOC_E_NONE;
}

int cprif_drv_basic_frame_debug_usage(int unit, int port, uint32 axc_id, uint8 rx_table, uint8 tx_table)
{
    cprimod_basic_frame_usage_entry_t* tx_usage_table = NULL;
    cprimod_basic_frame_usage_entry_t* rx_usage_table = NULL;
    int tx_num_entries;
    int rx_num_entries;

    SOC_INIT_FUNC_DEFS;


    tx_usage_table    = (cprimod_basic_frame_usage_entry_t*)sal_alloc(sizeof(cprimod_basic_frame_usage_entry_t) *
                                                                      CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY, "cprif_drv");
    if (tx_usage_table == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_RESOURCE, ("Unable to allocate memory\n"));
    }

    rx_usage_table    = (cprimod_basic_frame_usage_entry_t*)sal_alloc(sizeof(cprimod_basic_frame_usage_entry_t) *
                                                                      CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY, "cprif_drv");
    if (rx_usage_table == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_RESOURCE, ("Unable to allocate memory\n"));
    }


    tx_num_entries = CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY;
    rx_num_entries = CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY;


    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_entries_get(unit, port,
                                                 rx_table,
                                                 CPRIMOD_DIR_RX,
                                                 axc_id,
                                                 rx_usage_table,
                                                 CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY,  /* table size */
                                                 &rx_num_entries));

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_entries_get(unit, port,
                                                 tx_table,
                                                 CPRIMOD_DIR_TX,
                                                 axc_id,
                                                 tx_usage_table,
                                                 CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY,  /* table size */
                                                 &tx_num_entries));

    CPRIF_DIAG_OUT(("Rx Basic Frame Table...\n"));
    _cprif_print_usage_table(rx_usage_table, rx_num_entries);
    CPRIF_DIAG_OUT(("Tx Basic Frame Table...\n"));
    _cprif_print_usage_table(tx_usage_table, tx_num_entries);

exit:
    if (tx_usage_table) {
        sal_free(tx_usage_table);
    }
    if (rx_usage_table) {
        sal_free(rx_usage_table);
    }
    SOC_FUNC_RETURN;
}


int cprif_drv_basic_frame_debug_raw(int unit, int port, uint32 axc_id, uint8 rx_table, uint8 tx_table)
{
    cprif_bfa_bfp_table_entry_t entry;
    int table_index;
    uint8 table_end = 0;

    SOC_INIT_FUNC_DEFS;

    CPRIF_DIAG_OUT(("Rx Basic Frame HW Table...\n"));

    CPRIF_DIAG_OUT(("    +-------------------------------------------+\n"));
    CPRIF_DIAG_OUT(("    | AxC ID  |   RSRV_BIT  |    DATA BITS      |\n"));
    CPRIF_DIAG_OUT(("    +-------------------------------------------+\n"));

    for (table_index=0; (table_index<CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY) &&(!table_end) ; table_index++) {

        SOC_IF_ERROR_RETURN
            (cprif_drv_basic_frame_entry_get(unit, port,
                                             rx_table, CPRIMOD_DIR_RX,
                                           table_index,
                                           &entry));
        if ((entry.axc_id == axc_id) ||
             (CPRIMOD_AXC_ID_ALL == axc_id)) {
            CPRIF_DIAG_OUT(("    |   %3d   |     %3d     |      %3d          |\n",
                             entry.axc_id, entry.rsrv_bit_cnt, entry.data_bit_cnt));
            CPRIF_DIAG_OUT(("    +-------------------------------------------+\n"));
        }
        /* if both count are 0, then it is end of the table. */
        if ((entry.data_bit_cnt == 0) &&
            (entry.rsrv_bit_cnt == 0)) {
            table_end = TRUE;
        }
    }

    CPRIF_DIAG_OUT(("Tx Basic Frame HW Table...\n"));
    CPRIF_DIAG_OUT(("    +-------------------------------------------+\n"));
    CPRIF_DIAG_OUT(("    | AxC ID  |   RSRV_BIT  |    DATA BITS      |\n"));
    CPRIF_DIAG_OUT(("    +-------------------------------------------+\n"));

    table_end = 0;
    for (table_index=0; (table_index<CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY) &&(!table_end) ; table_index++) {

        SOC_IF_ERROR_RETURN
            (cprif_drv_basic_frame_entry_get(unit, port,
                                             tx_table, CPRIMOD_DIR_TX,
                                             table_index,
                                             &entry));
        if ((entry.axc_id == axc_id) ||
             (CPRIMOD_AXC_ID_ALL == axc_id)) {
            CPRIF_DIAG_OUT(("    |   %3d   |     %3d     |      %3d          |\n",
                             entry.axc_id, entry.rsrv_bit_cnt, entry.data_bit_cnt));
            CPRIF_DIAG_OUT(("    +-------------------------------------------+\n"));
        }
        /* if both count are 0, then it is end of the table. */
        if ((entry.data_bit_cnt == 0) &&
            (entry.rsrv_bit_cnt == 0)) {
            table_end = TRUE;
        }
    }

    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_container_entry_set(int unit, int port,
                                      cprimod_direction_t dir,
                                      uint8 index,
                                      const cprif_cpri_container_map_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;

    soc_mem_t rx_mem_select[] = { CPRI_CPRSR_MAP_TAB_CFG_0m,   /* slice#0 */
                                  CPRI_CPRSR_MAP_TAB_CFG_1m,   /* slice#1 */
                                  CPRI_CPRSR_MAP_TAB_CFG_2m,   /* slice#2 */
                                  CPRI_CPRSR_MAP_TAB_CFG_3m }; /* slice#3 */

    soc_mem_t tx_mem_select[] = { CPRI_CA_MAP_TAB_CFG_0m,      /* slice#0 */
                                  CPRI_CA_MAP_TAB_CFG_1m,      /* slice#1 */
                                  CPRI_CA_MAP_TAB_CFG_2m,      /* slice#2 */
                                  CPRI_CA_MAP_TAB_CFG_3m };    /* slice#3 */
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (dir == CPRIMOD_DIR_TX)
        local_mem  = tx_mem_select[port_core_index];
    else
        local_mem = rx_mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    field_buf = entry->map_method;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, MAP_METHODf, &field_buf);

    field_buf = entry->axc_id;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, AXC_IDf, &field_buf);

    field_buf = entry->stuffing_at_end;
    if (local_mem  == tx_mem_select[port_core_index]) {
        soc_mem_field_set(unit, local_mem, wr_data_ptr, STUFFING_AT_ENDf, &field_buf);
    } else {
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_332_0f, &field_buf);
    }

    if (entry->map_method == CPRIF_CONTAINER_MAP_CPRI_METHOD_1) {
        field_buf = entry->stuff_cnt/2;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, STUFF_CNTf, &field_buf);
    } else if (entry->map_method == CPRIF_CONTAINER_MAP_CPRI_METHOD_3) {
        /* stuffing conut for method #3 has quotient and remainder. */
        field_buf = entry->stuff_cnt;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, STUFF_CNTf, &field_buf);
    }


    field_buf = entry->cblk_cnt/2;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, CBLK_CNTf, &field_buf);

    field_buf = entry->naxc_cnt/2;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, NAXC_CNTf, &field_buf);

    field_buf = entry->Nv;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, NVf, &field_buf);

    field_buf = entry->Na;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, NAf, &field_buf);

    field_buf = entry->rfrm_sync;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, RFRM_SYNCf, &field_buf);

    field_buf = entry->hfrm_sync;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, HFRM_SYNCf, &field_buf);

    field_buf = entry->bfn_offset;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BFN_OFFSETf, &field_buf);

    field_buf = entry->hfn_offset;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, HFN_OFFSETf, &field_buf);

    field_buf = entry->bfrm_offset;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BFRM_OFFSETf, &field_buf);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;

}

int cprif_drv_cpri_container_entry_get(int unit, int port,
                                      cprimod_direction_t dir,
                                      uint8 index,
                                      cprif_cpri_container_map_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;

    soc_mem_t rx_mem_select[] = { CPRI_CPRSR_MAP_TAB_CFG_0m,   /* slice#0 */
                                  CPRI_CPRSR_MAP_TAB_CFG_1m,   /* slice#1 */
                                  CPRI_CPRSR_MAP_TAB_CFG_2m,   /* slice#2 */
                                  CPRI_CPRSR_MAP_TAB_CFG_3m }; /* slice#3 */

    soc_mem_t tx_mem_select[] = { CPRI_CA_MAP_TAB_CFG_0m,      /* slice#0 */
                                  CPRI_CA_MAP_TAB_CFG_1m,      /* slice#1 */
                                  CPRI_CA_MAP_TAB_CFG_2m,      /* slice#2 */
                                  CPRI_CA_MAP_TAB_CFG_3m };    /* slice#3 */
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (dir == CPRIMOD_DIR_TX)
        local_mem  = tx_mem_select[port_core_index];
    else
        local_mem = rx_mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, MAP_METHODf, &field_buf);
    entry->map_method = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, AXC_IDf, &field_buf);
    entry->axc_id = field_buf;

    if (local_mem  == tx_mem_select[port_core_index]) {
        soc_mem_field_get(unit, local_mem, rd_data_ptr, STUFFING_AT_ENDf, &field_buf);
    } else {
        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_332_0f, &field_buf);
    }
    entry->stuffing_at_end = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, STUFF_CNTf, &field_buf);
    if (entry->map_method == CPRIF_CONTAINER_MAP_CPRI_METHOD_3) {
        entry->stuff_cnt = field_buf;
    } else {
        entry->stuff_cnt = field_buf * 2;
    }

    soc_mem_field_get(unit, local_mem, rd_data_ptr,CBLK_CNTf , &field_buf);
    entry->cblk_cnt = field_buf * 2;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, NAXC_CNTf , &field_buf);
    entry->naxc_cnt = field_buf * 2;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, NVf, &field_buf);
    entry->Nv = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, NAf, &field_buf);
    entry->Na= field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, RFRM_SYNCf, &field_buf);
    entry->rfrm_sync = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, HFRM_SYNCf, &field_buf);
    entry->hfrm_sync = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BFN_OFFSETf, &field_buf);
    entry->bfn_offset = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, HFN_OFFSETf, &field_buf);
    entry->hfn_offset = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BFRM_OFFSETf, &field_buf);
    entry->bfrm_offset = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;

}


int cprif_drv_cpri_container_parser_sync_info_swap (int unit, int port,
                                                    uint8 index,
                                                    cprif_container_parser_sync_info_t *sync_info)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;

    soc_mem_t rx_mem_select[] = { CPRI_CPRSR_MAP_TAB_CFG_0m,   /* slice#0 */
                                  CPRI_CPRSR_MAP_TAB_CFG_1m,   /* slice#1 */
                                  CPRI_CPRSR_MAP_TAB_CFG_2m,   /* slice#2 */
                                  CPRI_CPRSR_MAP_TAB_CFG_3m }; /* slice#3 */

    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 old_value;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = rx_mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, wr_data_ptr));

    soc_mem_field_get(unit, local_mem, wr_data_ptr, RFRM_SYNCf, &old_value);
    field_buf = sync_info->rfrm_sync;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, RFRM_SYNCf, &field_buf);
    sync_info->rfrm_sync = old_value;

    soc_mem_field_get(unit, local_mem, wr_data_ptr, HFRM_SYNCf, &old_value);
    field_buf = sync_info->hfrm_sync;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, HFRM_SYNCf, &field_buf);
    sync_info->hfrm_sync = old_value;

    soc_mem_field_get(unit, local_mem, wr_data_ptr, HFN_OFFSETf, &old_value);
    field_buf = sync_info->hfn_offset;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, HFN_OFFSETf, &field_buf);
    sync_info->hfn_offset = old_value;

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;

}


int cprif_drv_rsvd4_container_entry_set(int unit, int port,
                                      cprimod_direction_t dir,
                                      uint8 index,
                                      const cprif_rsvd4_container_map_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t rx_mem_select[] = { CPRI_CPRSR_MAP_TAB_CFG_0m,   /* slice#0 */
                                  CPRI_CPRSR_MAP_TAB_CFG_1m,   /* slice#1 */
                                  CPRI_CPRSR_MAP_TAB_CFG_2m,   /* slice#2 */
                                  CPRI_CPRSR_MAP_TAB_CFG_3m }; /* slice#3 */

    soc_mem_t tx_mem_select[] = { CPRI_CA_MAP_TAB_CFG_0m,      /* slice#0 */
                                  CPRI_CA_MAP_TAB_CFG_1m,      /* slice#1 */
                                  CPRI_CA_MAP_TAB_CFG_2m,      /* slice#2 */
                                  CPRI_CA_MAP_TAB_CFG_3m };    /* slice#3 */
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (dir == CPRIMOD_DIR_TX)
        local_mem  = tx_mem_select[port_core_index];
    else
        local_mem = rx_mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);


    field_buf = entry->map_method;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, MAP_METHODf, &field_buf);

    field_buf = entry->axc_id;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, AXC_IDf, &field_buf);

    if (dir == CPRIMOD_DIR_TX) {
        field_buf = entry->stuffing_at_end;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, STUFFING_AT_ENDf, &field_buf);

        field_buf = entry->stuff_cnt/2;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_17_0f, &field_buf);

        field_buf = entry->cblk_cnt/2;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_17_1f, &field_buf);

        field_buf = entry->rfrm_offset & 0xFFFFF; /* lower 20 bits only */
        soc_mem_field_set(unit, local_mem, wr_data_ptr, RFRM_OFFSET_LOWf, &field_buf);

        field_buf = (entry->rfrm_offset >> 20) & 0xFF; /* middle 8 bits only */
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_17_8f, &field_buf);

        field_buf = (entry->rfrm_offset >> 28) & 0xF; /* upper 4 bits only */
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_17_11f, &field_buf);

        field_buf = entry->use_ts_dbm;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_17_2f, &field_buf);

        field_buf = entry->ts_dbm_prof_num;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_17_3f, &field_buf);

        field_buf = entry->num_active_slots;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_17_4f, &field_buf);

        field_buf = entry->msg_addr;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_17_5f, &field_buf);

        field_buf = entry->msg_type;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_17_7f, &field_buf);

        field_buf = entry->msg_ts_cnt;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_17_9f, &field_buf);

        field_buf = entry->msg_ts_offset;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_17_10f, &field_buf);

        field_buf = entry->msg_ts_mode;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_17_6f, &field_buf);

    } else {
        field_buf = entry->stuffing_at_end;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_332_0f, &field_buf);

        field_buf = entry->stuff_cnt/2;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_332_1f, &field_buf);

        field_buf = entry->cblk_cnt/2;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, CBLK_CNTf, &field_buf);

        field_buf = entry->rfrm_offset & 0xFFFFF; /* lower 20 bits only */
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_332_2f, &field_buf);

        field_buf = (entry->rfrm_offset >> 20) & 0xFFF; /* upper 12 bits only */
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_332_3f, &field_buf);

        field_buf = entry->msg_ts_mode;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_332_4f, &field_buf);

        field_buf = entry->msg_ts_sync;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_332_5f, &field_buf);

        field_buf = entry->msg_ts_cnt;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_332_6f, &field_buf);

    }


    field_buf = entry->hfrm_sync;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, HFRM_SYNCf, &field_buf);

    field_buf = entry->rfrm_sync;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, RFRM_SYNCf, &field_buf);


    field_buf = entry->bfn_offset;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BFN_OFFSETf, &field_buf);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));
exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;

}

int cprif_drv_rsvd4_container_entry_get(int unit, int port,
                                         cprimod_direction_t dir,
                                         uint8 index,
                                         cprif_rsvd4_container_map_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t rx_mem_select[] = { CPRI_CPRSR_MAP_TAB_CFG_0m,   /* slice#0 */
                                  CPRI_CPRSR_MAP_TAB_CFG_1m,   /* slice#1 */
                                  CPRI_CPRSR_MAP_TAB_CFG_2m,   /* slice#2 */
                                  CPRI_CPRSR_MAP_TAB_CFG_3m }; /* slice#3 */

    soc_mem_t tx_mem_select[] = { CPRI_CA_MAP_TAB_CFG_0m,      /* slice#0 */
                                  CPRI_CA_MAP_TAB_CFG_1m,      /* slice#1 */
                                  CPRI_CA_MAP_TAB_CFG_2m,      /* slice#2 */
                                  CPRI_CA_MAP_TAB_CFG_3m };    /* slice#3 */
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (dir == CPRIMOD_DIR_TX)
        local_mem  = tx_mem_select[port_core_index];
    else
        local_mem = rx_mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, MAP_METHODf, &field_buf);
    entry->map_method = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, AXC_IDf, &field_buf);
    entry->axc_id = field_buf;

    if (dir == CPRIMOD_DIR_TX) {
        soc_mem_field_get(unit, local_mem, rd_data_ptr, STUFFING_AT_ENDf, &field_buf);
        entry->stuffing_at_end  = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_17_0f, &field_buf);
        entry->stuff_cnt = field_buf * 2;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_17_1f, &field_buf);
        entry->cblk_cnt = field_buf * 2;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, RFRM_OFFSET_LOWf, &field_buf);
        entry->rfrm_offset = field_buf & 0xFFFFF;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_17_8f, &field_buf);
        entry->rfrm_offset |= ((field_buf & 0xFF) << 20);

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_17_11f, &field_buf);
        entry->rfrm_offset |= ((field_buf & 0xF) << 28);

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_17_2f, &field_buf);
        entry->use_ts_dbm = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_17_3f, &field_buf);
        entry->ts_dbm_prof_num = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_17_4f, &field_buf);
        entry->num_active_slots = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_17_5f, &field_buf);
        entry->msg_addr = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_17_7f, &field_buf);
        entry->msg_type = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_17_10f, &field_buf);
        entry->msg_ts_offset = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_17_9f, &field_buf);
        entry->msg_ts_cnt = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_17_6f, &field_buf);
        entry->msg_ts_mode = field_buf;

    } else {
        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_332_0f, &field_buf);
        entry->stuffing_at_end  = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_332_1f, &field_buf);
        entry->stuff_cnt = field_buf * 2;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, CBLK_CNTf, &field_buf);
        entry->cblk_cnt = field_buf * 2;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_332_2f, &field_buf);
        entry->rfrm_offset = field_buf & 0xFFFFF;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_332_3f, &field_buf);
        entry->rfrm_offset |= ((field_buf & 0xFFF) << 20);


        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_332_5f, &field_buf);
        entry->msg_ts_sync = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_332_6f, &field_buf);
        entry->msg_ts_cnt = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_332_4f, &field_buf);
        entry->msg_ts_mode = field_buf;

    }


    soc_mem_field_get(unit, local_mem, rd_data_ptr, HFRM_SYNCf, &field_buf);
    entry->hfrm_sync = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, RFRM_SYNCf, &field_buf);
    entry->rfrm_sync = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BFN_OFFSETf, &field_buf);
    entry->bfn_offset = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;

}

int cprif_container_map_print_cpri_entry (cprif_cpri_container_map_entry_t* entry)
{

    CPRIF_DIAG_OUT(("  0x%03x  0x%03x 0x%03x    0x%03x  0x%03x 0x%03x     0x%03x  0x%03x 0x%03x    0x%03x  0x%03x 0x%03x  0x%03x \n",
                            entry->map_method,
                            entry->axc_id,
                            entry->stuffing_at_end,
                            entry->stuff_cnt,
                            entry->cblk_cnt,
                            entry->naxc_cnt,
                            entry->Nv,
                            entry->Na,
                            entry->rfrm_sync,
                            entry->hfrm_sync,
                            entry->bfn_offset,
                            entry->hfn_offset,
                            entry->bfrm_offset
                            ));
    return SOC_E_NONE;
}

int cprif_container_map_print_rsvd4_entry (cprif_rsvd4_container_map_entry_t* entry)
{

    CPRIF_DIAG_OUT(("  0x%03x  0x%03x 0x%03x    0x%03x  0x%03x  0x%01x x0x%01x 0x%01x  0x%03x   0x%03x  0x%03x 0x%03x   0x%03x  0x%03x  0x%03x   0x%03x  0x%03x 0x%03x  0x%03x \n",
                            entry->map_method,
                            entry->axc_id,
                            entry->stuffing_at_end,
                            entry->stuff_cnt,
                            entry->cblk_cnt,
                            entry->rfrm_sync,
                            entry->hfrm_sync,
                            entry->msg_ts_sync,
                            entry->bfn_offset,
                            entry->rfrm_offset,
                            entry->bfn_offset,
                            entry->msg_ts_mode,
                            entry->msg_ts_cnt,
                            entry->use_ts_dbm,
                            entry->ts_dbm_prof_num,
                            entry->num_active_slots,
                            entry->msg_addr,
                            entry->msg_type,
                            entry->msg_ts_offset
                            ));
    return SOC_E_NONE;
}

int cprif_container_map_debug(int unit, int port, uint8 axc_id)
{
    cprif_cpri_container_map_entry_t cpri_entry;
    cprif_rsvd4_container_map_entry_t rsvd4_entry;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprif_drv_cpri_container_entry_get(unit, port,
                                                      CPRIMOD_DIR_RX,
                                                      axc_id,
                                                      &cpri_entry));

    if (cpri_entry.map_method == CPRIF_CONTAINER_MAP_RSVD4) {

    CPRIF_DIAG_OUT(("    +------------------------------------------------------------------------------------------------------------------------------------+\n"));
    CPRIF_DIAG_OUT(("    | Map M | AxC | S END | S CNT | CBLK CNT | SYNC rhm | BFN | RFRM | TS_MODE | TS CNT | DBM EN | Prof | ASlot | Addr | Type | TS Offset|\n"));
    CPRIF_DIAG_OUT(("    +------------------------------------------------------------------------------------------------------------------------------------+\n"));

        _SOC_IF_ERR_EXIT
            (cprif_drv_rsvd4_container_entry_get(unit, port,
                                              CPRIMOD_DIR_RX,
                                              axc_id,
                                              &rsvd4_entry));

        _SOC_IF_ERR_EXIT
            (cprif_drv_rsvd4_container_entry_get(unit, port,
                                              CPRIMOD_DIR_TX,
                                              axc_id,
                                              &rsvd4_entry));

    } else {

    CPRIF_DIAG_OUT(("    +----------------------------------------------------------------------------------------+\n"));
    CPRIF_DIAG_OUT(("    | Map M | AxC | S END | S CNT | CBLK CNT | Naxc | Nv | Na | SYNC rh | BFN | HFN  |  BFRM |\n"));
    CPRIF_DIAG_OUT(("    +---------------------------------------------------------------------------------------+\n"));


        _SOC_IF_ERR_EXIT
            (cprif_drv_cpri_container_entry_get(unit, port,
                                              CPRIMOD_DIR_TX,
                                              axc_id,
                                              &cpri_entry));


    }

exit:
    SOC_FUNC_RETURN;
}


/*
 *
 *  IQ Packing/Unpacking
 */
int cprif_drv_iq_buffer_config_set(int unit, int port,
                                    cprimod_direction_t dir,
                                    uint8 index,
                                    cprif_iq_buffer_config_t* entry)
{
    uint32 *wr_data_ptr;
    soc_mem_t local_mem;
    int blk;
    int phy_port;
    int port_core_index;
    uint32 field_buf;
    uint32 num_words;

    soc_mem_t rx_mem_select[] = { CPRI_CPRSR_IQ_PK_BUFF_CFG_0m,   /* slice#0 */
                                  CPRI_CPRSR_IQ_PK_BUFF_CFG_1m,   /* slice#1 */
                                  CPRI_CPRSR_IQ_PK_BUFF_CFG_2m,   /* slice#2 */
                                  CPRI_CPRSR_IQ_PK_BUFF_CFG_3m }; /* slice#3 */
    soc_mem_t tx_mem_select[] = { CPRI_CA_PAY_TAB_CFG_0m,   /* slice#0 */
                                  CPRI_CA_PAY_TAB_CFG_1m,   /* slice#1 */
                                  CPRI_CA_PAY_TAB_CFG_2m,   /* slice#2 */
                                  CPRI_CA_PAY_TAB_CFG_3m }; /* slice#3 */
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (dir == CPRIMOD_DIR_TX) {
        local_mem = tx_mem_select[port_core_index];
    } else {
        local_mem = rx_mem_select[port_core_index];
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Convert the bits into number of sample pair.
     */
    field_buf = entry->payload_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, PLD_SIZEf, &field_buf);

    field_buf = entry->last_packet_num;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, NTH_PLD_IDf, &field_buf);

    /*
     * Convert the bits into number of sample pair.
     */
    field_buf = entry->last_payload_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, NTH_PLD_SIZEf, &field_buf);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_iq_buffer_config_get(int unit, int port,
                                    cprimod_direction_t dir,
                                    uint8 index,
                                    cprif_iq_buffer_config_t* entry)
{
    uint32 *rd_data_ptr;
    soc_mem_t local_mem;
    int blk;
    int phy_port;
    int port_core_index;
    uint32 field_buf;
    uint32 num_words;

    soc_mem_t rx_mem_select[] = { CPRI_CPRSR_IQ_PK_BUFF_CFG_0m,   /* slice#0 */
                                  CPRI_CPRSR_IQ_PK_BUFF_CFG_1m,   /* slice#1 */
                                  CPRI_CPRSR_IQ_PK_BUFF_CFG_2m,   /* slice#2 */
                                  CPRI_CPRSR_IQ_PK_BUFF_CFG_3m }; /* slice#3 */
    soc_mem_t tx_mem_select[] = { CPRI_CA_PAY_TAB_CFG_0m,   /* slice#0 */
                                  CPRI_CA_PAY_TAB_CFG_1m,   /* slice#1 */
                                  CPRI_CA_PAY_TAB_CFG_2m,   /* slice#2 */
                                  CPRI_CA_PAY_TAB_CFG_3m }; /* slice#3 */
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (dir == CPRIMOD_DIR_TX) {
        local_mem = tx_mem_select[port_core_index];
    } else {
        local_mem = rx_mem_select[port_core_index];
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, PLD_SIZEf, &field_buf);
    entry->payload_size  = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, NTH_PLD_IDf, &field_buf);
    entry->last_packet_num = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, NTH_PLD_SIZEf, &field_buf);
    entry->last_payload_size = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_iq_buffer_debug(int unit, int port, uint8 axc_id)
{
    cprif_iq_buffer_config_t entry;
    SOC_INIT_FUNC_DEFS;

    cprif_iq_buffer_config_t_init(&entry);

    CPRIF_DIAG_OUT(("    +---------------------------------------------------------------------+\n"));
    CPRIF_DIAG_OUT(("    |  AxC      | Payload Size  | Last_Pkt_Num | Last Payload Size        |\n"));
    CPRIF_DIAG_OUT(("    +---------------------------------------------------------------------+\n"));

    _SOC_IF_ERR_EXIT(cprif_drv_iq_buffer_config_get(unit, port,
                                                CPRIMOD_DIR_RX,
                                                axc_id,
                                                &entry));

    CPRIF_DIAG_OUT(("  Rx 0x%03x 0x%03x  0x%03x 0x%03x\n", axc_id, entry.payload_size,entry.last_packet_num, entry.last_payload_size));

    _SOC_IF_ERR_EXIT(cprif_drv_iq_buffer_config_get(unit, port,
                                                CPRIMOD_DIR_TX,
                                                axc_id,
                                                &entry));

    CPRIF_DIAG_OUT(("  Tx 0x%03x 0x%03x  0x%03x 0x%03x\n", axc_id, entry.payload_size,entry.last_packet_num, entry.last_payload_size));
exit:
    SOC_FUNC_RETURN;
}


/* this is for cprif_drv_decap_data_entry_set as well */
int cprif_drv_encap_data_entry_set(int unit, int port,
                                    cprimod_direction_t dir,
                                    uint32 queue_num,
                                    const cprif_encap_decap_data_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t rx_mem_select[] = { CPRI_ENCAP_DATA_0m,
                                  CPRI_ENCAP_DATA_1m,
                                  CPRI_ENCAP_DATA_2m,
                                  CPRI_ENCAP_DATA_3m };

    soc_mem_t tx_mem_select[] = { CPRI_DECAP_DATA_0m,
                                  CPRI_DECAP_DATA_1m,
                                  CPRI_DECAP_DATA_2m,
                                  CPRI_DECAP_DATA_3m };


    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (dir == CPRIMOD_DIR_TX) {
        local_mem = tx_mem_select[port_core_index];
    } else {
        local_mem = rx_mem_select[port_core_index];
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read existing configuration since not all the fields will be set
     * here.
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, queue_num, wr_data_ptr));

    field_buf = entry->sample_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_SAMPLE_SIZEf, &field_buf);

    if (dir == CPRIMOD_DIR_RX) {

        field_buf = entry->valid;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_VALIDf, &field_buf);

        field_buf = entry->mux_enable;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_DEMUX_ENf, &field_buf);

        field_buf = entry->queue_size;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_SIZEf, &field_buf);

        field_buf = entry->work_queue_select;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_WQ_SELf, &field_buf);

    } else {
        field_buf = entry->mux_enable;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_MUX_ENf, &field_buf);

        field_buf = entry->queue_size;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_BUF_SIZEf, &field_buf);

        field_buf = entry->tx_cycle_size;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_CYC_SIZEf, &field_buf);


    }

    field_buf = entry->out_sample_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_OUT_SAMPLE_SIZEf, &field_buf);


    field_buf = entry->queue_offset;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_OFFSETf, &field_buf);


    field_buf = entry->sign_ext_enable;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_SIGN_EXDf, &field_buf);

    field_buf = entry->sign_ext_type;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_SIGN_EXD_TYPEf, &field_buf);

    field_buf = entry->bit_reversal;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_BIT_REVERSALf, &field_buf);


    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, queue_num, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;

}

int cprif_drv_encap_data_entry_get(int unit, int port,
                                 cprimod_direction_t dir,
                                 uint32 queue_num,
                                 cprif_encap_decap_data_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t rx_mem_select[] = { CPRI_ENCAP_DATA_0m,
                                  CPRI_ENCAP_DATA_1m,
                                  CPRI_ENCAP_DATA_2m,
                                  CPRI_ENCAP_DATA_3m };

    soc_mem_t tx_mem_select[] = { CPRI_DECAP_DATA_0m,
                                  CPRI_DECAP_DATA_1m,
                                  CPRI_DECAP_DATA_2m,
                                  CPRI_DECAP_DATA_3m };


    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (dir == CPRIMOD_DIR_TX) {
        local_mem = tx_mem_select[port_core_index];
    } else {
        local_mem = rx_mem_select[port_core_index];
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, queue_num, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_SAMPLE_SIZEf, &field_buf);
    entry->sample_size = field_buf;

    if (dir == CPRIMOD_DIR_RX) {

        soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_VALIDf, &field_buf);
        entry->valid = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_DEMUX_ENf, &field_buf);
        entry->mux_enable = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_SIZEf, &field_buf);
        entry->queue_size = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_WQ_SELf, &field_buf);
        entry->work_queue_select = field_buf;
    } else {
        soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_MUX_ENf, &field_buf);
        entry->mux_enable = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_BUF_SIZEf, &field_buf);
        entry->queue_size = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_CYC_SIZEf, &field_buf);
        entry->tx_cycle_size = field_buf;

    }

    soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_OUT_SAMPLE_SIZEf, &field_buf);
    entry->out_sample_size = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_OFFSETf, &field_buf);
    entry->queue_offset = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_SIGN_EXDf, &field_buf);
    entry->sign_ext_enable = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_SIGN_EXD_TYPEf, &field_buf);
    entry->sign_ext_type = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_BIT_REVERSALf, &field_buf);
    entry->bit_reversal= field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;

}

/* for both encap and decap allocation can use this routine */
int cprif_drv_encap_data_allocate_buffer(int unit, int port,
                                         cprimod_direction_t dir,
                                         uint16 size_in_bytes,
                                         uint16* offset)
{

    cprif_encap_decap_data_entry_t entry;
    uint32 *usage_map = NULL;
    uint8 queue;
    uint16 total_buf;
    uint16 num_words;
    uint16 num_entries;
    uint16 index;
    uint16 first_location;
    uint16 queue_entry_end_loc;
    uint8  location_found;
    SOC_INIT_FUNC_DEFS;


    if (dir == CPRIMOD_DIR_RX) {
        total_buf = CPRIF_ENCAP_BUFFER_NUM_ENTRIES;
    } else {
        total_buf = CPRIF_DECAP_BUFFER_NUM_ENTRIES;
    }
    /*
     * Allocate the memory to keep track of buffer usage.
     * Fill every location with 0 to mark as available.
     */
    num_words = CPRIF_NUM_PBMP_WORDS(total_buf);
    usage_map = (uint32*)sal_alloc((sizeof(uint32) * num_words), "cprif_drv");
    CPRIMOD_NULL_CHECK(usage_map);
    sal_memset(usage_map, 0, sizeof(uint32) * num_words);
    /*
     * Marked all the buffer entries that are allocated to the queue.
     */
    for (queue = 0; queue < CPRIF_MAX_NUM_OF_QUEUES; queue++) {
        _SOC_IF_ERR_EXIT
            (cprif_drv_encap_data_entry_get(unit, port, dir, queue, &entry));
        if (entry.queue_size != 0) {
            if(dir == CPRIMOD_DIR_TX) {
                queue_entry_end_loc = entry.queue_offset+(entry.queue_size
                                      * ((uint16)entry.tx_cycle_size));
            } else {
                queue_entry_end_loc = entry.queue_offset+entry.queue_size;
            }
            for (index = entry.queue_offset; index < queue_entry_end_loc; index++) {
                CPRIF_PBMP_IDX_MARK(usage_map,index);
            }
        }
    }
    /*
     * Find the buffer location that can accomodate num_entries.
     */
    index = 0;
    first_location = 0;
    location_found = FALSE;
    num_entries =  CPRIF_NUM_OF_BUFFER_ENTRIES(size_in_bytes);

    while (num_entries && (index < total_buf)) {
        num_entries =  CPRIF_NUM_OF_BUFFER_ENTRIES(size_in_bytes);
        /*
         * Find first available spot.
         */
        for ( ;index < total_buf; index++) {
            if (CPRIF_PBMP_AVAILABLE(usage_map, index)) {
                first_location = index;
                location_found  = TRUE;
                break;
            }
        } /* for */
        /*
         * Check that spot has enough available space.
         */
        if (location_found) {
            for ( ;(index < total_buf) && (num_entries > 0) ; index++) {
                if (CPRIF_PBMP_AVAILABLE(usage_map, index)){
                    num_entries--;
                } else {
                    location_found = FALSE;
                    break;
                }
            } /* for */
        }
    } /* while */

    if (num_entries == 0) {
        *offset = first_location;
    } else {
        _SOC_EXIT_WITH_ERR
            (SOC_E_RESOURCE, ("Unable To Allocate Buffer\n"));
    }
exit:
    if (usage_map) {
        sal_free(usage_map);
    }
    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_encap_data_entry_set(int unit, int port,
                                 cprimod_direction_t dir,
                                 uint32 queue_num,
                                 const cprif_rsvd4_encap_decap_data_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t rx_mem_select[] = { CPRI_ENCAP_DATA_0m,
                                  CPRI_ENCAP_DATA_1m,
                                  CPRI_ENCAP_DATA_2m,
                                  CPRI_ENCAP_DATA_3m };

    soc_mem_t tx_mem_select[] = { CPRI_DECAP_DATA_0m,
                                  CPRI_DECAP_DATA_1m,
                                  CPRI_DECAP_DATA_2m,
                                  CPRI_DECAP_DATA_3m };


    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (dir == CPRIMOD_DIR_TX) {
        local_mem = tx_mem_select[port_core_index];
    } else {
        local_mem = rx_mem_select[port_core_index];
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read existing configuration since not all the fields will be set
     * here.
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, queue_num, wr_data_ptr));

    if (dir == CPRIMOD_DIR_RX) {

        field_buf = entry->valid;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_VALIDf, &field_buf);

        field_buf = 0;  /* mux/demux is always off. */
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_DEMUX_ENf, &field_buf);

        field_buf = (entry->queue_size+CPRIF_DATA_BUFFER_BLOCK_SIZE-1)/CPRIF_DATA_BUFFER_BLOCK_SIZE;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_SIZEf, &field_buf);

        field_buf = entry->work_queue_select;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_WQ_SELf, &field_buf);

        field_buf = entry->gsm_control_location;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_331_3f, &field_buf);

        field_buf = 0;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_SIGN_EXDf, &field_buf);

        field_buf = entry->gsm_pad_enable;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_331_2f, &field_buf);

    } else {
        field_buf = 0; /* Mux Demux is off. */
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_MUX_ENf, &field_buf);

        field_buf = ((entry->queue_size+CPRIF_DATA_BUFFER_BLOCK_SIZE-1)/CPRIF_DATA_BUFFER_BLOCK_SIZE)+1;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_BUF_SIZEf, &field_buf);

        field_buf = entry->tx_cycle_size;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_CYC_SIZEf, &field_buf);

        field_buf = entry->gsm_pad_enable;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_64_2f, &field_buf);

        if(entry->gsm_pad_enable != 0){
          field_buf = entry->gsm_control_location;
          soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_64_3f, &field_buf);
        }else {
          field_buf = 0;
          soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_SIGN_EXDf, &field_buf);
        }
    }

    field_buf = entry->queue_offset;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_OFFSETf, &field_buf);


    field_buf = 0;  /* Sign extension is off */
    soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_SIGN_EXD_TYPEf, &field_buf);

    field_buf = entry->bit_reversal;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_BIT_REVERSALf, &field_buf);

    if(queue_num >= CPRIF_MAX_NUM_OF_DATA_QUEUES) {
        field_buf = 16;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_SAMPLE_SIZEf, &field_buf);

        field_buf = 16;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_OUT_SAMPLE_SIZEf, &field_buf);
    } else {
        if(entry->gsm_pad_enable != 0){

          if (dir == CPRIMOD_DIR_RX) {
            field_buf = entry->gsm_pad_size;
            soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_331_0f, &field_buf);

            field_buf = entry->gsm_extra_pad_size;
            soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_331_1f, &field_buf);

          } else {
            field_buf = entry->gsm_pad_size;
            soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_64_0f, &field_buf);

            field_buf = entry->gsm_extra_pad_size;
            soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_64_1f, &field_buf);

          }

        } else {
          field_buf = 8;
          soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_SAMPLE_SIZEf, &field_buf);

          field_buf = 8;
          soc_mem_field_set(unit, local_mem, wr_data_ptr, Q_OUT_SAMPLE_SIZEf, &field_buf);
        }

    }

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, queue_num, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;

}

int cprif_drv_rsvd4_encap_data_entry_get(int unit, int port,
                                 cprimod_direction_t dir,
                                 uint32 queue_num,
                                 cprif_rsvd4_encap_decap_data_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t rx_mem_select[] = { CPRI_ENCAP_DATA_0m,
                                  CPRI_ENCAP_DATA_1m,
                                  CPRI_ENCAP_DATA_2m,
                                  CPRI_ENCAP_DATA_3m };

    soc_mem_t tx_mem_select[] = { CPRI_DECAP_DATA_0m,
                                  CPRI_DECAP_DATA_1m,
                                  CPRI_DECAP_DATA_2m,
                                  CPRI_DECAP_DATA_3m };


    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (dir == CPRIMOD_DIR_TX) {
        local_mem = tx_mem_select[port_core_index];
    } else {
        local_mem = rx_mem_select[port_core_index];
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, queue_num, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_64_0f, &field_buf);
    entry->gsm_pad_size = field_buf;

    if (dir == CPRIMOD_DIR_RX) {

        soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_VALIDf, &field_buf);
        entry->valid = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_SIZEf, &field_buf);
        entry->queue_size = field_buf * CPRIF_DATA_BUFFER_BLOCK_SIZE;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_WQ_SELf, &field_buf);
        entry->work_queue_select = field_buf;
    } else {

        soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_BUF_SIZEf, &field_buf);
        entry->queue_size = field_buf * CPRIF_DATA_BUFFER_BLOCK_SIZE;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_CYC_SIZEf, &field_buf);
        entry->tx_cycle_size = field_buf;

    }

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_64_1f, &field_buf);
    entry->gsm_extra_pad_size = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, Q_OFFSETf, &field_buf);
    entry->queue_offset = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_64_2f, &field_buf);
    entry->gsm_pad_enable = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_64_3f, &field_buf);
    entry->gsm_control_location = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;

}


int cprif_drv_encap_header_entry_set(int unit, int port,
                                      uint32 queue_num,
                                      uint32 flags,
                                      const cprif_encap_header_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_QUE_ETH_HDR_0m,
                               CPRI_ENCAP_QUE_ETH_HDR_1m,
                               CPRI_ENCAP_QUE_ETH_HDR_2m,
                               CPRI_ENCAP_QUE_ETH_HDR_3m };
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read existing configuration since not all the fields will be set
     * here.
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, queue_num, wr_data_ptr));

    if (flags & CPRIF_ENCAP_HEADER_FLAGS_HDR_CONFIG_SET) {
        field_buf = entry->header_type;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_TYPEf, &field_buf);

        field_buf = entry->ver;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_VERf , &field_buf);

        field_buf = entry->pkttype;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_PKTTYPEf, &field_buf);

        field_buf = entry->ordering_info_index;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_ORDERINGINFO_INDEXf, &field_buf);

        field_buf = entry->mac_da_index;;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_DA_IDf, &field_buf);

        field_buf = entry->mac_sa_index;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_SA_IDf, &field_buf);

        field_buf = entry->vlan_type;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_VLAN_TYPEf, &field_buf);

        field_buf = entry->vlan_id_0_index;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_VLAN_ID_0f, &field_buf);

        field_buf = entry->vlan_id_1_index;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_VLAN_ID_1f, &field_buf);

        field_buf = entry->vlan_0_priority;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_VLAN_PRI_0f, &field_buf);

        field_buf = entry->vlan_1_priority;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_VLAN_PRI_1f, &field_buf);

        field_buf = entry->ether_type_index;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_VLAN_ETHER_TYPE_INDXf, &field_buf);

        field_buf = entry->use_tagid_for_vlan;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_USE_TAG_ID_FOR_VLANf, &field_buf);

        field_buf = entry->use_tagid_for_flowid;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_USE_TAG_ID_FOR_FLOWIDf, &field_buf);

        field_buf = entry->flow_id & 0xFF;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_FLOWIDf , &field_buf);

        field_buf = entry->use_opcode;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_USE_SUBTYPEf, &field_buf);

        field_buf = entry->roe_opcode;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_HDR_SUBTYPEf, &field_buf);
    }

    if (flags & CPRIF_ENCAP_HEADER_FLAGS_GSM_CONFIG_SET) {
        if(entry->tsn_bitmap != 0){
          field_buf = entry->tsn_bitmap;
          soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_334_1f, &field_buf);
        }
        /* ignore this for now.
        field_buf = entry->gsm_q_cnt_disable;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_334_2f, &field_buf);
        */
    }
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, queue_num, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_encap_header_entry_get(int unit, int port,
                                      uint32 queue_num,
                                      cprif_encap_header_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_QUE_ETH_HDR_0m,
                               CPRI_ENCAP_QUE_ETH_HDR_1m,
                               CPRI_ENCAP_QUE_ETH_HDR_2m,
                               CPRI_ENCAP_QUE_ETH_HDR_3m };
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read configuration
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, queue_num, rd_data_ptr));


    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_TYPEf, &field_buf);
    entry->header_type = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_FLOWIDf , &field_buf);
    entry->flow_id = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_VERf , &field_buf);
    entry->ver = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_PKTTYPEf, &field_buf);
    entry->pkttype = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_ORDERINGINFO_INDEXf, &field_buf);
    entry->ordering_info_index = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_DA_IDf, &field_buf);
    entry->mac_da_index = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_SA_IDf, &field_buf);
    entry->mac_sa_index = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_VLAN_TYPEf, &field_buf);
    entry->vlan_type = field_buf;

    field_buf = entry->vlan_id_0_index;
    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_VLAN_ID_0f, &field_buf);
    entry->vlan_id_0_index = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_VLAN_ID_1f, &field_buf);
    entry->vlan_id_1_index = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_VLAN_PRI_0f, &field_buf);
    entry->vlan_0_priority = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_VLAN_PRI_1f, &field_buf);
    entry->vlan_1_priority = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_VLAN_ETHER_TYPE_INDXf, &field_buf);
    entry->ether_type_index = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_USE_TAG_ID_FOR_VLANf, &field_buf);
    entry->use_tagid_for_vlan = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_USE_TAG_ID_FOR_FLOWIDf, &field_buf);
    entry->use_tagid_for_flowid = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_USE_SUBTYPEf, &field_buf);
    entry->use_opcode = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_HDR_SUBTYPEf, &field_buf);
    entry->roe_opcode = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_334_1f, &field_buf);
    entry->tsn_bitmap = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_334_2f, &field_buf);
    entry->gsm_q_cnt_disable = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}



int cprif_drv_cprimod_to_cprif_ordering_info_type (cprimod_ordering_info_type_t cprimod_type, uint8* cprif_type)
{
    switch (cprimod_type) {
        case cprimod_ordering_info_type_roe_sequence:
            *cprif_type = CPRIF_ORDERING_INFO_TYPE_ROE;
            break;

        case cprimod_ordering_info_type_bfn_for_qcnt:
            *cprif_type = CPRIF_ORDERING_INFO_TYPE_BFN_FOR_QCNT;
            break;

        case cprimod_ordering_info_type_use_pinfo:
            *cprif_type = CPRIF_ORDERING_INFO_TYPE_USE_PINFO;
            break;
        default:
            break;
    }
    return SOC_E_NONE;

}

int cprif_drv_cprif_to_cprimod_ordering_info_type (uint8 cprif_type , cprimod_ordering_info_type_t *cprimod_type )
{
    switch (cprif_type) {
        case CPRIF_ORDERING_INFO_TYPE_ROE :
            *cprimod_type = cprimod_ordering_info_type_roe_sequence;
            break;

        case CPRIF_ORDERING_INFO_TYPE_BFN_FOR_QCNT :
            *cprimod_type = cprimod_ordering_info_type_bfn_for_qcnt;
            break;

        case CPRIF_ORDERING_INFO_TYPE_USE_PINFO :
            *cprimod_type = cprimod_ordering_info_type_use_pinfo;
            break;
        default:
            break;
    }
    return SOC_E_NONE;
}

int cprif_drv_cprimod_to_cprif_ordering_info_inc_prop (cprimod_ordering_info_prop_t cprimod_prop,  uint8 *cprif_prop)
{
    switch (cprimod_prop) {
        case cprimod_ordering_info_prop_no_increment:
            *cprif_prop = CPRIF_ORDERING_INFO_PROP_NO_INC;
            break;

        case cprimod_ordering_info_prop_increment_by_constant:
            *cprif_prop = CPRIF_ORDERING_INFO_PROP_INC_CONST;
            break;

        case cprimod_ordering_info_prop_increment_by_payload:
            *cprif_prop = CPRIF_ORDERING_INFO_PROP_INC_BY_PAYLOAD;
            break;

        default:
            *cprif_prop = CPRIF_ORDERING_INFO_PROP_INC_CONST;
            break;
    }
    return SOC_E_NONE;
}


int cprif_drv_cprif_to_cprimod_ordering_info_inc_prop (uint8 cprif_prop, cprimod_ordering_info_prop_t  *cprimod_prop)
{
    switch (cprif_prop) {
        case CPRIF_ORDERING_INFO_PROP_NO_INC:
            *cprimod_prop = cprimod_ordering_info_prop_no_increment;
            break;

        case CPRIF_ORDERING_INFO_PROP_INC_CONST:
            *cprimod_prop = cprimod_ordering_info_prop_increment_by_constant;
            break;

        case  CPRIF_ORDERING_INFO_PROP_INC_BY_PAYLOAD:
            *cprimod_prop = cprimod_ordering_info_prop_increment_by_payload;
            break;

        default:
            *cprimod_prop = CPRIF_ORDERING_INFO_PROP_INC_CONST;
            break;
    }
    return SOC_E_NONE;
}


int cprif_drv_encap_vlan_ethtype_set(int unit, int port,
                                      uint8 ethtype_id,
                                      uint16 ethtype)

{
    uint64 reg_val;
    uint64 write_val;
    uint32 local_ethtype;


    switch (ethtype_id) {
        case CPRIF_ETHTYPE_ROE_ETHTYPE_0:
            COMPILER_64_SET(write_val, 0, (uint32) ethtype);
            SOC_IF_ERROR_RETURN(READ_CPRI_ENCAP_VLAN_CTRLr(unit, port,
                                                           &reg_val));
            soc_reg64_field_set(unit, CPRI_ENCAP_VLAN_CTRLr, &reg_val,
                                ENCAP_ROE_ETHERTYPE_0f, write_val);
            SOC_IF_ERROR_RETURN(WRITE_CPRI_ENCAP_VLAN_CTRLr(unit, port,
                                                            reg_val));
            break;

        case CPRIF_ETHTYPE_ROE_ETHTYPE_1:
            COMPILER_64_SET(write_val, 0, (uint32) ethtype);
            SOC_IF_ERROR_RETURN(READ_CPRI_ENCAP_VLAN_QINQ_CTRLr(unit,
                                                        port, &reg_val));
            soc_reg64_field_set(unit, CPRI_ENCAP_VLAN_QINQ_CTRLr,
                           &reg_val, ENCAP_ROE_ETHERTYPE_1f, write_val);
            SOC_IF_ERROR_RETURN(WRITE_CPRI_ENCAP_VLAN_QINQ_CTRLr(unit,
                                                        port, reg_val));

            break;

        case CPRIF_ETHTYPE_VLAN_TAGGED:
            SOC_IF_ERROR_RETURN(READ_CPRI_ENCAP_VLAN_CTRLr(unit, port,
                                                           &reg_val));
            local_ethtype = ethtype;
            local_ethtype = local_ethtype << 16;
            COMPILER_64_SET(write_val, 0, local_ethtype);
            soc_reg64_field_set(unit, CPRI_ENCAP_VLAN_CTRLr, &reg_val,
                                ENCAP_VLAN_TAGGEDf, write_val);
            SOC_IF_ERROR_RETURN(WRITE_CPRI_ENCAP_VLAN_CTRLr(unit, port,
                                                            reg_val));

            break;

        case CPRIF_ETHTYPE_VLAN_QINQ:
            local_ethtype = ethtype;
            local_ethtype = local_ethtype << 16;
            COMPILER_64_SET(write_val, 0, local_ethtype);
            SOC_IF_ERROR_RETURN(READ_CPRI_ENCAP_VLAN_QINQ_CTRLr(unit,
                                                        port, &reg_val));
            soc_reg64_field_set(unit, CPRI_ENCAP_VLAN_QINQ_CTRLr,
                                &reg_val, ENCAP_VLAN_QINQf, write_val);
            SOC_IF_ERROR_RETURN(WRITE_CPRI_ENCAP_VLAN_QINQ_CTRLr(unit,
                                                        port, reg_val));

            break;

        default:
            break;
    }

    return SOC_E_NONE;

}

int cprif_drv_encap_vlan_ethtype_get(int unit, int port,
                                      uint8 ethtype_id,
                                      uint16* ethtype)

{
    uint64 reg_val;
    uint64 read_val;

    switch (ethtype_id) {
        case CPRIF_ETHTYPE_ROE_ETHTYPE_0:
            SOC_IF_ERROR_RETURN(READ_CPRI_ENCAP_VLAN_CTRLr(unit, port,
                                                           &reg_val));
            read_val = soc_reg64_field_get(unit, CPRI_ENCAP_VLAN_CTRLr,
                                        reg_val, ENCAP_ROE_ETHERTYPE_0f);
            *ethtype = COMPILER_64_LO(read_val) & 0xFFFF;
            break;

        case CPRIF_ETHTYPE_ROE_ETHTYPE_1:
            SOC_IF_ERROR_RETURN(READ_CPRI_ENCAP_VLAN_QINQ_CTRLr(unit,
                                                        port, &reg_val));
            read_val = soc_reg64_field_get(unit,
                                    CPRI_ENCAP_VLAN_QINQ_CTRLr, reg_val,
                                    ENCAP_ROE_ETHERTYPE_1f);
            *ethtype = COMPILER_64_LO(read_val) & 0xFFFF;
            break;

        case CPRIF_ETHTYPE_VLAN_TAGGED:
            SOC_IF_ERROR_RETURN(READ_CPRI_ENCAP_VLAN_CTRLr(unit, port,
                                                           &reg_val));
            read_val = soc_reg64_field_get(unit, CPRI_ENCAP_VLAN_CTRLr,
                                           reg_val, ENCAP_VLAN_TAGGEDf);
            COMPILER_64_SHR(read_val, 16);
            *ethtype = COMPILER_64_LO(read_val) & 0xFFFF;
            break;

        case CPRIF_ETHTYPE_VLAN_QINQ:
            SOC_IF_ERROR_RETURN(READ_CPRI_ENCAP_VLAN_QINQ_CTRLr(unit,
                                                        port, &reg_val));
            read_val = soc_reg64_field_get(unit,
                                     CPRI_ENCAP_VLAN_QINQ_CTRLr, reg_val,
                                     ENCAP_VLAN_QINQf);
            COMPILER_64_SHR(read_val, 16);
            *ethtype = COMPILER_64_LO(read_val) & 0xFFFF;
            break;

        default:
            break;
    }

    return SOC_E_NONE;

}

int cprif_drv_encap_ordering_info_entry_set(int unit, int port,
                                             uint8 index ,
                                             const cprif_encap_ordering_info_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_ORDERING_INFO_TAB_0m,
                               CPRI_ENCAP_ORDERING_INFO_TAB_1m,
                               CPRI_ENCAP_ORDERING_INFO_TAB_2m,
                               CPRI_ENCAP_ORDERING_INFO_TAB_3m };

    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    if (index >=  CPRIF_ORDERING_INFO_TABLE_SIZE) {
        LOG_CLI(("Encap Ordering info only have %d entries, %d is out of range",
                           CPRIF_ORDERING_INFO_TABLE_SIZE,
                           index));
        return SOC_E_PARAM;
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read existing configuration since not all the fields will be set
     * here.
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, wr_data_ptr));

    field_buf = entry->type;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_TYPEf, &field_buf);

    field_buf = entry->p_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_PSIZEf, &field_buf);

    field_buf = entry->q_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_QSIZEf, &field_buf);

    field_buf = entry->max;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_SEQMAXf, &field_buf);

    field_buf = entry->increment;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_SEQINCf, &field_buf);

    field_buf = entry->pcnt_prop;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_PCNT_PROPf, &field_buf);

    field_buf = entry->qcnt_prop;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_QCNT_PROPf, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_encap_ordering_info_entry_get(int unit, int port,
                                      uint8 index,
                                      cprif_encap_ordering_info_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_ORDERING_INFO_TAB_0m,
                               CPRI_ENCAP_ORDERING_INFO_TAB_1m,
                               CPRI_ENCAP_ORDERING_INFO_TAB_2m,
                               CPRI_ENCAP_ORDERING_INFO_TAB_3m };
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    if (index >=  CPRIF_ORDERING_INFO_TABLE_SIZE) {
        LOG_CLI(("Encap Ordering info only have %d entries, %d is out of range",
                           CPRIF_ORDERING_INFO_TABLE_SIZE,
                           index));
        return SOC_E_PARAM;
    }


    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read configuration
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_TYPEf, &field_buf);
    entry->type = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_PSIZEf, &field_buf);
    entry->p_size = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_QSIZEf, &field_buf);
    entry->q_size = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_SEQMAXf, &field_buf);
    entry->max = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_SEQINCf, &field_buf);
    entry->increment = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_PCNT_PROPf, &field_buf);
    entry->pcnt_prop = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_QCNT_PROPf, &field_buf);
    entry->qcnt_prop = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_decap_ordering_info_entry_set(int unit, int port,
                                             uint8 index ,
                                            const cprif_decap_ordering_info_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_ORDERING_INFO_TAB_0m ,
                               CPRI_DECAP_ORDERING_INFO_TAB_1m ,
                               CPRI_DECAP_ORDERING_INFO_TAB_2m ,
                               CPRI_DECAP_ORDERING_INFO_TAB_3m };
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    if (index >=  CPRIF_ORDERING_INFO_TABLE_SIZE) {
        LOG_CLI(("Decap Ordering info only have %d entries, %d is out of range",
                           CPRIF_ORDERING_INFO_TABLE_SIZE,
                           index));
        return SOC_E_PARAM;
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read existing configuration since not all the fields will be set
     * here.
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, wr_data_ptr));

    field_buf = entry->type;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_TYPEf, &field_buf);

    field_buf = entry->p_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_PSIZEf, &field_buf);

    field_buf = entry->q_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_QSIZEf, &field_buf);

    field_buf = entry->max;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_SEQMAXf, &field_buf);

    field_buf = entry->increment;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_SEQINCf, &field_buf);


    field_buf = entry->pcnt_inc_p2;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_SEQNUM_P_INC_P2f, &field_buf);

    field_buf = entry->qcnt_inc_p2;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_SEQNUM_Q_INC_P2f, &field_buf);

    field_buf = entry->pcnt_extended;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_SEQNUM_P_EXTENDf, &field_buf);

    field_buf = entry->pcnt_pkt_count;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_SEQNUM_P_PKTCNTf, &field_buf);

    field_buf = entry->modulo_2;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_SEQNUM_MODULO_P2f, &field_buf);

    field_buf = entry->bias;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_SEQINC_BIASf,  &field_buf);

    field_buf = entry->gsm_tsn_bitmap;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_43_0f,  &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_decap_ordering_info_entry_get(int unit, int port,
                                      uint8 index,
                                      cprif_decap_ordering_info_entry_t *entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_ORDERING_INFO_TAB_0m ,
                               CPRI_DECAP_ORDERING_INFO_TAB_1m ,
                               CPRI_DECAP_ORDERING_INFO_TAB_2m ,
                               CPRI_DECAP_ORDERING_INFO_TAB_3m };
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    if (index >=  CPRIF_ORDERING_INFO_TABLE_SIZE) {
        LOG_CLI(("Encap Ordering info only have %d entries, %d is out of range",
                           CPRIF_ORDERING_INFO_TABLE_SIZE,
                           index));
        return SOC_E_PARAM;
    }


    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read configuration
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_TYPEf, &field_buf);
    entry->type = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_PSIZEf, &field_buf);
    entry->p_size = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_QSIZEf, &field_buf);
    entry->q_size = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_SEQMAXf, &field_buf);
    entry->max = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_SEQINCf, &field_buf);
    entry->increment = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_SEQNUM_P_INC_P2f, &field_buf);
    entry->pcnt_inc_p2 = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_SEQNUM_Q_INC_P2f, &field_buf);
    entry->qcnt_inc_p2 = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_SEQNUM_P_EXTENDf, &field_buf);
    entry->pcnt_extended = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_SEQNUM_P_PKTCNTf, &field_buf);
    entry->pcnt_pkt_count = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_SEQNUM_MODULO_P2f, &field_buf);
    entry->modulo_2 = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_SEQINC_BIASf,  &field_buf);
    entry->bias = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_43_0f,  &field_buf);
    entry->gsm_tsn_bitmap = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_decap_queue_to_ordering_info_index_set(int unit, int port,
                                                      uint32 queue,
                                                      uint32 index)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_QUE_NUM_TO_ORDERING_INDEX_0m ,
                               CPRI_DECAP_QUE_NUM_TO_ORDERING_INDEX_1m ,
                               CPRI_DECAP_QUE_NUM_TO_ORDERING_INDEX_2m ,
                               CPRI_DECAP_QUE_NUM_TO_ORDERING_INDEX_3m };

    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    field_buf = index;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_DECAP_ORDERING_INFO_INDEXf,  &field_buf);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, queue, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_decap_queue_to_ordering_info_index_get(int unit, int port,
                                                     uint32 queue,
                                                     uint32* index)

{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_QUE_NUM_TO_ORDERING_INDEX_0m ,
                               CPRI_DECAP_QUE_NUM_TO_ORDERING_INDEX_1m ,
                               CPRI_DECAP_QUE_NUM_TO_ORDERING_INDEX_2m ,
                               CPRI_DECAP_QUE_NUM_TO_ORDERING_INDEX_3m };

    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read configuration
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, queue, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_DECAP_ORDERING_INFO_INDEXf, &field_buf);
    *index = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_encap_queue_ordering_info_sequence_offset_set(int unit, int port,
                                                            uint32 queue,
                                                            uint32 pq_offset)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_ORDERING_INFO_STS_0m,
                               CPRI_ENCAP_ORDERING_INFO_STS_1m,
                               CPRI_ENCAP_ORDERING_INFO_STS_2m,
                               CPRI_ENCAP_ORDERING_INFO_STS_3m };


    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read existing configuration since not all the fields will be set
     * here.
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, queue, wr_data_ptr));

    field_buf = pq_offset;
    if (queue >= CPRIF_MAX_NUM_OF_DATA_QUEUES) {
        soc_mem_field_set(unit, local_mem, wr_data_ptr, CUR_SEQ_NUM_CTLf,  &field_buf);
    } else {
        soc_mem_field_set(unit, local_mem, wr_data_ptr, CUR_SEQ_NUM_DATAf,  &field_buf);
    }
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, queue, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_encap_queue_ordering_info_sequence_offset_get(int unit, int port,
                                                            uint32 queue,
                                                            uint8 control,
                                                            uint32* pq_offset)

{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_ORDERING_INFO_STS_0m,
                               CPRI_ENCAP_ORDERING_INFO_STS_1m,
                               CPRI_ENCAP_ORDERING_INFO_STS_2m,
                               CPRI_ENCAP_ORDERING_INFO_STS_3m };
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read configuration
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, queue, rd_data_ptr));

    if (queue >= CPRIF_MAX_NUM_OF_DATA_QUEUES) {
        soc_mem_field_get(unit, local_mem, rd_data_ptr, CUR_SEQ_NUM_CTLf,  &field_buf);
    } else {
        soc_mem_field_get(unit, local_mem, rd_data_ptr, CUR_SEQ_NUM_DATAf,  &field_buf);
    }

    *pq_offset = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_decap_queue_ordering_info_sequence_offset_set(int unit, int port,
                                                                uint32 queue,
                                                                uint32 pq_offset)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_ORDERING_SEQOFF_TAB_0m ,
                               CPRI_DECAP_ORDERING_SEQOFF_TAB_1m ,
                               CPRI_DECAP_ORDERING_SEQOFF_TAB_2m ,
                               CPRI_DECAP_ORDERING_SEQOFF_TAB_3m };
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read existing configuration since not all the fields will be set
     * here.
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, queue, wr_data_ptr));

    field_buf = pq_offset;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, ORDERING_INFO_SEQOFFf,  &field_buf);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, queue, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_decap_queue_ordering_info_sequence_offset_get(int unit, int port,
                                                               uint32 queue,
                                                               uint32* pq_offset)

{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_ORDERING_SEQOFF_TAB_0m ,
                               CPRI_DECAP_ORDERING_SEQOFF_TAB_1m ,
                               CPRI_DECAP_ORDERING_SEQOFF_TAB_2m ,
                               CPRI_DECAP_ORDERING_SEQOFF_TAB_3m };

    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read configuration
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, queue, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, ORDERING_INFO_SEQOFFf,  &field_buf);

    *pq_offset = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_encap_mac_da_set(int unit, int port,
                                 uint32 index,
                                 uint64 mac_addr)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_QUE_DA_TAB_0m,
                               CPRI_ENCAP_QUE_DA_TAB_1m,
                               CPRI_ENCAP_QUE_DA_TAB_2m,
                               CPRI_ENCAP_QUE_DA_TAB_3m };

    uint32 *wr_data_ptr;
    uint32 num_words;
    uint32 local_mac_addr[2];
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    COMPILER_64_TO_32_LO(local_mac_addr[0],mac_addr);
    COMPILER_64_TO_32_HI(local_mac_addr[1],mac_addr);
    local_mac_addr[1] &= 0xffff;

    soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_DAf, local_mac_addr);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_encap_mac_da_get(int unit, int port,
                                uint32 index,
                                uint64 *mac_addr)

{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_QUE_DA_TAB_0m,
                               CPRI_ENCAP_QUE_DA_TAB_1m,
                               CPRI_ENCAP_QUE_DA_TAB_2m,
                               CPRI_ENCAP_QUE_DA_TAB_3m };

    uint32 *rd_data_ptr;
    uint32 num_words;
    uint32 local_mac_addr[2];
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read configuration
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));
    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_DAf, local_mac_addr);
    COMPILER_64_SET(*mac_addr,local_mac_addr[1], local_mac_addr[0]);

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;


}
int cprif_drv_encap_mac_sa_set(int unit, int port,
                                 uint32 index,
                                 uint64 mac_addr)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_QUE_SA_TAB_0m,
                               CPRI_ENCAP_QUE_SA_TAB_1m,
                               CPRI_ENCAP_QUE_SA_TAB_2m,
                               CPRI_ENCAP_QUE_SA_TAB_3m };

    uint32 *wr_data_ptr;
    uint32 num_words;
    uint32 local_mac_addr[2];
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    COMPILER_64_TO_32_LO(local_mac_addr[0],mac_addr);
    COMPILER_64_TO_32_HI(local_mac_addr[1],mac_addr);
    local_mac_addr[1] &= 0xffff;

    soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_SAf, local_mac_addr);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_encap_mac_sa_get(int unit, int port,
                                uint32 index,
                                uint64* mac_addr)

{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_QUE_SA_TAB_0m,
                               CPRI_ENCAP_QUE_SA_TAB_1m,
                               CPRI_ENCAP_QUE_SA_TAB_2m,
                               CPRI_ENCAP_QUE_SA_TAB_3m };

    uint32 *rd_data_ptr;
    uint32 num_words;
    uint32 local_mac_addr[2];
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read configuration
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_SAf, local_mac_addr);
    COMPILER_64_SET(*mac_addr,local_mac_addr[1], local_mac_addr[0]);
exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_encap_vlan_entry_set(int unit, int port,
                                    uint32 vlan_table_num,
                                    uint32 index,
                                    uint32 vlan_id)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;

    soc_mem_t mem_select_vlan0[] = {CPRI_ENCAP_QUE_VLAN0_TAB_0m,
                                    CPRI_ENCAP_QUE_VLAN0_TAB_1m,
                                    CPRI_ENCAP_QUE_VLAN0_TAB_2m,
                                    CPRI_ENCAP_QUE_VLAN0_TAB_3m };


    soc_mem_t mem_select_vlan1[] = {CPRI_ENCAP_QUE_VLAN1_TAB_0m,
                                    CPRI_ENCAP_QUE_VLAN1_TAB_1m,
                                    CPRI_ENCAP_QUE_VLAN1_TAB_2m,
                                    CPRI_ENCAP_QUE_VLAN1_TAB_3m };
    uint32 *wr_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (vlan_table_num  == 0) {
        local_mem = mem_select_vlan0[port_core_index];
    } else {
        local_mem = mem_select_vlan1[port_core_index];
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    field_buf = vlan_id;
    if (vlan_table_num == 0) {
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_VLAN0f, &field_buf);
    } else {
        soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_VLAN1f, &field_buf);
    }
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_encap_vlan_entry_get(int unit, int port,
                                    uint32 vlan_table_num,
                                    uint32 index,
                                    uint32* vlan_id)

{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select_vlan0[] = {CPRI_ENCAP_QUE_VLAN0_TAB_0m,
                                    CPRI_ENCAP_QUE_VLAN0_TAB_1m,
                                    CPRI_ENCAP_QUE_VLAN0_TAB_2m,
                                    CPRI_ENCAP_QUE_VLAN0_TAB_3m };


    soc_mem_t mem_select_vlan1[] = {CPRI_ENCAP_QUE_VLAN1_TAB_0m,
                                    CPRI_ENCAP_QUE_VLAN1_TAB_1m,
                                    CPRI_ENCAP_QUE_VLAN1_TAB_2m,
                                    CPRI_ENCAP_QUE_VLAN1_TAB_3m };


    uint32 *rd_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (vlan_table_num == 0) {
        local_mem = mem_select_vlan0[port_core_index];
    } else {
        local_mem = mem_select_vlan1[port_core_index];
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read configuration
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));

    if (vlan_table_num == 0) {
        soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_VLAN0f, &field_buf);
    } else {
        soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_VLAN1f, &field_buf);
    }

    *vlan_id = field_buf;


exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_decap_vlan_ethtype_set(int unit, int port,
                                      uint8 ethtype_id,
                                      uint16 ethtype)

{
    uint64 reg_val;
    uint64 write_val;

    COMPILER_64_SET(write_val, 0, (uint32) ethtype);
    SOC_IF_ERROR_RETURN(READ_CPRI_DECAP_CTRLr(unit, port, &reg_val));

    switch (ethtype_id) {
        case CPRIF_DECAP_ROE_ETHTYPE:
            soc_reg64_field_set(unit, CPRI_DECAP_CTRLr, &reg_val,
                                DECAP_ROE_ETHERTYPEf, write_val);
            break;

        case CPRIF_DECAP_FAST_ETH_ETHTYPE:
            soc_reg64_field_set(unit, CPRI_DECAP_CTRLr, &reg_val,
                                DECAP_FAST_ETH_ETHERTYPEf, write_val);
            break;

        case CPRIF_DECAP_VLAN_TAGGED_ETHTYPE:
            soc_reg64_field_set(unit, CPRI_DECAP_CTRLr, &reg_val,
                                DECAP_VLAN_ETYPEf, write_val);

            break;

        case CPRIF_DECAP_VLAN_QINQ_ETHTYPE:
            soc_reg64_field_set(unit, CPRI_DECAP_CTRLr, &reg_val,
                                DECAP_QINQ_ETYPEf, write_val);
            break;

        default:
            break;
    }

    SOC_IF_ERROR_RETURN(WRITE_CPRI_DECAP_CTRLr(unit, port, reg_val));
    return SOC_E_NONE;

}

int cprif_drv_decap_vlan_ethtype_get(int unit, int port,
                                      uint8 ethtype_id,
                                      uint16* ethtype)

{
    uint64 reg_val;
    uint64 read_val;

    SOC_IF_ERROR_RETURN(READ_CPRI_DECAP_CTRLr(unit, port, &reg_val));
    switch (ethtype_id) {
        case CPRIF_DECAP_ROE_ETHTYPE:
            read_val = soc_reg64_field_get(unit, CPRI_DECAP_CTRLr,
                                           reg_val, DECAP_ROE_ETHERTYPEf);
            *ethtype = COMPILER_64_LO(read_val) & 0xFFFF;
            break;

        case CPRIF_DECAP_FAST_ETH_ETHTYPE:
            read_val = soc_reg64_field_get(unit, CPRI_DECAP_CTRLr,
                                     reg_val, DECAP_FAST_ETH_ETHERTYPEf);
            *ethtype = COMPILER_64_LO(read_val) & 0xFFFF;
            break;

        case CPRIF_DECAP_VLAN_TAGGED_ETHTYPE:
            read_val = soc_reg64_field_get(unit, CPRI_DECAP_CTRLr,
                                           reg_val, DECAP_VLAN_ETYPEf);
            *ethtype = COMPILER_64_LO(read_val) & 0xFFFF;
            break;

        case CPRIF_DECAP_VLAN_QINQ_ETHTYPE:
            read_val = soc_reg64_field_get(unit, CPRI_DECAP_CTRLr,
                                           reg_val, DECAP_QINQ_ETYPEf);
            *ethtype = COMPILER_64_LO(read_val) & 0xFFFF;
            break;

        default:
            break;
    }
    return SOC_E_NONE;
}

int cprif_drv_decap_pkt_type_config_set(int unit, int port,
                                         uint16 roe_ethtype,
                                         uint16 fast_eth_ethtype,
                                         uint16 vlan_ethtype,
                                         uint16 qinq_ethtype)
{

    uint64 reg_val;
    uint64 write_val;

    SOC_IF_ERROR_RETURN(READ_CPRI_DECAP_CTRLr(unit, port, &reg_val));

    COMPILER_64_SET(write_val, 0, (uint32) roe_ethtype);
    soc_reg64_field_set(unit, CPRI_DECAP_CTRLr, &reg_val,
                        DECAP_ROE_ETHERTYPEf, write_val);

    COMPILER_64_SET(write_val, 0, (uint32) fast_eth_ethtype);
    soc_reg64_field_set(unit, CPRI_DECAP_CTRLr, &reg_val,
                        DECAP_FAST_ETH_ETHERTYPEf, write_val);

    COMPILER_64_SET(write_val, 0, (uint32) vlan_ethtype);
    soc_reg64_field_set(unit, CPRI_DECAP_CTRLr, &reg_val,
                        DECAP_VLAN_ETYPEf, write_val);

    COMPILER_64_SET(write_val, 0, (uint32) qinq_ethtype);
    soc_reg64_field_set(unit, CPRI_DECAP_CTRLr, &reg_val,
                        DECAP_QINQ_ETYPEf, write_val);

    SOC_IF_ERROR_RETURN(WRITE_CPRI_DECAP_CTRLr(unit, port, write_val));

    return SOC_E_NONE;

}


int cprif_drv_decap_pkt_type_config_get(int unit, int port,
                                         uint16* roe_ethtype,
                                         uint16* fast_eth_ethtype,
                                         uint16* vlan_ethtype,
                                         uint16* qinq_ethtype)

{
    uint64 reg_val;
    uint64 read_val;

    SOC_IF_ERROR_RETURN(READ_CPRI_DECAP_CTRLr(unit, port, &reg_val));

    read_val = soc_reg64_field_get(unit, CPRI_DECAP_CTRLr, reg_val,
                                   DECAP_ROE_ETHERTYPEf);
    *roe_ethtype = COMPILER_64_LO(read_val) & 0xFFFF;

    read_val = soc_reg64_field_get(unit, CPRI_DECAP_CTRLr, reg_val,
                                   DECAP_FAST_ETH_ETHERTYPEf);
    *fast_eth_ethtype = COMPILER_64_LO(read_val) & 0xFFFF;

    read_val = soc_reg64_field_get(unit, CPRI_DECAP_CTRLr, reg_val,
                                   DECAP_VLAN_ETYPEf );
    *vlan_ethtype = COMPILER_64_LO(read_val) & 0xFFFF;

    read_val = soc_reg64_field_get(unit, CPRI_DECAP_CTRLr, reg_val,
                                   DECAP_QINQ_ETYPEf);
    *qinq_ethtype = COMPILER_64_LO(read_val) & 0xFFFF;
    return SOC_E_NONE;
}

int cprif_drv_decap_flow_classification_entry_set(int unit, int port,
                                                   uint8 subtype,
                                                   uint32 queue_num,
                                                   uint32 option,
                                                   uint32 flow_type )
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_CLS_TO_QUEUE_LOOKUP_0m,
                               CPRI_DECAP_CLS_TO_QUEUE_LOOKUP_1m,
                               CPRI_DECAP_CLS_TO_QUEUE_LOOKUP_2m,
                               CPRI_DECAP_CLS_TO_QUEUE_LOOKUP_3m};

    uint32 *wr_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    field_buf = flow_type;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_DECAP_CFS_FLOW_TYPEf, &field_buf);

    field_buf = option;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_DECAP_CFS_USE_FLOWIDf, &field_buf);

    field_buf = queue_num;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_DECAP_CFS_QUEUE_NUMf, &field_buf);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, subtype, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_decap_flow_classification_entry_get(int unit, int port,
                                                   uint8 subtype,
                                                   uint32* queue_num,
                                                   uint32* option,
                                                   uint32* flow_type )

{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_CLS_TO_QUEUE_LOOKUP_0m,
                               CPRI_DECAP_CLS_TO_QUEUE_LOOKUP_1m ,
                               CPRI_DECAP_CLS_TO_QUEUE_LOOKUP_2m,
                               CPRI_DECAP_CLS_TO_QUEUE_LOOKUP_3m};
    uint32 *rd_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read configuration
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, subtype, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_DECAP_CFS_QUEUE_NUMf, &field_buf);
    *queue_num = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_DECAP_CFS_USE_FLOWIDf, &field_buf);
    *option = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_DECAP_CFS_FLOW_TYPEf, &field_buf);
    *flow_type = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_decap_flow_to_queue_map_set(int unit, int port,
                                           uint32 flow_id,
                                           uint32 queue_num)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_FLOWID_QUE_LOOKUP_0m,
                               CPRI_DECAP_FLOWID_QUE_LOOKUP_1m,
                               CPRI_DECAP_FLOWID_QUE_LOOKUP_2m,
                               CPRI_DECAP_FLOWID_QUE_LOOKUP_3m };

    uint32 *wr_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    field_buf = queue_num;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_NUMf, &field_buf);
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, flow_id, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_decap_flow_to_queue_map_get(int unit, int port,
                                           uint32 flow_id,
                                           uint32* queue_num)

{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_FLOWID_QUE_LOOKUP_0m,
                               CPRI_DECAP_FLOWID_QUE_LOOKUP_1m,
                               CPRI_DECAP_FLOWID_QUE_LOOKUP_2m,
                               CPRI_DECAP_FLOWID_QUE_LOOKUP_3m };

    uint32 *rd_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    /*
     * Read configuration
     */
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, flow_id, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_NUMf, &field_buf);
    *queue_num = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_framer_tgen_tick_set(int unit, int port,
                                       uint32 divider,
                                       uint32 bfrm_ticks,
                                       uint32 bfrm_ticks_bitmap,
                                       uint32 bitmap_size)
{


    uint64 write_val;
    uint64 field_buf;
    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_CFG1r(unit, port, &write_val));

    COMPILER_64_SET(field_buf,0,divider);
    soc_reg64_field_set(unit, CPRI_TXFRM_TGEN_CFG1r, &write_val, TX_TGEN_DIVIDERf, field_buf);
    COMPILER_64_SET(field_buf,0,bfrm_ticks_bitmap);
    soc_reg64_field_set(unit, CPRI_TXFRM_TGEN_CFG1r, &write_val, TX_TGEN_BFRM_TICKS_BITMAPf, field_buf);
    COMPILER_64_SET(field_buf,0,bitmap_size);
    soc_reg64_field_set(unit, CPRI_TXFRM_TGEN_CFG1r, &write_val, TX_TGEN_BFRM_TICKS_BITMAP_SIZEf, field_buf);
    COMPILER_64_SET(field_buf,0,bfrm_ticks);
    soc_reg64_field_set(unit, CPRI_TXFRM_TGEN_CFG1r, &write_val, TX_TGEN_BFRM_TICKSf, field_buf);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_TGEN_CFG1r(unit, port, write_val));

    return SOC_E_NONE;

}

int cprif_drv_tx_framer_tgen_tick_get(int unit, int port,
                                       uint32* divider,
                                       uint32* bfrm_ticks,
                                       uint32* bfrm_ticks_bitmap,
                                       uint32* bitmap_size)

{
    uint64 reg_val;
    uint64 read_val;

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_CFG1r(unit, port, &reg_val));

    read_val = soc_reg64_field_get(unit, CPRI_TXFRM_TGEN_CFG1r, reg_val,
                                   TX_TGEN_DIVIDERf);
    *divider =  COMPILER_64_LO(read_val);

    read_val = soc_reg64_field_get(unit, CPRI_TXFRM_TGEN_CFG1r, reg_val,
                                   TX_TGEN_BFRM_TICKS_BITMAPf);
    *bfrm_ticks =  COMPILER_64_LO(read_val);

    read_val = soc_reg64_field_get(unit, CPRI_TXFRM_TGEN_CFG1r, reg_val,
                                   TX_TGEN_BFRM_TICKS_BITMAP_SIZEf);
    *bitmap_size = COMPILER_64_LO(read_val);

    read_val = soc_reg64_field_get(unit, CPRI_TXFRM_TGEN_CFG1r, reg_val,
                                   TX_TGEN_BFRM_TICKSf);
    *bfrm_ticks_bitmap = COMPILER_64_LO(read_val);

    return SOC_E_NONE;
}

int cprif_drv_tx_framer_tgen_enable(int unit, int port,
                                     uint8 enable)
{
    uint64 reg_val;
    uint64 write_val;

    COMPILER_64_SET(write_val, 0, (uint32) enable? 1:0 );

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_CFG2r(unit, port, &reg_val));
    soc_reg64_field_set(unit, CPRI_TXFRM_TGEN_CFG2r, &reg_val,
                        TX_TGEN_ENABLEf, write_val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_TGEN_CFG2r(unit, port, reg_val));

    if(!enable) {
        /*
         * to disable TGEN, GLAS need to be reset.
         */

        SOC_IF_ERROR_RETURN
            (cprif_drv_glas_reset_set(unit, port, 1));

        SOC_IF_ERROR_RETURN
            (cprif_drv_glas_reset_set(unit, port, 0));
    }

    return SOC_E_NONE;
}

int cprif_drv_tx_framer_tgen_offset_set(int unit, int port,
                                         uint64  offset)
{
    uint64 write_val;
    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_CFG2r(unit, port, &write_val));
    soc_reg64_field_set(unit, CPRI_TXFRM_TGEN_CFG2r, &write_val, TX_TGEN_TS_OFFSETf, offset);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_TGEN_CFG2r(unit, port, write_val));
    return SOC_E_NONE;
}

int cprif_drv_tx_framer_tgen_offset_get(int unit, int port,
                                         uint64*  offset)
{
    uint64 reg_val;
    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_CFG2r(unit, port, &reg_val));
    *offset = soc_reg64_field_get(unit, CPRI_TXFRM_TGEN_CFG2r, reg_val, TX_TGEN_TS_OFFSETf);
    return SOC_E_NONE;
}

int cprif_drv_tx_framer_tgen_next_bfn_config_set(int unit, int port,
                                           uint32  bfn)
{
    uint64 reg_val;
    uint64 write_val;

    COMPILER_64_SET(write_val, 0, bfn);

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_CFG4r(unit, port, &reg_val));
    soc_reg64_field_set(unit, CPRI_TXFRM_TGEN_CFG4r, &reg_val,
                        TX_TGEN_BFNf, write_val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_TGEN_CFG4r(unit, port, reg_val));

    return SOC_E_NONE;
}

int cprif_drv_tx_framer_tgen_next_bfn_config_get(int unit, int port,
                                           uint32* bfn)
{
    uint64 reg_val;
    uint64 read_val;

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_CFG4r(unit, port, &reg_val));
    read_val = soc_reg64_field_get(unit, CPRI_TXFRM_TGEN_CFG4r,
                                   reg_val, TX_TGEN_BFNf);
    *bfn = COMPILER_64_LO(read_val) & 0xFFFF;

    return SOC_E_NONE;
}

int cprif_drv_tx_framer_tgen_next_hfn_config_set(int unit, int port,
                                           uint32 hfn)
{
    uint64 reg_val;
    uint64 write_val;

    COMPILER_64_SET(write_val, 0, hfn);

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_CFG3r(unit, port, &reg_val));
    soc_reg64_field_set(unit, CPRI_TXFRM_TGEN_CFG3r, &reg_val,
                        TX_TGEN_HFNf, write_val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_TGEN_CFG3r(unit, port, reg_val));

    return SOC_E_NONE;
}

int cprif_drv_tx_framer_tgen_next_hfn_config_get(int unit, int port,
                                           uint32* hfn)
{
    uint64 reg_val;
    uint64 read_val;

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_CFG3r(unit, port, &reg_val));
    read_val = soc_reg64_field_get(unit, CPRI_TXFRM_TGEN_CFG3r, reg_val,
                                   TX_TGEN_HFNf);
    *hfn = COMPILER_64_LO(read_val) & 0xFF;

    return SOC_E_NONE;
}

int cprif_drv_tx_framer_next_bfn_get(int unit, int port,
                                           uint32* bfn)
{
    uint64 reg_val;
    uint64 read_val;

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_STATUSr(unit, port, &reg_val));
    read_val = soc_reg64_field_get(unit, CPRI_TXFRM_TGEN_STATUSr,
                                   reg_val, TX_TGEN_BF_CNT_LIVEf);
    *bfn = COMPILER_64_LO(read_val);

    return SOC_E_NONE;
}

int cprif_drv_tx_framer_next_hfn_get(int unit, int port,
                                           uint32* hfn)
{
    uint64 reg_val;
    uint64 read_val;

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_STATUSr(unit, port, &reg_val));
    read_val = soc_reg64_field_get(unit, CPRI_TXFRM_TGEN_STATUSr, reg_val,
                                   TX_TGEN_HF_CNT_LIVEf);
    *hfn = COMPILER_64_LO(read_val);

    return SOC_E_NONE;
}


int cprif_drv_tx_framer_tgen_select_counter_set(int unit, int port,
                                                 uint8  hfn_bfn)
{
    uint64 reg_val;
    uint64 write_val;

    COMPILER_64_SET(write_val, 0, (uint32) hfn_bfn);

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_CFG1r(unit, port, &reg_val));
    soc_reg64_field_set(unit, CPRI_TXFRM_TGEN_CFG1r, &reg_val,
                        TX_TGEN_BFN_SELf, write_val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_TGEN_CFG1r(unit, port, reg_val));

    return SOC_E_NONE;
}

int cprif_drv_tx_framer_tgen_select_counter_get(int unit, int port,
                                                 uint8*  hfn_bfn)
{
    uint64 reg_val;
    uint64 read_val;

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_TGEN_CFG1r(unit, port, &reg_val));
    read_val = soc_reg64_field_get(unit, CPRI_TXFRM_TGEN_CFG1r, reg_val,
                                   TX_TGEN_BFN_SELf);
    *hfn_bfn = COMPILER_64_LO(read_val) & 0xFF;

    return SOC_E_NONE;
}

/*
 *
 *   RSVD4 Related Drivers.
 */

int cprif_drv_rsvd4_header_compare_entry_set(int unit, int port,
                                              int index,
                                              cprimod_header_compare_entry_t* entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_HDR_CMP_TAB_CFG_0m,
                               CPRI_HDR_CMP_TAB_CFG_1m,
                               CPRI_HDR_CMP_TAB_CFG_2m,
                               CPRI_HDR_CMP_TAB_CFG_3m};

    uint32 *wr_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    field_buf = entry->valid;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VALIDf, &field_buf);

    field_buf = entry->match_data;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, MATCH_DATAf, &field_buf);

    field_buf = entry->mask;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, MASKf, &field_buf);

    field_buf = entry->flow_id;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, FLOW_IDf, &field_buf);

    field_buf = entry->flow_type;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, CTRL_FLOWf, &field_buf);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;

}

int cprif_drv_rsvd4_header_compare_entry_get(int unit, int port,
                                              int index,
                                              cprimod_header_compare_entry_t* entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_HDR_CMP_TAB_CFG_0m,
                               CPRI_HDR_CMP_TAB_CFG_1m,
                               CPRI_HDR_CMP_TAB_CFG_2m,
                               CPRI_HDR_CMP_TAB_CFG_3m};

    uint32 *rd_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VALIDf, &field_buf);
    entry->valid =  field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, MATCH_DATAf, &field_buf);
    entry->match_data = field_buf;;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, MASKf, &field_buf);
    entry->mask = field_buf;;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, FLOW_IDf, &field_buf);
    entry->flow_id = field_buf;;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, CTRL_FLOWf, &field_buf);
    entry->flow_type = field_buf;;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_header_compare_entry_valid_bit_swap (int unit, int port,
                                                     int index,
                                                     uint32* valid_bit)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_HDR_CMP_TAB_CFG_0m,
                               CPRI_HDR_CMP_TAB_CFG_1m,
                               CPRI_HDR_CMP_TAB_CFG_2m,
                               CPRI_HDR_CMP_TAB_CFG_3m};

    uint32 *data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(data_ptr);
    sal_memset(data_ptr, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, data_ptr));

    soc_mem_field_get(unit, local_mem, data_ptr, VALIDf, &field_buf);
    if (field_buf != *valid_bit) {
        soc_mem_field_set(unit, local_mem, data_ptr, VALIDf, valid_bit);
        *valid_bit = field_buf;
        _SOC_IF_ERR_EXIT
            (soc_mem_write(unit, local_mem, blk, index, data_ptr));

    }
exit:
    sal_free(data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_modulo_rule_entry_set(int unit, int port,
                                           uint8  index,
                                           uint8  control_rule,
                                           cprimod_modulo_rule_entry_t* entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t data_mem_select[] = { BRCM_RESERVED_CPM_23m,
                                    BRCM_RESERVED_CPM_24m,
                                    BRCM_RESERVED_CPM_25m,
                                    BRCM_RESERVED_CPM_26m
                                  };
    soc_mem_t ctrl_mem_select[] = { BRCM_RESERVED_CPM_27m,
                                    BRCM_RESERVED_CPM_28m,
                                    BRCM_RESERVED_CPM_29m,
                                    BRCM_RESERVED_CPM_30m
                                  };

    uint32 *wr_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (control_rule) {
        local_mem = ctrl_mem_select[port_core_index];
    } else {
        local_mem = data_mem_select[port_core_index];
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    if (control_rule) {
        field_buf = entry->active;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_27_5f, &field_buf);

        field_buf = entry->modulo_value;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_27_4f, &field_buf);

        field_buf = entry->modulo_index;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_27_3f, &field_buf);

        field_buf = entry->dbm_enable;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_27_2f, &field_buf);

        field_buf = entry->flow_dbm_id;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_27_1f, &field_buf);

        field_buf = entry->flow_type;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_27_0f, &field_buf);
    } else {
        field_buf = entry->active;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_23_5f, &field_buf);

        field_buf = entry->modulo_value;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_23_4f, &field_buf);

        field_buf = entry->modulo_index;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_23_3f, &field_buf);

        field_buf = entry->dbm_enable;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_23_2f, &field_buf);

        field_buf = entry->flow_dbm_id;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_23_1f, &field_buf);

        field_buf = entry->flow_type;
        soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_23_0f, &field_buf);
    }

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;

}


int cprif_drv_rsvd4_modulo_rule_entry_get(int unit, int port,
                                           uint8  index,
                                           uint8  control_rule,
                                           cprimod_modulo_rule_entry_t* entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t data_mem_select[] = { BRCM_RESERVED_CPM_23m,
                                    BRCM_RESERVED_CPM_24m,
                                    BRCM_RESERVED_CPM_25m,
                                    BRCM_RESERVED_CPM_26m
                                  };
    soc_mem_t ctrl_mem_select[] = { BRCM_RESERVED_CPM_27m,
                                    BRCM_RESERVED_CPM_28m,
                                    BRCM_RESERVED_CPM_29m,
                                    BRCM_RESERVED_CPM_30m
                                  };

    uint32 *rd_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (control_rule) {
        local_mem = ctrl_mem_select[port_core_index];
    } else {
        local_mem = data_mem_select[port_core_index];
    }

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));

    if (control_rule) {
        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_27_5f, &field_buf);
        entry->active = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_27_4f, &field_buf);
        entry->modulo_value = field_buf;;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_27_3f, &field_buf);
        entry->modulo_index = field_buf;;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_27_2f, &field_buf);
        entry->dbm_enable = field_buf;;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_27_1f, &field_buf);
        entry->flow_dbm_id = field_buf;;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_27_0f, &field_buf);
        entry->flow_type = field_buf;;

    } else {
        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_23_5f, &field_buf);
        entry->active = field_buf;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_23_4f, &field_buf);
        entry->modulo_value = field_buf;;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_23_3f, &field_buf);
        entry->modulo_index = field_buf;;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_23_2f, &field_buf);
        entry->dbm_enable = field_buf;;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_23_1f, &field_buf);
        entry->flow_dbm_id = field_buf;;

        soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_23_0f, &field_buf);
        entry->flow_type = field_buf;;
    }

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;

}


int cprif_drv_rsvd4_dual_bitmap_rule_entry_set(int unit, int port,
                                           uint8  index,
                                           cprimod_dbm_rule_entry_t* entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem0;
    soc_mem_t local_mem1;
    soc_mem_t dbm_rule0_mem_select[] = { BRCM_RESERVED_CPM_31m,
                                         BRCM_RESERVED_CPM_32m,
                                         BRCM_RESERVED_CPM_33m,
                                         BRCM_RESERVED_CPM_34m
                                       };
    soc_mem_t dbm_rule1_mem_select[] = { BRCM_RESERVED_CPM_35m,
                                         BRCM_RESERVED_CPM_36m,
                                         BRCM_RESERVED_CPM_37m,
                                         BRCM_RESERVED_CPM_38m
                                       };

    uint32 *wr_data_ptr0 = NULL;
    uint32 *wr_data_ptr1 = NULL;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);


    if (index >= CPRIF_DBM_RULE_PROFILE_TABLE_SIZE) {
        LOG_CLI(("Index %d is out of range > than %d",
                          index,
                          CPRIF_DBM_RULE_PROFILE_TABLE_SIZE));
        return SOC_E_PARAM;
    }

    local_mem0 = dbm_rule0_mem_select[port_core_index];
    local_mem1 = dbm_rule1_mem_select[port_core_index];

    /*
     * Preparing DBM Rule 0.
     */

    num_words = soc_mem_entry_words(unit, local_mem0);
    wr_data_ptr0 = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    if (wr_data_ptr0 == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_RESOURCE, ("Unable to allocate memory\n"));
    }
    sal_memset(wr_data_ptr0, 0, sizeof(uint32) * num_words);

    field_buf = entry->bm1_mult;
    soc_mem_field_set(unit, local_mem0, wr_data_ptr0, BRCM_RESERVED_CPM_31_2f, &field_buf);

    soc_mem_field_set(unit, local_mem0, wr_data_ptr0, BRCM_RESERVED_CPM_31_1f, entry->bm1);

    field_buf = entry->bm1_size;
    soc_mem_field_set(unit, local_mem0, wr_data_ptr0, BRCM_RESERVED_CPM_31_0f, &field_buf);


    /*
     * Preparing DBM Rule 1.
     */

    num_words = soc_mem_entry_words(unit, local_mem1);
    wr_data_ptr1 = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    if (wr_data_ptr1 == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_RESOURCE, ("Unable to allocate memory\n"));
    }
    sal_memset(wr_data_ptr1, 0, sizeof(uint32) * num_words);

    soc_mem_field_set(unit, local_mem1, wr_data_ptr1, BRCM_RESERVED_CPM_35_4f, entry->bm2);

    field_buf = entry->bm2_size;
    soc_mem_field_set(unit, local_mem1, wr_data_ptr1, BRCM_RESERVED_CPM_35_3f, &field_buf);

    field_buf = entry->num_slots;
    soc_mem_field_set(unit, local_mem1, wr_data_ptr1, BRCM_RESERVED_CPM_35_2f, &field_buf);

    field_buf = entry->pos_grp_index;
    soc_mem_field_set(unit, local_mem1, wr_data_ptr1, BRCM_RESERVED_CPM_35_1f, &field_buf);

    field_buf = entry->pos_grp_size;
    soc_mem_field_set(unit, local_mem1, wr_data_ptr1, BRCM_RESERVED_CPM_35_0f, &field_buf);


    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem0, blk, index, wr_data_ptr0));
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem1, blk, index, wr_data_ptr1));

exit:
    if (wr_data_ptr0) {
        sal_free(wr_data_ptr0);
    }
    if (wr_data_ptr1) {
        sal_free(wr_data_ptr1);
    }

    SOC_FUNC_RETURN;

}

int cprif_drv_rsvd4_dual_bitmap_rule_entry_get(int unit, int port,
                                               uint8  index,
                                               cprimod_dbm_rule_entry_t* entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem0;
    soc_mem_t local_mem1;
    soc_mem_t dbm_rule0_mem_select[] = { BRCM_RESERVED_CPM_31m,
                                         BRCM_RESERVED_CPM_32m,
                                         BRCM_RESERVED_CPM_33m,
                                         BRCM_RESERVED_CPM_34m
                                       };
    soc_mem_t dbm_rule1_mem_select[] = { BRCM_RESERVED_CPM_35m,
                                         BRCM_RESERVED_CPM_36m,
                                         BRCM_RESERVED_CPM_37m,
                                         BRCM_RESERVED_CPM_38m
                                       };

    uint32 *rd_data_ptr0 = 0;
    uint32 *rd_data_ptr1 = 0;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);


    if (index >= CPRIF_DBM_RULE_PROFILE_TABLE_SIZE) {
        LOG_CLI(("Index %d is out of range > than %d",
                          index,
                          CPRIF_DBM_RULE_PROFILE_TABLE_SIZE));
        return SOC_E_PARAM;
    }

    local_mem0 = dbm_rule0_mem_select[port_core_index];
    local_mem1 = dbm_rule1_mem_select[port_core_index];

    /*
     * Preparing DBM Rule 0.
     */

    num_words = soc_mem_entry_words(unit, local_mem0);
    rd_data_ptr0 = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr0);
    sal_memset(rd_data_ptr0, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem0, blk, index, rd_data_ptr0));

    soc_mem_field_get(unit, local_mem0, rd_data_ptr0, BRCM_RESERVED_CPM_31_2f, &field_buf);
    entry->bm1_mult = field_buf;

    soc_mem_field_get(unit, local_mem0, rd_data_ptr0, BRCM_RESERVED_CPM_31_1f, entry->bm1);

    soc_mem_field_get(unit, local_mem0, rd_data_ptr0, BRCM_RESERVED_CPM_31_0f, &field_buf);
    entry->bm1_size  = field_buf;;


    /*
     * Preparing DBM Rule 1.
     */

    num_words = soc_mem_entry_words(unit, local_mem1);
    rd_data_ptr1 = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr1);
    sal_memset(rd_data_ptr1, 0, sizeof(uint32) * num_words);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem1, blk, index, rd_data_ptr1));

    soc_mem_field_get(unit, local_mem1, rd_data_ptr1, BRCM_RESERVED_CPM_35_4f, entry->bm2);

    soc_mem_field_get(unit, local_mem1, rd_data_ptr1, BRCM_RESERVED_CPM_35_3f, &field_buf);
    entry->bm2_size  = field_buf;;

    soc_mem_field_get(unit, local_mem1, rd_data_ptr1, BRCM_RESERVED_CPM_35_2f, &field_buf);
    entry->num_slots = field_buf;;

    soc_mem_field_get(unit, local_mem1, rd_data_ptr1, BRCM_RESERVED_CPM_35_1f, &field_buf);
    entry->pos_grp_index = field_buf;;

    soc_mem_field_get(unit, local_mem1, rd_data_ptr1, BRCM_RESERVED_CPM_35_0f, &field_buf);
    entry->pos_grp_size = field_buf;;



exit:
    sal_free(rd_data_ptr0);
    sal_free(rd_data_ptr1);

    SOC_FUNC_RETURN;

}

int cprif_drv_rsvd4_secondary_dual_bitmap_rule_entry_set(int unit, int port,
                                           uint8  index,
                                           cprimod_secondary_dbm_rule_entry_t* entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { BRCM_RESERVED_CPM_18m,
                               BRCM_RESERVED_CPM_19m,
                               BRCM_RESERVED_CPM_20m,
                               BRCM_RESERVED_CPM_21m
                             };


    uint32 *wr_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);


    if (index >= CPRIF_SECONDARY_DBM_RULE_PROFILE_TABLE_SIZE) {
        LOG_CLI(("Index %d is out of range > than %d",
                          index,
                          CPRIF_SECONDARY_DBM_RULE_PROFILE_TABLE_SIZE));
        return SOC_E_PARAM;
    }

    local_mem = mem_select[port_core_index];


    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    field_buf = entry->bm1_mult;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_18_5f, &field_buf);

    field_buf = entry->bm1;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_18_4f, &field_buf);

    field_buf = entry->bm1_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_18_3f, &field_buf);

    field_buf = entry->bm2;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_18_2f, &field_buf);

    field_buf = entry->bm2_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_18_1f, &field_buf);

    field_buf = entry->num_slots;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_18_0f, &field_buf);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);

    SOC_FUNC_RETURN;

}

int cprif_drv_rsvd4_secondary_dual_bitmap_rule_entry_get(int unit, int port,
                                           uint8  index,
                                           cprimod_secondary_dbm_rule_entry_t* entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { BRCM_RESERVED_CPM_18m,
                               BRCM_RESERVED_CPM_19m,
                               BRCM_RESERVED_CPM_20m,
                               BRCM_RESERVED_CPM_21m
                             };


    uint32 *rd_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    if (index >= CPRIF_SECONDARY_DBM_RULE_PROFILE_TABLE_SIZE) {
        LOG_CLI(("Index %d is out of range > than %d",
                          index,
                          CPRIF_SECONDARY_DBM_RULE_PROFILE_TABLE_SIZE));
        return SOC_E_PARAM;
    }

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_18_5f, &field_buf);
    entry->bm1_mult = field_buf ;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_18_4f, &field_buf);
    entry->bm1  = field_buf ;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_18_3f, &field_buf);
    entry->bm1_size = field_buf ;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_18_2f, &field_buf);
    entry->bm2 = field_buf ;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_18_1f, &field_buf);
    entry->bm2_size = field_buf ;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_18_0f, &field_buf);
    entry->num_slots = field_buf ;

exit:
    sal_free(rd_data_ptr);

    SOC_FUNC_RETURN;

}

int cprif_drv_rsvd4_dbm_position_entry_set(int unit, int port,
                                           uint8  index,
                                           cprimod_dbm_pos_table_entry_t* entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { BRCM_RESERVED_CPM_39m,
                               BRCM_RESERVED_CPM_40m,
                               BRCM_RESERVED_CPM_41m,
                               BRCM_RESERVED_CPM_42m
                             };


    uint32 *wr_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);


    if (index >= CPRIF_DBM_POSITION_TABLE_SIZE) {
        LOG_CLI(("Index %d is out of range > than %d",
                          index,
                          CPRIF_DBM_POSITION_TABLE_SIZE));
        return SOC_E_PARAM;
    }

    local_mem = mem_select[port_core_index];


    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    field_buf = entry->valid;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_39_3f, &field_buf);

    field_buf = entry->flow_type;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_39_1f, &field_buf);

    field_buf = entry->flow_id;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_39_2f, &field_buf);

    field_buf = entry->index;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_39_0f, &field_buf);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);

    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_dbm_position_entry_get(int unit, int port,
                                           uint8  index,
                                           cprimod_dbm_pos_table_entry_t* entry)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { BRCM_RESERVED_CPM_39m,
                               BRCM_RESERVED_CPM_40m,
                               BRCM_RESERVED_CPM_41m,
                               BRCM_RESERVED_CPM_42m
                             };


    uint32 *rd_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);


    if (index >= CPRIF_DBM_POSITION_TABLE_SIZE) {
        LOG_CLI(("Index %d is out of range > than %d",
                          index,
                          CPRIF_DBM_POSITION_TABLE_SIZE));
        return SOC_E_PARAM;
    }

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_39_3f, &field_buf);
    entry->valid = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_39_1f, &field_buf);
    entry->flow_type = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_39_2f, &field_buf);
    entry->flow_id = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_39_0f, &field_buf);
    entry->index = field_buf;


exit:
    sal_free(rd_data_ptr);

    SOC_FUNC_RETURN;
}

/*
 * Below are the tasks useful to clear status during Dynamic Add/Del axcs
 */

int cprif_drv_encap_queue_sts_clear(int unit, int port, int queue_num)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_QUEUE_STS_0m,
                               CPRI_ENCAP_QUEUE_STS_1m,
                               CPRI_ENCAP_QUEUE_STS_2m,
                               CPRI_ENCAP_QUEUE_STS_3m
                             };
    uint32 *mem_data_ptr;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    CPRIF_DRV_MEM_PREP;
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT(soc_mem_write(unit, local_mem, blk, queue_num,
                                   mem_data_ptr));
exit:
    sal_free(mem_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_encap_ordering_info_sts_clear(int unit, int port, int queue_num)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_ORDERING_INFO_STS_0m,
                               CPRI_ENCAP_ORDERING_INFO_STS_1m,
                               CPRI_ENCAP_ORDERING_INFO_STS_2m,
                               CPRI_ENCAP_ORDERING_INFO_STS_3m
                             };
    uint32 *mem_data_ptr;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    CPRIF_DRV_MEM_PREP;
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT(soc_mem_write(unit, local_mem, blk, queue_num,
                                   mem_data_ptr));
exit:
    sal_free(mem_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_encap_eth_hdr_clear(int unit, int port, int queue_num)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_QUE_ETH_HDR_0m,
                               CPRI_ENCAP_QUE_ETH_HDR_1m,
                               CPRI_ENCAP_QUE_ETH_HDR_2m,
                               CPRI_ENCAP_QUE_ETH_HDR_3m
                             };
    uint32 *mem_data_ptr;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    CPRIF_DRV_MEM_PREP;
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT(soc_mem_write(unit, local_mem, blk, queue_num,
                                   mem_data_ptr));
exit:
    sal_free(mem_data_ptr);
    SOC_FUNC_RETURN;

}

int cprif_drv_encap_data_tab_clear(int unit, int port, int queue_num)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_DATA_0m,
                               CPRI_ENCAP_DATA_1m,
                               CPRI_ENCAP_DATA_2m,
                               CPRI_ENCAP_DATA_3m
                             };
    uint32 *mem_data_ptr;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    CPRIF_DRV_MEM_PREP;
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT(soc_mem_write(unit, local_mem, blk, queue_num,
                                   mem_data_ptr));
exit:
    sal_free(mem_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_container_parser_mapp_state_tab_clear(int unit, int port, int axc_id)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_CPRSR_MAP_STATE_TAB_STS_0m,
                               CPRI_CPRSR_MAP_STATE_TAB_STS_1m,
                               CPRI_CPRSR_MAP_STATE_TAB_STS_2m,
                               CPRI_CPRSR_MAP_STATE_TAB_STS_3m
                             };
    uint32 *mem_data_ptr;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    CPRIF_DRV_MEM_PREP;
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT(soc_mem_write(unit, local_mem, blk, axc_id,
                                   mem_data_ptr));
exit:
    sal_free(mem_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_container_parser_map_ts_prev_valid_set (int unit, int port, int flow_id, uint32 valid)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_CPRSR_MAP_STATE_TAB_STS_0m,
                               CPRI_CPRSR_MAP_STATE_TAB_STS_1m,
                               CPRI_CPRSR_MAP_STATE_TAB_STS_2m,
                               CPRI_CPRSR_MAP_STATE_TAB_STS_3m
                             };

    uint32 *wr_data_ptr;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    phy_port            = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index     = ((phy_port-1)%4);
    local_mem           = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, flow_id, wr_data_ptr));

    field_buf = valid? 1:0;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_333_1f, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, flow_id, wr_data_ptr));
exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_iq_pak_buff_state_tab_clear(int unit, int port, int axc_id)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_IQ_PK_BUFF_AXC_STATE_STS_0m,
                               CPRI_IQ_PK_BUFF_AXC_STATE_STS_1m,
                               CPRI_IQ_PK_BUFF_AXC_STATE_STS_2m,
                               CPRI_IQ_PK_BUFF_AXC_STATE_STS_3m
                             };
    uint32 *mem_data_ptr;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    CPRIF_DRV_MEM_PREP;
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT(soc_mem_write(unit, local_mem, blk, axc_id,
                                   mem_data_ptr));
exit:
    sal_free(mem_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_iq_pak_buff_payld_tab_clear(int unit, int port, int axc_id)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_IQ_PK_BUFF_AXC_PLD_STS_0m,
                               CPRI_IQ_PK_BUFF_AXC_PLD_STS_1m,
                               CPRI_IQ_PK_BUFF_AXC_PLD_STS_2m,
                               CPRI_IQ_PK_BUFF_AXC_PLD_STS_3m
                             };
    uint32 *mem_data_ptr;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    CPRIF_DRV_MEM_PREP;
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT(soc_mem_write(unit, local_mem, blk, axc_id,
                                   mem_data_ptr));
exit:
    sal_free(mem_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_iq_unpsk_buff_state_tbl_clear(int unit, int port, int axc_id)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_IQU_STATE_STS_0m,
                               CPRI_IQU_STATE_STS_1m,
                               CPRI_IQU_STATE_STS_2m,
                               CPRI_IQU_STATE_STS_3m
                             };
    uint32 *mem_data_ptr;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    CPRIF_DRV_MEM_PREP;
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT(soc_mem_write(unit, local_mem, blk, axc_id,
                                   mem_data_ptr));
exit:
    sal_free(mem_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_cont_assembly_map_state_tbl_clear(int unit, int port, int axc_id)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_CA_MAP_STATE_TAB_STS_0m,
                               CPRI_CA_MAP_STATE_TAB_STS_1m,
                               CPRI_CA_MAP_STATE_TAB_STS_2m,
                               CPRI_CA_MAP_STATE_TAB_STS_3m
                             };
    uint32 *mem_data_ptr;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    CPRIF_DRV_MEM_PREP;
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT(soc_mem_write(unit, local_mem, blk, axc_id,
                                   mem_data_ptr));
exit:
    sal_free(mem_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_decap_queue_state_tbl_clear(int unit, int port, int queue_num)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_QUEUE_STS_0m,
                               CPRI_DECAP_QUEUE_STS_1m,
                               CPRI_DECAP_QUEUE_STS_2m,
                               CPRI_DECAP_QUEUE_STS_3m
                             };
    uint32 *mem_data_ptr;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    CPRIF_DRV_MEM_PREP;
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT(soc_mem_write(unit, local_mem, blk, queue_num,
                                   mem_data_ptr));
exit:
    sal_free(mem_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_decap_queue_data_tbl_clear(int unit, int port, int queue_num)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_DATA_0m,
                               CPRI_DECAP_DATA_1m,
                               CPRI_DECAP_DATA_2m,
                               CPRI_DECAP_DATA_3m
                             };
    uint32 *mem_data_ptr;
    uint32 num_words;
    SOC_INIT_FUNC_DEFS;

    CPRIF_DRV_MEM_PREP;
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT(soc_mem_write(unit, local_mem, blk, queue_num,
                                   mem_data_ptr));
exit:
    sal_free(mem_data_ptr);
    SOC_FUNC_RETURN;
}

/*
 * Program rxframer ctrl register with length of ctrl-word (based on speed)
 * NB: this information is also in basic_frame_cfg.ctrl_w_length,
 *  but we duplicate it here so that users don't need to deal
 *  with that structure.
 */
int cprif_drv_set_rxframer_cw_num_bytes(int unit, int port, cprimod_port_speed_t speed)
{
    uint64 data;
    uint64 write_val;
    uint32 cw_len;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RXFRM_CPRSR_CTRLr(unit, port, &data));

    cw_len = 0;
    switch (speed) {
        case cprimodSpd614p4:
            cw_len = 1;    /*  fl=16 */
            break;
        case cprimodSpd1228p8:
            cw_len = 2;    /*  fl=32 */
            break;
        case cprimodSpd2457p6:
            cw_len = 4;    /*  fl=64 */
            break;
        case cprimodSpd3072p0:
            cw_len = 5;    /*  fl=80 */
            break;
        case cprimodSpd4915p2:
            cw_len = 8;    /* fl=128 */
            break;
        case cprimodSpd6144p0:
            cw_len = 10;   /* fl=160 */
            break;
        case cprimodSpd8110p08:
            cw_len = 16;   /* fl=256 */
            break;
        case cprimodSpd9830p4:
            cw_len = 16;   /* fl=256 */
            break;
        case cprimodSpd10137p6:
            cw_len = 16;   /* fl=320 */
            break;
        case cprimodSpd12165p12:
            cw_len = 16;   /* fl=768 */
            break;
        case cprimodSpd24330p24:
            cw_len = 16;   /* fl=768 */
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Speed is not supported for cpri"));
            break;
    }

    COMPILER_64_SET(write_val, 0, cw_len);
    soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &data, RXFRM_CW_NUM_BYTESf, write_val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RXFRM_CPRSR_CTRLr(unit, port, data));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rxpcs_interface_width_get(int unit, int port,
                                        int *is_10bit)
{
    uint64 data;
    uint64 read_val;


    SOC_IF_ERROR_RETURN(READ_CPRI_RXPCS_CTRLr(unit, port, &data));

    read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                   RXPCS_10B_MODEf);

    *is_10bit = COMPILER_64_IS_ZERO(read_val)? 0:1;

    return SOC_E_NONE;

}


/*
 *  THIS is only for B0.  And only for Tunnel
 */
int cprif_drv_rxpcs_tunnel_mode_set(int unit, int port, int enable,
                                    uint32 timestamp_interval,
                                    int restore_rsvd4,
                                    cprimod_port_speed_t restore_speed)
{
    uint64 data;
    uint64 write_val;
    cprif_pm_version_t pm_version;
    uint64 cpm307_data;

    SOC_IF_ERROR_RETURN
        (cprif_pm_version_get(unit, port, &pm_version));

   if (CPRIF_CLPORT_IS_A0(pm_version)) {
        LOG_CLI(("TUNNEL Mode is not supported in A0.\n"));
        return SOC_E_UNAVAIL;
    }

    SOC_IF_ERROR_RETURN(READ_CPRI_RXPCS_CTRLr(unit, port, &data));
    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port, &cpm307_data));

    if (enable) {
        COMPILER_64_SET(write_val, 0, TRUE);
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data, RXPCS_TUNNEL_MODEf, write_val);

        /* force it to 8B/10B mode. */
        COMPILER_64_SET(write_val, 0, 0);
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data, RXPCS_MODEf, write_val);

        /* set timestamp interval for tunnel mode, in bits. */
        COMPILER_64_SET(write_val, 0, timestamp_interval & 0xFF);
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data, RXPCS_64B66B_VALID_SH_CNTf, write_val);

        COMPILER_64_SET(write_val, 0, (timestamp_interval & 0xFF00)>> 8 );
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data, RXPCS_64B66B_INVALID_SH_CNTf, write_val);

        /*
         * Disable  RSVD4 related items.
         */
        COMPILER_64_SET(write_val, 0, 0);
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_307r, &cpm307_data, RXPCS_BIT_ENDIAN_FLIPf, write_val);
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_307r, &cpm307_data, BRCM_RESERVED_CPM_307_0f, write_val);


    } else { /* disable */
        COMPILER_64_SET(write_val, 0, FALSE);
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data, RXPCS_TUNNEL_MODEf, write_val);

        /* restore the line coding mode. */
        if ( restore_speed >= cprimodSpd10137p6) {
            COMPILER_64_SET(write_val, 0, cprimodCpriPcsMode64b66b);
        } else {
            COMPILER_64_SET(write_val, 0, cprimodCpriPcsMode8b10b);
        }
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data, RXPCS_MODEf, write_val);

        /* Set those to default value. */
        COMPILER_64_SET(write_val, 0, 0x40);
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data, RXPCS_64B66B_VALID_SH_CNTf, write_val);

        COMPILER_64_SET(write_val, 0, 0x10 );
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data, RXPCS_64B66B_INVALID_SH_CNTf, write_val);

       /*
        *  Restore RSVD4 related items if require.
        */
        if (restore_rsvd4) {
            COMPILER_64_SET(write_val, 0, 1);
            soc_reg64_field_set(unit, BRCM_RESERVED_CPM_307r, &cpm307_data, RXPCS_BIT_ENDIAN_FLIPf, write_val);
            soc_reg64_field_set(unit, BRCM_RESERVED_CPM_307r, &cpm307_data, BRCM_RESERVED_CPM_307_0f, write_val);
        }
    }

    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_RXPCS_CTRLr(unit, port, data));

    SOC_IF_ERROR_RETURN
        (WRITE_BRCM_RESERVED_CPM_307r(unit, port, cpm307_data));
    return SOC_E_NONE;
}

/*
 *  THIS is only for both A0 and B0.
 */
int cprif_drv_rxpcs_tunnel_mode_get(int unit, int port, int* enable, uint32* timestamp_interval )
{
    uint64 data;
    uint64 read_val;
    cprif_pm_version_t pm_version;

    SOC_IF_ERROR_RETURN 
        (cprif_pm_version_get(unit, port, &pm_version));

    /* 
     * If A0, just return as not enable, as it does not support
     * tunnel mode. And not an error condition unlike set.
     */
    if (CPRIF_CLPORT_IS_A0(pm_version)) {
        *enable = FALSE;
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(READ_CPRI_RXPCS_CTRLr(unit, port, &data));

    read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                   RXPCS_TUNNEL_MODEf);
    if (COMPILER_64_IS_ZERO(read_val)) {
        *enable = 0;
    } else {
        *enable = 1;
    }

    if (*enable) {

        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                       RXPCS_64B66B_VALID_SH_CNTf);

        *timestamp_interval = COMPILER_64_LO(read_val) & 0xFF;

        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                       RXPCS_64B66B_INVALID_SH_CNTf);

        *timestamp_interval |= ((COMPILER_64_LO(read_val) & 0xFF) << 8);
    }
    return SOC_E_NONE;
}

/*
 *  THIS is only for B0.  And only for Tunnel and RSVD4
 */
int cprif_drv_txpcs_agnostic_mode_set(int unit, int port, int enable,
                                      cprimod_agnostic_mode_type_t mode,
                                      uint32 timestamp_interval,
                                      int restore_rsvd4,
                                      cprimod_port_speed_t restore_speed)
{
    uint64 data;
    uint64 cpm12_data;
    uint64 write_val;
    cprif_pm_version_t pm_version;

    SOC_IF_ERROR_RETURN
        (cprif_pm_version_get(unit, port, &pm_version));

    if (CPRIF_CLPORT_IS_A0(pm_version)) {
        LOG_CLI(("This feature  is not supported in A0.\n"));
        return SOC_E_UNAVAIL;
    }

    SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));

    SOC_IF_ERROR_RETURN
        (READ_BRCM_RESERVED_CPM_12r(unit, port, &cpm12_data));

    if (enable) {
        switch (mode) {
            case cprimod_agnostic_mode_rsvd4:

                COMPILER_64_SET(write_val, 0, FALSE);
                soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data, TX_PCS_TUNNEL_MODEf, write_val);


                COMPILER_64_SET(write_val, 0, TRUE);
                soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data, BRCM_RESERVED_CPM_11_8f, write_val); /* AGNOSTIC_MODE */

                break;

            case cprimod_agnostic_mode_tunnel:

                COMPILER_64_SET(write_val, 0, TRUE);
                soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data, TX_PCS_TUNNEL_MODEf, write_val);

                COMPILER_64_SET(write_val, 0, FALSE);
                soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data, BRCM_RESERVED_CPM_11_8f, write_val);

                /* set timestamp interval for tunnel mode, in bits. */
                COMPILER_64_SET(write_val, 0, timestamp_interval);
                soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data, TX_PCS_TUNNEL_TS_BIT_CNTf, write_val);

                /* force it to 8B/10B mode. */
                COMPILER_64_SET(write_val, 0, 0);
                soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data, TX_PCS_MODEf, write_val);

                /*
                 * Disable RSVD4 mode.
                 */
                COMPILER_64_SET(write_val, 0, 0);
                soc_reg64_field_set(unit, BRCM_RESERVED_CPM_12r, &cpm12_data,
                                    BRCM_RESERVED_CPM_12_8f, write_val);
                break;

            default:
                break;
        } /* switch */
    } else { /* disable */
        COMPILER_64_SET(write_val, 0, FALSE);
        soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data, TX_PCS_TUNNEL_MODEf, write_val);
        soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data, BRCM_RESERVED_CPM_11_8f, write_val);

        /* restore the line coding mode. */
        if ( restore_speed >= cprimodSpd10137p6) {
            COMPILER_64_SET(write_val, 0, cprimodCpriPcsMode64b66b);
        } else {
            COMPILER_64_SET(write_val, 0, cprimodCpriPcsMode8b10b);
        }
        soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data, TX_PCS_MODEf, write_val);

        /* restore timestamp interval for tunnel mode to 0. */
        COMPILER_64_SET(write_val, 0, 0);
        soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data, TX_PCS_TUNNEL_TS_BIT_CNTf, write_val);

        if (restore_rsvd4) {
            /*
             * Restore RSVD4 mode.
             */
            COMPILER_64_SET(write_val, 0, 1);
            soc_reg64_field_set(unit, BRCM_RESERVED_CPM_12r, &cpm12_data,
                                BRCM_RESERVED_CPM_12_8f, write_val);
        }
    }

    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_TXPCS_CTRLr(unit, port, data));

    SOC_IF_ERROR_RETURN
        (WRITE_BRCM_RESERVED_CPM_12r(unit, port, cpm12_data));
    return SOC_E_NONE;
}

/*
 *  THIS is only for B0. And Tunnel and RSVD4
 */
int cprif_drv_txpcs_agnostic_mode_get(int unit, int port, int* enable, cprimod_agnostic_mode_type_t* mode,  uint32* timestamp_interval)
{
    uint64 data;
    uint32 is_tunnel_mode=0;
    uint32 is_rsvd4_mode=0;
    cprif_pm_version_t pm_version;
   
    SOC_IF_ERROR_RETURN
        (cprif_pm_version_get(unit, port, &pm_version));
   
    SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));

    is_rsvd4_mode = soc_reg64_field32_get(unit, CPRI_TXPCS_CTRLr, data,
                                           BRCM_RESERVED_CPM_11_8f);

    if (CPRIF_CLPORT_IS_B0(pm_version)) {
        is_tunnel_mode = soc_reg64_field32_get(unit, CPRI_TXPCS_CTRLr, data,
                                               TX_PCS_TUNNEL_MODEf);
    }
    /* check for tunnel mode enabled. */
    if (is_tunnel_mode) {
        *enable = TRUE;
        *mode   = cprimod_agnostic_mode_tunnel;

        *timestamp_interval = soc_reg64_field32_get(unit, CPRI_TXPCS_CTRLr, data,
                                                    TX_PCS_TUNNEL_TS_BIT_CNTf);
    } else if (is_rsvd4_mode){
        *enable = TRUE;
        *mode   = cprimod_agnostic_mode_rsvd4;
    } else {
        *enable = FALSE;
    }

    return SOC_E_NONE;
}

int cprif_drv_cpri_rxpcs_speed_set(int unit, int port, const phymod_phy_access_t* phy,
                                   cprimod_port_speed_t speed)
{
    int    osr;
    uint64 data;
    uint64 write_val;
    uint32 is10b;
    uint32 pcs_wd_len;
    cprimod_cpri_pcs_mode_t pcs_mode;
    SOC_INIT_FUNC_DEFS;

    /* making RSVD4 0 for reporting port type */
    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port, &data));
    soc_reg64_field32_set(unit, BRCM_RESERVED_CPM_307r, &data, BRCM_RESERVED_CPM_307_0f, 0);
    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_307r(unit, port, data));
    SOC_IF_ERROR_RETURN(READ_CPRI_RXPCS_CTRLr(unit, port, &data));
    pcs_wd_len = 0;
    switch (speed) {
        case cprimodSpd614p4:
            pcs_wd_len = 8;
            pcs_mode = cprimodCpriPcsMode8b10b;
            is10b = 1;
            osr = 13;
            break;
        case cprimodSpd1228p8:
            pcs_wd_len =  16;
            pcs_mode = cprimodCpriPcsMode8b10b;
            is10b = 1;
            osr = 9;
            break;
        case cprimodSpd2457p6:
            pcs_wd_len =  32;
            pcs_mode = cprimodCpriPcsMode8b10b;
            is10b = 1;
            osr = 5;
            break;
        case cprimodSpd3072p0:
            pcs_wd_len =  40;
            pcs_mode = cprimodCpriPcsMode8b10b;
            is10b = 1;
            osr = 5;
            break;
        case cprimodSpd4915p2:
            pcs_wd_len =  64;
            pcs_mode = cprimodCpriPcsMode8b10b;
            is10b = 0;
            osr = 2;
            break;
        case cprimodSpd6144p0:
            pcs_wd_len =  80;
            pcs_mode = cprimodCpriPcsMode8b10b;
            is10b = 0;
            osr = 2;
            break;

        /* Removed this as that speed is not supported.
           Leave the code for reference.
        case cprimodSpd8110p08:
            pcs_wd_len =  80;
            pcs_mode = cprimodCpriPcsMode64b66b;
            is10b = 0;
            osr = 1;
            break;
        */
        case cprimodSpd9830p4:
            pcs_wd_len = 128;
            pcs_mode = cprimodCpriPcsMode8b10b;
            is10b = 0;
            osr = 1;
            break;
        case cprimodSpd10137p6:
            pcs_wd_len = 160;
            pcs_mode = cprimodCpriPcsMode64b66b;
            is10b = 0;
            osr = 1;
            break;
        case cprimodSpd12165p12:
            pcs_wd_len = 192;
            pcs_mode = cprimodCpriPcsMode64b66b;
            is10b = 0;
            osr = 1;
            break;
        case cprimodSpd24330p24:
            pcs_wd_len = 384;
            pcs_mode = cprimodCpriPcsMode64b66b;
            is10b = 0;
            osr = 0;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Speed is not supported for cpri"));
            break;
    }

    COMPILER_64_SET(write_val, 0, (uint32) pcs_mode);
    soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data, RXPCS_MODEf,
                        write_val);

    COMPILER_64_SET(write_val, 0, pcs_wd_len);
    soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data, RXPCS_WORD_LENf,
                        write_val);

    COMPILER_64_SET(write_val, 0, is10b);
    soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data, RXPCS_10B_MODEf,
                        write_val);

    SOC_IF_ERROR_RETURN(WRITE_CPRI_RXPCS_CTRLr(unit, port, data));
    SOC_IF_ERROR_RETURN(READ_CPRI_PMD_RX_CTRLr(unit, port, &data));
    COMPILER_64_SET(write_val, 0, osr);
    soc_reg64_field_set(unit, CPRI_PMD_RX_CTRLr, &data, PMD_RX_OSR_MODEf, write_val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_PMD_RX_CTRLr(unit, port, data));
    falcon2_monterey_frc_rx_osr(&phy->access, osr);

    SOC_IF_ERROR_RETURN(cprif_drv_set_rxframer_cw_num_bytes(unit, port, speed));

    /*
     * Clear the RSVD4 speed.
     */
    SOC_IF_ERROR_RETURN
        (READ_BRCM_RESERVED_CPM_307r(unit, port, &data));

    COMPILER_64_SET(write_val, 0, 0);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_307r, &data,
                        BRCM_RESERVED_CPM_307_2f, write_val);

    SOC_IF_ERROR_RETURN
        (WRITE_BRCM_RESERVED_CPM_307r(unit, port, data));


exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_rxpcs_speed_get(int unit, int port,
                                   cprimod_port_speed_t *speed)
{
    uint64 data;
    uint64 read_val;
    uint32 pcs_wd_len;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RXPCS_CTRLr(unit, port, &data));

    read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                   RXPCS_WORD_LENf);
    pcs_wd_len = COMPILER_64_LO(read_val);

    switch (pcs_wd_len) {

        case 8:
            *speed = cprimodSpd614p4;
            break;

        case 16:
            *speed = cprimodSpd1228p8;
            break;

        case 32:
            *speed = cprimodSpd2457p6;
            break;

        case 40:
            *speed = cprimodSpd3072p0;
            break;

        case 64:
            *speed = cprimodSpd4915p2;
            break;

        case 80:
            *speed = cprimodSpd6144p0;
            break;

        case 128:
            *speed = cprimodSpd9830p4;
            break;

        case 160:
            *speed = cprimodSpd10137p6;
            break;

        case 192:
            *speed = cprimodSpd12165p12;
            break;

        case 384:
            *speed = cprimodSpd24330p24;
            break;

        default:
            *speed = cprimodSpdCount;
            break;

    }
    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_txpcs_speed_set(int unit, int port, const phymod_phy_access_t* phy,
                                   cprimod_port_speed_t speed)
{
    int    osr;
    uint64 data;
    uint64 write_val;
    uint32 data32;
    cprimod_cpri_pcs_mode_t pcs_mode;
    int pcs_word_len, ctrl_byte_len;
    struct falcon2_monterey_uc_lane_config_st serdes_firmware_config;
    SOC_INIT_FUNC_DEFS;

    pcs_word_len =   8;
    pcs_mode = cprimodCpriPcsMode8b10b;
    ctrl_byte_len=1;
    sal_memset(&serdes_firmware_config, 0x0, sizeof(serdes_firmware_config));
    /* coverity[check_return:FALSE] */
    falcon2_monterey_get_uc_lane_cfg(&phy->access, &serdes_firmware_config);
    serdes_firmware_config.field.dfe_on = 1;

    switch (speed) {
        case cprimodSpd614p4:
            pcs_word_len =   8;
            pcs_mode = cprimodCpriPcsMode8b10b;
            ctrl_byte_len=1;
            osr = 13;
            serdes_firmware_config.field.dfe_on = 0;
            break;
        case cprimodSpd1228p8:
            pcs_word_len =  16;
            pcs_mode = cprimodCpriPcsMode8b10b;
            ctrl_byte_len=2;
            osr = 9;
            serdes_firmware_config.field.dfe_on = 0;
            break;
        case cprimodSpd2457p6:
            pcs_word_len =  32;
            pcs_mode = cprimodCpriPcsMode8b10b;
            ctrl_byte_len=4;
            osr = 5;
            serdes_firmware_config.field.dfe_on = 0;
            break;
        case cprimodSpd3072p0:
            pcs_word_len =  40;
            pcs_mode = cprimodCpriPcsMode8b10b;
            ctrl_byte_len=5;
            osr = 5;
            serdes_firmware_config.field.dfe_on = 0;
            break;
        case cprimodSpd4915p2:
            pcs_word_len =  64;
            pcs_mode = cprimodCpriPcsMode8b10b;
            ctrl_byte_len=8;
            osr = 2;
            serdes_firmware_config.field.dfe_on = 0;
            break;
        case cprimodSpd6144p0:
            pcs_word_len =  80;
            pcs_mode = cprimodCpriPcsMode8b10b;
            ctrl_byte_len=10;
            osr = 2;
            serdes_firmware_config.field.dfe_on = 0;
            break;
        /* remove this speed as it is not supported.
           Leaving the code for future reference.
        case cprimodSpd8110p08:
            pcs_word_len =  80;
            pcs_mode = cprimodCpriPcsMode64b66b;
            ctrl_byte_len=16;
            osr = 1;
            serdes_firmware_config.field.dfe_on = 0;
            break;
        */
        case cprimodSpd9830p4:
            pcs_word_len = 128;
            pcs_mode = cprimodCpriPcsMode8b10b;
            ctrl_byte_len=16;
            osr = 1;
            serdes_firmware_config.field.dfe_on = 0;
            break;
        case cprimodSpd10137p6:
            pcs_word_len = 160;
            pcs_mode = cprimodCpriPcsMode64b66b;
            ctrl_byte_len=16;
            osr = 1;
            break;
        case cprimodSpd12165p12:
            pcs_word_len = 192;
            pcs_mode = cprimodCpriPcsMode64b66b;
            ctrl_byte_len=16;
            osr = 1;
            break;
        case cprimodSpd24330p24:
            pcs_word_len = 384;
            pcs_mode = cprimodCpriPcsMode64b66b;
            ctrl_byte_len=16;
            osr = 0;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Speed is not supported for cpri"));
            break;
    }

    SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));

    COMPILER_64_SET(write_val, 0, (uint32) pcs_mode);
    soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data, TX_PCS_MODEf,
                        write_val);

    COMPILER_64_SET(write_val, 0, (uint32) pcs_word_len);
    soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data,
                        TX_PCS_WORD_LEN_8B10Bf, write_val);

    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXPCS_CTRLr(unit, port, data));

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_CWA_GENERALr(unit, port,
                                                     &data32));
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_GENERALr, &data32, TX_CWA_TCWf,
                      ctrl_byte_len);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_CWA_GENERALr(unit, port,
                                                      data32));

    SOC_IF_ERROR_RETURN(READ_CPRI_PMD_TX_CTRLr(unit, port, &data));
    COMPILER_64_SET(write_val, 0, osr);
    soc_reg64_field_set(unit, CPRI_PMD_TX_CTRLr, &data, PMD_TX_OSR_MODEf, write_val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_PMD_TX_CTRLr(unit, port, data));

    falcon2_monterey_frc_tx_osr(&phy->access, osr);
    /* coverity[check_return:FALSE] */
    falcon2_monterey_set_uc_lane_cfg(&phy->access, serdes_firmware_config);

    /*
     * Clear the RSVD4 speed setting.
     */
    SOC_IF_ERROR_RETURN
        (READ_BRCM_RESERVED_CPM_3r(unit, port, &data));

    COMPILER_64_SET(write_val, 0, 0);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_3r, &data,
                        BRCM_RESERVED_CPM_3_2f, write_val);

    SOC_IF_ERROR_RETURN
        (WRITE_BRCM_RESERVED_CPM_3r(unit, port, data));


exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_txpcs_speed_get(int unit, int port,
                                   cprimod_port_speed_t *speed)
{
    uint64 data;
    uint64 read_val;
    uint32 pcs_word_len;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));

    read_val = soc_reg64_field_get(unit, CPRI_TXPCS_CTRLr, data,
                                   TX_PCS_WORD_LEN_8B10Bf);
    pcs_word_len = COMPILER_64_LO(read_val);

    switch (pcs_word_len) {

        case 8:
            *speed = cprimodSpd614p4;
            break;

        case 16:
            *speed = cprimodSpd1228p8;
            break;
        case 32:
            *speed = cprimodSpd2457p6;
            break;

        case 40:
            *speed = cprimodSpd3072p0;
            break;

        case 64:
            *speed = cprimodSpd4915p2;
            break;

        case 80:
            *speed = cprimodSpd6144p0;
            break;

        case 128:
            *speed = cprimodSpd9830p4;
            break;

        case 160:
            *speed = cprimodSpd10137p6;
            break;

        case 192:
            *speed = cprimodSpd12165p12;
            break;

        case 384:
            *speed = cprimodSpd24330p24;
            break;

        default:
            *speed = cprimodSpdCount;
            break;
    }
 SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_rxpcs_speed_set(int unit, int port, const phymod_phy_access_t* phy,
                                    cprimod_port_rsvd4_speed_mult_t speed)
{
    uint64 data;
    uint64 write_val;
    uint32 data32;
    uint32 speedi, speedv;
    uint32 speedi_h, speedi_l;
    uint64 speedi_64;
    int osr;
    uint32 is10b;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RXPCS_CTRLr(unit, port, &data));
    if (speed == cprimodRsvd4SpdMult4X) {
        speedi = 4;
        speedv = 2;
        is10b = 1;
        osr = 5;
    } else if (speed == cprimodRsvd4SpdMult8X) {
        speedi = 8;
        speedv = 3;
        is10b = 0;
        osr = 2;
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Speed is not supported for rsvd4"));
    }

    SOC_IF_ERROR_RETURN(READ_CPRI_PMD_RX_CTRLr(unit, port, &data));
    COMPILER_64_SET(write_val, 0, osr);
    soc_reg64_field_set(unit, CPRI_PMD_RX_CTRLr, &data, PMD_RX_OSR_MODEf, write_val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_PMD_RX_CTRLr(unit, port, data));
    falcon2_monterey_frc_rx_osr(&phy->access, osr);
    SOC_IF_ERROR_RETURN(READ_CPRI_RXPCS_CTRLr(unit, port, &data));
    COMPILER_64_SET(write_val, 0, is10b);
    soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data, RXPCS_10B_MODEf,
                        write_val);
    COMPILER_64_SET(write_val, 0, (uint32) cprimodCpriPcsMode8b10b);
    soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                        RXPCS_MODEf, write_val);

    COMPILER_64_SET(write_val, (uint32) 0, (uint32) 0);
    soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                        RXPCS_WORD_LENf, write_val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RXPCS_CTRLr(unit, port, data));

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port,
                                                           &data));
    COMPILER_64_SET(write_val, (uint32) 0, (uint32) 1);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_307r, &data,
                        RXPCS_BIT_ENDIAN_FLIPf, write_val);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_307r, &data,
                        BRCM_RESERVED_CPM_307_6f, write_val);

    COMPILER_64_SET(write_val, 0, (uint32) speedv);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_307r, &data,
                        BRCM_RESERVED_CPM_307_2f, write_val);

    COMPILER_64_SET(write_val, 0, 1);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_307r, &data,
                        BRCM_RESERVED_CPM_307_0f, write_val);
    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_307r(unit, port,
                                                            data));

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_308r(unit, port,
                                                           &data));
    COMPILER_64_SET(write_val, (uint32) 0, (uint32) CPRIF_DRV_NUMBER_OF_MSG);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_308r, &data,
                        BRCM_RESERVED_CPM_308_3f, write_val);

    COMPILER_64_SET(write_val, (uint32) 0, (uint32) CPRIF_DRV_N_MG_IN_MSTR_FRM);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_308r, &data,
                        BRCM_RESERVED_CPM_308_2f, write_val);
    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_308r(unit, port,
                                                            data));

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_309r(unit, port,
                                                           &data));
    speedi_h = 0;
    speedi_l = (speedi * CPRIF_DRV_NUMBER_OF_MSG *
               (CPRIF_DRV_N_MG_IN_MSTR_FRM*CPRIF_DRV_RSVD4_MSG_SIZE+1)) & 0xffffffff;
    COMPILER_64_SET(speedi_64, speedi_h, speedi_l);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_309r, &data,
                        BRCM_RESERVED_CPM_309_0f, speedi_64);

    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_309r(unit, port,
                                                            data));


    SOC_IF_ERROR_RETURN(READ_CPRI_ENCAP_TDM_CTRLr(unit, port, &data32));
    soc_reg_field_set(unit, CPRI_ENCAP_TDM_CTRLr, &data32,
                        TDM_DISABLEf, 1);

    SOC_IF_ERROR_RETURN(WRITE_CPRI_ENCAP_TDM_CTRLr(unit, port, data32));

    _SOC_IF_ERR_EXIT(READ_CPRI_RXFRM_BFRM_CTRLr(unit, port, &data32));
    soc_reg_field_set(unit, CPRI_RXFRM_BFRM_CTRLr, &data32,
                        BRCM_RESERVED_CPM_306_0f, 1);

    SOC_IF_ERROR_RETURN(WRITE_CPRI_RXFRM_BFRM_CTRLr(unit, port, data32));

    /*
     * Clear the CPRI speed (PCS_WORD_LEN).
     */
    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXPCS_CTRLr(unit, port, &data));

    COMPILER_64_SET(write_val, 0, 0);
    soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data, RXPCS_WORD_LENf, write_val);

    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_RXPCS_CTRLr(unit, port, data));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_rxpcs_speed_get(int unit, int port,
                                    cprimod_port_rsvd4_speed_mult_t *speed)
{
    uint64 data;
    uint64 read_val;
    uint32 speedi;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port,
                                                           &data));
    read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_307r,
                                  data, BRCM_RESERVED_CPM_307_2f);
    speedi = COMPILER_64_LO(read_val);

    if (speedi ==  2) {
        *speed = cprimodRsvd4SpdMult4X;
    } else if (speedi ==  3) {
        *speed = cprimodRsvd4SpdMult8X;
    } else {
        *speed = cprimodRsvd4SpdMultCount;
    }

    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_txpcs_speed_set(int unit, int port, const phymod_phy_access_t* phy,
                                    cprimod_port_rsvd4_speed_mult_t speed)
{
    uint64 data;
    uint64 write_val;
    uint32 speedi, speedv;
    uint32 speedi_h, speedi_l;
    uint64 speedi_64;
    int osr;
    SOC_INIT_FUNC_DEFS;

    if (speed == cprimodRsvd4SpdMult4X) {
        speedi = 4;
        speedv = 2;
        osr = 5;
    } else if (speed == cprimodRsvd4SpdMult8X) {
        speedi = 8;
        speedv = 3;
        osr = 2;
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Speed is not supported for rsvd4"));
    }
    SOC_IF_ERROR_RETURN(READ_CPRI_PMD_TX_CTRLr(unit, port, &data));
    COMPILER_64_SET(write_val, 0, osr);
    soc_reg64_field_set(unit, CPRI_PMD_TX_CTRLr, &data, PMD_TX_OSR_MODEf, write_val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_PMD_TX_CTRLr(unit, port, data));
    falcon2_monterey_frc_tx_osr(&phy->access, osr);

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_12r(unit, port,
                                                         &data));
    /*
     * Force TRANSMITTER_EN=0 per spec.  Application need to
     * set to TRANSMITTER_EN = 1 when it is ready.
     */
    COMPILER_64_SET(write_val, 0, 0);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_12r, &data,
                        BRCM_RESERVED_CPM_12_7f, write_val);

    COMPILER_64_SET(write_val, 0, 1);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_12r, &data,
                        BRCM_RESERVED_CPM_12_3f, write_val);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_12r, &data,
                        BRCM_RESERVED_CPM_12_8f, write_val);
    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_12r(unit, port,
                                                          data));
    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_3r(unit, port,
                                                         &data));

    /*
     * The padding is 1 for default supported configuration.
     * cprimod_rsvd4_tx_padding_size_set() is used as well.
     */
    COMPILER_64_SET(write_val, 0, 1);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_3r, &data,
                        BRCM_RESERVED_CPM_3_1f, write_val);

    COMPILER_64_SET(write_val, 0, speedv);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_3r, &data,
                        BRCM_RESERVED_CPM_3_2f, write_val);

    COMPILER_64_SET(write_val, 0, (uint32) CPRIF_DRV_NUMBER_OF_MSG);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_3r, &data,
                        BRCM_RESERVED_CPM_3_3f, write_val);

    COMPILER_64_SET(write_val, 0, (uint32) CPRIF_DRV_N_MG_IN_MSTR_FRM);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_3r, &data,
                        BRCM_RESERVED_CPM_3_4f, write_val);
    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_3r(unit, port,
                                                          data));

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_4r(unit, port,
                                                         &data));
    speedi_h = 0;
    speedi_l = ((speedi*CPRIF_DRV_NUMBER_OF_MSG)-1) & 0xffffffff;
    COMPILER_64_SET(speedi_64, speedi_h, speedi_l);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_4r, &data,
                        BRCM_RESERVED_CPM_4_0f, speedi_64);
    speedi_l = ((speedi*(CPRIF_DRV_N_MG_IN_MSTR_FRM-1) *
               CPRIF_DRV_NUMBER_OF_MSG)-1) & 0xffffffff;
    COMPILER_64_SET(speedi_64, speedi_h, speedi_l);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_4r, &data,
                        BRCM_RESERVED_CPM_4_1f, speedi_64);
    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_4r(unit, port, data));
    COMPILER_64_ZERO(data);
    COMPILER_64_SET(write_val, 0, 1);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_2r, &data,
                        BRCM_RESERVED_CPM_2_0f, write_val);
    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_2r(unit, port,
                                                          data));

    /*
     * Clear the PCS_WORD_LEN in RSVD4 mode.
     */
    SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
    COMPILER_64_SET(write_val, 0, 0);
    soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data,
                        TX_PCS_WORD_LEN_8B10Bf, write_val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXPCS_CTRLr(unit, port, data));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_txpcs_speed_get(int unit, int port,
                                    cprimod_port_rsvd4_speed_mult_t *speed)
{
    uint64 data;
    uint64 read_val;
    uint32 speedv;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_3r(unit, port,&data));
    read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_3r,
                                   data, BRCM_RESERVED_CPM_3_2f);

    speedv = COMPILER_64_LO(read_val);
    *speed = cprimodSpdCount;
    if (speedv ==  2) {
        *speed = cprimodRsvd4SpdMult4X;
    } else if (speedv ==  3) {
        *speed = cprimodRsvd4SpdMult8X;
    } else {
        *speed = cprimodRsvd4SpdMultCount;
    }

    SOC_FUNC_RETURN;
}

int cprif_drv_txcpri_port_type_get(int unit, int port,
                                   cprimod_port_interface_type_t *interface)
{
    uint64 data;
    uint64 read_val;
    uint32 iftype;
    cprif_pm_version_t pm_version;
    int tunnel_enable;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN
        (cprif_pm_version_get(unit, port, &pm_version));
    /*
     * If A0, just return as not enable, as it does not support
     * tunnel mode.
     */
    if (CPRIF_CLPORT_IS_A0(pm_version)) {
        tunnel_enable = FALSE;
    } else {
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));

        read_val = soc_reg64_field_get(unit, CPRI_TXPCS_CTRLr, data,
                                       TX_PCS_TUNNEL_MODEf);
        if (COMPILER_64_IS_ZERO(read_val)) {
            tunnel_enable = FALSE;
        } else {
            tunnel_enable = TRUE;
            *interface = cprimodTunnel;
        }
    }

    if (!tunnel_enable) {
        /*
         * checking the PCS configuration for RSVD4 enable.
         * BRCM_RESERVED_CPM_2,BRCM_RESERVED_CPM_2_0f is not
         * available in agnostic mode.
        */
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_12r(unit, port,
                                                         &data));
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_12r,
                                   data, BRCM_RESERVED_CPM_12_8f);
        iftype = COMPILER_64_LO(read_val);

        if (iftype == 1) {
            *interface = cprimodRsvd4;
        } else {
            *interface = cprimodCpri;
        }
    }
    SOC_FUNC_RETURN;
}

int cprif_drv_rxcpri_port_type_get(int unit, int port,
                                   cprimod_port_interface_type_t *interface)
{
    uint64 data;
    uint64 read_val;
    uint32 iftype;
    cprif_pm_version_t pm_version;
    int tunnel_enable;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN
        (cprif_pm_version_get(unit, port, &pm_version));

    /*
     * If A0, just return as not enable, as it does not support
     * tunnel mode.
     */
    if (CPRIF_CLPORT_IS_A0(pm_version)) {
        tunnel_enable = FALSE;
    } else {
        SOC_IF_ERROR_RETURN(READ_CPRI_RXPCS_CTRLr(unit, port, &data));

        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                       RXPCS_TUNNEL_MODEf);
        if (COMPILER_64_IS_ZERO(read_val)) {
            tunnel_enable = FALSE;
        } else {
            tunnel_enable = TRUE;
            *interface = cprimodTunnel;
        }
    }
    if (!tunnel_enable) {
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port,&data));

        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_307r,
                                   data, BRCM_RESERVED_CPM_307_0f);
        iftype = COMPILER_64_LO(read_val);

        if (iftype == 1) {
            *interface = cprimodRsvd4;
        } else {
            *interface = cprimodCpri;
        }
    }
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_frame_optional_config_set(int unit, int port,
                                        cprimod_rx_config_field_t field,
                                        int value)
{
    uint64 data;
    uint64 write_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RXPCS_CTRLr(unit, port, &data));
    COMPILER_64_SET(write_val, 0, (uint32) value);
    switch (field) {
    case cprimodRxConfig8b10bAllowSeedChange:
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                            RXPCS_8B10B_ALLOW_SEED_CHANGEf, write_val);
        break;
    case cprimodRxConfig8b10bDescrHw:
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                            RXPCS_8B10B_DESCR_HWf, write_val);
        break;
    case cprimodRxConfig8b10bDescrEn:
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                            RXPCS_8B10B_DESCR_ENf, write_val);
        break;
    case cprimodRxConfiglinkAcqSwMode:
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                            RXPCS_LINK_ACQ_SW_MODEf, write_val);
        break;
    case cprimodRxConfig64b66bLosOption:
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                            RXPCS_64B66B_LOS_OPTIONf, write_val);
        break;
    case cprimodRxConfig8b10bForceCommaAlignEn:
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                            RXPCS_8B10B_FORCE_COMMA_ALIGN_ENf, write_val);
        break;
    case cprimodRxConfig64b66bBerWindowLimit:
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                            RXPCS_64B66B_BER_WINDOW_LIMITf, write_val);
        break;
    case cprimodRxConfig64b66bBerLimit:
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                            RXPCS_64B66B_BER_LIMITf, write_val);
        break;
    case cprimodRxConfigtestMode:
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                            RXPCS_TEST_MODEf, write_val);
        break;
    case cprimodRxConfig64b66bInvalidShCnt:
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                            RXPCS_64B66B_INVALID_SH_CNTf, write_val);
        break;
    case cprimodRxConfig64b66bValidShCnt:
        soc_reg64_field_set(unit, CPRI_RXPCS_CTRLr, &data,
                            RXPCS_64B66B_VALID_SH_CNTf, write_val);
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for Rx cpri"));
        break;
    }
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RXPCS_CTRLr(unit, port, data));

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_rx_frame_optional_config_get(int unit, int port,
                                           cprimod_rx_config_field_t field,
                                           int* value)
{
    uint64 data;
    uint64 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RXPCS_CTRLr(unit, port, &data));
    switch (field) {
    case cprimodRxConfig8b10bAllowSeedChange:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                     RXPCS_8B10B_ALLOW_SEED_CHANGEf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxConfig8b10bDescrHw:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                     RXPCS_8B10B_DESCR_HWf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxConfig8b10bDescrEn:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                     RXPCS_8B10B_DESCR_ENf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxConfiglinkAcqSwMode:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                     RXPCS_LINK_ACQ_SW_MODEf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxConfig64b66bLosOption:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                     RXPCS_64B66B_LOS_OPTIONf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxConfig8b10bForceCommaAlignEn:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                     RXPCS_8B10B_FORCE_COMMA_ALIGN_ENf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxConfig64b66bBerWindowLimit:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                     RXPCS_64B66B_BER_WINDOW_LIMITf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxConfig64b66bBerLimit:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                     RXPCS_64B66B_BER_LIMITf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxConfigtestMode:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                     RXPCS_TEST_MODEf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxConfig64b66bInvalidShCnt:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                     RXPCS_64B66B_INVALID_SH_CNTf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxConfig64b66bValidShCnt:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_CTRLr, data,
                                     RXPCS_64B66B_VALID_SH_CNTf);
        *value = COMPILER_64_LO(read_val);
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for Rx cpri"));
        break;
    }

exit:
    SOC_FUNC_RETURN;

}


int cprif_drv_tx_frame_optional_config_set(int unit, int port,
                                           cprimod_tx_config_field_t field,
                                           uint32 value)
{
    uint64 data;
    uint64 write_val;
    uint32 data32;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_SET(write_val, 0, value);
    switch (field) {
    case cprimodTxConfigagnosticMode:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
        soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data,
                            BRCM_RESERVED_CPM_11_8f, write_val);
        SOC_IF_ERROR_RETURN(WRITE_CPRI_TXPCS_CTRLr(unit, port, data));
        break;
    case cprimodTxConfigtxpmdDisableOverrideVal:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
        soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data,
                            TX_PCS_TXPMD_DISABLE_OVERRIDE_VALf, write_val);
        SOC_IF_ERROR_RETURN(WRITE_CPRI_TXPCS_CTRLr(unit, port, data));
        break;
    case cprimodTxConfigtxpmdDisableOverrideEn:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
        soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data,
                            TX_PCS_TXPMD_DISABLE_OVERRIDE_ENf, write_val);
        SOC_IF_ERROR_RETURN(WRITE_CPRI_TXPCS_CTRLr(unit, port, data));
        break;
    case cprimodTxConfigseed8B10B:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
        soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data,
                            TX_PCS_SEED_8B10Bf, write_val);
        SOC_IF_ERROR_RETURN(WRITE_CPRI_TXPCS_CTRLr(unit, port, data));
        break;
    case cprimodTxConfigscrambleBypass:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
        soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data,
                            TX_PCS_SCRAMBLE_BYPASSf, write_val);
        SOC_IF_ERROR_RETURN(WRITE_CPRI_TXPCS_CTRLr(unit, port, data));
        break;
    case cprimodTxConfigseed8b10b:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
        soc_reg64_field_set(unit, CPRI_TXPCS_CTRLr, &data,
                            TX_PCS_SEED_8B10Bf, write_val);
        SOC_IF_ERROR_RETURN(WRITE_CPRI_TXPCS_CTRLr(unit, port, data));
        break;
    case cprimodTxConfigcwaScrEn8b10b:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_CWA_GENERALr(unit, port, &data32));
        soc_reg_field_set(unit, CPRI_TXFRM_CWA_GENERALr, &data32,
                          TX_CWA_SCR_EN_8B10Bf, value);
        SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_CWA_GENERALr(unit, port, data32));
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for Tx cpri"));
        break;
    }

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_tx_frame_optional_config_get(int unit, int port,
                                           cprimod_tx_config_field_t field,
                                           uint32* value)
{
    uint64 data;
    uint64 read_val;
    uint32 data32;
    SOC_INIT_FUNC_DEFS;

    switch (field) {
    case cprimodTxConfigagnosticMode:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
        read_val = soc_reg64_field_get(unit, CPRI_TXPCS_CTRLr, data,
                                     BRCM_RESERVED_CPM_11_8f);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodTxConfigtxpmdDisableOverrideVal:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
        read_val = soc_reg64_field_get(unit, CPRI_TXPCS_CTRLr, data,
                                     TX_PCS_TXPMD_DISABLE_OVERRIDE_VALf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodTxConfigtxpmdDisableOverrideEn:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
        read_val = soc_reg64_field_get(unit, CPRI_TXPCS_CTRLr, data,
                                     TX_PCS_TXPMD_DISABLE_OVERRIDE_ENf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodTxConfigseed8B10B:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
        read_val = soc_reg64_field_get(unit, CPRI_TXPCS_CTRLr, data,
                                     TX_PCS_SEED_8B10Bf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodTxConfigscrambleBypass:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
        read_val = soc_reg64_field_get(unit, CPRI_TXPCS_CTRLr, data,
                                     TX_PCS_SCRAMBLE_BYPASSf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodTxConfigseed8b10b:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXPCS_CTRLr(unit, port, &data));
        read_val = soc_reg64_field_get(unit, CPRI_TXPCS_CTRLr, data,
                                     TX_PCS_SEED_8B10Bf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodTxConfigcwaScrEn8b10b:
        SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_CWA_GENERALr(unit, port, &data32));
        *value = soc_reg_field_get(unit, CPRI_TXFRM_CWA_GENERALr, data32,
                                   TX_CWA_SCR_EN_8B10Bf);
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for Tx cpri"));
        break;
    }

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_rsvd4_rx_frame_optional_config_set(int unit, int port,
                                                 cprimod_rsvd4_rx_config_field_t field,
                                                 int value)
{
    uint64 data;
    uint64 write_val;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_SET(write_val, 0, (uint32) value);
    switch (field) {
    case CPRIMOD_RSVD4_RX_CONFIG_DESCRAMBLE_ENABLE:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port, &data));
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_307r, &data,
                            BRCM_RESERVED_CPM_307_1f, write_val);
        SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_307r(unit, port, data));
        break;
    case CPRIMOD_RSVD4_RX_CONFIG_SYNC_T:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port, &data));
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_307r, &data,
                            BRCM_RESERVED_CPM_307_3f, write_val);
        SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_307r(unit, port, data));
        break;
    case CPRIMOD_RSVD4_RX_CONFIG_UNSYNC_T:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port, &data));
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_307r, &data,
                            BRCM_RESERVED_CPM_307_4f, write_val);
        SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_307r(unit, port, data));
        break;
    case CPRIMOD_RSVD4_RX_CONFIG_FWD_IDLE_TO_FRAMER:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port, &data));
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_307r, &data,
                            BRCM_RESERVED_CPM_307_7f, write_val);
        SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_307r(unit, port, data));
        break;

    case CPRIMOD_RSVD4_RX_CONFIG_FRAME_SYNC_T:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_308r(unit, port, &data));
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_308r, &data,
                            BRCM_RESERVED_CPM_308_0f, write_val);
        SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_308r(unit, port, data));
        break;
    case CPRIMOD_RSVD4_RX_CONFIG_FRAME_UNSYNC_T:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_308r(unit, port, &data));
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_308r, &data,
                            BRCM_RESERVED_CPM_308_1f, write_val);
        SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_308r(unit, port, data));
        break;
    case CPRIMOD_RSVD4_RX_CONFIG_BLOCK_SIZE:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port, &data));
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_307r, &data,
                            BRCM_RESERVED_CPM_307_5f, write_val);
        SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_307r(unit, port, data));
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for Rx rsvd4"));
        break;
    }

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_rsvd4_rx_frame_optional_config_get(int unit, int port,
                                                 cprimod_rsvd4_rx_config_field_t field,
                                                 int* value)
{
    uint64 data;
    uint64 read_val;
    SOC_INIT_FUNC_DEFS;

    switch (field) {
    case CPRIMOD_RSVD4_RX_CONFIG_DESCRAMBLE_ENABLE:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port, &data));
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_307r, data,
                                     BRCM_RESERVED_CPM_307_1f);
        *value = COMPILER_64_LO(read_val);
        break;
    case CPRIMOD_RSVD4_RX_CONFIG_SYNC_T:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port, &data));
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_307r, data,
                                     BRCM_RESERVED_CPM_307_3f);
        *value = COMPILER_64_LO(read_val);
        break;
    case CPRIMOD_RSVD4_RX_CONFIG_UNSYNC_T:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port, &data));
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_307r, data,
                                     BRCM_RESERVED_CPM_307_4f);
        *value = COMPILER_64_LO(read_val);
        break;
    case CPRIMOD_RSVD4_RX_CONFIG_FRAME_SYNC_T:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_308r(unit, port, &data));
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_308r, data,
                                     BRCM_RESERVED_CPM_308_0f);
        *value = COMPILER_64_LO(read_val);
        break;
    case CPRIMOD_RSVD4_RX_CONFIG_FRAME_UNSYNC_T:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_308r(unit, port, &data));
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_308r, data,
                                     BRCM_RESERVED_CPM_308_1f);
        *value = COMPILER_64_LO(read_val);
        break;
    case CPRIMOD_RSVD4_RX_CONFIG_BLOCK_SIZE:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port, &data));
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_307r, data,
                                     BRCM_RESERVED_CPM_307_5f);
        *value = COMPILER_64_LO(read_val);
        break;
    case CPRIMOD_RSVD4_RX_CONFIG_FWD_IDLE_TO_FRAMER:
        SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_307r(unit, port, &data));
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_307r, data,
                            BRCM_RESERVED_CPM_307_7f);
        *value = COMPILER_64_LO(read_val);
        break;

    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for Rx rsvd4"));
        break;
    }

exit:
    SOC_FUNC_RETURN;

}


int cprif_drv_rsvd4_rx_fsm_state_set(int unit, int port,
                                     cprimod_rsvd4_rx_fsm_state_t state)
{
    uint64 data;
    uint64 write_val;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(data);
    COMPILER_64_SET(write_val, 0, state);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_310r, &data,
                        BRCM_RESERVED_CPM_310_0f, write_val);
    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_310r(unit, port, data));

    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_rx_fsm_state_get(int unit, int port,
                                     cprimod_rsvd4_rx_fsm_state_t* state)
{
    uint64 data;
    uint64 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_310r(unit, port, &data));
    read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_310r, data,
                                 BRCM_RESERVED_CPM_310_0f);
    *state = COMPILER_64_LO(read_val);

    SOC_FUNC_RETURN;
}


int cprif_drv_rsvd4_rx_overide_set(int unit, int port,
                                   cprimod_rsvd4_rx_overide_t parameter,
                                   int enable, int value)
{
    uint64 data;
    uint64 write_val, write_val1;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_311r(unit, port, &data));

    COMPILER_64_SET(write_val, 0, (uint32) value);
    COMPILER_64_SET(write_val1, 0, (uint32) enable);
    switch (parameter) {
    case cprimodRsvd4RxOverideFrameUnsyncTInvldMgRecvd:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_311r, &data,
                            BRCM_RESERVED_CPM_311_0f, write_val);
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_311r, &data,
                            BRCM_RESERVED_CPM_311_1f, write_val1);
        break;
    case cprimodRsvd4RxOverideFrameSyncTVldMgRecvd:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_311r, &data,
                            BRCM_RESERVED_CPM_311_2f, write_val);
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_311r, &data,
                            BRCM_RESERVED_CPM_311_3f, write_val1);
        break;
    case cprimodRsvd4RxOverideKMgIdlesRecvd:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_311r, &data,
                            BRCM_RESERVED_CPM_311_4f, write_val);
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_311r, &data,
                            BRCM_RESERVED_CPM_311_5f, write_val1);
        break;
    case cprimodRsvd4RxOverideIdleReqRecvd:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_311r, &data,
                            BRCM_RESERVED_CPM_311_6f, write_val);
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_311r, &data,
                            BRCM_RESERVED_CPM_311_7f, write_val1);
        break;
    case cprimodRsvd4RxOverideIdleAckRecvd:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_311r, &data,
                            BRCM_RESERVED_CPM_311_8f, write_val);
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_311r, &data,
                            BRCM_RESERVED_CPM_311_9f, write_val1);
        break;
    case cprimodRsvd4RxOverideSeedCapAndVerifyDone:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_311r, &data,
                            BRCM_RESERVED_CPM_311_10f, write_val);
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_311r, &data,
                            BRCM_RESERVED_CPM_311_11f, write_val1);
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for rsvd4 Rx overide"));
        break;
    }
    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_311r(unit, port, data));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_rx_overide_get(int unit, int port,
                                   cprimod_rsvd4_rx_overide_t parameter,
                                   int* enable, int* value)
{
    uint64 data;
    uint64 read_val, read_val1;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_311r(unit, port, &data));
    switch (parameter) {
    case cprimodRsvd4RxOverideFrameUnsyncTInvldMgRecvd:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_311r, data,
                                     BRCM_RESERVED_CPM_311_0f);
        *value = COMPILER_64_LO(read_val);
        read_val1 = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_311r, data,
                                      BRCM_RESERVED_CPM_311_1f);
        *enable = COMPILER_64_LO(read_val1);
        break;
    case cprimodRsvd4RxOverideFrameSyncTVldMgRecvd:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_311r, data,
                                     BRCM_RESERVED_CPM_311_2f);
        *value = COMPILER_64_LO(read_val);
        read_val1 = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_311r, data,
                                      BRCM_RESERVED_CPM_311_3f);
        *enable = COMPILER_64_LO(read_val1);
        break;
    case cprimodRsvd4RxOverideKMgIdlesRecvd:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_311r, data,
                                     BRCM_RESERVED_CPM_311_4f);
        *value = COMPILER_64_LO(read_val);
        read_val1 = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_311r, data,
                                      BRCM_RESERVED_CPM_311_5f);
        *enable = COMPILER_64_LO(read_val1);
        break;
    case cprimodRsvd4RxOverideIdleReqRecvd:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_311r, data,
                                     BRCM_RESERVED_CPM_311_6f);
        *value = COMPILER_64_LO(read_val);
        read_val1 = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_311r, data,
                                      BRCM_RESERVED_CPM_311_7f);
        *enable = COMPILER_64_LO(read_val1);
        break;
    case cprimodRsvd4RxOverideIdleAckRecvd:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_311r, data,
                                     BRCM_RESERVED_CPM_311_8f);
        *value = COMPILER_64_LO(read_val);
        read_val1 = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_311r, data,
                                      BRCM_RESERVED_CPM_311_9f);
        *enable = COMPILER_64_LO(read_val1);
        break;
    case cprimodRsvd4RxOverideSeedCapAndVerifyDone:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_311r, data,
                                     BRCM_RESERVED_CPM_311_10f);
        *value = COMPILER_64_LO(read_val);
        read_val1 = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_311r, data,
                                      BRCM_RESERVED_CPM_311_11f);
        *enable = COMPILER_64_LO(read_val1);
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for rsvd4 Rx overide"));
        break;
    }

exit:
    SOC_FUNC_RETURN;

}


int cprif_drv_rsvd4_tx_frame_optional_config_set(int unit, int port,
                                                 cprimod_rsvd4_tx_config_field_t field,
                                                 int value)
{
    uint64 data;
    uint64 write_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_12r(unit, port, &data));
    COMPILER_64_SET(write_val, 0, (uint32) value);
    switch (field) {
    case CPRIMOD_RSVD4_TX_CONFIG_FORCE_OFF_STATE:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_12r, &data,
                            BRCM_RESERVED_CPM_12_0f, write_val);
        break;
    case CPRIMOD_RSVD4_TX_CONFIG_ACK_T:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_12r, &data,
                            BRCM_RESERVED_CPM_12_1f, write_val);
        break;
    case CPRIMOD_RSVD4_TX_CONFIG_FORCE_IDLE_ACK:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_12r, &data,
                            BRCM_RESERVED_CPM_12_4f, write_val);
        break;
    case CPRIMOD_RSVD4_TX_CONFIG_LOS_ENABLE:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_12r, &data,
                            BRCM_RESERVED_CPM_12_6f, write_val);
        break;
    case CPRIMOD_RSVD4_TX_CONFIG_SCRAMBLE_ENABLE:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_12r, &data,
                            BRCM_RESERVED_CPM_12_5f, write_val);
        break;
    case CPRIMOD_RSVD4_TX_CONFIG_SCRAMBLE_SEED:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_12r, &data,
                            BRCM_RESERVED_CPM_12_2f, write_val);
        break;
    case CPRIMOD_RSVD4_TX_CONFIG_TRANSMITTER_ENABLE:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_12r, &data,
                            BRCM_RESERVED_CPM_12_7f, write_val);
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for Tx rsvd4 overide"));
        break;
    }
    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_12r(unit, port, data));

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_rsvd4_tx_frame_optional_config_get(int unit, int port,
                                                 cprimod_rsvd4_tx_config_field_t field,
                                                 int* value)
{
    uint64 data;
    uint64 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_12r(unit, port, &data));
    switch (field) {
    case CPRIMOD_RSVD4_TX_CONFIG_FORCE_OFF_STATE:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_12r, data,
                                     BRCM_RESERVED_CPM_12_0f);
        *value = COMPILER_64_LO(read_val);
        break;
    case CPRIMOD_RSVD4_TX_CONFIG_ACK_T:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_12r, data,
                                     BRCM_RESERVED_CPM_12_1f);
        *value = COMPILER_64_LO(read_val);
        break;
    case CPRIMOD_RSVD4_TX_CONFIG_FORCE_IDLE_ACK:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_12r, data,
                                     BRCM_RESERVED_CPM_12_4f);
        *value = COMPILER_64_LO(read_val);
        break;
    case CPRIMOD_RSVD4_TX_CONFIG_LOS_ENABLE:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_12r, data,
                                     BRCM_RESERVED_CPM_12_6f);
        *value = COMPILER_64_LO(read_val);
        break;
    case CPRIMOD_RSVD4_TX_CONFIG_SCRAMBLE_ENABLE:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_12r, data,
                                     BRCM_RESERVED_CPM_12_5f);
        *value = COMPILER_64_LO(read_val);
        break;
    case CPRIMOD_RSVD4_TX_CONFIG_SCRAMBLE_SEED:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_12r, data,
                                     BRCM_RESERVED_CPM_12_2f);
        *value = COMPILER_64_LO(read_val);
        break;
    case CPRIMOD_RSVD4_TX_CONFIG_TRANSMITTER_ENABLE:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_12r, data,
                                     BRCM_RESERVED_CPM_12_7f);
        *value = COMPILER_64_LO(read_val);
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for Tx rsvd4 overide"));
        break;
    }

exit:
    SOC_FUNC_RETURN;

}


int cprif_drv_rsvd4_tx_fsm_state_set(int unit, int port,
                                     cprimod_rsvd4_tx_fsm_state_t state)
{
    uint64 data;
    uint64 write_val;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(data);
    COMPILER_64_SET(write_val, 0, state);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_13r, &data,
                        BRCM_RESERVED_CPM_13_0f, write_val);
    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_13r(unit, port, data));

    SOC_FUNC_RETURN;

}

int cprif_drv_rsvd4_tx_fsm_state_get(int unit, int port,
                                     cprimod_rsvd4_tx_fsm_state_t* state)
{
    uint64 data;
    uint64 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_13r(unit, port, &data));
    read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_13r, data,
                                 BRCM_RESERVED_CPM_13_0f);
    *state = COMPILER_64_LO(read_val);

    SOC_FUNC_RETURN;
}


int cprif_drv_rsvd4_tx_overide_set(int unit, int port,
                                   cprimod_rsvd4_tx_overide_t parameter,
                                   int enable, int value)
{
    uint64 data;
    uint64 write_val, write_val1;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_14r(unit, port, &data));
    COMPILER_64_SET(write_val, 0, value);
    COMPILER_64_SET(write_val1, 0, enable);
    switch (parameter) {
    case cprimodRsvd4TxOverideStartTx:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_14r, &data,
                            BRCM_RESERVED_CPM_14_0f, write_val);
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_14r, &data,
                            BRCM_RESERVED_CPM_14_1f, write_val1);
        break;
    case cprimodRsvd4TxOverideRxPcsAckCap:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_14r, &data,
                            BRCM_RESERVED_CPM_14_2f, write_val);
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_14r, &data,
                            BRCM_RESERVED_CPM_14_3f, write_val1);
        break;
    case cprimodRsvd4TxOverideRxPcsIdleReq:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_14r, &data,
                            BRCM_RESERVED_CPM_14_4f, write_val);
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_14r, &data,
                            BRCM_RESERVED_CPM_14_5f, write_val1);
        break;
    case cprimodRsvd4TxOverideRxPcsScrLock:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_14r, &data,
                            BRCM_RESERVED_CPM_14_6f, write_val);
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_14r, &data,
                            BRCM_RESERVED_CPM_14_7f, write_val1);
        break;
    case cprimodRsvd4TxOverideLosStauts:
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_14r, &data,
                            BRCM_RESERVED_CPM_14_8f, write_val);
        soc_reg64_field_set(unit, BRCM_RESERVED_CPM_14r, &data,
                            BRCM_RESERVED_CPM_14_9f, write_val1);
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for Tx rsvd4 overide"));
        break;
    }
    SOC_IF_ERROR_RETURN(WRITE_BRCM_RESERVED_CPM_14r(unit, port, data));

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_rsvd4_tx_overide_get(int unit, int port,
                                   cprimod_rsvd4_tx_overide_t parameter,
                                   int* enable, int* value)
{
    uint64 data;
    uint64 read_val, read_val1;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_BRCM_RESERVED_CPM_14r(unit, port, &data));
    switch (parameter) {
    case cprimodRsvd4TxOverideStartTx:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_14r, data,
                                     BRCM_RESERVED_CPM_14_0f);
        *value = COMPILER_64_LO(read_val);
        read_val1 = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_14r, data,
                                     BRCM_RESERVED_CPM_14_1f);
        *enable = COMPILER_64_LO(read_val1);
        break;
    case cprimodRsvd4TxOverideRxPcsAckCap:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_14r, data,
                                     BRCM_RESERVED_CPM_14_2f);
        *value = COMPILER_64_LO(read_val);
        read_val1 = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_14r, data,
                                      BRCM_RESERVED_CPM_14_3f);
        *enable = COMPILER_64_LO(read_val1);
        break;
    case cprimodRsvd4TxOverideRxPcsIdleReq:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_14r, data,
                                     BRCM_RESERVED_CPM_14_4f);
        *value = COMPILER_64_LO(read_val);
        read_val1 = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_14r, data,
                                      BRCM_RESERVED_CPM_14_5f);
        *enable = COMPILER_64_LO(read_val1);
        break;
    case cprimodRsvd4TxOverideRxPcsScrLock:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_14r, data,
                                     BRCM_RESERVED_CPM_14_6f);
        *value = COMPILER_64_LO(read_val);
        read_val1 = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_14r, data,
                                      BRCM_RESERVED_CPM_14_7f);
        *enable = COMPILER_64_LO(read_val1);
        break;
    case cprimodRsvd4TxOverideLosStauts:
        read_val = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_14r, data,
                                     BRCM_RESERVED_CPM_14_8f);
        *value = COMPILER_64_LO(read_val);
        read_val1 = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_14r, data,
                                      BRCM_RESERVED_CPM_14_9f);
        *enable = COMPILER_64_LO(read_val1);
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for Tx rsvd4 overide"));
        break;
    }

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_pmd_port_status_get(int unit, int port, cprimod_pmd_port_status_t status,
                                uint32* value)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPMPORT_PMD_PORT_STATUSr(unit, port, &data));
    switch (status) {
    case cprimodPmdPortStatusTxClockValid:
        *value = soc_reg_field_get(unit, CPMPORT_PMD_PORT_STATUSr, data,
                                   PMD_TX_CLK_VLD_STSf);
        break;

    case cprimodPmdPortStatusRxClockValid:
        *value = soc_reg_field_get(unit, CPMPORT_PMD_PORT_STATUSr, data,
                                   PMD_RX_CLK_VLD_STSf);
        break;

    case cprimodPmdPortStatusRxLock:
        *value = soc_reg_field_get(unit, CPMPORT_PMD_PORT_STATUSr, data,
                                   PMD_RX_LOCK_STSf);
        break;

    case cprimodPmdPortStatusEnergyDetected:
        *value = soc_reg_field_get(unit, CPMPORT_PMD_PORT_STATUSr, data,
                                   PMD_ENERGY_DETECT_STSf);
        break;

    case cprimodPmdPortStatusSignalDetected:
        *value = soc_reg_field_get(unit, CPMPORT_PMD_PORT_STATUSr, data,
                                   PMD_SIGNAL_DETECT_STSf);
        break;

    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for PMD Port status."));
        break;
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_pcs_status_get(int unit, int port, cprimod_rx_pcs_status_t status,
                                uint32* value)
{
    uint64 data;
    uint64 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RXPCS_STATUSr(unit, port, &data));
    switch (status) {
    case cprimodRxPcsStatusLinkStatusLive:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_STATUSr, data,
                                     RXPCS_LINK_STATUS_LIVEf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxPcsStatusSeedLocked:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_STATUSr, data,
                                     RXPCS_SEED_LOCKEDf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxPcsStatusSeedVector:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_STATUSr, data,
                                     RXPCS_SEED_VECTORf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxPcsStatusLosLive:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_STATUSr, data,
                                     RXPCS_LOS_LIVEf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxPcsStatus64b66bHiBerLive:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_STATUSr, data,
                                     RXPCS_64B66B_HI_BER_LIVEf);
        *value = COMPILER_64_LO(read_val);
        break;
    case cprimodRxPcsStatus64b66bBlockLockLive:
        read_val = soc_reg64_field_get(unit, CPRI_RXPCS_STATUSr, data,
                                     RXPCS_64B66B_BLOCK_LOCK_LIVEf);
        *value = COMPILER_64_LO(read_val);
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config is not supported for rsvd4 Rx overide"));
        break;
    }

exit:
    SOC_FUNC_RETURN;
}

/*
* Functions related to init
*/

int cprif_drv_datapath_credit_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, DATAPATH_CREDIT_RSTf,
                      val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_datapath_credit_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val,
                             DATAPATH_CREDIT_RSTf);

    SOC_FUNC_RETURN;
}


int cprif_drv_cip_rx_dp_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_CIP_RX_DPf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_cip_rx_dp_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val, RESET_CIP_RX_DPf);

    SOC_FUNC_RETURN;
}


int cprif_drv_cip_tx_dp_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_CIP_TX_DPf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_cip_tx_dp_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val, RESET_CIP_TX_DPf);

    SOC_FUNC_RETURN;
}


int cprif_drv_cip_tx_h_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_CIP_TX_Hf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_cip_tx_h_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val, RESET_CIP_TX_Hf);

    SOC_FUNC_RETURN;
}


int cprif_drv_cip_rx_h_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_CIP_RX_Hf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_cip_rx_h_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val, RESET_CIP_RX_Hf);

    SOC_FUNC_RETURN;
}


int cprif_drv_datapath_rx_dp_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_DATAPATH_RX_DPf,
                      val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_datapath_rx_dp_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val,
                             RESET_DATAPATH_RX_DPf);

    SOC_FUNC_RETURN;
}


int cprif_drv_datapath_tx_dp_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_DATAPATH_TX_DPf,
                      val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_datapath_tx_dp_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val,
                             RESET_DATAPATH_TX_DPf);

    SOC_FUNC_RETURN;
}


int cprif_drv_datapath_rx_h_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_DATAPATH_RX_Hf,
                      val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_datapath_rx_h_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val,
                             RESET_DATAPATH_RX_Hf);

    SOC_FUNC_RETURN;
}


int cprif_drv_datapath_tx_h_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_DATAPATH_TX_Hf,
                      val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_datapath_tx_h_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val,
                             RESET_DATAPATH_TX_Hf);

    SOC_FUNC_RETURN;
}

/*
 * set Reg_cpri_cpri_rst_ctrl.reset_rx_dp to a particular value.
 * Default is 0 (not in reset).
 */
int cprif_drv_rx_dp_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_RX_DPf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_rx_dp_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val, RESET_RX_DPf);

    SOC_FUNC_RETURN;
}

/*
 * set Reg_cpri_cpri_rst_ctrl.reset_rx_h to a particular value.
 * Default is 0 (not in reset).
 */
int cprif_drv_rx_h_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_RX_Hf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_rx_h_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val, RESET_RX_Hf);

    SOC_FUNC_RETURN;
}

/*
 * set Reg_cpri_cpri_rst_ctrl.reset_tx_dp to a particular value.
 * Default is 0 (not in reset).
 */
int cprif_drv_tx_dp_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_TX_DPf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_tx_dp_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val, RESET_TX_DPf);

    SOC_FUNC_RETURN;
}

/*
 * set Reg_cpri_cpri_rst_ctrl.reset_tx_h to a particular value.
 * Default is 0 (not in reset).
 */
int cprif_drv_tx_h_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_TX_Hf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_tx_h_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val, RESET_TX_Hf);

    SOC_FUNC_RETURN;
}

/*
 * set Reg_cpri_cpri_rst_ctrl.glas_rst to a particular value.
 * Default is 0 (not in reset).
 */
int cprif_drv_glas_reset_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, GLAS_RSTf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_glas_reset_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RST_CTRLr, read_val, GLAS_RSTf);

    SOC_FUNC_RETURN;
}

/*
 * set Reg_cpri_cpri_clk_ctrl.disable_cipclk_clk to a particular value.
 * Default is 0 (not gated).
 */
int cprif_drv_disable_cip_clk_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_CLK_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_CLK_CTRLr, &read_val, DISABLE_CIPCLK_CLKf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_CLK_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_disable_cip_clk_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_CLK_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_CLK_CTRLr, read_val, DISABLE_CIPCLK_CLKf);

    SOC_FUNC_RETURN;
}

/*
 * set Reg_cpri_cpri_clk_ctrl.disable_glas_clk to a particular value.
 * Default is 0 (not gated).
 */
int cprif_drv_disable_glas_clk_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_CLK_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_CLK_CTRLr, &read_val, DISABLE_GLAS_CLKf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_CLK_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_disable_glas_clk_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_CLK_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_CLK_CTRLr, read_val, DISABLE_GLAS_CLKf);

    SOC_FUNC_RETURN;
}

/*
 * set Reg_cpri_cpri_clk_ctrl.disable_rx_clk to a particular value.
 * Default is 0 (not gated).
 */
int cprif_drv_disable_rx_clk_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_CLK_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_CLK_CTRLr, &read_val, DISABLE_RX_CLKf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_CLK_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_disable_rx_clk_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_CLK_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_CLK_CTRLr, read_val, DISABLE_RX_CLKf);

    SOC_FUNC_RETURN;
}

/*
 * set Reg_cpri_cpri_clk_ctrl.disable_tx_clk to a particular value.
 * Default is 0 (not gated).
 */
int cprif_drv_disable_tx_clk_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_CLK_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_CLK_CTRLr, &read_val, DISABLE_TX_CLKf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_CLK_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_disable_tx_clk_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_CLK_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_CLK_CTRLr, read_val, DISABLE_TX_CLKf);

    SOC_FUNC_RETURN;
}

/*
 * Programming the cip_clk selection for a port. You could choose cip
 * generated from PLL0 or PLL1
 */
int cprif_drv_cip_clk_pll_select_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_CIP_CLK_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_CIP_CLK_CTRLr, &read_val,
                      USE_PMD_CIP_CLK_PLL1f,val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_CIP_CLK_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_cip_clk_pll_select_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_CIP_CLK_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_CIP_CLK_CTRLr, read_val,
                             USE_PMD_CIP_CLK_PLL1f);

    SOC_FUNC_RETURN;
}

int cprif_drv_cip_clk_div_ctrl_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_CIP_CLK_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_CIP_CLK_CTRLr, &read_val,
                      CIP_CLK_DIV_CTRLf,val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_CIP_CLK_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_cip_clk_div_ctrl_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_CIP_CLK_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_CIP_CLK_CTRLr, read_val,
                             CIP_CLK_DIV_CTRLf);

    SOC_FUNC_RETURN;
}

int cprif_drv_tx_flush_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_TX_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_TX_CTRLr, &read_val, TX_FLUSHf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TX_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_tx_flush_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_TX_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_TX_CTRLr, read_val, TX_FLUSHf);

    SOC_FUNC_RETURN;
}

int cprif_drv_rx_disable_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RX_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RX_CTRLr, &read_val, RX_DISABLEf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RX_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_rx_disable_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RX_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPRI_RX_CTRLr, read_val, RX_DISABLEf);

    SOC_FUNC_RETURN;
}


int cprif_drv_cpri_or_enet_port_set(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    read_val = 0;
    SOC_IF_ERROR_RETURN(READ_CPMPORT_TOP_LANE_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPMPORT_TOP_LANE_CTRLr, &read_val,
                      LOGICAL_LANE_OWNERf, val);
    SOC_IF_ERROR_RETURN(WRITE_CPMPORT_TOP_LANE_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_or_enet_port_get(int unit, int port, int* val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPMPORT_TOP_LANE_CTRLr(unit, port, &read_val));
    *val = soc_reg_field_get(unit, CPMPORT_TOP_LANE_CTRLr, read_val,
                             LOGICAL_LANE_OWNERf);

    SOC_FUNC_RETURN;
}

int cprif_drv_cip_top_ctrl_set(int unit, int port, int val)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_TOP_CTRL_CONFIGr(unit, port, &data));
    soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &data, CIP_TOP_CTRLf, val);
    SOC_IF_ERROR_RETURN(WRITE_TOP_CTRL_CONFIGr(unit, port, data));

    SOC_FUNC_RETURN;
}

int cprif_drv_cip_top_ctrl_get(int unit, int port, int *val)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_TOP_CTRL_CONFIGr(unit, port, &data));
    *val = soc_reg_field_get(unit, TOP_CTRL_CONFIGr, data, CIP_TOP_CTRLf);

    SOC_FUNC_RETURN;
}

int cprif_drv_pmd_iddq_set(int unit, int port, int val)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_TOP_CTRL_CONFIGr(unit, port, &data));
    soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &data, PMD_IDDQf, val);
    SOC_IF_ERROR_RETURN(WRITE_TOP_CTRL_CONFIGr(unit, port, data));

    SOC_FUNC_RETURN;
}

int cprif_drv_pmd_iddq_get(int unit, int port, int *val)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_TOP_CTRL_CONFIGr(unit, port, &data));
    *val = soc_reg_field_get(unit, TOP_CTRL_CONFIGr, data, PMD_IDDQf);

    SOC_FUNC_RETURN;
}

int cprif_drv_top_ctrl_config_pll_ctrl_set(int unit, int port, int pll_num, int cip_pll_ctrl)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_TOP_CTRL_CONFIGr(unit, port, &data));
    if (pll_num == 0) {
        soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &data, CIP_PLL0_CTRLf, cip_pll_ctrl);
    }
    if (pll_num == 1) {
        soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &data, CIP_PLL1_CTRLf, cip_pll_ctrl);
    }
    SOC_IF_ERROR_RETURN(WRITE_TOP_CTRL_CONFIGr(unit, port, data));

    SOC_FUNC_RETURN;
}

int cprif_drv_top_ctrl_config_pll_ctrl_get(int unit, int port, int pll_num, int *cip_pll_ctrl)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_TOP_CTRL_CONFIGr(unit, port, &data));
    if (pll_num == 0) {
        *cip_pll_ctrl = soc_reg_field_get(unit, TOP_CTRL_CONFIGr, data, CIP_PLL0_CTRLf);
    }
    if (pll_num == 1) {
        *cip_pll_ctrl = soc_reg_field_get(unit, TOP_CTRL_CONFIGr, data, CIP_PLL1_CTRLf);
    }

    SOC_FUNC_RETURN;
}

int cprif_drv_top_ctrl_config_ref_clk_ctrl_set(int unit, int port, int pll_num, int refin, int refout)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_TOP_CTRL_CONFIGr(unit, port, &data));
    if (pll_num == 0) {
        soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &data, PLL0_REFIN_ENf, refin);
        soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &data, PLL0_REFOUT_ENf, refout);
    }
    if (pll_num == 1) {
        soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &data, PLL1_REFIN_ENf, refin);
        soc_reg_field_set(unit, TOP_CTRL_CONFIGr, &data, PLL1_REFOUT_ENf, refout);
    }
    SOC_IF_ERROR_RETURN(WRITE_TOP_CTRL_CONFIGr(unit, port, data));

    SOC_FUNC_RETURN;
}

int cprif_drv_top_ctrl_config_ref_clk_ctrl_get(int unit, int port, int pll_num, int *refin, int *refout)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_TOP_CTRL_CONFIGr(unit, port, &data));
    if (pll_num == 0) {
        *refin  = soc_reg_field_get(unit, TOP_CTRL_CONFIGr, data, PLL0_REFIN_ENf);
        *refout = soc_reg_field_get(unit, TOP_CTRL_CONFIGr, data, PLL0_REFOUT_ENf);
    }
    if (pll_num == 1) {
        *refin  = soc_reg_field_get(unit, TOP_CTRL_CONFIGr, data, PLL1_REFIN_ENf);
        *refout = soc_reg_field_get(unit, TOP_CTRL_CONFIGr, data, PLL1_REFOUT_ENf);
    }

    SOC_FUNC_RETURN;
}

int cprif_drv_pmd_ctrl_ln_por_h_rstb_set(int unit, int port)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_PMD_TX_RST_CTRLr(unit, port, &data));
    soc_reg_field_set(unit, CPRI_PMD_TX_RST_CTRLr, &data, PMD_LN_TX_DP_H_RSTBf, 0);
    soc_reg_field_set(unit, CPRI_PMD_TX_RST_CTRLr, &data, PMD_LN_TX_H_RSTBf, 0);
    soc_reg_field_set(unit, CPRI_PMD_TX_RST_CTRLr, &data, PMD_LN_TX_H_PWRDNf, 0);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_PMD_TX_RST_CTRLr(unit, port, data));

    SOC_IF_ERROR_RETURN(READ_CPRI_PMD_RX_RST_CTRLr(unit, port, &data));
    soc_reg_field_set(unit, CPRI_PMD_RX_RST_CTRLr, &data, PMD_LN_RX_DP_H_RSTBf, 0);
    soc_reg_field_set(unit, CPRI_PMD_RX_RST_CTRLr, &data, PMD_LN_RX_H_RSTBf, 0);
    soc_reg_field_set(unit, CPRI_PMD_RX_RST_CTRLr, &data, PMD_LN_RX_H_PWRDNf, 0);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_PMD_RX_RST_CTRLr(unit, port, data));

    SOC_IF_ERROR_RETURN(READ_CPRI_PMD_TX_RST_CTRLr(unit, port, &data));
    soc_reg_field_set(unit, CPRI_PMD_TX_RST_CTRLr, &data, PMD_LN_TX_DP_H_RSTBf, 1);
    soc_reg_field_set(unit, CPRI_PMD_TX_RST_CTRLr, &data, PMD_LN_TX_H_RSTBf, 1);
    soc_reg_field_set(unit, CPRI_PMD_TX_RST_CTRLr, &data, PMD_LN_TX_H_PWRDNf, 0);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_PMD_TX_RST_CTRLr(unit, port, data));

    SOC_IF_ERROR_RETURN(READ_CPRI_PMD_RX_RST_CTRLr(unit, port, &data));
    soc_reg_field_set(unit, CPRI_PMD_RX_RST_CTRLr, &data, PMD_LN_RX_DP_H_RSTBf, 1);
    soc_reg_field_set(unit, CPRI_PMD_RX_RST_CTRLr, &data, PMD_LN_RX_H_RSTBf, 1);
    soc_reg_field_set(unit, CPRI_PMD_RX_RST_CTRLr, &data, PMD_LN_RX_H_PWRDNf, 0);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_PMD_RX_RST_CTRLr(unit, port, data));
    SOC_FUNC_RETURN;
}

int cprif_drv_pmd_ctrl_por_h_rstb_set(int unit, int port, int por_h_rstb)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPMPORT_PMD_CTRLr(unit, port, &data));
    soc_reg_field_set(unit, CPMPORT_PMD_CTRLr, &data, PMD_POR_H_RSTBf, por_h_rstb);
    SOC_IF_ERROR_RETURN(WRITE_CPMPORT_PMD_CTRLr(unit, port, data));

    SOC_FUNC_RETURN;
}

int cprif_drv_pmd_ctrl_por_h_rstb_get(int unit, int port, int *por_h_rstb)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPMPORT_PMD_CTRLr(unit, port, &data));
    *por_h_rstb = soc_reg_field_get(unit, CPMPORT_PMD_CTRLr, data, PMD_POR_H_RSTBf);

    SOC_FUNC_RETURN;
}

int cprif_drv_pmd_core_datapath_hard_reset_set(int unit, int port, int pll_num, int rst)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPMPORT_PMD_CTRLr(unit, port, &data));
    if (pll_num==0) {
        soc_reg_field_set(unit, CPMPORT_PMD_CTRLr, &data, PMD_CORE_PLL0_DP_H_RSTBf, rst);
    }
    if (pll_num==1) {
        soc_reg_field_set(unit, CPMPORT_PMD_CTRLr, &data, PMD_CORE_PLL1_DP_H_RSTBf, rst);
    }
    SOC_IF_ERROR_RETURN(WRITE_CPMPORT_PMD_CTRLr(unit, port, data));

    SOC_FUNC_RETURN;
}

int cprif_drv_pmd_core_datapath_hard_reset_get(int unit, int port, int pll_num, int *rst)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPMPORT_PMD_CTRLr(unit, port, &data));
    if (pll_num==0) {
        *rst = soc_reg_field_get(unit, CPMPORT_PMD_CTRLr, data, PMD_CORE_PLL0_DP_H_RSTBf);
    }
    if (pll_num==1) {
        *rst = soc_reg_field_get(unit, CPMPORT_PMD_CTRLr, data, PMD_CORE_PLL1_DP_H_RSTBf);
    }

    SOC_FUNC_RETURN;
}

int cprif_drv_cpmport_pll_ctrl_config_set(int unit, int port, int pll_num, int lcref_sel)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    if (pll_num == 0) {
        SOC_IF_ERROR_RETURN(READ_CPMPORT_PLL0_CTRL_CONFIGr(unit, port, &data));
        soc_reg_field_set(unit, CPMPORT_PLL0_CTRL_CONFIGr, &data, LCREF_SELf, lcref_sel);
        SOC_IF_ERROR_RETURN(WRITE_CPMPORT_PLL0_CTRL_CONFIGr(unit, port, data));
    }
    if (pll_num == 1) {
        SOC_IF_ERROR_RETURN(READ_CPMPORT_PLL1_CTRL_CONFIGr(unit, port, &data));
        soc_reg_field_set(unit, CPMPORT_PLL1_CTRL_CONFIGr, &data, LCREF_SELf, lcref_sel);
        SOC_IF_ERROR_RETURN(WRITE_CPMPORT_PLL1_CTRL_CONFIGr(unit, port, data));
    }

    SOC_FUNC_RETURN;
}

int cprif_drv_cpmport_pll_ctrl_config_get(int unit, int port, int pll_num, int *lcref_sel)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    if (pll_num == 0) {
        SOC_IF_ERROR_RETURN(READ_CPMPORT_PLL0_CTRL_CONFIGr(unit, port, &data));
        *lcref_sel = soc_reg_field_get(unit, CPMPORT_PLL0_CTRL_CONFIGr, data, LCREF_SELf);
    }
    if (pll_num == 1) {
        SOC_IF_ERROR_RETURN(READ_CPMPORT_PLL1_CTRL_CONFIGr(unit, port, &data));
        *lcref_sel = soc_reg_field_get(unit, CPMPORT_PLL1_CTRL_CONFIGr, data, LCREF_SELf);
    }

    SOC_FUNC_RETURN;
}

int cprif_drv_lane_swap_set(int unit, int port, int lane, cprimod_direction_t dir,
                            int phyport)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    data = 0;
    if (dir == CPRIMOD_DIR_RX) {
        switch (lane) {
        case 0:
            soc_reg_field_set(unit, CPMPORT_TOP_RX_PHY_LANE_CTRL_0r, &data,
                              RX_LOG_PHY_MAPf, phyport);
            SOC_IF_ERROR_RETURN(WRITE_CPMPORT_TOP_RX_PHY_LANE_CTRL_0r(unit, port, data));
            break;
        case 1:
            soc_reg_field_set(unit, CPMPORT_TOP_RX_PHY_LANE_CTRL_1r, &data,
                              RX_LOG_PHY_MAPf, phyport);
            SOC_IF_ERROR_RETURN(WRITE_CPMPORT_TOP_RX_PHY_LANE_CTRL_1r(unit, port, data));
            break;
        case 2:
            soc_reg_field_set(unit, CPMPORT_TOP_RX_PHY_LANE_CTRL_2r, &data,
                              RX_LOG_PHY_MAPf, phyport);
            SOC_IF_ERROR_RETURN(WRITE_CPMPORT_TOP_RX_PHY_LANE_CTRL_2r(unit, port, data));
            break;
        case 3:
            soc_reg_field_set(unit, CPMPORT_TOP_RX_PHY_LANE_CTRL_3r, &data,
                              RX_LOG_PHY_MAPf, phyport);
            SOC_IF_ERROR_RETURN(WRITE_CPMPORT_TOP_RX_PHY_LANE_CTRL_3r(unit, port, data));
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Wrong logical port number"));
            break;
        }
    } else {
        switch (lane) {
        case 0:
            soc_reg_field_set(unit, CPMPORT_TOP_TX_PHY_LANE_CTRL_0r, &data,
                              TX_LOG_PHY_MAPf, phyport);
            SOC_IF_ERROR_RETURN(WRITE_CPMPORT_TOP_TX_PHY_LANE_CTRL_0r(unit, port, data));
            break;
        case 1:
            soc_reg_field_set(unit, CPMPORT_TOP_TX_PHY_LANE_CTRL_1r, &data,
                              TX_LOG_PHY_MAPf, phyport);
            SOC_IF_ERROR_RETURN(WRITE_CPMPORT_TOP_TX_PHY_LANE_CTRL_1r(unit, port, data));
            break;
        case 2:
            soc_reg_field_set(unit, CPMPORT_TOP_TX_PHY_LANE_CTRL_2r, &data,
                              TX_LOG_PHY_MAPf, phyport);
            SOC_IF_ERROR_RETURN(WRITE_CPMPORT_TOP_TX_PHY_LANE_CTRL_2r(unit, port, data));
            break;
        case 3:
            soc_reg_field_set(unit, CPMPORT_TOP_TX_PHY_LANE_CTRL_3r, &data,
                              TX_LOG_PHY_MAPf, phyport);
            SOC_IF_ERROR_RETURN(WRITE_CPMPORT_TOP_TX_PHY_LANE_CTRL_3r(unit, port, data));
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Wrong logical port number"));
            break;
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_lane_swap_get(int unit, int port, int lane, cprimod_direction_t dir,
                            int *phyport)
{
    uint32 data;
    SOC_INIT_FUNC_DEFS;

    if (dir == CPRIMOD_DIR_RX) {
        switch (lane) {
        case 0:
            SOC_IF_ERROR_RETURN(READ_CPMPORT_TOP_RX_PHY_LANE_CTRL_0r(unit, port, &data));
            *phyport = soc_reg_field_get(unit, CPMPORT_TOP_RX_PHY_LANE_CTRL_0r,
                                         data, RX_LOG_PHY_MAPf);
            break;
        case 1:
            SOC_IF_ERROR_RETURN(READ_CPMPORT_TOP_RX_PHY_LANE_CTRL_1r(unit, port, &data));
            *phyport = soc_reg_field_get(unit, CPMPORT_TOP_RX_PHY_LANE_CTRL_1r,
                                         data, RX_LOG_PHY_MAPf);
            break;
        case 2:
            SOC_IF_ERROR_RETURN(READ_CPMPORT_TOP_RX_PHY_LANE_CTRL_2r(unit, port, &data));
            *phyport = soc_reg_field_get(unit, CPMPORT_TOP_RX_PHY_LANE_CTRL_2r,
                                         data, RX_LOG_PHY_MAPf);
            break;
        case 3:
            SOC_IF_ERROR_RETURN(READ_CPMPORT_TOP_RX_PHY_LANE_CTRL_3r(unit, port, &data));
            *phyport = soc_reg_field_get(unit, CPMPORT_TOP_RX_PHY_LANE_CTRL_3r,
                                         data, RX_LOG_PHY_MAPf);
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Wrong logical port number"));
            break;
        }
    } else {
        switch (lane) {
        case 0:
            SOC_IF_ERROR_RETURN(READ_CPMPORT_TOP_TX_PHY_LANE_CTRL_0r(unit, port, &data));
            *phyport = soc_reg_field_get(unit, CPMPORT_TOP_TX_PHY_LANE_CTRL_0r,
                                         data, TX_LOG_PHY_MAPf);
            break;
        case 1:
            SOC_IF_ERROR_RETURN(READ_CPMPORT_TOP_TX_PHY_LANE_CTRL_1r(unit, port, &data));
            *phyport = soc_reg_field_get(unit, CPMPORT_TOP_TX_PHY_LANE_CTRL_1r,
                                         data, TX_LOG_PHY_MAPf);
            break;
        case 2:
            SOC_IF_ERROR_RETURN(READ_CPMPORT_TOP_TX_PHY_LANE_CTRL_2r(unit, port, &data));
            *phyport = soc_reg_field_get(unit, CPMPORT_TOP_TX_PHY_LANE_CTRL_2r,
                                         data, TX_LOG_PHY_MAPf);
            break;
        case 3:
            SOC_IF_ERROR_RETURN(READ_CPMPORT_TOP_TX_PHY_LANE_CTRL_3r(unit, port, &data));
            *phyport = soc_reg_field_get(unit, CPMPORT_TOP_TX_PHY_LANE_CTRL_3r,
                                         data, TX_LOG_PHY_MAPf);
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Wrong logical port number"));
            break;
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_rx_master_frame_sync_config_set(int unit, int port,
                                                    uint32  master_frame_number,
                                                    uint64  master_frame_start_time)
{
    uint64 reg_val;
    uint64 field_buf;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_SET(field_buf,0,master_frame_number);
    _SOC_IF_ERR_EXIT
        (READ_BRCM_RESERVED_CPM_317r(unit, port, &reg_val));
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_317r, &reg_val, BRCM_RESERVED_CPM_317_0f, field_buf);

    COMPILER_64_COPY(field_buf,master_frame_start_time);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_317r, &reg_val, BRCM_RESERVED_CPM_317_1f, field_buf);

    _SOC_IF_ERR_EXIT
        (WRITE_BRCM_RESERVED_CPM_317r(unit, port, reg_val));

exit:

    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_rx_master_frame_sync_config_get(int unit, int port,
                                                    uint32*  master_frame_number,
                                                    uint64*  master_frame_start_time)
{
    uint64 reg_val;
    uint64 field_buf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (READ_BRCM_RESERVED_CPM_317r(unit, port, &reg_val));

    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_317r, reg_val, BRCM_RESERVED_CPM_317_1f);
    COMPILER_64_TO_32_LO(*master_frame_number,field_buf);

    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_317r, reg_val, BRCM_RESERVED_CPM_317_0f);
    COMPILER_64_COPY(*master_frame_start_time,field_buf);

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_control_word_status_get(int unit, int port, cprif_drv_rx_control_word_status_t* info)
{
    uint64 reg_val;
    uint64 field_buf;

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_CPRSR_STATUSr(unit, port, &reg_val));

    field_buf           = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_STATUSr, reg_val, RXFRM_HFNf);
    COMPILER_64_TO_32_LO(info->hyper_frame,field_buf);

    field_buf           = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_STATUSr, reg_val, RXFRM_BFNf);
    COMPILER_64_TO_32_LO(info->radio_frame,field_buf);

    field_buf           = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_STATUSr, reg_val, RXFRM_FNf);
    COMPILER_64_TO_32_LO(info->basic_frame,field_buf);

    field_buf           = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_STATUSr, reg_val, RXFRM_HDLC_RATEf);
    COMPILER_64_TO_32_LO(info->hdlc_rate,field_buf);

    field_buf           = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_STATUSr, reg_val, RXFRM_PROT_VERf);
    COMPILER_64_TO_32_LO(info->protocol_ver,field_buf);

    field_buf           = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_STATUSr, reg_val, RXFRM_ETH_PTRf);
    COMPILER_64_TO_32_LO(info->eth_pointer,field_buf);

    field_buf           = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_STATUSr, reg_val, RXFRM_L1_RESETf);
    COMPILER_64_TO_32_LO(info->reset,field_buf);

    field_buf           = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_STATUSr, reg_val, RXFRM_L1_RAIf);
    COMPILER_64_TO_32_LO(info->l1_rai,field_buf);

    field_buf           = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_STATUSr, reg_val, RXFRM_L1_SDIf);
    COMPILER_64_TO_32_LO(info->l1_sdi,field_buf);

    field_buf           = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_STATUSr, reg_val, RXFRM_L1_LOSf);
    COMPILER_64_TO_32_LO(info->l1_los,field_buf);

    field_buf           = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_STATUSr, reg_val, RXFRM_L1_LOFf);
    COMPILER_64_TO_32_LO(info->l1_lof,field_buf);

    return SOC_E_NONE;
}

int cprif_drv_cpri_rx_l1_signal_signal_protection_set(int unit, int port, uint32 signal_map, uint32 enable)
{
    uint64 reg_val, tmp_en;

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_CPRSR_CTRLr(unit, port, &reg_val));

    if (enable != 0) {
        COMPILER_64_SET(tmp_en, 0, 1);
    } else {
        COMPILER_64_SET(tmp_en, 0, 0);
    }

    if (CPRIMOD_L1_SIGNAL_PROT_MAP_LOF_GET(signal_map)) {
        soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &reg_val, RXFRM_HW_SIG_PROTECT_LOFf, tmp_en );
    }

    if (CPRIMOD_L1_SIGNAL_PROT_MAP_LOS_GET(signal_map)) {
        soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &reg_val, RXFRM_HW_SIG_PROTECT_LOSf, tmp_en );
    }

    if (CPRIMOD_L1_SIGNAL_PROT_MAP_SDI_GET(signal_map)) {
        soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &reg_val, RXFRM_HW_SIG_PROTECT_SDIf, tmp_en );
    }

    if (CPRIMOD_L1_SIGNAL_PROT_MAP_RAI_GET(signal_map)) {
        soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &reg_val, RXFRM_HW_SIG_PROTECT_RAIf, tmp_en );
    }

    if (CPRIMOD_L1_SIGNAL_PROT_MAP_RESET_GET(signal_map)) {
        soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &reg_val, RXFRM_HW_SIG_PROTECT_RESETf, tmp_en );
    }

    if (CPRIMOD_L1_SIGNAL_PROT_MAP_ETH_PTR_GET(signal_map)) {
        soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &reg_val, RXFRM_HW_SIG_PROTECT_ETH_PTRf, tmp_en );
    }

    if (CPRIMOD_L1_SIGNAL_PROT_MAP_HDLC_RATE_GET(signal_map)) {
        soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &reg_val, RXFRM_HW_SIG_PROTECT_HDLC_RATEf, tmp_en );
    }

    if (CPRIMOD_L1_SIGNAL_PROT_MAP_PROT_VER_GET(signal_map)) {
        soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &reg_val, RXFRM_HW_SIG_PROTECT_PROT_VERf, tmp_en );
    }

    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_RXFRM_CPRSR_CTRLr(unit, port, reg_val));

    return SOC_E_NONE;
}

int cprif_drv_cpri_rx_l1_signal_signal_protection_get(int unit, int port, uint32 signal_map, uint32* enable)
{
    uint64 reg_val, ret_val64;

    *enable = 0;
    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_CPRSR_CTRLr(unit, port, &reg_val));

    if (CPRIMOD_L1_SIGNAL_PROT_MAP_LOF_GET(signal_map)) {
        ret_val64 = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_CTRLr, reg_val, RXFRM_HW_SIG_PROTECT_LOFf);
        if (!COMPILER_64_IS_ZERO(ret_val64) ) {
            CPRIMOD_L1_SIGNAL_PROT_MAP_LOF_SET(*enable);
        }
    }

    if (CPRIMOD_L1_SIGNAL_PROT_MAP_LOS_GET(signal_map)) {
        ret_val64 = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_CTRLr, reg_val, RXFRM_HW_SIG_PROTECT_LOSf);
        if (!COMPILER_64_IS_ZERO(ret_val64)) {
            CPRIMOD_L1_SIGNAL_PROT_MAP_LOS_SET(*enable);
        }
    }

    if (CPRIMOD_L1_SIGNAL_PROT_MAP_SDI_GET(signal_map)) {
        ret_val64 = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_CTRLr, reg_val, RXFRM_HW_SIG_PROTECT_SDIf);
        if (!COMPILER_64_IS_ZERO(ret_val64)) {
            CPRIMOD_L1_SIGNAL_PROT_MAP_SDI_SET(*enable);
        }
    }

    if (CPRIMOD_L1_SIGNAL_PROT_MAP_RAI_GET(signal_map)) {
        ret_val64 = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_CTRLr, reg_val, RXFRM_HW_SIG_PROTECT_RAIf);
        if (!COMPILER_64_IS_ZERO(ret_val64)) {
            CPRIMOD_L1_SIGNAL_PROT_MAP_RAI_SET(*enable);
        }
    }

    if (CPRIMOD_L1_SIGNAL_PROT_MAP_RESET_GET(signal_map)) {
        ret_val64 = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_CTRLr, reg_val, RXFRM_HW_SIG_PROTECT_RESETf);
        if (!COMPILER_64_IS_ZERO(ret_val64)) {
            CPRIMOD_L1_SIGNAL_PROT_MAP_RESET_SET(*enable);
        }
    }

    if (CPRIMOD_L1_SIGNAL_PROT_MAP_ETH_PTR_GET(signal_map)) {
        ret_val64 = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_CTRLr, reg_val, RXFRM_HW_SIG_PROTECT_ETH_PTRf);
        if (!COMPILER_64_IS_ZERO(ret_val64)) {
            CPRIMOD_L1_SIGNAL_PROT_MAP_ETH_PTR_SET(*enable);
        }
    }

    if (CPRIMOD_L1_SIGNAL_PROT_MAP_HDLC_RATE_GET(signal_map)) {
        ret_val64 = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_CTRLr, reg_val, RXFRM_HW_SIG_PROTECT_HDLC_RATEf);
        if (!COMPILER_64_IS_ZERO(ret_val64)) {
            CPRIMOD_L1_SIGNAL_PROT_MAP_HDLC_RATE_SET(*enable);
        }
    }

    if (CPRIMOD_L1_SIGNAL_PROT_MAP_PROT_VER_GET(signal_map)) {
        ret_val64 = soc_reg64_field_get(unit, CPRI_RXFRM_CPRSR_CTRLr, reg_val, RXFRM_HW_SIG_PROTECT_PROT_VERf);
        if (!COMPILER_64_IS_ZERO(ret_val64)) {
            CPRIMOD_L1_SIGNAL_PROT_MAP_PROT_VER_SET(*enable);
        }
    }

    return SOC_E_NONE;
}

int cprif_drv_cpri_port_rx_cw_slow_hdlc_config_set(int unit, int port, cprif_cpri_rx_hdlc_config_t* config_info)
{
    uint64 reg_val, val64;

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_CPRSR_CTRLr(unit, port, &reg_val));
    /*
     *Need to pass uint64 to soc_reg64_field_set;
     *Use COMPILER_64_SET to create one
     */
    COMPILER_64_SET(val64, 0, config_info->cw_size);
    soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &reg_val, RXFRM_HDLC_CW_SIZEf, val64);

    COMPILER_64_SET(val64, 0, config_info->cw_sel);
    soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &reg_val, RXFRM_HDLC_CW_SELf, val64);

    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_RXFRM_CPRSR_CTRLr(unit, port, reg_val));

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RX_HDLC_CTRLr(unit, port, &reg_val));

    COMPILER_64_SET(val64, 0, config_info->ignore_fcs_err);
    soc_reg64_field_set(unit, CPRI_RX_HDLC_CTRLr, &reg_val, RX_HDLC_IGNORE_FCS_ERRf, val64);
    COMPILER_64_SET(val64, 0, config_info->fcs_size);
    soc_reg64_field_set(unit, CPRI_RX_HDLC_CTRLr, &reg_val, RX_HDLC_FCS_SIZEf, val64);
    COMPILER_64_SET(val64, 0, config_info->runt_frame_drop);
    soc_reg64_field_set(unit, CPRI_RX_HDLC_CTRLr, &reg_val, RX_HDLC_RUNT_FRAME_DROPf, val64);
    COMPILER_64_SET(val64, 0, config_info->min_frame_size);
    soc_reg64_field_set(unit, CPRI_RX_HDLC_CTRLr, &reg_val, RX_HDLC_MIN_FRAME_SIZEf, val64);
    COMPILER_64_SET(val64, 0, config_info->long_frame_drop);
    soc_reg64_field_set(unit, CPRI_RX_HDLC_CTRLr, &reg_val, RX_HDLC_LONG_FRAME_DROPf, val64);
    COMPILER_64_SET(val64, 0, config_info->max_frame_size);
    soc_reg64_field_set(unit, CPRI_RX_HDLC_CTRLr, &reg_val, RX_HDLC_MAX_FRAME_SIZEf, val64);
    COMPILER_64_SET(val64, 0, config_info->use_fe_mac);
    soc_reg64_field_set(unit, CPRI_RX_HDLC_CTRLr, &reg_val, RX_HDLC_USE_FE_MACf, val64);
    COMPILER_64_SET(val64, 0, config_info->queue_num);
    soc_reg64_field_set(unit, CPRI_RX_HDLC_CTRLr, &reg_val, RX_HDLC_QUEUE_NUMf, val64);
    COMPILER_64_SET(val64, 0, config_info->crc_byte_swap);
    soc_reg64_field_set(unit, CPRI_RX_HDLC_CTRLr, &reg_val, RX_HDLC_CRC_BYTE_ORDERf, val64);
    COMPILER_64_SET(val64, 0, config_info->crc_init_val);
    soc_reg64_field_set(unit, CPRI_RX_HDLC_CTRLr, &reg_val, RX_HDLC_CRC_INIT_VALf, val64);

    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_RX_HDLC_CTRLr(unit, port, reg_val));
    return SOC_E_NONE;
}

int cprif_drv_cpri_port_rx_cw_slow_hdlc_config_get(int unit, int port, cprif_cpri_rx_hdlc_config_t* config_info)
{
    uint64 reg_val;

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_CPRSR_CTRLr(unit, port, &reg_val));

    config_info->cw_size = soc_reg64_field32_get(unit, CPRI_RXFRM_CPRSR_CTRLr, reg_val, RXFRM_HDLC_CW_SIZEf);
    config_info->cw_sel  = soc_reg64_field32_get(unit, CPRI_RXFRM_CPRSR_CTRLr, reg_val, RXFRM_HDLC_CW_SELf);

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RX_HDLC_CTRLr(unit, port, &reg_val));

    config_info->ignore_fcs_err     = soc_reg64_field32_get(unit, CPRI_RX_HDLC_CTRLr, reg_val, RX_HDLC_IGNORE_FCS_ERRf);
    config_info->fcs_size           = soc_reg64_field32_get(unit, CPRI_RX_HDLC_CTRLr, reg_val, RX_HDLC_FCS_SIZEf);
    config_info->runt_frame_drop    = soc_reg64_field32_get(unit, CPRI_RX_HDLC_CTRLr, reg_val, RX_HDLC_RUNT_FRAME_DROPf);
    config_info->min_frame_size     = soc_reg64_field32_get(unit, CPRI_RX_HDLC_CTRLr, reg_val, RX_HDLC_MIN_FRAME_SIZEf);
    config_info->long_frame_drop    = soc_reg64_field32_get(unit, CPRI_RX_HDLC_CTRLr, reg_val, RX_HDLC_LONG_FRAME_DROPf);
    config_info->max_frame_size     = soc_reg64_field32_get(unit, CPRI_RX_HDLC_CTRLr, reg_val, RX_HDLC_MAX_FRAME_SIZEf);
    config_info->use_fe_mac         = soc_reg64_field32_get(unit, CPRI_RX_HDLC_CTRLr, reg_val, RX_HDLC_USE_FE_MACf);
    config_info->queue_num          = soc_reg64_field32_get(unit, CPRI_RX_HDLC_CTRLr, reg_val, RX_HDLC_QUEUE_NUMf);
    config_info->crc_byte_swap      = soc_reg64_field32_get(unit, CPRI_RX_HDLC_CTRLr, reg_val, RX_HDLC_CRC_BYTE_ORDERf);
    config_info->crc_init_val       = soc_reg64_field32_get(unit, CPRI_RX_HDLC_CTRLr, reg_val, RX_HDLC_CRC_INIT_VALf);

    return SOC_E_NONE;
}
int cprif_drv_cpri_port_tx_cw_slow_hdlc_config_set(int unit, int port, cprif_cpri_tx_hdlc_config_t* config_info)
{
    uint32 reg_val;

    SOC_IF_ERROR_RETURN
        (READ_CPRI_TX_HDLC_CTRLr(unit, port, &reg_val));

    soc_reg_field_set(unit, CPRI_TX_HDLC_CTRLr, &reg_val, TX_HDLC_CRC_MODEf, config_info->crc_mode);
    soc_reg_field_set(unit, CPRI_TX_HDLC_CTRLr, &reg_val, TX_HDLC_CW_SELf,   config_info->cw_sel);
    soc_reg_field_set(unit, CPRI_TX_HDLC_CTRLr, &reg_val, TX_HDLC_CW_SIZEf,  config_info->cw_size);
    soc_reg_field_set(unit, CPRI_TX_HDLC_CTRLr, &reg_val, TX_HDLC_FCS_SIZEf, config_info->fcs_size);
    soc_reg_field_set(unit, CPRI_TX_HDLC_CTRLr, &reg_val, TX_HDLC_MIN_FLAGf, config_info->flag_size);
    soc_reg_field_set(unit, CPRI_TX_HDLC_CTRLr, &reg_val, TX_HDLC_USE_FE_MACf, config_info->use_fe_mac);
    soc_reg_field_set(unit, CPRI_TX_HDLC_CTRLr, &reg_val, TX_HDLC_QUEUE_NUMf, config_info->queue_num);
    soc_reg_field_set(unit, CPRI_TX_HDLC_CTRLr, &reg_val, TX_HDLC_CRC_BYTE_ORDERf, config_info->crc_byte_swap);
    soc_reg_field_set(unit, CPRI_TX_HDLC_CTRLr, &reg_val, TX_HDLC_CRC_INIT_VALf, config_info->crc_init_val);
    soc_reg_field_set(unit, CPRI_TX_HDLC_CTRLr, &reg_val, TX_HDLC_FILLING_FLAG_SELf, config_info->filling_flag_pattern);

    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_TX_HDLC_CTRLr(unit, port, reg_val));
    return SOC_E_NONE;

}

int cprif_drv_cpri_port_tx_cw_slow_hdlc_config_get(int unit, int port, cprif_cpri_tx_hdlc_config_t* config_info)
{
    uint32 reg_val;

    SOC_IF_ERROR_RETURN
        (READ_CPRI_TX_HDLC_CTRLr(unit, port, &reg_val));

    config_info->crc_mode       = soc_reg_field_get(unit, CPRI_TX_HDLC_CTRLr, reg_val, TX_HDLC_CRC_MODEf);
    config_info->cw_sel         = soc_reg_field_get(unit, CPRI_TX_HDLC_CTRLr, reg_val, TX_HDLC_CW_SELf);
    config_info->cw_size        = soc_reg_field_get(unit, CPRI_TX_HDLC_CTRLr, reg_val, TX_HDLC_CW_SIZEf);
    config_info->fcs_size       = soc_reg_field_get(unit, CPRI_TX_HDLC_CTRLr, reg_val, TX_HDLC_FCS_SIZEf);
    config_info->flag_size      = soc_reg_field_get(unit, CPRI_TX_HDLC_CTRLr, reg_val, TX_HDLC_MIN_FLAGf);
    config_info->use_fe_mac     = soc_reg_field_get(unit, CPRI_TX_HDLC_CTRLr, reg_val, TX_HDLC_USE_FE_MACf);
    config_info->queue_num      = soc_reg_field_get(unit, CPRI_TX_HDLC_CTRLr, reg_val, TX_HDLC_QUEUE_NUMf);
    config_info->crc_byte_swap  = soc_reg_field_get(unit, CPRI_TX_HDLC_CTRLr, reg_val, TX_HDLC_CRC_BYTE_ORDERf);
    config_info->crc_init_val   = soc_reg_field_get(unit, CPRI_TX_HDLC_CTRLr, reg_val, TX_HDLC_CRC_INIT_VALf);
    config_info->filling_flag_pattern   = soc_reg_field_get(unit, CPRI_TX_HDLC_CTRLr, reg_val, TX_HDLC_FILLING_FLAG_SELf);

    return SOC_E_NONE;

}
int cprif_drv_cpri_port_rx_cw_fast_eth_word_config_set(int unit, int port, cprif_cpri_rx_fast_eth_word_config_t* config_info)
{
    uint64 rxfrm_cprsr, val64;

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_CPRSR_CTRLr(unit, port, &rxfrm_cprsr));

    COMPILER_64_SET(val64, 0,  config_info->sub_channel_start);
    soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &rxfrm_cprsr, RXFRM_ETH_SCHAN_STARTf, val64);
    COMPILER_64_SET(val64, 0, config_info->sub_channel_size);
    soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &rxfrm_cprsr, RXFRM_ETH_SCHAN_SIZEf, val64);
    COMPILER_64_SET(val64, 0, config_info->cw_sel);
    soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &rxfrm_cprsr, RXFRM_ETH_CW_SELf, val64);
    COMPILER_64_SET(val64, 0, config_info->cw_size);
    soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &rxfrm_cprsr, RXFRM_ETH_CW_SIZEf, val64);

    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_RXFRM_CPRSR_CTRLr(unit, port, rxfrm_cprsr));
    return SOC_E_NONE;
}

int cprif_drv_cpri_port_rx_cw_fast_eth_word_config_get(int unit, int port, cprif_cpri_rx_fast_eth_word_config_t* config_info)
{
    uint64 rxfrm_cprsr;

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_CPRSR_CTRLr(unit, port, &rxfrm_cprsr));

    config_info->sub_channel_start  = soc_reg64_field32_get(unit, CPRI_RXFRM_CPRSR_CTRLr, rxfrm_cprsr, RXFRM_ETH_SCHAN_STARTf);
    config_info->sub_channel_size   = soc_reg64_field32_get(unit, CPRI_RXFRM_CPRSR_CTRLr, rxfrm_cprsr, RXFRM_ETH_SCHAN_SIZEf);
    config_info->cw_sel             = soc_reg64_field32_get(unit, CPRI_RXFRM_CPRSR_CTRLr, rxfrm_cprsr, RXFRM_ETH_CW_SELf);
    config_info->cw_size            = soc_reg64_field32_get(unit, CPRI_RXFRM_CPRSR_CTRLr, rxfrm_cprsr, RXFRM_ETH_CW_SIZEf);

    return SOC_E_NONE;
}

int cprif_drv_cpri_port_rx_cw_hdlc_fast_eth_sizes_swap(int unit, int port, uint32* hdlc_size, uint32* fast_eth_size)
{
    uint64 val64;
    uint64 rxfrm_cprsr;
    uint32 old_hdlc_size;
    uint32 old_fast_eth_size;

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_CPRSR_CTRLr(unit, port, &rxfrm_cprsr));

    old_fast_eth_size   = soc_reg64_field32_get(unit, CPRI_RXFRM_CPRSR_CTRLr, rxfrm_cprsr, RXFRM_ETH_CW_SIZEf);
    old_hdlc_size       = soc_reg64_field32_get(unit, CPRI_RXFRM_CPRSR_CTRLr, rxfrm_cprsr, RXFRM_HDLC_CW_SIZEf);

    COMPILER_64_SET(val64, 0, *fast_eth_size);
    soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &rxfrm_cprsr, RXFRM_ETH_CW_SIZEf, val64);
    COMPILER_64_SET(val64, 0, *hdlc_size);
    soc_reg64_field_set(unit, CPRI_RXFRM_CPRSR_CTRLr, &rxfrm_cprsr, RXFRM_HDLC_CW_SIZEf, val64);

    *fast_eth_size  = old_fast_eth_size;
    *hdlc_size      = old_hdlc_size;

    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_RXFRM_CPRSR_CTRLr(unit, port, rxfrm_cprsr));
    return SOC_E_NONE;
}

int cprif_drv_cpri_port_rx_cw_fast_eth_config_set(int unit, int port, cprif_cpri_rx_fast_eth_config_t* config_info)
{
    uint64 rxfrm_fast_eth_ctrl, val64;

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RX_FAST_ETH_CTRLr(unit, port, &rxfrm_fast_eth_ctrl));
    COMPILER_64_SET(val64, 0, config_info->queue_num);
    soc_reg64_field_set(unit, CPRI_RX_FAST_ETH_CTRLr, &rxfrm_fast_eth_ctrl, RX_FAST_ETH_QUEUE_NUMf, val64);
    COMPILER_64_SET(val64, 0, config_info->ignore_fcs_err ? 1:0);
    soc_reg64_field_set(unit, CPRI_RX_FAST_ETH_CTRLr, &rxfrm_fast_eth_ctrl, RX_FAST_ETH_IGNORE_FCS_ERRf, val64);
    COMPILER_64_SET(val64, 0, config_info->min_packet_size);
    soc_reg64_field_set(unit, CPRI_RX_FAST_ETH_CTRLr, &rxfrm_fast_eth_ctrl, RX_FAST_ETH_MIN_PKT_SIZEf, val64);
    COMPILER_64_SET(val64, 0, config_info->min_packet_drop ? 1:0);
    soc_reg64_field_set(unit, CPRI_RX_FAST_ETH_CTRLr, &rxfrm_fast_eth_ctrl, RX_FAST_ETH_DROP_MIN_PKTf, val64);
    COMPILER_64_SET(val64, 0, config_info->max_packet_size);
    soc_reg64_field_set(unit, CPRI_RX_FAST_ETH_CTRLr, &rxfrm_fast_eth_ctrl, RX_FAST_ETH_MAX_PKT_SIZEf, val64);
    COMPILER_64_SET(val64, 0, config_info->max_packet_drop ? 1:0);
    soc_reg64_field_set(unit, CPRI_RX_FAST_ETH_CTRLr, &rxfrm_fast_eth_ctrl, RX_FAST_ETH_DROP_MAX_PKTf, val64);
    COMPILER_64_SET(val64, 0, config_info->strip_crc? 1:0);
    soc_reg64_field_set(unit, CPRI_RX_FAST_ETH_CTRLr, &rxfrm_fast_eth_ctrl, RX_FAST_ETH_STRIP_CRCf, val64);

    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_RX_FAST_ETH_CTRLr(unit, port, rxfrm_fast_eth_ctrl));
    return SOC_E_NONE;
}

int cprif_drv_cpri_port_rx_cw_fast_eth_config_get(int unit, int port, cprif_cpri_rx_fast_eth_config_t* config_info)
{
    uint64 rxfrm_fast_eth_ctrl;

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RX_FAST_ETH_CTRLr(unit, port, &rxfrm_fast_eth_ctrl));

    config_info->queue_num       = soc_reg64_field32_get(unit, CPRI_RX_FAST_ETH_CTRLr, rxfrm_fast_eth_ctrl, RX_FAST_ETH_QUEUE_NUMf);
    config_info->ignore_fcs_err  = soc_reg64_field32_get(unit, CPRI_RX_FAST_ETH_CTRLr, rxfrm_fast_eth_ctrl, RX_FAST_ETH_IGNORE_FCS_ERRf);
    config_info->min_packet_size = soc_reg64_field32_get(unit, CPRI_RX_FAST_ETH_CTRLr, rxfrm_fast_eth_ctrl, RX_FAST_ETH_MIN_PKT_SIZEf);
    config_info->min_packet_drop = soc_reg64_field32_get(unit, CPRI_RX_FAST_ETH_CTRLr, rxfrm_fast_eth_ctrl, RX_FAST_ETH_DROP_MIN_PKTf);
    config_info->max_packet_size = soc_reg64_field32_get(unit, CPRI_RX_FAST_ETH_CTRLr, rxfrm_fast_eth_ctrl, RX_FAST_ETH_MAX_PKT_SIZEf);
    config_info->max_packet_drop = soc_reg64_field32_get(unit, CPRI_RX_FAST_ETH_CTRLr, rxfrm_fast_eth_ctrl, RX_FAST_ETH_DROP_MAX_PKTf);
    config_info->strip_crc       = soc_reg64_field32_get(unit, CPRI_RX_FAST_ETH_CTRLr, rxfrm_fast_eth_ctrl, RX_FAST_ETH_STRIP_CRCf);

    return SOC_E_NONE;
}

int cprif_drv_cpri_port_tx_cw_fast_eth_config_set(int unit, int port, cprif_cpri_tx_fast_eth_config_t* config_info)
{
  uint64 txfrm_cwa, val64;

   SOC_IF_ERROR_RETURN
        (READ_CPRI_TX_FAST_ETH_CTRLr(unit, port, &txfrm_cwa));

    COMPILER_64_SET(val64, 0, config_info->queue_num);
    soc_reg64_field_set(unit, CPRI_TX_FAST_ETH_CTRLr, &txfrm_cwa, TX_FAST_ETH_QUEUE_NUMf, val64);
    COMPILER_64_SET(val64, 0, config_info->min_ipg);
    soc_reg64_field_set(unit, CPRI_TX_FAST_ETH_CTRLr, &txfrm_cwa, TX_FAST_ETH_MIN_IPGf, val64);
    COMPILER_64_SET(val64, 0, config_info->crc_mode);
    soc_reg64_field_set(unit, CPRI_TX_FAST_ETH_CTRLr, &txfrm_cwa, TX_FAST_ETH_CRC_MODEf, val64);
    COMPILER_64_SET(val64, 0, config_info->cw_sel);
    soc_reg64_field_set(unit, CPRI_TX_FAST_ETH_CTRLr, &txfrm_cwa, TX_FAST_ETH_CW_SELf, val64);
    COMPILER_64_SET(val64, 0, config_info->cw_size);
    soc_reg64_field_set(unit, CPRI_TX_FAST_ETH_CTRLr, &txfrm_cwa, TX_FAST_ETH_CW_SIZEf, val64);
    COMPILER_64_SET(val64, 0, config_info->sub_channel_size);
    soc_reg64_field_set(unit, CPRI_TX_FAST_ETH_CTRLr, &txfrm_cwa, TX_FAST_ETH_SCHAN_SIZEf, val64);
    COMPILER_64_SET(val64, 0, config_info->sub_channel_start);
    soc_reg64_field_set(unit, CPRI_TX_FAST_ETH_CTRLr, &txfrm_cwa, TX_FAST_ETH_SCHAN_STARTf, val64);
    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_TX_FAST_ETH_CTRLr(unit, port, txfrm_cwa));

    return SOC_E_NONE;
}

int cprif_drv_cpri_port_tx_cw_fast_eth_config_get(int unit, int port, cprif_cpri_tx_fast_eth_config_t* config_info)
{
  uint64 txfrm_cwa;

   SOC_IF_ERROR_RETURN
        (READ_CPRI_TX_FAST_ETH_CTRLr(unit, port, &txfrm_cwa));

    config_info->queue_num  = soc_reg64_field32_get(unit, CPRI_TX_FAST_ETH_CTRLr, txfrm_cwa, TX_FAST_ETH_QUEUE_NUMf);
    config_info->min_ipg    = soc_reg64_field32_get(unit, CPRI_TX_FAST_ETH_CTRLr, txfrm_cwa, TX_FAST_ETH_MIN_IPGf);
    config_info->crc_mode   = soc_reg64_field32_get(unit, CPRI_TX_FAST_ETH_CTRLr, txfrm_cwa, TX_FAST_ETH_CRC_MODEf);
    config_info->cw_sel     = soc_reg64_field32_get(unit, CPRI_TX_FAST_ETH_CTRLr, txfrm_cwa, TX_FAST_ETH_CW_SELf);
    config_info->cw_size    = soc_reg64_field32_get(unit, CPRI_TX_FAST_ETH_CTRLr, txfrm_cwa, TX_FAST_ETH_CW_SIZEf);

    config_info->sub_channel_size   = soc_reg64_field32_get(unit, CPRI_TX_FAST_ETH_CTRLr, txfrm_cwa, TX_FAST_ETH_SCHAN_SIZEf);
    config_info->sub_channel_start  = soc_reg64_field32_get(unit, CPRI_TX_FAST_ETH_CTRLr, txfrm_cwa, TX_FAST_ETH_SCHAN_STARTf);

    return SOC_E_NONE;
}

int cprif_drv_rx_vsd_ctrl_copy_id_get(int unit, int port, uint32* copy_id)
{
  uint32 reg_val=0;

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_VSD_CTRLr(unit, port, &reg_val));
    *copy_id =  soc_reg_field_get(unit, CPRI_RXFRM_VSD_CTRLr, reg_val, RXFRM_VSD_CTRL_SEC_NUM_GEN_COPY_IDf);

    return SOC_E_NONE;
}

int cprif_drv_rx_vsd_ctrl_copy_id_set(int unit, int port, uint32 copy_id)
{
  uint32 reg_val=0;

    if (copy_id > 1) {
        LOG_CLI(("copy_id %d is out of range, either 0 or 1  only", copy_id));
        return SOC_E_PARAM;
    }
    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_VSD_CTRLr(unit, port, &reg_val));
    soc_reg_field_set(unit, CPRI_RXFRM_VSD_CTRLr, &reg_val, RXFRM_VSD_CTRL_SEC_NUM_GEN_COPY_IDf, copy_id);
    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_RXFRM_VSD_CTRLr(unit, port, reg_val));

    return SOC_E_NONE;
}

int cprif_drv_rx_vsd_ctrl_rsvd_mask_set(int unit, int port, uint32* rsvd_mask)
{
    uint64 reg_val;

    COMPILER_64_SET(reg_val,rsvd_mask[1],rsvd_mask[0]);
    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_RXFRM_VSD_RSRV_MASK0_CTRLr(unit, port, reg_val));

    COMPILER_64_SET(reg_val,rsvd_mask[3],rsvd_mask[2]);
    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_RXFRM_VSD_RSRV_MASK1_CTRLr(unit, port, reg_val));

    return SOC_E_NONE;

}

int cprif_drv_rx_vsd_ctrl_rsvd_mask_get(int unit, int port, uint32* rsvd_mask)
{
    uint64 reg_val;

    /* get current rsvd_mask. */
    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_VSD_RSRV_MASK0_CTRLr(unit, port, &reg_val));
    rsvd_mask[0] = COMPILER_64_LO(reg_val);
    rsvd_mask[1] = COMPILER_64_HI(reg_val);

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_VSD_RSRV_MASK1_CTRLr(unit, port, &reg_val));
    rsvd_mask[2] = COMPILER_64_LO(reg_val);
    rsvd_mask[3] = COMPILER_64_HI(reg_val);

    return SOC_E_NONE;
}

int cprif_drv_rx_vsd_ctrl_ctrl_mask_set(int unit, int port, uint32 copy_id, uint32* ctrl_mask)
{
    uint64 reg_val0;
    uint64 reg_val1;

    COMPILER_64_SET(reg_val0,ctrl_mask[1],ctrl_mask[0]);
    COMPILER_64_SET(reg_val1,ctrl_mask[3],ctrl_mask[2]);

    if (copy_id == 0) {
        SOC_IF_ERROR_RETURN
            (WRITE_CPRI_RXFRM_VSD_MASK0_COPY0_CTRLr(unit, port,reg_val0));
        SOC_IF_ERROR_RETURN
            (WRITE_CPRI_RXFRM_VSD_MASK1_COPY0_CTRLr(unit, port,reg_val1));
    } else {
        SOC_IF_ERROR_RETURN
            (WRITE_CPRI_RXFRM_VSD_MASK0_COPY1_CTRLr(unit, port,reg_val0));
        SOC_IF_ERROR_RETURN
            (WRITE_CPRI_RXFRM_VSD_MASK1_COPY1_CTRLr(unit, port,reg_val1));
    }
    return SOC_E_NONE;
}

int cprif_drv_rx_vsd_ctrl_ctrl_mask_get(int unit, int port, uint32 copy_id, uint32* ctrl_mask)
{
    uint64 reg_val0;
    uint64 reg_val1;
    if (copy_id == 0) {
        SOC_IF_ERROR_RETURN
            (READ_CPRI_RXFRM_VSD_MASK0_COPY0_CTRLr(unit, port,&reg_val0));
        SOC_IF_ERROR_RETURN
            (READ_CPRI_RXFRM_VSD_MASK1_COPY0_CTRLr(unit, port,&reg_val1));
    } else {
        SOC_IF_ERROR_RETURN
            (READ_CPRI_RXFRM_VSD_MASK0_COPY1_CTRLr(unit, port,&reg_val0));
        SOC_IF_ERROR_RETURN
            (READ_CPRI_RXFRM_VSD_MASK1_COPY1_CTRLr(unit, port,&reg_val1));
    }
    ctrl_mask[0] = COMPILER_64_LO(reg_val0);
    ctrl_mask[1] = COMPILER_64_HI(reg_val0);

    ctrl_mask[2] = COMPILER_64_LO(reg_val1);
    ctrl_mask[3] = COMPILER_64_HI(reg_val1);

    return SOC_E_NONE;
}

int cprif_drv_rx_vsd_ctrl_config_set(int unit, int port, cprif_cpri_vsd_config_t* config_info)
{
    uint32 copy_id=0xff;
    uint64 ctrl_mask;
    uint32 reg_val=0;

    /*
     * Clear the control mask before updating the config to stop the processing.
     */

    SOC_IF_ERROR_RETURN
        (cprif_drv_rx_vsd_ctrl_copy_id_get(unit, port, &copy_id));

    COMPILER_64_ZERO(ctrl_mask);
    if (copy_id == 0){
        SOC_IF_ERROR_RETURN
            (WRITE_CPRI_RXFRM_VSD_MASK0_COPY1_CTRLr(unit, port,ctrl_mask));
        SOC_IF_ERROR_RETURN
            (WRITE_CPRI_RXFRM_VSD_MASK1_COPY1_CTRLr(unit, port,ctrl_mask));
        copy_id = 1; /* new copy id */
    } else { /* copy_id is 1 */
        SOC_IF_ERROR_RETURN
            (WRITE_CPRI_RXFRM_VSD_MASK0_COPY0_CTRLr(unit, port,ctrl_mask));
        SOC_IF_ERROR_RETURN
            (WRITE_CPRI_RXFRM_VSD_MASK1_COPY0_CTRLr(unit, port,ctrl_mask));
        copy_id = 0; /* new copy id */
    }

    SOC_IF_ERROR_RETURN
        (cprif_drv_rx_vsd_ctrl_copy_id_set(unit, port, copy_id));

    /*
     * Update the vsd ctrl configuration.
     */

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_VSD_CTRLr(unit, port, &reg_val));

    soc_reg_field_set(unit, CPRI_RXFRM_VSD_CTRLr, &reg_val, RXFRM_VSD_CTRL_BYTE_ORDERf, config_info->byte_order);
    soc_reg_field_set(unit, CPRI_RXFRM_VSD_CTRLr, &reg_val, RXFRM_VSD_CTRL_QUEUE_NUMf, config_info->queue_num);
    soc_reg_field_set(unit, CPRI_RXFRM_VSD_CTRLr, &reg_val, RXFRM_VSD_CTRL_FLOW_BYTES_COPY0f, config_info->flow_bytes);
    soc_reg_field_set(unit, CPRI_RXFRM_VSD_CTRLr, &reg_val, RXFRM_VSD_CTRL_SCHAN_STEP_COPY0f, config_info->sub_channel_steps);
    soc_reg_field_set(unit, CPRI_RXFRM_VSD_CTRLr, &reg_val, RXFRM_VSD_CTRL_SCHAN_BYTES_COPY0f, config_info->sub_channel_num_bytes);
    soc_reg_field_set(unit, CPRI_RXFRM_VSD_CTRLr, &reg_val, RXFRM_VSD_CTRL_SCHAN_SIZE_COPY0f, config_info->sub_channel_size);
    soc_reg_field_set(unit, CPRI_RXFRM_VSD_CTRLr, &reg_val, RXFRM_VSD_CTRL_SCHAN_START_COPY0f, config_info->sub_channel_start);

    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_RXFRM_VSD_CTRLr(unit, port, reg_val));

    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_RXFRM_VSD_RSRV_MASK0_CTRLr(unit, port,config_info->rsvd_mask[0]));
    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_RXFRM_VSD_RSRV_MASK1_CTRLr(unit, port,config_info->rsvd_mask[1]));

    return SOC_E_NONE;
}

int cprif_drv_rx_vsd_ctrl_config_get(int unit, int port, cprif_cpri_vsd_config_t* config_info)
{
    uint32 reg_val=0;


    /*
     * Get the vsd ctrl configuration.
     */

    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_VSD_CTRLr(unit, port, &reg_val));

    config_info->byte_order         = soc_reg_field_get(unit, CPRI_RXFRM_VSD_CTRLr, reg_val, RXFRM_VSD_CTRL_BYTE_ORDERf);
    config_info->queue_num          = soc_reg_field_get(unit, CPRI_RXFRM_VSD_CTRLr, reg_val, RXFRM_VSD_CTRL_QUEUE_NUMf);
    config_info->flow_bytes         = soc_reg_field_get(unit, CPRI_RXFRM_VSD_CTRLr, reg_val, RXFRM_VSD_CTRL_FLOW_BYTES_COPY0f);
    config_info->sub_channel_steps  = soc_reg_field_get(unit, CPRI_RXFRM_VSD_CTRLr, reg_val, RXFRM_VSD_CTRL_SCHAN_STEP_COPY0f);
    config_info->sub_channel_size   = soc_reg_field_get(unit, CPRI_RXFRM_VSD_CTRLr, reg_val, RXFRM_VSD_CTRL_SCHAN_SIZE_COPY0f);
    config_info->sub_channel_start  = soc_reg_field_get(unit, CPRI_RXFRM_VSD_CTRLr, reg_val, RXFRM_VSD_CTRL_SCHAN_START_COPY0f);
    config_info->sub_channel_num_bytes =soc_reg_field_get(unit, CPRI_RXFRM_VSD_CTRLr, reg_val, RXFRM_VSD_CTRL_SCHAN_BYTES_COPY0f);


    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_VSD_RSRV_MASK0_CTRLr(unit, port, &config_info->rsvd_mask[0]));
    SOC_IF_ERROR_RETURN
        (READ_CPRI_RXFRM_VSD_RSRV_MASK1_CTRLr(unit, port, &config_info->rsvd_mask[1]));

    return SOC_E_NONE;
}


int cprif_drv_rx_vsd_ctrl_flow_config_set(int unit, int port,
                                       uint32 sector_num,
                                       const cprif_cpri_rx_vsd_ctrl_flow_config_t *entry)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_VSDCTRL_CFLW_PGC_TAB_CFG_0m,
                               CPRI_VSDCTRL_CFLW_PGC_TAB_CFG_1m,
                               CPRI_VSDCTRL_CFLW_PGC_TAB_CFG_2m,
                               CPRI_VSDCTRL_CFLW_PGC_TAB_CFG_3m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = entry->group_id;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GROUP_IDf, &field_buf);

    field_buf = entry->hyper_frame_number;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, HFN_INDEXf, &field_buf);

    field_buf = entry->hyper_frame_modulo;;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, HFN_MODULOf, &field_buf);

    field_buf = entry->filter_zero_data;;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, FILTER_ZERO_DATAf, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, sector_num, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_vsd_ctrl_flow_config_get(int unit, int port,
                                         uint32 sector_num,
                                          cprif_cpri_rx_vsd_ctrl_flow_config_t *entry)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_VSDCTRL_CFLW_PGC_TAB_CFG_0m,
                               CPRI_VSDCTRL_CFLW_PGC_TAB_CFG_1m,
                               CPRI_VSDCTRL_CFLW_PGC_TAB_CFG_2m,
                               CPRI_VSDCTRL_CFLW_PGC_TAB_CFG_3m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , sector_num, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GROUP_IDf, &field_buf);
    entry->group_id = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, HFN_INDEXf, &field_buf);
    entry->hyper_frame_number = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, HFN_MODULOf, &field_buf);
    entry->hyper_frame_modulo = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, FILTER_ZERO_DATAf, &field_buf);
    entry->filter_zero_data = field_buf;
exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_vsd_ctrl_group_num_sector_set(int unit, int port,
                                               uint32 group_id,
                                               uint32 num_sector)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_VSDCTRL_NUM_SEC_TAB_CFG_0m,
                               CPRI_VSDCTRL_NUM_SEC_TAB_CFG_1m,
                               CPRI_VSDCTRL_NUM_SEC_TAB_CFG_2m,
                               CPRI_VSDCTRL_NUM_SEC_TAB_CFG_3m };
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = num_sector;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, NUM_SECf, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, group_id, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_vsd_ctrl_group_num_sector_get(int unit, int port,
                                               uint32 group_id,
                                               uint32* num_sector)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_VSDCTRL_NUM_SEC_TAB_CFG_0m,
                               CPRI_VSDCTRL_NUM_SEC_TAB_CFG_1m,
                               CPRI_VSDCTRL_NUM_SEC_TAB_CFG_2m,
                               CPRI_VSDCTRL_NUM_SEC_TAB_CFG_3m };

    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);
    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , group_id, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, NUM_SECf, &field_buf);
    *num_sector = field_buf;
exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_vsd_ctrl_group_config_set(int unit, int port,
                                        uint32 group_id,
                                        uint32 tag_id,
                                        uint32 grp_ptr_index)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_VSDCTRL_GRP_CFG_TAB_CFG_0m,
                               CPRI_VSDCTRL_GRP_CFG_TAB_CFG_1m,
                               CPRI_VSDCTRL_GRP_CFG_TAB_CFG_2m,
                               CPRI_VSDCTRL_GRP_CFG_TAB_CFG_3m };
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = tag_id;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, TAG_IDf, &field_buf);

    field_buf = grp_ptr_index;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GRP_PTRf, &field_buf);
    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, group_id, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_vsd_ctrl_group_config_get(int unit, int port,
                                           uint32 group_id,
                                           uint32* tag_id,
                                           uint32* grp_ptr_index)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_VSDCTRL_GRP_CFG_TAB_CFG_0m,
                               CPRI_VSDCTRL_GRP_CFG_TAB_CFG_1m,
                               CPRI_VSDCTRL_GRP_CFG_TAB_CFG_2m,
                               CPRI_VSDCTRL_GRP_CFG_TAB_CFG_3m };

    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , group_id, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, TAG_IDf, &field_buf);
    *tag_id = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GRP_PTRf, &field_buf);
    *grp_ptr_index = field_buf;
exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_rx_vsd_ctrl_group_assign_ptr_set(int unit, int port,
                                               uint32 ptr_index,
                                               uint32 sec_num)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_VSDCTRL_GRP_ASSIGN_TAB_CFG_0m,
                               CPRI_VSDCTRL_GRP_ASSIGN_TAB_CFG_1m,
                               CPRI_VSDCTRL_GRP_ASSIGN_TAB_CFG_2m,
                               CPRI_VSDCTRL_GRP_ASSIGN_TAB_CFG_3m };
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = sec_num;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, SEC_NUMf, &field_buf);


    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, ptr_index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_vsd_ctrl_group_assign_ptr_get(int unit, int port,
                                                uint32 ptr_index,
                                                uint32* sec_num)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_VSDCTRL_GRP_ASSIGN_TAB_CFG_0m,
                               CPRI_VSDCTRL_GRP_ASSIGN_TAB_CFG_1m,
                               CPRI_VSDCTRL_GRP_ASSIGN_TAB_CFG_2m,
                               CPRI_VSDCTRL_GRP_ASSIGN_TAB_CFG_3m };

    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);
    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , ptr_index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, SEC_NUMf, &field_buf);
    *sec_num = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_allocate_grp_ptr (uint32* grp_ptr_mask, uint32 num_sec, uint32* ptr_index)
{
    uint16 num_entries;
    uint16 index;
    uint16 first_location;
    uint8  location_found;

    /*
     * Find the grp ptr location that can accomodate num_sec.
     */
    index = 0;
    first_location = 0;
    location_found = FALSE;
    num_entries = num_sec;

    while (num_entries && (index < CPRIF_VSD_CTRL_NUM_GROUP_PTR)) {
        num_entries = num_sec;
        /*
         * Find first available spot.
         */
        for ( ; index < CPRIF_VSD_CTRL_NUM_GROUP_PTR; index++) {
            if (CPRIF_PBMP_AVAILABLE(grp_ptr_mask, index)) {
                first_location = index;
                location_found  = TRUE;
                break;
            }
        } /* for */
        /*
         * Check that spot has enough available space.
         */
        if (location_found) {
            for ( ;(index < CPRIF_VSD_CTRL_NUM_GROUP_PTR) && (num_entries > 0) ; index++) {
                if (CPRIF_PBMP_AVAILABLE(grp_ptr_mask, index)){
                    num_entries--;
                } else {
                    location_found = FALSE;
                    break;
                }
            } /* for */
        }
    } /* while */

    if (num_entries == 0) {
        *ptr_index = first_location;
    } else {
        return SOC_E_UNAVAIL;
    }
    return SOC_E_NONE;
}


int cprif_drv_tx_vsd_ctrl_copy_id_get(int unit, int port, uint32* copy_id)
{
  uint32 reg_val=0;

    SOC_IF_ERROR_RETURN
        (READ_CPRI_TXFRM_CWA_VSD_CTRL_CFGr(unit, port, &reg_val));
    *copy_id =  soc_reg_field_get(unit, CPRI_TXFRM_CWA_VSD_CTRL_CFGr, reg_val,
                                  TX_CWA_VSD_CTRL_COPY_IDf);

    return SOC_E_NONE;
}

int cprif_drv_tx_vsd_ctrl_copy_id_set(int unit, int port, uint32 copy_id)
{
  uint32 reg_val=0;

    if (copy_id > 1) {
        LOG_CLI(("copy_id %d is out of range, either 0 or 1  only", copy_id));
        return SOC_E_PARAM;
    }
    SOC_IF_ERROR_RETURN
        (READ_CPRI_TXFRM_CWA_VSD_CTRL_CFGr(unit, port, &reg_val));
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_VSD_CTRL_CFGr, &reg_val, TX_CWA_VSD_CTRL_COPY_IDf, copy_id);
    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_TXFRM_CWA_VSD_CTRL_CFGr(unit, port, reg_val));

    return SOC_E_NONE;
}

int cprif_drv_tx_vsd_ctrl_rsvd_mask_set(int unit, int port, uint32* rsvd_mask)
{
    uint64 reg_val;

    COMPILER_64_SET(reg_val,rsvd_mask[1],rsvd_mask[0]);
    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_TXFRM_CWA_VSD_CTRL_RSRV_MASK0_CTRL2r(unit, port, reg_val));

    COMPILER_64_SET(reg_val,rsvd_mask[3],rsvd_mask[2]);
    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_TXFRM_CWA_VSD_CTRL_RSRV_MASK1_CTRL2r(unit, port, reg_val));

    return SOC_E_NONE;

}

int cprif_drv_tx_vsd_ctrl_rsvd_mask_get(int unit, int port, uint32* rsvd_mask)
{
    uint64 reg_val;

    /* get current rsvd_mask. */
    SOC_IF_ERROR_RETURN
        (READ_CPRI_TXFRM_CWA_VSD_CTRL_RSRV_MASK0_CTRL2r(unit, port, &reg_val));
    rsvd_mask[0] = COMPILER_64_LO(reg_val);
    rsvd_mask[1] = COMPILER_64_HI(reg_val);

    SOC_IF_ERROR_RETURN
        (READ_CPRI_TXFRM_CWA_VSD_CTRL_RSRV_MASK1_CTRL2r(unit, port, &reg_val));
    rsvd_mask[2] = COMPILER_64_LO(reg_val);
    rsvd_mask[3] = COMPILER_64_HI(reg_val);

    return SOC_E_NONE;
}

int cprif_drv_tx_vsd_ctrl_ctrl_mask_set(int unit, int port, uint32 copy_id, uint32* ctrl_mask)
{
    uint64 reg_val0;
    uint64 reg_val1;

    COMPILER_64_SET(reg_val0,ctrl_mask[1],ctrl_mask[0]);
    COMPILER_64_SET(reg_val1,ctrl_mask[3],ctrl_mask[2]);

    if (copy_id == 0) {
        SOC_IF_ERROR_RETURN
            (WRITE_CPRI_TXFRM_CWA_VSD_CTRL_MASK0_CTRL0r(unit, port,reg_val0));
        SOC_IF_ERROR_RETURN
            (WRITE_CPRI_TXFRM_CWA_VSD_CTRL_MASK1_CTRL0r(unit, port,reg_val1));
    } else {
        SOC_IF_ERROR_RETURN
            (WRITE_CPRI_TXFRM_CWA_VSD_CTRL_MASK0_CTRL1r(unit, port,reg_val0));
        SOC_IF_ERROR_RETURN
            (WRITE_CPRI_TXFRM_CWA_VSD_CTRL_MASK1_CTRL1r(unit, port,reg_val1));
    }
    return SOC_E_NONE;
}

int cprif_drv_tx_vsd_ctrl_ctrl_mask_get(int unit, int port, uint32 copy_id, uint32* ctrl_mask)
{
    uint64 reg_val0;
    uint64 reg_val1;
    if (copy_id == 0) {
        SOC_IF_ERROR_RETURN
            (READ_CPRI_TXFRM_CWA_VSD_CTRL_MASK0_CTRL0r(unit, port,&reg_val0));
        SOC_IF_ERROR_RETURN
            (READ_CPRI_TXFRM_CWA_VSD_CTRL_MASK1_CTRL0r(unit, port,&reg_val1));
    } else {
        SOC_IF_ERROR_RETURN
            (READ_CPRI_TXFRM_CWA_VSD_CTRL_MASK0_CTRL1r(unit, port,&reg_val0));
        SOC_IF_ERROR_RETURN
            (READ_CPRI_TXFRM_CWA_VSD_CTRL_MASK1_CTRL1r(unit, port,&reg_val1));
    }
    ctrl_mask[0] = COMPILER_64_LO(reg_val0);
    ctrl_mask[1] = COMPILER_64_HI(reg_val0);

    ctrl_mask[2] = COMPILER_64_LO(reg_val1);
    ctrl_mask[3] = COMPILER_64_HI(reg_val1);

    return SOC_E_NONE;
}


int cprif_drv_tx_vsd_ctrl_flow_config_set(int unit, int port,
                                       uint32 sector_num,
                                       const cprif_cpri_tx_vsd_ctrl_flow_config_t *entry)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_CWA_VSD_CTRL_CONFIG_CFG_0m,
                               CPRI_CWA_VSD_CTRL_CONFIG_CFG_1m,
                               CPRI_CWA_VSD_CTRL_CONFIG_CFG_2m,
                               CPRI_CWA_VSD_CTRL_CONFIG_CFG_3m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = entry->hyper_frame_number;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, HFN_INDEXf, &field_buf);

    field_buf = entry->hyper_frame_modulo;;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, HFN_MODULOf, &field_buf);

    field_buf = entry->repeat_mode;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, REPEAT_MODEf, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, sector_num, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_vsd_ctrl_flow_config_get(int unit, int port,
                                         uint32 sector_num,
                                          cprif_cpri_tx_vsd_ctrl_flow_config_t *entry)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_CWA_VSD_CTRL_CONFIG_CFG_0m,
                               CPRI_CWA_VSD_CTRL_CONFIG_CFG_1m,
                               CPRI_CWA_VSD_CTRL_CONFIG_CFG_2m,
                               CPRI_CWA_VSD_CTRL_CONFIG_CFG_3m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);
    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , sector_num, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, HFN_INDEXf, &field_buf);
    entry->hyper_frame_number = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, HFN_MODULOf, &field_buf);
    entry->hyper_frame_modulo = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, REPEAT_MODEf, &field_buf);
    entry->repeat_mode = field_buf;
exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_tx_vsd_ctrl_flow_id_to_group_mapping_set(int unit, int port,
                                                       uint32 roe_flow_id,
                                                       uint32 group_id)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_CWA_VSD_CTRL_FLOWID_MAP_TAB_CFG_0m,
                               CPRI_CWA_VSD_CTRL_FLOWID_MAP_TAB_CFG_1m,
                               CPRI_CWA_VSD_CTRL_FLOWID_MAP_TAB_CFG_2m,
                               CPRI_CWA_VSD_CTRL_FLOWID_MAP_TAB_CFG_3m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = group_id;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_CTRL_GROUP_IDf, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, roe_flow_id, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_vsd_ctrl_flow_id_to_group_mapping_get(int unit, int port,
                                                       uint32 roe_flow_id,
                                                       uint32* group_id)

{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_CWA_VSD_CTRL_FLOWID_MAP_TAB_CFG_0m,
                               CPRI_CWA_VSD_CTRL_FLOWID_MAP_TAB_CFG_1m,
                               CPRI_CWA_VSD_CTRL_FLOWID_MAP_TAB_CFG_2m,
                               CPRI_CWA_VSD_CTRL_FLOWID_MAP_TAB_CFG_3m};

    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);
    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , roe_flow_id, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_CTRL_GROUP_IDf, &field_buf);
    *group_id = field_buf;
exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_tx_vsd_ctrl_group_config_set(int unit, int port,
                                           uint32 group_id,
                                           uint32 num_sector,
                                           uint32 grp_ptr_index)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_CWA_VSD_CTRL_GRP_CFG_TAB_CFG_0m,
                               CPRI_CWA_VSD_CTRL_GRP_CFG_TAB_CFG_1m,
                               CPRI_CWA_VSD_CTRL_GRP_CFG_TAB_CFG_2m,
                               CPRI_CWA_VSD_CTRL_GRP_CFG_TAB_CFG_3m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = num_sector;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_CTRL_NUM_SECf, &field_buf);

    field_buf = grp_ptr_index;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_CTRL_GROUP_PTRf, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, group_id, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_vsd_ctrl_group_config_get(int unit, int port,
                                           uint32 group_id,
                                           uint32* num_sector,
                                           uint32* grp_ptr_index)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_CWA_VSD_CTRL_GRP_CFG_TAB_CFG_0m,
                               CPRI_CWA_VSD_CTRL_GRP_CFG_TAB_CFG_1m,
                               CPRI_CWA_VSD_CTRL_GRP_CFG_TAB_CFG_2m,
                               CPRI_CWA_VSD_CTRL_GRP_CFG_TAB_CFG_3m};

    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);
    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , group_id, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_CTRL_NUM_SECf, &field_buf);
    *num_sector = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_CTRL_GROUP_PTRf, &field_buf);
    *grp_ptr_index = field_buf;
exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_vsd_ctrl_group_assign_ptr_set(int unit, int port,
                                               uint32 ptr_index,
                                               uint32 sec_num)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_CWA_VSD_CTRL_GRP_ASSIGN_CFG_TAB_CFG_0m,
                               CPRI_CWA_VSD_CTRL_GRP_ASSIGN_CFG_TAB_CFG_1m,
                               CPRI_CWA_VSD_CTRL_GRP_ASSIGN_CFG_TAB_CFG_2m,
                               CPRI_CWA_VSD_CTRL_GRP_ASSIGN_CFG_TAB_CFG_3m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = sec_num;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_CTRL_SECTORf, &field_buf);


    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, ptr_index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_vsd_ctrl_group_assign_ptr_get(int unit, int port,
                                                uint32 ptr_index,
                                                uint32* sec_num)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_CWA_VSD_CTRL_GRP_ASSIGN_CFG_TAB_CFG_0m,
                               CPRI_CWA_VSD_CTRL_GRP_ASSIGN_CFG_TAB_CFG_1m,
                               CPRI_CWA_VSD_CTRL_GRP_ASSIGN_CFG_TAB_CFG_2m,
                               CPRI_CWA_VSD_CTRL_GRP_ASSIGN_CFG_TAB_CFG_3m};

    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);
    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , ptr_index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_CTRL_SECTORf, &field_buf);
    *sec_num = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_tx_vsd_ctrl_config_set(int unit, int port, const cprif_cpri_vsd_config_t* config_info)
{
    uint32 copy_id=0xff;
    uint64 ctrl_mask;
    uint32 reg_val=0;

    /*
     * Clear the control mask before updating the config to stop the processing.
     */

    SOC_IF_ERROR_RETURN
        (cprif_drv_tx_vsd_ctrl_copy_id_get(unit, port, &copy_id));

    COMPILER_64_ZERO(ctrl_mask);
    if (copy_id == 0){
        SOC_IF_ERROR_RETURN
            (WRITE_CPRI_TXFRM_CWA_VSD_CTRL_MASK0_CTRL1r(unit, port,ctrl_mask));
        SOC_IF_ERROR_RETURN
            (WRITE_CPRI_TXFRM_CWA_VSD_CTRL_MASK1_CTRL1r(unit, port,ctrl_mask));
        copy_id = 1; /* new copy id */
    } else { /* copy_id is 1 */
        SOC_IF_ERROR_RETURN
            (WRITE_CPRI_TXFRM_CWA_VSD_CTRL_MASK0_CTRL0r(unit, port,ctrl_mask));
        SOC_IF_ERROR_RETURN
            (WRITE_CPRI_TXFRM_CWA_VSD_CTRL_MASK1_CTRL0r(unit, port,ctrl_mask));
        copy_id = 0; /* new copy id */
    }

    SOC_IF_ERROR_RETURN
        (cprif_drv_tx_vsd_ctrl_copy_id_set(unit, port, copy_id));

    /*
     * Update the vsd ctrl configuration.
     */

    SOC_IF_ERROR_RETURN
        (READ_CPRI_TXFRM_CWA_VSD_CTRL_CTRLr(unit, port, &reg_val));
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_VSD_CTRL_CTRLr, &reg_val, TX_CWA_VSD_CTRL_BYTE_ORDERf,  config_info->byte_order);
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_VSD_CTRL_CTRLr, &reg_val, TX_CWA_VSD_CTRL_FLOW_BYTESf,  config_info->flow_bytes);
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_VSD_CTRL_CTRLr, &reg_val, TX_CWA_VSD_CTRL_SCHAN_STEPf,  config_info->sub_channel_steps);
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_VSD_CTRL_CTRLr, &reg_val, TX_CWA_VSD_CTRL_SCHAN_BYTESf, config_info->sub_channel_num_bytes);
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_VSD_CTRL_CTRLr, &reg_val, TX_CWA_VSD_CTRL_SCHAN_SIZEf,  config_info->sub_channel_size);
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_VSD_CTRL_CTRLr, &reg_val, TX_CWA_VSD_CTRL_SCHAN_STARTf, config_info->sub_channel_start);
    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_TXFRM_CWA_VSD_CTRL_CTRLr(unit, port, reg_val));

    SOC_IF_ERROR_RETURN
        (READ_CPRI_TXFRM_CWA_VSD_CTRL_CFGr(unit, port, &reg_val));
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_VSD_CTRL_CFGr, &reg_val, TX_CWA_VSD_CTRL_QUEUE_NUMf, config_info->queue_num);
    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_TXFRM_CWA_VSD_CTRL_CFGr(unit, port, reg_val));

    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_TXFRM_CWA_VSD_CTRL_RSRV_MASK0_CTRL2r(unit, port,config_info->rsvd_mask[0]));
    SOC_IF_ERROR_RETURN
        (WRITE_CPRI_TXFRM_CWA_VSD_CTRL_RSRV_MASK1_CTRL2r(unit, port,config_info->rsvd_mask[1]));

    return SOC_E_NONE;
}

int cprif_drv_tx_vsd_ctrl_config_get(int unit, int port, cprif_cpri_vsd_config_t* config_info)
{
    uint32 reg_val=0;

    SOC_IF_ERROR_RETURN
        (READ_CPRI_TXFRM_CWA_VSD_CTRL_CTRLr(unit, port, &reg_val));

    config_info->byte_order = soc_reg_field_get(unit, CPRI_TXFRM_CWA_VSD_CTRL_CTRLr, reg_val, TX_CWA_VSD_CTRL_BYTE_ORDERf);
    config_info->flow_bytes = soc_reg_field_get(unit, CPRI_TXFRM_CWA_VSD_CTRL_CTRLr, reg_val, TX_CWA_VSD_CTRL_FLOW_BYTESf);
    config_info->sub_channel_steps = soc_reg_field_get(unit, CPRI_TXFRM_CWA_VSD_CTRL_CTRLr, reg_val, TX_CWA_VSD_CTRL_SCHAN_STEPf);
    config_info->sub_channel_num_bytes = soc_reg_field_get(unit, CPRI_TXFRM_CWA_VSD_CTRL_CTRLr, reg_val, TX_CWA_VSD_CTRL_SCHAN_BYTESf);
    config_info->sub_channel_size = soc_reg_field_get(unit, CPRI_TXFRM_CWA_VSD_CTRL_CTRLr, reg_val, TX_CWA_VSD_CTRL_SCHAN_SIZEf);
    config_info->sub_channel_start = soc_reg_field_get(unit, CPRI_TXFRM_CWA_VSD_CTRL_CTRLr, reg_val, TX_CWA_VSD_CTRL_SCHAN_STARTf);

    SOC_IF_ERROR_RETURN
        (READ_CPRI_TXFRM_CWA_VSD_CTRL_CFGr(unit, port, &reg_val));
    config_info->queue_num = soc_reg_field_get(unit, CPRI_TXFRM_CWA_VSD_CTRL_CFGr, reg_val, TX_CWA_VSD_CTRL_QUEUE_NUMf);

    SOC_IF_ERROR_RETURN
        (READ_CPRI_TXFRM_CWA_VSD_CTRL_RSRV_MASK0_CTRL2r(unit, port, &config_info->rsvd_mask[0]));

    SOC_IF_ERROR_RETURN
        (READ_CPRI_TXFRM_CWA_VSD_CTRL_RSRV_MASK1_CTRL2r(unit, port, &config_info->rsvd_mask[1]));

    return SOC_E_NONE;
}
int cprif_drv_encap_tag_to_flow_id_set(int unit, int port,
                                       uint32 tag_id,
                                       uint32 flow_id)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_QUE_CTL_FLOWID_TAB_0m,
                               CPRI_ENCAP_QUE_CTL_FLOWID_TAB_1m,
                               CPRI_ENCAP_QUE_CTL_FLOWID_TAB_2m,
                               CPRI_ENCAP_QUE_CTL_FLOWID_TAB_3m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = flow_id;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, QUEUE_ENCAP_CTL_FLOWIDf, &field_buf);


    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, tag_id, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_encap_tag_to_flow_id_get(int unit, int port,
                                       uint32 tag_id,
                                       uint32* flow_id)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_ENCAP_QUE_CTL_FLOWID_TAB_0m,
                               CPRI_ENCAP_QUE_CTL_FLOWID_TAB_0m,
                               CPRI_ENCAP_QUE_CTL_FLOWID_TAB_0m,
                               CPRI_ENCAP_QUE_CTL_FLOWID_TAB_0m};

    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , tag_id, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, QUEUE_ENCAP_CTL_FLOWIDf, &field_buf);
    *flow_id = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_vsd_raw_config_set(int unit, int port,
                                 uint32 index,
                                 cprif_cpri_vsd_raw_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_VSDRAW_LOC_TAB_CFG_0m,
                              CPRI_VSDRAW_LOC_TAB_CFG_1m,
                              CPRI_VSDRAW_LOC_TAB_CFG_2m,
                              CPRI_VSDRAW_LOC_TAB_CFG_3m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = config->schan_start;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_SCHAN_STARTf, &field_buf);

    field_buf = config->schan_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_SCHAN_SIZEf, &field_buf);

    field_buf = config->cw_select;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_CW_SELf, &field_buf);

    field_buf = config->cw_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_CW_SIZEf, &field_buf);


    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_vsd_raw_config_get(int unit, int port,
                                 uint32 index,
                                 cprif_cpri_vsd_raw_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_VSDRAW_LOC_TAB_CFG_0m,
                              CPRI_VSDRAW_LOC_TAB_CFG_1m,
                              CPRI_VSDRAW_LOC_TAB_CFG_2m,
                              CPRI_VSDRAW_LOC_TAB_CFG_3m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_SCHAN_STARTf, &field_buf);
    config->schan_start = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_SCHAN_SIZEf, &field_buf);
    config->schan_size = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_CW_SELf, &field_buf);
    config->cw_select = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_CW_SIZEf, &field_buf);
    config->cw_size = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_rx_vsd_raw_config_cw_sel_size_swap (int unit, int port,
                                                  uint32 index,
                                                  uint32* cw_sel,
                                                  uint32* cw_size)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_VSDRAW_LOC_TAB_CFG_0m,
                              CPRI_VSDRAW_LOC_TAB_CFG_1m,
                              CPRI_VSDRAW_LOC_TAB_CFG_2m,
                              CPRI_VSDRAW_LOC_TAB_CFG_3m};
    uint32 *data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(data_ptr);
    sal_memset(data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , index, data_ptr));

    soc_mem_field_get(unit, local_mem, data_ptr, VSD_RAW_CW_SELf, &field_buf);
    soc_mem_field_set(unit, local_mem, data_ptr, VSD_RAW_CW_SELf, cw_sel);
    *cw_sel = field_buf;

    soc_mem_field_get(unit, local_mem, data_ptr, VSD_RAW_CW_SIZEf, &field_buf);
    soc_mem_field_set(unit, local_mem, data_ptr, VSD_RAW_CW_SIZEf, cw_size);
    *cw_size = field_buf;

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, index, data_ptr));

exit:
    sal_free(data_ptr);
    SOC_FUNC_RETURN;
}
int cprif_drv_rx_vsd_raw_filter_config_set(int unit, int port,
                                        uint32 index,
                                        cprif_cpri_rx_vsd_raw_filter_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_VSDRAW_FLT_TAB_CFG_0m,
                              CPRI_VSDRAW_FLT_TAB_CFG_1m,
                              CPRI_VSDRAW_FLT_TAB_CFG_2m,
                              CPRI_VSDRAW_FLT_TAB_CFG_3m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = config->filter_mode;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_FILTER_MODEf, &field_buf);

    field_buf = config->hfn_index;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_HFN_INDEXf, &field_buf);

    field_buf = config->hfn_modulo;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_HFN_MODULOf, &field_buf);

    field_buf = config->match_offset;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_MATCH_OFFSETf, &field_buf);

    field_buf = config->match_value;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_MATCH_VALUEf, &field_buf);

    field_buf = config->match_mask;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_MATCH_MASKf, &field_buf);

    field_buf = config->queue_num;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_QUEUE_NUMf, &field_buf);


    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_vsd_raw_filter_config_get(int unit, int port,
                                           uint32 index,
                                           cprif_cpri_rx_vsd_raw_filter_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_VSDRAW_FLT_TAB_CFG_0m,
                              CPRI_VSDRAW_FLT_TAB_CFG_1m,
                              CPRI_VSDRAW_FLT_TAB_CFG_2m,
                              CPRI_VSDRAW_FLT_TAB_CFG_3m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_FILTER_MODEf, &field_buf);
    config->filter_mode = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_HFN_INDEXf, &field_buf);
    config->hfn_index = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_HFN_MODULOf, &field_buf);
    config->hfn_modulo = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_MATCH_OFFSETf, &field_buf);
    config->match_offset = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_MATCH_VALUEf, &field_buf);
    config->match_value = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_MATCH_MASKf, &field_buf);
    config->match_mask = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_QUEUE_NUMf, &field_buf);
    config->queue_num = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_tx_vsd_raw_config_set(int unit, int port,
                                    uint32 index,
                                    cprif_cpri_vsd_raw_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_CWA_VSD_RAW_LOC_TAB_CFG_0m,
                              CPRI_CWA_VSD_RAW_LOC_TAB_CFG_1m,
                              CPRI_CWA_VSD_RAW_LOC_TAB_CFG_2m,
                              CPRI_CWA_VSD_RAW_LOC_TAB_CFG_3m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = config->schan_start;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_SCHAN_STARTf, &field_buf);

    field_buf = config->schan_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_SCHAN_SIZEf, &field_buf);

    field_buf = config->cw_select;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_CW_SELf, &field_buf);

    field_buf = config->cw_size;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_CW_SIZEf, &field_buf);


    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_vsd_raw_config_get(int unit, int port,
                                    uint32 index,
                                    cprif_cpri_vsd_raw_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_CWA_VSD_RAW_LOC_TAB_CFG_0m,
                              CPRI_CWA_VSD_RAW_LOC_TAB_CFG_1m,
                              CPRI_CWA_VSD_RAW_LOC_TAB_CFG_2m,
                              CPRI_CWA_VSD_RAW_LOC_TAB_CFG_3m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_SCHAN_STARTf, &field_buf);
    config->schan_start = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_SCHAN_SIZEf, &field_buf);
    config->schan_size = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_CW_SELf, &field_buf);
    config->cw_select = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_CW_SIZEf, &field_buf);
    config->cw_size = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}


int cprif_drv_tx_vsd_raw_filter_config_set(int unit, int port,
                                        uint32 index,
                                        cprif_cpri_tx_vsd_raw_filter_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_CWA_VSD_RAW_MAP_TAB_CFG_0m,
                              CPRI_CWA_VSD_RAW_MAP_TAB_CFG_1m,
                              CPRI_CWA_VSD_RAW_MAP_TAB_CFG_2m,
                              CPRI_CWA_VSD_RAW_MAP_TAB_CFG_3m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = config->queue_num;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_QUEUE_NUMf, &field_buf);

    field_buf = config->map_mode;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_MAP_MODEf, &field_buf);

    field_buf = config->repeat_mode;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_REPEAT_MODEf, &field_buf);

    field_buf = config->bfn0_filter_enable;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_BFN0_FILTER_MODEf, &field_buf);

    field_buf = config->bfn1_filter_enable;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_BFN1_FILTER_MODEf, &field_buf);

    field_buf = config->hfn_index;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_HFN_INDEXf, &field_buf);

    field_buf = config->hfn_modulo;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_HFN_MODULOf, &field_buf);

    field_buf = config->idle_value;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, VSD_RAW_IDLE_VALUEf, &field_buf);


    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_vsd_raw_filter_config_get(int unit, int port,
                                 uint32 index,
                                 cprif_cpri_tx_vsd_raw_filter_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_CWA_VSD_RAW_MAP_TAB_CFG_0m,
                              CPRI_CWA_VSD_RAW_MAP_TAB_CFG_1m,
                              CPRI_CWA_VSD_RAW_MAP_TAB_CFG_2m,
                              CPRI_CWA_VSD_RAW_MAP_TAB_CFG_3m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_QUEUE_NUMf, &field_buf);
    config->queue_num = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_MAP_MODEf, &field_buf);
    config->map_mode = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_REPEAT_MODEf, &field_buf);
    config->repeat_mode = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_BFN0_FILTER_MODEf, &field_buf);
    config->bfn0_filter_enable = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_BFN1_FILTER_MODEf, &field_buf);
    config->bfn1_filter_enable = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_HFN_INDEXf, &field_buf);
    config->hfn_index = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_HFN_MODULOf, &field_buf);
    config->hfn_modulo = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, VSD_RAW_IDLE_VALUEf, &field_buf);
    config->idle_value = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_vsd_raw_radio_frame_filter_set(int unit, int port_num,
                                                uint32 bfn0_value,
                                                uint32 bfn0_mask,
                                                uint32 bfn1_value,
                                                uint32 bfn1_mask)
{
     uint64 vsd_raw_ctrl, val64;

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_CWA_VSD_RAW_CTRLr(unit, port_num, &vsd_raw_ctrl));
    COMPILER_64_SET(val64, 0, bfn0_value);
    soc_reg64_field_set(unit, CPRI_TXFRM_CWA_VSD_RAW_CTRLr, &vsd_raw_ctrl, TX_CWA_VSD_RAW_BFN0_VALUEf, val64);
    COMPILER_64_SET(val64, 0, bfn0_mask);
    soc_reg64_field_set(unit, CPRI_TXFRM_CWA_VSD_RAW_CTRLr, &vsd_raw_ctrl, TX_CWA_VSD_RAW_BFN0_MASKf, val64);
    COMPILER_64_SET(val64, 0, bfn1_value);
    soc_reg64_field_set(unit, CPRI_TXFRM_CWA_VSD_RAW_CTRLr, &vsd_raw_ctrl, TX_CWA_VSD_RAW_BFN1_VALUEf, val64);
    COMPILER_64_SET(val64, 0, bfn1_mask);
    soc_reg64_field_set(unit, CPRI_TXFRM_CWA_VSD_RAW_CTRLr, &vsd_raw_ctrl, TX_CWA_VSD_RAW_BFN1_MASKf, val64);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_CWA_VSD_RAW_CTRLr(unit, port_num, vsd_raw_ctrl));
    return SOC_E_NONE;
}

int cprif_drv_tx_vsd_raw_radio_frame_filter_get(int unit, int port_num,
                                                uint32* bfn0_value,
                                                uint32* bfn0_mask,
                                                uint32* bfn1_value,
                                                uint32* bfn1_mask)
{
     uint64 vsd_raw_ctrl;

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_CWA_VSD_RAW_CTRLr(unit, port_num, &vsd_raw_ctrl));

    *bfn0_value = soc_reg64_field32_get(unit, CPRI_TXFRM_CWA_VSD_RAW_CTRLr, vsd_raw_ctrl, TX_CWA_VSD_RAW_BFN0_VALUEf);
    *bfn0_mask  = soc_reg64_field32_get(unit, CPRI_TXFRM_CWA_VSD_RAW_CTRLr, vsd_raw_ctrl, TX_CWA_VSD_RAW_BFN0_MASKf);
    *bfn1_value = soc_reg64_field32_get(unit, CPRI_TXFRM_CWA_VSD_RAW_CTRLr, vsd_raw_ctrl, TX_CWA_VSD_RAW_BFN1_VALUEf);
    *bfn1_mask  = soc_reg64_field32_get(unit, CPRI_TXFRM_CWA_VSD_RAW_CTRLr, vsd_raw_ctrl, TX_CWA_VSD_RAW_BFN1_MASKf);

    return SOC_E_NONE;
}

int cprif_drv_rx_brcm_rsvd5_config_set(int unit, int port,
                                       uint32 schan_start, uint32 schan_size,
                                       uint32 queue_num, int parity_disable)
{
    uint32 reg_val=0;
    SOC_INIT_FUNC_DEFS;


    soc_reg_field_set(unit, BRCM_RESERVED_CPM_315r, &reg_val,
                      BRCM_RESERVED_CPM_315_0f, schan_start);

    soc_reg_field_set(unit, BRCM_RESERVED_CPM_315r, &reg_val,
                      BRCM_RESERVED_CPM_315_1f, schan_size);

    soc_reg_field_set(unit, BRCM_RESERVED_CPM_315r, &reg_val,
                      BRCM_RESERVED_CPM_315_2f, parity_disable?1:0);

    _SOC_IF_ERR_EXIT(WRITE_BRCM_RESERVED_CPM_315r(unit, port, reg_val));

    _SOC_IF_ERR_EXIT(READ_BRCM_RESERVED_CPM_300r(unit, port, &reg_val));

    soc_reg_field_set(unit, BRCM_RESERVED_CPM_300r, &reg_val,
                      BRCM_RESERVED_CPM_300_0f, queue_num);

    _SOC_IF_ERR_EXIT(WRITE_BRCM_RESERVED_CPM_300r(unit, port, reg_val));


exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_rx_brcm_rsvd5_config_get(int unit, int port,
                                       uint32* schan_start, uint32* schan_size,
                                       uint32* queue_num, int* parity_disable)
{
    uint32 reg_val=0;
    uint32 field_buf=0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_BRCM_RESERVED_CPM_315r(unit, port, &reg_val));
    *schan_start = soc_reg_field_get(unit, BRCM_RESERVED_CPM_315r, reg_val,
                      BRCM_RESERVED_CPM_315_0f);

    *schan_size = soc_reg_field_get(unit, BRCM_RESERVED_CPM_315r, reg_val,
                      BRCM_RESERVED_CPM_315_1f);

    field_buf = soc_reg_field_get(unit, BRCM_RESERVED_CPM_315r, reg_val,
                      BRCM_RESERVED_CPM_315_2f);
    if (field_buf) {
        *parity_disable = 1;
    } else {
        *parity_disable = 0;
    }
    _SOC_IF_ERR_EXIT(READ_BRCM_RESERVED_CPM_300r(unit, port, &reg_val));

    *queue_num = soc_reg_field_get(unit, BRCM_RESERVED_CPM_300r, reg_val,
                      BRCM_RESERVED_CPM_300_0f);

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_brcm_rsvd5_config_size_swap (int unit, int port,
                                              uint32* schan_size)
{
    uint32 reg_val=0;
    uint32 field_buf=0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_BRCM_RESERVED_CPM_315r(unit, port, &reg_val));

    field_buf  = soc_reg_field_get(unit, BRCM_RESERVED_CPM_315r, reg_val,
                      BRCM_RESERVED_CPM_315_1f);

    soc_reg_field_set(unit, BRCM_RESERVED_CPM_315r, &reg_val,
                      BRCM_RESERVED_CPM_315_1f, *schan_size);

    *schan_size = field_buf;
   _SOC_IF_ERR_EXIT(WRITE_BRCM_RESERVED_CPM_315r(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_brcm_rsvd5_config_set(int unit, int port,
                                       uint32 schan_start,
                                       uint32 schan_size,
                                       uint32 queue_num,
                                       int crc_enable)
{
    uint64 reg_val;
    uint64 val64;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val);
    COMPILER_64_ZERO(val64);
    COMPILER_64_SET(val64, 0, schan_start);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_0r, &reg_val,
                      BRCM_RESERVED_CPM_0_3f, val64);

    COMPILER_64_SET(val64, 0, schan_size);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_0r, &reg_val,
                      BRCM_RESERVED_CPM_0_2f, val64);

    COMPILER_64_SET(val64, 0, queue_num);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_0r, &reg_val,
                      BRCM_RESERVED_CPM_0_0f, val64);

    COMPILER_64_SET(val64, 0, crc_enable?1:0);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_0r, &reg_val,
                      BRCM_RESERVED_CPM_0_1f, val64);

    _SOC_IF_ERR_EXIT(WRITE_BRCM_RESERVED_CPM_0r(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}


int cprif_drv_tx_brcm_rsvd5_config_get(int unit, int port,
                                       uint32* schan_start,
                                       uint32* schan_size,
                                       uint32* queue_num,
                                       int* crc_enable)
{
    uint64 reg_val;
    uint64 field_buf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_BRCM_RESERVED_CPM_0r(unit, port, &reg_val));
    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_0r, reg_val,
                      BRCM_RESERVED_CPM_0_3f);

    *schan_start = COMPILER_64_LO(field_buf);

    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_0r, reg_val,
                      BRCM_RESERVED_CPM_0_2f);
    *schan_size =  COMPILER_64_LO(field_buf);

    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_0r, reg_val,
                      BRCM_RESERVED_CPM_0_0f);
    *queue_num =  COMPILER_64_LO(field_buf);

    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_0r, reg_val,
                      BRCM_RESERVED_CPM_0_1f);
    if (!COMPILER_64_IS_ZERO(field_buf)) {
        *crc_enable = 1;
    } else {
        *crc_enable = 0;
    }
exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_gcw_config_set(int unit, int port,
                                uint32 index,
                                cprif_cpri_rx_gcw_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_GCW_CFG_TAB_CFG_0m,
                              CPRI_GCW_CFG_TAB_CFG_1m,
                              CPRI_GCW_CFG_TAB_CFG_2m,
                              CPRI_GCW_CFG_TAB_CFG_3m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = config->Ns;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_NSf, &field_buf);

    field_buf = config->Xs;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_XSf, &field_buf);

    field_buf = config->Y;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_Yf, &field_buf);


    field_buf = config->mask;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_MASKf, &field_buf);

    field_buf = config->filter_mode;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_FILTER_MODEf, &field_buf);


    field_buf = config->hfn_index;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_HFN_INDEXf, &field_buf);

    field_buf = config->hfn_modulo;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_HFN_MODULOf, &field_buf);

    field_buf = config->match_value;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_MATCH_VALUEf, &field_buf);

    field_buf = config->match_mask;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_MATCH_MASKf, &field_buf);



    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_gcw_config_get(int unit, int port,
                                uint32 index,
                                cprif_cpri_rx_gcw_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_GCW_CFG_TAB_CFG_0m,
                              CPRI_GCW_CFG_TAB_CFG_1m,
                              CPRI_GCW_CFG_TAB_CFG_2m,
                              CPRI_GCW_CFG_TAB_CFG_3m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_NSf, &field_buf);
    config->Ns = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_XSf, &field_buf);
    config->Xs = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_Yf, &field_buf);
    config->Y = field_buf;


    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_MASKf, &field_buf);
    config->mask = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_FILTER_MODEf, &field_buf);
    config->filter_mode = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_HFN_INDEXf, &field_buf);
    config->hfn_index = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_HFN_MODULOf, &field_buf);
    config->hfn_modulo = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_MATCH_VALUEf, &field_buf);
    config->match_value  = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_MATCH_MASKf, &field_buf);
    config->match_mask = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_gcw_config_set(int unit, int port,
                                uint32 index,
                                cprif_cpri_tx_gcw_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_CWA_GCW_CFG_TAB_CFG_0m,
                              CPRI_CWA_GCW_CFG_TAB_CFG_1m,
                              CPRI_CWA_GCW_CFG_TAB_CFG_2m,
                              CPRI_CWA_GCW_CFG_TAB_CFG_3m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = config->Ns;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_NSf, &field_buf);

    field_buf = config->Xs;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_XSf, &field_buf);

    field_buf = config->Y;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_Yf, &field_buf);


    field_buf = config->mask;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_MASKf, &field_buf);

    field_buf = config->repeat_mode;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_REPEAT_MODEf, &field_buf);

    field_buf = config->bfn0_filter_enable;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_BFN0_FILTER_MODEf, &field_buf);

    field_buf = config->bfn1_filter_enable;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_BFN1_FILTER_MODEf, &field_buf);


    field_buf = config->hfn_index;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_HFN_INDEXf, &field_buf);

    field_buf = config->hfn_modulo;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, GCW_HFN_MODULOf, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_gcw_config_get(int unit, int port,
                                uint32 index,
                                cprif_cpri_tx_gcw_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_CWA_GCW_CFG_TAB_CFG_0m,
                              CPRI_CWA_GCW_CFG_TAB_CFG_1m,
                              CPRI_CWA_GCW_CFG_TAB_CFG_2m,
                              CPRI_CWA_GCW_CFG_TAB_CFG_3m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_NSf, &field_buf);
    config->Ns = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_XSf, &field_buf);
    config->Xs = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_Yf, &field_buf);
    config->Y = field_buf;


    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_MASKf, &field_buf);
    config->mask = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_REPEAT_MODEf, &field_buf);
    config->repeat_mode = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_BFN0_FILTER_MODEf, &field_buf);
    config->bfn0_filter_enable = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_BFN1_FILTER_MODEf, &field_buf);
    config->bfn1_filter_enable = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_HFN_INDEXf, &field_buf);
    config->hfn_index = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_HFN_MODULOf, &field_buf);
    config->hfn_modulo = field_buf;


exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_gcw_filter_set(int unit, int port,
                                   uint32 bfn0_value, uint32 bfn0_mask,
                                   uint32 bfn1_value, uint32 bfn1_mask)
{
    uint64 gcw_ctrl, val64;

    COMPILER_64_ZERO(gcw_ctrl);
    COMPILER_64_SET(val64, 0, bfn0_value);
    soc_reg64_field_set(unit, CPRI_TXFRM_CWA_GCWr, &gcw_ctrl, TX_CWA_GCW_BFN0_VALUEf, val64);

    COMPILER_64_SET(val64, 0, bfn0_mask);
    soc_reg64_field_set(unit, CPRI_TXFRM_CWA_GCWr, &gcw_ctrl, TX_CWA_GCW_BFN0_MASKf, val64);

    COMPILER_64_SET(val64, 0, bfn1_value);
    soc_reg64_field_set(unit, CPRI_TXFRM_CWA_GCWr, &gcw_ctrl, TX_CWA_GCW_BFN1_VALUEf, val64);

    COMPILER_64_SET(val64, 0, bfn1_mask);
    soc_reg64_field_set(unit, CPRI_TXFRM_CWA_GCWr, &gcw_ctrl, TX_CWA_GCW_BFN1_MASKf, val64);

    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_CWA_GCWr(unit, port, gcw_ctrl));

    return SOC_E_NONE;
}

int cprif_drv_tx_gcw_filter_get(int unit, int port,
                                   uint32* bfn0_value, uint32* bfn0_mask,
                                   uint32* bfn1_value, uint32* bfn1_mask)
{
    uint64 gcw_ctrl, val64;

    COMPILER_64_ZERO(gcw_ctrl);
    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_CWA_GCWr(unit, port, &gcw_ctrl));

    val64 = soc_reg64_field_get(unit, CPRI_TXFRM_CWA_GCWr, gcw_ctrl, TX_CWA_GCW_BFN0_VALUEf);
    *bfn0_value = COMPILER_64_LO(val64);

    val64 = soc_reg64_field_get(unit, CPRI_TXFRM_CWA_GCWr, gcw_ctrl, TX_CWA_GCW_BFN0_MASKf);
    *bfn0_mask = COMPILER_64_LO(val64);

    val64 = soc_reg64_field_get(unit, CPRI_TXFRM_CWA_GCWr, gcw_ctrl, TX_CWA_GCW_BFN1_VALUEf);
    *bfn1_value = COMPILER_64_LO(val64);

    val64 = soc_reg64_field_get(unit, CPRI_TXFRM_CWA_GCWr, gcw_ctrl, TX_CWA_GCW_BFN1_MASKf);
    *bfn1_mask = COMPILER_64_LO(val64);

    return SOC_E_NONE;
}

int cprif_drv_tx_control_word_set(int unit, int port,
                                 cprif_drv_tx_control_word_t* control)
{
    uint32 reg_val;

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_CWA_L1r(unit, port, &reg_val));
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_L1r, &reg_val, TX_CWA_POINTER_Pf, control->enet_ptr );
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_L1r, &reg_val, TX_CWA_L1_FUNCf, control->l1_fun);
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_L1r, &reg_val, TX_CWA_HDLC_BITRATEf, control->hdlc_rate);
    soc_reg_field_set(unit, CPRI_TXFRM_CWA_L1r, &reg_val, TX_CWA_PROTOCOLf,  control->protocol_ver);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_CWA_L1r(unit, port, reg_val));
    return SOC_E_NONE;
}

int cprif_drv_tx_control_word_get(int unit, int port,
                                 cprif_drv_tx_control_word_t* control)
{
    uint32 reg_val;

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_CWA_L1r(unit, port, &reg_val));
    control->enet_ptr       = soc_reg_field_get(unit, CPRI_TXFRM_CWA_L1r, reg_val, TX_CWA_POINTER_Pf);
    control->l1_fun         = soc_reg_field_get(unit, CPRI_TXFRM_CWA_L1r, reg_val, TX_CWA_L1_FUNCf);
    control->hdlc_rate      = soc_reg_field_get(unit, CPRI_TXFRM_CWA_L1r, reg_val, TX_CWA_HDLC_BITRATEf);
    control->protocol_ver   = soc_reg_field_get(unit, CPRI_TXFRM_CWA_L1r, reg_val, TX_CWA_PROTOCOLf);
    return SOC_E_NONE;
}

int cprif_drv_tx_control_word_size_set(int unit, int port, uint32 cw_size_in_bytes)
{
    uint32 reg_val=0;

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_CWA_GENERALr(unit, port,
                                                     &reg_val));

    soc_reg_field_set(unit, CPRI_TXFRM_CWA_GENERALr, &reg_val, TX_CWA_TCWf,
                      cw_size_in_bytes);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_CWA_GENERALr(unit, port,
                                                      reg_val));

    return SOC_E_NONE;
}

int cprif_drv_tx_control_word_size_get(int unit, int port, uint32* cw_size_in_bytes)
{
    uint32 reg_val=0;

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_CWA_GENERALr(unit, port,
                                                     &reg_val));

    *cw_size_in_bytes = soc_reg_field_get(unit, CPRI_TXFRM_CWA_GENERALr, reg_val, TX_CWA_TCWf);

    return SOC_E_NONE;
}

/* RSVD4 */
int cprif_drv_rsvd4_rx_control_message_config_set( int unit, int port,
                                                   uint32 sm_queue_num,
                                                   uint32 default_tag,
                                                   uint32 no_match_tag)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_BRCM_RESERVED_CPM_300r(unit, port, &reg_val));

    soc_reg_field_set(unit, BRCM_RESERVED_CPM_300r, &reg_val,
                      BRCM_RESERVED_CPM_300_0f, sm_queue_num);

    _SOC_IF_ERR_EXIT(WRITE_BRCM_RESERVED_CPM_300r(unit, port, reg_val));


    _SOC_IF_ERR_EXIT(READ_BRCM_RESERVED_CPM_318r(unit, port, &reg_val));

    soc_reg_field_set(unit, BRCM_RESERVED_CPM_318r, &reg_val,
                      BRCM_RESERVED_CPM_318_1f,  default_tag);

    soc_reg_field_set(unit, BRCM_RESERVED_CPM_318r, &reg_val,
                      BRCM_RESERVED_CPM_318_2f, no_match_tag);

    _SOC_IF_ERR_EXIT(WRITE_BRCM_RESERVED_CPM_318r(unit, port, reg_val));
exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_rx_control_flow_config_set ( int unit, int port,
                                             uint32 control_flow_id,
                                             cprif_rsvd4_rx_ctrl_flow_config_t* config)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {BRCM_RESERVED_CPM_339m,
                              BRCM_RESERVED_CPM_340m,
                              BRCM_RESERVED_CPM_341m,
                              BRCM_RESERVED_CPM_342m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = config->proc_type;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_339_0f, &field_buf);

    field_buf = config->queue_num;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_339_1f, &field_buf);

    field_buf = config->sync_profile;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_339_2f, &field_buf);


    field_buf = config->sync_enable;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_339_3f, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, control_flow_id, wr_data_ptr));
exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_rx_control_flow_config_get( int unit, int port,
                                                uint32 control_flow_id,
                                                cprif_rsvd4_rx_ctrl_flow_config_t* config)

{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {BRCM_RESERVED_CPM_339m,
                              BRCM_RESERVED_CPM_340m,
                              BRCM_RESERVED_CPM_341m,
                              BRCM_RESERVED_CPM_342m};

    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num, control_flow_id, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_339_0f, &field_buf);
    config->proc_type = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_339_1f, &field_buf);
    config->queue_num = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_339_2f, &field_buf);
    config->sync_profile= field_buf;


    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_339_3f, &field_buf);
    config->sync_enable= field_buf;


exit:
    sal_free(rd_data_ptr);
   SOC_FUNC_RETURN;
}

int cprif_drv_rx_control_flow_tag_option_set(int unit, int port,
                                             uint32 control_flow_id,
                                             uint32 tag_option)
{
    uint32 reg_val;
    uint32 tag_opt;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_BRCM_RESERVED_CPM_318r(unit, port, &reg_val));

    tag_opt = soc_reg_field_get(unit, BRCM_RESERVED_CPM_318r, reg_val,
                      BRCM_RESERVED_CPM_318_0f);

    if (tag_option) {
        tag_opt |=  (0x1 << control_flow_id);
    } else {
        tag_opt &= ~(0x1 << control_flow_id);
    }

    soc_reg_field_set(unit, BRCM_RESERVED_CPM_318r, &reg_val,
                      BRCM_RESERVED_CPM_318_0f, tag_opt);

    _SOC_IF_ERR_EXIT(WRITE_BRCM_RESERVED_CPM_318r(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_sync_profile_entry_set (int unit, int port,
                                          uint32 profile_id,
                                          cprif_rsvd4_rx_sync_profile_entry_t* profile)
{
    uint64 reg_val;
    uint64 val64;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val);
    COMPILER_64_ZERO(val64);
    COMPILER_64_SET(val64, 0, profile->bfn_offset);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_319r, &reg_val,
                        BRCM_RESERVED_CPM_319_2f, val64);

    COMPILER_64_SET(val64, 0, profile->rfrm_offset);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_319r, &reg_val,
                        BRCM_RESERVED_CPM_319_1f, val64);

    COMPILER_64_SET(val64, 0, profile->count_cycle);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_319r, &reg_val,
                        BRCM_RESERVED_CPM_319_0f, val64);
    switch (profile_id) {

        case 0 :
            _SOC_IF_ERR_EXIT
                (WRITE_BRCM_RESERVED_CPM_319r(unit, port, reg_val));
            break;
        case 1 :
            _SOC_IF_ERR_EXIT
                (WRITE_BRCM_RESERVED_CPM_320r(unit, port, reg_val));
            break;
        case 2 :
            _SOC_IF_ERR_EXIT
                (WRITE_BRCM_RESERVED_CPM_321r(unit, port, reg_val));
            break;
        case 3 :
            _SOC_IF_ERR_EXIT
                (WRITE_BRCM_RESERVED_CPM_322r(unit, port, reg_val));
            break;
        default:
            LOG_CLI(("profile_id %d is out of range, between 0-3 ", profile_id));
            return SOC_E_PARAM;
            break;
    }
exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_rx_sync_profile_entry_get (int unit, int port,
                                          uint32 profile_id,
                                          cprif_rsvd4_rx_sync_profile_entry_t* profile)
{
    uint64 reg_val;
    uint64 field_buf;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_SET(reg_val, 0, 0);
    COMPILER_64_SET(field_buf, 0, 0);

    switch (profile_id) {

        case 0 :
            _SOC_IF_ERR_EXIT
                (READ_BRCM_RESERVED_CPM_319r(unit, port, &reg_val));
            break;
        case 1 :
            _SOC_IF_ERR_EXIT
                (READ_BRCM_RESERVED_CPM_320r(unit, port, &reg_val));
            break;
        case 2 :
            _SOC_IF_ERR_EXIT
                (READ_BRCM_RESERVED_CPM_321r(unit, port, &reg_val));
            break;
        case 3 :
            _SOC_IF_ERR_EXIT
                (READ_BRCM_RESERVED_CPM_322r(unit, port, &reg_val));
            break;
        default:
            LOG_CLI(("profile_id %d is out of range, between 0-3 ", profile_id));
            return SOC_E_PARAM;
            break;
    }
    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_319r, reg_val,
                        BRCM_RESERVED_CPM_319_2f);

    profile->bfn_offset = COMPILER_64_LO(field_buf);

    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_319r, reg_val,
                        BRCM_RESERVED_CPM_319_1f);
    profile->rfrm_offset = COMPILER_64_LO(field_buf);

    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_319r, reg_val,
                        BRCM_RESERVED_CPM_319_0f);
    profile->count_cycle = COMPILER_64_LO(field_buf);

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_rx_tag_config_set ( int unit, int port,
                                  uint32 default_tag,
                                  uint32 no_match_tag)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_BRCM_RESERVED_CPM_318r(unit, port, &reg_val));

    soc_reg_field_set(unit, BRCM_RESERVED_CPM_318r, &reg_val,
                      BRCM_RESERVED_CPM_318_1f, default_tag);

    soc_reg_field_set(unit, BRCM_RESERVED_CPM_318r, &reg_val,
                      BRCM_RESERVED_CPM_318_2f, no_match_tag);

    _SOC_IF_ERR_EXIT(WRITE_BRCM_RESERVED_CPM_318r(unit, port, reg_val));
exit:
    SOC_FUNC_RETURN;
}


int cprif_drv_rx_tag_gen_entry_set( int unit, int port,
                                    uint32 index,
                                    cprif_rx_tag_gen_entry_t* entry)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {BRCM_RESERVED_CPM_335m,
                              BRCM_RESERVED_CPM_336m,
                              BRCM_RESERVED_CPM_337m,
                              BRCM_RESERVED_CPM_338m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = entry->valid;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_335_0f, &field_buf);

    field_buf = entry->header;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_335_1f, &field_buf);

    field_buf = entry->mask;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_335_2f, &field_buf);


    field_buf = entry->tag_id;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_335_3f, &field_buf);

    field_buf = entry->rtwp_word_count;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_335_4f, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, index, wr_data_ptr));
exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_tag_gen_entry_get( int unit, int port,
                                    uint32 index,
                                    cprif_rx_tag_gen_entry_t* entry)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {BRCM_RESERVED_CPM_335m,
                              BRCM_RESERVED_CPM_336m,
                              BRCM_RESERVED_CPM_337m,
                              BRCM_RESERVED_CPM_338m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_335_0f, &field_buf);
    entry->valid = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_335_1f, &field_buf);
    entry->header = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_335_2f, &field_buf);
    entry->mask = field_buf;


    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_335_3f, &field_buf);
    entry->tag_id = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_335_4f, &field_buf);
    entry->rtwp_word_count = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_brcm_rsvd4_tx_control_group_entry_set( int unit, int port,
                                                  uint32 index,
                                                  cprif_control_group_entry_t* entry)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {BRCM_RESERVED_CPM_56m,
                              BRCM_RESERVED_CPM_57m,
                              BRCM_RESERVED_CPM_58m,
                              BRCM_RESERVED_CPM_59m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = entry->valid_mask;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_56_8f , &field_buf);

    field_buf = entry->queue_num[0];
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_56_7f, &field_buf);

    field_buf = entry->queue_num[1];
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_56_6f, &field_buf);

    field_buf = entry->queue_num[2];
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_56_5f, &field_buf);

    field_buf = entry->queue_num[3];
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_56_4f, &field_buf);

    field_buf = entry->proc_type[0];
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_56_3f, &field_buf);

    field_buf = entry->proc_type[1];
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_56_2f, &field_buf);

    field_buf = entry->proc_type[2];
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_56_1f, &field_buf);

    field_buf = entry->proc_type[3];
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_56_0f, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, index, wr_data_ptr));
exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_brcm_rsvd4_tx_control_group_entry_get( int unit, int port,
                                                  uint32 index,
                                                  cprif_control_group_entry_t* entry)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {BRCM_RESERVED_CPM_56m,
                              BRCM_RESERVED_CPM_57m,
                              BRCM_RESERVED_CPM_58m,
                              BRCM_RESERVED_CPM_59m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_56_8f, &field_buf);
    entry->valid_mask = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_56_7f, &field_buf);
    entry->queue_num[0] = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_56_6f, &field_buf);
    entry->queue_num[1] = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_56_5f, &field_buf);
    entry->queue_num[2] = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_56_4f, &field_buf);
    entry->queue_num[3] = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_56_3f, &field_buf);
    entry->proc_type[0] = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_56_2f, &field_buf);
    entry->proc_type[1] = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_56_1f, &field_buf);
    entry->proc_type[2] = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_56_0f, &field_buf);
    entry->proc_type[3] = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_control_eth_msg_config_set(int unit, int port,
                                            uint32 msg_node,
                                            uint32 msg_subnode,
                                            uint32 msg_type,
                                            uint32 msg_padding)
{
  uint64 reg_val, val64;

   SOC_IF_ERROR_RETURN
        (READ_BRCM_RESERVED_CPM_5r(unit, port, &reg_val));

    COMPILER_64_SET(val64, 0, msg_node);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_5r, &reg_val, BRCM_RESERVED_CPM_5_3f, val64);
    COMPILER_64_SET(val64, 0,msg_subnode);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_5r, &reg_val, BRCM_RESERVED_CPM_5_2f, val64);
    COMPILER_64_SET(val64, 0, msg_type);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_5r, &reg_val, BRCM_RESERVED_CPM_5_1f, val64);
    COMPILER_64_SET(val64, 0, msg_padding);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_5r, &reg_val, BRCM_RESERVED_CPM_5_0f, val64);
    SOC_IF_ERROR_RETURN
        (WRITE_BRCM_RESERVED_CPM_5r(unit, port, reg_val));

    return SOC_E_NONE;
}

int cprif_drv_tx_control_eth_msg_config_get(int unit, int port,
                                            uint32* msg_node,
                                            uint32* msg_subnode,
                                            uint32* msg_type,
                                            uint32* msg_padding)
{
  uint64 reg_val;
  uint64 field_buf;

   SOC_IF_ERROR_RETURN
        (READ_BRCM_RESERVED_CPM_5r(unit, port, &reg_val));

    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_5r, reg_val, BRCM_RESERVED_CPM_5_3f);
    *msg_node = COMPILER_64_LO(field_buf);
    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_5r, reg_val, BRCM_RESERVED_CPM_5_2f);
    *msg_subnode = COMPILER_64_LO(field_buf);
    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_5r, reg_val, BRCM_RESERVED_CPM_5_1f);
    *msg_type=COMPILER_64_LO(field_buf);
    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_5r, reg_val, BRCM_RESERVED_CPM_5_0f);
    *msg_padding =COMPILER_64_LO(field_buf);

    return SOC_E_NONE;
}

int cprif_drv_tx_control_single_msg_config_set(int unit, int port,
                                            uint32 msg_id,
                                            uint32 msg_type)
{
  uint64 reg_val, val64;

   SOC_IF_ERROR_RETURN
        (READ_BRCM_RESERVED_CPM_5r(unit, port, &reg_val));

    COMPILER_64_SET(val64, 0, msg_id);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_5r, &reg_val, BRCM_RESERVED_CPM_5_4f, val64);
    COMPILER_64_SET(val64, 0, msg_type);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_5r, &reg_val, BRCM_RESERVED_CPM_5_5f, val64);
    SOC_IF_ERROR_RETURN
        (WRITE_BRCM_RESERVED_CPM_5r(unit, port, reg_val));

    return SOC_E_NONE;
}

int cprif_drv_tx_control_single_tunnel_msg_config_set(int unit, int port,
                                            uint32 crc_option)
{
  uint64 reg_val, val64;

   SOC_IF_ERROR_RETURN
        (READ_BRCM_RESERVED_CPM_5r(unit, port, &reg_val));

    COMPILER_64_SET(val64, 0, crc_option);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_5r, &reg_val, BRCM_RESERVED_CPM_5_6f, val64);
    SOC_IF_ERROR_RETURN
        (WRITE_BRCM_RESERVED_CPM_5r(unit, port, reg_val));

    return SOC_E_NONE;
}

int cprif_drv_tx_control_single_msg_config_get(int unit, int port,
                                            uint32* msg_id,
                                            uint32* msg_type)
{
  uint64 reg_val;
  uint64 field_buf;

   SOC_IF_ERROR_RETURN
        (READ_BRCM_RESERVED_CPM_5r(unit, port, &reg_val));

    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_5r, reg_val, BRCM_RESERVED_CPM_5_4f);
    *msg_id = COMPILER_64_LO(field_buf);
    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_5r, reg_val, BRCM_RESERVED_CPM_5_5f);
    *msg_type = COMPILER_64_LO(field_buf);

    return SOC_E_NONE;
}


int cprif_drv_brcm_rsvd4_tx_control_flow_config_set( int unit, int port,
                                                 uint32 flow_id,
                                                 uint32 queue_num,
                                                 uint32 proc_type)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {BRCM_RESERVED_CPM_52m,
                              BRCM_RESERVED_CPM_53m,
                              BRCM_RESERVED_CPM_54m,
                              BRCM_RESERVED_CPM_55m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);


    field_buf = queue_num;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_52_0f, &field_buf);


    field_buf = proc_type;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_52_1f,&field_buf);


    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, flow_id, wr_data_ptr));
exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_brcm_rsvd4_tx_control_flow_config_get( int unit, int port,
                                                 uint32 flow_id,
                                                 uint32* queue_num,
                                                 uint32* proc_type)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {BRCM_RESERVED_CPM_52m,
                              BRCM_RESERVED_CPM_53m,
                              BRCM_RESERVED_CPM_54m,
                              BRCM_RESERVED_CPM_55m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num, flow_id, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_52_0f, &field_buf);
    *queue_num = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_52_1f, &field_buf);
    *proc_type = field_buf;


exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_brcm_rsvd4_control_flow_header_index_set( int unit, int port,
                                                        uint32 roe_flow_id,
                                                        uint32 header_index)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {BRCM_RESERVED_CPM_44m,
                              BRCM_RESERVED_CPM_45m,
                              BRCM_RESERVED_CPM_46m,
                              BRCM_RESERVED_CPM_47m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);


    field_buf = header_index;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_44_0f, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, roe_flow_id, wr_data_ptr));
exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_brcm_rsvd4_control_flow_header_index_get( int unit, int port,
                                                        uint32 roe_flow_id,
                                                        uint32* header_index)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {BRCM_RESERVED_CPM_44m,
                              BRCM_RESERVED_CPM_45m,
                              BRCM_RESERVED_CPM_46m,
                              BRCM_RESERVED_CPM_47m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num, roe_flow_id, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_44_0f, &field_buf);
    *header_index= field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_brcm_rsvd4_control_header_entry_set( int unit, int port,
                                                   uint32 index,
                                                   uint32 header_node,
                                                   uint32 header_subnode,
                                                   uint32 node)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {BRCM_RESERVED_CPM_48m,
                              BRCM_RESERVED_CPM_49m,
                              BRCM_RESERVED_CPM_50m,
                              BRCM_RESERVED_CPM_51m};
    uint32 *wr_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    field_buf = (header_node << 5)|header_subnode;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_48_1f, &field_buf);

    field_buf = node;
    soc_mem_field_set(unit, local_mem, wr_data_ptr, BRCM_RESERVED_CPM_48_0f, &field_buf);

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk_num, index, wr_data_ptr));
exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_brcm_rsvd4_control_header_entry_get( int unit, int port,
                                                   uint32 index,
                                                   uint32* header_node,
                                                   uint32* header_subnode,
                                                   uint32* node)
{
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {BRCM_RESERVED_CPM_48m,
                              BRCM_RESERVED_CPM_49m,
                              BRCM_RESERVED_CPM_50m,
                              BRCM_RESERVED_CPM_51m};
    uint32 *rd_data_ptr;
    uint32 field_buf;
    uint32 num_words;
    uint32 blk_num=0;
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_48_1f, &field_buf);
    *header_node = field_buf >> 5;
    *header_subnode = field_buf & 0x1F;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, BRCM_RESERVED_CPM_48_1f, &field_buf);
    *node = field_buf;
exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_stuff_rsrv_bits_set( int unit, int port,
                                   int roe_stuffing_bit,
                                   int roe_reserved_bit)
{
    uint32 data32;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_TXFRM_GENERAL_CTRLr(unit, port,
                                                      &data32));
    soc_reg_field_set(unit, CPRI_TXFRM_GENERAL_CTRLr, &data32, CA_STUFF_BITf,
                      roe_stuffing_bit);
    soc_reg_field_set(unit, CPRI_TXFRM_GENERAL_CTRLr, &data32, BFA_RSRV_BITf,
                      roe_reserved_bit);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_TXFRM_GENERAL_CTRLr(unit, port,
                                                       data32));

    SOC_FUNC_RETURN;
}

int cprif_drv_pmd_reset_seq(int unit, int port, int pmd_touched)
{
    int val;
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    if(pmd_touched == 0) {
        SOC_IF_ERROR_RETURN(cprif_drv_cip_top_ctrl_get(unit, port, &val));
        if(val == 0) {
            SOC_IF_ERROR_RETURN(READ_CLPORT_XGXS0_CTRL_REGr(unit, port, &reg_val));
            soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, RSTB_HWf, 1);
            soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, PWRDWNf, 0);
            soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, IDDQf, 0);
            SOC_IF_ERROR_RETURN(WRITE_CLPORT_XGXS0_CTRL_REGr(unit, port, reg_val));
        } else {
            /* removing iddq from cpri side */
            SOC_IF_ERROR_RETURN(READ_CLPORT_XGXS0_CTRL_REGr(unit, port, &reg_val));
            soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, RSTB_HWf, 1);
            soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, PWRDWNf, 0);
            soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, IDDQf, 0);
            SOC_IF_ERROR_RETURN(WRITE_CLPORT_XGXS0_CTRL_REGr(unit, port, reg_val));
            SOC_IF_ERROR_RETURN(cprif_drv_pmd_iddq_set(unit, port, 0));
        }
    }
    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_port_interrupt_enable_set(int unit, int port,
                                             int int_dir, int enable)
{
    uint32 rval = 0, new_rval = 0;
    soc_field_t rx_fields[4] = {
                                PORT_0_RX_INTERRUPT_ENABLEf,
                                PORT_1_RX_INTERRUPT_ENABLEf,
                                PORT_2_RX_INTERRUPT_ENABLEf,
                                PORT_3_RX_INTERRUPT_ENABLEf
                               };
    soc_field_t tx_fields[4] = {
                                PORT_0_TX_INTERRUPT_ENABLEf,
                                PORT_1_TX_INTERRUPT_ENABLEf,
                                PORT_2_TX_INTERRUPT_ENABLEf,
                                PORT_3_TX_INTERRUPT_ENABLEf
                               };
    soc_field_t rx_field = INVALIDf;
    soc_field_t tx_field = INVALIDf;
    int phy_port = 0;
    int port_index = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_INTR_ENABLEr(unit, port, &rval));

    new_rval = rval;
    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_index = ((phy_port-1)%4);

    if (int_dir & CPRIF_RX_INTR) {
        rx_field = rx_fields[port_index];
        soc_reg_field_set(unit, CPRI_INTR_ENABLEr, &new_rval, rx_field, enable);
    }

    if (int_dir & CPRIF_TX_INTR) {
        tx_field = tx_fields[port_index];
        soc_reg_field_set(unit, CPRI_INTR_ENABLEr, &new_rval, tx_field, enable);
    }

    if (rx_field != INVALIDf || tx_field != INVALIDf) {
        if (rval != new_rval) {
            _SOC_IF_ERR_EXIT(WRITE_CPRI_INTR_ENABLEr(unit, port, new_rval));
        }
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Unknown interrupt type"));
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_port_interrupt_enable_get(int unit, int port,
                                             int int_dir, int *enable)
{
    uint32 rval;
    soc_field_t rx_fields[4] = {
                                PORT_0_RX_INTERRUPT_ENABLEf,
                                PORT_1_RX_INTERRUPT_ENABLEf,
                                PORT_2_RX_INTERRUPT_ENABLEf,
                                PORT_3_RX_INTERRUPT_ENABLEf
                               };
    soc_field_t tx_fields[4] = {
                                PORT_0_TX_INTERRUPT_ENABLEf,
                                PORT_1_TX_INTERRUPT_ENABLEf,
                                PORT_2_TX_INTERRUPT_ENABLEf,
                                PORT_3_TX_INTERRUPT_ENABLEf
                               };
    soc_field_t field = INVALIDf;
    int phy_port = 0;
    int port_index = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_INTR_ENABLEr(unit, port, &rval));

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_index = ((phy_port-1)%4);

    switch (int_dir) {
        case CPRIF_RX_INTR:
             field = rx_fields[port_index];
             break;
        case CPRIF_TX_INTR:
             field = tx_fields[port_index];
             break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                               ("Unknown interrupt type"));
            break;
    }

    *enable = soc_reg_field_get(unit, CPRI_INTR_ENABLEr, rval, field);

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_port_interrupt_status_get(int unit, int port,
                                             int *status)
{
    uint32 rval;
    soc_field_t fields[8] = {
                                PORT_0_RX_INTERRUPT_STATUSf,
                                PORT_1_RX_INTERRUPT_STATUSf,
                                PORT_2_RX_INTERRUPT_STATUSf,
                                PORT_3_RX_INTERRUPT_STATUSf,
                                PORT_0_TX_INTERRUPT_STATUSf,
                                PORT_1_TX_INTERRUPT_STATUSf,
                                PORT_2_TX_INTERRUPT_STATUSf,
                                PORT_3_TX_INTERRUPT_STATUSf
                            };
    uint32 port_intr_status[8] = {
                                    CPRIF_PORT_0_RX_INTR,
                                    CPRIF_PORT_1_RX_INTR,
                                    CPRIF_PORT_2_RX_INTR,
                                    CPRIF_PORT_3_RX_INTR,
                                    CPRIF_PORT_0_TX_INTR,
                                    CPRIF_PORT_1_TX_INTR,
                                    CPRIF_PORT_2_TX_INTR,
                                    CPRIF_PORT_3_TX_INTR
                                 };
    int i = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_INTR_STATUSr(unit, port, &rval));


    for (i = 0; i < sizeof(fields)/sizeof(soc_field_t); i++) {
        if (soc_reg_field_get(unit, CPRI_INTR_STATUSr, rval, fields[i])) {
            *status |= port_intr_status[i];
        }
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * Enable CPRI port RX interrupts.
 * RX interrupts can be of 3 types
 * 1. RX operational error type interrupts.
 * 2. RX functional error type interrupts.
 * 3. RX memory ECC error type interrupts.
 */
int cprif_drv_cpri_port_rx_interrupts_enable_set(int unit,
                                                 int port,
                                                 int rx_intr_flags,
                                                 int enable)
{
    uint32 rval, new_rval;
    uint32 rx_op_err = 0;
    uint32 rx_func_err = 0;
    uint32 rx_ecc_err = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_RX_INTR_ENABLEr(unit, port, &rval));

    new_rval = rval;
    if (rx_intr_flags & CPRIF_RX_OP_ERR_INTR) {
        rx_op_err = soc_reg_field_get(unit, CPRI_RX_INTR_ENABLEr,
                                      rval, RX_OP_ERR_INTERRUPT_ENABLEf);
        if (enable != rx_op_err) {
            soc_reg_field_set(unit, CPRI_RX_INTR_ENABLEr, &new_rval,
                              RX_OP_ERR_INTERRUPT_ENABLEf, enable);
        }
    }

    if (rx_intr_flags & CPRIF_RX_FUNC_ERR_INTR) {
        rx_func_err = soc_reg_field_get(unit, CPRI_RX_INTR_ENABLEr,
                                      rval, RX_FUNC_INTERRUPT_ENABLEf);
        if (enable != rx_func_err) {
            soc_reg_field_set(unit, CPRI_RX_INTR_ENABLEr, &new_rval,
                              RX_FUNC_INTERRUPT_ENABLEf, enable);
        }
    }

    if (rx_intr_flags & CPRIF_RX_ECC_ERR_INTR) {
        rx_ecc_err = soc_reg_field_get(unit, CPRI_RX_INTR_ENABLEr,
                                      rval, RX_ECC_INTERRUPT_ENABLEf);
        if (enable != rx_ecc_err) {
            soc_reg_field_set(unit, CPRI_RX_INTR_ENABLEr, &new_rval,
                              RX_ECC_INTERRUPT_ENABLEf, enable);
        }
    }

    if (new_rval != rval) {
        _SOC_IF_ERR_EXIT(WRITE_CPRI_RX_INTR_ENABLEr(unit, port, new_rval));
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * Get CPRI port RX interrupts enable status.
 */
int cprif_drv_cpri_port_rx_interrupt_enable_get(int unit,
                                                int port,
                                                int rx_intr_flags,
                                                int *status)
{
    uint32 rval;
    soc_field_t field = INVALIDf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_RX_INTR_ENABLEr(unit, port, &rval));

    switch (rx_intr_flags) {
        case CPRIF_RX_OP_ERR_INTR:
            field = RX_OP_ERR_INTERRUPT_ENABLEf;
            break;
        case CPRIF_RX_FUNC_ERR_INTR:
            field = RX_FUNC_INTERRUPT_ENABLEf;
            break;
        case CPRIF_RX_ECC_ERR_INTR:
            field = RX_ECC_INTERRUPT_ENABLEf;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                               ("Unknown Rx interrupt type"));
            break;
    }

    if (field != INVALIDf) {
        *status = soc_reg_field_get(unit, CPRI_RX_INTR_ENABLEr, rval, field);
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * Enable CPRI port RX Operational error interrupts.
 * RX Lane Operational Error Interrupts.
 * 1. RX Lane Combined DATA Queue Overflow Error
 * 2. RX Lane Combined Control Queue Overflow Error
 * 3. RX Lane Combined GSM Time-stamp Error
 * 4. RX Lane MISC WQ and CDC Overflow Error
 * 5. RX RX Lane RxFramer BRCM_RSVD4 Time-stamp
 *    detection Error
 */
int cprif_drv_cpri_port_rx_op_err_interrupts_enable_set(int unit,
                                                      int port,
                                                      int rx_op_err_intr_flags,
                                                      int enable)
{
    uint32 rval, new_rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_RX_OP_ERR_INTR_ENABLEr(unit, port, &rval));

    new_rval = rval;
    if (rx_op_err_intr_flags & CPRI_INTR_RX_ENCAP_DATA_Q_OVRFLOW) {
        if(!(soc_reg_field_get(unit, CPRI_RX_OP_ERR_INTR_ENABLEr,
                               new_rval,
                               RX_ENCAP_DATA_Q_OVFL_ERR_INTERRUPT_ENABLEf))) {
            soc_reg_field_set(unit, CPRI_RX_OP_ERR_INTR_ENABLEr, &new_rval,
                              RX_ENCAP_DATA_Q_OVFL_ERR_INTERRUPT_ENABLEf,
                              enable);
        }
    }

    if (rx_op_err_intr_flags & CPRI_INTR_RX_ENCAP_CTRL_Q_OVRFLOW) {
        if(!(soc_reg_field_get(unit, CPRI_RX_OP_ERR_INTR_ENABLEr,
                               new_rval,
                               RX_ENCAP_CTRL_Q_OVFL_ERR_INTERRUPT_ENABLEf))) {
            soc_reg_field_set(unit, CPRI_RX_OP_ERR_INTR_ENABLEr, &new_rval,
                              RX_ENCAP_CTRL_Q_OVFL_ERR_INTERRUPT_ENABLEf,
                              enable);
        }
    }

    if (rx_op_err_intr_flags & CPRI_INTR_RX_ENCAP_GSM_TIMESTAMP) {
        if(!(soc_reg_field_get(unit, CPRI_RX_OP_ERR_INTR_ENABLEr,
                               new_rval, BRCM_RESERVED_CPM_301_2f))) {
            soc_reg_field_set(unit, CPRI_RX_OP_ERR_INTR_ENABLEr, &new_rval,
                              BRCM_RESERVED_CPM_301_2f, enable);
        }
    }

    if (rx_op_err_intr_flags & CPRI_INTR_RX_ENCAP_MISC_ERROR) {
        if(!(soc_reg_field_get(unit, CPRI_RX_OP_ERR_INTR_ENABLEr,
                               new_rval, RX_ENCAP_MISC_INTERRUPT_ENABLEf))) {
            soc_reg_field_set(unit, CPRI_RX_OP_ERR_INTR_ENABLEr, &new_rval,
                              RX_ENCAP_MISC_INTERRUPT_ENABLEf, enable);
        }
    }

    if (rx_op_err_intr_flags & CPRI_INTR_RX_ENCAP_RSVD4_EVENTS) {
        if(!(soc_reg_field_get(unit, CPRI_RX_OP_ERR_INTR_ENABLEr,
                               new_rval, BRCM_RESERVED_CPM_301_3f))) {
            soc_reg_field_set(unit, CPRI_RX_OP_ERR_INTR_ENABLEr, &new_rval,
                              BRCM_RESERVED_CPM_301_3f, enable);
        }
    }

    if (new_rval != rval) {
        _SOC_IF_ERR_EXIT(WRITE_CPRI_RX_OP_ERR_INTR_ENABLEr(unit, port,
                                                           new_rval));
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * Enable CPRI port RX Operational error interrupts.
 * RX Lane Operational Error Interrupts.
 * 1. RX Lane Combined DATA Queue Overflow Error
 * 2. RX Lane Combined Control Queue Overflow Error
 * 3. RX Lane Combined GSM Time-stamp Error
 * 4. RX Lane MISC WQ and CDC Overflow Error
 * 5. RX RX Lane RxFramer BRCM_RSVD4 Time-stamp
 *    detection Error
 */
int cprif_drv_cpri_port_rx_op_err_interrupts_enable_get(int unit,
                                                  int port,
                                                  int rx_op_err_intr_flags,
                                                  int *status)
{
    uint32 rval;
    soc_field_t field = INVALIDf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_RX_OP_ERR_INTR_ENABLEr(unit, port, &rval));

    switch(rx_op_err_intr_flags) {
        case CPRI_INTR_RX_ENCAP_DATA_Q_OVRFLOW:
            field = RX_ENCAP_DATA_Q_OVFL_ERR_INTERRUPT_ENABLEf;
            break;
        case CPRI_INTR_RX_ENCAP_CTRL_Q_OVRFLOW:
            field = RX_ENCAP_CTRL_Q_OVFL_ERR_INTERRUPT_ENABLEf;
            break;
        case CPRI_INTR_RX_ENCAP_GSM_TIMESTAMP:
            field = BRCM_RESERVED_CPM_301_2f;
            break;
        case CPRI_INTR_RX_ENCAP_MISC_ERROR:
            field = RX_ENCAP_MISC_INTERRUPT_ENABLEf;
            break;
        case CPRI_INTR_RX_ENCAP_RSVD4_EVENTS:
            field = BRCM_RESERVED_CPM_301_3f;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                               ("Unknown Rx interrupt type"));
            break;
    }

    if (field != INVALIDf) {
        *status = soc_reg_field_get(unit, CPRI_RX_OP_ERR_INTR_ENABLEr, rval,
                                    field);
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * Enable CPRI port RX Functional interrupts.
 * RX Functional interrupts can be of 3 types
 * 1. RX
 * 2. RX functional error type interrupts.
 * 3. RX memory ECC error type interrupts.
 */
int cprif_drv_cpri_port_rx_func_interrupts_enable_set(int unit,
                                                      int port,
                                                      int rx_func_intr_flags,
                                                      int enable)
{
    uint32 rval, new_rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_RX_FUNC_INTR_ENABLEr(unit, port, &rval));

    new_rval = rval;
    if (rx_func_intr_flags & CPRI_INTR_RXFRM_CW_PARSER) {
        if(!(soc_reg_field_get(unit, CPRI_RX_FUNC_INTR_ENABLEr,
                               new_rval, RX_RXFRM_CPRSR_INTERRUPT_ENABLEf))) {
            soc_reg_field_set(unit, CPRI_RX_FUNC_INTR_ENABLEr, &new_rval,
                              RX_RXFRM_CPRSR_INTERRUPT_ENABLEf, enable);
        }
    }

    if (rx_func_intr_flags & CPRI_INTR_RX_1588_SW) {
        if(!(soc_reg_field_get(unit, CPRI_RX_FUNC_INTR_ENABLEr,
                               new_rval, RX_1588_SW_INTERRUPT_ENABLEf))) {
            soc_reg_field_set(unit, CPRI_RX_FUNC_INTR_ENABLEr, &new_rval,
                              RX_1588_SW_INTERRUPT_ENABLEf, enable);
        }
    }

    if (rx_func_intr_flags & CPRI_INTR_RX_1588_FIFO) {
        if(!(soc_reg_field_get(unit, CPRI_RX_FUNC_INTR_ENABLEr,
                               new_rval, RX_1588_FIFO_INTERRUPT_ENABLEf))) {
            soc_reg_field_set(unit, CPRI_RX_FUNC_INTR_ENABLEr, &new_rval,
                              RX_1588_FIFO_INTERRUPT_ENABLEf, enable);
        }
    }

    if (rx_func_intr_flags & CPRI_INTR_RXFRM_GCW) {
        if(!(soc_reg_field_get(unit, CPRI_RX_FUNC_INTR_ENABLEr,
                               new_rval, RX_RXFRM_GCW_INTERRUPT_ENABLEf))) {
            soc_reg_field_set(unit, CPRI_RX_FUNC_INTR_ENABLEr, &new_rval,
                              RX_RXFRM_GCW_INTERRUPT_ENABLEf, enable);
        }
    }

    if (rx_func_intr_flags & CPRI_INTR_RXPCS) {
        if(!(soc_reg_field_get(unit, CPRI_RX_FUNC_INTR_ENABLEr,
                               new_rval, RX_RXPCS_INTERRUPT_ENABLEf))) {
            soc_reg_field_set(unit, CPRI_RX_FUNC_INTR_ENABLEr, &new_rval,
                              RX_RXPCS_INTERRUPT_ENABLEf, enable);
        }
    }

    if (rx_func_intr_flags & CPRI_INTR_RXPCS_FEC) {
        if(!(soc_reg_field_get(unit, CPRI_RX_FUNC_INTR_ENABLEr,
                               new_rval, RX_RXPCS_FEC_INTERRUPT_ENABLEf))) {
            soc_reg_field_set(unit, CPRI_RX_FUNC_INTR_ENABLEr, &new_rval,
                              RX_RXPCS_FEC_INTERRUPT_ENABLEf, enable);
        }
    }

    if (new_rval != rval) {
        _SOC_IF_ERR_EXIT(WRITE_CPRI_RX_FUNC_INTR_ENABLEr(unit, port, new_rval));
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * Get CPRI port RX functional interrupts enable status.
 */
int cprif_drv_cpri_port_rx_func_interrupts_enable_get(int unit,
                                                      int port,
                                                      int rx_func_intr_flags,
                                                      int *status)
{
    uint32 rval;
    soc_field_t field = INVALIDf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_RX_FUNC_INTR_ENABLEr(unit, port, &rval));

    switch(rx_func_intr_flags) {
        case CPRI_INTR_RXFRM_CW_PARSER:
            field = RX_RXFRM_CPRSR_INTERRUPT_ENABLEf;
            break;
        case CPRI_INTR_RX_1588_SW:
            field = RX_1588_SW_INTERRUPT_ENABLEf;
            break;
        case CPRI_INTR_RX_1588_FIFO:
            field = RX_1588_FIFO_INTERRUPT_ENABLEf;
            break;
        case CPRI_INTR_RXFRM_GCW:
            field = RX_RXFRM_GCW_INTERRUPT_ENABLEf;
            break;
        case CPRI_INTR_RXPCS:
            field = RX_RXPCS_INTERRUPT_ENABLEf;
            break;
        case CPRI_INTR_RXPCS_FEC:
            field = RX_RXPCS_FEC_INTERRUPT_ENABLEf;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                               ("Unknown Rx interrupt type"));
            break;
    }

    if (field != INVALIDf) {
        *status = soc_reg_field_get(unit, CPRI_RX_FUNC_INTR_ENABLEr,
                                    rval, field);
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * Enable CPRI port TX interrupts.
 * TX interrupts can be of 3 types
 * 1. TX operational error type interrupts.
 * 2. TX functional error type interrupts.
 * 3. TX memory ECC error type interrupts.
 */
int cprif_drv_cpri_port_tx_interrupts_enable_set(int unit,
                                                 int port,
                                                 int tx_intr_flags,
                                                 int enable)
{
    uint32 rval, new_rval;
    uint32 tx_op_err = 0;
    uint32 tx_func_err = 0;
    uint32 tx_ecc_err = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_TX_INTR_ENABLEr(unit, port, &rval));

    new_rval = rval;
    if (tx_intr_flags & CPRIF_TX_OP_ERR_INTR) {
        tx_op_err = soc_reg_field_get(unit, CPRI_TX_INTR_ENABLEr,
                                      rval, TX_OP_ERR_INTERRUPT_ENABLEf);
        if (enable != tx_op_err) {
            soc_reg_field_set(unit, CPRI_TX_INTR_ENABLEr, &new_rval,
                              TX_OP_ERR_INTERRUPT_ENABLEf, enable);
        }
    }

    if (tx_intr_flags & CPRIF_TX_FUNC_ERR_INTR) {
        tx_func_err = soc_reg_field_get(unit, CPRI_TX_INTR_ENABLEr,
                                      rval, TX_FUNC_INTERRUPT_ENABLEf);
        if (enable != tx_func_err) {
            soc_reg_field_set(unit, CPRI_TX_INTR_ENABLEr, &new_rval,
                              TX_FUNC_INTERRUPT_ENABLEf, enable);
        }
    }

    if (tx_intr_flags & CPRIF_TX_ECC_ERR_INTR) {
        tx_ecc_err = soc_reg_field_get(unit, CPRI_TX_INTR_ENABLEr,
                                      rval, TX_ECC_INTERRUPT_ENABLEf);
        if (enable != tx_ecc_err) {
            soc_reg_field_set(unit, CPRI_TX_INTR_ENABLEr, &new_rval,
                              TX_ECC_INTERRUPT_ENABLEf, enable);
        }
    }

    if (new_rval != rval) {
        _SOC_IF_ERR_EXIT(WRITE_CPRI_TX_INTR_ENABLEr(unit, port, new_rval));
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * Get CPRI port TX interrupts enable status.
 */
int cprif_drv_cpri_port_tx_interrupt_enable_get(int unit,
                                                int port,
                                                int tx_intr_flags,
                                                int *status)
{
    uint32 rval;
    soc_field_t field = INVALIDf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_TX_INTR_ENABLEr(unit, port, &rval));

    switch (tx_intr_flags) {
        case CPRIF_TX_OP_ERR_INTR:
            field = TX_OP_ERR_INTERRUPT_ENABLEf;
            break;
        case CPRIF_TX_FUNC_ERR_INTR:
            field = TX_FUNC_INTERRUPT_ENABLEf;
            break;
        case CPRIF_TX_ECC_ERR_INTR:
            field = TX_ECC_INTERRUPT_ENABLEf;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                               ("Unknown Rx interrupt type"));
            break;
    }

    if (field != INVALIDf) {
        *status = soc_reg_field_get(unit, CPRI_TX_INTR_ENABLEr, rval, field);
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * Enable CPRI port TX Functional interrupts.
 * TX Functional interrupts can be of 3 types
 * 1. TX
 * 2. TX functional error type interrupts.
 * 3. TX memory ECC error type interrupts.
 */
int cprif_drv_cpri_port_tx_op_err_interrupts_enable_set(int unit,
                                                      int port,
                                                      int tx_op_err_intr_flags,
                                                      int enable)
{
    uint32 rval, new_rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_TX_OP_ERR_INTR_ENABLEr(unit, port, &rval));

    new_rval = rval;

    if (tx_op_err_intr_flags & CPRI_INTR_TX_DECAP_DATA_Q_OVRFLOW) {
        if(!(soc_reg_field_get(unit, CPRI_TX_OP_ERR_INTR_ENABLEr,
                          new_rval, TX_DECAP_DATA_Q_OVFL_ERR_INTERRUPT_ENABLEf))) {
            soc_reg_field_set(unit, CPRI_TX_OP_ERR_INTR_ENABLEr, &new_rval,
                          TX_DECAP_DATA_Q_OVFL_ERR_INTERRUPT_ENABLEf, enable);
        }
    }

    if (tx_op_err_intr_flags & CPRI_INTR_TX_DECAP_DATA_Q_UNDRFLOW) {
        if(!(soc_reg_field_get(unit, CPRI_TX_OP_ERR_INTR_ENABLEr,
                           new_rval, TX_DECAP_DATA_Q_UDFL_ERR_INTERRUPT_ENABLEf))) {
            soc_reg_field_set(unit, CPRI_TX_OP_ERR_INTR_ENABLEr, &new_rval,
                           TX_DECAP_DATA_Q_UDFL_ERR_INTERRUPT_ENABLEf, enable);
        }
    }

    if (tx_op_err_intr_flags & CPRI_INTR_TX_DECAP_DATA_Q_BUFFSIZE_ERR) {
        if(!(soc_reg_field_get(unit, CPRI_TX_OP_ERR_INTR_ENABLEr,
                      new_rval, TX_DECAP_DATA_Q_BUFFSIZE_ERR_INTERRUPT_ENABLEf))) {
            soc_reg_field_set(unit, CPRI_TX_OP_ERR_INTR_ENABLEr, &new_rval,
                      TX_DECAP_DATA_Q_BUFFSIZE_ERR_INTERRUPT_ENABLEf, enable);
        }
    }

    if (tx_op_err_intr_flags & CPRI_INTR_TX_DECAP_AG_MODE_INTR) {
        if(!(soc_reg_field_get(unit, CPRI_TX_OP_ERR_INTR_ENABLEr,
                               new_rval, TX_DECAP_AG_MODE_INTERRUPT_ENABLEf))) {
            soc_reg_field_set(unit, CPRI_TX_OP_ERR_INTR_ENABLEr, &new_rval,
                              TX_DECAP_AG_MODE_INTERRUPT_ENABLEf, enable);
        }
    }

    if (tx_op_err_intr_flags & CPRI_INTR_TX_TXPCS_MISC_INTR) {
        if(!(soc_reg_field_get(unit, CPRI_TX_OP_ERR_INTR_ENABLEr,
                               new_rval, TX_TXPCS_MISC_INTERRUPT_ENABLEf))) {
            soc_reg_field_set(unit, CPRI_TX_OP_ERR_INTR_ENABLEr, &new_rval,
                              TX_TXPCS_MISC_INTERRUPT_ENABLEf, enable);
        }
    }

    if (new_rval != rval) {
        _SOC_IF_ERR_EXIT(WRITE_CPRI_TX_OP_ERR_INTR_ENABLEr(unit, port, new_rval));
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * Get CPRI port TX functional interrupts enable status.
 */
int cprif_drv_cpri_port_tx_op_err_interrupts_enable_get(int unit,
                                                        int port,
                                                        int tx_op_err_intr_flags,
                                                        int *status)
{
    uint32 rval;
    soc_field_t field = INVALIDf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_TX_OP_ERR_INTR_ENABLEr(unit, port, &rval));


    switch (tx_op_err_intr_flags) {
        case CPRI_INTR_TX_DECAP_DATA_Q_OVRFLOW:
            field = TX_DECAP_DATA_Q_OVFL_ERR_INTERRUPT_ENABLEf;
            break;
        case CPRI_INTR_TX_DECAP_DATA_Q_UNDRFLOW:
            field = TX_DECAP_DATA_Q_UDFL_ERR_INTERRUPT_ENABLEf;
            break;
        case CPRI_INTR_TX_DECAP_DATA_Q_BUFFSIZE_ERR:
            field = TX_DECAP_DATA_Q_BUFFSIZE_ERR_INTERRUPT_ENABLEf;
            break;
        case CPRI_INTR_TX_DECAP_AG_MODE_INTR:
            field = TX_DECAP_AG_MODE_INTERRUPT_ENABLEf;
            break;
        case CPRI_INTR_TX_TXPCS_MISC_INTR:
            field = TX_TXPCS_MISC_INTERRUPT_ENABLEf;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                               ("Unknown Rx interrupt type"));
            break;
    }

    if (field != INVALIDf) {
        *status = soc_reg_field_get(unit, CPRI_TX_OP_ERR_INTR_ENABLEr, rval,
                                    field);
    }

exit:
    SOC_FUNC_RETURN;
}


/*
 * Enable CPRI port TX Functional interrupts.
 * TX Functional interrupts can be of 3 types
 * 1. TX
 * 2. TX functional error type interrupts.
 * 3. TX memory ECC error type interrupts.
 */
int cprif_drv_cpri_port_tx_func_interrupts_enable_set(int unit,
                                                      int port,
                                                      int tx_func_intr_flags,
                                                      int enable)
{
    uint32 rval, new_rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_TX_FUNC_INTR_ENABLEr(unit, port, &rval));

    new_rval = rval;

    if (tx_func_intr_flags & CPRI_INTR_TX_1588_SW) {
        if(!(soc_reg_field_get(unit, CPRI_TX_FUNC_INTR_ENABLEr,
                               new_rval, TX_1588_SW_INTERRUPT_ENABLEf))) {
            soc_reg_field_set(unit, CPRI_TX_FUNC_INTR_ENABLEr, &new_rval,
                              TX_1588_SW_INTERRUPT_ENABLEf, enable);
        }
    }

    if (tx_func_intr_flags & CPRI_INTR_TX_1588_FIFO) {
        if(!(soc_reg_field_get(unit, CPRI_TX_FUNC_INTR_ENABLEr,
                               new_rval, TX_1588_FIFO_INTERRUPT_ENABLEf))) {
            soc_reg_field_set(unit, CPRI_TX_FUNC_INTR_ENABLEr, &new_rval,
                              TX_1588_FIFO_INTERRUPT_ENABLEf, enable);
        }
    }

    if (tx_func_intr_flags & CPRI_INTR_TX_GCW) {
        if(!(soc_reg_field_get(unit, CPRI_TX_FUNC_INTR_ENABLEr,
                               new_rval, TX_GCW_INTERRUPT_ENABLEf))) {
            soc_reg_field_set(unit, CPRI_TX_FUNC_INTR_ENABLEr, &new_rval,
                              TX_GCW_INTERRUPT_ENABLEf, enable);
        }
    }

    if (new_rval != rval) {
        _SOC_IF_ERR_EXIT(WRITE_CPRI_TX_FUNC_INTR_ENABLEr(unit, port, new_rval));
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * Get CPRI port TX functional interrupts enable status.
 */
int cprif_drv_cpri_port_tx_func_interrupts_enable_get(int unit,
                                                      int port,
                                                      int tx_func_intr_flags,
                                                      int *status)
{
    uint32 rval;
    soc_field_t field = INVALIDf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_TX_FUNC_INTR_ENABLEr(unit, port, &rval));

    switch (tx_func_intr_flags) {
        case CPRI_INTR_TX_1588_SW:
            field = TX_1588_SW_INTERRUPT_ENABLEf;
            break;
        case CPRI_INTR_TX_1588_FIFO:
            field = TX_1588_FIFO_INTERRUPT_ENABLEf;
            break;
        case CPRI_INTR_TX_GCW:
            field = TX_GCW_INTERRUPT_ENABLEf;
            break;
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM,
                               ("Unknown Rx interrupt type"));
            break;
    }

    if (field != INVALIDf) {
        *status = soc_reg_field_get(unit, CPRI_TX_FUNC_INTR_ENABLEr, rval, field);
    }

exit:
    SOC_FUNC_RETURN;
}

/* Enable interrupt to detect link UP - Link Status Latched High. */
int cprif_drv_rx_pcs_link_up_intr_enable_set(int unit,
                                               int port,
                                               int enable)
{
    uint32 rval;
    uint32 rxpcs_los_lh = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_INTR_ENABLEr(unit, port, &rval));
    rxpcs_los_lh = soc_reg_field_get(unit, CPRI_RXPCS_INTR_ENABLEr,
                                     rval, RXPCS_LINK_STATUS_LH_INTERRUPT_ENf);

    /* do nothing if the requested operation is already set */
    if (!(enable && rxpcs_los_lh)) {
        soc_reg_field_set(unit, CPRI_RXPCS_INTR_ENABLEr, &rval,
                          RXPCS_LINK_STATUS_LH_INTERRUPT_ENf, enable);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_RXPCS_INTR_ENABLEr(unit, port, rval));
    }

exit:
    SOC_FUNC_RETURN;
}

/* Get interrupt enable Status to detect link UP event */
int cprif_drv_rx_pcs_link_up_intr_enable_get(int unit,
                                               int port,
                                               int *status)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_INTR_ENABLEr(unit, port, &rval));
    *status = soc_reg_field_get(unit, CPRI_RXPCS_INTR_ENABLEr,
                                rval, RXPCS_LINK_STATUS_LH_INTERRUPT_ENf);

exit:
    SOC_FUNC_RETURN;
}

/*
 * Get link interrupt Status by checking Link Status Latched High.
 * The link is UP is the LOS LL is set.
 */
int cprif_drv_rx_pcs_link_up_intr_status_get(int unit,
                                               int port,
                                               int *status)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_INTR_STATUSr(unit, port, &rval));
    *status = soc_reg_field_get(unit, CPRI_RXPCS_INTR_STATUSr,
                                rval, RXPCS_LINK_STATUS_LH_INTERRUPT_STATUSf);

exit:
    SOC_FUNC_RETURN;
}

/*
 * This function checks if the link is UP by checking the
 * RXPCS Link Staus status. If the Link Stauts LH(latched high)
 * status is set, meaning the link is UP, the Link Status LH STATUS
 * is cleared by writing 1 to the field.
 *
 * Note: Currently called from the link event interrupt
 * handler, to check a link status change.
 */
int cprif_drv_rx_pcs_link_up_intr_status_clr_get(int unit,
                                                   int port,
                                                   int *status)
{
    uint32 rval;
    int is_enabled = 0;
    SOC_INIT_FUNC_DEFS;

    /*
     * Check if the Link up event interrupt is enabled
     * if not, ignore and return.
     */
    _SOC_IF_ERR_EXIT(cprif_drv_rx_pcs_link_up_intr_enable_get(unit, port,
                                                            &is_enabled));

    if(!is_enabled) {
        return SOC_E_NONE;
    }

    _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_INTR_STATUSr(unit, port, &rval));
    *status = soc_reg_field_get(unit, CPRI_RXPCS_INTR_STATUSr,
                                rval, RXPCS_LINK_STATUS_LH_INTERRUPT_STATUSf);

    if (*status) {
        LOG_CLI(("Port(%s)-%d UP!! Clearing interrupt \n",
             SOC_PORT_NAME(unit, port), port));
        /*
         *  Only Clear the bit that are processing.
         */
        rval = 0;
        soc_reg_field_set(unit, CPRI_RXPCS_INTR_STATUSr, &rval,
                          RXPCS_LINK_STATUS_LH_INTERRUPT_STATUSf, 1);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_RXPCS_INTR_STATUSr(unit, port, rval));
    }

exit:
    SOC_FUNC_RETURN;
}

/* Enable interrupt to detect link DOWN - Link Status signal is latched low */
int cprif_drv_rx_pcs_link_down_intr_enable_set(int unit,
                                               int port,
                                               int enable)
{
    uint32 rval;
    uint32 rxpcs_los_ll = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_INTR_ENABLEr(unit, port, &rval));
    rxpcs_los_ll = soc_reg_field_get(unit, CPRI_RXPCS_INTR_ENABLEr,
                                     rval, RXPCS_LINK_STATUS_LL_INTERRUPT_ENf);

    /* do nothing if the requested operation is already set */
    if (!(enable && rxpcs_los_ll)) {
        soc_reg_field_set(unit, CPRI_RXPCS_INTR_ENABLEr, &rval,
                          RXPCS_LINK_STATUS_LL_INTERRUPT_ENf, enable);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_RXPCS_INTR_ENABLEr(unit, port, rval));
    }

exit:
    SOC_FUNC_RETURN;
}

/* Get interrupt enable status to detect link DOWN - Link Status signal(latched low) */
int cprif_drv_rx_pcs_link_down_intr_enable_get(int unit,
                                               int port,
                                               int *status)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_INTR_ENABLEr(unit, port, &rval));
    *status = soc_reg_field_get(unit, CPRI_RXPCS_INTR_ENABLEr,
                                rval, RXPCS_LINK_STATUS_LL_INTERRUPT_ENf);

exit:
    SOC_FUNC_RETURN;
}

/*
 * Get link interrupt Status by checking Link Status signal - latched low.
 * The link is DOWN is the LOS LH is set.
 */
int cprif_drv_rx_pcs_link_down_intr_status_get(int unit,
                                               int port,
                                               int *status)
{
    uint32 rval;
    int is_enabled;
    SOC_INIT_FUNC_DEFS;

    /*
     * Check if the Link down event interrupt is enabled
     * if not, ignore the status and return.
     */
    _SOC_IF_ERR_EXIT(cprif_drv_rx_pcs_link_down_intr_enable_get(unit, port,
                                                            &is_enabled));
    if (is_enabled) {
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_INTR_STATUSr(unit, port, &rval));
        *status = soc_reg_field_get(unit, CPRI_RXPCS_INTR_STATUSr,
                                    rval, RXPCS_LINK_STATUS_LL_INTERRUPT_STATUSf);
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * This function checks if the link is DOWN by checking the
 * RXPCS LINK_STATUS LL signal status. If the LINK_STATUS LL(latched low)
 * status is set, meaning the link is DOWN, the Link Status LL STATUS
 * is cleared by writing 1 to the field.
 *
 * Note: Currently called from the link event interrupt
 * handler, to check a link status change.
 */
int cprif_drv_rx_pcs_link_down_intr_status_clr_get(int unit,
                                                 int port,
                                                 int *status)
{
    uint32 rval;
    int is_enabled = 0;
    SOC_INIT_FUNC_DEFS;

    /*
     * Check if the Link down event interrupt is enabled
     * if not, return.
     */
    _SOC_IF_ERR_EXIT(cprif_drv_rx_pcs_link_down_intr_enable_get(unit, port,
                                                            &is_enabled));
    if (!is_enabled) {
        return SOC_E_NONE;
    }
    _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_INTR_STATUSr(unit, port, &rval));
    *status = soc_reg_field_get(unit, CPRI_RXPCS_INTR_STATUSr,
                                rval, RXPCS_LINK_STATUS_LL_INTERRUPT_STATUSf);

    if (*status) {
        LOG_CLI(("Port(%s)-%d DOWN!! Clearing interrupt\n",
                 SOC_PORT_NAME(unit, port), port));
        /*
         *  Only Clear the bit that are processing.
         */
        rval = 0;
        soc_reg_field_set(unit, CPRI_RXPCS_INTR_STATUSr, &rval,
                          RXPCS_LINK_STATUS_LL_INTERRUPT_STATUSf, 1);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_RXPCS_INTR_STATUSr(unit, port, rval));
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_1588_fifo_intr_enable_set(int unit, int port, int enable)
{
    uint32 rval;
    int curr_val = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_RX_1588_FIFO_INTR_ENABLE_CONTROLr(unit, port,
                                                                 &rval));
    curr_val = soc_reg_field_get(unit, CPRI_RX_1588_FIFO_INTR_ENABLE_CONTROLr,
                                 rval, RX_TS_FIFO_INTERRUPT_ENf);

    if (curr_val != enable) {
        soc_reg_field_set(unit, CPRI_RX_1588_FIFO_INTR_ENABLE_CONTROLr, &rval,
                          RX_TS_FIFO_INTERRUPT_ENf, enable);
    }

    _SOC_IF_ERR_EXIT(WRITE_CPRI_RX_1588_FIFO_INTR_ENABLE_CONTROLr(unit, port,
                                                                  rval));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_1588_fifo_intr_enable_get(int unit, int port, int *status)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_RX_1588_FIFO_INTR_ENABLE_CONTROLr(unit, port,
                                                                 &rval));
    *status = soc_reg_field_get(unit, CPRI_RX_1588_FIFO_INTR_ENABLE_CONTROLr,
                                rval, RX_TS_FIFO_INTERRUPT_ENf);

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_1588_fifo_intr_enable_set(int unit, int port, int enable)
{
    uint32 rval;
    int curr_val = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_TX_1588_FIFO_INTR_ENABLE_CONTROLr(unit, port,
                                                                 &rval));
    curr_val = soc_reg_field_get(unit, CPRI_TX_1588_FIFO_INTR_ENABLE_CONTROLr,
                                 rval, TX_TS_FIFO_INTERRUPT_ENf);

    if (curr_val != enable) {
        soc_reg_field_set(unit, CPRI_TX_1588_FIFO_INTR_ENABLE_CONTROLr, &rval,
                          TX_TS_FIFO_INTERRUPT_ENf, enable);
    }

    _SOC_IF_ERR_EXIT(WRITE_CPRI_TX_1588_FIFO_INTR_ENABLE_CONTROLr(unit, port,
                                                                  rval));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_1588_fifo_intr_enable_get(int unit, int port, int *status)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_TX_1588_FIFO_INTR_ENABLE_CONTROLr(unit, port,
                                                                 &rval));
    *status = soc_reg_field_get(unit, CPRI_TX_1588_FIFO_INTR_ENABLE_CONTROLr,
                                rval, TX_TS_FIFO_INTERRUPT_ENf);

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_1588_sw_intr_enable_set(int unit, int port, int enable)
{
    uint32 rval;
    int curr_val = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_RX_1588_CAP_INTR_ENABLE_CONTROLr(unit, port,
                                                                 &rval));
    curr_val = soc_reg_field_get(unit, CPRI_RX_1588_CAP_INTR_ENABLE_CONTROLr,
                                 rval, RX_CAPTURED_TS_INTERRUPT_ENf);

    if (curr_val != enable) {
        soc_reg_field_set(unit, CPRI_RX_1588_CAP_INTR_ENABLE_CONTROLr, &rval,
                          RX_CAPTURED_TS_INTERRUPT_ENf, enable);
    }

    _SOC_IF_ERR_EXIT(WRITE_CPRI_RX_1588_CAP_INTR_ENABLE_CONTROLr(unit, port,
                                                                  rval));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rx_1588_sw_intr_enable_get(int unit, int port, int *status)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_RX_1588_CAP_INTR_ENABLE_CONTROLr(unit, port,
                                                                 &rval));
    *status = soc_reg_field_get(unit, CPRI_RX_1588_CAP_INTR_ENABLE_CONTROLr,
                                rval, RX_CAPTURED_TS_INTERRUPT_ENf);

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_1588_sw_intr_enable_set(int unit, int port, int enable)
{
    uint32 rval;
    int curr_val = 0;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_TX_1588_CAP_INTR_ENABLE_CONTROLr(unit, port,
                                                                 &rval));
    curr_val = soc_reg_field_get(unit, CPRI_TX_1588_CAP_INTR_ENABLE_CONTROLr,
                                 rval, TX_CAPTURED_TS_INTERRUPT_ENf);

    if (curr_val != enable) {
        soc_reg_field_set(unit, CPRI_TX_1588_CAP_INTR_ENABLE_CONTROLr, &rval,
                          TX_CAPTURED_TS_INTERRUPT_ENf, enable);
    }

    _SOC_IF_ERR_EXIT(WRITE_CPRI_TX_1588_CAP_INTR_ENABLE_CONTROLr(unit, port,
                                                                  rval));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_1588_sw_intr_enable_get(int unit, int port, int *status)
{
    uint32 rval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_TX_1588_CAP_INTR_ENABLE_CONTROLr(unit, port,
                                                                 &rval));
    *status = soc_reg_field_get(unit, CPRI_TX_1588_CAP_INTR_ENABLE_CONTROLr,
                                rval, TX_CAPTURED_TS_INTERRUPT_ENf);

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_fast_clk_bit_time_period_set(int unit, int port, uint32 bit_time_period)
{
    uint32 fast_clk_tp;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_FAST_CLK_CTRLr(unit, port, &fast_clk_tp));
    soc_reg_field_set(unit, CPRI_FAST_CLK_CTRLr, &fast_clk_tp, FAST_CLK_PERIODf, bit_time_period);
    _SOC_IF_ERR_EXIT(WRITE_CPRI_FAST_CLK_CTRLr(unit, port, fast_clk_tp));

exit:
    SOC_FUNC_RETURN;
}


int cprif_drv_fast_clk_bit_time_period_get(int unit, int port, uint32* fast_clk)
{
    uint32 fast_clk_tp;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(READ_CPRI_FAST_CLK_CTRLr(unit, port, &fast_clk_tp));
    *fast_clk = soc_reg_field_get(unit, CPRI_FAST_CLK_CTRLr, fast_clk_tp, FAST_CLK_PERIODf);

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_1588_bit_time_period_set(int unit, int port, cprimod_direction_t direction, cprimod_port_speed_t speed)
{
    uint32 bit_period32;
    uint64 bit_period64;
    uint64 bit_period;
    SOC_INIT_FUNC_DEFS;

    bit_period32 = 0x682b;
    switch(speed) {
    case cprimodSpd614p4   :
        bit_period32 = 0x682ab; /*  ROUND((1000*1024*32*8)/614.4   ,0) */
        break;
    case cprimodSpd1228p8  :
        bit_period32 = 0x34155; /*  ROUND((1000*1024*32*8)/1228.8  ,0) */
        break;
    case cprimodSpd2457p6  :
        bit_period32 = 0x1a0ab; /*  ROUND((1000*1024*32*8)/2457.6  ,0) */
        break;
    case cprimodSpd3072p0  :
        bit_period32 = 0x14d55; /*  ROUND((1000*1024*32*8)/3072    ,0) */
        break;
    case cprimodSpd4915p2  :
        bit_period32 = 0xd055 ; /*  ROUND((1000*1024*32*8)/4915.2  ,0) */
        break;
    case cprimodSpd6144p0  :
        bit_period32 = 0xa6aa ; /*  ROUND((1000*1024*32*8)/6144    ,0) */
        break;
    case cprimodSpd8110p08 :
        bit_period32 = 0x7e43 ; /*  ROUND((1000*1024*32*8)/8110.08 ,0) */
        break;
    case cprimodSpd9830p4  :
        bit_period32 = 0x682b ; /*  ROUND((1000*1024*32*8)/9830.4  ,0) */
        break;
    case cprimodSpd10137p6 :
        bit_period32 = 0x6503 ; /*  ROUND((1000*1024*32*8)/10137.6 ,0) */
        break;
    case cprimodSpd12165p12:
        bit_period32 = 0x542d ; /*  ROUND((1000*1024*32*8)/12165.12,0) */
        break;
    case cprimodSpd24330p24:
        bit_period32 = 0x2a16 ; /*  ROUND((1000*1024*32*8)/24330.24,0) */
        break;
    case cprimodSpdCount :
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Bad speed"));
        break;
    }
    COMPILER_64_SET(bit_period64, 0, bit_period32);
    if(direction == CPRIMOD_DIR_TX) {
        _SOC_IF_ERR_EXIT(READ_CPRI_TX_1588_CTRLr(unit, port, &bit_period));
        soc_reg64_field_set(unit, CPRI_TX_1588_CTRLr, &bit_period, TX_BIT_TIME_PERIODf, bit_period64);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_TX_1588_CTRLr(unit, port, bit_period));
    } else {
        _SOC_IF_ERR_EXIT(READ_CPRI_RX_1588_CTRLr(unit, port, &bit_period));
        soc_reg64_field_set(unit, CPRI_RX_1588_CTRLr, &bit_period, RX_BIT_TIME_PERIODf, bit_period64);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_RX_1588_CTRLr(unit, port, bit_period));
    }

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_1588_bit_time_period_get(int unit, int port, cprimod_direction_t direction, uint32* bit_time_period)
{
    uint64 bit_period;
    uint64 bit_period64;
    SOC_INIT_FUNC_DEFS;
    if(direction == CPRIMOD_DIR_TX) {
        _SOC_IF_ERR_EXIT(READ_CPRI_TX_1588_CTRLr(unit, port, &bit_period));
        bit_period64 = soc_reg64_field_get(unit, CPRI_TX_1588_CTRLr, bit_period, TX_BIT_TIME_PERIODf);
    } else {
        _SOC_IF_ERR_EXIT(READ_CPRI_RX_1588_CTRLr(unit, port, &bit_period));
        bit_period64 = soc_reg64_field_get(unit, CPRI_RX_1588_CTRLr, bit_period, RX_BIT_TIME_PERIODf);
    }
    *bit_time_period = COMPILER_64_LO(bit_period64);

exit:
    SOC_FUNC_RETURN;
}


int cprif_drv_1588_timestamp_capture_config_set(int unit, int port, cprimod_direction_t direction, cprimod_1588_capture_config_t *config)
{
    uint64 ctrl;
    uint64 cap_bfn;
    uint64 cap_hfn;
    uint64 hfn_only;
    uint64 val64;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_SET(cap_bfn, 0, config->capture_bfn_num);
    COMPILER_64_SET(cap_hfn, 0, config->capture_hfn_num);
    if(config->mode == cprimod_1588_MATCH_HFN_ONLY) {
         COMPILER_64_SET(hfn_only, 0, 1);
    } else {
         COMPILER_64_SET(hfn_only, 0, 0);
    }
    COMPILER_64_SET(val64, 0, 1);
    if(direction == CPRIMOD_DIR_TX) {
        _SOC_IF_ERR_EXIT(READ_CPRI_TX_1588_CTRLr(unit, port, &ctrl));
        soc_reg64_field_set(unit, CPRI_TX_1588_CTRLr, &ctrl, TX_TS_CAPTURE_EVERY_BFNf, hfn_only);
        soc_reg64_field_set(unit, CPRI_TX_1588_CTRLr, &ctrl, TX_TS_CAPTURE_BFNf,       cap_bfn);
        soc_reg64_field_set(unit, CPRI_TX_1588_CTRLr, &ctrl, TX_TS_CAPTURE_HFNf,       cap_hfn);
        soc_reg64_field_set(unit, CPRI_TX_1588_CTRLr, &ctrl,
                      TX_TS_CAPTURE_REQf, val64);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_TX_1588_CTRLr(unit, port, ctrl));
    } else {
        _SOC_IF_ERR_EXIT(READ_CPRI_RX_1588_CTRLr(unit, port, &ctrl));
        soc_reg64_field_set(unit, CPRI_RX_1588_CTRLr, &ctrl, RX_TS_CAPTURE_EVERY_BFNf, hfn_only);
        soc_reg64_field_set(unit, CPRI_RX_1588_CTRLr, &ctrl, RX_TS_CAPTURE_BFNf,       cap_bfn);
        soc_reg64_field_set(unit, CPRI_RX_1588_CTRLr, &ctrl, RX_TS_CAPTURE_HFNf,       cap_hfn);
        soc_reg64_field_set(unit, CPRI_RX_1588_CTRLr, &ctrl,
                      RX_TS_CAPTURE_REQf, val64);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_RX_1588_CTRLr(unit, port, ctrl));
    }

exit:
    SOC_FUNC_RETURN;

}
int cprif_drv_1588_timestamp_capture_config_get(int unit, int port, cprimod_direction_t direction, cprimod_1588_capture_config_t* config)
{
    uint64 ctrl;
    uint64 cap_bfn;
    uint64 cap_hfn;
    uint64 hfn_only;
    uint8  hfn_only8;
    SOC_INIT_FUNC_DEFS;

    if(direction == CPRIMOD_DIR_TX) {
        _SOC_IF_ERR_EXIT(READ_CPRI_TX_1588_CTRLr(unit, port, &ctrl));
        hfn_only = soc_reg64_field_get(unit, CPRI_TX_1588_CTRLr, ctrl, TX_TS_CAPTURE_EVERY_BFNf);
        cap_bfn  = soc_reg64_field_get(unit, CPRI_TX_1588_CTRLr, ctrl, TX_TS_CAPTURE_BFNf);
        cap_hfn  = soc_reg64_field_get(unit, CPRI_TX_1588_CTRLr, ctrl, TX_TS_CAPTURE_HFNf);
    } else {
        _SOC_IF_ERR_EXIT(READ_CPRI_RX_1588_CTRLr(unit, port, &ctrl));
        hfn_only = soc_reg64_field_get(unit, CPRI_RX_1588_CTRLr, ctrl, RX_TS_CAPTURE_EVERY_BFNf);
        cap_bfn  = soc_reg64_field_get(unit, CPRI_RX_1588_CTRLr, ctrl, RX_TS_CAPTURE_BFNf);
        cap_hfn  = soc_reg64_field_get(unit, CPRI_RX_1588_CTRLr, ctrl, RX_TS_CAPTURE_HFNf);
    }
    config->capture_bfn_num = COMPILER_64_LO(cap_bfn) & 0xFFFF;
    config->capture_hfn_num = COMPILER_64_LO(cap_hfn) & 0xFFFF;
    hfn_only8 = COMPILER_64_LO(hfn_only) & 0xff;
    if(hfn_only8) {
        config->mode = cprimod_1588_MATCH_HFN_ONLY;
    } else {
        config->mode = cprimod_1588_MATCH_BFN_AND_HFN;
    }

exit:
    SOC_FUNC_RETURN;
}


int cprif_drv_1588_captured_timestamp_get(int unit, int port, cprimod_direction_t direction, cprimod_1588_ts_type_t ts_type, uint8 mx_cnt, uint8* count, uint64* captured_time)
{
    int i;
    uint64 ctrl;
    uint64 ts;
    uint64 val_zero;
    uint64 ts_stat;
    uint32 ts_stat32;
    SOC_INIT_FUNC_DEFS;
    if (mx_cnt==0) {
        *count = 0;
        SOC_FUNC_RETURN;
    }
    if (ts_type == CPRIMOD_1588_TS_TYPE_SW_CAP) {
        if (direction == CPRIMOD_DIR_TX) {
            _SOC_IF_ERR_EXIT(READ_CPRI_TX_1588_CAP_INTR_STATUSr(unit, port, &ts));
            ts_stat = soc_reg64_field_get(unit, CPRI_TX_1588_CAP_INTR_STATUSr, ts, TX_CAPTURED_TS_INTERRUPT_STATUSf);
            ts_stat32 = COMPILER_64_LO(ts_stat);
            if (ts_stat32) {
                *count = 1;
                *captured_time = soc_reg64_field_get(unit, CPRI_TX_1588_CAP_INTR_STATUSr, ts, TX_CAPTURED_TSf);
                _SOC_IF_ERR_EXIT(WRITE_CPRI_TX_1588_CAP_INTR_STATUSr(unit, port, ts));
                /* disable Tx capture as auto clear is Hw not working */
                COMPILER_64_ZERO(val_zero);
                _SOC_IF_ERR_EXIT(READ_CPRI_TX_1588_CTRLr(unit, port, &ctrl));
                soc_reg64_field_set(unit, CPRI_TX_1588_CTRLr, &ctrl,
                              TX_TS_CAPTURE_REQf, val_zero);
                _SOC_IF_ERR_EXIT(WRITE_CPRI_TX_1588_CTRLr(unit, port, ctrl));
            } else {
                *count = 0;
            }
        } else {
            _SOC_IF_ERR_EXIT(READ_CPRI_RX_1588_CAP_INTR_STATUSr(unit, port, &ts));
            ts_stat = soc_reg64_field_get(unit, CPRI_RX_1588_CAP_INTR_STATUSr, ts, RX_CAPTURED_TS_INTERRUPT_STATUSf);
            ts_stat32 = COMPILER_64_LO(ts_stat);
            if (ts_stat32) {
                *count = 1;
                *captured_time = soc_reg64_field_get(unit, CPRI_RX_1588_CAP_INTR_STATUSr, ts, RX_CAPTURED_TSf);
                _SOC_IF_ERR_EXIT(WRITE_CPRI_RX_1588_CAP_INTR_STATUSr(unit, port, ts));
            } else {
                *count = 0;
            }
        }
    } else if (ts_type == CPRIMOD_1588_TS_TYPE_FIFO) {
        if (direction == CPRIMOD_DIR_TX) {
            for (i=0; i < mx_cnt; i++) {
                _SOC_IF_ERR_EXIT(READ_CPRI_TX_1588_FIFO_INTR_STATUSr(unit, port, &ts));
                ts_stat = soc_reg64_field_get(unit, CPRI_TX_1588_FIFO_INTR_STATUSr, ts, TX_TS_FIFO_INTERRUPT_STATUSf);
                ts_stat32 = COMPILER_64_LO(ts_stat);
                if (ts_stat32 == 0) {
                    break;
                } else {
                    captured_time[i] = soc_reg64_field_get(unit, CPRI_TX_1588_FIFO_INTR_STATUSr, ts, TX_TS_FIFOf);
                    _SOC_IF_ERR_EXIT(WRITE_CPRI_TX_1588_FIFO_INTR_STATUSr(unit, port, ts));
                }
            }
            *count = i;
        } else {
            for (i=0; i < mx_cnt; i++) {
                _SOC_IF_ERR_EXIT(READ_CPRI_RX_1588_FIFO_INTR_STATUSr(unit, port, &ts));
                ts_stat = soc_reg64_field_get(unit, CPRI_RX_1588_FIFO_INTR_STATUSr, ts, RX_TS_FIFO_INTERRUPT_STATUSf);
                ts_stat32 = COMPILER_64_LO(ts_stat);
                if (ts_stat32 == 0) {
                    break;
                } else {
                    captured_time[i] = soc_reg64_field_get(unit, CPRI_RX_1588_FIFO_INTR_STATUSr, ts, RX_TS_FIFOf);
                    _SOC_IF_ERR_EXIT(WRITE_CPRI_RX_1588_FIFO_INTR_STATUSr(unit, port, ts));
                }
            }
            *count = i;
        }
    }

exit:
    SOC_FUNC_RETURN;
}


int cprif_drv_1588_timestamp_adjust_set(int unit, int port, cprimod_direction_t direction, cprimod_1588_time_t* adjust_time)
{
    uint64 ts_adj_l;
    uint64 adj_time;
    uint64 adj_time_sub;
    uint64 bitflipmsk;
    uint64 bit53mask;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_COPY(adj_time, adjust_time->nano_sec);
    COMPILER_64_SHL(adj_time,4);
    COMPILER_64_SET(adj_time_sub,0, adjust_time->sub_nano_sec & 0xf);
    COMPILER_64_OR(adj_time, adj_time_sub);
    if(adjust_time->sign_bit) {
        COMPILER_64_SET(bitflipmsk, 0xffffffff, 0xffffffff);
        COMPILER_64_XOR(adj_time, bitflipmsk);
        COMPILER_64_ADD_32(adj_time, 1);
    }
    COMPILER_64_SET(bit53mask, 0x1fffff, 0xffffffff);
    COMPILER_64_AND(adj_time, bit53mask);
    COMPILER_64_ZERO(ts_adj_l);
    if(direction == CPRIMOD_DIR_TX) {
        soc_reg64_field_set(unit, CPRI_TX_TS_ADJ_CTRLr, &ts_adj_l, TX_TS_ADJf, adj_time);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_TX_TS_ADJ_CTRLr(unit, port, ts_adj_l));
    } else {
        soc_reg64_field_set(unit, CPRI_RX_TS_ADJ_CTRLr, &ts_adj_l, RX_TS_ADJf, adj_time);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_RX_TS_ADJ_CTRLr(unit, port, ts_adj_l));
    }

exit:
    SOC_FUNC_RETURN;

}
int cprif_drv_1588_timestamp_adjust_get(int unit, int port, cprimod_direction_t direction, cprimod_1588_time_t* adjust_time)
{
    uint64 ts_adj;
    uint64 adj_time;
    uint64 adj_time_cp;
    uint64 bitflipmsk;
    uint64 bit53mask;
    uint32 subns;
    SOC_INIT_FUNC_DEFS;
    adjust_time->sign_bit = 0;
    if(direction == CPRIMOD_DIR_TX) {
        _SOC_IF_ERR_EXIT(READ_CPRI_TX_TS_ADJ_CTRLr(unit, port, &ts_adj));
        adj_time = soc_reg64_field_get(unit, CPRI_TX_TS_ADJ_CTRLr, ts_adj, TX_TS_ADJf);
    } else {
        _SOC_IF_ERR_EXIT(READ_CPRI_RX_TS_ADJ_CTRLr(unit, port, &ts_adj));
        adj_time = soc_reg64_field_get(unit, CPRI_RX_TS_ADJ_CTRLr, ts_adj, RX_TS_ADJf);
    }
    COMPILER_64_COPY(adj_time_cp, adj_time);
    COMPILER_64_SHR(adj_time_cp,52);
    if(!COMPILER_64_IS_ZERO(adj_time_cp)) {
        adjust_time->sign_bit = 1;
        COMPILER_64_SET(bitflipmsk, 0xffffffff, 0xffffffff);
        COMPILER_64_XOR(adj_time, bitflipmsk);
        COMPILER_64_ADD_32(adj_time, 1);
    }
    COMPILER_64_SET(bit53mask, 0x1fffff, 0xffffffff);
    COMPILER_64_AND(adj_time, bit53mask);
    COMPILER_64_TO_32_LO(subns, adj_time);
    adjust_time->sub_nano_sec = subns & 0xf;
    COMPILER_64_COPY(adjust_time->nano_sec, adj_time);
    COMPILER_64_SHR(adjust_time->nano_sec,4);

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_1588_timestamp_fifo_config_set(int unit, int port, cprimod_direction_t direction, uint16 modulo)
{
    uint64 ctrl;
    uint64 mod_ctl;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_SET(mod_ctl, 0, modulo);
    if(direction == CPRIMOD_DIR_TX) {
        _SOC_IF_ERR_EXIT(READ_CPRI_TX_1588_CTRLr(unit, port, &ctrl));
        soc_reg64_field_set(unit, CPRI_TX_1588_CTRLr, &ctrl, TX_TS_MOD_CNTf, mod_ctl);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_TX_1588_CTRLr(unit, port, ctrl));
    } else {
        _SOC_IF_ERR_EXIT(READ_CPRI_RX_1588_CTRLr(unit, port, &ctrl));
        soc_reg64_field_set(unit, CPRI_RX_1588_CTRLr, &ctrl, RX_TS_MOD_CNTf, mod_ctl);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_RX_1588_CTRLr(unit, port, ctrl));
    }

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_1588_timestamp_fifo_config_get(int unit, int port, cprimod_direction_t direction, uint16* modulo)
{
    uint64 ctrl;
    uint64 mod_ctl;
    SOC_INIT_FUNC_DEFS;
    if(direction == CPRIMOD_DIR_TX) {
        _SOC_IF_ERR_EXIT(READ_CPRI_TX_1588_CTRLr(unit, port, &ctrl));
        mod_ctl = soc_reg64_field_get(unit, CPRI_TX_1588_CTRLr, ctrl, TX_TS_MOD_CNTf);
    } else {
        _SOC_IF_ERR_EXIT(READ_CPRI_RX_1588_CTRLr(unit, port, &ctrl));
        mod_ctl = soc_reg64_field_get(unit, CPRI_RX_1588_CTRLr, ctrl, RX_TS_MOD_CNTf);
    }
    COMPILER_64_TO_32_LO(*modulo, mod_ctl);

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_1588_timestamp_cmic_config_set(int unit, int port, cprimod_direction_t direction, uint16 modulo)
{
    uint32 mod_ctrl;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(READ_CPRI_RF_TS_CTRLr(unit, port, &mod_ctrl));
    if(direction == CPRIMOD_DIR_TX) {
       soc_reg_field_set(unit, CPRI_RF_TS_CTRLr, &mod_ctrl, TX_RF_TS_IF_MOD_CNTf, modulo);
    } else {
       soc_reg_field_set(unit, CPRI_RF_TS_CTRLr, &mod_ctrl, RX_RF_TS_IF_MOD_CNTf, modulo);
    }
    _SOC_IF_ERR_EXIT(WRITE_CPRI_RF_TS_CTRLr(unit, port, mod_ctrl));

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_1588_timestamp_cmic_config_get(int unit, int port, cprimod_direction_t direction, uint16* modulo)
{
    uint32 mod_ctrl;
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(READ_CPRI_RF_TS_CTRLr(unit, port, &mod_ctrl));
    if(direction == CPRIMOD_DIR_TX) {
       *modulo = soc_reg_field_get(unit, CPRI_RF_TS_CTRLr, mod_ctrl, TX_RF_TS_IF_MOD_CNTf);
    } else {
       *modulo = soc_reg_field_get(unit, CPRI_RF_TS_CTRLr, mod_ctrl, RX_RF_TS_IF_MOD_CNTf);
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_ingress_presentation_time_enable_set(int unit, int port, int enable)
{
    uint32 reg_val=0;
    uint64 reg64_val;
    uint32 field_buf;
    uint64 field64_buf;
    cprif_pm_version_t pm_version;

    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg64_val);
    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));

    if (CPRIF_CLPORT_IS_A0(pm_version)) {
        /*
         * Only A0, use 32 bits.
         */
        _SOC_IF_ERR_EXIT(READ_CPRI_ING_PRES_TM_CTRLr_REG32(unit, port, &reg_val));
        field_buf = enable ? 1 : 0;
        soc_reg_field_set(unit, CPRI_ING_PRES_TM_CTRLr , &reg_val,
                            ING_AGNOSTIC_MODE_ENf, field_buf);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_ING_PRES_TM_CTRLr_REG32(unit, port, reg_val));
    } else if (CPRIF_CLPORT_IS_B0(pm_version)) {
        /*
         *  B0 use 64 bits.
         */
        _SOC_IF_ERR_EXIT(READ_CPRI_ING_PRES_TM_CTRLr(unit, port, &reg64_val));
        if (enable) {
            COMPILER_64_SET(field64_buf, 0, 1);
        } else {
            COMPILER_64_ZERO(field64_buf);
        }
        soc_reg64_field_set(unit, CPRI_ING_PRES_TM_CTRLr , &reg64_val,
                            ING_AGNOSTIC_MODE_ENf, field64_buf);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_ING_PRES_TM_CTRLr(unit, port, reg64_val));
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Need Support for New Version.\n"));
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_ingress_presentation_time_enable_get(int unit, int port, int* enable)
{
    uint32 reg_val;
    uint64 reg64_val;
    uint32 field_buf;
    uint64 field64_buf;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));

    *enable = 0;
    if (CPRIF_CLPORT_IS_A0(pm_version)) {
        /*
         * Only A0, use 32 bits.
         */
        _SOC_IF_ERR_EXIT(READ_CPRI_ING_PRES_TM_CTRLr_REG32(unit, port, &reg_val));
        field_buf = soc_reg_field_get(unit, CPRI_ING_PRES_TM_CTRLr , reg_val,
                                      ING_AGNOSTIC_MODE_ENf);
        *enable = field_buf? 1:0;
    } else if (CPRIF_CLPORT_IS_B0(pm_version)) {
        /*
         *  B0 use 64 bits.
         */
        _SOC_IF_ERR_EXIT(READ_CPRI_ING_PRES_TM_CTRLr(unit, port, &reg64_val));
        field64_buf = soc_reg64_field_get(unit, CPRI_ING_PRES_TM_CTRLr , reg64_val,
                                        ING_AGNOSTIC_MODE_ENf);
        *enable = (COMPILER_64_IS_ZERO(field64_buf))? 0:1;

    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Need Support for New Version.\n"));
    }

exit:
    SOC_FUNC_RETURN;
}


int cprif_drv_ingress_presentation_time_config_set(int unit, int port, cprif_cpri_ingress_pres_time_ctrl_t* config)
{
    uint32 reg_val;
    uint64 reg64_val;
    uint32 field_buf;
    uint64 field64_buf;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));

    if (CPRIF_CLPORT_IS_A0(pm_version)) {
        /*
         * Only A0, use 32 bits.
         */
        _SOC_IF_ERR_EXIT(READ_CPRI_ING_PRES_TM_CTRLr_REG32(unit, port, &reg_val));
        field_buf = config->pres_mod_cnt;
        soc_reg_field_set(unit, CPRI_ING_PRES_TM_CTRLr , &reg_val,
                          ING_PRES_MOD_CNTf, field_buf);
        field_buf = config->bfn_offset;
        soc_reg_field_set(unit, CPRI_ING_PRES_TM_CTRLr , &reg_val,
                          ING_PRES_BFN_OFFSETf, field_buf);
        field_buf = config->hfn_offset;
        soc_reg_field_set(unit, CPRI_ING_PRES_TM_CTRLr , &reg_val,
                          ING_PRES_HFN_OFFSETf, field_buf);
        field_buf = config->hfrm_sync;
        soc_reg_field_set(unit, CPRI_ING_PRES_TM_CTRLr , &reg_val,
                          ING_PRES_HFRM_SYNCf, field_buf);
        field_buf = config->rfrm_sync;
        soc_reg_field_set(unit, CPRI_ING_PRES_TM_CTRLr , &reg_val,
                          ING_PRES_RFRM_SYNCf, field_buf);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_ING_PRES_TM_CTRLr_REG32(unit, port, reg_val));

    } else if (CPRIF_CLPORT_IS_B0(pm_version)) {
        /*
         * B0 use 64 bits.
         */
        _SOC_IF_ERR_EXIT(READ_CPRI_ING_PRES_TM_CTRLr(unit, port, &reg64_val));
        COMPILER_64_SET(field64_buf, 0, config->pres_mod_cnt);
        soc_reg64_field_set(unit, CPRI_ING_PRES_TM_CTRLr , &reg64_val,
                            ING_PRES_MOD_CNTf, field64_buf);
        COMPILER_64_SET(field64_buf, 0, config->bfn_offset);
        soc_reg64_field_set(unit, CPRI_ING_PRES_TM_CTRLr , &reg64_val,
                            ING_PRES_BFN_OFFSETf, field64_buf);
        COMPILER_64_SET(field64_buf, 0, config->hfn_offset);
        soc_reg64_field_set(unit, CPRI_ING_PRES_TM_CTRLr , &reg64_val,
                            ING_PRES_HFN_OFFSETf, field64_buf);
        COMPILER_64_SET(field64_buf, 0, config->hfrm_sync);
        soc_reg64_field_set(unit, CPRI_ING_PRES_TM_CTRLr , &reg64_val,
                            ING_PRES_HFRM_SYNCf, field64_buf);
        COMPILER_64_SET(field64_buf, 0, config->rfrm_sync);
        soc_reg64_field_set(unit, CPRI_ING_PRES_TM_CTRLr , &reg64_val,
                            ING_PRES_RFRM_SYNCf, field64_buf);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_ING_PRES_TM_CTRLr(unit, port, reg64_val));

        /* set approximate increment disable setting. */
        _SOC_IF_ERR_EXIT(READ_CPRI_ING_TS_APPROX_INCr(unit, port, &reg_val));
        field_buf = config->approx_inc_disable? 1:0;
        soc_reg_field_set(unit, CPRI_ING_TS_APPROX_INCr, &reg_val,
                          ING_PRES_TS_APPROX_INC_DISABLEf, field_buf);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_ING_TS_APPROX_INCr(unit, port, reg_val));


    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Need Support for New Version.\n"));
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_ingress_presentation_time_config_get(int unit, int port, cprif_cpri_ingress_pres_time_ctrl_t* config)
{
    uint32 reg_val;
    uint64 reg64_val;
    uint64 rval64;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));

    if (CPRIF_CLPORT_IS_A0(pm_version)) {
        /*
         * Only A0, use 32 bits.
         */
        _SOC_IF_ERR_EXIT(READ_CPRI_ING_PRES_TM_CTRLr_REG32(unit, port, &reg_val));

        config->pres_mod_cnt = soc_reg_field_get(unit, CPRI_ING_PRES_TM_CTRLr, reg_val,
                                                 ING_PRES_MOD_CNTf);

        config->bfn_offset = soc_reg_field_get(unit, CPRI_ING_PRES_TM_CTRLr, reg_val,
                                               ING_PRES_BFN_OFFSETf);

        config->hfn_offset = soc_reg_field_get(unit, CPRI_ING_PRES_TM_CTRLr, reg_val,
                                               ING_PRES_HFN_OFFSETf);

        config->hfrm_sync = soc_reg_field_get(unit, CPRI_ING_PRES_TM_CTRLr, reg_val,
                                              ING_PRES_HFRM_SYNCf);

        config->rfrm_sync = soc_reg_field_get(unit, CPRI_ING_PRES_TM_CTRLr, reg_val,
                            ING_PRES_RFRM_SYNCf);

    } else if (CPRIF_CLPORT_IS_B0(pm_version)) {
        /*
         *  B0 use 64 bits.
         */
        _SOC_IF_ERR_EXIT(READ_CPRI_ING_PRES_TM_CTRLr(unit, port, &reg64_val));

        rval64 = soc_reg64_field_get(unit, CPRI_ING_PRES_TM_CTRLr, reg64_val,
                                                 ING_PRES_MOD_CNTf);
        COMPILER_64_TO_32_LO(config->pres_mod_cnt, rval64);

        rval64 = soc_reg64_field_get(unit, CPRI_ING_PRES_TM_CTRLr, reg64_val,
                                               ING_PRES_BFN_OFFSETf);
        COMPILER_64_TO_32_LO(config->bfn_offset, rval64);

        rval64 = soc_reg64_field_get(unit, CPRI_ING_PRES_TM_CTRLr, reg64_val,
                                               ING_PRES_HFN_OFFSETf);
        COMPILER_64_TO_32_LO(config->hfn_offset, rval64);

        rval64 = soc_reg64_field_get(unit, CPRI_ING_PRES_TM_CTRLr, reg64_val,
                                              ING_PRES_HFRM_SYNCf);
        COMPILER_64_TO_32_LO(config->hfrm_sync, rval64);

        rval64 = soc_reg64_field_get(unit, CPRI_ING_PRES_TM_CTRLr, reg64_val,
                                            ING_PRES_RFRM_SYNCf);
        COMPILER_64_TO_32_LO(config->rfrm_sync, rval64);
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Need Support for New Version.\n"));
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_ingress_presentation_time_adjust_set(int unit, int port, uint32 time)
{
    uint32 reg_val;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_ING_PRES_TM_OFFSETr(unit, port, &reg_val));
    field_buf = time;
    soc_reg_field_set(unit, CPRI_ING_PRES_TM_OFFSETr , &reg_val,
                      ING_PRES_TIME_OFFSETf, field_buf);
    _SOC_IF_ERR_EXIT(WRITE_CPRI_ING_PRES_TM_OFFSETr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_ingress_presentation_time_adjust_get(int unit, int port, uint32* time)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_ING_PRES_TM_OFFSETr(unit, port, &reg_val));
    *time = soc_reg_field_get(unit, CPRI_ING_PRES_TM_OFFSETr , reg_val,
                              ING_PRES_TIME_OFFSETf);

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_ingress_presentation_time_approximate_increment_set(int unit, int port, uint32 time)
{
    uint32 reg_val;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_ING_TS_APPROX_INCr(unit, port, &reg_val));
    field_buf = time;
    soc_reg_field_set(unit, CPRI_ING_TS_APPROX_INCr , &reg_val,
                      ING_PRES_TS_APPROX_INCf, field_buf);
    _SOC_IF_ERR_EXIT(WRITE_CPRI_ING_TS_APPROX_INCr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_ingress_presentation_time_approximate_increment_get(int unit, int port, uint32* time)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_ING_TS_APPROX_INCr(unit, port, &reg_val));
    *time = soc_reg_field_get(unit, CPRI_ING_TS_APPROX_INCr , reg_val,
                              ING_PRES_TS_APPROX_INCf);

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_egress_agnostic_mode_enable_set(int unit, int port, int enable, cprimod_cpri_roe_ordering_info_option_t mode)
{
    uint64 reg_val;
    uint64 field_buf;
    uint32 agnostic_mode = 0;
    uint32 buff_extend = 0;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

     _SOC_IF_ERR_EXIT(READ_CPRI_DECAP_AGNOSTIC_MODE_CTRLr(unit, port, &reg_val));

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));

    if (CPRIF_CLPORT_IS_A0(pm_version)) {
        if (mode != CPRIMOD_CPRI_ROE_ORDERING_INFO_OPTION_TIMESTAMP){
            LOG_CLI(("Forcing to timestamp Mode. This A0 HW only support Timestamp Mode.\n"));
        }
        COMPILER_64_SET(field_buf, 0, (uint32) enable ? 1: 0);
        soc_reg64_field_set(unit, CPRI_DECAP_AGNOSTIC_MODE_CTRLr , &reg_val,
                            EGR_AGNOSTIC_MODE_ENf, field_buf);
    } else if (CPRIF_CLPORT_IS_B0(pm_version)) {

        if (enable) {
            if (mode == CPRIMOD_CPRI_ROE_ORDERING_INFO_OPTION_TIMESTAMP){
                agnostic_mode   = 1;
                buff_extend      = 0;
            } else {/* Seq_Num */
                agnostic_mode   = 0;
                buff_extend      = 1;
            }
        } else { /* Disable */
            agnostic_mode   = 0;
            buff_extend      = 0;
        }

        COMPILER_64_SET(field_buf, 0, agnostic_mode);
        soc_reg64_field_set(unit, CPRI_DECAP_AGNOSTIC_MODE_CTRLr , &reg_val,
                            EGR_AGNOSTIC_MODE_ENf, field_buf);

        COMPILER_64_SET(field_buf, 0, buff_extend);
        soc_reg64_field_set(unit, CPRI_DECAP_AGNOSTIC_MODE_CTRLr , &reg_val,
                            EGR_BUFF_EXTENDf, field_buf);
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Need Support for New Version.\n"));
    }
    _SOC_IF_ERR_EXIT(WRITE_CPRI_DECAP_AGNOSTIC_MODE_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_egress_agnostic_mode_enable_get(int unit, int port, int* enable, cprimod_cpri_roe_ordering_info_option_t* mode)
{
    uint64 reg_val;
    uint64 field_buf;
    uint32 agnostic_mode;
    uint32 buff_extend;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_DECAP_AGNOSTIC_MODE_CTRLr(unit, port, &reg_val));

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));

    field_buf = soc_reg64_field_get(unit, CPRI_DECAP_AGNOSTIC_MODE_CTRLr , reg_val,
                                    EGR_AGNOSTIC_MODE_ENf);
    agnostic_mode = COMPILER_64_LO(field_buf);

    if (CPRIF_CLPORT_IS_A0(pm_version)) {
        *enable = agnostic_mode? 1 :0;
        /* In A0, only timestamp is supported. */
        *mode   = CPRIMOD_CPRI_ROE_ORDERING_INFO_OPTION_TIMESTAMP;
    } else if (CPRIF_CLPORT_IS_B0(pm_version)) {
        field_buf = soc_reg64_field_get(unit, CPRI_DECAP_AGNOSTIC_MODE_CTRLr , reg_val,
                                        EGR_BUFF_EXTENDf);
        buff_extend = COMPILER_64_LO(field_buf);
        if (agnostic_mode) {
            *enable = TRUE;
            *mode   = CPRIMOD_CPRI_ROE_ORDERING_INFO_OPTION_TIMESTAMP;
        } else if (buff_extend) {
            *enable = TRUE;
            *mode   = CPRIMOD_CPRI_ROE_ORDERING_INFO_OPTION_SEQ_NUM;
        } else {
            *enable = FALSE;
        }
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Need Support for New Version.\n"));
    }
exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_egress_agnostic_mode_config_set(int unit, int port, cprif_cpri_egress_agnostic_config_t* config)
{
    uint64 reg_val;
    uint64 field_buf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_DECAP_AGNOSTIC_MODE_CTRLr(unit, port, &reg_val));

    COMPILER_64_SET(field_buf, 0, (uint32) config->rd_ptr_max);
    soc_reg64_field_set(unit, CPRI_DECAP_AGNOSTIC_MODE_CTRLr , &reg_val,
                      EGR_AGNOSTIC_MODE_RD_PTR_MAXf, field_buf);

    COMPILER_64_SET(field_buf, 0, (uint32) config->queue_num);
    soc_reg64_field_set(unit, CPRI_DECAP_AGNOSTIC_MODE_CTRLr , &reg_val,
                      EGR_AGNOSTIC_MODE_QUEUE_NUMf, field_buf);

    COMPILER_64_SET(field_buf, 0, (uint32) config->mod_cnt);
    soc_reg64_field_set(unit, CPRI_DECAP_AGNOSTIC_MODE_CTRLr , &reg_val,
                      EGR_AGNOSTIC_MODE_MOD_CNTf, field_buf);

    _SOC_IF_ERR_EXIT(WRITE_CPRI_DECAP_AGNOSTIC_MODE_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_egress_agnostic_mode_config_get(int unit, int port, cprif_cpri_egress_agnostic_config_t* config)
{
    uint64 reg_val;
    uint64 field_buf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_DECAP_AGNOSTIC_MODE_CTRLr(unit, port, &reg_val));

    field_buf = soc_reg64_field_get(unit, CPRI_DECAP_AGNOSTIC_MODE_CTRLr , reg_val,
                                           EGR_AGNOSTIC_MODE_RD_PTR_MAXf);

    config->rd_ptr_max = COMPILER_64_LO(field_buf);
    field_buf = soc_reg64_field_get(unit, CPRI_DECAP_AGNOSTIC_MODE_CTRLr , reg_val,
                                          EGR_AGNOSTIC_MODE_QUEUE_NUMf);
    config->queue_num = COMPILER_64_LO(field_buf);
    field_buf = soc_reg64_field_get(unit, CPRI_DECAP_AGNOSTIC_MODE_CTRLr , reg_val,
                                        EGR_AGNOSTIC_MODE_MOD_CNTf);
    config->mod_cnt = COMPILER_64_LO(field_buf);
exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_egress_presentation_time_config_set(int unit, int port, uint32 mod_offset, uint32 mod_count)
{
    uint64 reg_val;
    uint64 field_buf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_DECAP_PRES_TIME_CTRLr(unit, port, &reg_val));
    COMPILER_64_SET(field_buf,0, mod_offset);
    soc_reg64_field_set(unit, CPRI_DECAP_PRES_TIME_CTRLr , &reg_val,
                      PRES_TIME_MOD_OFFSETf, field_buf);
    COMPILER_64_SET(field_buf,0, mod_count);
    soc_reg64_field_set(unit, CPRI_DECAP_PRES_TIME_CTRLr , &reg_val,
                      PRES_TIME_MOD_CNTf, field_buf);
    _SOC_IF_ERR_EXIT(WRITE_CPRI_DECAP_PRES_TIME_CTRLr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_egress_presentation_time_config_get(int unit, int port, uint32* mod_offset, uint32* mod_count)
{
    uint64 reg_val;
    uint64 field_buf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CPRI_DECAP_PRES_TIME_CTRLr(unit, port, &reg_val));
    field_buf = soc_reg64_field_get(unit, CPRI_DECAP_PRES_TIME_CTRLr , reg_val,
                                    PRES_TIME_MOD_OFFSETf);
    *mod_offset = COMPILER_64_LO(field_buf);

    field_buf = soc_reg64_field_get(unit, CPRI_DECAP_PRES_TIME_CTRLr , reg_val,
                                   PRES_TIME_MOD_CNTf);
    *mod_count = COMPILER_64_LO(field_buf);
exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_port_ecc_interrupt_status_get(int unit, int port,
                                    cprimod_cpri_ecc_intr_info_t *ecc_err_info)
{
    uint32 rval = 0;
    uint32 i = 0, err_addr = 0x0;
    int is_ecc_intr_enabled = 0;
    int ecc1b_err = 0, ecc2b_err = 0, ecc_multib_err = 0;
    _cpri_ecc_interrupts_info_t *intr_ptr = &_cpri_port_ecc_intrs_info[0];
    SOC_INIT_FUNC_DEFS;

    /*
     * Check if RX or TX ECC interrupts are enabled, currently
     * there is no selective RX or TX ECC interrupt enabling.
     * Checking either RX or TX ECC interrupt is ok to process
     * further.
     */
    _SOC_IF_ERR_EXIT(cprif_drv_cpri_port_rx_interrupt_enable_get(unit, port,
                                CPRIF_RX_ECC_ERR_INTR, &is_ecc_intr_enabled));

    if (!is_ecc_intr_enabled) {
        /* ecc interrupt not enabled skip further processing */
        return SOC_E_NONE;
    }
    /* Init the ecc error type to none */
    ecc_err_info->ecc_num_bits_err = CPRIMOD_PM_ECC_ERR_NONE;

    while (intr_ptr->ecc_status_reg != INVALIDr) {
        _SOC_IF_ERR_EXIT(soc_reg32_get(unit, intr_ptr->ecc_status_reg,
                                       port, 0, &rval));

        /* Check if 1-bit ECC error */
        if (intr_ptr->ecc1b_err_status_fld != INVALIDf) {
            ecc1b_err = soc_reg_field_get(unit, intr_ptr->ecc_status_reg,
                                          rval, intr_ptr->ecc1b_err_status_fld);
        }

        /* Check if 2-bit ECC error */
        if (intr_ptr->ecc2b_err_status_fld != INVALIDf) {
            ecc2b_err = soc_reg_field_get(unit, intr_ptr->ecc_status_reg,
                                          rval, intr_ptr->ecc2b_err_status_fld);
        }

        /* Check if multi-bit ECC error */
        if (intr_ptr->ecc_multib_err_status_fld != INVALIDf) {
            ecc_multib_err = soc_reg_field_get(unit, intr_ptr->ecc_status_reg,
                                          rval,
                                          intr_ptr->ecc_multib_err_status_fld);
        }

        /* Get the address */
        if (ecc1b_err || ecc2b_err || ecc_multib_err) {
            err_addr = soc_reg_field_get(unit, intr_ptr->ecc_status_reg,
                                          rval, intr_ptr->ecc_event_addr);

            ecc_err_info->port = port;
            ecc_err_info->err_mem_info = intr_ptr->mem;
            ecc_err_info->ecc_reg_info = intr_ptr->ecc_status_reg;
            ecc_err_info->ecc_num_bits_err = (ecc1b_err)?
                                   CPRIMOD_PM_ECC_ERR_1_BIT: (ecc2b_err)?
                                   CPRIMOD_PM_ECC_ERR_2_BIT:
                                   CPRIMOD_PM_ECC_ERR_MULTI_BIT;
            ecc_err_info->err_addr = err_addr;
            ecc_err_info->mem_str = intr_ptr->mem_str;
        }

        /* Clear status if 1-bit ECC error */
        if (ecc1b_err) {
            soc_reg_field_set(unit, intr_ptr->ecc_status_reg,
                              &rval, intr_ptr->ecc1b_err_status_fld, 1);
        }

        /* Clear status if 2-bit ECC error */
        if (ecc2b_err) {
            soc_reg_field_set(unit, intr_ptr->ecc_status_reg,
                              &rval, intr_ptr->ecc2b_err_status_fld, 1);
        }

        /* Clear status if multi-bit ECC error */
        if (ecc_multib_err) {
            soc_reg_field_set(unit, intr_ptr->ecc_status_reg,
                              &rval, intr_ptr->ecc_multib_err_status_fld, 1);
        }

        _SOC_IF_ERR_EXIT(soc_reg32_set(unit, intr_ptr->ecc_status_reg,
                                       port, 0, rval));
        i++;

        intr_ptr = &_cpri_port_ecc_intrs_info[i];

        if (ecc1b_err || ecc2b_err || ecc_multib_err) {
            LOG_VERBOSE(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit,
                    "[i-%d] [1b-%d] [2b-%d] [mb-%d] [addr-%x]\n\n"),
                        i, ecc1b_err, ecc2b_err,
                        ecc_multib_err, err_addr));
            break;
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_port_ecc_interrupt_enable_set(int unit, int port, int enable)
{
    uint32 rval = 0;
    uint32 i = 0;
    _cpri_ecc_interrupts_info_t *intr_ptr = &_cpri_port_ecc_intrs_info[0];
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprif_drv_cpri_port_rx_interrupts_enable_set(unit, port,
                                CPRIF_RX_ECC_ERR_INTR, enable));

    _SOC_IF_ERR_EXIT(cprif_drv_cpri_port_tx_interrupts_enable_set(unit, port,
                                CPRIF_TX_ECC_ERR_INTR, enable));

    while (intr_ptr->ecc_en_reg != INVALIDr) {
        _SOC_IF_ERR_EXIT(soc_reg32_get(unit, intr_ptr->ecc_en_reg,
                                       port, 0, &rval));

        /* Enable 1-bit ECC error reporting */
        if (intr_ptr->ecc1b_en_fld != INVALIDf) {
            soc_reg_field_set(unit, intr_ptr->ecc_en_reg,
                              &rval, intr_ptr->ecc1b_en_fld,
                              enable);
        }

        /* Enable 2-bit ECC error reporting */
        if (intr_ptr->ecc2b_en_fld != INVALIDf) {
            soc_reg_field_set(unit, intr_ptr->ecc_en_reg,
                              &rval, intr_ptr->ecc2b_en_fld,
                              enable);
        }
        _SOC_IF_ERR_EXIT(soc_reg32_set(unit, intr_ptr->ecc_en_reg,
                                       port, 0, rval));
        intr_ptr = &_cpri_port_ecc_intrs_info[++i];
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_port_interrupt_type_num_subid_get(int unit, int port,
                                                     cprimod_cpri_port_intr_type_t intr,
                                                     int* num_subid)

{
    SOC_INIT_FUNC_DEFS;
    if (!(intr < _shrCpriIntrCount)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Invalid interrupt type(%d)", intr));
    }
    *num_subid = _cpri_port_intrs_info[intr].max_id;
exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_port_interrupt_type_enable_set(int unit, int port,
                                                  cprimod_cpri_port_intr_type_t intr,
                                                  int data, int enable)
{
    uint32 rval = 0;
    uint64 rval64;
    SOC_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(rval64);
    if (!(intr < _shrCpriIntrCount)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Invalid interrupt type(%d)", intr));
    }

    /* check if the register is 64-bit */
    if (_cpri_port_intrs_info[intr].is_64bit) {
        _SOC_IF_ERR_EXIT(soc_reg64_get(unit,
                         _cpri_port_intrs_info[intr].enable_reg,
                         port, 0, &rval64));
        /* Check if the programming is queue num or AxC id based */
        if (_cpri_port_intrs_info[intr].max_id != 0) {
            if (enable) {
                COMPILER_64_BITSET(rval64, data);
            } else {
                COMPILER_64_BITCLR(rval64, data);
            }
        } else {
            /* set the field */
            soc_reg64_field32_set(unit, _cpri_port_intrs_info[intr].enable_reg,
                         &rval64, _cpri_port_intrs_info[intr].enable_field,
                         enable);
        }
        _SOC_IF_ERR_EXIT(soc_reg64_set(unit,
                             _cpri_port_intrs_info[intr].enable_reg,
                             port, 0, rval64));
    } else {
        _SOC_IF_ERR_EXIT(soc_reg32_get(unit,
                             _cpri_port_intrs_info[intr].enable_reg,
                             port, 0, &rval));
        /* Check if the programming is queue num based */
        if (_cpri_port_intrs_info[intr].max_id != 0) {
            rval |= (1 << data);
        } else {
            /* set the field */
            soc_reg_field_set(unit, _cpri_port_intrs_info[intr].enable_reg,
                              &rval,
                              _cpri_port_intrs_info[intr].enable_field, enable);
        }
        _SOC_IF_ERR_EXIT(soc_reg32_set(unit,
                     _cpri_port_intrs_info[intr].enable_reg, port, 0, rval));
    }
exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_port_interrupt_type_enable_get(int unit, int port,
                                            cprimod_cpri_port_intr_type_t intr,
                                            int data, int *enable)
{
    uint32 rval = 0;
    uint64 rval64;
    SOC_INIT_FUNC_DEFS;

    if (!(intr < _shrCpriIntrCount)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Invalid interrupt type(%d)", intr));
    }

    /* check if the register is 64-bit */
    COMPILER_64_ZERO(rval64);
    if (_cpri_port_intrs_info[intr].is_64bit) {
        _SOC_IF_ERR_EXIT(soc_reg64_get(unit,
                         _cpri_port_intrs_info[intr].enable_reg,
                         port, 0, &rval64));
        /* Check if the programming is queue num or AxC id based */
        if (_cpri_port_intrs_info[intr].max_id != 0) {
            *enable = COMPILER_64_BITTEST(rval64, data);
        } else {
            /* set the field */
            *enable = soc_reg64_field32_get(unit,
                                _cpri_port_intrs_info[intr].enable_reg, rval64,
                                _cpri_port_intrs_info[intr].enable_field);
        }
    } else {
        _SOC_IF_ERR_EXIT(soc_reg32_get(unit,
                             _cpri_port_intrs_info[intr].enable_reg,
                             port, 0, &rval));
        /* Check if the programming is queue num based */
        if (_cpri_port_intrs_info[intr].max_id != 0) {
            *enable = (rval & (1 << data));
        } else {
            /* set the field */
            *enable = soc_reg_field_get(unit,
                                _cpri_port_intrs_info[intr].enable_reg, rval,
                                _cpri_port_intrs_info[intr].enable_field);
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_port_interrupt_type_status_get(int unit, int port,
                                            cprimod_cpri_port_intr_type_t intr,
                                            int data, int *status)
{
    uint32 rval = 0;
    uint32 clear32 = 0;
    uint64 rval64;
    uint64 clear64;
    SOC_INIT_FUNC_DEFS;

    if (!(intr < _shrCpriIntrCount)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Invalid interrupt type(%d)", intr));
    }

    /* check if the register is 64-bit */
    COMPILER_64_ZERO(rval64);
    COMPILER_64_ZERO(clear64);
    if (_cpri_port_intrs_info[intr].is_status_64bit) {
        _SOC_IF_ERR_EXIT(soc_reg64_get(unit,
                         _cpri_port_intrs_info[intr].status_reg,
                         port, 0, &rval64));
        /* Check if the programming is queue num or AxC id based */
        if (_cpri_port_intrs_info[intr].max_id != 0) {
            *status = COMPILER_64_BITTEST(rval64, data);
            COMPILER_64_BITSET(clear64, data);
        } else {
            /* get the field */
            *status = soc_reg64_field32_get(unit,
                                _cpri_port_intrs_info[intr].status_reg, rval64,
                                _cpri_port_intrs_info[intr].status_field);

            /* prep to clear the status w1tc bit. */
            soc_reg64_field32_set(unit,
                                _cpri_port_intrs_info[intr].status_reg, &clear64,
                                _cpri_port_intrs_info[intr].status_field,1);
        }

        if (*status) {
            _SOC_IF_ERR_EXIT
                (soc_reg64_set(unit,
                               _cpri_port_intrs_info[intr].status_reg,
                               port, 0, clear64));
        }
    } else {
        _SOC_IF_ERR_EXIT(soc_reg32_get(unit,
                             _cpri_port_intrs_info[intr].status_reg,
                             port, 0, &rval));
        /* Check if the programming is queue num based */
        if (_cpri_port_intrs_info[intr].max_id != 0) {
            *status = (rval & (1 << data));
            clear32 = (1 << data);
        } else {
            /* set the field */
            *status = soc_reg_field_get(unit,
                                _cpri_port_intrs_info[intr].status_reg, rval,
                                _cpri_port_intrs_info[intr].status_field);
            /* prep to clear w1tc bit. */
            soc_reg_field_set(unit,
                              _cpri_port_intrs_info[intr].status_reg, &clear32,
                              _cpri_port_intrs_info[intr].status_field,1);
        }
        if (*status) {
            _SOC_IF_ERR_EXIT
                (soc_reg32_set(unit,
                               _cpri_port_intrs_info[intr].status_reg,
                               port, 0, clear32));
        }
    }

exit:
    SOC_FUNC_RETURN;
}


int cprif_drv_compression_entry_set(int unit, int port,
                                    int index,
                                    uint32 comp_val_0,
                                    uint32 comp_val_1)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_COMP_TAB_0m,
                               CPRI_DECAP_COMP_TAB_1m,
                               CPRI_DECAP_COMP_TAB_2m,
                               CPRI_DECAP_COMP_TAB_3m};

    uint32 *wr_data_ptr = NULL;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_COMPRESSION_TABLE_SIZE) {
        _SOC_EXIT_WITH_ERR
            (SOC_E_PARAM, ("Compression table unsuppoprted index %d.\n",index));
    }

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    field_buf = comp_val_0;
    soc_mem_field_set(unit, local_mem, wr_data_ptr,COMP_VAL_0f, &field_buf);
    field_buf = comp_val_1;
    soc_mem_field_set(unit, local_mem, wr_data_ptr,COMP_VAL_1f, &field_buf);


    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;

}

int cprif_drv_compression_entry_get(int unit, int port,
                                              int index,
                                              uint32* comp_val_0,
                                              uint32* comp_val_1)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_COMP_TAB_0m,
                               CPRI_DECAP_COMP_TAB_1m,
                               CPRI_DECAP_COMP_TAB_2m,
                               CPRI_DECAP_COMP_TAB_3m};
    uint32 *rd_data_ptr = NULL;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_COMPRESSION_TABLE_SIZE) {
        _SOC_EXIT_WITH_ERR
            (SOC_E_PARAM, ("Compression table unsuppoprted index %d.\n",index));
    }

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, COMP_VAL_0f, &field_buf);
    *comp_val_0 = field_buf;

    soc_mem_field_get(unit, local_mem, rd_data_ptr, COMP_VAL_1f, &field_buf);
    *comp_val_1 = field_buf;
exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_decompression_entry_set(int unit, int port,
                                      int index,
                                      uint32 decomp_val)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_ULAW_LUT_0m,
                               CPRI_DECAP_ULAW_LUT_1m,
                               CPRI_DECAP_ULAW_LUT_2m,
                               CPRI_DECAP_ULAW_LUT_3m};

    uint32 *wr_data_ptr = NULL;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_DECOMPRESSION_TABLE_SIZE) {
        _SOC_EXIT_WITH_ERR
            (SOC_E_PARAM, ("DeCompression table unsuppoprted index %d.\n",index));
    }

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    wr_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(wr_data_ptr);
    sal_memset(wr_data_ptr, 0, sizeof(uint32) * num_words);

    field_buf = decomp_val;
    soc_mem_field_set(unit, local_mem, wr_data_ptr,DECOMP_VALf, &field_buf);


    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, wr_data_ptr));

exit:
    sal_free(wr_data_ptr);
    SOC_FUNC_RETURN;

}

int cprif_drv_decompression_entry_get(int unit, int port,
                                      int index,
                                      uint32* decomp_val)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = { CPRI_DECAP_ULAW_LUT_0m,
                               CPRI_DECAP_ULAW_LUT_1m,
                               CPRI_DECAP_ULAW_LUT_2m,
                               CPRI_DECAP_ULAW_LUT_3m};
    uint32 *rd_data_ptr = NULL;
    uint32 num_words;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_DECOMPRESSION_TABLE_SIZE) {
        _SOC_EXIT_WITH_ERR
            (SOC_E_PARAM, ("DeCompression table unsuppoprted index %d.\n",index));
    }

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, DECOMP_VALf, &field_buf);
    *decomp_val = field_buf;

exit:
    sal_free(rd_data_ptr);
    SOC_FUNC_RETURN;
}

int cprif_drv_decap_queue_flow_control_entry_set(int unit, int port,
                                                 int index,
                                                 int enable,
                                                 uint32 queue_num,
                                                 uint32 xon_buff_count,
                                                 uint32 xoff_buff_count)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_DECAP_FLOW_CONTROL_0m,
                              CPRI_DECAP_FLOW_CONTROL_1m,
                              CPRI_DECAP_FLOW_CONTROL_2m,
                              CPRI_DECAP_FLOW_CONTROL_3m};
    uint32 wr_data=0;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_CONTROL_FLOW_TABLE_SIZE) {
        _SOC_EXIT_WITH_ERR
            (SOC_E_PARAM, ("Flow Control table unsuppoprted index %d.\n",index));
    }

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];


    field_buf = enable? 1:0;
    soc_mem_field_set(unit, local_mem, &wr_data, FLOW_CONTROL_ENf, &field_buf);

    field_buf = queue_num;
    soc_mem_field_set(unit, local_mem, &wr_data, QUEUE_NUMf, &field_buf);

    field_buf = xon_buff_count;
    soc_mem_field_set(unit, local_mem, &wr_data, XONf, &field_buf);

    field_buf = xoff_buff_count;
    soc_mem_field_set(unit, local_mem, &wr_data, XOFFf, &field_buf);

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, index, &wr_data));

exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_decap_queue_flow_control_entry_get(int unit, int port,
                                                 int index,
                                                 int* enable,
                                                 uint32* queue_num,
                                                 uint32* xon_buff_count,
                                                 uint32* xoff_buff_count)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_DECAP_FLOW_CONTROL_0m,
                              CPRI_DECAP_FLOW_CONTROL_1m,
                              CPRI_DECAP_FLOW_CONTROL_2m,
                              CPRI_DECAP_FLOW_CONTROL_3m};
    uint32 rd_data=0;
    uint32 field_buf;
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_CONTROL_FLOW_TABLE_SIZE) {
        _SOC_EXIT_WITH_ERR
            (SOC_E_PARAM, ("Flow Control table unsuppoprted index %d.\n",index));
    }

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];


    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk, index, &rd_data));

    soc_mem_field_get(unit, local_mem, &rd_data, FLOW_CONTROL_ENf, &field_buf);
    *enable = field_buf;

    soc_mem_field_get(unit, local_mem, &rd_data, QUEUE_NUMf, &field_buf);
    *queue_num = field_buf;

    soc_mem_field_get(unit, local_mem, &rd_data, XONf, &field_buf);
    *xon_buff_count = field_buf;

    soc_mem_field_get(unit, local_mem, &rd_data, XOFFf, &field_buf);
    *xoff_buff_count = field_buf;
exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_port_fec_enable_set(int unit, int port, cprimod_direction_t dir, int enable)
{
    uint64 data;
    SOC_INIT_FUNC_DEFS;

    if (dir == CPRIMOD_DIR_RX) {
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_FEC_CL91_CTRLr(unit, port, &data));
        soc_reg64_field32_set(unit, CPRI_RXPCS_FEC_CL91_CTRLr, &data, RXPCS_FEC_CL91_ENABLEf, (uint32) enable);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_RXPCS_FEC_CL91_CTRLr(unit, port, data));
    } else {
        _SOC_IF_ERR_EXIT(READ_CPRI_TXPCS_FEC_CL91_CTRLr(unit, port, &data));
        soc_reg64_field32_set(unit, CPRI_TXPCS_FEC_CL91_CTRLr, &data, TXPCS_FEC_CL91_ENABLEf, (uint32) enable);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_TXPCS_FEC_CL91_CTRLr(unit, port, data));
    }
exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_port_fec_enable_get(int unit, int port, cprimod_direction_t dir, int* enable)
{
    uint64 data;
    SOC_INIT_FUNC_DEFS;

    if (dir == CPRIMOD_DIR_RX) {
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_FEC_CL91_CTRLr(unit, port, &data));
        *enable = soc_reg64_field32_get(unit, CPRI_RXPCS_FEC_CL91_CTRLr, data, RXPCS_FEC_CL91_ENABLEf);
    } else {
        _SOC_IF_ERR_EXIT(READ_CPRI_TXPCS_FEC_CL91_CTRLr(unit, port, &data));
        *enable = soc_reg64_field32_get(unit, CPRI_TXPCS_FEC_CL91_CTRLr, data, TXPCS_FEC_CL91_ENABLEf);
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_port_fec_timestamp_config_set(int unit, int port, cprimod_direction_t dir, const cprimod_fec_timestamp_config_t* config)
{
    uint64 data;
    uint32 source_select=0;
    SOC_INIT_FUNC_DEFS;

    if (dir == CPRIMOD_DIR_RX) {
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_CL91_TS_CTRLr(unit, port, &data));
        soc_reg64_field32_set(unit, CPRI_RXPCS_CL91_TS_CTRLr, &data, CL91_TS_ENf,                 config->enable);

        source_select = (config->fec_fifo_ts_source_select == CPRIMOD_CPRI_FEC_TS_SOURCE_257B)? 1:0;
        soc_reg64_field32_set(unit, CPRI_RXPCS_CL91_TS_CTRLr, &data, SEND_CL91_257B_TS_TO_TSFIFOf, source_select);

        source_select = (config->fec_cmic_ts_source_select == CPRIMOD_CPRI_FEC_TS_SOURCE_66B)? 1:0;
        soc_reg64_field32_set(unit, CPRI_RXPCS_CL91_TS_CTRLr, &data, SEND_CL91_66B_TS_TO_CMICf,    source_select);

        soc_reg64_field32_set(unit, CPRI_RXPCS_CL91_TS_CTRLr, &data, CB_3_BIT_OFFSETf,            config->control_bit_offset_0);
        soc_reg64_field32_set(unit, CPRI_RXPCS_CL91_TS_CTRLr, &data, CB_2_BIT_OFFSETf,            config->control_bit_offset_1);
        soc_reg64_field32_set(unit, CPRI_RXPCS_CL91_TS_CTRLr, &data, CB_1_BIT_OFFSETf,            config->control_bit_offset_2);
        soc_reg64_field32_set(unit, CPRI_RXPCS_CL91_TS_CTRLr, &data, CB_0_BIT_OFFSETf,            config->control_bit_offset_3);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_RXPCS_CL91_TS_CTRLr(unit, port, data));
    } else {
        _SOC_IF_ERR_EXIT(READ_CPRI_TXPCS_CL91_TS_CTRLr(unit, port, &data));
        soc_reg64_field32_set(unit, CPRI_TXPCS_CL91_TS_CTRLr, &data, CL91_TS_ENf,                 config->enable);
        soc_reg64_field32_set(unit, CPRI_TXPCS_CL91_TS_CTRLr, &data, SEND_CL91_257B_TS_TO_TSFIFOf,(uint32)config->fec_fifo_ts_source_select);
        soc_reg64_field32_set(unit, CPRI_TXPCS_CL91_TS_CTRLr, &data, SEND_CL91_66B_TS_TO_CMICf,   (uint32)config->fec_cmic_ts_source_select);
        soc_reg64_field32_set(unit, CPRI_TXPCS_CL91_TS_CTRLr, &data, CB_3_BIT_OFFSETf,            config->control_bit_offset_0);
        soc_reg64_field32_set(unit, CPRI_TXPCS_CL91_TS_CTRLr, &data, CB_2_BIT_OFFSETf,            config->control_bit_offset_1);
        soc_reg64_field32_set(unit, CPRI_TXPCS_CL91_TS_CTRLr, &data, CB_1_BIT_OFFSETf,            config->control_bit_offset_2);
        soc_reg64_field32_set(unit, CPRI_TXPCS_CL91_TS_CTRLr, &data, CB_0_BIT_OFFSETf,            config->control_bit_offset_3);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_TXPCS_CL91_TS_CTRLr(unit, port, data));
    }
exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_port_fec_timestamp_config_get(int unit, int port, cprimod_direction_t dir, cprimod_fec_timestamp_config_t* config)
{
    uint64 data;
    SOC_INIT_FUNC_DEFS;

    if (dir == CPRIMOD_DIR_RX) {
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_CL91_TS_CTRLr(unit, port, &data));
        config->enable = soc_reg64_field32_get(unit, CPRI_RXPCS_CL91_TS_CTRLr, data, CL91_TS_ENf);
        config->fec_fifo_ts_source_select = soc_reg64_field32_get(unit, CPRI_RXPCS_CL91_TS_CTRLr, data, SEND_CL91_257B_TS_TO_TSFIFOf);
        if (config->fec_fifo_ts_source_select == 1) {
            config->fec_fifo_ts_source_select = CPRIMOD_CPRI_FEC_TS_SOURCE_257B;
        } else {
            config->fec_fifo_ts_source_select = CPRIMOD_CPRI_FEC_TS_SOURCE_66B;
        }
        config->fec_cmic_ts_source_select = soc_reg64_field32_get(unit, CPRI_RXPCS_CL91_TS_CTRLr, data, SEND_CL91_66B_TS_TO_CMICf);
        if (config->fec_cmic_ts_source_select ==1) {
            config->fec_cmic_ts_source_select = CPRIMOD_CPRI_FEC_TS_SOURCE_66B;
        } else {
            config->fec_cmic_ts_source_select = CPRIMOD_CPRI_FEC_TS_SOURCE_257B;
        }

        config->control_bit_offset_0 = soc_reg64_field32_get(unit, CPRI_RXPCS_CL91_TS_CTRLr, data, CB_3_BIT_OFFSETf);
        config->control_bit_offset_1 = soc_reg64_field32_get(unit, CPRI_RXPCS_CL91_TS_CTRLr, data, CB_2_BIT_OFFSETf);
        config->control_bit_offset_2 = soc_reg64_field32_get(unit, CPRI_RXPCS_CL91_TS_CTRLr, data, CB_1_BIT_OFFSETf);
        config->control_bit_offset_3 = soc_reg64_field32_get(unit, CPRI_RXPCS_CL91_TS_CTRLr, data, CB_0_BIT_OFFSETf);
    } else {
        _SOC_IF_ERR_EXIT(READ_CPRI_TXPCS_CL91_TS_CTRLr(unit, port, &data));
        config->enable = soc_reg64_field32_get(unit, CPRI_TXPCS_CL91_TS_CTRLr, data, CL91_TS_ENf);
        config->fec_fifo_ts_source_select = soc_reg64_field32_get(unit, CPRI_TXPCS_CL91_TS_CTRLr, data, SEND_CL91_257B_TS_TO_TSFIFOf);
        config->fec_cmic_ts_source_select = soc_reg64_field32_get(unit, CPRI_TXPCS_CL91_TS_CTRLr, data, SEND_CL91_66B_TS_TO_CMICf);
        config->control_bit_offset_0 = soc_reg64_field32_get(unit, CPRI_TXPCS_CL91_TS_CTRLr, data, CB_3_BIT_OFFSETf);
        config->control_bit_offset_1 = soc_reg64_field32_get(unit, CPRI_TXPCS_CL91_TS_CTRLr, data, CB_2_BIT_OFFSETf);
        config->control_bit_offset_2 = soc_reg64_field32_get(unit, CPRI_TXPCS_CL91_TS_CTRLr, data, CB_1_BIT_OFFSETf);
        config->control_bit_offset_3 = soc_reg64_field32_get(unit, CPRI_TXPCS_CL91_TS_CTRLr, data, CB_0_BIT_OFFSETf);
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_port_fec_aux_config_set(int unit, int port, cprimod_fec_aux_config_para_t parameter_id, uint32 value)
{
    uint64 data;
    SOC_INIT_FUNC_DEFS;

    switch(parameter_id) {
    case CPRIMOD_CPRI_FEC_AUX_CONFIG_SCRAMBLE_BYPASS_TX:
        _SOC_IF_ERR_EXIT(READ_CPRI_TXPCS_FEC_CL91_CTRLr(unit, port, &data));
        soc_reg64_field32_set(unit, CPRI_TXPCS_FEC_CL91_CTRLr, &data, TXPCS_FEC_CL91_SCR_BYPASSf, value);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_TXPCS_FEC_CL91_CTRLr(unit, port, data));
        break;
    case CPRIMOD_CPRI_FEC_AUX_CONFIG_DESCRAMBLE_BYPASS_RX:
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_FEC_1_CTRLr(unit, port, &data));
        soc_reg64_field32_set(unit, CPRI_RXPCS_FEC_1_CTRLr, &data, RXPCS_FEC_CS_CW_DESCRAMBLE_BYPASSf, value);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_RXPCS_FEC_1_CTRLr(unit, port, data));
        break;
    case CPRIMOD_CPRI_FEC_AUX_CONFIG_BYPASS_INDICATION_ENABLE:
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_FEC_2_CTRLr(unit, port, &data));
        soc_reg64_field32_set(unit, CPRI_RXPCS_FEC_2_CTRLr, &data, RXPCS_FEC_DECODER_BYPASS_INDICATION_ENABLEf, value);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_RXPCS_FEC_2_CTRLr(unit, port, data));
        break;
    case CPRIMOD_CPRI_FEC_AUX_CONFIG_BYPASS_CORRECTION_ENABLE:
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_FEC_2_CTRLr(unit, port, &data));
        soc_reg64_field32_set(unit, CPRI_RXPCS_FEC_2_CTRLr, &data, RXPCS_FEC_DECODER_BYPASS_CORRECTION_ENABLEf, value);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_RXPCS_FEC_2_CTRLr(unit, port, data));
        break;
    case CPRIMOD_CPRI_FEC_AUX_CONFIG_CORRUPT_ALTERNATE_BLOCK_ENABLE:
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_FEC_3_CTRLr(unit, port, &data));
        soc_reg64_field32_set(unit, CPRI_RXPCS_FEC_3_CTRLr, &data, RXPCS_FEC_GBOX_CORRUPT_ALTERNATE_BLOCKS_ENABLEf, value);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_RXPCS_FEC_3_CTRLr(unit, port, data));
        break;
    case CPRIMOD_CPRI_FEC_AUX_CONFIG_SYMBOL_ERROR_CORRUPT_ENABLE:
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_FEC_3_CTRLr(unit, port, &data));
        soc_reg64_field32_set(unit, CPRI_RXPCS_FEC_3_CTRLr, &data, RXPCS_FEC_GBOX_SYMBOL_ERROR_CORRUPT_ENABLEf, value);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_RXPCS_FEC_3_CTRLr(unit, port, data));
        break;
    case CPRIMOD_CPRI_FEC_AUX_CONFIG_SYMBOL_ERROR_TIMER_PERIOD:
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_FEC_3_CTRLr(unit, port, &data));
        soc_reg64_field32_set(unit, CPRI_RXPCS_FEC_3_CTRLr, &data, RXPCS_FEC_GBOX_SYMBOL_ERROR_TMR_PERIODf, value);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_RXPCS_FEC_3_CTRLr(unit, port, data));
        break;
    case CPRIMOD_CPRI_FEC_AUX_CONFIG_SYMBOL_ERROR_COUNT_THRESHOLD:
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_FEC_3_CTRLr(unit, port, &data));
        soc_reg64_field32_set(unit, CPRI_RXPCS_FEC_3_CTRLr, &data, RXPCS_FEC_GBOX_SYMBOL_ERR_CNT_THRESHOLDf, value);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_RXPCS_FEC_3_CTRLr(unit, port, data));
        break;
    case CPRIMOD_CPRI_FEC_AUX_CONFIG_SYMBOL_ERROR_WINDOW:
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_FEC_3_CTRLr(unit, port, &data));
        soc_reg64_field32_set(unit, CPRI_RXPCS_FEC_3_CTRLr, &data, RXPCS_FEC_GBOX_SET_SYMB_ERR_WINDOW_128f, value);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_RXPCS_FEC_3_CTRLr(unit, port, data));
        break;
    case CPRIMOD_CPRI_FEC_AUX_CONFIG_TICK_DENOMINATOR:
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_FEC_4_CTRLr(unit, port, &data));
        soc_reg64_field32_set(unit, CPRI_RXPCS_FEC_4_CTRLr, &data, RXPCS_FEC_GBOX_TICK_DENOMf, value);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_RXPCS_FEC_4_CTRLr(unit, port, data));
        break;
    case CPRIMOD_CPRI_FEC_AUX_CONFIG_TICK_NUMERATOR:
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_FEC_4_CTRLr(unit, port, &data));
        soc_reg64_field32_set(unit, CPRI_RXPCS_FEC_4_CTRLr, &data, RXPCS_FEC_GBOX_TICK_NUMERf, value);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_RXPCS_FEC_4_CTRLr(unit, port, data));
        break;
    case CPRIMOD_CPRI_FEC_AUX_CONFIG_SOFT_RESET:
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_FEC_0_CTRLr(unit, port, &data));
        soc_reg64_field32_set(unit, CPRI_RXPCS_FEC_0_CTRLr, &data, RXPCS_FEC_SOFT_RESETf, value);
        _SOC_IF_ERR_EXIT(WRITE_CPRI_RXPCS_FEC_0_CTRLr(unit, port, data));
        break;
    default :
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Bad Aux config"));
        break;
    }
exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_port_fec_aux_config_get(int unit, int port, cprimod_fec_aux_config_para_t parameter_id, uint32* value)
{
    uint64 data;
    SOC_INIT_FUNC_DEFS;

    switch(parameter_id) {
    case CPRIMOD_CPRI_FEC_AUX_CONFIG_SCRAMBLE_BYPASS_TX:
        _SOC_IF_ERR_EXIT(READ_CPRI_TXPCS_FEC_CL91_CTRLr(unit, port, &data));
        *value = soc_reg64_field32_get(unit, CPRI_TXPCS_FEC_CL91_CTRLr, data, TXPCS_FEC_CL91_SCR_BYPASSf);
        break;
    case CPRIMOD_CPRI_FEC_AUX_CONFIG_DESCRAMBLE_BYPASS_RX:
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_FEC_1_CTRLr(unit, port, &data));
        *value = soc_reg64_field32_get(unit, CPRI_RXPCS_FEC_1_CTRLr, data, RXPCS_FEC_CS_CW_DESCRAMBLE_BYPASSf);
        break;
    case CPRIMOD_CPRI_FEC_AUX_CONFIG_BYPASS_INDICATION_ENABLE:
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_FEC_2_CTRLr(unit, port, &data));
        *value = soc_reg64_field32_get(unit, CPRI_RXPCS_FEC_2_CTRLr, data, RXPCS_FEC_DECODER_BYPASS_INDICATION_ENABLEf);
        break;
    case CPRIMOD_CPRI_FEC_AUX_CONFIG_BYPASS_CORRECTION_ENABLE:
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_FEC_2_CTRLr(unit, port, &data));
        *value = soc_reg64_field32_get(unit, CPRI_RXPCS_FEC_2_CTRLr, data, RXPCS_FEC_DECODER_BYPASS_CORRECTION_ENABLEf);
        break;
    case CPRIMOD_CPRI_FEC_AUX_CONFIG_CORRUPT_ALTERNATE_BLOCK_ENABLE:
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_FEC_3_CTRLr(unit, port, &data));
        *value = soc_reg64_field32_get(unit, CPRI_RXPCS_FEC_3_CTRLr, data, RXPCS_FEC_GBOX_CORRUPT_ALTERNATE_BLOCKS_ENABLEf);
        break;
    case CPRIMOD_CPRI_FEC_AUX_CONFIG_SYMBOL_ERROR_CORRUPT_ENABLE:
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_FEC_3_CTRLr(unit, port, &data));
        *value = soc_reg64_field32_get(unit, CPRI_RXPCS_FEC_3_CTRLr, data, RXPCS_FEC_GBOX_SYMBOL_ERROR_CORRUPT_ENABLEf);
        break;
    case CPRIMOD_CPRI_FEC_AUX_CONFIG_SYMBOL_ERROR_TIMER_PERIOD:
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_FEC_3_CTRLr(unit, port, &data));
        *value = soc_reg64_field32_get(unit, CPRI_RXPCS_FEC_3_CTRLr, data, RXPCS_FEC_GBOX_SYMBOL_ERROR_TMR_PERIODf);
        break;
    case CPRIMOD_CPRI_FEC_AUX_CONFIG_SYMBOL_ERROR_COUNT_THRESHOLD:
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_FEC_3_CTRLr(unit, port, &data));
        *value = soc_reg64_field32_get(unit, CPRI_RXPCS_FEC_3_CTRLr, data, RXPCS_FEC_GBOX_SYMBOL_ERR_CNT_THRESHOLDf);
        break;
    case CPRIMOD_CPRI_FEC_AUX_CONFIG_SYMBOL_ERROR_WINDOW:
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_FEC_3_CTRLr(unit, port, &data));
        *value = soc_reg64_field32_get(unit, CPRI_RXPCS_FEC_3_CTRLr, data, RXPCS_FEC_GBOX_SET_SYMB_ERR_WINDOW_128f);
        break;
    case CPRIMOD_CPRI_FEC_AUX_CONFIG_TICK_DENOMINATOR:
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_FEC_4_CTRLr(unit, port, &data));
        *value = soc_reg64_field32_get(unit, CPRI_RXPCS_FEC_4_CTRLr, data, RXPCS_FEC_GBOX_TICK_DENOMf);
        break;
    case CPRIMOD_CPRI_FEC_AUX_CONFIG_TICK_NUMERATOR:
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_FEC_4_CTRLr(unit, port, &data));
        *value = soc_reg64_field32_get(unit, CPRI_RXPCS_FEC_4_CTRLr, data, RXPCS_FEC_GBOX_TICK_NUMERf);
        break;
    case CPRIMOD_CPRI_FEC_AUX_CONFIG_SOFT_RESET:
        _SOC_IF_ERR_EXIT(READ_CPRI_RXPCS_FEC_0_CTRLr(unit, port, &data));
        *value = soc_reg64_field32_get(unit, CPRI_RXPCS_FEC_0_CTRLr, data, RXPCS_FEC_SOFT_RESETf);
        break;
    default :
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Bad Aux config"));
        break;
    }
exit:
    SOC_FUNC_RETURN;

}

int cprif_drv_port_fec_stat_get (int unit, int port, cprimod_cpri_fec_stat_type_t type, uint32* value)
{
    uint64 data;
    SOC_INIT_FUNC_DEFS;

    switch (type) {
        case CPRIMOD_CPRI_FEC_STAT_TYPE_CORRECTED_CODEWORD:
            SOC_IF_ERROR_RETURN
                (READ_CPRI_RXPCS_FEC_0_STATUSr(unit, port, &data));
            *value = soc_reg64_field32_get(unit, CPRI_RXPCS_FEC_0_STATUSr, data,
                                       RXPCS_FEC_DECODER_CORRECTED_CW_CNTf);
        break;

        case CPRIMOD_CPRI_FEC_STAT_TYPE_UNCORRECTED_CODEWORD:
            SOC_IF_ERROR_RETURN
                (READ_CPRI_RXPCS_FEC_1_STATUSr(unit, port, &data));
            *value = soc_reg64_field32_get(unit, CPRI_RXPCS_FEC_1_STATUSr, data,
                                       RXPCS_FEC_DECODER_UNCORRECTED_CW_CNTf);
        break;

        case CPRIMOD_CPRI_FEC_STAT_TYPE_BIT_ERROR:
            SOC_IF_ERROR_RETURN
                (READ_CPRI_RXPCS_FEC_2_STATUSr(unit, port, &data));
            *value = soc_reg64_field32_get(unit, CPRI_RXPCS_FEC_2_STATUSr, data,
                                       RXPCS_FEC_DECODER_BIT_ERROR_CNTf);
        break;

        case CPRIMOD_CPRI_FEC_STAT_TYPE_SYMBOL_ERROR:
            SOC_IF_ERROR_RETURN
                (READ_CPRI_RXPCS_FEC_3_STATUSr(unit, port, &data));
            *value = soc_reg64_field32_get(unit, CPRI_RXPCS_FEC_3_STATUSr, data,
                                       RXPCS_FEC_DECODER_SYMBOL_ERROR_CNT_0f);
        break;

        default:
        *value = 0;
        _SOC_EXIT_WITH_ERR
            (SOC_E_PARAM, ("This type is not supported for FEC  statistics."));
        break;
    }
exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_init(int unit, int port)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprif_drv_pmd_ctrl_ln_por_h_rstb_set(unit, port));
    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, GLAS_RST_Hf,          1);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, GLAS_RSTf,            1);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, DATAPATH_CREDIT_RSTf, 1);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_TX_Hf,          1);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_TX_DPf,         1);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_RX_Hf,          1);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_RX_DPf,         1);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_DATAPATH_TX_Hf, 1);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_DATAPATH_TX_DPf,1);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_DATAPATH_RX_Hf, 1);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_DATAPATH_RX_DPf,1);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_CIP_TX_Hf,      1);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_CIP_TX_DPf,     1);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_CIP_RX_Hf,      1);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_CIP_RX_DPf,     1);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));
    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, GLAS_RST_Hf,          0);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, GLAS_RSTf,            0);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, DATAPATH_CREDIT_RSTf, 0);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_TX_Hf,          0);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_TX_DPf,         0);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_RX_Hf,          0);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_RX_DPf,         0);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_DATAPATH_TX_Hf, 0);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_DATAPATH_TX_DPf,0);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_DATAPATH_RX_Hf, 0);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_DATAPATH_RX_DPf,0);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_CIP_TX_Hf,      0);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_CIP_TX_DPf,     0);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_CIP_RX_Hf,      0);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_CIP_RX_DPf,     0);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_port_rx_datapath_reset(int unit, int port, int val)
{
    uint32 read_val;
 
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_RX_DPf,         val);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_CIP_RX_DPf,     val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_port_rx_pmd_datapath_reset(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_PMD_RX_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_PMD_RX_RST_CTRLr, &read_val, PMD_LN_RX_DP_H_RSTBf, (val^1));
    SOC_IF_ERROR_RETURN(WRITE_CPRI_PMD_RX_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}


int cprif_drv_port_tx_datapath_reset(int unit, int port, int val)
{
    uint32 read_val;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(READ_CPRI_RST_CTRLr(unit, port, &read_val));
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_TX_DPf,         val);
    soc_reg_field_set(unit, CPRI_RST_CTRLr, &read_val, RESET_CIP_TX_DPf,     val);
    SOC_IF_ERROR_RETURN(WRITE_CPRI_RST_CTRLr(unit, port, read_val));

    SOC_FUNC_RETURN;
}

int cprif_drv_rx_gcw_word_get(int unit, int port, int group_index, uint16* word)
{
    uint64 reg_val;
    uint64 field_buf;
    int reg_index;
    int phy_port;
    int port_core_index;    
    uint32 mask;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_GCW_CFG_TAB_CFG_0m,
                              CPRI_GCW_CFG_TAB_CFG_1m,
                              CPRI_GCW_CFG_TAB_CFG_2m,
                              CPRI_GCW_CFG_TAB_CFG_3m};


    soc_reg_t   reg_select[] = {CPRI_RX_GCW_0_STATUSr,
                                CPRI_RX_GCW_1_STATUSr,
                                CPRI_RX_GCW_2_STATUSr,
                                CPRI_RX_GCW_3_STATUSr};

    soc_field_t reg_field_select[] = {RXFRM_GCW_WORD0f,  
                                      RXFRM_GCW_WORD1f,  
                                      RXFRM_GCW_WORD2f,  
                                      RXFRM_GCW_WORD3f,  
                                      RXFRM_GCW_WORD4f,  
                                      RXFRM_GCW_WORD5f,  
                                      RXFRM_GCW_WORD6f,  
                                      RXFRM_GCW_WORD7f,  
                                      RXFRM_GCW_WORD8f,  
                                      RXFRM_GCW_WORD9f,  
                                      RXFRM_GCW_WORD10f,  
                                      RXFRM_GCW_WORD11f,  
                                      RXFRM_GCW_WORD12f,  
                                      RXFRM_GCW_WORD13f,  
                                      RXFRM_GCW_WORD14f,  
                                      RXFRM_GCW_WORD15f};

    uint32 *rd_data_ptr;
    uint32 field_buf32;
    uint32 num_words;
    uint32 blk_num=0;

    SOC_INIT_FUNC_DEFS;

    if (group_index >= CPRIF_GCW_MAX_GROUP) {
        _SOC_EXIT_WITH_ERR
            (SOC_E_PARAM, ("GCW group index %d.\n",group_index));
    }

    reg_index  = group_index / CPRIF_GCW_NUM_WORD_IN_STATUS ;

    _SOC_IF_ERR_EXIT
        (soc_reg_get(unit, reg_select[reg_index], port, 0, &reg_val));

    field_buf = soc_reg64_field_get(unit, reg_select[reg_index], reg_val,
                                    reg_field_select[group_index]);

    /* Mask reading from CPRI_GCW_CFG_TAB_CFG memory*/

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    num_words = soc_mem_entry_words(unit, local_mem);
    rd_data_ptr = (uint32*)sal_alloc(sizeof(uint32) * num_words, "");
    CPRIMOD_NULL_CHECK(rd_data_ptr);
    sal_memset(rd_data_ptr, 0, sizeof(uint32) * num_words);

    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    _SOC_IF_ERR_EXIT
        (soc_mem_read(unit, local_mem, blk_num , group_index, rd_data_ptr));

    soc_mem_field_get(unit, local_mem, rd_data_ptr, GCW_MASKf, &field_buf32);
    mask = field_buf32;

    *word = 0;
    *word = (mask & 1) ?  *word | (COMPILER_64_LO(field_buf) & 0x00FF) : *word & 0xFF00;
    *word = (mask & 2) ?  *word | (COMPILER_64_LO(field_buf) & 0xFF00) : *word & 0x00FF;

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_tx_gcw_word_set(int unit, int port, int group_index, uint16 word)
{
    int blk;
    int phy_port;
    int port_core_index;
    soc_mem_t local_mem;
    soc_mem_t mem_select[] = {CPRI_CWA_GCW_REGS_CFG_0m,
                              CPRI_CWA_GCW_REGS_CFG_1m, 
                              CPRI_CWA_GCW_REGS_CFG_2m, 
                              CPRI_CWA_GCW_REGS_CFG_3m};
    uint16 wr_data=0;
    SOC_INIT_FUNC_DEFS;

    if (group_index >= CPRIF_GCW_MAX_GROUP) {
        _SOC_EXIT_WITH_ERR
            (SOC_E_PARAM, ("GCW group index %d.\n",group_index));
    }

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);

    local_mem = mem_select[port_core_index];

    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    wr_data = word;

    _SOC_IF_ERR_EXIT
        (soc_mem_write(unit, local_mem, blk, group_index, &wr_data));

exit:
    SOC_FUNC_RETURN;
}


int cprif_drv_encap_header_field_mapping_set(int unit, int port, cprif_drv_encap_hdr_field_mapping_t *mapping)
{
    uint64 reg_val;
    uint64 field_buf;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));

    if (CPRIF_CLPORT_IS_B0(pm_version)){
        _SOC_IF_ERR_EXIT
            (READ_CPRI_ENCAP_HDR_FLD_MAPPINGr(unit, port, &reg_val));

        COMPILER_64_SET(field_buf,0, mapping->hdr_byte0_sel);
        soc_reg64_field_set(unit, CPRI_ENCAP_HDR_FLD_MAPPINGr, &reg_val,
                            HDR_BYTE0_SELf, field_buf);

        COMPILER_64_SET(field_buf,0, mapping->hdr_byte1_sel);
        soc_reg64_field_set(unit, CPRI_ENCAP_HDR_FLD_MAPPINGr, &reg_val,
                            HDR_BYTE1_SELf, field_buf);

        COMPILER_64_SET(field_buf,0, mapping->hdr_byte2_sel);
        soc_reg64_field_set(unit, CPRI_ENCAP_HDR_FLD_MAPPINGr, &reg_val,
                            HDR_BYTE2_SELf, field_buf);

        COMPILER_64_SET(field_buf,0, mapping->hdr_byte3_sel);
        soc_reg64_field_set(unit, CPRI_ENCAP_HDR_FLD_MAPPINGr, &reg_val,
                            HDR_BYTE3_SELf, field_buf);

        COMPILER_64_SET(field_buf,0, mapping->hdr_byte4_sel);
        soc_reg64_field_set(unit, CPRI_ENCAP_HDR_FLD_MAPPINGr, &reg_val,
                            HDR_BYTE4_SELf, field_buf);

        COMPILER_64_SET(field_buf,0, mapping->hdr_byte5_sel);
        soc_reg64_field_set(unit, CPRI_ENCAP_HDR_FLD_MAPPINGr, &reg_val,
                            HDR_BYTE5_SELf, field_buf);

        COMPILER_64_SET(field_buf,0, mapping->hdr_byte6_sel);
        soc_reg64_field_set(unit, CPRI_ENCAP_HDR_FLD_MAPPINGr, &reg_val,
                            HDR_BYTE6_SELf, field_buf);

        COMPILER_64_SET(field_buf,0, mapping->hdr_byte7_sel);
        soc_reg64_field_set(unit, CPRI_ENCAP_HDR_FLD_MAPPINGr, &reg_val,
                            HDR_BYTE7_SELf, field_buf);

        COMPILER_64_SET(field_buf,0, mapping->hdr_byte8_sel);
        soc_reg64_field_set(unit, CPRI_ENCAP_HDR_FLD_MAPPINGr, &reg_val,
                            HDR_BYTE8_SELf, field_buf);

        COMPILER_64_SET(field_buf,0, mapping->hdr_byte9_sel);
        soc_reg64_field_set(unit, CPRI_ENCAP_HDR_FLD_MAPPINGr, &reg_val,
                            HDR_BYTE9_SELf, field_buf);

        COMPILER_64_SET(field_buf,0, mapping->hdr_byte10_sel);
        soc_reg64_field_set(unit, CPRI_ENCAP_HDR_FLD_MAPPINGr, &reg_val,
                            HDR_BYTE10_SELf, field_buf);

        COMPILER_64_SET(field_buf,0, mapping->hdr_byte11_sel);
        soc_reg64_field_set(unit, CPRI_ENCAP_HDR_FLD_MAPPINGr, &reg_val,
                            HDR_BYTE11_SELf, field_buf);

        _SOC_IF_ERR_EXIT
            (WRITE_CPRI_ENCAP_HDR_FLD_MAPPINGr(unit, port, reg_val));
    } else {
        _SOC_EXIT_WITH_ERR
            (SOC_E_UNAVAIL, ("CPRI_ENCAP_HDR_FLD_MAPPING is not support in this Version..\n"));
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_encap_header_field_mapping_get(int unit, int port, cprif_drv_encap_hdr_field_mapping_t *mapping)
{
    uint64 reg_val;
    uint64 field_buf;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));

    if (CPRIF_CLPORT_IS_B0(pm_version)){
        _SOC_IF_ERR_EXIT
            (READ_CPRI_ENCAP_HDR_FLD_MAPPINGr(unit, port, &reg_val));

        field_buf = soc_reg64_field_get(unit, CPRI_ENCAP_HDR_FLD_MAPPINGr, reg_val,
                            HDR_BYTE0_SELf);
        mapping->hdr_byte0_sel = COMPILER_64_LO(field_buf);

        field_buf = soc_reg64_field_get(unit, CPRI_ENCAP_HDR_FLD_MAPPINGr, reg_val,
                            HDR_BYTE1_SELf);
        mapping->hdr_byte1_sel = COMPILER_64_LO(field_buf);

        field_buf = soc_reg64_field_get(unit, CPRI_ENCAP_HDR_FLD_MAPPINGr, reg_val,
                            HDR_BYTE2_SELf);
        mapping->hdr_byte2_sel = COMPILER_64_LO(field_buf);

        field_buf = soc_reg64_field_get(unit, CPRI_ENCAP_HDR_FLD_MAPPINGr, reg_val,
                            HDR_BYTE3_SELf);
        mapping->hdr_byte3_sel = COMPILER_64_LO(field_buf);

        field_buf = soc_reg64_field_get(unit, CPRI_ENCAP_HDR_FLD_MAPPINGr, reg_val,
                            HDR_BYTE4_SELf);
        mapping->hdr_byte4_sel = COMPILER_64_LO(field_buf);

        field_buf = soc_reg64_field_get(unit, CPRI_ENCAP_HDR_FLD_MAPPINGr, reg_val,
                            HDR_BYTE5_SELf);
        mapping->hdr_byte5_sel = COMPILER_64_LO(field_buf);

        field_buf = soc_reg64_field_get(unit, CPRI_ENCAP_HDR_FLD_MAPPINGr, reg_val,
                            HDR_BYTE6_SELf);
        mapping->hdr_byte6_sel = COMPILER_64_LO(field_buf);

        field_buf = soc_reg64_field_get(unit, CPRI_ENCAP_HDR_FLD_MAPPINGr, reg_val,
                            HDR_BYTE7_SELf);
        mapping->hdr_byte7_sel = COMPILER_64_LO(field_buf);

        field_buf = soc_reg64_field_get(unit, CPRI_ENCAP_HDR_FLD_MAPPINGr, reg_val,
                            HDR_BYTE8_SELf);
        mapping->hdr_byte8_sel = COMPILER_64_LO(field_buf);

        field_buf = soc_reg64_field_get(unit, CPRI_ENCAP_HDR_FLD_MAPPINGr, reg_val,
                            HDR_BYTE9_SELf);
        mapping->hdr_byte9_sel = COMPILER_64_LO(field_buf);

        field_buf = soc_reg64_field_get(unit, CPRI_ENCAP_HDR_FLD_MAPPINGr, reg_val,
                            HDR_BYTE10_SELf);
        mapping->hdr_byte10_sel = COMPILER_64_LO(field_buf);

        field_buf = soc_reg64_field_get(unit, CPRI_ENCAP_HDR_FLD_MAPPINGr, reg_val,
                            HDR_BYTE11_SELf);
        mapping->hdr_byte11_sel = COMPILER_64_LO(field_buf);

    } else {
        _SOC_EXIT_WITH_ERR
            (SOC_E_UNAVAIL, ("CPRI_ENCAP_HDR_FLD_MAPPING is not support in this Version..\n"));
    }

exit:
    SOC_FUNC_RETURN;
}
int cprif_drv_decap_header_field_mapping_set(int unit, int port, cprif_drv_decap_hdr_field_mapping_t *mapping)
{
    uint32 reg_val;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));

    if (CPRIF_CLPORT_IS_B0(pm_version)){
        _SOC_IF_ERR_EXIT
            (READ_CPRI_DECAP_HDR_FLD_MAPPINGr(unit, port, &reg_val));

        soc_reg_field_set(unit, CPRI_DECAP_HDR_FLD_MAPPINGr, &reg_val, SUBTYPE_SELf,           mapping->subtype);
        soc_reg_field_set(unit, CPRI_DECAP_HDR_FLD_MAPPINGr, &reg_val, FLOWID_SELf,            mapping->flowid);
        soc_reg_field_set(unit, CPRI_DECAP_HDR_FLD_MAPPINGr, &reg_val, LENGTH_15_8_SELf,       mapping->length_15_8);
        soc_reg_field_set(unit, CPRI_DECAP_HDR_FLD_MAPPINGr, &reg_val, LENGTH_7_0_SELf,        mapping->length_7_0);
        soc_reg_field_set(unit, CPRI_DECAP_HDR_FLD_MAPPINGr, &reg_val, ORDERINGINFO_31_24_SELf, mapping->orderinginfo_31_24);
        soc_reg_field_set(unit, CPRI_DECAP_HDR_FLD_MAPPINGr, &reg_val, ORDERINGINFO_23_16_SELf, mapping->orderinginfo_23_16);
        soc_reg_field_set(unit, CPRI_DECAP_HDR_FLD_MAPPINGr, &reg_val, ORDERINGINFO_15_8_SELf,  mapping->orderinginfo_15_8);
        soc_reg_field_set(unit, CPRI_DECAP_HDR_FLD_MAPPINGr, &reg_val, ORDERINGINFO_7_0_SELf,   mapping->orderinginfo_7_0);

       _SOC_IF_ERR_EXIT
            (WRITE_CPRI_DECAP_HDR_FLD_MAPPINGr(unit, port, reg_val));
    } else {
        _SOC_EXIT_WITH_ERR
            (SOC_E_UNAVAIL, ("CPRI_DECAP_HDR_FLD_MAPPING is not support in this Version..\n"));
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_decap_header_field_mapping_get(int unit, int port, cprif_drv_decap_hdr_field_mapping_t *mapping)
{
    uint32 reg_val;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));

    if (CPRIF_CLPORT_IS_B0(pm_version)){
        _SOC_IF_ERR_EXIT
            (READ_CPRI_DECAP_HDR_FLD_MAPPINGr(unit, port, &reg_val));

        mapping->subtype                = soc_reg_field_get(unit, CPRI_DECAP_HDR_FLD_MAPPINGr, reg_val, SUBTYPE_SELf);
        mapping->flowid                 = soc_reg_field_get(unit, CPRI_DECAP_HDR_FLD_MAPPINGr, reg_val, FLOWID_SELf);
        mapping->length_15_8            = soc_reg_field_get(unit, CPRI_DECAP_HDR_FLD_MAPPINGr, reg_val, LENGTH_15_8_SELf);
        mapping->length_7_0             = soc_reg_field_get(unit, CPRI_DECAP_HDR_FLD_MAPPINGr, reg_val, LENGTH_7_0_SELf);
        mapping->orderinginfo_31_24     = soc_reg_field_get(unit, CPRI_DECAP_HDR_FLD_MAPPINGr, reg_val, ORDERINGINFO_31_24_SELf);
        mapping->orderinginfo_23_16     = soc_reg_field_get(unit, CPRI_DECAP_HDR_FLD_MAPPINGr, reg_val, ORDERINGINFO_23_16_SELf);
        mapping->orderinginfo_15_8      = soc_reg_field_get(unit, CPRI_DECAP_HDR_FLD_MAPPINGr, reg_val, ORDERINGINFO_15_8_SELf);
        mapping->orderinginfo_7_0       = soc_reg_field_get(unit, CPRI_DECAP_HDR_FLD_MAPPINGr, reg_val, ORDERINGINFO_7_0_SELf);

    } else {
        _SOC_EXIT_WITH_ERR
            (SOC_E_UNAVAIL, ("CPRI_DECAP_HDR_FLD_MAPPING is not support in this Version..\n"));
    }

exit:
    SOC_FUNC_RETURN;
}


int cprif_drv_decap_header_field_mask_set(int unit, int port, cprif_drv_decap_hdr_field_mapping_t *mask)
{
    uint64 reg_val;
    uint64 field_buf;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));

    if (CPRIF_CLPORT_IS_B0(pm_version)){
        _SOC_IF_ERR_EXIT
            (READ_CPRI_DECAP_HDR_FLD_MASKINGr(unit, port, &reg_val));

        COMPILER_64_SET(field_buf,0, mask->subtype);
        soc_reg64_field_set(unit, CPRI_DECAP_HDR_FLD_MASKINGr, &reg_val,
                            SUBTYPE_MASKf, field_buf);

        COMPILER_64_SET(field_buf,0, mask->flowid);
        soc_reg64_field_set(unit, CPRI_DECAP_HDR_FLD_MASKINGr, &reg_val,
                            FLOWID_MASKf, field_buf);

        COMPILER_64_SET(field_buf,0, mask->length_15_8);
        soc_reg64_field_set(unit, CPRI_DECAP_HDR_FLD_MASKINGr, &reg_val,
                            LENGTH_15_8_MASKf, field_buf);

        COMPILER_64_SET(field_buf,0, mask->length_7_0);
        soc_reg64_field_set(unit, CPRI_DECAP_HDR_FLD_MASKINGr, &reg_val,
                            LENGTH_7_0_MASKf, field_buf);

        COMPILER_64_SET(field_buf,0, mask->orderinginfo_31_24);
        soc_reg64_field_set(unit, CPRI_DECAP_HDR_FLD_MASKINGr, &reg_val,
                            ORDERINGINFO_31_24_MASKf, field_buf);

        COMPILER_64_SET(field_buf,0, mask->orderinginfo_23_16);
        soc_reg64_field_set(unit, CPRI_DECAP_HDR_FLD_MASKINGr, &reg_val,
                            ORDERINGINFO_23_16_MASKf, field_buf);

        COMPILER_64_SET(field_buf,0, mask->orderinginfo_15_8);
        soc_reg64_field_set(unit, CPRI_DECAP_HDR_FLD_MASKINGr, &reg_val,
                            ORDERINGINFO_15_8_MASKf, field_buf);

        COMPILER_64_SET(field_buf,0, mask->orderinginfo_7_0);
        soc_reg64_field_set(unit, CPRI_DECAP_HDR_FLD_MASKINGr, &reg_val,
                            ORDERINGINFO_7_0_MASKf, field_buf);


        _SOC_IF_ERR_EXIT
            (WRITE_CPRI_DECAP_HDR_FLD_MASKINGr(unit, port, reg_val));
    } else {
        _SOC_EXIT_WITH_ERR
            (SOC_E_UNAVAIL, ("CPRI_DECAP_HDR_FLD_MASKING is not support in this Version..\n"));
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_decap_header_field_mask_get(int unit, int port, cprif_drv_decap_hdr_field_mapping_t *mask)
{
    uint64 reg_val;
    uint64 field_buf;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));

    if (CPRIF_CLPORT_IS_B0(pm_version)){
        _SOC_IF_ERR_EXIT
            (READ_CPRI_DECAP_HDR_FLD_MASKINGr(unit, port, &reg_val));

        field_buf = soc_reg64_field_get(unit, CPRI_DECAP_HDR_FLD_MASKINGr, reg_val,
                            SUBTYPE_MASKf);
        mask->subtype = COMPILER_64_LO(field_buf);

        field_buf = soc_reg64_field_get(unit, CPRI_DECAP_HDR_FLD_MASKINGr, reg_val,
                            FLOWID_MASKf);
        mask->flowid = COMPILER_64_LO(field_buf);

        field_buf = soc_reg64_field_get(unit, CPRI_DECAP_HDR_FLD_MASKINGr, reg_val,
                            LENGTH_15_8_MASKf);
        mask->length_15_8 = COMPILER_64_LO(field_buf);

        field_buf = soc_reg64_field_get(unit, CPRI_DECAP_HDR_FLD_MASKINGr, reg_val,
                            LENGTH_7_0_MASKf);
        mask->length_7_0 = COMPILER_64_LO(field_buf);

        field_buf = soc_reg64_field_get(unit, CPRI_DECAP_HDR_FLD_MASKINGr, reg_val,
                            ORDERINGINFO_31_24_MASKf);
        mask->orderinginfo_31_24 = COMPILER_64_LO(field_buf);

        field_buf = soc_reg64_field_get(unit, CPRI_DECAP_HDR_FLD_MASKINGr, reg_val,
                            ORDERINGINFO_23_16_MASKf);
        mask->orderinginfo_23_16 = COMPILER_64_LO(field_buf);

        field_buf = soc_reg64_field_get(unit, CPRI_DECAP_HDR_FLD_MASKINGr, reg_val,
                            ORDERINGINFO_15_8_MASKf);
        mask->orderinginfo_15_8 =  COMPILER_64_LO(field_buf);

        field_buf = soc_reg64_field_get(unit, CPRI_DECAP_HDR_FLD_MASKINGr, reg_val,
                            ORDERINGINFO_7_0_MASKf);
        mask->orderinginfo_7_0 = COMPILER_64_LO(field_buf);
    } else {
        _SOC_EXIT_WITH_ERR
            (SOC_E_PARAM, ("CPRI_DECAP_HDR_FLD_MASKING is not support in this Version..\n"));
    }

exit:
    SOC_FUNC_RETURN;
}

/* any negative value above the threshold will be set to -256 */
int cprif_drv_compression_negative_saturation_threshold_set( int unit, int port, uint32 threshold_value )
{
    uint32 reg_val;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));

    if (CPRIF_CLPORT_IS_B0(pm_version)){

        /* 
         * Check for value over 14 bits.
         */
        if ((threshold_value & ~0x3FFF) != 0) {
        _SOC_EXIT_WITH_ERR
            (SOC_E_PARAM, (" The threshold_value cannot exceed 14 bit value.\n"));
        }
    

        _SOC_IF_ERR_EXIT
            (READ_CPRI_DECAP_COMP_NEG_SAT_CTRLr(unit, port, &reg_val));

        soc_reg_field_set(unit, CPRI_DECAP_COMP_NEG_SAT_CTRLr, &reg_val, COMP_NEG_SATf, threshold_value);

       _SOC_IF_ERR_EXIT
            (WRITE_CPRI_DECAP_COMP_NEG_SAT_CTRLr(unit, port, reg_val));
    } else {
        _SOC_EXIT_WITH_ERR
            (SOC_E_PARAM, ("CPRI_DECAP_COMP_NEG_SAT_CTRL is not support in this Version..\n"));
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_compression_negative_saturation_threshold_get( int unit, int port, uint32* threshold_value )
{
    uint32 reg_val;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));

    if (CPRIF_CLPORT_IS_B0(pm_version)){
        _SOC_IF_ERR_EXIT
            (READ_CPRI_DECAP_COMP_NEG_SAT_CTRLr(unit, port, &reg_val));

        *threshold_value = soc_reg_field_get(unit, CPRI_DECAP_COMP_NEG_SAT_CTRLr, reg_val, COMP_NEG_SATf);

    } else {
        _SOC_EXIT_WITH_ERR
            (SOC_E_PARAM, ("CPRI_DECAP_COMP_NEG_SAT_CTRL is not support in this Version..\n"));
    }

exit:
    SOC_FUNC_RETURN;
}

/* -256 will force to this value. */
int cprif_drv_decompression_negative_saturation_value_set( int unit, int port, uint32 sat_value)
{
    uint32 reg_val;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));

    if (CPRIF_CLPORT_IS_B0(pm_version)){
        /* 
         * Check for value over 15 bits.
         */
        if ((sat_value & ~0x7FFF) != 0) {
        _SOC_EXIT_WITH_ERR
            (SOC_E_PARAM, (" The saturation value cannot exceed 15 bit value.\n"));
        }

        _SOC_IF_ERR_EXIT
            (READ_CPRI_DECAP_DCOMP_NEG_SAT_CTRLr(unit, port, &reg_val));

        soc_reg_field_set(unit, CPRI_DECAP_DCOMP_NEG_SAT_CTRLr, &reg_val, DCOMP_NEG_SATf, sat_value);

       _SOC_IF_ERR_EXIT
            (WRITE_CPRI_DECAP_DCOMP_NEG_SAT_CTRLr(unit, port, reg_val));
    } else {
        _SOC_EXIT_WITH_ERR
            (SOC_E_PARAM, ("CPRI_DECAP_DCOMP_NEG_SAT_CTRL is not support in this Version..\n"));
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_decompression_negative_saturation_value_get( int unit, int port, uint32* sat_value)
{
    uint32 reg_val;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));

    if (CPRIF_CLPORT_IS_B0(pm_version)){
        _SOC_IF_ERR_EXIT
            (READ_CPRI_DECAP_DCOMP_NEG_SAT_CTRLr(unit, port, &reg_val));

        *sat_value = soc_reg_field_get(unit, CPRI_DECAP_DCOMP_NEG_SAT_CTRLr, reg_val, DCOMP_NEG_SATf);

    } else {
        _SOC_EXIT_WITH_ERR
            (SOC_E_PARAM, ("CPRI_DECAP_DCOMP_NEG_SAT_CTRL is not support in this Version..\n"));
    }

exit:
    SOC_FUNC_RETURN;
}


int cprif_drv_ingress_traffic_rate_set(int unit, int port, uint32 rate)
{
    uint32 reg_val;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));

    if (CPRIF_CLPORT_IS_B0(pm_version)){
        _SOC_IF_ERR_EXIT
            (READ_CPMPORT_TOP_LANE_CTRLr(unit, port, &reg_val));

        soc_reg_field_set(unit, CPMPORT_TOP_LANE_CTRLr, &reg_val, INGRESS_RATEf, rate);

        _SOC_IF_ERR_EXIT
            (WRITE_CPMPORT_TOP_LANE_CTRLr(unit, port, reg_val));
    } 

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_ingress_traffic_rate_get(int unit, int port, uint32* rate)
{
    uint32 reg_val;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));

    if (CPRIF_CLPORT_IS_B0(pm_version)){
        _SOC_IF_ERR_EXIT
            (READ_CPMPORT_TOP_LANE_CTRLr(unit, port, &reg_val));

        *rate = soc_reg_field_get(unit, CPMPORT_TOP_LANE_CTRLr, reg_val, INGRESS_RATEf);

    } else {
        _SOC_EXIT_WITH_ERR
            (SOC_E_PARAM, ("CPMPORT_TOP_LANE_CTRL INGRESS_RATE is not support in this Version..\n"));
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_decompression_traffic_rate_increase_set(int unit, int port, int enable)
{
    uint32 reg_val;
    uint32 encap_rsvd;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));
    /* 
     * B0 and C0 share the same pm version. 
     * This is intended for B0 and C0 version.
     */
    if (CPRIF_CLPORT_IS_B0(pm_version)){
        _SOC_IF_ERR_EXIT
            (READ_CPRI_ENCAP_RSVD_CONTROLr(unit, port, &reg_val));

        encap_rsvd = soc_reg_field_get(unit, CPRI_ENCAP_RSVD_CONTROLr, reg_val, ENCAP_RSVDf);
        /*
         * Only bit0 need to be set. Do not touch other bits as it may have other uses.
         */
        if (enable) {
            encap_rsvd |= 0x00001;
        } else {
            encap_rsvd &= 0xFFFE;
        }
        soc_reg_field_set(unit, CPRI_ENCAP_RSVD_CONTROLr, &reg_val, ENCAP_RSVDf, encap_rsvd );
        _SOC_IF_ERR_EXIT
            (WRITE_CPRI_ENCAP_RSVD_CONTROLr(unit, port, reg_val));
    }
exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_decompression_traffic_rate_increase_get(int unit, int port, int* enable)
{
    uint32 reg_val;
    uint32 encap_rsvd;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));
    /* 
     * B0 and C0 share the same pm version. 
     * This is intended for B0 and C0 version.
     */
    if (CPRIF_CLPORT_IS_B0(pm_version)){
        _SOC_IF_ERR_EXIT
            (READ_CPRI_ENCAP_RSVD_CONTROLr(unit, port, &reg_val));

        encap_rsvd = soc_reg_field_get(unit, CPRI_ENCAP_RSVD_CONTROLr, reg_val, ENCAP_RSVDf);
        /*
         * Only bit0 need to be check.
         */
        if (encap_rsvd & 0x0001) {
            *enable = 1;
        } else {
            *enable = 0;
        }
    }
exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_extended_header_enable_set (int unit, int port, cprimod_direction_t dir, int enable)
{
    uint32 reg_val;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));

    if (CPRIF_CLPORT_IS_B0(pm_version)){

         if (dir == CPRIMOD_DIR_RX) {
            _SOC_IF_ERR_EXIT
                (READ_CPRI_ENCAP_EXD_HDR_CTRLr(unit, port, &reg_val));

            soc_reg_field_set(unit, CPRI_ENCAP_EXD_HDR_CTRLr, &reg_val, EXTENDED_HDR_ENf, enable?1:0);

            _SOC_IF_ERR_EXIT
                (WRITE_CPRI_ENCAP_EXD_HDR_CTRLr(unit, port, reg_val));
        } else {
           _SOC_IF_ERR_EXIT
                (READ_CPRI_DECAP_EXD_HDR_CTRLr(unit, port, &reg_val));

            soc_reg_field_set(unit, CPRI_ENCAP_EXD_HDR_CTRLr, &reg_val, EXTENDED_HDR_ENf, enable?1:0);

            _SOC_IF_ERR_EXIT
                (WRITE_CPRI_DECAP_EXD_HDR_CTRLr(unit, port, reg_val));
        }
    } else {
        _SOC_EXIT_WITH_ERR
            (SOC_E_PARAM, ("EXTENDED HEADER  is not support in this Version..\n"));
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_extended_header_enable_get (int unit, int port, cprimod_direction_t dir, int* enable)
{
    uint32 reg_val;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));

    if (CPRIF_CLPORT_IS_B0(pm_version)){

         if (dir == CPRIMOD_DIR_RX) {
            _SOC_IF_ERR_EXIT
                (READ_CPRI_ENCAP_EXD_HDR_CTRLr(unit, port, &reg_val));

            *enable = soc_reg_field_get(unit, CPRI_ENCAP_EXD_HDR_CTRLr, reg_val, EXTENDED_HDR_ENf);

        } else {
           _SOC_IF_ERR_EXIT
                (READ_CPRI_DECAP_EXD_HDR_CTRLr(unit, port, &reg_val));

            *enable = soc_reg_field_get(unit, CPRI_DECAP_EXD_HDR_CTRLr, reg_val, EXTENDED_HDR_ENf);
        }
    } else {
        _SOC_EXIT_WITH_ERR
            (SOC_E_PARAM, ("EXTENDED HEADER  is not support in this Version..\n"));
    }

exit:
    SOC_FUNC_RETURN;
}

/*
 * adjust 8 bits signed to 5 bits signed.
 */
uint32 _convert_int_to_cprif_drv_adjust_value (int adjust_value) {

    uint32 value=0;

    /*
     * saturate the value to its max.
     */
    if ((adjust_value < CPRIF_LEN_ADJUST_NEG_MAX) ||
        (adjust_value > CPRIF_LEN_ADJUST_POS_MAX)) {
        if (adjust_value < 0) {
            adjust_value = CPRIF_LEN_ADJUST_NEG_MAX;
        } else {
            adjust_value = CPRIF_LEN_ADJUST_POS_MAX;
        }
    }

    /* removed signed and extra bits. */
    value = adjust_value & 0x0F;

    if ( adjust_value < 0) {
        value |= CPRIF_LEN_ADJUST_SIGNED_BIT;
    }
    return value;
}

int _convert_cprif_drv_to_int_adjust_value (uint32 value) {

    int adjust_value = 0;

    /* get  value */
    adjust_value = value & CPRIF_LEN_ADJUST_VALUE_BITS;

    if (value & CPRIF_LEN_ADJUST_SIGNED_BIT) {
       /* negative # */
        adjust_value = adjust_value * (-1);
    }

    return adjust_value;
}


int cprif_drv_length_field_adjustment_set (int unit, int port, cprimod_direction_t dir, int adjust_value)
{
    uint32 reg_val;
    uint32 field_value=0;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));

    if (CPRIF_CLPORT_IS_B0(pm_version)){

        field_value = _convert_int_to_cprif_drv_adjust_value(adjust_value);

        if (dir == CPRIMOD_DIR_RX) {
            _SOC_IF_ERR_EXIT
                (READ_CPRI_ENCAP_LEN_FLD_ADJr(unit, port, &reg_val));

            soc_reg_field_set(unit, CPRI_ENCAP_LEN_FLD_ADJr, &reg_val, LENGTH_ADJ_VALUEf, field_value);

            _SOC_IF_ERR_EXIT
                (WRITE_CPRI_ENCAP_LEN_FLD_ADJr(unit, port, reg_val));
        } else {
           _SOC_IF_ERR_EXIT
                (READ_CPRI_DECAP_LEN_FLD_ADJr(unit, port, &reg_val));

            soc_reg_field_set(unit, CPRI_DECAP_LEN_FLD_ADJr, &reg_val, LENGTH_ADJ_VALUEf, field_value);

            _SOC_IF_ERR_EXIT
                (WRITE_CPRI_DECAP_LEN_FLD_ADJr(unit, port, reg_val));
        }
    } else {
        _SOC_EXIT_WITH_ERR
            (SOC_E_PARAM, ("EXTENDED HEADER  is not support in this Version..\n"));
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_length_field_adjustment_get (int unit, int port, cprimod_direction_t dir, int* adjust_value)
{
    uint32 reg_val;
    uint32 field_value=0;
    cprif_pm_version_t pm_version;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_pm_version_get(unit, port, &pm_version));

    if (CPRIF_CLPORT_IS_B0(pm_version)){

        if (dir == CPRIMOD_DIR_RX) {
            _SOC_IF_ERR_EXIT
                (READ_CPRI_ENCAP_LEN_FLD_ADJr(unit, port, &reg_val));

            field_value = soc_reg_field_get(unit, CPRI_ENCAP_LEN_FLD_ADJr, reg_val, LENGTH_ADJ_VALUEf);
        } else {
           _SOC_IF_ERR_EXIT
                (READ_CPRI_DECAP_LEN_FLD_ADJr(unit, port, &reg_val));

            field_value = soc_reg_field_get(unit, CPRI_DECAP_LEN_FLD_ADJr, reg_val, LENGTH_ADJ_VALUEf);
        }

        field_value = _convert_cprif_drv_to_int_adjust_value(field_value);
    } else {
        _SOC_EXIT_WITH_ERR
            (SOC_E_PARAM, ("EXTENDED HEADER  is not support in this Version..\n"));
    }
exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_tx_padding_size_set (int unit, int port, uint32 padding_size)
{
    uint64 reg_val;
    uint64 field_buf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (READ_BRCM_RESERVED_CPM_3r(unit, port, &reg_val));

    COMPILER_64_SET(field_buf, 0, padding_size);
    soc_reg64_field_set(unit, BRCM_RESERVED_CPM_3r, &reg_val,
                        BRCM_RESERVED_CPM_3_1f, field_buf);

    _SOC_IF_ERR_EXIT
        (WRITE_BRCM_RESERVED_CPM_3r(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_rsvd4_tx_padding_size_get (int unit, int port, uint32* padding_size)
{
    uint64 reg_val;
    uint64 field_buf;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (READ_BRCM_RESERVED_CPM_3r(unit, port, &reg_val));

    field_buf = soc_reg64_field_get(unit, BRCM_RESERVED_CPM_3r, reg_val,
                                    BRCM_RESERVED_CPM_3_1f);
    *padding_size = COMPILER_64_LO(field_buf);

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_work_queue_interrupt_status_clear (int unit, int port)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (READ_CPRI_ENCAP_MISC_FIFO_INTR_STATUSr(unit, port, &reg_val));
    /* Write 1 will clear the fields. */
    soc_reg_field_set(unit, CPRI_ENCAP_MISC_FIFO_INTR_STATUSr, &reg_val,
                        ENCAP_WQ_FIFO_FAULT_INTERRUPT_STATUS_0f,1);
    soc_reg_field_set(unit, CPRI_ENCAP_MISC_FIFO_INTR_STATUSr, &reg_val,
                        ENCAP_WQ_FIFO_FAULT_INTERRUPT_STATUS_1f,1);
    soc_reg_field_set(unit, CPRI_ENCAP_MISC_FIFO_INTR_STATUSr, &reg_val,
                        ENCAP_WQ_FIFO_FAULT_INTERRUPT_STATUS_2f,1);
    soc_reg_field_set(unit, CPRI_ENCAP_MISC_FIFO_INTR_STATUSr, &reg_val,
                        ENCAP_WQ_FIFO_FAULT_INTERRUPT_STATUS_3f,1);
    soc_reg_field_set(unit, CPRI_ENCAP_MISC_FIFO_INTR_STATUSr, &reg_val,
                        ENCAP_WQ_FIFO_FAULT_INTERRUPT_STATUS_0f,1);
    soc_reg_field_set(unit, CPRI_ENCAP_MISC_FIFO_INTR_STATUSr, &reg_val,
                        ENCAP_WQ_FIFO_FAULT_INTERRUPT_STATUS_0f,1);
    _SOC_IF_ERR_EXIT
        (WRITE_CPRI_ENCAP_MISC_FIFO_INTR_STATUSr(unit, port, reg_val));

exit:
    SOC_FUNC_RETURN;
}

int cprif_drv_cpri_byte_error_count_clear (int unit, int port)
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    /* reading of the register itself will clear the counter.. */
    _SOC_IF_ERR_EXIT
        (READ_CPRI_RX_STATS_CNTR_0_STATUSr(unit, port, &reg_val));
exit:
    SOC_FUNC_RETURN;
}

/* select the clock source between external and internal (PLL0/1) */
int cprif_drv_tsc_clk_ext_select(int unit, int port, int ext_clk )
{
    uint32 reg_val;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(READ_CLPORT_XGXS0_CTRL_REGr(unit, port, &reg_val));
    soc_reg_field_set(unit, CLPORT_XGXS0_CTRL_REGr, &reg_val, EXT_TSC_CLK_ENf, ext_clk? 1 : 0);
    _SOC_IF_ERR_EXIT(WRITE_CLPORT_XGXS0_CTRL_REGr(unit, port, reg_val)); 

exit:
    SOC_FUNC_RETURN;
}

#ifdef CPRIMOD_SUPPORT_ECC_INJECT
int cprif_drv_cpri_port_ecc_interrupt_test(int unit, int port)
{
    uint32  rval = 0;
    uint64  rval64 = 0;
    uint32  err_addr = 0x0;
    uint32  is_64 = 0;
    uint32  ecc_ctrl_inc = 0;
    uint32  ecc_err_type = 0;
    uint32  entry[SOC_MAX_MEM_WORDS];
    int     ecc1b_err = 0;
    int     ecc2b_err = 0;
    int     ecc_multib_err = 0;
    int     cnt = 0;
    int     min_idx = 0, max_idx = 0, use_idx = 0;
    int     blk, phy_port;
    int     cpri_mem_cnt = 0;
    int     idx = 0;
    int     intr_info_idx = 0;
    soc_mem_t  *cpri_mem_list_ptr = NULL;
    soc_mem_t  mem;
    _cpri_ecc_interrupts_info_t *intr_ptr = &_cpri_port_ecc_intrs_info[0];
    _cpri_ecc_inject_ctrl_t     *ecc_ctrl = &_cpri_ecc_err_ctrl_info[0];
    SOC_INIT_FUNC_DEFS;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    /* Get the count of CPRI memories */
    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
        if (!soc_mem_is_valid(unit, mem) ||
            !(SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem)) ==
             SOC_BLK_CPRI)) {
            continue;
        }
        cpri_mem_cnt++;
    }

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit, "Cpri Memories count - %d\n"), cpri_mem_cnt));

    cpri_mem_list_ptr = (soc_mem_t *)sal_alloc(sizeof(soc_mem_t) * cpri_mem_cnt,
                                              "cpri_mem_list");

    if (!cpri_mem_list_ptr) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Failed to allocate memory\n"));
    }

    /* Populate the array with CPRI related memories */
    for (mem = 0; mem <= NUM_SOC_MEM; mem++) {
        if (!soc_mem_is_valid(unit, mem) ||
            !(SOC_BLOCK_TYPE(unit, SOC_MEM_BLOCK_ANY(unit, mem)) ==
             SOC_BLK_CPRI)) {
            continue;
        }
        cpri_mem_list_ptr[idx++] = mem;
    }

    /* index to cpri memory */
    idx = 0;

    while (ecc_ctrl->en_r != INVALIDr) {

        /* Check if register is 64-bit to call 64-bit reg operations */
        is_64 = 0;
        if (SOC_REG_IS_64(unit, ecc_ctrl->en_r)) {
            is_64 = 1;
        }

        /*
         * Enable Inject 1-bit ecc error and then 2-bits ecc error
         * on read.
         * Select an index in the memory and perform a read, to trigger
         * and ECC error. Check the ECC interrupt status and report.
         */
        for (ecc_err_type = _cpriEccInject1bErrOnRead;
             ecc_err_type < _cpriEccInjectCount;
             ecc_err_type++)
        {
            if (is_64) {
                _SOC_IF_ERR_EXIT(soc_reg64_get(unit, ecc_ctrl->en_r,
                                               port, 0, &rval64));

                /* Inject ECC error */
                if (ecc_ctrl->en_f != INVALIDf) {
                    soc_reg64_field32_set(unit, ecc_ctrl->en_r, &rval64,
                                      ecc_ctrl->en_f, ecc_err_type);
                }

                _SOC_IF_ERR_EXIT(soc_reg64_set(unit, ecc_ctrl->en_r,
                                               port, 0, rval64));
            } else {
                _SOC_IF_ERR_EXIT(soc_reg32_get(unit, ecc_ctrl->en_r,
                                               port, 0, &rval));

                /* Inject ECC error */
                if (ecc_ctrl->en_f != INVALIDf) {
                    soc_reg_field_set(unit, ecc_ctrl->en_r, &rval,
                                      ecc_ctrl->en_f, ecc_err_type);
                }

                _SOC_IF_ERR_EXIT(soc_reg32_set(unit, ecc_ctrl->en_r,
                                               port, 0, rval));
            }


            /*
             * Perform a memory read on each Mem and
             * check the ECC intr status
             */
            for (idx = 0; idx < cpri_mem_cnt; idx++) {

                mem = cpri_mem_list_ptr[idx];
                min_idx = soc_mem_index_min(unit, mem);
                max_idx = soc_mem_index_max(unit, mem);
                use_idx = (max_idx - min_idx)/2;

                _SOC_IF_ERR_EXIT(soc_mem_read(unit, mem, blk, use_idx, entry));

                cnt = 0;
                intr_info_idx = 0;
                intr_ptr = &_cpri_port_ecc_intrs_info[intr_info_idx];
                while (intr_ptr->ecc_status_reg != INVALIDr) {

                    _SOC_IF_ERR_EXIT(soc_reg32_get(unit,
                                                   intr_ptr->ecc_status_reg,
                                                   port, 0, &rval));

                    /* Check if 1-bit ECC error */
                    if (intr_ptr->ecc1b_err_status_fld != INVALIDf) {
                        ecc1b_err = soc_reg_field_get(unit,
                                         intr_ptr->ecc_status_reg,
                                         rval, intr_ptr->ecc1b_err_status_fld);
                    }

                    /* Check if 2-bit ECC error */
                    if (intr_ptr->ecc2b_err_status_fld != INVALIDf) {
                        ecc2b_err = soc_reg_field_get(unit,
                                         intr_ptr->ecc_status_reg,
                                         rval, intr_ptr->ecc2b_err_status_fld);
                    }

                    /* Check if multi-bit ECC error */
                    if (intr_ptr->ecc_multib_err_status_fld != INVALIDf) {
                        ecc_multib_err = soc_reg_field_get(unit,
                                           intr_ptr->ecc_status_reg, rval,
                                           intr_ptr->ecc_multib_err_status_fld);
                    }

                    /* Get the address */
                    if (ecc1b_err || ecc2b_err || ecc_multib_err) {
                        err_addr = soc_reg_field_get(unit,
                                               intr_ptr->ecc_status_reg,
                                               rval, intr_ptr->ecc_event_addr);

                        cnt++;
                    }

                    /* Clear error if 1-bit ECC error */
                    if (ecc1b_err) {
                        soc_reg_field_set(unit, intr_ptr->ecc_status_reg, &rval,
                                          intr_ptr->ecc1b_err_status_fld, 1);
                    }

                    /* Clear error if 2-bit ECC error */
                    if (ecc2b_err) {
                        soc_reg_field_set(unit, intr_ptr->ecc_status_reg, &rval,
                                          intr_ptr->ecc2b_err_status_fld, 1);
                    }

                    /* Clear error if multi-bit ECC error */
                    if (ecc_multib_err) {
                        soc_reg_field_set(unit, intr_ptr->ecc_status_reg, &rval,
                                      intr_ptr->ecc_multib_err_status_fld, 1);
                    }

                    _SOC_IF_ERR_EXIT(soc_reg32_set(unit,
                                                   intr_ptr->ecc_status_reg,
                                                   port, 0, rval));

                    intr_info_idx++;

                    if (ecc1b_err || ecc2b_err || ecc_multib_err) {
                        LOG_VERBOSE(BSL_LS_BCM_PORT,
                                (BSL_META_U(unit,
                                "r - %s\nf - %s\nr - %s\nm(%s) - %s\n\r\n"),
                                SOC_REG_NAME(unit, ecc_ctrl->en_r),
                                SOC_FIELD_NAME(unit, ecc_ctrl->en_f),
                                SOC_REG_NAME(unit, intr_ptr->ecc_status_reg),
                                ecc1b_err? "1-bit": ecc2b_err? "2-bit": "m-bit",
                                SOC_MEM_NAME(unit, mem)));

                        LOG_VERBOSE(BSL_LS_BCM_PORT,
                                (BSL_META_U(unit,
                                "[i-%d] [1b-%d] [2b-%d] [mb-%d] [addr-%x]\n\n"),
                                intr_info_idx, ecc1b_err, ecc2b_err,
                                ecc_multib_err, err_addr));
                    }
                    intr_ptr = &_cpri_port_ecc_intrs_info[intr_info_idx];
                }

            }

            if (is_64) {
                _SOC_IF_ERR_EXIT(soc_reg64_get(unit, ecc_ctrl->en_r,
                                               port, 0, &rval64));

                /* Set Inject ECC error to NONE */
                if (ecc_ctrl->en_f != INVALIDf) {
                    soc_reg64_field32_set(unit, ecc_ctrl->en_r, &rval64,
                                      ecc_ctrl->en_f, _cpriEccInjectnoErr);
                }

                _SOC_IF_ERR_EXIT(soc_reg64_set(unit, ecc_ctrl->en_r,
                                               port, 0, rval64));
            } else {
                _SOC_IF_ERR_EXIT(soc_reg32_get(unit, ecc_ctrl->en_r,
                                               port, 0, &rval));

                /* Set Inject ECC error to NONE */
                if (ecc_ctrl->en_f != INVALIDf) {
                    soc_reg_field_set(unit, ecc_ctrl->en_r, &rval,
                                      ecc_ctrl->en_f, _cpriEccInjectnoErr);
                }

                _SOC_IF_ERR_EXIT(soc_reg32_set(unit, ecc_ctrl->en_r,
                                               port, 0, rval));
            }

            is_64 = 0;
            if (SOC_REG_IS_64(unit,  ecc_ctrl->dis_r)){
             is_64 = 1;
            }

            if (is_64) {
                _SOC_IF_ERR_EXIT(soc_reg64_get(unit, ecc_ctrl->dis_r,
                                               port, 0, &rval64));

                /* Disable - Inject ECC error */
                if (ecc_ctrl->dis_f != INVALIDf) {
                    soc_reg64_field32_set(unit, ecc_ctrl->dis_r, &rval64,
                                      ecc_ctrl->dis_f, 1);
                }

                _SOC_IF_ERR_EXIT(soc_reg64_set(unit, ecc_ctrl->dis_r,
                                               port, 0, rval64));

                /* Disable - Inject ECC error */
                if (ecc_ctrl->dis_f != INVALIDf) {
                    soc_reg64_field32_set(unit, ecc_ctrl->dis_r, &rval64,
                                      ecc_ctrl->dis_f, 0);
                }

                _SOC_IF_ERR_EXIT(soc_reg64_set(unit, ecc_ctrl->dis_r,
                                               port, 0, rval64));
            } else {
                _SOC_IF_ERR_EXIT(soc_reg32_get(unit, ecc_ctrl->dis_r,
                                               port, 0, &rval));

                /* Disable - Inject ECC error */
                if (ecc_ctrl->dis_f != INVALIDf) {
                    soc_reg_field_set(unit, ecc_ctrl->dis_r, &rval,
                                      ecc_ctrl->dis_f, 1);
                }

                _SOC_IF_ERR_EXIT(soc_reg32_set(unit, ecc_ctrl->dis_r,
                                               port, 0, rval));

                /* Disable - Inject ECC error */
                if (ecc_ctrl->dis_f != INVALIDf) {
                    soc_reg_field_set(unit, ecc_ctrl->dis_r, &rval,
                                      ecc_ctrl->dis_f, 0);
                }

                _SOC_IF_ERR_EXIT(soc_reg32_set(unit, ecc_ctrl->dis_r,
                                               port, 0, rval));
            }
        }
        ecc_ctrl_inc++;
        ecc_ctrl = &_cpri_ecc_err_ctrl_info[ecc_ctrl_inc];
    }

exit:
    if (cpri_mem_list_ptr) {
        sal_free(cpri_mem_list_ptr);
    }
    SOC_FUNC_RETURN;

}
#endif /* CPRIMOD_SUPPORT_ECC_INJECT */

#endif /* CPRIMOD_CPRI_FALCON_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
