

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

 
#include <soc/dnx/dbal/dbal.h>
#include <bcm/oam.h>

#include <src/bcm/dnx/oam/oam_oamp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oam_access.h>
#include <soc/dnx/swstate/auto_generated/access/oam_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

#include <bcm/sat.h>
#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/types.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/sat/sat.h>
#include <bcm_int/dnx/rx/rx_trap.h>





#define _DNX_PP_OAM_PTCH_OPAQUE_VALUE           (7)
#define DNX_PP_OAM_LOOPBACK_TST_INFO_FLAGS_LBM (0x1)
#define DNX_PP_OAM_LOOPBACK_TST_INFO_FLAGS_TST (0x2)
#define DNX_PP_OAM_LOOPBACK_TST_TRAP_CNT_MASK  (0xFFFFFFF)
#define DNX_PP_OAM_LOOPBACK_TST_TRAP_FLAGS_TST (0x20000000)
#define DNX_PP_OAM_LOOPBACK_TST_TRAP_FLAGS_LBM (0x10000000)
#define DNX_OAM_TST_LB_ETH_SA_MAC_SECOND_BYTE_MASK UTILEX_BITS_MASK(15 - SAL_UINT8_NOF_BITS - 1,0)
#define DNX_TST_TVL_LENGTH_MIN                 (34)
#define DNX_TST_LB_SAT_PKT_HDR_LENGTH          (127)
#define DNX_TST_LB_PKT_SAT_SEQ_NUM_OFFSET      (4)
#define DNX_TST_LB_PKT_PLAYLOAD_OFFSET         (11)



typedef struct
{
    uint8 field1;
    uint8 field2;
    uint8 field3;
    uint8 field4;
    uint8 field5;
    uint8 field6;
    uint8 field7;
    uint8 field8;
    uint16 field9;
    uint32 field10;
} dnx_PPH_base_head;

typedef struct
{
    uint32 flag;                
    bcm_oam_endpoint_t mepid;   
    int lb_tlv_num;
    bcm_oam_tlv_t tlv;          
    uint32 pkt_pri;             
    uint32 inner_pkt_pri;       
    bcm_cos_t int_pri;          
} DNX_OAM_SAT_GTF_ENTRY;
typedef struct
{
    uint32 flag;                
    bcm_oam_endpoint_t mepid;   
    uint8 identifier_tc;        
    uint32 identifier_color;    
    uint32 identifier_trap_id;  
    uint32 identifier_session_id;       
    uint32 session_map_flag;    
    bcm_oam_tlv_t tlv;
} DNX_OAM_SAT_CTF_ENTRY;

typedef struct
{
    uint32 endpoint_id;         
    uint8 flags;                
    bcm_mac_t mac_address;      
    uint8 pkt_pri;              
    uint8 inner_pkt_pri;        
    bcm_cos_t int_pri;          
} DNX_PP_OAM_LOOPBACK_TST_INFO;


static shr_error_e
_dnx_oam_tst_lb_ssp_get(
    int unit,
    uint32 *ssp)
{

    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ssp, _SHR_E_PARAM, "ssp Is NULL\n");
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_GENERAL_CFG, &entry_handle_id));

    
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PTCH_PP_SSP, INST_SINGLE, ssp);

    
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
_dnx_ins_ptch2_for_tst_lb(
    uint8 *buffer,
    int byte_start_offset,
    uint8 is_up,
    uint8 oam_prt_qualifier_profile,
    uint8 pp_ssp)
{
    int offset = byte_start_offset;
    int parser_program_ctl = 0;
    parser_program_ctl = is_up ? 1 : 0;

     
    buffer[offset++] = (parser_program_ctl << 7) | (oam_prt_qualifier_profile << 4);
    buffer[offset++] = pp_ssp;

    return offset;
}


static shr_error_e
_dnx_ins_itmh_for_tst_lb(
    uint8 *buffer,
    int byte_start_offset,
    int fwd_dest_info,
    uint8 fwd_tc,
    uint8 fwd_dp,
    uint8 snoop_cmd,
    uint8 in_mirr_flag,
    uint8 pph_type,
    int is_jericho_mode)
{
    int offset = byte_start_offset;
    int dest_exten = pph_type;
    if (!is_jericho_mode)
    {
        buffer[offset++] = ((pph_type & 0x3) << 1) | (in_mirr_flag & 0x1);
        buffer[offset++] = ((fwd_dp & 0x3) << 6) | ((fwd_dest_info & 0x1F8000) >> 15);  
        buffer[offset++] = ((fwd_dest_info & 0x7F80) >> 7);     
        buffer[offset++] = ((fwd_dest_info & 0x7F) << 1) | (snoop_cmd & 0x8);   
        buffer[offset++] = ((snoop_cmd & 0x7) << 4) | ((fwd_tc & 0x7) << 1) | (dest_exten & 0x1);
    }
    else
    {
        buffer[offset++] =
            ((pph_type & 0x3) << 6) | ((in_mirr_flag & 0x1) << 5) | ((fwd_dp & 0x3) << 2) | ((fwd_dest_info & 0x70000)
                                                                                             >> 16);
        buffer[offset++] = (fwd_dest_info & 0xff00) >> 8;
        buffer[offset++] = (fwd_dest_info & 0xff);
        buffer[offset++] = ((snoop_cmd & 0xf) << 4) | ((fwd_tc & 0x7) << 1);
    }
    return offset;
}

static int
_dnx_ins_itmh_ext(
    int unit,
    uint8 *buffer,
    int byte_start_offset,
    uint8 type,
    const dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    int is_jericho_mode)
{
    int fwd_dst_info;
    int offset = byte_start_offset;
    fwd_dst_info = mep_db_entry->fec_id_or_glob_out_lif.glob_out_lif;
    if (is_jericho_mode)
    {
        buffer[offset++] = (type & 0x7 << 5) | ((fwd_dst_info & 0xF0000) >> 16);
        buffer[offset++] = (fwd_dst_info & 0xff00) >> 8;
        buffer[offset++] = (fwd_dst_info & 0xFF);
    }
    else
    {
        buffer[offset++] = (type & 0x1 << 6) | ((fwd_dst_info & 0x3F0000) >> 16);
        buffer[offset++] = (fwd_dst_info & 0xff00) >> 8;
        buffer[offset++] = (fwd_dst_info & 0xFF);
    }
    return offset;
}

static int
_dnx_ins_eth_for_tst_lb(
    uint8 *buffer,
    int byte_start_offset,
    bcm_mac_t dest_addr,
    bcm_mac_t src_addr,
    const uint16 *tpids,
    const bcm_vlan_t * vlans,
    int no_tags,
    uint16 ether_type)
{
    int i = 0, offset = byte_start_offset;
    for (i = 0; i < 6; i++)
    {
        buffer[offset++] = dest_addr[i];
    }
    for (i = 0; i < 6; i++)
    {
        buffer[offset++] = src_addr[i];
    }
    i = 0;
    while (no_tags)
    {
        buffer[offset++] = (tpids[i] >> 8) & 0xff;
        buffer[offset++] = tpids[i] & 0xff;
        buffer[offset++] = ((vlans[i] >> 8) & 0xff);    
        buffer[offset++] = vlans[i] & 0xff;
        --no_tags;
        i++;
    }

    buffer[offset++] = (ether_type >> 8) & 0xff;
    buffer[offset++] = ether_type & 0xff;

    return offset;
}

static int
_dnx_ins_pph_for_tst_lb(
    uint8 *buffer,
    const dnx_PPH_base_head * pph_fields,
    int byte_start_offset,
    int is_jericho_mode)
{
    int offset = byte_start_offset;
    if (!is_jericho_mode)
    {
        buffer[offset++] = 0;
        buffer[offset++] = 0;
        buffer[offset++] = (pph_fields->field9 & 0x38000) >> 15;
        buffer[offset++] = (pph_fields->field9 & 0x7F80) >> 7;
        buffer[offset++] = ((pph_fields->field9 & 0x7F) << 1) | ((pph_fields->field10 & 0x200000) >> 21);
        buffer[offset++] = (pph_fields->field10 & 0x1FE000) >> 13;
        buffer[offset++] = (pph_fields->field10 & 0x1FE0) >> 4;
        buffer[offset++] = (pph_fields->field10 & 0x1F) << 3;
        buffer[offset++] = 0;
        buffer[offset++] = 0;
        buffer[offset++] = 0;
        
        buffer[offset++] = (pph_fields->field2 & 0x1F) << 1 | 1;
    }
    else
    {
        buffer[offset++] = ((pph_fields->field1 & 0x3) << 4) | (pph_fields->field2 & 0xf);
        buffer[offset++] = ((pph_fields->field3 & 0x7f) << 1) | (pph_fields->field4 & 0x1);
        buffer[offset++] =
            ((pph_fields->field5 & 0x3) << 6) | ((pph_fields->field6 & 0x3) << 4) | ((pph_fields->field7 & 0x1) << 3) |
            ((pph_fields->field8 & 0x1) << 2) | ((pph_fields->field9 & 0xC000) >> 14);
        buffer[offset++] = ((pph_fields->field9 & 0x3FC0) >> 6);
        buffer[offset++] = ((pph_fields->field9 & 0x7F) << 2) | ((pph_fields->field10 & 0x30000) >> 16);
        buffer[offset++] = ((pph_fields->field10 & 0xff00) >> 8);
        buffer[offset++] = (pph_fields->field10 & 0xff);
    }
    return offset;
}

static int
_dnx_ins_mpls_label_for_tst_lb(
    uint8 *buffer,
    int label,
    uint8 exp,
    uint8 bos,
    uint8 ttl,
    int byte_start_offset)
{
    int offset = byte_start_offset;

    buffer[offset++] = ((label & 0xFF000) >> 12);
    buffer[offset++] = ((label & 0xFF0) >> 4);
    buffer[offset++] = ((label & 0xF) << 4) | ((exp & 0x7) << 1) | (bos & 0x1);
    buffer[offset++] = ttl;

    return offset;
}

static int
_dnx_ins_gal_gach_for_tst_lb(
    uint8 *buffer,
    uint32 stuff,
    int byte_start_offset,
    int is_mpls_pwe_gach)
{
    int offset = byte_start_offset;
    if (is_mpls_pwe_gach)
    {
        buffer[offset++] = ((stuff & 0xFF000000) >> 24) | 0x10;
    }
    else
    {
        buffer[offset++] = ((stuff & 0xFF000000) >> 24);
    }
    buffer[offset++] = ((stuff & 0xFF0000) >> 16);
    buffer[offset++] = ((stuff & 0xFF00) >> 8);
    buffer[offset++] = stuff & 0xFF;

    return offset;
}

static int
_dnx_ins_oam_pdu_header_for_tst_lb(
    uint8 *buffer,
    int level,
    int is_lbm,
    int byte_start_offset)
{
    int offset = byte_start_offset;
    uint8 opcode = (is_lbm & DNX_PP_OAM_LOOPBACK_TST_INFO_FLAGS_LBM) ? bcmOamOpcodeLBM : bcmOamOpcodeTST;

    buffer[offset++] = ((level & 0x7) << 5);
    buffer[offset++] = opcode;
    buffer[offset++] = 0;       
    buffer[offset++] = 4;       

    return offset;
}

