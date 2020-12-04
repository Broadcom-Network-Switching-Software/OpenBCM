/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *  
 *   File:    tcb.c
 *   Purpose: Handle HX5 specific tcb APIs
 */

#include <bcm/cosq.h>

#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/trident3.h>
#include <bcm_int/esw/helix5.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/stack.h>
#include <soc/helix5.h>

#if defined (INCLUDE_TCB) && defined (BCM_TCB_SUPPORT)
#include <bcm_int/esw/tcb.h>

#define _BCM_HX5_TCB_MAX_NUM    1
#define _BCM_HX5_TCB_PROFILE_MAX_NUM    8
#define _BCM_HX5_TCB_THRESHOLD_MAX      0x3ffff

#define _HX5_NUM_PROFILE_COS 26

#define _HX5_TCB_UCQ_SCOPE_QUEUE_MASK  0x7ff

#define _HX5_IS_ST_PORT(unit, mmu_port) \
    ((mmu_port < 16) ? 1 : 0)

#define _MMU_CELLS_TO_BYTES(cells, cell_bytes)  cells * cell_bytes
#define _MMU_BYTES_TO_CELLS(bytes, cell_bytes)  (bytes + (cell_bytes - 1)) \
                                                / cell_bytes


STATIC soc_field_t _bcm_hx5_cosq_tcb_threshold_profile_map_fields[] = {
    QUEUE_0f, QUEUE_1f, QUEUE_2f, QUEUE_3f, QUEUE_4f, QUEUE_5f, QUEUE_6f, QUEUE_7f, 
    QUEUE_8f, QUEUE_9f, QUEUE_10f, QUEUE_11f, QUEUE_12f, QUEUE_13f, QUEUE_14f, QUEUE_15f, 
    QUEUE_16f, QUEUE_17f, QUEUE_18f, QUEUE_19f, QUEUE_20f, QUEUE_21f, QUEUE_22f, QUEUE_23f, 
    QUEUE_24f, QUEUE_25f 
};


STATIC int
_bcm_hx5_cosq_tcb_intr_enable_set(int unit, int enable)
{
    uint32 rval;

    if (SOC_REG_IS_VALID(unit, MMU_XCFG_XPE_CPU_INT_ENr)) {
        BCM_IF_ERROR_RETURN(READ_MMU_XCFG_XPE_CPU_INT_ENr(unit, &rval));
        soc_reg_field_set(unit, MMU_XCFG_XPE_CPU_INT_ENr, &rval,
                          TCB_FREEZE_INT_ENf, enable);
        BCM_IF_ERROR_RETURN(WRITE_MMU_XCFG_XPE_CPU_INT_ENr(unit, rval));
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_hx5_cosq_tcb_intr_status_reset(int unit)
{
    uint32 rval;

    BCM_IF_ERROR_RETURN(READ_MMU_XCFG_XPE_CPU_INT_CLEARr(unit, &rval));
    soc_reg_field_set(unit, MMU_XCFG_XPE_CPU_INT_CLEARr, &rval,
                      TCB_FREEZE_INT_CLRf, 0);
    BCM_IF_ERROR_RETURN(WRITE_MMU_XCFG_XPE_CPU_INT_CLEARr(unit, rval));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_hx5_cosq_tcb_current_phase_get
 * Purpose:
 *      Get tcb buffer running status.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) TCB buffer id.
 *      status - (OUT) TCB buffer status.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_hx5_cosq_tcb_current_phase_get(int unit, bcm_cosq_buffer_id_t buffer_id, int *status)
{
    uint32 rval;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_current_phase_get: unit %d, buffer_id 0\n"),
                         unit));

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, TCB_CURRENT_PHASEr, buffer_id, 0, &rval));
    *status = soc_reg_field_get(unit,TCB_CURRENT_PHASEr, rval, CURRENT_PHASEf);

    return BCM_E_NONE;
}

STATIC int
_bcm_hx5_cosq_tcb_hw_event_cb(int unit)
{
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    bcm_cosq_buffer_id_t buffer_id;
    int status;

    tcb_unit_info = TCB_UNIT_CONTROL(unit);
    if (!tcb_unit_info) {
        return 0;
    }

    _bcm_hx5_cosq_tcb_intr_enable_set(unit, 0);
    _bcm_hx5_cosq_tcb_intr_status_reset(unit);

    for (buffer_id = 0; buffer_id < _BCM_HX5_TCB_MAX_NUM; buffer_id++) {

        /* Go over the enabled TCB to see which is freezed */
        if(tcb_unit_info->tcb_enabled_bmp & (1 << buffer_id)) {
            BCM_IF_ERROR_RETURN(
                    _bcm_hx5_cosq_tcb_current_phase_get(unit, buffer_id, &status));
            if (_TCB_FROZEN_PHASE == status) {
                if (tcb_unit_info->tcb_evt_cb) {
                    tcb_unit_info->tcb_evt_cb(unit, buffer_id,
                            tcb_unit_info->tcb_evt_user_data);
                }
            }
        }
    }

    _bcm_hx5_cosq_tcb_intr_enable_set(unit, 1);
    return BCM_E_NONE;
}

int
soc_hx5_tcb_set_hw_intr_cb(int unit)
{
    BCM_IF_ERROR_RETURN(
            soc_td3_set_tcb_callback(unit, &_bcm_hx5_cosq_tcb_hw_event_cb));
    return SOC_E_NONE;
}

STATIC int
_bcm_hx5_cosq_tcb_buffer_ready_clear(int unit, bcm_cosq_buffer_id_t buffer_id)
{
    uint32 rval;
    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, TCB_BUFFER_READYr, buffer_id, 0, &rval));
    soc_reg_field_set(unit, TCB_BUFFER_READYr, &rval, BUFFER_READYf, 0);
    BCM_IF_ERROR_RETURN(soc_reg32_set(unit, TCB_BUFFER_READYr, buffer_id, 0, rval));

    return BCM_E_NONE;
}

STATIC int
_bcm_hx5_cosq_tcb_is_enabled(int unit, bcm_cosq_buffer_id_t buffer_id)
{
     _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
     int enabled;

     tcb_unit_info = TCB_UNIT_CONTROL(unit);
     enabled = tcb_unit_info->tcb_enabled_bmp & (1 << buffer_id);
     if (0 == enabled) {
         return FALSE;
     } else {
         return TRUE;
     }
}

STATIC int
_bcm_hx5_cosq_tcb_profile_id_alloc(int unit,
                                    bcm_cosq_buffer_id_t buffer_id, int *index)
{
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    int i, profile_count;
    SHR_BITDCL *bitmap;
    _bcm_cosq_tcb_info_t *tcb_info;

    tcb_unit_info = TCB_UNIT_CONTROL(unit);

    tcb_info = tcb_unit_info->tcb_info[buffer_id];
    bitmap = tcb_info->profile_bmp;
    profile_count = _BCM_HX5_TCB_PROFILE_MAX_NUM;

    for(i = 0; i < profile_count; i++) {
        if(!SHR_BITGET(bitmap, i)) {
            SHR_BITSET(bitmap, i);
            *index = i;
            return BCM_E_NONE;
        }
    }

    LOG_INFO(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                               "No free profile in TCB %d\n"),
                               buffer_id));

    return BCM_E_RESOURCE;
}

STATIC int
_bcm_hx5_cosq_tcb_ext_freeze_set(int unit,
                                 bcm_cosq_buffer_id_t buffer_id, int enable)
{
    uint32 rval;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_ext_freeze_set: unit %d, buffer_id %d, enable %d\n"),
                         unit, buffer_id, enable));

    BCM_IF_ERROR_RETURN(READ_MMU_GCFG_TCB_FREEZEr(unit, &rval));

    if(enable) {
        soc_reg_field_set(unit,MMU_GCFG_TCB_FREEZEr, &rval, TCB_FREEZE_XPEf, 1);
    } else {
        soc_reg_field_set(unit,MMU_GCFG_TCB_FREEZEr, &rval, TCB_FREEZE_XPEf, 0);
    }

    BCM_IF_ERROR_RETURN(WRITE_MMU_GCFG_TCB_FREEZEr(unit, rval));

    return BCM_E_NONE;
}

STATIC int
_bcm_hx5_cosq_tcb_evt_overflow_count_get(int unit,
        bcm_cosq_buffer_id_t buffer_id,
        int *num)
{
    uint32 rval;

    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, TCB_EVENT_BUFFER_OVERFLOW_COUNTERr,
                buffer_id, 0, &rval));
    *num = soc_reg_field_get(unit, TCB_EVENT_BUFFER_OVERFLOW_COUNTERr,
            rval, OVERFLOW_COUNTERf);

    return BCM_E_NONE;
}