static shr_error_e
_dnx_oam_sat_ctf_info_get(
    int unit,
    uint32 ctf_id,
    DNX_OAM_SAT_CTF_ENTRY * ctf_info,
    int *found)
{
    int rv;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(found, _SHR_E_PARAM, "found Is NULL\n");
    SHR_NULL_CHECK(ctf_info, _SHR_E_PARAM, "ctf_info Is NULL\n");
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_MEP_TST_CTF_INFO_SW, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_FLOW_ID, ctf_id);

    
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LB_TST_MEP_ID, INST_SINGLE,
                               (uint32 *) &ctf_info->mepid);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TLV_TYPE, INST_SINGLE, &ctf_info->tlv.tlv_type);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TLV_LENGTH, INST_SINGLE,
                               (uint32 *) &ctf_info->tlv.tlv_length);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TLV_PATTERN, INST_SINGLE,
                               &ctf_info->tlv.four_byte_repeatable_pattern);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TRAP_ID, INST_SINGLE, &ctf_info->identifier_trap_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CTF_SESSION_ID, INST_SINGLE,
                               &ctf_info->identifier_session_id);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SESSION_MAP_FLAG, INST_SINGLE,
                               &ctf_info->session_map_flag);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CTF_INFO_FLAGS, INST_SINGLE, &ctf_info->flag);

    
    rv = dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT);
    if (rv == _SHR_E_NONE)
    {
        *found = TRUE;
    }
    else if (rv == _SHR_E_NOT_FOUND)
    {
        *found = FALSE;
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
_dnx_oam_sat_ctf_info_set(
    int unit,
    uint32 ctf_id,
    const DNX_OAM_SAT_CTF_ENTRY * ctf_info)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_MEP_TST_CTF_INFO_SW, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_FLOW_ID, ctf_id);

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LB_TST_MEP_ID, INST_SINGLE, ctf_info->mepid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TLV_TYPE, INST_SINGLE, ctf_info->tlv.tlv_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TLV_LENGTH, INST_SINGLE, ctf_info->tlv.tlv_length);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TLV_PATTERN, INST_SINGLE,
                                 ctf_info->tlv.four_byte_repeatable_pattern);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAP_ID, INST_SINGLE, ctf_info->identifier_trap_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SESSION_MAP_FLAG, INST_SINGLE,
                                 ctf_info->session_map_flag);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CTF_SESSION_ID, INST_SINGLE,
                                 ctf_info->identifier_session_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CTF_INFO_FLAGS, INST_SINGLE, ctf_info->flag);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
_dnx_oam_sat_ctf_info_del(
    int unit,
    uint32 ctf_id)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_MEP_TST_CTF_INFO_SW, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_FLOW_ID, ctf_id);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
_dnx_oam_sat_gtf_info_get(
    int unit,
    uint32 gtf_id,
    DNX_OAM_SAT_GTF_ENTRY * gtf_info,
    int *found)
{
    int rv;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_MEP_TST_GTF_INFO_SW, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_FLOW_ID, gtf_id);

    
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_GTF_INFO_FLAGS, INST_SINGLE, &gtf_info->flag);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LB_TLV_NUM, INST_SINGLE, &gtf_info->flag);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LB_TST_MEP_ID, INST_SINGLE,
                               (uint32 *) &gtf_info->mepid);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TLV_TYPE, INST_SINGLE, &gtf_info->tlv.tlv_type);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TLV_LENGTH, INST_SINGLE,
                               (uint32 *) &gtf_info->tlv.tlv_length);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TLV_PATTERN, INST_SINGLE,
                               &gtf_info->tlv.four_byte_repeatable_pattern);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PKT_PRI, INST_SINGLE, &gtf_info->pkt_pri);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INNER_PKT_PRI, INST_SINGLE, &gtf_info->inner_pkt_pri);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INT_PRI, INST_SINGLE, (uint32 *) &gtf_info->int_pri);

    
    rv = dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT);
    if (rv == _SHR_E_NONE)
    {
        *found = TRUE;
    }
    else if (rv == _SHR_E_NOT_FOUND)
    {
        *found = FALSE;
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
_dnx_oam_sat_gtf_info_set(
    int unit,
    uint32 gtf_id,
    const DNX_OAM_SAT_GTF_ENTRY * gtf_info)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_MEP_TST_GTF_INFO_SW, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_FLOW_ID, gtf_id);

    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LB_TST_MEP_ID, INST_SINGLE, gtf_info->mepid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LB_TLV_NUM, INST_SINGLE, gtf_info->lb_tlv_num);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TLV_TYPE, INST_SINGLE, gtf_info->tlv.tlv_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TLV_LENGTH, INST_SINGLE, gtf_info->tlv.tlv_length);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TLV_PATTERN, INST_SINGLE,
                                 gtf_info->tlv.four_byte_repeatable_pattern);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PKT_PRI, INST_SINGLE, gtf_info->pkt_pri);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INNER_PKT_PRI, INST_SINGLE, gtf_info->inner_pkt_pri);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INT_PRI, INST_SINGLE, gtf_info->int_pri);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
void
_dnx_oam_lb_tst_info_t_init(
    DNX_PP_OAM_LOOPBACK_TST_INFO * tst_info)
{
    if (NULL != tst_info)
    {
        sal_memset(tst_info, 0, sizeof(DNX_PP_OAM_LOOPBACK_TST_INFO));
        tst_info->int_pri = -1;
        tst_info->pkt_pri = 0xff;
        tst_info->inner_pkt_pri = 0xff;
    }
    return;
}

static shr_error_e
_dnx_oam_sat_gtf_info_del(
    int unit,
    uint32 gtf_id)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_MEP_TST_GTF_INFO_SW, &entry_handle_id));

    
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_FLOW_ID, gtf_id);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
oamp_tst_udh_header_size_get(
    int unit,
    uint32 *udh_size)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_UDH, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_UDH_SIZE, INST_SINGLE, udh_size));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