STATIC int
_bcm_hx5_cosq_tcb_buffer_info_get(int unit,
                                  bcm_cosq_buffer_id_t buffer_id,
                                  int type, int *arg)
{
    uint32 rval;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_buffer_info_get: unit %d, buffer_id %d, type %d\n"),
                         unit, buffer_id, type));

    BCM_IF_ERROR_RETURN(soc_reg32_get(unit, TCB_BUFFER_STATUSr, buffer_id, 0, &rval));

    switch(type) {
        case _TCB_BUF_STATUS_WRAPPED:
            *arg = soc_reg_field_get(unit, TCB_BUFFER_STATUSr, rval,
                                     TCB_BUFFER_WRAP_AROUND_FLAGf);
            break;
        case _TCB_BUF_STATUS_PTR:
            *arg = soc_reg_field_get(unit, TCB_BUFFER_STATUSr, rval,
                                     TCB_BUFFER_END_PTRf);
            break;
        case _TCB_BUF_STATUS_EVT_NUM:
            *arg = soc_reg_field_get(unit, TCB_BUFFER_STATUSr, rval,
                                     EVENT_BUFFER_SIZEf);
            break;
        case _TCB_BUF_STATUS_0_USED:
            *arg = soc_reg_field_get(unit, TCB_BUFFER_STATUSr, rval,
                                     TCB_BUFFER_ADDRESS_0_USEDf);
            break;
        default:
            break;
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_hx5_cosq_tcb_buffer_get_rd_num(int unit,
                                    bcm_cosq_buffer_id_t buffer_id, int *num)
{
    int wrap_flag, end_idx, add_0_used;

    BCM_IF_ERROR_RETURN(_bcm_hx5_cosq_tcb_buffer_info_get(unit, buffer_id,
                            _TCB_BUF_STATUS_WRAPPED, &wrap_flag));
    BCM_IF_ERROR_RETURN(_bcm_hx5_cosq_tcb_buffer_info_get(unit, buffer_id,
                            _TCB_BUF_STATUS_PTR, &end_idx));
    BCM_IF_ERROR_RETURN(_bcm_hx5_cosq_tcb_buffer_info_get(unit, buffer_id,
                            _TCB_BUF_STATUS_0_USED, &add_0_used));

    if (0 == add_0_used) {
        *num = 0;
    } else if (0 == wrap_flag) {
        *num = end_idx + 1;
    } else {
        *num = 1024;
    }

    return BCM_E_NONE;
}

STATIC void
_bcm_hx5_cosq_tcb_pkt_order_fix(uint8 *pkt_data)
{
    uint32 temp32;
    int i;

    for (i = 36; i >= 20; i -= 4) {
        temp32 = *(uint32*)(&pkt_data[i]);
        *(uint32*)(&pkt_data[i]) = soc_ntohl(*(uint32*)(&pkt_data[36 - i]));
        *(uint32*)(&pkt_data[36 - i]) = soc_ntohl(temp32);
    }

    return;
}

int
bcm_hx5_cosq_tcb_uc_queue_resolve(int unit, bcm_cosq_buffer_id_t buffer_id, int queue,
                                  bcm_gport_t *uc_gport)
{
    int id;
    bcm_port_t mmu_port, local_port, phy_port;
    soc_info_t *si;

    si = &SOC_INFO(unit);
    if (queue < SOC_HX5_DOWNLINK_PORTQ_BASE) {
        mmu_port = queue / 26;
    } else {
        mmu_port = ((queue - SOC_HX5_DOWNLINK_PORTQ_BASE) / 16) + 16; 
    }
    phy_port = si->port_m2p_mapping[mmu_port];
    local_port = si->port_p2l_mapping[phy_port];
    id = queue - si->port_uc_cosq_base[local_port];
    BCM_GPORT_UCAST_QUEUE_GROUP_SYSQID_SET(*uc_gport, local_port, id);
    return BCM_E_NONE;
}

STATIC int
_bcm_hx5_cosq_tcb_celldata_read(int unit, bcm_cosq_buffer_id_t buffer_id,
                                int num, bcm_cosq_tcb_buffer_t *record_entries)
{
    uint32 mem_wid;
    soc_mem_t mem;
    mmu_tcb_buffer_cell_data_upper_entry_t *pud;
    mmu_tcb_buffer_cell_data_lower_entry_t *pld;
    uint8 *tcb_md_buf = NULL;
    int i, j = 0, idx, wrapped;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_celldata_read: unit %d, buffer_id %d, num %d\n"),
                         unit, buffer_id, num));

    BCM_IF_ERROR_RETURN(_bcm_hx5_cosq_tcb_buffer_info_get(unit, buffer_id,
                                                     _TCB_BUF_STATUS_PTR, &idx));
    BCM_IF_ERROR_RETURN(_bcm_hx5_cosq_tcb_buffer_info_get(unit, buffer_id,
                                                     _TCB_BUF_STATUS_WRAPPED,
                                                     &wrapped));

    mem_wid = sizeof(uint32) * soc_mem_entry_words(unit,
                                               MMU_TCB_BUFFER_CELL_DATA_UPPERm);
    mem = SOC_MEM_UNIQUE_ACC(unit, MMU_TCB_BUFFER_CELL_DATA_UPPERm)[buffer_id];

    if (wrapped) {
        tcb_md_buf = soc_cm_salloc(unit, mem_wid * _BCM_TCB_RD_MAX_NUM,
                                   "tcb cell data upper");
        if (tcb_md_buf == NULL) {
            return BCM_E_MEMORY;
        }

        SOC_IF_ERROR_RETURN(soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, 0,
                                               _BCM_TCB_RD_MAX_NUM - 1, tcb_md_buf));

        for (i = idx + 1, j = 0; j < num; i++, j++) {
            pud = soc_mem_table_idx_to_pointer(unit, MMU_TCB_BUFFER_CELL_DATA_UPPERm,
                        mmu_tcb_buffer_cell_data_upper_entry_t *, tcb_md_buf,
                        i % _BCM_TCB_RD_MAX_NUM);
            soc_MMU_TCB_BUFFER_CELL_DATA_UPPERm_field_get(unit, (uint32*)pud,
                        CBIDATAf, (uint32*)(record_entries[j].buf_data));
            _bcm_hx5_cosq_tcb_pkt_order_fix(record_entries[j].buf_data);
        }
    } else {
        tcb_md_buf = soc_cm_salloc(unit, mem_wid * num, "tcb cell data upper");
        if (tcb_md_buf == NULL) {
            return BCM_E_MEMORY;
        }

        SOC_IF_ERROR_RETURN(soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, 0, num - 1,
                                               tcb_md_buf));
        for(i = 0; i < num; i++) {
            pud = soc_mem_table_idx_to_pointer(unit, MMU_TCB_BUFFER_CELL_DATA_UPPERm,
                    mmu_tcb_buffer_cell_data_upper_entry_t *, tcb_md_buf, i);
            soc_MMU_TCB_BUFFER_CELL_DATA_UPPERm_field_get(unit, pud,
                        CBIDATAf, (uint32*)(record_entries[i].buf_data));
            _bcm_hx5_cosq_tcb_pkt_order_fix(record_entries[i].buf_data);
        }
    }

    soc_cm_sfree(unit, tcb_md_buf);

    mem_wid = sizeof(uint32) * soc_mem_entry_words(unit,
                                               MMU_TCB_BUFFER_CELL_DATA_LOWERm);
    mem = SOC_MEM_UNIQUE_ACC(unit, MMU_TCB_BUFFER_CELL_DATA_LOWERm)[buffer_id];

    if (wrapped) {
        tcb_md_buf = soc_cm_salloc(unit, mem_wid * _BCM_TCB_RD_MAX_NUM,
                                   "tcb metadata");
        if (tcb_md_buf == NULL) {
            return BCM_E_MEMORY;
        }

        SOC_IF_ERROR_RETURN(soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, 0,
                                               _BCM_TCB_RD_MAX_NUM - 1, tcb_md_buf));

        for (i = idx + 1, j = 0; j < num; i++, j++) {
            pld = soc_mem_table_idx_to_pointer(unit, MMU_TCB_BUFFER_CELL_DATA_LOWERm,
                        mmu_tcb_buffer_cell_data_lower_entry_t *, tcb_md_buf,
                        i % _BCM_TCB_RD_MAX_NUM);
            soc_MMU_TCB_BUFFER_CELL_DATA_LOWERm_field_get(unit, pld,
                        CBIDATAf, (uint32*)(&(record_entries[j].buf_data[40])));
            _bcm_hx5_cosq_tcb_pkt_order_fix((uint8*)(&(record_entries[j].buf_data[40])));
        }
    } else {
        tcb_md_buf = soc_cm_salloc(unit, mem_wid * num, "tcb cell data lower");
        if (tcb_md_buf == NULL) {
            return BCM_E_MEMORY;
        }

        SOC_IF_ERROR_RETURN(soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, 0, num - 1,
                                               tcb_md_buf));
        for(i = 0; i < num; i++) {
            pld = soc_mem_table_idx_to_pointer(unit, MMU_TCB_BUFFER_CELL_DATA_LOWERm,
                    mmu_tcb_buffer_cell_data_lower_entry_t *, tcb_md_buf, i);

            soc_MMU_TCB_BUFFER_CELL_DATA_LOWERm_field_get(unit, pld,
                        CBIDATAf, (uint32*)(&(record_entries[i].buf_data[40])));
            _bcm_hx5_cosq_tcb_pkt_order_fix((uint8*)(&(record_entries[i].buf_data[40])));
        }
    }

    soc_cm_sfree(unit, tcb_md_buf);
    return BCM_E_NONE;
}

STATIC int
_bcm_hx5_cosq_tcb_modport_set(int unit, int gpp, bcm_gport_t *modport)
{
    bcm_module_t mod_in, mod_out;
    bcm_port_t port_in, port_out;

    port_in = gpp & SOC_MEM_FIF_DGPP_PORT_MASK;
    mod_in = (gpp & SOC_MEM_FIF_DGPP_MOD_ID_MASK) >>
             SOC_MEM_FIF_DGPP_MOD_ID_SHIFT_BITS;
    BCM_IF_ERROR_RETURN
        (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                                 mod_in, port_in, &mod_out, &port_out));
    BCM_GPORT_MODPORT_SET(*modport, mod_out, port_out);

    return BCM_E_NONE;
}