static shr_error_e
_dnx_db_fes_user_header_sizes_get(
    int unit,
    uint32 *user_header_0_size,
    uint32 *user_header_1_size,
    uint32 *user_header_egress_pmf_offset_0,
    uint32 *user_header_egress_pmf_offset_1)
{
    uint32 user_header_size_lcl[4];
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(user_header_0_size, _SHR_E_PARAM, "user_header_0_size");
    SHR_NULL_CHECK(user_header_1_size, _SHR_E_PARAM, "user_header_1_size");
    SHR_NULL_CHECK(user_header_egress_pmf_offset_0, _SHR_E_PARAM, "user_header_egress_pmf_offset_0");
    SHR_NULL_CHECK(user_header_egress_pmf_offset_1, _SHR_E_PARAM, "user_header_egress_pmf_offset_1");

    user_header_size_lcl[0] = dnx_data_field.udh.field_class_id_size_0_get(unit);
    user_header_size_lcl[1] = dnx_data_field.udh.field_class_id_size_1_get(unit);
    user_header_size_lcl[2] = dnx_data_field.udh.field_class_id_size_2_get(unit);
    user_header_size_lcl[3] = dnx_data_field.udh.field_class_id_size_3_get(unit);

    *user_header_0_size = user_header_size_lcl[0] + user_header_size_lcl[2];
    *user_header_1_size = user_header_size_lcl[1] + user_header_size_lcl[3];

    
    *user_header_egress_pmf_offset_0 = 0;
    *user_header_egress_pmf_offset_1 = 0;
    if (user_header_size_lcl[0] == 0)
    {
        *user_header_egress_pmf_offset_0 = (*user_header_0_size);
        *user_header_egress_pmf_offset_1 = (*user_header_0_size) + 8;
    }
    else if (user_header_size_lcl[0] == 8)
    {
        *user_header_egress_pmf_offset_0 = 0;
        *user_header_egress_pmf_offset_1 = (*user_header_0_size);
    }
    else        
    {
        *user_header_egress_pmf_offset_0 = 0;
        *user_header_egress_pmf_offset_1 = 8;
    }
exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
_dnx_oam_y1731_mpls_tp_gach_gal_get(
    int unit,
    int *gach,
    uint32 *gal)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_NULL_CHECK(gach, _SHR_E_PARAM, "gach Is NULL\n");
    SHR_NULL_CHECK(gal, _SHR_E_PARAM, "gal Is NULL\n");
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_INIT_Y1731_GENERAL_CFG, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_Y1731_MPLSTP_GAL, INST_SINGLE, gal);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(bcm_dnx_oam_mpls_tp_channel_type_tx_get(unit, bcmOamMplsTpChannelY1731, gach));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static soc_error_t
_dnx_pp_oam_oamp_lb_tst_header_set(
    int unit,
    const DNX_PP_OAM_LOOPBACK_TST_INFO * lb_tst_info,
    uint32 is_up,
    uint8 *header_buffer,
    int *header_offset)
{
    dnx_oam_oamp_ccm_endpoint_t mep_db_entry;
    int next_is_itmh;
    uint32 ssp;
    int offset = 0;
    soc_reg_above_64_val_t reg_data;
    uint32 _sys_phy_port_id = 0;
    uint32 user_header_0_size = 0, user_header_1_size, udh_size = 0;
    uint32 user_header_egress_pmf_offset_0_dummy = 0, user_header_egress_pmf_offset_1_dummy = 0;
    int pwe_gach = 0;
    uint8 pph_type;
    int is_jericho_mode = 0;
    uint16 vlans[2], tpids[2];
    bcm_mac_t peer_d_mac;
    bcm_mac_t gtf_smac;
    bcm_mac_t sa_mac_addr_msb;
    uint8 tc, dp, bos = 1;
    uint32 gal, fwd_dst_info;
    int gach;
    uint16 ether_type = 0x8902;
    uint64 work_reg_64;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(header_buffer, _SHR_E_PARAM, "header_buffer Is NULL\n");
    SHR_NULL_CHECK(header_offset, _SHR_E_PARAM, "header_offset Is NULL\n");

    if (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
        DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        is_jericho_mode = 1;
    }

    
    mep_db_entry.mep_type = 0;
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_get(unit, lb_tst_info->endpoint_id, is_up, &mep_db_entry));

    
    next_is_itmh = (mep_db_entry.mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE
                    || mep_db_entry.mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP
                    || (mep_db_entry.mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM
                        && !_SHR_IS_FLAG_SET(mep_db_entry.flags, DNX_OAMP_OAM_CCM_MEP_IS_UPMEP)));
    if (mep_db_entry.mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM
        && _SHR_IS_FLAG_SET(mep_db_entry.flags, DNX_OAMP_OAM_CCM_MEP_IS_UPMEP))
    {
        ssp = mep_db_entry.unified_port_access.ccm_eth_up_mep_port.pp_port;
    }
    else
    {
        SHR_IF_ERR_EXIT(_dnx_oam_tst_lb_ssp_get(unit, &ssp));
    }

    offset = _dnx_ins_ptch2_for_tst_lb(header_buffer, offset, is_up, _DNX_PP_OAM_PTCH_OPAQUE_VALUE, ssp);

    
    if (next_is_itmh)
    {

        SHR_IF_ERR_EXIT(dnx_oam_oamp_itmh_priority_profile_get(unit, mep_db_entry.itmh_tc_dp_profile, &tc, &dp));
        if (mep_db_entry.mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE
            || mep_db_entry.mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP)
        {
            _sys_phy_port_id = mep_db_entry.dest_sys_port_agr;
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_oam_oamp_local_port_to_system_port_get(unit,
                                                                       mep_db_entry.unified_port_access.port_profile,
                                                                       &_sys_phy_port_id));
        }
        if (is_jericho_mode)
        {
            fwd_dst_info = 0x10000 | (_sys_phy_port_id & 0x7fff);
        }
        else
        {
            fwd_dst_info = 0xc0000 | (_sys_phy_port_id & 0x7fff);
        }
        pph_type = (mep_db_entry.mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM && !is_up) ? 0 : 1;
        offset = _dnx_ins_itmh_for_tst_lb(header_buffer, offset, fwd_dst_info, tc, dp, 0, 0, pph_type, is_jericho_mode);
    }

    if (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
        DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        SHR_IF_ERR_EXIT(_dnx_db_fes_user_header_sizes_get(unit,
                                                          &user_header_0_size,
                                                          &user_header_1_size,
                                                          &user_header_egress_pmf_offset_0_dummy,
                                                          &user_header_egress_pmf_offset_1_dummy));

        udh_size = is_up ? (user_header_1_size / 8 + user_header_0_size / 8) : 0;
    }
    else
    {
        SHR_IF_ERR_EXIT(oamp_tst_udh_header_size_get(unit, (uint32 *) &udh_size));
    }

    if (mep_db_entry.mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM)
    {   
        if (!_SHR_IS_FLAG_SET(mep_db_entry.flags, DNX_OAMP_OAM_CCM_MEP_IS_UPMEP) && (dnx_drv_soc_property_get(unit, "injection_with_user_header_enable", 0      
                                                                                     ) == 1))
        {
            sal_memset(&header_buffer[offset], 0, udh_size);
        }

        
        if (mep_db_entry.nof_vlan_tags == 1)
        {
            
            vlans[0] = mep_db_entry.inner_vid;
            if (lb_tst_info->pkt_pri != 0xff)
            {
                
                vlans[0] = ((lb_tst_info->pkt_pri & 0xf) << 12) + (mep_db_entry.inner_vid & 0xfff);
            }
            SHR_IF_ERR_EXIT(dnx_oam_oamp_tpid_profile_get(unit, mep_db_entry.inner_tpid_index, &tpids[0]));
        }
        else if (mep_db_entry.nof_vlan_tags == 2)
        {
            vlans[1] = mep_db_entry.outer_vid;
            if (lb_tst_info->inner_pkt_pri != 0xff)
            {
                
                vlans[1] = ((lb_tst_info->inner_pkt_pri & 0xf) << 12) + (mep_db_entry.outer_vid & 0xfff);
            }
            SHR_IF_ERR_EXIT(dnx_oam_oamp_tpid_profile_get(unit, mep_db_entry.outer_tpid_index, &tpids[1]));
        }

        
        SHR_IF_ERR_EXIT(dnx_oam_oamp_sa_mac_msb_profile_get(unit, mep_db_entry.sa_gen_msb_profile, sa_mac_addr_msb));
        sal_memcpy(gtf_smac, sa_mac_addr_msb, DNX_OAM_MAC_SIZE);
        
        gtf_smac[5] = (uint8) mep_db_entry.sa_gen_lsb;
        gtf_smac[4] = (gtf_smac[4] & ~DNX_OAM_TST_LB_ETH_SA_MAC_SECOND_BYTE_MASK)
            | (uint8) ((mep_db_entry.sa_gen_lsb >> SAL_UINT8_NOF_BITS) & DNX_OAM_TST_LB_ETH_SA_MAC_SECOND_BYTE_MASK);
        sal_memcpy(peer_d_mac, lb_tst_info->mac_address, sizeof(bcm_mac_t));

        offset =
            _dnx_ins_eth_for_tst_lb(header_buffer, offset, peer_d_mac, gtf_smac, tpids, vlans,
                                    mep_db_entry.nof_vlan_tags, ether_type);
    }
    else
    {
        
        dnx_PPH_base_head pph;
        uint8 exp, ttl;
        offset = _dnx_ins_itmh_ext(unit, header_buffer, offset, 0, &mep_db_entry, is_jericho_mode);
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, OAMP_REG_0127r, 0, 0, reg_data););

        pph.field1 = 0;
        
        pph.field2 = 0x4;
        pph.field3 = udh_size;
        pph.field4 = 0;
        pph.field5 = 0;
        
        COMPILER_64_COPY(work_reg_64, *reg_data);
        pph.field6 = soc_reg64_field32_get(unit, OAMP_REG_0127r, work_reg_64, FIELD_6_7f);
        pph.field7 = soc_reg64_field32_get(unit, OAMP_REG_0127r, work_reg_64, FIELD_11_11f);
        pph.field8 = 0;
        pph.field9 = soc_reg64_field32_get(unit, OAMP_REG_0127r, work_reg_64, FIELD_12_27f);
        pph.field10 = soc_reg64_field32_get(unit, OAMP_REG_0127r, work_reg_64, FIELD_28_45f);

        offset = _dnx_ins_pph_for_tst_lb(header_buffer, &pph, offset, is_jericho_mode);

        
        sal_memset(&header_buffer[offset], 0, udh_size);
        offset = offset + udh_size;

        SHR_IF_ERR_EXIT(dnx_oam_oamp_mpls_pwe_exp_ttl_profile_get(unit, mep_db_entry.push_profile, &exp, &ttl));

        if (mep_db_entry.mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP)
        {
            
            offset = _dnx_ins_mpls_label_for_tst_lb(header_buffer, mep_db_entry.label, exp, 0, ttl, offset);
            SHR_IF_ERR_EXIT(_dnx_oam_y1731_mpls_tp_gach_gal_get(unit, &gach, &gal));
            offset = _dnx_ins_gal_gach_for_tst_lb(header_buffer, gal, offset, 0);
            offset = _dnx_ins_gal_gach_for_tst_lb(header_buffer, gach, offset, 1);
        }
        else if (mep_db_entry.mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE)
        {
            offset = _dnx_ins_mpls_label_for_tst_lb(header_buffer, mep_db_entry.label, exp, bos, ttl, offset);
            SHR_IF_ERR_EXIT(bcm_dnx_oam_mpls_tp_channel_type_tx_get(unit, bcmOamMplsTpChannelPweonoam, &pwe_gach));
            offset = _dnx_ins_gal_gach_for_tst_lb(header_buffer, pwe_gach, offset, 1);
        }
    }

    
    offset = _dnx_ins_oam_pdu_header_for_tst_lb(header_buffer, mep_db_entry.mdl, lb_tst_info->flags, offset);
    *header_offset = offset;

exit:
    SHR_FUNC_EXIT;
}

static int
_dnx_ins_oam_data_tlv_head_for_tst_lb(
    uint8 *buffer,
    int length,
    int byte_start_offset)
{
    int offset = byte_start_offset;

    buffer[offset++] = 0;       
    buffer[offset++] = 0;
    buffer[offset++] = 0;
    buffer[offset++] = 0;
    buffer[offset++] = 3;       
    buffer[offset++] = (length >> 8) & 0xff;    
    buffer[offset++] = (length & 0xff); 

    return offset;
}