STATIC int
_bcm_hx5_cosq_tcb_metadata_read(int unit, bcm_cosq_buffer_id_t buffer_id,
                                int num, bcm_cosq_tcb_buffer_t *record_entries)
{
    uint32 mem_wid;
    soc_mem_t mem;
    tcb_buffer_metadata_entry_t *pmd;
    uint8 *tcb_md_buf = NULL;
    int i, j, idx, wrapped;
    int dvp, dgpp, sgpp, src_port, dst_port, qnum, depth, cell_bytes;
    bcm_port_t mmu_port, local_port, phy_port = 0;
    soc_info_t *si;
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    bcm_gport_t ingress_gport, egress_gport, dvp_gport;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_metadata_read: unit %d, buffer_id %d, num %d\n"),
                         unit, buffer_id, num));

    si = &SOC_INFO(unit);
    tcb_unit_info = TCB_UNIT_CONTROL(unit);
    cell_bytes = tcb_unit_info->cell_bytes;

    BCM_IF_ERROR_RETURN(_bcm_hx5_cosq_tcb_buffer_info_get(unit, buffer_id,
                                                     _TCB_BUF_STATUS_PTR, &idx));
    BCM_IF_ERROR_RETURN(_bcm_hx5_cosq_tcb_buffer_info_get(unit, buffer_id,
                                                     _TCB_BUF_STATUS_WRAPPED,
                                                     &wrapped));

    mem_wid = sizeof(uint32) * soc_mem_entry_words(unit, TCB_BUFFER_METADATAm);
    mem = SOC_MEM_UNIQUE_ACC(unit, TCB_BUFFER_METADATAm)[buffer_id];

    if (wrapped) {
        tcb_md_buf = soc_cm_salloc(unit, mem_wid * _BCM_TCB_RD_MAX_NUM,
                                   "tcb metadata");
        if (tcb_md_buf == NULL) {
            return BCM_E_MEMORY;
        }

        SOC_IF_ERROR_RETURN(soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, 0,
                                _BCM_TCB_RD_MAX_NUM - 1, tcb_md_buf));

        for (i = idx + 1, j = 0; j < num; i++, j++) {
            pmd = soc_mem_table_idx_to_pointer(unit, TCB_BUFFER_METADATAm,
                        tcb_buffer_metadata_entry_t *, tcb_md_buf,
                        i % _BCM_TCB_RD_MAX_NUM);

            soc_TCB_BUFFER_METADATAm_field_get(unit, pmd, TIMESTAMPf,
                                       (uint32*)(&(record_entries[j].timestamp)));

            dvp = soc_TCB_EVENT_BUFFERm_field32_get(unit, pmd, DVPf);
            _bcm_vp_encode_gport(unit, dvp, &dvp_gport);

            dgpp = soc_TCB_BUFFER_METADATAm_field32_get(unit, pmd, DGPPf);
            _bcm_hx5_cosq_tcb_modport_set(unit, dgpp, &egress_gport);

            sgpp = soc_TCB_BUFFER_METADATAm_field32_get(unit, pmd, SGPP_SVPf);
            if (sgpp & _TCB_SGPP_TYPE_MASK) {
                sgpp &= _TCB_SGPP_MASK;
                _bcm_hx5_cosq_tcb_modport_set(unit, sgpp, &ingress_gport);
            } else {
                sgpp &= _TCB_SGPP_MASK;
                _bcm_vp_encode_gport(unit, sgpp, &ingress_gport);
            }

            src_port = soc_TCB_BUFFER_METADATAm_field32_get(unit, pmd, SRC_PORTf);

            mmu_port = src_port;
            phy_port = si->port_m2p_mapping[mmu_port];
            local_port = si->port_p2l_mapping[phy_port];
            BCM_GPORT_LOCAL_SET(record_entries[j].ingress_gport, local_port);

            qnum = soc_TCB_BUFFER_METADATAm_field32_get(unit, pmd, QNUMf);
            BCM_IF_ERROR_RETURN
                (_bcm_cosq_tcb_uc_queue_resolve(unit, buffer_id, qnum,
                                                &record_entries[j].queue));

            dst_port = BCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(record_entries[j].queue);
            BCM_GPORT_LOCAL_SET(record_entries[j].egress_gport, dst_port);

            depth = soc_TCB_BUFFER_METADATAm_field32_get(unit, pmd, Q_DEPTHf);
            record_entries[j].queue_size = _MMU_CELLS_TO_BYTES(depth, cell_bytes);

            depth = soc_TCB_BUFFER_METADATAm_field32_get(unit, pmd, SRVPOOL_DEPTHf);
            record_entries[j].service_pool_size = _MMU_CELLS_TO_BYTES(depth, cell_bytes);
        }
    } else {
        tcb_md_buf = soc_cm_salloc(unit, mem_wid * num, "tcb metadata");
        if (tcb_md_buf == NULL) {
            return BCM_E_MEMORY;
        }

        SOC_IF_ERROR_RETURN(soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, 0,
                                               num - 1, tcb_md_buf));
        for(i = 0; i < num; i++) {
            pmd = soc_mem_table_idx_to_pointer(unit, TCB_BUFFER_METADATAm,
                    tcb_buffer_metadata_entry_t *, tcb_md_buf, i);

            soc_TCB_BUFFER_METADATAm_field_get(unit, pmd, TIMESTAMPf,
                                       (uint32*)(&(record_entries[i].timestamp)));

            dvp = soc_TCB_EVENT_BUFFERm_field32_get(unit, pmd, DVPf);
            _bcm_vp_encode_gport(unit, dvp, &dvp_gport);

            dgpp = soc_TCB_BUFFER_METADATAm_field32_get(unit, pmd, DGPPf);
            _bcm_hx5_cosq_tcb_modport_set(unit, dgpp, &egress_gport);

            sgpp = soc_TCB_BUFFER_METADATAm_field32_get(unit, pmd, SGPP_SVPf);
            if (sgpp & _TCB_SGPP_TYPE_MASK) {
                sgpp &= _TCB_SGPP_MASK;
                _bcm_hx5_cosq_tcb_modport_set(unit, sgpp, &ingress_gport);
            } else {
                sgpp &= _TCB_SGPP_MASK;
                _bcm_vp_encode_gport(unit, sgpp, &ingress_gport);
            }


            src_port = soc_TCB_BUFFER_METADATAm_field32_get(unit, pmd, SRC_PORTf);
            mmu_port = src_port;

            phy_port = si->port_m2p_mapping[mmu_port];
            local_port = si->port_p2l_mapping[phy_port];
            BCM_GPORT_LOCAL_SET(record_entries[i].ingress_gport, local_port);
            qnum = soc_TCB_BUFFER_METADATAm_field32_get(unit, pmd, QNUMf);
            BCM_IF_ERROR_RETURN
                (_bcm_cosq_tcb_uc_queue_resolve(unit, buffer_id, qnum,
                                                &record_entries[i].queue));

            dst_port = BCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(record_entries[i].queue);
            BCM_GPORT_LOCAL_SET(record_entries[i].egress_gport, dst_port);

            depth = soc_TCB_BUFFER_METADATAm_field32_get(unit, pmd, Q_DEPTHf);
            record_entries[i].queue_size = _MMU_CELLS_TO_BYTES(depth, cell_bytes);

            depth = soc_TCB_BUFFER_METADATAm_field32_get(unit, pmd, SRVPOOL_DEPTHf);
            record_entries[i].service_pool_size = _MMU_CELLS_TO_BYTES(depth, cell_bytes);
        }
    }

    soc_cm_sfree(unit, tcb_md_buf);
    return BCM_E_NONE;
}

STATIC int
_bcm_hx5_cosq_tcb_event_buffer_read(int unit, bcm_cosq_buffer_id_t buffer_id,
                                 int num, bcm_cosq_tcb_event_t *record_entries)
{
    uint32 mem_wid;
    tcb_event_buffer_entry_t *ped;
    uint8 *tcb_evt_buf = NULL;
    soc_mem_t mem;
    int idx;
    int dvp, dgpp, sgpp, src_port, dst_port, qnum;
    int drop_reason = 0, drop_vector, depth, cell_bytes;
    bcm_port_t mmu_port, local_port, phy_port;
    soc_info_t *si;
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    bcm_gport_t ingress_gport, egress_gport, dvp_gport;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_event_buffer_read: unit %d, buffer_id %d, num %d\n"),
                         unit, buffer_id, num));

    si = &SOC_INFO(unit);
    tcb_unit_info = TCB_UNIT_CONTROL(unit);
    cell_bytes = tcb_unit_info->cell_bytes;

    mem_wid = sizeof(uint32) * soc_mem_entry_words(unit, TCB_EVENT_BUFFERm);

    tcb_evt_buf = soc_cm_salloc(unit, num * mem_wid, "tcb event");

    if (tcb_evt_buf == NULL) {
        return BCM_E_MEMORY;
    }

    mem = SOC_MEM_UNIQUE_ACC(unit, TCB_EVENT_BUFFERm)[buffer_id];
    SOC_IF_ERROR_RETURN(soc_mem_read_range(unit, mem, MEM_BLOCK_ANY, 0, num - 1,
                                           tcb_evt_buf));

    for (idx = 0; idx < num; idx++) {
        ped = soc_mem_table_idx_to_pointer
                    (unit, TCB_EVENT_BUFFERm, tcb_event_buffer_entry_t *,
                     tcb_evt_buf, idx);

        soc_TCB_EVENT_BUFFERm_field_get(unit, ped, TIMESTAMPf,
                                       (uint32*)(&(record_entries[idx].timestamp)));

        dvp = soc_TCB_EVENT_BUFFERm_field32_get(unit, ped, DVPf);
        _bcm_vp_encode_gport(unit, dvp, &dvp_gport);

        dgpp = soc_TCB_EVENT_BUFFERm_field32_get(unit, ped, DGPPf);
        _bcm_hx5_cosq_tcb_modport_set(unit, dgpp, &egress_gport);

        sgpp = soc_TCB_EVENT_BUFFERm_field32_get(unit, ped, SGPP_SVPf);
        if (sgpp & _TCB_SGPP_TYPE_MASK) {
            sgpp &= _TCB_SGPP_MASK;
            _bcm_hx5_cosq_tcb_modport_set(unit, sgpp, &ingress_gport);
        } else {
            sgpp &= _TCB_SGPP_MASK;
            _bcm_vp_encode_gport(unit, sgpp, &ingress_gport);
        }

        src_port = soc_TCB_EVENT_BUFFERm_field32_get(unit, ped, SRC_PORTf);
        mmu_port = src_port;
        phy_port = si->port_m2p_mapping[mmu_port];
        local_port = si->port_p2l_mapping[phy_port];
        BCM_GPORT_LOCAL_SET(record_entries[idx].ingress_gport, local_port);

        qnum = soc_TCB_EVENT_BUFFERm_field32_get(unit, ped, QNUMf);
        BCM_IF_ERROR_RETURN
            (_bcm_cosq_tcb_uc_queue_resolve(unit, buffer_id, qnum,
                                            &record_entries[idx].queue));

        dst_port = BCM_GPORT_UCAST_QUEUE_GROUP_SYSPORTID_GET(record_entries[idx].queue);
        BCM_GPORT_LOCAL_SET(record_entries[idx].egress_gport, dst_port);

        depth = soc_TCB_EVENT_BUFFERm_field32_get(unit, ped, Q_DEPTHf);
        record_entries[idx].queue_size = _MMU_CELLS_TO_BYTES(depth, cell_bytes);

        depth = soc_TCB_EVENT_BUFFERm_field32_get(unit, ped, SRVPOOL_DEPTHf);
        record_entries[idx].service_pool_size = _MMU_CELLS_TO_BYTES(depth, cell_bytes);

        record_entries[idx].color =
                soc_TCB_EVENT_BUFFERm_field32_get(unit, ped, CNGf);

        drop_vector = soc_TCB_EVENT_BUFFERm_field32_get(unit, ped,
                                                        DROP_RSN_VECTORf);
        if (drop_vector & _TCB_EVENT_DROP_REASON_IADMN_MASK ||
            drop_vector & _TCB_EVENT_DROP_REASON_PURGE_MASK) {
            drop_reason = 1 << _SHR_TCB_TRACK_IADMN_DROP_SHIFT;
        }
        if (drop_vector & _TCB_EVENT_DROP_REASON_EADMN_MASK) {
            drop_reason |= 1 << _SHR_TCB_TRACK_EADMN_DROP_SHIFT;
        }
        if (drop_vector & _TCB_EVENT_DROP_REASON_WRED_MASK) {
            drop_reason |= 1 << _SHR_TCB_TRACK_WRED_DROP_SHIFT;
        }
        if (drop_vector & _TCB_EVENT_DROP_REASON_CFAP_MASK) {
            drop_reason |= 1 << _SHR_TCB_TRACK_CFAP_DROP_SHIFT;
        }
        record_entries[idx].drop_reason = drop_reason;
    }

    soc_cm_sfree(unit, tcb_evt_buf);

    return BCM_E_NONE;

}