static int
_dnx_ins_oam_test_tlv_head_for_tst_lb(
    uint8 *buffer,
    int length,
    int patten_type,
    int byte_start_offset)
{
    int offset = byte_start_offset;
    offset += 4;
    buffer[offset++] = 32;      
    buffer[offset++] = (length >> 8) & 0xff;    
    buffer[offset++] = (length & 0xff); 

    
    switch (patten_type)
    {
        case bcmOamTlvTypeTestPrbsWithCRC:
            buffer[offset++] = 3;
            break;
        case bcmOamTlvTypeTestPrbsWithoutCRC:
            buffer[offset++] = 2;
            break;
        case bcmOamTlvTypeTestNullWithCRC:
            buffer[offset++] = 1;
            break;
        case bcmOamTlvTypeTestNullWithoutCRC:
            buffer[offset++] = 0;
            break;
        default:
            break;
    }

    return offset;
}
shr_error_e
_dnx_oam_tst_rx_add_verify(
    int unit,
    const bcm_oam_tst_rx_t * tst_ptr)
{
    uint32 tst_trap_used_cnt = 0;
    int nof_max_endpoints;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(tst_ptr, _SHR_E_PARAM, "tst_ptr Is NULL\n");
    nof_max_endpoints = dnx_data_oam.oamp.max_nof_endpoint_id_get(unit);
    if (tst_ptr->endpoint_id < 0 || tst_ptr->endpoint_id >= nof_max_endpoints)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Endpoint id should be in range 0-%d \n\t", (nof_max_endpoints - 1));
    }
    if ((tst_ptr->flags & BCM_OAM_TST_RX_WITH_ID) || (tst_ptr->flags & BCM_OAM_TST_RX_UPDATE))
    {
        SHR_RANGE_VERIFY(tst_ptr->ctf_id, DNX_SAT_CTF_ID_MIN, DNX_SAT_CTF_ID_MAX, _SHR_E_PARAM, "ctf_id");
    }

    if ((tst_ptr->expected_tlv.tlv_type != bcmOamTlvTypeNone)
        && (tst_ptr->expected_tlv.tlv_length < DNX_TST_TVL_LENGTH_MIN))
    {
        SHR_ERR_EXIT(BCM_E_PARAM,
                     " TLV length should be larger than 34 ( Make sure that packet will be larger than 64 bytes) .");
    }
    else if ((tst_ptr->expected_tlv.tlv_type == bcmOamTlvTypeNone) && (tst_ptr->expected_tlv.tlv_length != 0))
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "  TLV length should be 0 for tlvNone \n");
    }

    SHR_IF_ERR_EXIT(oam_sw_db_info.tst_trap_used_cnt.get(unit, &tst_trap_used_cnt));
    if ((tst_trap_used_cnt & DNX_PP_OAM_LOOPBACK_TST_TRAP_FLAGS_LBM))
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "Trap code bcmRxTrapEgTxOamUpMEPDest2 already used by LB.");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dnx_oam_tst_tx_add_verify(
    int unit,
    const bcm_oam_tst_tx_t * tst_ptr)
{
    int found = 0;
    int nof_max_endpoints;
    DNX_OAM_SAT_GTF_ENTRY gtf_info;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(tst_ptr, _SHR_E_PARAM, "tst_ptr Is NULL\n");
    nof_max_endpoints = dnx_data_oam.oamp.max_nof_endpoint_id_get(unit);
    if (tst_ptr->endpoint_id < 0 || tst_ptr->endpoint_id >= nof_max_endpoints)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Endpoint id should be in range 0-%d \n\t", (nof_max_endpoints - 1));
    }
    if ((tst_ptr->flags & BCM_OAM_TST_TX_WITH_ID) || (tst_ptr->flags & BCM_OAM_TST_TX_UPDATE))
    {
        SHR_RANGE_VERIFY(tst_ptr->gtf_id, DNX_SAT_GTF_ID_MIN, DNX_SAT_GTF_ID_MAX, _SHR_E_PARAM, "gtf_id");
    }
    if (tst_ptr->tlv.tlv_length < DNX_TST_TVL_LENGTH_MIN)
    {
        SHR_ERR_EXIT(BCM_E_PARAM,
                     " TLV length should be longer than 34 ( Since packet should be longer than 64 bytes) .");
    }
    if (tst_ptr->flags & BCM_OAM_TST_TX_UPDATE)
    {   
        SHR_IF_ERR_EXIT(_dnx_oam_sat_gtf_info_get(unit, tst_ptr->gtf_id, &gtf_info, &found));
        if (!found)
        {
            SHR_ERR_EXIT(BCM_E_NOT_FOUND, "gtf_id %d not found.", tst_ptr->gtf_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
bcm_dnx_oam_tst_rx_add(
    int unit,
    bcm_oam_tst_rx_t * tst_ptr)
{
    int found;
    int rv = BCM_E_NONE;
    int trap_id = 0;
    int tst_trap_id = 0;
    int trap_index = 0;
    int trap_data = 0;
    bcm_sat_ctf_t ctf_id = 0;
    DNX_OAM_SAT_CTF_ENTRY ctf_info;
    bcm_sat_ctf_packet_info_t packet_info;
    bcm_oam_endpoint_info_t endpoint_info;
    int index = 0;
    bcm_sat_ctf_identifier_t identifier;
    int session_id = 0, max_session_id;
    int tc = 0;
    int dp = 0;
    int tc_max = 7;
    int dp_max = 3;
    uint32 tst_trap_used_cnt = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_INVOKE_VERIFY_DNX(_dnx_oam_tst_rx_add_verify(unit, tst_ptr));

    bcm_oam_endpoint_info_t_init(&endpoint_info);
    SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_get(unit, tst_ptr->endpoint_id, &endpoint_info));

    
    if (!DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(&endpoint_info))
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "Endpoint %d is not accelerated.", tst_ptr->endpoint_id);
    }
    
    SHR_IF_ERR_EXIT(bcm_dnx_rx_trap_type_get(unit, 0, bcmRxTrapEgTxOamUpMEPDest2, &tst_trap_id));
    trap_id = DNX_RX_TRAP_ID_TYPE_GET(tst_trap_id);

    
    if (tst_ptr->flags & BCM_OAM_TST_RX_WITH_ID)        
    {
        ctf_id = tst_ptr->ctf_id;
        
        SHR_IF_ERR_EXIT(bcm_dnx_sat_ctf_create(unit, BCM_SAT_CTF_WITH_ID, &ctf_id));
    }
    else if (tst_ptr->flags & BCM_OAM_TST_RX_UPDATE)    
    {
        ctf_id = tst_ptr->ctf_id;
        rv = _dnx_oam_sat_ctf_info_get(unit, ctf_id, &ctf_info, &found);
        if (!found)
        {
            SHR_ERR_EXIT(BCM_E_NOT_FOUND, "ctf_id %d not found.", ctf_id);
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }
    }
    else
    {
        
        SHR_IF_ERR_EXIT(bcm_dnx_sat_ctf_create(unit, 0, &ctf_id));
    }

    tst_ptr->ctf_id = ctf_id;   
    ctf_info.flag = DNX_PP_OAM_LOOPBACK_TST_INFO_FLAGS_TST;     
    ctf_info.mepid = tst_ptr->endpoint_id;

    if (!(tst_ptr->flags & BCM_OAM_TST_RX_UPDATE))
    {
        if ((tst_trap_used_cnt & DNX_PP_OAM_LOOPBACK_TST_TRAP_CNT_MASK) == 0)
        {
            rv = bcm_dnx_sat_ctf_trap_add(unit, trap_id);
            SHR_IF_ERR_EXIT(rv);
        }
        tst_trap_used_cnt++;
        SHR_IF_ERR_EXIT(oam_sw_db_info.
                        tst_trap_used_cnt.set(unit, tst_trap_used_cnt | DNX_PP_OAM_LOOPBACK_TST_TRAP_FLAGS_TST));

        trap_data = tst_ptr->endpoint_id;
        SHR_IF_ERR_EXIT(dnx_sat_max_value_get(unit, dnxSATCtfTrapSessionId, &max_session_id));
        
        if (!ctf_info.session_map_flag)
        {       
            for (session_id = 0; session_id < max_session_id; session_id++)
            {
                SHR_IF_ERR_EXIT(dnx_sat_session_inuse(unit, session_id, &found));
                if (found)
                {
                    continue;   
                }
                rv = bcm_dnx_sat_ctf_trap_data_to_session_map(unit, trap_data, 0, session_id);  
                SHR_IF_ERR_EXIT(rv);
                break;
            }
        }
        ctf_info.session_map_flag++;

        SHR_IF_ERR_EXIT(dnx_sat_trap_idx_get(unit, trap_id, &trap_index));

        
        bcm_sat_ctf_identifier_t_init(&identifier);
        for (dp = 0; dp <= dp_max; dp++)
        {
            for (tc = 0; tc <= tc_max; tc++)
            {
                identifier.session_id = session_id;
                identifier.trap_id = trap_index;
                identifier.color = dp;
                identifier.tc = tc;
                rv = bcm_dnx_sat_ctf_identifier_map(unit, &identifier, ctf_id);
                SHR_IF_ERR_EXIT(rv);
            }
        }
        
        ctf_info.identifier_session_id = session_id;    
        ctf_info.identifier_trap_id = trap_index;       

    }

    
    bcm_sat_ctf_packet_info_t_init(&packet_info);
    packet_info.sat_header_type = bcmSatHeaderY1731;

    
    packet_info.offsets.seq_number_offset = DNX_TST_LB_PKT_SAT_SEQ_NUM_OFFSET;

    if (tst_ptr->expected_tlv.tlv_type == bcmOamTlvTypeData)
    {
        packet_info.offsets.payload_offset = DNX_TST_LB_PKT_PLAYLOAD_OFFSET;
    }
    else
    {
        packet_info.offsets.payload_offset = DNX_TST_LB_PKT_PLAYLOAD_OFFSET + 1;        
    }

    
    if ((tst_ptr->expected_tlv.tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
        || (tst_ptr->expected_tlv.tlv_type == bcmOamTlvTypeTestPrbsWithoutCRC))
    {
        packet_info.payload.payload_type = bcmSatPayloadPRBS;
    }
    else if ((tst_ptr->expected_tlv.tlv_type == bcmOamTlvTypeTestNullWithCRC)
             || (tst_ptr->expected_tlv.tlv_type == bcmOamTlvTypeTestNullWithoutCRC))
    {
        packet_info.payload.payload_type = bcmSatPayloadConstant4Bytes;
        for (index = 0; index < BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE; index++)
        {
            packet_info.payload.payload_pattern[index] = 0;
        }
    }

    
    if ((tst_ptr->expected_tlv.tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
        || (tst_ptr->expected_tlv.tlv_type == bcmOamTlvTypeTestNullWithCRC))
    {
        
        packet_info.offsets.crc_byte_offset = 8;
        packet_info.flags = BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV | BCM_SAT_CTF_PACKET_INFO_ADD_CRC;
    }
    else
    {
        packet_info.flags = BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV;
    }
    rv = bcm_dnx_sat_ctf_packet_config(unit, ctf_id, &packet_info);
    SHR_IF_ERR_EXIT(rv);

    
    ctf_info.tlv.tlv_type = tst_ptr->expected_tlv.tlv_type;
    ctf_info.tlv.tlv_length = tst_ptr->expected_tlv.tlv_length;

    SHR_IF_ERR_EXIT(_dnx_oam_sat_ctf_info_set(unit, ctf_id, &ctf_info));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
bcm_dnx_oam_tst_rx_get(
    int unit,
    bcm_oam_tst_rx_t * tst_ptr)
{
    int found = 0;
    int rv = BCM_E_NONE;
    DNX_OAM_SAT_CTF_ENTRY ctf_info;
    bcm_sat_ctf_stat_t stat;
    int index = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_NULL_CHECK(tst_ptr, _SHR_E_PARAM, "tst_ptr Is NULL\n");

    
    for (index = 0; index < dnx_data_sat.collector.nof_ctf_ids_get(unit); index++)
    {
        SHR_IF_ERR_EXIT(_dnx_oam_sat_ctf_info_get(unit, index, &ctf_info, &found));
        if (found && (ctf_info.flag == DNX_PP_OAM_LOOPBACK_TST_INFO_FLAGS_TST)
            && (ctf_info.mepid == tst_ptr->endpoint_id))
        {
            
            break;
        }
    }
    if (index == dnx_data_sat.collector.nof_ctf_ids_get(unit))
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No endpoint with id %d found.", tst_ptr->endpoint_id);
    }
    tst_ptr->ctf_id = index;
    tst_ptr->expected_tlv.tlv_type = ctf_info.tlv.tlv_type;
    tst_ptr->expected_tlv.tlv_length = ctf_info.tlv.tlv_length;
    
    bcm_sat_ctf_stat_t_init(&stat);
    rv = bcm_dnx_sat_ctf_stat_get(unit, tst_ptr->ctf_id, 0, &stat);
    SHR_IF_ERR_EXIT(rv);

    COMPILER_64_COPY(tst_ptr->rx_count, stat.received_packet_cnt);
    COMPILER_64_COPY(tst_ptr->invalid_tlv_count, stat.err_packet_cnt);
    COMPILER_64_COPY(tst_ptr->out_of_sequence, stat.out_of_order_packet_cnt);

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
bcm_dnx_oam_tst_rx_delete(
    int unit,
    bcm_oam_tst_rx_t * tst_ptr)
{
    int found = 0;
    int rv = BCM_E_NONE;
    int trap_id = 0;
    uint32 hw_trap_id = 0;
    int trap_data = 0;
    int tst_trap_id = 0;
    bcm_sat_ctf_t ctf_id = 0;
    DNX_OAM_SAT_CTF_ENTRY ctf_info;
    bcm_sat_ctf_identifier_t identifier;
    bcm_oam_endpoint_info_t endpoint_info;
    int tc = 0;
    int dp = 0;
    int tc_max = 7;
    int dp_max = 3;
    uint32 tst_trap_used_cnt = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_NULL_CHECK(tst_ptr, _SHR_E_PARAM, "tst_ptr Is NULL\n");

    ctf_id = tst_ptr->ctf_id;
    rv = _dnx_oam_sat_ctf_info_get(unit, tst_ptr->ctf_id, &ctf_info, &found);
    if (!found)
    {
        SHR_ERR_EXIT(BCM_E_NOT_FOUND, "ctf_id %d not found.", ctf_id);
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

    SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_get(unit, tst_ptr->endpoint_id, &endpoint_info));
    
    if (!DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(&endpoint_info))
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "Endpoint %d is not accelerated.", tst_ptr->endpoint_id);
    }

    
    SHR_IF_ERR_EXIT(bcm_dnx_rx_trap_type_get(unit, 0, bcmRxTrapEgTxOamUpMEPDest2, &tst_trap_id));
    trap_id = DNX_RX_TRAP_ID_TYPE_GET(tst_trap_id);

    ctf_info.session_map_flag--;
    if (ctf_info.session_map_flag == 0) 
    {
        trap_data = tst_ptr->endpoint_id;       
        rv = bcm_dnx_sat_ctf_trap_data_to_session_unmap(unit, trap_data, 0);
        SHR_IF_ERR_EXIT(rv);
    }

    
    identifier.session_id = ctf_info.identifier_session_id;     
    identifier.trap_id = ctf_info.identifier_trap_id;

    
    
    for (dp = 0; dp <= dp_max; dp++)
    {
        for (tc = 0; tc <= tc_max; tc++)
        {
            identifier.color = dp;
            identifier.tc = tc;
            rv = bcm_dnx_sat_ctf_identifier_unmap(unit, &identifier);
            SHR_IF_ERR_EXIT(rv);
        }
    }
    SHR_IF_ERR_EXIT(rv);

    
    rv = bcm_dnx_sat_ctf_destroy(unit, ctf_id);
    SHR_IF_ERR_EXIT(rv);

    hw_trap_id = trap_id;

    SHR_IF_ERR_EXIT(oam_sw_db_info.tst_trap_used_cnt.get(unit, &tst_trap_used_cnt));

    if ((tst_trap_used_cnt & DNX_PP_OAM_LOOPBACK_TST_TRAP_CNT_MASK))
    {
        tst_trap_used_cnt--;
    }
    
    if ((tst_trap_used_cnt & DNX_PP_OAM_LOOPBACK_TST_TRAP_CNT_MASK) == 0)
    {
        rv = bcm_dnx_sat_ctf_trap_remove(unit, hw_trap_id);
        tst_trap_used_cnt = 0;
        SHR_IF_ERR_EXIT(rv);
    }
    SHR_IF_ERR_EXIT(oam_sw_db_info.tst_trap_used_cnt.set(unit, tst_trap_used_cnt));

    
    rv = _dnx_oam_sat_ctf_info_del(unit, ctf_id);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
bcm_dnx_oam_tst_tx_add(
    int unit,
    bcm_oam_tst_tx_t * tst_ptr)
{
    int found = 0;
    int rv = BCM_E_NONE;
    bcm_sat_gtf_t gtf_id = 0;
    bcm_oam_endpoint_info_t endpoint_info;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    int core_id;
    uint32 tm_port;
    bcm_pkt_t *sat_header = NULL;
    bcm_sat_gtf_packet_config_t *p_config = NULL;

    dnx_oam_oamp_ccm_endpoint_t mep_db_entry;
    DNX_OAM_SAT_GTF_ENTRY gtf_info;
    DNX_PP_OAM_LOOPBACK_TST_INFO tst_info;
    uint32 user_header_0_size = 0, user_header_1_size = 0, udh_size = 0, min_size = 0;
    uint32 user_header_egress_pmf_offset_0_dummy = 0, user_header_egress_pmf_offset_1_dummy = 0;
    uint32 gtf_seq_number_offset;
    int offset = 0;
    uint8 is_test_tlv = 0;
    int i = 0, priority = 0;
    uint32 gtf_packet_length_pattern[BCM_SAT_GTF_NUM_OF_PRIORITIES][BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH] = {
        {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7}, {0, 1, 2, 3,
                                                                                                           4, 5, 6, 7,
                                                                                                           0, 1, 2, 3,
                                                                                                           4, 5, 6, 7,
                                                                                                           0, 1, 2, 3,
                                                                                                           4, 5, 6, 7,
                                                                                                           0, 1, 2, 3,
                                                                                                           4, 5, 6, 7}
    };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(_dnx_oam_tst_tx_add_verify(unit, tst_ptr));

    p_config = sal_alloc(sizeof(bcm_sat_gtf_packet_config_t), "bcm_petra_oam_tst_tx_add");
    if (p_config == NULL)
    {
        SHR_ERR_EXIT(BCM_E_MEMORY, "Memory allocation failue\n");
    }

    
    rv = bcm_pkt_alloc(unit, DNX_TST_LB_SAT_PKT_HDR_LENGTH, 0, &sat_header);
    SHR_IF_ERR_EXIT(rv);
    _dnx_oam_lb_tst_info_t_init(&tst_info);
    bcm_oam_endpoint_info_t_init(&endpoint_info);
    SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_get(unit, tst_ptr->endpoint_id, &endpoint_info));
    if (DNX_OAM_IS_ENDPOINT_RFC6374(&endpoint_info))
    {
        mep_db_entry.mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374;
    }
    else
    {
        
        mep_db_entry.mep_type = 0;
    }
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_get(unit, tst_ptr->endpoint_id,
                                                     _SHR_IS_FLAG_SET(endpoint_info.flags2,
                                                                      BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN),
                                                     &mep_db_entry));

    if (tst_ptr->flags & BCM_OAM_TST_TX_WITH_ID)
    {   
        gtf_id = tst_ptr->gtf_id;
        rv = bcm_dnx_sat_gtf_create(unit, BCM_SAT_GTF_WITH_ID, &gtf_id);
        SHR_IF_ERR_EXIT(rv);
    }
    else if (tst_ptr->flags & BCM_OAM_TST_TX_UPDATE)
    {   
        gtf_id = tst_ptr->gtf_id;
        SHR_IF_ERR_EXIT(_dnx_oam_sat_gtf_info_get(unit, gtf_id, &gtf_info, &found));
    }
    else
    {
        rv = bcm_dnx_sat_gtf_create(unit, 0, &gtf_id);
        SHR_IF_ERR_EXIT(rv);
    }

    
    tst_ptr->gtf_id = gtf_id;   

    gtf_info.flag = DNX_PP_OAM_LOOPBACK_TST_INFO_FLAGS_TST;     
    gtf_info.mepid = tst_ptr->endpoint_id;

    tst_info.flags = DNX_PP_OAM_LOOPBACK_TST_INFO_FLAGS_TST;
    tst_info.endpoint_id = tst_ptr->endpoint_id;
    
    sal_memcpy(tst_info.mac_address, tst_ptr->peer_da_mac_address, sizeof(bcm_mac_t));

    
    SHR_IF_ERR_EXIT(_dnx_pp_oam_oamp_lb_tst_header_set(unit, &tst_info,
                                                       _SHR_IS_FLAG_SET(endpoint_info.flags,
                                                                        BCM_OAM_ENDPOINT_UP_FACING),
                                                       sat_header->pkt_data->data, &offset));
    gtf_seq_number_offset = offset;
    
    bcm_sat_gtf_packet_config_t_init(p_config);

    p_config->header_info.pkt_data = sat_header->pkt_data;
    p_config->header_info.pkt_data->len = gtf_seq_number_offset + DNX_TST_LB_PKT_SAT_SEQ_NUM_OFFSET;
    p_config->header_info.blk_count = 1;

    
    p_config->sat_header_type = bcmSatHeaderY1731;

    
    p_config->offsets.seq_number_offset = gtf_seq_number_offset;        

    
    if (tst_ptr->tlv.tlv_type == bcmOamTlvTypeData)
    {
        offset = _dnx_ins_oam_data_tlv_head_for_tst_lb(sat_header->pkt_data->data, tst_ptr->tlv.tlv_length, offset);
    }
    
    else if ((tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
             || (tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestPrbsWithoutCRC)
             || (tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestNullWithCRC)
             || (tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestNullWithoutCRC))
    {
        is_test_tlv = 1;
        offset =
            _dnx_ins_oam_test_tlv_head_for_tst_lb(sat_header->pkt_data->data, tst_ptr->tlv.tlv_length,
                                                  tst_ptr->tlv.tlv_type, offset);
    }

    sat_header->pkt_data->len = offset; 

    
    if ((tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
        || (tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestPrbsWithoutCRC))
    {
        p_config->payload.payload_type = bcmSatPayloadPRBS;
        
    }
    else if ((tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestNullWithCRC)
             || (tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestNullWithoutCRC))
    {
        p_config->payload.payload_type = bcmSatPayloadConstant4Bytes;   
        for (i = 0; i < BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE; i++)
        {
            p_config->payload.payload_pattern[i] = 0;
        }
    }
    else        
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "Endpoint %d ,the TLV bype of  tst Tx was wrong .", tst_ptr->endpoint_id);
    }
    

    if (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
        DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        SHR_IF_ERR_EXIT(_dnx_db_fes_user_header_sizes_get(unit,
                                                          &user_header_0_size,
                                                          &user_header_1_size,
                                                          &user_header_egress_pmf_offset_0_dummy,
                                                          &user_header_egress_pmf_offset_1_dummy));

        udh_size =
            _SHR_IS_FLAG_SET(endpoint_info.flags2,
                             BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN) ? 0 : (user_header_1_size / 8 +
                                                                                   user_header_0_size / 8);
    }
    else
    {
        SHR_IF_ERR_EXIT(oamp_tst_udh_header_size_get(unit, (uint32 *) &udh_size));
    }
    
    for (priority = 0; priority < BCM_SAT_GTF_NUM_OF_PRIORITIES; priority++)
    {
        
        for (i = 0; i < BCM_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS; i++)
        {
            
            p_config->packet_edit[priority].packet_length[i] = offset - (is_test_tlv ? 1 : 0) + tst_ptr->tlv.tlv_length + 1;    
            min_size = _SHR_IS_FLAG_SET(endpoint_info.flags2, BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN) ? 62 : 67;
            if (p_config->packet_edit[priority].packet_length[i] < (min_size + udh_size))
            {
                p_config->packet_edit[priority].packet_length[i] = min_size + udh_size; 
            }
        }
        for (i = 0; i < BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH; i++)
        {
            p_config->packet_edit[priority].packet_length_pattern[i] = gtf_packet_length_pattern[0][i];;
        }
        p_config->packet_edit[priority].pattern_length = 1;     
        p_config->packet_edit[priority].number_of_ctfs = 1;
        
        if ((tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
            || (tst_ptr->tlv.tlv_type == bcmOamTlvTypeTestNullWithCRC))
        {
            
            p_config->offsets.crc_byte_offset = gtf_seq_number_offset + DNX_TST_LB_PKT_SAT_SEQ_NUM_OFFSET;
            p_config->packet_edit[priority].flags =
                BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV | BCM_SAT_CTF_PACKET_INFO_ADD_CRC;
        }
        else
        {
            p_config->packet_edit[priority].flags = BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV;
        }
    }
    if (_SHR_IS_FLAG_SET(endpoint_info.flags, BCM_OAM_ENDPOINT_UP_FACING))
    {
        bcm_vlan_port_t vlan_port;
        vlan_port.vlan_port_id = endpoint_info.gport;
        SHR_IF_ERR_EXIT(bcm_dnx_vlan_port_find(unit, &vlan_port));
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, vlan_port.port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
        
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, gport_info.local_port, &core_id, &tm_port));
        p_config->packet_context_id = core_id;
    }
    SHR_IF_ERR_EXIT(bcm_dnx_sat_gtf_packet_config(unit, gtf_id, p_config));

    
    gtf_info.tlv.tlv_type = tst_ptr->tlv.tlv_type;
    gtf_info.tlv.tlv_length = tst_ptr->tlv.tlv_length;
    
    rv = _dnx_oam_sat_gtf_info_set(unit, gtf_id, &gtf_info);
    SHR_IF_ERR_EXIT(rv);
exit:
    bcm_pkt_free(unit, sat_header);
    SHR_FREE(p_config);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
bcm_dnx_oam_tst_tx_delete(
    int unit,
    bcm_oam_tst_tx_t * tst_ptr)
{
    bcm_sat_gtf_t gtf_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    SHR_NULL_CHECK(tst_ptr, _SHR_E_PARAM, "tst_ptr Is NULL\n");

    
    SHR_IF_ERR_EXIT(bcm_dnx_sat_gtf_destroy(unit, tst_ptr->gtf_id));

    
    SHR_IF_ERR_EXIT(_dnx_oam_sat_gtf_info_del(unit, gtf_id));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
bcm_dnx_oam_tst_tx_get(
    int unit,
    bcm_oam_tst_tx_t * tst_ptr)
{
    int found = 0;
    int index = 0;
    DNX_OAM_SAT_GTF_ENTRY gtf_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(tst_ptr, _SHR_E_PARAM, "tst_ptr Is NULL\n");

    
    for (index = 0; index < dnx_data_sat.generator.nof_gtf_ids_get(unit); index++)
    {
        SHR_IF_ERR_EXIT(_dnx_oam_sat_gtf_info_get(unit, index, &gtf_info, &found));
        if (found && (gtf_info.flag == DNX_PP_OAM_LOOPBACK_TST_INFO_FLAGS_LBM)
            && (gtf_info.mepid == tst_ptr->endpoint_id))
        {
            
            tst_ptr->gtf_id = index;
            tst_ptr->tlv.tlv_type = gtf_info.tlv.tlv_type;
            tst_ptr->tlv.tlv_length = gtf_info.tlv.tlv_length;
            break;
        }
    }
    if (index == dnx_data_sat.generator.nof_gtf_ids_get(unit))
    {
        SHR_IF_ERR_EXIT(_SHR_E_NOT_FOUND);
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
_dnx_oam_lb_rx_add_verify(
    int unit,
    const bcm_oam_loopback_t * lb_ptr)
{
    uint32 tst_trap_used_cnt = 0;
    int nof_max_endpoints;
    int i;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(lb_ptr, _SHR_E_PARAM, "lb_ptr Is NULL\n");
    nof_max_endpoints = dnx_data_oam.oamp.max_nof_endpoint_id_get(unit);
    if (lb_ptr->id < 0 || lb_ptr->id >= nof_max_endpoints)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Endpoint id should be in range 0-%d \n\t", (nof_max_endpoints - 1));
    }
    if ((lb_ptr->flags & BCM_OAM_LOOPBACK_WITH_CTF_ID) || (lb_ptr->flags & BCM_OAM_LOOPBACK_UPDATE))
    {
        SHR_RANGE_VERIFY(lb_ptr->ctf_id, DNX_SAT_CTF_ID_MIN, DNX_SAT_CTF_ID_MAX, _SHR_E_PARAM, "ctf_id");
    }
    if ((lb_ptr->flags & BCM_OAM_TST_TX_WITH_ID) || (lb_ptr->flags & BCM_OAM_TST_TX_UPDATE))
    {
        SHR_RANGE_VERIFY(lb_ptr->gtf_id, DNX_SAT_GTF_ID_MIN, DNX_SAT_GTF_ID_MAX, _SHR_E_PARAM, "gtf_id");
    }

    for (i = 0; i < lb_ptr->num_tlvs; i++)
    {
        if ((lb_ptr->tlvs[i].tlv_type != bcmOamTlvTypeNone) && (lb_ptr->tlvs[i].tlv_length < DNX_TST_TVL_LENGTH_MIN))
        {
            SHR_ERR_EXIT(BCM_E_PARAM,
                         " TLV length should be larger than 34 ( Make sure that packet will be larger than 64 bytes) .");
        }
        else if ((lb_ptr->tlvs[i].tlv_type == bcmOamTlvTypeNone) && (lb_ptr->tlvs[i].tlv_length != 0))
        {
            SHR_ERR_EXIT(BCM_E_PARAM, "  TLV length should be 0 for tlvNone \n");
        }
    }
    SHR_IF_ERR_EXIT(oam_sw_db_info.tst_trap_used_cnt.get(unit, &tst_trap_used_cnt));
    if ((tst_trap_used_cnt & DNX_PP_OAM_LOOPBACK_TST_TRAP_FLAGS_TST))
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "Trap code bcmRxTrapEgTxOamUpMEPDest2 already used by LB.");
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
_dnx_sat_loopback_add_rx(
    int unit,
    bcm_oam_loopback_t * lb_ptr)
{
    int found;
    int rv = BCM_E_NONE;
    int trap_id = 0;
    int lb_trap_id = 0;
    int trap_index = 0;
    int trap_data = 0;
    bcm_sat_ctf_t ctf_id = 0;
    DNX_OAM_SAT_CTF_ENTRY ctf_info;
    bcm_sat_ctf_packet_info_t packet_info;
    bcm_oam_endpoint_info_t endpoint_info;
    int index = 0;
    int index_2 = 0;
    bcm_sat_ctf_identifier_t identifier;
    int session_id = 0, max_session_id;
    int tc = 0;
    int dp = 0;
    int tc_max = 7;
    int dp_max = 3;
    uint32 lb_trap_used_cnt = 0;
    SHR_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(_dnx_oam_lb_rx_add_verify(unit, lb_ptr));

    bcm_oam_endpoint_info_t_init(&endpoint_info);
    SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_get(unit, lb_ptr->id, &endpoint_info));

    
    if (!DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(&endpoint_info))
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "Endpoint %d is not accelerated.", lb_ptr->id);
    }

    
    SHR_IF_ERR_EXIT(bcm_dnx_rx_trap_type_get(unit, 0, bcmRxTrapEgTxOamUpMEPDest2, &lb_trap_id));
    trap_id = DNX_RX_TRAP_ID_TYPE_GET(lb_trap_id);

    
    if (lb_ptr->flags & BCM_OAM_LOOPBACK_WITH_CTF_ID)   
    {
        ctf_id = lb_ptr->ctf_id;
        
        SHR_IF_ERR_EXIT(bcm_dnx_sat_ctf_create(unit, BCM_SAT_CTF_WITH_ID, &ctf_id));
    }
    else if (lb_ptr->flags & BCM_OAM_LOOPBACK_UPDATE)   
    {
        ctf_id = lb_ptr->ctf_id;
        rv = _dnx_oam_sat_ctf_info_get(unit, ctf_id, &ctf_info, &found);
        if (!found)
        {
            SHR_ERR_EXIT(BCM_E_NOT_FOUND, "ctf_id %d not found.", ctf_id);
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }
    }
    else
    {
        
        SHR_IF_ERR_EXIT(bcm_dnx_sat_ctf_create(unit, 0, &ctf_id));
    }

    lb_ptr->ctf_id = ctf_id;    
    ctf_info.flag = DNX_PP_OAM_LOOPBACK_TST_INFO_FLAGS_LBM;     
    ctf_info.mepid = lb_ptr->id;

    if (!(lb_ptr->flags & BCM_OAM_LOOPBACK_UPDATE))
    {
        if ((lb_trap_used_cnt & DNX_PP_OAM_LOOPBACK_TST_TRAP_CNT_MASK) == 0)
        {
            rv = bcm_dnx_sat_ctf_trap_add(unit, trap_id);
            SHR_IF_ERR_EXIT(rv);
        }
        lb_trap_used_cnt++;
        SHR_IF_ERR_EXIT(oam_sw_db_info.
                        tst_trap_used_cnt.set(unit, lb_trap_used_cnt | DNX_PP_OAM_LOOPBACK_TST_TRAP_FLAGS_LBM));

        trap_data = lb_ptr->id;
        SHR_IF_ERR_EXIT(dnx_sat_max_value_get(unit, dnxSATCtfTrapSessionId, &max_session_id));
        
        if (!ctf_info.session_map_flag)
        {       
            for (session_id = 0; session_id < max_session_id; session_id++)
            {
                SHR_IF_ERR_EXIT(dnx_sat_session_inuse(unit, session_id, &found));
                if (found)
                {
                    continue;   
                }
                rv = bcm_dnx_sat_ctf_trap_data_to_session_map(unit, trap_data, 0, session_id);  
                SHR_IF_ERR_EXIT(rv);
                break;
            }
        }
        ctf_info.session_map_flag++;

        SHR_IF_ERR_EXIT(dnx_sat_trap_idx_get(unit, trap_id, &trap_index));

        
        bcm_sat_ctf_identifier_t_init(&identifier);
        for (dp = 0; dp <= dp_max; dp++)
        {
            for (tc = 0; tc <= tc_max; tc++)
            {
                identifier.session_id = session_id;
                identifier.trap_id = trap_index;
                identifier.color = dp;
                identifier.tc = tc;
                rv = bcm_dnx_sat_ctf_identifier_map(unit, &identifier, ctf_id);
                SHR_IF_ERR_EXIT(rv);
            }
        }
        
        ctf_info.identifier_session_id = session_id;    
        ctf_info.identifier_trap_id = trap_index;       

    }
    
    bcm_sat_ctf_packet_info_t_init(&packet_info);
    packet_info.sat_header_type = bcmSatHeaderY1731;

    
    packet_info.offsets.seq_number_offset = 4;

    for (index = 0; index < BCM_OAM_MAX_NUM_TLVS_FOR_LBM; index++)
    {
        if (lb_ptr->tlvs[index].tlv_type == bcmOamTlvTypeData)
        {
            packet_info.offsets.payload_offset = 11;
        }
        else
        {
            packet_info.offsets.payload_offset = 12;    
        }

        
        if ((lb_ptr->tlvs[index].tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
            || (lb_ptr->tlvs[index].tlv_type == bcmOamTlvTypeTestPrbsWithoutCRC))
        {
            packet_info.payload.payload_type = bcmSatPayloadPRBS;
        }
        else if ((lb_ptr->tlvs[index].tlv_type == bcmOamTlvTypeTestNullWithCRC)
                 || (lb_ptr->tlvs[index].tlv_type == bcmOamTlvTypeTestNullWithoutCRC))
        {
            packet_info.payload.payload_type = bcmSatPayloadConstant4Bytes;
            for (index_2 = 0; index_2 < BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE; index_2++)
            {
                packet_info.payload.payload_pattern[index_2] = 0;
            }
        }

        
        if (lb_ptr->tlvs[index].tlv_type == bcmOamTlvTypeData)
        {
            packet_info.payload.payload_type = bcmSatPayloadConstant4Bytes;
            packet_info.payload.payload_pattern[0] =
                (lb_ptr->tlvs[index].four_byte_repeatable_pattern & 0xFF000000) >> 24;
            packet_info.payload.payload_pattern[1] =
                (lb_ptr->tlvs[index].four_byte_repeatable_pattern & 0xFF0000) >> 16;
            packet_info.payload.payload_pattern[2] = (lb_ptr->tlvs[index].four_byte_repeatable_pattern & 0xFF00) >> 8;
            packet_info.payload.payload_pattern[3] = (lb_ptr->tlvs[index].four_byte_repeatable_pattern & 0xFF);
            packet_info.payload.payload_pattern[4] =
                (lb_ptr->tlvs[index].four_byte_repeatable_pattern & 0xFF000000) >> 24;
            packet_info.payload.payload_pattern[5] =
                (lb_ptr->tlvs[index].four_byte_repeatable_pattern & 0xFF0000) >> 16;
            packet_info.payload.payload_pattern[6] = (lb_ptr->tlvs[index].four_byte_repeatable_pattern & 0xFF00) >> 8;
            packet_info.payload.payload_pattern[7] = (lb_ptr->tlvs[index].four_byte_repeatable_pattern & 0xFF);
        }

        ctf_info.tlv.tlv_length = lb_ptr->tlvs[index].tlv_length;

        
        if ((lb_ptr->tlvs[index].tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
            || (lb_ptr->tlvs[index].tlv_type == bcmOamTlvTypeTestNullWithCRC))
        {
            
            packet_info.offsets.crc_byte_offset = 8;
            packet_info.flags = BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV | BCM_SAT_CTF_PACKET_INFO_ADD_CRC;
        }
        else
        {
            packet_info.flags = BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV;
        }
    }

    rv = bcm_dnx_sat_ctf_packet_config(unit, ctf_id, &packet_info);
    SHR_IF_ERR_EXIT(rv);

    
    ctf_info.tlv.tlv_type = lb_ptr->tlvs[0].tlv_type;
    ctf_info.tlv.tlv_length = lb_ptr->tlvs[0].tlv_length;

    SHR_IF_ERR_EXIT(_dnx_oam_sat_ctf_info_set(unit, ctf_id, &ctf_info));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
_dnx_sat_loopback_add_tx(
    int unit,
    bcm_oam_loopback_t * lb_ptr)
{
    int found = 0;
    int rv = BCM_E_NONE;
    bcm_sat_gtf_t gtf_id = 0;
    bcm_oam_endpoint_info_t endpoint_info;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    int core_id;
    uint32 tm_port;
    bcm_pkt_t *sat_header = NULL;
    bcm_sat_gtf_packet_config_t *p_config = NULL;

    dnx_oam_oamp_ccm_endpoint_t mep_db_entry;
    DNX_OAM_SAT_GTF_ENTRY gtf_info;
    DNX_PP_OAM_LOOPBACK_TST_INFO tst_info;
    uint32 user_header_0_size = 0, user_header_1_size = 0, udh_size = 0, min_size = 0;
    uint32 user_header_egress_pmf_offset_0_dummy = 0, user_header_egress_pmf_offset_1_dummy = 0;
    uint32 gtf_seq_number_offset;
    int offset = 0;
    uint8 is_test_tlv = 0;
    int i = 0, i2 = 0, priority = 0;
    uint32 gtf_packet_length_pattern[BCM_SAT_GTF_NUM_OF_PRIORITIES][BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH] = {
        {0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7, 0, 1, 2, 3, 4, 5, 6, 7}, {0, 1, 2, 3,
                                                                                                           4, 5, 6, 7,
                                                                                                           0, 1, 2, 3,
                                                                                                           4, 5, 6, 7,
                                                                                                           0, 1, 2, 3,
                                                                                                           4, 5, 6, 7,
                                                                                                           0, 1, 2, 3,
                                                                                                           4, 5, 6, 7}
    };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_INVOKE_VERIFY_DNX(_dnx_oam_lb_rx_add_verify(unit, lb_ptr));

    p_config = sal_alloc(sizeof(bcm_sat_gtf_packet_config_t), "bcm_petra_oam_tst_tx_add");
    if (p_config == NULL)
    {
        SHR_ERR_EXIT(BCM_E_MEMORY, "Memory allocation failue\n");
    }

    
    rv = bcm_pkt_alloc(unit, DNX_TST_LB_SAT_PKT_HDR_LENGTH, 0, &sat_header);
    SHR_IF_ERR_EXIT(rv);
    _dnx_oam_lb_tst_info_t_init(&tst_info);
    bcm_oam_endpoint_info_t_init(&endpoint_info);
    SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_get(unit, lb_ptr->id, &endpoint_info));
    if (DNX_OAM_IS_ENDPOINT_RFC6374(&endpoint_info))
    {
        mep_db_entry.mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374;
    }
    else
    {
        
        mep_db_entry.mep_type = 0;
    }
    SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_get(unit, lb_ptr->id,
                                                     _SHR_IS_FLAG_SET(endpoint_info.flags2,
                                                                      BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN),
                                                     &mep_db_entry));

    if (lb_ptr->flags & BCM_OAM_TST_TX_WITH_ID)
    {   
        gtf_id = lb_ptr->gtf_id;
        rv = bcm_dnx_sat_gtf_create(unit, BCM_SAT_GTF_WITH_ID, &gtf_id);
        SHR_IF_ERR_EXIT(rv);
    }
    else if (lb_ptr->flags & BCM_OAM_TST_TX_UPDATE)
    {   
        gtf_id = lb_ptr->gtf_id;
        SHR_IF_ERR_EXIT(_dnx_oam_sat_gtf_info_get(unit, gtf_id, &gtf_info, &found));
    }
    else
    {
        rv = bcm_dnx_sat_gtf_create(unit, 0, &gtf_id);
        SHR_IF_ERR_EXIT(rv);
    }

    
    lb_ptr->gtf_id = gtf_id;    

    gtf_info.flag = DNX_PP_OAM_LOOPBACK_TST_INFO_FLAGS_LBM;     
    gtf_info.mepid = lb_ptr->id;

    tst_info.flags = DNX_PP_OAM_LOOPBACK_TST_INFO_FLAGS_LBM;
    tst_info.endpoint_id = lb_ptr->id;
    
    sal_memcpy(tst_info.mac_address, lb_ptr->peer_da_mac_address, sizeof(bcm_mac_t));

    
    SHR_IF_ERR_EXIT(_dnx_pp_oam_oamp_lb_tst_header_set(unit, &tst_info,
                                                       _SHR_IS_FLAG_SET(endpoint_info.flags,
                                                                        BCM_OAM_ENDPOINT_UP_FACING),
                                                       sat_header->pkt_data->data, &offset));
    gtf_seq_number_offset = offset;
    
    bcm_sat_gtf_packet_config_t_init(p_config);

    p_config->header_info.pkt_data = sat_header->pkt_data;
    p_config->header_info.pkt_data->len = gtf_seq_number_offset + DNX_TST_LB_PKT_SAT_SEQ_NUM_OFFSET;
    p_config->header_info.blk_count = 1;

    
    p_config->sat_header_type = bcmSatHeaderY1731;

    
    p_config->offsets.seq_number_offset = gtf_seq_number_offset;        

    
    
    for (i = 0; i < lb_ptr->num_tlvs; i++)
    {
        
        if (lb_ptr->tlvs[i].tlv_type == bcmOamTlvTypeData)
        {
            offset =
                _dnx_ins_oam_data_tlv_head_for_tst_lb(sat_header->pkt_data->data, lb_ptr->tlvs[i].tlv_length, offset);
        }
        
        else if ((lb_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
                 || (lb_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestPrbsWithoutCRC)
                 || (lb_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestNullWithCRC)
                 || (lb_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestNullWithoutCRC))
        {
            is_test_tlv = 1;
            offset =
                _dnx_ins_oam_test_tlv_head_for_tst_lb(sat_header->pkt_data->data, lb_ptr->tlvs[i].tlv_length,
                                                      lb_ptr->tlvs[i].tlv_type, offset);
        }
        else if (lb_ptr->tlvs[i].tlv_type == bcmOamTlvTypeNone)
        {
            continue;
        }

        sat_header->pkt_data->len = offset;     

        
        if ((lb_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
            || (lb_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestPrbsWithoutCRC))
        {
            p_config->payload.payload_type = bcmSatPayloadPRBS;
            
        }
        else if ((lb_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestNullWithCRC)
                 || (lb_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestNullWithoutCRC))
        {
            p_config->payload.payload_type = bcmSatPayloadConstant4Bytes;       
            sal_memset(p_config->payload.payload_pattern, 0, BCM_SAT_PAYLOAD_MAX_PATTERN_SIZE);
        }

        
        if ((lb_ptr->tlvs[i].tlv_type == bcmOamTlvTypeData))
        {
            p_config->payload.payload_type = bcmSatPayloadConstant4Bytes;       
            p_config->payload.payload_pattern[0] = (lb_ptr->tlvs[i].four_byte_repeatable_pattern & 0xFF000000) >> 24;
            p_config->payload.payload_pattern[1] = (lb_ptr->tlvs[i].four_byte_repeatable_pattern & 0xFF0000) >> 16;
            p_config->payload.payload_pattern[2] = (lb_ptr->tlvs[i].four_byte_repeatable_pattern & 0xFF00) >> 8;
            p_config->payload.payload_pattern[3] = (lb_ptr->tlvs[i].four_byte_repeatable_pattern & 0xFF);
            p_config->payload.payload_pattern[4] = (lb_ptr->tlvs[i].four_byte_repeatable_pattern & 0xFF000000) >> 24;
            p_config->payload.payload_pattern[5] = (lb_ptr->tlvs[i].four_byte_repeatable_pattern & 0xFF0000) >> 16;
            p_config->payload.payload_pattern[6] = (lb_ptr->tlvs[i].four_byte_repeatable_pattern & 0xFF00) >> 8;
            p_config->payload.payload_pattern[7] = (lb_ptr->tlvs[i].four_byte_repeatable_pattern & 0xFF);
        }
        gtf_info.tlv.tlv_type = lb_ptr->tlvs[i].tlv_type;
        gtf_info.tlv.tlv_length = lb_ptr->tlvs[i].tlv_length;
    }

    
    gtf_info.lb_tlv_num = lb_ptr->num_tlvs;
    gtf_info.int_pri = lb_ptr->int_pri;
    gtf_info.pkt_pri = lb_ptr->pkt_pri;
    gtf_info.inner_pkt_pri = lb_ptr->inner_pkt_pri;
    i = 0;
    
    if (dnx_data_headers.system_headers.system_headers_mode_get(unit) ==
        DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
    {
        SHR_IF_ERR_EXIT(_dnx_db_fes_user_header_sizes_get(unit,
                                                          &user_header_0_size,
                                                          &user_header_1_size,
                                                          &user_header_egress_pmf_offset_0_dummy,
                                                          &user_header_egress_pmf_offset_1_dummy));

        udh_size =
            _SHR_IS_FLAG_SET(endpoint_info.flags2,
                             BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN) ? 0 : (user_header_1_size / 8 +
                                                                                   user_header_0_size / 8);
    }
    else
    {
        SHR_IF_ERR_EXIT(oamp_tst_udh_header_size_get(unit, (uint32 *) &udh_size));
    }
    
    min_size = _SHR_IS_FLAG_SET(endpoint_info.flags2, BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN) ? 62 : 67;
    for (priority = 0; priority < BCM_SAT_GTF_NUM_OF_PRIORITIES; priority++)
    {
        for (i2 = 0; i2 < BCM_SAT_GTF_PACKET_LENGTH_NUM_OF_PATTERNS; i2++)
        {
            
            p_config->packet_edit[priority].packet_length[i2] = offset - (is_test_tlv ? 1 : 0) + lb_ptr->tlvs[i].tlv_length + 1;        
            if (p_config->packet_edit[priority].packet_length[i2] < (min_size + udh_size))
            {
                p_config->packet_edit[priority].packet_length[i2] = min_size + udh_size;        
            }
        }

        for (i2 = 0; i2 < BCM_SAT_GTF_PACKET_MAX_PATTERN_LENGTH; i2++)
        {
            p_config->packet_edit[priority].packet_length_pattern[i2] = gtf_packet_length_pattern[0][i2];
        }

        p_config->packet_edit[priority].pattern_length = 1;
        p_config->packet_edit[priority].number_of_ctfs = 1;
        
        if ((lb_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestPrbsWithCRC)
            || (lb_ptr->tlvs[i].tlv_type == bcmOamTlvTypeTestNullWithCRC))
        {
            
            p_config->offsets.crc_byte_offset = gtf_seq_number_offset + 4;
            p_config->packet_edit[priority].flags =
                BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV | BCM_SAT_CTF_PACKET_INFO_ADD_CRC;
        }
        else
        {
            p_config->packet_edit[priority].flags = BCM_SAT_CTF_PACKET_INFO_ADD_END_TLV;
        }
    }

    if (_SHR_IS_FLAG_SET(endpoint_info.flags, BCM_OAM_ENDPOINT_UP_FACING))
    {
        bcm_vlan_port_t vlan_port;
        vlan_port.vlan_port_id = endpoint_info.gport;
        SHR_IF_ERR_EXIT(bcm_dnx_vlan_port_find(unit, &vlan_port));
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, vlan_port.port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
        
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, gport_info.local_port, &core_id, &tm_port));
        p_config->packet_context_id = core_id;
    }
    SHR_IF_ERR_EXIT(bcm_dnx_sat_gtf_packet_config(unit, gtf_id, p_config));

    
    rv = _dnx_oam_sat_gtf_info_set(unit, gtf_id, &gtf_info);
    SHR_IF_ERR_EXIT(rv);
exit:
    bcm_pkt_free(unit, sat_header);
    SHR_FREE(p_config);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}


shr_error_e
bcm_dnx_oam_loopback_add(
    int unit,
    bcm_oam_loopback_t * loopback_ptr)
{
    int rv = BCM_E_NONE;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    
    rv = _dnx_sat_loopback_add_rx(unit, loopback_ptr);
    SHR_IF_ERR_EXIT(rv);

    
    rv = _dnx_sat_loopback_add_tx(unit, loopback_ptr);
    SHR_IF_ERR_EXIT(rv);
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}


shr_error_e
bcm_dnx_oam_loopback_get(
    int unit,
    bcm_oam_loopback_t * lb_ptr)
{
    int found = 0;
    int rv = BCM_E_NONE;
    DNX_OAM_SAT_CTF_ENTRY ctf_info;
    DNX_OAM_SAT_GTF_ENTRY gtf_info;
    bcm_sat_ctf_stat_t stat;
    bcm_sat_gtf_stat_multi_t gtf_tx_count;
    int index = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(lb_ptr, _SHR_E_PARAM, "lb_ptr Is NULL\n");

    
    for (index = 0; index < dnx_data_sat.collector.nof_ctf_ids_get(unit); index++)
    {
        SHR_IF_ERR_EXIT(_dnx_oam_sat_ctf_info_get(unit, index, &ctf_info, &found));
        if (found && (ctf_info.flag == DNX_PP_OAM_LOOPBACK_TST_INFO_FLAGS_LBM) && (ctf_info.mepid == lb_ptr->id))
        {
            
            break;
        }
    }
    if (index == dnx_data_sat.collector.nof_ctf_ids_get(unit))
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No endpoint with id %d found.", lb_ptr->id);
    }
    lb_ptr->ctf_id = index;
    
    bcm_sat_ctf_stat_t_init(&stat);
    rv = bcm_dnx_sat_ctf_stat_get(unit, lb_ptr->ctf_id, 0, &stat);
    SHR_IF_ERR_EXIT(rv);

    COMPILER_64_TO_32_LO(lb_ptr->rx_count, stat.received_packet_cnt);
    COMPILER_64_TO_32_LO(lb_ptr->invalid_tlv_count, stat.err_packet_cnt);
    COMPILER_64_TO_32_LO(lb_ptr->out_of_sequence, stat.out_of_order_packet_cnt);

    
    for (index = 0; index < dnx_data_sat.generator.nof_gtf_ids_get(unit); index++)
    {
        SHR_IF_ERR_EXIT(_dnx_oam_sat_gtf_info_get(unit, index, &gtf_info, &found));
        if (found && (gtf_info.flag == DNX_PP_OAM_LOOPBACK_TST_INFO_FLAGS_LBM) && (gtf_info.mepid == lb_ptr->id))
        {
            
            break;
        }
    }
    if (index == dnx_data_sat.generator.nof_gtf_ids_get(unit))
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "No endpoint with id %d found.", lb_ptr->id);
    }
    lb_ptr->gtf_id = index;
    rv = bcm_dnx_sat_gtf_stat_multi_get(unit, index, &gtf_tx_count);
    SHR_IF_ERR_EXIT(rv);
    COMPILER_64_TO_32_LO(lb_ptr->tx_count, gtf_tx_count.transmitted_packet_cnt);
    lb_ptr->drop_count = lb_ptr->tx_count - lb_ptr->rx_count;
    if (lb_ptr->num_tlvs == 1)
    {
        lb_ptr->tlvs[0].tlv_length = gtf_info.tlv.tlv_length;
        lb_ptr->tlvs[0].tlv_type = gtf_info.tlv.tlv_type;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
bcm_dnx_oam_loopback_delete(
    int unit,
    bcm_oam_loopback_t * lb_ptr)
{
    int found = 0;
    int rv = BCM_E_NONE;
    int trap_id = 0;
    int trap_data = 0;
    int lb_trap_id = 0;
    bcm_sat_ctf_t ctf_id;
    bcm_sat_gtf_t gtf_id;
    DNX_OAM_SAT_CTF_ENTRY ctf_info;
    DNX_OAM_SAT_GTF_ENTRY gtf_info;
    bcm_sat_ctf_identifier_t identifier;
    bcm_oam_endpoint_info_t endpoint_info;
    int tc = 0;
    int dp = 0;
    int tc_max = 7;
    int dp_max = 3;
    uint32 tst_trap_used_cnt = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(lb_ptr, _SHR_E_PARAM, "lb_ptr Is NULL\n");

    ctf_id = lb_ptr->ctf_id;
    rv = _dnx_oam_sat_ctf_info_get(unit, lb_ptr->ctf_id, &ctf_info, &found);
    if (!found)
    {
        SHR_ERR_EXIT(BCM_E_NOT_FOUND, "ctf_id %d not found.", ctf_id);
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }
    gtf_id = lb_ptr->gtf_id;
    rv = _dnx_oam_sat_gtf_info_get(unit, lb_ptr->gtf_id, &gtf_info, &found);
    if (!found)
    {
        SHR_ERR_EXIT(BCM_E_NOT_FOUND, "gtf_id %d not found.", ctf_id);
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

    SHR_IF_ERR_EXIT(bcm_dnx_oam_endpoint_get(unit, lb_ptr->id, &endpoint_info));
    
    if (!DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(&endpoint_info))
    {
        SHR_ERR_EXIT(BCM_E_PARAM, "Endpoint %d is not accelerated.", lb_ptr->id);
    }

    
    SHR_IF_ERR_EXIT(bcm_dnx_rx_trap_type_get(unit, 0, bcmRxTrapEgTxOamUpMEPDest2, &lb_trap_id));
    trap_id = DNX_RX_TRAP_ID_TYPE_GET(lb_trap_id);

    ctf_info.session_map_flag--;
    if (ctf_info.session_map_flag == 0) 
    {
        trap_data = lb_ptr->id;
        SHR_IF_ERR_EXIT(bcm_dnx_sat_ctf_trap_data_to_session_unmap(unit, trap_data, 0));
    }

    
    identifier.session_id = ctf_info.identifier_session_id;     
    identifier.trap_id = ctf_info.identifier_trap_id;

    
    for (dp = 0; dp <= dp_max; dp++)
    {
        for (tc = 0; tc <= tc_max; tc++)
        {
            identifier.color = dp;
            identifier.tc = tc;
            rv = bcm_dnx_sat_ctf_identifier_unmap(unit, &identifier);
            SHR_IF_ERR_EXIT(rv);
        }
    }

    
    rv = bcm_dnx_sat_ctf_destroy(unit, ctf_id);
    SHR_IF_ERR_EXIT(rv);
    
    rv = bcm_dnx_sat_gtf_destroy(unit, lb_ptr->gtf_id);
    SHR_IF_ERR_EXIT(rv);

    SHR_IF_ERR_EXIT(oam_sw_db_info.tst_trap_used_cnt.get(unit, &tst_trap_used_cnt));

    if ((tst_trap_used_cnt & DNX_PP_OAM_LOOPBACK_TST_TRAP_CNT_MASK))
    {
        tst_trap_used_cnt--;
    }
    
    if ((tst_trap_used_cnt & DNX_PP_OAM_LOOPBACK_TST_TRAP_CNT_MASK) == 0)
    {
        rv = bcm_dnx_sat_ctf_trap_remove(unit, trap_id);
        tst_trap_used_cnt = 0;
        SHR_IF_ERR_EXIT(rv);
    }
    SHR_IF_ERR_EXIT(oam_sw_db_info.tst_trap_used_cnt.set(unit, tst_trap_used_cnt));

    
    rv = _dnx_oam_sat_ctf_info_del(unit, ctf_id);
    rv = _dnx_oam_sat_gtf_info_del(unit, gtf_id);

    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}