/*
 * Function:
 *      bcm_hx5_cosq_tcb_enable
 * Purpose:
 *      Enable or disable a TCB buffer.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) TCB buffer id.
 *      enable - (IN) enable or disable.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_hx5_cosq_tcb_enable(int unit, bcm_cosq_buffer_id_t buffer_id, int enable)
{
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    int mask, rv = BCM_E_NONE;
    uint64 rval64;

    tcb_unit_info = TCB_UNIT_CONTROL(unit);
    TCB_ID_CHECK(buffer_id);
    mask = 1;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_enable: unit %d, buffer_id %d, enable %d\n"),
                         unit, buffer_id, enable));

    if (!(tcb_unit_info->tcb_config_init_bmp & (1 << buffer_id))) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb config of buffer %d is not initialized\n"),
                             buffer_id));
        return BCM_E_DISABLED;
    }

    if (enable) {
        if (tcb_unit_info->tcb_enabled_bmp & mask) {
            /* tcb is already enabled */
            rv = BCM_E_BUSY;
        } else { /* enable tcb */
            SOC_IF_ERROR_RETURN(soc_reg64_get(unit, TCB_CONFIGr, buffer_id, 0,
                                              &rval64));
            soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, TCB_ENABLEf, 1);
            SOC_IF_ERROR_RETURN(soc_reg64_set(unit, TCB_CONFIGr, buffer_id, 0,
                                              rval64));

            if (tcb_unit_info->tcb_enabled_bmp == 0) {
                /* Enable interrupt */
                BCM_IF_ERROR_RETURN(_bcm_hx5_cosq_tcb_intr_enable_set(unit, 1));
            }
            tcb_unit_info->tcb_enabled_bmp |= mask;
        }
    } else {
        if (!(tcb_unit_info->tcb_enabled_bmp & mask)) {
            /* tcb is already disabled */
            rv =  BCM_E_PARAM;
        } else { /* disable tcb */
            BCM_IF_ERROR_RETURN(_bcm_hx5_cosq_tcb_ext_freeze_set(unit, buffer_id, 0));

            SOC_IF_ERROR_RETURN(soc_reg64_get(unit, TCB_CONFIGr, buffer_id, 0,
                                              &rval64));
            soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, TCB_ENABLEf, 0);
            SOC_IF_ERROR_RETURN(soc_reg64_set(unit, TCB_CONFIGr, buffer_id, 0,
                                              rval64));

            BCM_IF_ERROR_RETURN(_bcm_hx5_cosq_tcb_buffer_ready_clear(unit, buffer_id));

            tcb_unit_info->tcb_enabled_bmp &= ~mask;
            if (tcb_unit_info->tcb_enabled_bmp == 0) {
                /* Disable interrupt */
                BCM_IF_ERROR_RETURN(_bcm_hx5_cosq_tcb_intr_enable_set(unit, 0));
            }
        }
    }

    return rv;

}

/*
 * Function:
 *      bcm_hx5_cosq_tcb_freeze
 * Purpose:
 *      External freeze a TCB buffer.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) TCB buffer id.
 *      enable - (IN) enable or disable.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_hx5_cosq_tcb_freeze(int unit, bcm_cosq_buffer_id_t buffer_id, int enable)
{
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    tcb_unit_info = TCB_UNIT_CONTROL(unit);

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_freeze: unit %d, buffer_id %d, enable %d\n"),
                         unit, buffer_id, enable));

    if (buffer_id == -1) {
        buffer_id = 0;
    }

    TCB_ID_CHECK(buffer_id);

    if (!tcb_unit_info->tcb_enabled_bmp) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb buffer %d is disabled, can not freeze\n"),
                             buffer_id));
        return BCM_E_DISABLED;
    }

    if (0 != enable) {
        BCM_IF_ERROR_RETURN(
                _bcm_hx5_cosq_tcb_ext_freeze_set(unit, buffer_id, 1));
    } else {
        BCM_IF_ERROR_RETURN(
                _bcm_hx5_cosq_tcb_ext_freeze_set(unit, buffer_id, 0));
        BCM_IF_ERROR_RETURN(
                _bcm_hx5_cosq_tcb_buffer_ready_clear(unit, buffer_id));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_hx5_cosq_tcb_deinit
 * Purpose:
 *      Deinitialize all TCB internal structure.
 * Parameters:
 *      unit - (IN) BCM device number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_hx5_cosq_tcb_deinit(int unit)
{
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    _bcm_cosq_tcb_info_t *tcb_info;
    int i = 0;

    tcb_unit_info = TCB_UNIT_CONTROL(unit);
    if (tcb_unit_info == NULL) {
        return BCM_E_NONE;
    }

    if (NULL != tcb_unit_info->tcb_info[i]) {
        tcb_info = tcb_unit_info->tcb_info[i];
        if (NULL != tcb_info->profile_bmp) {
            sal_free(tcb_info->profile_bmp);
            tcb_info->profile_bmp = NULL;
        }
        sal_free(tcb_unit_info->tcb_info[i]);
        tcb_unit_info->tcb_info[i] = NULL;
    }

    if (NULL != tcb_unit_info->tcb_evt_cb) {
        tcb_unit_info->tcb_evt_cb = NULL;
    }

    if (NULL != tcb_unit_info->tcb_evt_user_data) {
        tcb_unit_info->tcb_evt_user_data = NULL;
    }

    if (NULL != tcb_unit_info->tcb_reslock) {
        sal_mutex_destroy(tcb_unit_info->tcb_reslock);
        tcb_unit_info->tcb_reslock = NULL;
    }

    sal_free(tcb_unit_info);

    TCB_UNIT_CONTROL(unit) = NULL;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_hx5_cosq_tcb_config_get
 * Purpose:
 *      Get TCB instance control status.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) TCB buffer id.
 *      config - (OUT) TCB config information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_hx5_cosq_tcb_config_get(int unit, bcm_cosq_buffer_id_t buffer_id,
                                    bcm_cosq_tcb_config_t *config)
{
    uint64 rval64;
    uint32 trigger, timer;
    int type, entity_num, propability;
    bcm_port_t mmu_port, phy_port, local_port;
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    soc_info_t *si;
    tcb_unit_info = TCB_UNIT_CONTROL(unit);

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_config_get: unit %d, buffer_id %d\n"),
                         unit, buffer_id));

    TCB_ID_CHECK(buffer_id);
    if (NULL == config) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit, "config is null!\n")));
        return BCM_E_PARAM;
    }

    si = &SOC_INFO(unit);

    /* check TCB instance status */
    if (!(tcb_unit_info->tcb_config_init_bmp & (1 << buffer_id))) {
        config->scope_type = bcmCosqTcbScopeMaxCount;
        config->gport = BCM_GPORT_INVALID;
        config->cosq = -1;
        config->trigger_reason = BCM_COSQ_TCB_INGRESS_ADMIN_DROP |
                                 BCM_COSQ_TCB_EGRESS_ADMIN_DROP  |
                                 BCM_COSQ_TCB_CFAP_DROP          |
                                 BCM_COSQ_TCB_WRED_DROP;
        config->pre_freeze_capture_num = _TCB_CAP_NUM_DEFAULT;
        config->pre_freeze_capture_time = _TCB_CAP_TIM_DEFAULT * 10 / si->frequency;
        config->pre_sample_probability = _TCB_PRE_SAMP_RATE_DEFAULT + 1;
        config->post_sample_probability = _TCB_POST_SAMP_RATE_DEFAULT + 1;

        return BCM_E_NONE;
    }

    SOC_IF_ERROR_RETURN(soc_reg64_get(unit, TCB_CONFIGr, buffer_id, 0, &rval64));

    type = soc_reg64_field32_get(unit, TCB_CONFIGr, rval64, SCOPE_ENTITY_TYPEf);

    switch(type) {
        case _TCB_SCOPE_UCQ:
            config->scope_type = bcmCosqTcbScopeUcastQueue;
            entity_num = soc_reg64_field32_get(unit, TCB_CONFIGr, rval64,
                                               SCOPE_ENTITYf);
            BCM_IF_ERROR_RETURN
                (_bcm_cosq_tcb_uc_queue_resolve(unit, buffer_id, entity_num,
                                                &config->gport));
            break;
        case _TCB_SCOPE_ING_PORT:
            config->scope_type = bcmCosqTcbScopeIngressPort;
            entity_num = soc_reg64_field32_get(unit, TCB_CONFIGr, rval64,
                                               SCOPE_ENTITYf);
            mmu_port = entity_num;
            phy_port = si->port_m2p_mapping[mmu_port];
            local_port = si->port_p2l_mapping[phy_port];
            BCM_GPORT_LOCAL_SET(config->gport, local_port);
            break;
        case _TCB_SCOPE_EGR_PORT:
            config->scope_type = bcmCosqTcbScopeEgressPort;
            entity_num = soc_reg64_field32_get(unit, TCB_CONFIGr, rval64,
                                               SCOPE_ENTITYf);
            mmu_port = entity_num;
            phy_port = si->port_m2p_mapping[mmu_port];
            local_port = si->port_p2l_mapping[phy_port];
            BCM_GPORT_LOCAL_SET(config->gport, local_port);
            break;
        default:
            LOG_ERROR(BSL_LS_BCM_COSQ,
                      (BSL_META_U(unit,
                                  "scope type %d is illegal\n"),
                                  type));
            return BCM_E_PARAM;
    }

    trigger = soc_reg64_field32_get(unit, TCB_CONFIGr, rval64, TRACK_IADMN_DROPf);
    config->trigger_reason = trigger << _SHR_TCB_TRACK_IADMN_DROP_SHIFT;
    trigger = soc_reg64_field32_get(unit, TCB_CONFIGr, rval64, TRACK_EADMN_DROPf);
    config->trigger_reason |= trigger << _SHR_TCB_TRACK_EADMN_DROP_SHIFT;
    trigger = soc_reg64_field32_get(unit, TCB_CONFIGr, rval64, TRACK_CFAP_DROPf);
    config->trigger_reason |= trigger << _SHR_TCB_TRACK_CFAP_DROP_SHIFT;
    trigger = soc_reg64_field32_get(unit, TCB_CONFIGr, rval64, TRACK_WRED_DROPf);
    config->trigger_reason |= trigger << _SHR_TCB_TRACK_WRED_DROP_SHIFT;

    config->pre_freeze_capture_num = soc_reg64_field32_get(unit, TCB_CONFIGr,
                                                           rval64, REV_COUNTERf);
    timer = soc_reg64_field32_get(unit, TCB_CONFIGr, rval64,
                                                WD_TIMER_PRESETf);
    config->pre_freeze_capture_time =  timer * 10 / si->frequency;
    propability = soc_reg64_field32_get(unit, TCB_CONFIGr, rval64,
                                        PRE_TRIGGER_SAMPLE_RATEf);
    config->pre_sample_probability = propability + 1;
    propability = soc_reg64_field32_get(unit, TCB_CONFIGr, rval64,
                                        POST_TRIGGER_SAMPLE_RATEf);
    config->post_sample_probability = propability + 1;

    return BCM_E_NONE;

}

/*
 * Function:
 *      bcm_hx5_cosq_tcb_config_set
 * Purpose:
 *      Set TCB instance config attributes.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) TCB buffer id.
 *      config - (IN) TCB config information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_hx5_cosq_tcb_config_set(int unit, bcm_cosq_buffer_id_t buffer_id,
                            bcm_cosq_tcb_config_t *config)
{
    uint64 rval64 = COMPILER_64_INIT(0,0);
    int timer;
    bcm_port_t local_port;
    bcm_gport_t gport;
    int mmu_port, phy_port, q_num, arg = 0, cosq;
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    soc_info_t *si;
    tcb_unit_info = TCB_UNIT_CONTROL(unit);

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_config_set: unit %d, buffer_id %d\n"),
                         unit, buffer_id));

    TCB_ID_CHECK(buffer_id);
    if (NULL == config) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit, "config is null!\n")));
        return BCM_E_PARAM;
    }

    if (tcb_unit_info->tcb_enabled_bmp) {
        /* tcb can only be configured in idle state */
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb buffer %d is enabled\n"),
                             buffer_id));
        return BCM_E_BUSY;
    }

    gport = config->gport;
    si = &SOC_INFO(unit);

    /* Reset default value */
    if ((config->scope_type == bcmCosqTcbScopeMaxCount) &&
        (config->gport == BCM_GPORT_INVALID) &&
        (config->cosq == -1) &&
        (config->trigger_reason == (BCM_COSQ_TCB_INGRESS_ADMIN_DROP |
                                    BCM_COSQ_TCB_EGRESS_ADMIN_DROP  |
                                    BCM_COSQ_TCB_CFAP_DROP          |
                                    BCM_COSQ_TCB_WRED_DROP)) &&
        (config->pre_freeze_capture_num == _TCB_CAP_NUM_DEFAULT) &&
        (config->pre_freeze_capture_time ==
                                _TCB_CAP_TIM_DEFAULT * 10 / si->frequency) &&
        (config->pre_sample_probability == _TCB_PRE_SAMP_RATE_DEFAULT + 1) &&
        (config->post_sample_probability == _TCB_POST_SAMP_RATE_DEFAULT + 1)) {

        SOC_IF_ERROR_RETURN(soc_reg64_set(unit, TCB_CONFIGr, buffer_id, 0, rval64));
        /* update TCB instance status */
        if ((tcb_unit_info->tcb_config_init_bmp & (1 << buffer_id))) {
            tcb_unit_info->tcb_config_init_bmp &= ~(1 << buffer_id);
        }
        return BCM_E_NONE;
    }

    SOC_IF_ERROR_RETURN(soc_reg64_get(unit, TCB_CONFIGr, buffer_id, 0, &rval64));

    switch(config->scope_type) {
        case bcmCosqTcbScopeUcastQueue:
            if (!BCM_GPORT_IS_SET(gport)) {
                LOG_INFO(BSL_LS_BCM_COSQ,
                         (BSL_META_U(unit,
                                     "tcb config set, gport 0x%x is illegal\n"),
                                     gport));
                return BCM_E_PARAM;
            }
            if (!BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
                LOG_INFO(BSL_LS_BCM_COSQ,
                         (BSL_META_U(unit,
                                     "tcb config set, gport 0x%x is illegal\n"),
                                     gport));
                return BCM_E_PARAM;
            }

            BCM_IF_ERROR_RETURN(_bcm_hx5_cosq_index_resolve(unit, gport, 0,
                                    _BCM_HX5_COSQ_INDEX_STYLE_UCAST_QUEUE, NULL,
                                    &cosq, NULL));
            q_num = cosq;
            soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, SCOPE_ENTITY_TYPEf,
                                  _TCB_SCOPE_UCQ);
            arg = q_num;
            break;
        case bcmCosqTcbScopeIngressPort:
            if (BCM_GPORT_IS_SET(gport)){
                BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, gport,
                                                           &local_port));
            } else if (SOC_PORT_VALID(unit, gport)) {
                local_port = gport;
            } else {
                LOG_INFO(BSL_LS_BCM_COSQ,
                         (BSL_META_U(unit,
                                     "tcb config set, gport 0x%x is illegal\n"),
                                     gport));
                return BCM_E_PORT;
            }

            if (IS_CPU_PORT(unit, local_port) || IS_LB_PORT(unit, local_port)) {
                LOG_INFO(BSL_LS_BCM_COSQ,
                         (BSL_META_U(unit,
                                     "tcb config set, gport 0x%x is illegal\n"),
                                     gport));
                return BCM_E_PORT;
            }

            phy_port = si->port_l2p_mapping[local_port];
            mmu_port = si->port_p2m_mapping[phy_port];

            soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, SCOPE_ENTITY_TYPEf,
                              _TCB_SCOPE_ING_PORT);
            arg = mmu_port;
            break;
        case bcmCosqTcbScopeEgressPort:
            if (BCM_GPORT_IS_SET(gport)){
                BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, gport,
                                                           &local_port));
            } else if (SOC_PORT_VALID(unit, gport)) {
                local_port = gport;
            } else {
                LOG_INFO(BSL_LS_BCM_COSQ,
                         (BSL_META_U(unit,
                                     "tcb config set, gport 0x%x is illegal\n"),
                                     gport));
                return BCM_E_PORT;
            }

            if (IS_CPU_PORT(unit, local_port) || IS_LB_PORT(unit, local_port)) {
                LOG_INFO(BSL_LS_BCM_COSQ,
                         (BSL_META_U(unit,
                                     "tcb config set, gport 0x%x is illegal\n"),
                                     gport));
                return BCM_E_PORT;
            }

            phy_port = si->port_l2p_mapping[local_port];
            mmu_port = si->port_p2m_mapping[phy_port];

            soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, SCOPE_ENTITY_TYPEf,
                                  _TCB_SCOPE_EGR_PORT);
            arg = mmu_port;
            break;
        default:
            LOG_INFO(BSL_LS_BCM_COSQ,
                     (BSL_META_U(unit,
                                 "tcb config set, scope type %d is illegal\n"),
                                 config->scope_type));
            return BCM_E_PARAM;
    }

    soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, SCOPE_ENTITYf, arg);

    if (config->trigger_reason > _TCB_TRIGGER_MAX) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb config set, trigger reason %d is illegal\n"),
                             config->trigger_reason));
        return BCM_E_PARAM;
    }
    soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, TRACK_IADMN_DROPf,
                      (config->trigger_reason & BCM_COSQ_TCB_INGRESS_ADMIN_DROP) ? 1:0);
    soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, TRACK_EADMN_DROPf,
                      (config->trigger_reason & BCM_COSQ_TCB_EGRESS_ADMIN_DROP) ? 1:0);
    soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, TRACK_CFAP_DROPf,
                      (config->trigger_reason & BCM_COSQ_TCB_CFAP_DROP) ? 1:0);
    soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, TRACK_WRED_DROPf,
                      (config->trigger_reason & BCM_COSQ_TCB_WRED_DROP) ? 1:0);

    /* rev_counter valid range is 2 - 1024 */
    if ((config->pre_freeze_capture_num < _TCB_CAP_NUM_MIN) ||
        (config->pre_freeze_capture_num > _TCB_CAP_NUM_MAX)) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb config set, pre freeze capture %d is illegal\n"),
                             config->pre_freeze_capture_num));
        return BCM_E_PARAM;
    }
    soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, REV_COUNTERf,
                          config->pre_freeze_capture_num);

    /* WD_TIMER_PRESET is a counter of clock cycles * 10, a value 1 means 10 clock cycles */
    timer = (config->pre_freeze_capture_time) * (si->frequency) / 10;
    if (timer > _TCB_CAP_TIM_MAX) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb config set, pre freeze capture time %d is illegal\n"),
                             config->pre_freeze_capture_time));
        return BCM_E_PARAM;
    }
    soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, WD_TIMER_PRESETf,
                          timer);

    if (config->post_sample_probability > _TCB_POST_SAMP_RATE_MAX ||
        config->post_sample_probability <= 0) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb config set, post sample probability %d is illegal\n"),
                             config->post_sample_probability));
        return BCM_E_PARAM;
    }
    soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, POST_TRIGGER_SAMPLE_RATEf,
                          config->post_sample_probability - 1);

    if (config->pre_sample_probability > _TCB_PRE_SAMP_RATE_MAX ||
        config->pre_sample_probability <= 0) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb config set, pre sample probability %d is illegal\n"),
                             config->pre_sample_probability));
        return BCM_E_PARAM;
    }
    soc_reg64_field32_set(unit, TCB_CONFIGr, &rval64, PRE_TRIGGER_SAMPLE_RATEf,
                          config->pre_sample_probability - 1);

    SOC_IF_ERROR_RETURN(soc_reg64_set(unit, TCB_CONFIGr, buffer_id, 0, rval64));

    /* update TCB instance status */
    if (!(tcb_unit_info->tcb_config_init_bmp & (1 << buffer_id))) {
        tcb_unit_info->tcb_config_init_bmp |= (1 << buffer_id);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_hx5_cosq_tcb_gport_threshold_mapping_set
 * Purpose:
 *      Set threshold profile for a UCQ or a port.
 * Parameters:
 *      unit - (IN) unit number
 *      gport - (IN) Ucq or a port
 *      cosq  - (IN) Reserved field
 *      buffer_id - (IN) TCB buffer id.
 *      index - (IN) Threshold profile index
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      When gport is a port, SDK will attach all Ucqs in this port to one threshold profile.
 */
int
bcm_hx5_cosq_tcb_gport_threshold_mapping_set(int unit,
                                      bcm_gport_t gport, bcm_cos_queue_t cosq,
                                      bcm_cosq_buffer_id_t buffer_id,
                                      int index)
{
    soc_mem_t profile_mem;
    soc_field_t field;
    bcm_port_t local_port;
    int mmu_port, phy_port, i, old_profile;
    bcm_cos_t cos;
    soc_info_t *si;
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    _bcm_cosq_tcb_info_t *tcb_config;
    uint32 entry[SOC_MAX_MEM_WORDS];

    si = &SOC_INFO(unit);
    tcb_unit_info = TCB_UNIT_CONTROL(unit);
    TCB_ID_CHECK(buffer_id);

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_threshold_mapping_set: unit %d, gport 0x%x, "
                         "cosq %d, buffer_id %d, index %d\n"),
                         unit, gport, cosq, buffer_id, index));

    if (tcb_unit_info->tcb_enabled_bmp) {
        /* tcb can only be configured in idle state */
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb buffer %d is enabled\n"),
                             buffer_id));
        return BCM_E_BUSY;
    }

    tcb_config = tcb_unit_info->tcb_info[buffer_id];
    profile_mem = SOC_MEM_UNIQUE_ACC(unit, TCB_THRESHOLD_PROFILE_MAPm)[buffer_id];

    if (BCM_GPORT_IS_SET(gport)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN(_bcm_hx5_cosq_index_resolve(unit, gport, 0,
                                    _BCM_HX5_COSQ_INDEX_STYLE_COS, &local_port,
                                    &cos, NULL));
            cos = cos - si->port_uc_cosq_base[local_port];
        } else if (BCM_GPORT_IS_LOCAL(gport)) {
            local_port = BCM_GPORT_LOCAL_GET(gport);
            cos = -1;
        } else {
            LOG_INFO(BSL_LS_BCM_COSQ,
                     (BSL_META_U(unit,
                                 "tcb_threshold_mapping_set, gport 0x%x is illegal\n"),
                                 gport));
            return BCM_E_PARAM;
        }
    } else {
        local_port = gport;
        cos = -1;
    }

    if (!SOC_PORT_VALID(unit, local_port) || IS_CPU_PORT(unit, local_port) ||
        IS_LB_PORT(unit, local_port)) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb_threshold_mapping_set, gport 0x%x is illegal\n"),
                             gport));
        return BCM_E_PORT;
    }

    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];

    if (!SHR_BITGET(tcb_config->profile_bmp, index)) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit,
                              "profile %d is illegal\n"),
                              index));
        return BCM_E_NOT_FOUND;
    }

    if (-1 == cos) {
        BCM_IF_ERROR_RETURN (soc_mem_read(unit, profile_mem, MEM_BLOCK_ANY,
                                          mmu_port, entry));
        for (i = 0; i < _HX5_NUM_PROFILE_COS; i++) {
            field = _bcm_hx5_cosq_tcb_threshold_profile_map_fields[i];
            old_profile = soc_mem_field32_get(unit, TCB_THRESHOLD_PROFILE_MAPm,
                                              entry, field);
            tcb_config->profile_ref_count[old_profile]--;
            soc_mem_field32_set(unit, TCB_THRESHOLD_PROFILE_MAPm, entry, field,
                                index);
            tcb_config->profile_ref_count[index]++;
        }
        BCM_IF_ERROR_RETURN (soc_mem_write(unit, profile_mem, MEM_BLOCK_ALL,
                                           mmu_port, entry));
    } else {
        field = _bcm_hx5_cosq_tcb_threshold_profile_map_fields[cos];
        BCM_IF_ERROR_RETURN (soc_mem_read(unit, profile_mem, MEM_BLOCK_ANY,
                                          mmu_port, entry));
        old_profile = soc_mem_field32_get(unit, TCB_THRESHOLD_PROFILE_MAPm,
                                          entry, field);
        tcb_config->profile_ref_count[old_profile]--;

        soc_mem_field32_set(unit, TCB_THRESHOLD_PROFILE_MAPm, entry,
                            field, index);
        tcb_config->profile_ref_count[index]++;
        BCM_IF_ERROR_RETURN (soc_mem_write(unit, profile_mem, MEM_BLOCK_ALL,
                                           mmu_port, entry));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_hx5_cosq_tcb_gport_threshold_mapping_get
 * Purpose:
 *      Set threshold profile for a UCQ or a port.
 * Parameters:
 *      unit - (IN) unit number
 *      gport - (IN) Ucq or a port
 *      cosq  - (IN) Reserved field
 *      buffer_id - (IN) TCB buffer id.
 *      index - (OUT) Threshold profile index
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      When gport is a port, SDK will return the threshold profile index of the first Ucq in this
 *      port.
 */
int
bcm_hx5_cosq_tcb_gport_threshold_mapping_get(int unit,
                                     bcm_gport_t gport, bcm_cos_queue_t cosq,
                                     bcm_cosq_buffer_id_t buffer_id, int *index)
{
    soc_mem_t profile_mem;
    soc_field_t field;
    bcm_port_t local_port;
    int mmu_port, phy_port, pipe, port_index, cos;
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    _bcm_cosq_tcb_info_t *tcb_config;
    soc_info_t *si;
    uint32 entry[SOC_MAX_MEM_WORDS];

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_threshold_mapping_get: unit %d, gport 0x%x, "
                         "cosq %d, buffer_id %d\n"),
                         unit, gport, cosq, buffer_id));

    TCB_ID_CHECK(buffer_id);
    if (NULL == index) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit, "index is null!\n")));
        return BCM_E_PARAM;
    }

    tcb_unit_info = TCB_UNIT_CONTROL(unit);
    si = &SOC_INFO(unit);

    profile_mem = SOC_MEM_UNIQUE_ACC(unit, TCB_THRESHOLD_PROFILE_MAPm)[buffer_id];

    if (BCM_GPORT_IS_SET(gport)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN(_bcm_hx5_cosq_index_resolve(unit, gport, 0,
                                    _BCM_HX5_COSQ_INDEX_STYLE_COS, &local_port,
                                    &cos, NULL));
            cos = cos - si->port_uc_cosq_base[local_port];
        } else if (BCM_GPORT_IS_LOCAL(gport)) {
            local_port = BCM_GPORT_LOCAL_GET(gport);
            cos = 0;
        } else {
            LOG_INFO(BSL_LS_BCM_COSQ,
                     (BSL_META_U(unit,
                                 "tcb_threshold_mapping_get, gport 0x%x is illegal\n"),
                                 gport));
            return BCM_E_PARAM;
        }
    } else {
        local_port = gport;
        cos = 0;
    }

    if (!SOC_PORT_VALID(unit, local_port) || IS_CPU_PORT(unit, local_port) ||
        IS_LB_PORT(unit, local_port)) {
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb_threshold_mapping_get, gport 0x%x is illegal\n"),
                             gport));
        return BCM_E_PORT;
    }

    phy_port = si->port_l2p_mapping[local_port];
    mmu_port = si->port_p2m_mapping[phy_port];
    pipe = si->port_pipe[local_port];
    port_index = mmu_port % 66 + pipe * 66;

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, profile_mem, MEM_BLOCK_ANY,
                                     port_index, entry));

    field = _bcm_hx5_cosq_tcb_threshold_profile_map_fields[cos];
    *index = soc_mem_field32_get(unit,TCB_THRESHOLD_PROFILE_MAPm, entry, field);

    tcb_config = tcb_unit_info->tcb_info[buffer_id];
    if (!SHR_BITGET(tcb_config->profile_bmp, *index)) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit,
                              "profile %d is illegal\n"),
                              *index));
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;

}

#ifdef BCM_WARM_BOOT_SUPPORT
int
bcm_hx5_cosq_tcb_reinit(int unit)
{
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    _bcm_cosq_tcb_info_t *tcb_config;
    int i = 0, j, k, n, enable, index, start_threshold, stop_threshold;
    uint64 rval64;
    soc_mem_t profile_mem;
    soc_field_t field;

    tcb_unit_info = TCB_UNIT_CONTROL(unit);
    TCB_INIT(unit);

    SOC_IF_ERROR_RETURN(soc_reg64_get(unit, TCB_CONFIGr, i, 0, &rval64));
    enable = soc_reg64_field32_get(unit, TCB_CONFIGr, rval64, TCB_ENABLEf);
    if (enable) {
        tcb_unit_info->tcb_enabled_bmp |= 1 << i;
    }

    profile_mem = SOC_MEM_UNIQUE_ACC(unit, TCB_THRESHOLD_PROFILE_MAPm)[i];
    tcb_config = tcb_unit_info->tcb_info[i];
    SHR_BITSET(tcb_config->profile_bmp, 0);

    for (n = 1; n < _BCM_HX5_TCB_PROFILE_MAX_NUM; n++) {
        BCM_IF_ERROR_RETURN(soc_reg64_get(unit, TCB_THRESHOLD_PROFILEr, i,
                                          n, &rval64));
        start_threshold = soc_reg_field_get(unit, TCB_THRESHOLD_PROFILEr,
                                            rval64, CAPTURE_START_THRESHOLDf);
        stop_threshold = soc_reg_field_get(unit, TCB_THRESHOLD_PROFILEr,
                                           rval64, CAPTURE_END_THRESHOLDf);
        if (start_threshold != 0 || stop_threshold != 0) {
            SHR_BITSET(tcb_config->profile_bmp, n);
        }

    }
    for (j = 0; j < soc_mem_index_count(unit, profile_mem); j++) {
        BCM_IF_ERROR_RETURN (soc_mem_read(unit, profile_mem, MEM_BLOCK_ANY,
                                          j, &rval64));
        for (k = 0; k < _HX5_NUM_PROFILE_COS; k++) {
            field = _bcm_hx5_cosq_tcb_threshold_profile_map_fields[k];
            index = soc_mem_field32_get(unit,TCB_THRESHOLD_PROFILE_MAPm,
                                        &rval64, field);
            for (n = 0; n < _BCM_HX5_TCB_PROFILE_MAX_NUM; n++) {
                if (index == n) {
                    if(SHR_BITGET(tcb_config->profile_bmp, n)) {
                    tcb_config->profile_ref_count[n]++;
                    }
                }
            }
        }
    }

    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *      bcm_hx5_cosq_tcb_threshold_profile_create
 * Purpose:
 *      Create a threshold profile for a specified TCB buffer.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      flags - (IN) BCM_COSQ_TCB_THRESHOLD_PROFILE_WITH_ID: Index arg is given
 *                       BCM_COSQ_TCB_THRESHOLD_PROFILE_REPLACE: Replace existing profile
 *      buffer_id - (IN) TCB buffer id.
 *      threshold - (IN) Threshold profile
 *      index - (INOUT) profile index
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If user do not specify index, SDK will alloc a profile index and return to user.
 *      With BCM_COSQ_TCB_THRESHOLD_PROFILE_REPLACE flag user could directly update a
 *      existing threshold profile to new values.
 */
int
bcm_hx5_cosq_tcb_threshold_profile_create(int unit, int flags,
                                     bcm_cosq_buffer_id_t buffer_id,
                                     bcm_cosq_tcb_threshold_profile_t *threshold,
                                     int *index)
{
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    _bcm_cosq_tcb_info_t *tcb_config;

    uint64 rval64;
    int start_threshold, stop_threshold, i, start_cell, stop_cell, cell_bytes, threshold_max;

    tcb_unit_info = TCB_UNIT_CONTROL(unit);
    TCB_ID_CHECK(buffer_id);
    cell_bytes = tcb_unit_info->cell_bytes;
    threshold_max = tcb_unit_info->threshold_max;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_threshold_profile_create: unit %d, "
                         "flags %d, buffer_id %d\n"),
                         unit, flags, buffer_id));

    if (_bcm_hx5_cosq_tcb_is_enabled(unit, buffer_id)) {
        /* tcb can only be configured in idle state */
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb buffer %d is enabled\n"),
                             buffer_id));
        return BCM_E_BUSY;
    }

    if (NULL == threshold || NULL == index) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit, "threshold or index is null!\n")));
        return BCM_E_PARAM;
    }

    start_threshold = _MMU_BYTES_TO_CELLS(threshold->start_threshold_bytes,
                                              cell_bytes);
    stop_threshold = _MMU_BYTES_TO_CELLS(threshold->stop_threshold_bytes,
                                             cell_bytes);
    if (start_threshold> threshold_max || start_threshold <= 0 ||
        stop_threshold > threshold_max || stop_threshold <= 0 ||
        start_threshold <= stop_threshold) {
        /* start_threshold should be higher than stop_threshold */
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb profile create: start threshold %d or "
                             "stop threshold %d is illegal.\n"),
                             start_threshold, stop_threshold));
        return BCM_E_PARAM;
    }

    if (flags & BCM_COSQ_TCB_THRESHOLD_PROFILE_REPLACE) {
        if (!(flags & BCM_COSQ_TCB_THRESHOLD_PROFILE_WITH_ID)) {
            LOG_INFO(BSL_LS_BCM_COSQ,
                     (BSL_META_U(unit,
                                 "tcb profile create: flags %d is illegal.\n"),
                                 flags));
            return BCM_E_PARAM;
        }
        if (*index >= _BCM_HX5_TCB_PROFILE_MAX_NUM || *index <= 0) {
            LOG_INFO(BSL_LS_BCM_COSQ,
                     (BSL_META_U(unit,
                                 "tcb profile create: index %d is illegal.\n"),
                                 *index));
            return BCM_E_PARAM;
        }

        tcb_config = tcb_unit_info->tcb_info[buffer_id];
        if (!SHR_BITGET(tcb_config->profile_bmp, *index))
        {
            LOG_ERROR(BSL_LS_BCM_COSQ,
                      (BSL_META_U(unit, "index %d is illegal.\n"),
                                  *index));
            return BCM_E_NOT_FOUND;
        }
    } else if (flags & BCM_COSQ_TCB_THRESHOLD_PROFILE_WITH_ID) {
        if (*index >= _BCM_HX5_TCB_PROFILE_MAX_NUM || *index <= 0) {
            LOG_ERROR(BSL_LS_BCM_COSQ,
                      (BSL_META_U(unit, "index %d is illegal.\n"),
                                  *index));
            return BCM_E_PARAM;
        }

        tcb_config = tcb_unit_info->tcb_info[buffer_id];
        if (SHR_BITGET(tcb_config->profile_bmp, *index))
        {
            LOG_ERROR(BSL_LS_BCM_COSQ,
                      (BSL_META_U(unit, "index %d is illegal.\n"),
                                  *index));
            return BCM_E_EXISTS;
        }
        SHR_BITSET(tcb_config->profile_bmp, *index);

    } else {/* allocate new profile*/
        for (i = 0; i < _BCM_HX5_TCB_PROFILE_MAX_NUM; i++) {
            BCM_IF_ERROR_RETURN(soc_reg64_get(unit, TCB_THRESHOLD_PROFILEr,
                                              buffer_id, i, &rval64));
            start_cell = soc_reg64_field_get(unit, TCB_THRESHOLD_PROFILEr, rval64,
                                           CAPTURE_START_THRESHOLDf);
            stop_cell = soc_reg64_field_get(unit, TCB_THRESHOLD_PROFILEr, rval64,
                                          CAPTURE_END_THRESHOLDf);
            if (start_threshold == start_cell && stop_threshold == stop_cell) {
                *index = i;
                return BCM_E_NONE;
            }
        }
        BCM_IF_ERROR_RETURN(
                _bcm_hx5_cosq_tcb_profile_id_alloc(unit, buffer_id, index));
    }

    BCM_IF_ERROR_RETURN(soc_reg64_get(unit, TCB_THRESHOLD_PROFILEr, buffer_id,
                                      *index, &rval64));
    soc_reg64_field_set(unit, TCB_THRESHOLD_PROFILEr, &rval64,
                        CAPTURE_START_THRESHOLDf, start_threshold);
    soc_reg64_field_set(unit, TCB_THRESHOLD_PROFILEr, &rval64,
                        CAPTURE_END_THRESHOLDf, stop_threshold);

    BCM_IF_ERROR_RETURN(soc_reg64_set(unit, TCB_THRESHOLD_PROFILEr, buffer_id, *index, rval64));

    return BCM_E_NONE;
}
/*
 * Function:
 *      bcm_hx5_cosq_tcb_threshold_profile_get
 * Purpose:
 *      Get a threshold profile for a specified TCB buffer.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) TCB buffer id.
 *      index - (IN) profile index
 *      threshold - (OUT) Threshold profile
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_hx5_cosq_tcb_threshold_profile_get(int unit,
                              bcm_cosq_buffer_id_t buffer_id, int index,
                              bcm_cosq_tcb_threshold_profile_t *threshold)
{
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    _bcm_cosq_tcb_info_t *tcb_config;

    uint64 rval64;
    int cells, cell_bytes;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_threshold_profile_get: unit %d, "
                         "buffer_id %d index %d\n"),
                         unit, buffer_id, index));

    tcb_unit_info = TCB_UNIT_CONTROL(unit);
    TCB_ID_CHECK(buffer_id);
    cell_bytes = tcb_unit_info->cell_bytes;

    if (index >= _BCM_HX5_TCB_PROFILE_MAX_NUM || index <= 0) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit, "index %d is illegal.\n"), index));
        return BCM_E_PARAM;
    }

    if (NULL == threshold) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit, "threshold is null!\n")));
        return BCM_E_PARAM;
    }

    tcb_config = tcb_unit_info->tcb_info[buffer_id];
    if (!SHR_BITGET(tcb_config->profile_bmp, index))
    {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit, "index %d is illegal.\n"), index));
        return BCM_E_NOT_FOUND;
    }

    SOC_IF_ERROR_RETURN(soc_reg64_get(unit, TCB_THRESHOLD_PROFILEr, buffer_id,
                                      index, &rval64));
    cells = soc_reg64_field_get(unit, TCB_THRESHOLD_PROFILEr, rval64,
                              CAPTURE_START_THRESHOLDf);
    threshold->start_threshold_bytes = _MMU_CELLS_TO_BYTES(cells, cell_bytes);
    cells = soc_reg64_field_get(unit, TCB_THRESHOLD_PROFILEr, rval64,
                              CAPTURE_END_THRESHOLDf);
    threshold->stop_threshold_bytes = _MMU_CELLS_TO_BYTES(cells, cell_bytes);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_hx5_cosq_tcb_threshold_profile_destroy
 * Purpose:
 *      Destory a existing threshold profile for a specified TCB buffer.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) TCB buffer id.
 *      index - (IN) profile index
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_hx5_cosq_tcb_threshold_profile_destroy(int unit,
                                             bcm_cosq_buffer_id_t buffer_id,
                                             int index)
{
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    _bcm_cosq_tcb_info_t *tcb_config;
    uint64 rval64;
    soc_mem_t profile_mem;
    soc_field_t field;
    int num_entries, mem_wsz;
    int i, j, index_min, index_max;
    int old_profile;
    void *pentry;
    char *dmabuf;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_threshold_profile_destroy: unit %d, "
                         "buffer_id %d index %d\n"),
                         unit, buffer_id, index));

    tcb_unit_info = TCB_UNIT_CONTROL(unit);
    TCB_ID_CHECK(buffer_id);

    if (_bcm_hx5_cosq_tcb_is_enabled(unit, buffer_id)) {
        /* tcb can only be configured in idle state */
        LOG_INFO(BSL_LS_BCM_COSQ,
                 (BSL_META_U(unit,
                             "tcb buffer %d is enabled\n"),
                             buffer_id));
        return BCM_E_BUSY;
    }

    if (index >= _BCM_HX5_TCB_PROFILE_MAX_NUM || index <= 0) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit, "index %d is illegal.\n"), index));
        return BCM_E_PARAM;
    }

    tcb_config = tcb_unit_info->tcb_info[buffer_id];
    if (!SHR_BITGET(tcb_config->profile_bmp, index))
    {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit, "index %d is illegal.\n"), index));
        return BCM_E_NOT_FOUND;
    }

    SOC_IF_ERROR_RETURN(soc_reg64_get(unit, TCB_THRESHOLD_PROFILEr, buffer_id,
                                      index, &rval64));
    soc_reg64_field_set(unit, TCB_THRESHOLD_PROFILEr, &rval64,
                        CAPTURE_START_THRESHOLDf, 0);
    soc_reg64_field_set(unit, TCB_THRESHOLD_PROFILEr, &rval64,
                        CAPTURE_END_THRESHOLDf, 0);
    SOC_IF_ERROR_RETURN(soc_reg64_set(unit, TCB_THRESHOLD_PROFILEr, buffer_id, index, rval64));

    /* Reset profile map table */
    profile_mem = SOC_MEM_UNIQUE_ACC(unit, TCB_THRESHOLD_PROFILE_MAPm)[buffer_id];
    num_entries = soc_mem_index_count(unit, profile_mem);
    index_min = soc_mem_index_min(unit, profile_mem);
    index_max = soc_mem_index_max(unit, profile_mem);
    mem_wsz = sizeof(uint32) * soc_mem_entry_words(unit, profile_mem);

    dmabuf = soc_cm_salloc(unit, num_entries * mem_wsz, "tcb profile");
    if (dmabuf == NULL) {
        return BCM_E_MEMORY;
    }

    if (soc_mem_read_range(unit, profile_mem, MEM_BLOCK_ALL,
                           index_min, index_max, dmabuf)) {
        soc_cm_sfree(unit, dmabuf);
        return BCM_E_INTERNAL;
    }

    for (i = index_min; i <= index_max; i++) {
        pentry = soc_mem_table_idx_to_pointer(unit, profile_mem, void*, dmabuf, i);
        for (j = 0; j < _HX5_NUM_PROFILE_COS;j++) {
            field = _bcm_hx5_cosq_tcb_threshold_profile_map_fields[j];
            old_profile = soc_mem_field32_get(unit, TCB_THRESHOLD_PROFILE_MAPm,
                                              pentry, field);
            if (old_profile == index) {
                tcb_config->profile_ref_count[index]--;
                soc_mem_field32_set(unit, TCB_THRESHOLD_PROFILE_MAPm,
                                    pentry, field, 0);
                tcb_config->profile_ref_count[0]++;
            }
        }
    }

    if (soc_mem_write_range(unit, profile_mem, MEM_BLOCK_ALL,
                            index_min, index_max, dmabuf)) {
        soc_cm_sfree(unit, dmabuf);
        return BCM_E_INTERNAL;
    }

    if (tcb_config->profile_ref_count[index] != 0) {
        LOG_ERROR(BSL_LS_BCM_COSQ, (BSL_META_U(unit,
                                    "TCB buffer %d profile %d ref_count %d != 0\n"),
                                    buffer_id, index,
                                    tcb_config->profile_ref_count[index]));
    }

    SHR_BITCLR(tcb_config->profile_bmp, index);
    soc_cm_sfree(unit, dmabuf);

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_hx5_cosq_tcb_init
 * Purpose:
 *      Initialize (clear) all TCB internal structure.
 * Parameters:
 *      unit - (IN) BCM device number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_hx5_cosq_tcb_init(int unit)
{
    _bcm_cosq_tcb_unit_ctrl_t *tcb_unit_info;
    _bcm_cosq_tcb_info_t *tcb_info;
    bcm_cosq_buffer_id_t buffer_id = 0;
    soc_mem_t profile_mem;

    (void)_bcm_hx5_cosq_tcb_deinit(unit);

    tcb_unit_info = TCB_UNIT_CONTROL(unit);
    if (NULL == tcb_unit_info) {
        tcb_unit_info = sal_alloc(sizeof(_bcm_cosq_tcb_unit_ctrl_t), "tcb_dev_info");
        if (!tcb_unit_info) {
            return BCM_E_MEMORY;
        }
    }

    sal_memset(tcb_unit_info, 0, sizeof(_bcm_cosq_tcb_unit_ctrl_t));

    TCB_UNIT_CONTROL(unit) = tcb_unit_info;

    tcb_unit_info->tcb_num = _BCM_HX5_TCB_MAX_NUM;
    tcb_unit_info->cell_bytes = _HX5_MMU_BYTES_PER_CELL;
    tcb_unit_info->threshold_max = _BCM_HX5_TCB_THRESHOLD_MAX;

    /* Create protection mutex. */
    if (tcb_unit_info->tcb_reslock == NULL) {
        tcb_unit_info->tcb_reslock = sal_mutex_create("tcb_resource_lock");
    }

    if (tcb_unit_info->tcb_reslock == NULL) {
        (void)_bcm_hx5_cosq_tcb_deinit(unit);
        return BCM_E_MEMORY;
    }

    tcb_unit_info->tcb_info[buffer_id] =
            sal_alloc(sizeof(_bcm_cosq_tcb_info_t), "tcb info");
    if (NULL == tcb_unit_info->tcb_info[buffer_id]) {
        (void)_bcm_hx5_cosq_tcb_deinit(unit);
        return BCM_E_MEMORY;
    }

    sal_memset(tcb_unit_info->tcb_info[buffer_id], 0, sizeof(_bcm_cosq_tcb_info_t));

    tcb_info = tcb_unit_info->tcb_info[buffer_id];
    tcb_info->profile_bmp =
            sal_alloc(SHR_BITALLOCSIZE(_BCM_HX5_TCB_PROFILE_MAX_NUM),
                      "profile_bitmap");
    if (NULL == tcb_info->profile_bmp) {
        (void)_bcm_hx5_cosq_tcb_deinit(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(tcb_info->profile_bmp, 0,
               SHR_BITALLOCSIZE(_BCM_HX5_TCB_PROFILE_MAX_NUM));

    if (!SOC_WARM_BOOT(unit)) {
        SHR_BITSET(tcb_info->profile_bmp, 0);
        profile_mem =
            SOC_MEM_UNIQUE_ACC(unit, TCB_THRESHOLD_PROFILE_MAPm)[buffer_id];
        tcb_info = tcb_unit_info->tcb_info[buffer_id];
        tcb_info->profile_ref_count[0] =
                    soc_mem_index_count(unit, profile_mem) * 10;
    }

    soc_hx5_tcb_set_hw_intr_cb(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_hx5_cosq_tcb_buffer_multi_get
 * Purpose:
 *      Get packet records in a specified TCB buffer.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) buffer id
 *      array_max - (IN) Number of entries to be retrieved
 *      buffer_array - (OUT) Pakcet record array
 *      array_count - (OUT) Number of packet records returned in buffer_array
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      When array_max is 0, buffer_array is NULL, array_count will return actural packet record
 *      count in the TCB buffer.
 */
int
bcm_hx5_cosq_tcb_buffer_multi_get(int unit,
                          bcm_cosq_buffer_id_t buffer_id, int count,
                          bcm_cosq_tcb_buffer_t *entries, int *num)
{
    int i_count = 0, status;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_buffer_multi_get: unit %d, buffer_id %d, count %d\n"),
                         unit, buffer_id, count));

    TCB_ID_CHECK(buffer_id);

    if (count < 0) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit, "count %d is illegal\n"), count));
        return BCM_E_PARAM;
    }

    if((count > 0) && (NULL == entries)) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit, "entries is null\n")));
        return BCM_E_PARAM;
    }

    if (NULL == num) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit, "num is null\n")));
        return BCM_E_PARAM;
    }

    /* event buffer can only be read in freeze status */
    BCM_IF_ERROR_RETURN(_bcm_hx5_cosq_tcb_current_phase_get(unit, buffer_id, &status));
    if (_TCB_FROZEN_PHASE != status) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit,
                              "tcb buffer can only be read in freeze status "
                              "current status %d\n"),
                              status));
        return BCM_E_BUSY;
    }

    BCM_IF_ERROR_RETURN(_bcm_hx5_cosq_tcb_buffer_get_rd_num(unit, buffer_id, num));

    if((0 == *num) || (0 == count)) {
        return BCM_E_NONE;
    }

    if(count < *num) {
        i_count = count;
        *num = count;
    } else {
        i_count = *num;
    }
    BCM_IF_ERROR_RETURN(_bcm_hx5_cosq_tcb_metadata_read(unit, buffer_id, i_count,
                                                   entries));
    BCM_IF_ERROR_RETURN(_bcm_hx5_cosq_tcb_celldata_read(unit, buffer_id, i_count,
                                                   entries));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_hx5_cosq_tcb_event_multi_get
 * Purpose:
 *      Get drop event records in a specified TCB buffer.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      buffer_id - (IN) buffer id
 *      array_max - (IN) Number of entries to be retrieved
 *      event_array - (OUT) Drop event record array
 *      array_count - (OUT) Number of drop event records returned in event_array
 *      overflow_count - (OUT) Drop event count exceed the event buffer.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      When array_max is 0, event_array is NULL, array_count will return actural drop event
 *      record count in the TCB buffer.
 */
int
bcm_hx5_cosq_tcb_event_multi_get(int unit,
                            bcm_cosq_buffer_id_t buffer_id, int count,
                            bcm_cosq_tcb_event_t *entries, int *num,
                            int *overflow_num)
{
    int i_count = 0;
    int status;

    LOG_INFO(BSL_LS_BCM_COSQ,
             (BSL_META_U(unit,
                         "tcb_event_multi_get: unit %d, buffer_id %d, count %d\n"),
                         unit, buffer_id, count));

    TCB_ID_CHECK(buffer_id);

    if (count < 0) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit, "count %d is illegal\n"), count));
        return BCM_E_PARAM;
    }

    if((count > 0) && (NULL == entries)) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit, "entries is null\n")));
        return BCM_E_PARAM;
    }

    if (NULL == num) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit, "num is null\n")));
        return BCM_E_PARAM;
    }

    if (NULL == overflow_num) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit, "overflow is null\n")));
        return BCM_E_PARAM;
    }

    /* event buffer can only be read in freeze status */
    BCM_IF_ERROR_RETURN(_bcm_hx5_cosq_tcb_current_phase_get(unit, buffer_id, &status));
    if (_TCB_FROZEN_PHASE != status) {
        LOG_ERROR(BSL_LS_BCM_COSQ,
                  (BSL_META_U(unit,
                              "event buffer can only be read in freeze status "
                              "current status %d\n"),
                              status));
        return BCM_E_BUSY;
    }

    BCM_IF_ERROR_RETURN(_bcm_hx5_cosq_tcb_buffer_info_get(unit, buffer_id,
                                                     _TCB_BUF_STATUS_EVT_NUM, num));
    BCM_IF_ERROR_RETURN(_bcm_hx5_cosq_tcb_evt_overflow_count_get(unit, buffer_id,
                                                            overflow_num));

    if ((0 == *num) || (0 == count)) {
        return BCM_E_NONE;
    }

    if (count < *num) {
        i_count = count;
        *num = count;
    } else {
        i_count = *num;
    }
    BCM_IF_ERROR_RETURN(_bcm_hx5_cosq_tcb_event_buffer_read(unit, buffer_id,
                                                            i_count, entries));

    return BCM_E_NONE;
}

#endif /* INCLUDE_TCB && BCM_TCB_SUPPORT */
