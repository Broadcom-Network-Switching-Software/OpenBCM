/*! \file bcmcth_mon_flowtracker_drv.c
 *
 * Flowtracker driver APIs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <shr/shr_ha.h>

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmbd/bcmbd_mcs.h>
#include <bcmbd/bcmbd_mcs.h>
#include <shr/shr_util_pack.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmpkt/flexhdr/bcmpkt_rxpmd_flex.h>
#include <bcmpkt/bcmpkt_dev.h>
#include <bcmpkt/bcmpkt_rxpmd.h>
#include <bcmcth/bcmcth_mon_drv.h>
#include <bcmcth/bcmcth_mon_rx_flex.h>
#include <bcmcth/bcmcth_mon_flowtracker_drv.h>
#include <bcmcth/bcmcth_mon_flowtracker_int.h>
#include <bcmcth/bcmcth_mon_flowtracker_pack.h>
#include <bcmcth/bcmcth_mon_flowtracker_static.h>
#include <bcmbd/mcs_shared/mcs_mon_flowtracker_msg.h>
#include <bcmissu/issu_api.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MON

#define FT_COMPILER_64_TO_32_LO(dst, src)  ((dst) = (uint32_t) (src))
#define FT_COMPILER_64_TO_32_HI(dst, src)  ((dst) = (uint32_t) ((src) >> 32))

/* Invalid collector Id, implies collector is being deleted. */
#define FT_INVALID_COLLECTOR_ID 0XFFFFFFFF

/* Invalid export profile id, implies export profile is being deleted. */
#define FT_INVALID_EXPORT_PROFILE_ID 0XFFFFFFFF

/* Default enterprise number. */
#define FT_DEFAULT_ENTERPRISE_NUMBER 0

static bcmcth_mon_ft_info_t *ft_info[BCMDRD_CONFIG_MAX_UNITS] = {0};

bcmcth_mon_ft_info_t * bcmcth_mon_ft_info_get(int unit)
{
    if (ft_info[unit]) {
        return ft_info[unit];
    }
    return NULL;
}

int bcmcth_mon_ft_sw_resources_alloc(int unit, bool warm)
{
    bcmcth_mon_ft_info_t *info = NULL;
    uint32_t ha_alloc_size = 0,ha_req_size = 0;
    bool is_sim = FALSE;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(ft_info[unit], sizeof(bcmcth_mon_ft_info_t), "bcmcthMonFtInfo");
    SHR_NULL_CHECK(ft_info[unit], SHR_E_MEMORY);
    sal_memset(ft_info[unit], 0, sizeof(bcmcth_mon_ft_info_t));
    info = ft_info[unit];

    /* Alloc the HA flowtracker info SW structure for this unit */
    ha_alloc_size = sizeof(bcmcth_mon_ft_ha_t);
    ha_req_size = ha_alloc_size;
    info->ha = shr_ha_mem_alloc(unit, BCMMGMT_MON_COMP_ID,
                                BCMMON_FLOWTRACKER_SUB_COMP_ID,
                                "flowtrackerHaCtrl",
                                &ha_alloc_size);

    SHR_NULL_CHECK(info->ha, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((ha_alloc_size < ha_req_size) ? SHR_E_MEMORY : SHR_E_NONE);

    if (!warm) {
        sal_memset(info->ha, 0, ha_alloc_size);

        SHR_IF_ERR_VERBOSE_EXIT(
                bcmissu_struct_info_report(unit, BCMMGMT_MON_COMP_ID,
                    BCMMON_FLOWTRACKER_SUB_COMP_ID, 0,
                    ha_req_size, 1,
                    BCMCTH_MON_FT_HA_T_ID));

    }

    for (i=0; i < FT_EM_GRP_ID_MAP_SIZE; i++) {
        info->ha->em_group_id_map[i]= FT_INVALID_EM_GROUP_ID;
    }

    is_sim = bcmdrd_feature_is_sim(unit);
    if (!is_sim) {
        /*
         * Allocate DMA buffer
         *
         * DMA buffer will be used to send and receive 'long' messages
         * between SDK Host and uController (BTE).
         */
        info->dma_buffer = bcmdrd_hal_dma_alloc(unit,
                                                sizeof(mcs_ft_msg_ctrl_t),
                                                "bcmcthMonFlowTracker",
                                                &(info->dma_buffer_p_addr));
        if (!info->dma_buffer) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }
        sal_memset(info->dma_buffer, 0, sizeof(mcs_ft_msg_ctrl_t));
    }

    /* Allocate the static flows database */
    SHR_IF_ERR_VERBOSE_EXIT(
        bcmcth_mon_flowtracker_static_flows_db_init(unit, warm));

 exit:
    SHR_FUNC_EXIT();
}

void
bcmcth_mon_ft_uc_free(int unit)
{
    int rv;
    if (ft_info[unit] == NULL) {
        return;
    }
    if (ft_info[unit]->dma_buffer != NULL) {
        bcmdrd_hal_dma_free(unit, sizeof(mcs_ft_msg_ctrl_t),
                            ft_info[unit]->dma_buffer,
                            ft_info[unit]->dma_buffer_p_addr);
    }

    rv = bcmcth_mon_flowtracker_static_flows_db_free(unit);
    if (SHR_FAILURE(rv)) {
           LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Unit%d:\
                    bcmcth_mon_flowtracker_static_flows_db_free failed\n"),
                    unit));
    }

    SHR_FREE(ft_info[unit]);
}



static int
bcmcth_mon_ft_util_control_defaults_get(int unit, bcmcth_mon_ft_control_t *entry)
{
    bcmlrd_client_field_info_t *fields_info = NULL;
    uint32_t field_count = 0;
    size_t actual_fld_num;
    size_t idx;
    int i = 0;
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;

    SHR_FUNC_ENTER(unit);
    ft_drv = bcmcth_mon_ft_drv_get(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_mon_flowtracker_control_out_fields_count(unit, &field_count));

    SHR_ALLOC(fields_info, sizeof(bcmlrd_client_field_info_t) * field_count,
              "bcmcthMonFtControlFldInfo");
    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
       (bcmlrd_table_fields_def_get(unit, "MON_FLOWTRACKER_CONTROL",
                                    field_count, fields_info, &actual_fld_num));

    for (idx = 0; idx < actual_fld_num; idx++) {
        if (SHR_BITGET(entry->fbmp, fields_info[idx].id)) {
            /* The field is already configured, don't overwrite with default. */
            continue;
        }

        switch (fields_info[idx].id) {
            case MON_FLOWTRACKER_CONTROLt_FLOWTRACKERf:
                entry->flowtracker =  fields_info[idx].def.is_true;
                break;

            case MON_FLOWTRACKER_CONTROLt_OBSERVATION_DOMAINf:
                entry->observation_domain = fields_info[idx].def.u32;
                break;

            case MON_FLOWTRACKER_CONTROLt_MAX_GROUPSf:
                entry->max_groups = fields_info[idx].def.u32;
                break;

            case MON_FLOWTRACKER_CONTROLt_MAX_EXPORT_LENGTHf:
                entry->max_export_length = fields_info[idx].def.u32;
                break;

            case MON_FLOWTRACKER_CONTROLt_MAX_FLOWSf:
                for (i = 0; i < ft_drv->num_pipes; i++) {
                    entry->max_flows[i] = fields_info[idx].def.u32;
                }
                break;

            case MON_FLOWTRACKER_CONTROLt_PKT_BYTE_CTR_ING_EFLEX_ACTION_PROFILE_IDf:
                entry->ctr_ing_flex_action_profile_id =
                                                    fields_info[idx].def.u16;
                break;
            case MON_FLOWTRACKER_CONTROLt_DOUBLE_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_ACTION_PROFILE_IDf:
                entry->db_ewide_hb_ctr_ing_flex_action_profile_id =
                                                    fields_info[idx].def.u16;
                break;
            case MON_FLOWTRACKER_CONTROLt_QUAD_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_ACTION_PROFILE_IDf:
                entry->qd_ewide_hb_ctr_ing_flex_action_profile_id =
                                                    fields_info[idx].def.u16;
                break;
            case MON_FLOWTRACKER_CONTROLt_DOUBLE_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_GROUP_ACTIONf:
                entry->db_ewide_hb_ctr_ing_flex_grp_action =
                                                    fields_info[idx].def.u16;
                break;
            case MON_FLOWTRACKER_CONTROLt_QUAD_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_GROUP_ACTIONf:
                entry->qd_ewide_hb_ctr_ing_flex_grp_action =
                                                    fields_info[idx].def.u16;
                break;

            case MON_FLOWTRACKER_CONTROLt_ETRAPf:
                entry->etrap = fields_info[idx].def.is_true;
                break;

            case MON_FLOWTRACKER_CONTROLt_SCAN_INTERVAL_USECSf:
                entry->scan_interval_usecs = fields_info[idx].def.u32;
                break;

            case MON_FLOWTRACKER_CONTROLt_FLOW_START_TIMESTAMP_ENABLEf:
                entry->flow_start_timestamp_enable =
                                                fields_info[idx].def.is_true;
                break;
            case MON_FLOWTRACKER_CONTROLt_HOST_MEMf:
                entry->host_mem = fields_info[idx].def.u16;
                break;
            case MON_FLOWTRACKER_CONTROLt_HARDWARE_LEARNf:
                entry->hw_learn = fields_info[idx].def.u16;
                break;
            case MON_FLOWTRACKER_CONTROLt_MAX_GROUPS_OPERf:
            case MON_FLOWTRACKER_CONTROLt_MAX_EXPORT_LENGTH_OPERf:
            case MON_FLOWTRACKER_CONTROLt_MAX_FLOWS_OPERf:
            case MON_FLOWTRACKER_CONTROLt_PKT_BYTE_CTR_ING_EFLEX_ACTION_PROFILE_ID_OPERf:
            case MON_FLOWTRACKER_CONTROLt_DOUBLE_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_ACTION_PROFILE_ID_OPERf:
            case MON_FLOWTRACKER_CONTROLt_QUAD_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_ACTION_PROFILE_ID_OPERf:
            case MON_FLOWTRACKER_CONTROLt_DOUBLE_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_GROUP_ACTION_OPERf:
            case MON_FLOWTRACKER_CONTROLt_QUAD_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_GROUP_ACTION_OPERf:
            case MON_FLOWTRACKER_CONTROLt_ETRAP_OPERf:
            case MON_FLOWTRACKER_CONTROLt_SCAN_INTERVAL_USECS_OPERf:
            case MON_FLOWTRACKER_CONTROLt_FLOW_START_TIMESTAMP_ENABLE_OPERf:
            case MON_FLOWTRACKER_CONTROLt_HOST_MEM_OPERf:
            case MON_FLOWTRACKER_CONTROLt_HARDWARE_LEARN_OPERf:
            case MON_FLOWTRACKER_CONTROLt_OPERATIONAL_STATEf:
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FREE(fields_info);
    SHR_FUNC_EXIT();
}

/* Do a field by field copy into src entry. Only copy those fields which are set
 * in the field bitmap of dst.
 */
static void
bcmcth_mon_ft_util_control_fields_copy(
                        int unit,
                        bcmcth_mon_ft_control_t *src,
                        bcmcth_mon_ft_control_t *dst)
{
    int i;
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;

    ft_drv = bcmcth_mon_ft_drv_get(unit);

    /* Copy the field bitmap of dst into src. */
    sal_memcpy(&(src->fbmp), &(dst->fbmp), sizeof(src->fbmp));
    sal_memcpy(&(src->fbmp_maxflows), &(dst->fbmp_maxflows),
               sizeof(src->fbmp_maxflows));
    if (SHR_BITGET(dst->fbmp,
                   MON_FLOWTRACKER_CONTROLt_FLOWTRACKERf)) {
        src->flowtracker = dst->flowtracker;
    }
    if (SHR_BITGET(dst->fbmp,
                   MON_FLOWTRACKER_CONTROLt_OBSERVATION_DOMAINf)) {
        src->observation_domain = dst->observation_domain;
    }
    if (SHR_BITGET(dst->fbmp,
            MON_FLOWTRACKER_CONTROLt_MAX_GROUPSf)) {
        src->max_groups = dst->max_groups;
    }
    if (SHR_BITGET(dst->fbmp,
            MON_FLOWTRACKER_CONTROLt_MAX_EXPORT_LENGTHf)) {
        src->max_export_length = dst->max_export_length;
    }
    if (SHR_BITGET(dst->fbmp,
            MON_FLOWTRACKER_CONTROLt_MAX_FLOWSf)) {
        for (i = 0; i < ft_drv->num_pipes; i++) {
            if (SHR_BITSET(dst->fbmp_maxflows, i)) {
                src->max_flows[i] = dst->max_flows[i];
            }
        }
    }
    if (SHR_BITGET(dst->fbmp,
            MON_FLOWTRACKER_CONTROLt_PKT_BYTE_CTR_ING_EFLEX_ACTION_PROFILE_IDf)) {
        src->ctr_ing_flex_action_profile_id =
                                        dst->ctr_ing_flex_action_profile_id;
    }
    if (SHR_BITGET(dst->fbmp,
            MON_FLOWTRACKER_CONTROLt_DOUBLE_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_ACTION_PROFILE_IDf)) {
        src->db_ewide_hb_ctr_ing_flex_action_profile_id =
                                dst->db_ewide_hb_ctr_ing_flex_action_profile_id;
    }
    if (SHR_BITGET(dst->fbmp,
            MON_FLOWTRACKER_CONTROLt_QUAD_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_ACTION_PROFILE_IDf)) {
        src->qd_ewide_hb_ctr_ing_flex_action_profile_id =
                                dst->qd_ewide_hb_ctr_ing_flex_action_profile_id;
    }
    if (SHR_BITGET(dst->fbmp,
            MON_FLOWTRACKER_CONTROLt_DOUBLE_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_GROUP_ACTIONf)) {
        src->db_ewide_hb_ctr_ing_flex_grp_action =
                                dst->db_ewide_hb_ctr_ing_flex_grp_action;
    }
    if (SHR_BITGET(dst->fbmp,
            MON_FLOWTRACKER_CONTROLt_QUAD_ENTRY_WIDE_HITBIT_CTR_ING_EFLEX_GROUP_ACTIONf)) {
        src->qd_ewide_hb_ctr_ing_flex_grp_action =
                                dst->qd_ewide_hb_ctr_ing_flex_grp_action;
    }
    if (SHR_BITGET(dst->fbmp,
            MON_FLOWTRACKER_CONTROLt_ETRAPf)) {
        src->etrap = dst->etrap;
    }
    if (SHR_BITGET(dst->fbmp,
            MON_FLOWTRACKER_CONTROLt_SCAN_INTERVAL_USECSf)) {
        src->scan_interval_usecs = dst->scan_interval_usecs;
    }
    if (SHR_BITGET(dst->fbmp,
            MON_FLOWTRACKER_CONTROLt_HOST_MEMf)) {
        src->host_mem = dst->host_mem;
    }
    if (SHR_BITGET(dst->fbmp,
            MON_FLOWTRACKER_CONTROLt_FLOW_START_TIMESTAMP_ENABLEf)) {
        src->flow_start_timestamp_enable = dst->flow_start_timestamp_enable;
    }

    if (SHR_BITGET(dst->fbmp,
            MON_FLOWTRACKER_CONTROLt_HARDWARE_LEARNf)) {
        src->hw_learn = dst->hw_learn;
    }
}

static int
bcmcth_mon_ft_uc_shutdown(int unit)
{
    bcmcth_mon_ft_info_t *info;

    SHR_FUNC_ENTER(unit);

    info = bcmcth_mon_ft_info_get(unit);

    if (info->ha->init == false) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_eapp_shutdown(unit, info->ha->uc_num));
    info->ha->init = false;
    info->ha->run = false;

 exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_hw_learn_opt_msg_fill(int unit,
                                    bcmcth_mon_ft_control_t *entry,
                                    mcs_ft_msg_ctrl_hw_learn_opt_msg_t *msg)
{
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    SHR_FUNC_ENTER(unit);

    ft_drv = bcmcth_mon_ft_drv_get(unit);

    /* Call chip specific msg fill routine */
    if (ft_drv && ft_drv->ft_hw_learn_opt_msg_init) {
        SHR_IF_ERR_VERBOSE_EXIT(
            ft_drv->ft_hw_learn_opt_msg_init(unit, entry, msg));
    }
 exit:
    SHR_FUNC_EXIT();
}

/*
 * From a bitmap get the start offset and width in bits which are set to 1. In
 * case multiple chunks are set, the offset and width of the first chunk is
 * returned.
 */
static void
bmp_offset_width_get(uint32_t *bmp, uint16_t num_bits, int *offset, int *width)
{
    int wp, bp;
    int i;

    *offset = -1;
    *width  = 0;

    for (i = 0; i < num_bits; i++) {
        wp = i / 32;
        bp = i % 32;

        if ((bmp[wp] & (1 << bp)) != 0) {
            if (*offset == -1) {
                *offset = i;
            }
            (*width)++;
        } else {
            if (*offset != -1) {
                break;
            }
        }
    }
}

static int
bcmcth_mon_ft_rxpmd_flex_format_msg_fill(
                                int unit,
                                mcs_ft_msg_ctrl_rxpmd_flex_format_t *msg)
{
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    int fids[MCS_FT_RXPMD_FLEX_FIELD_COUNT];
    int uc_fids[MCS_FT_RXPMD_FLEX_FIELD_COUNT];
    int num_fids, i;
    uint32_t rxpmd_flex_ref[BCMPKT_RXPMD_SIZE_WORDS];
    uint32_t rxpmd_flex_mask[BCMPKT_RXPMD_SIZE_WORDS];
    bcmlrd_variant_t variant;
    uint32_t val;
    int offset, width;
    int rv, profile, profile_count = 0, profile_idx = 0;
    SHR_BITDCL *prof_bmp = NULL;
    bcmdrd_dev_type_t dev_type;
    uint32_t flexdata_len_words;
    uint32_t flexdata_len_bytes;
    uint32_t *flexdata_addr;

    SHR_FUNC_ENTER(unit);

    ft_drv = bcmcth_mon_ft_drv_get(unit);
    if ((ft_drv == NULL) || (ft_drv->ft_rxpmd_flex_fids_get == NULL)) {
        SHR_EXIT();
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (ft_drv->ft_rxpmd_flex_fids_get(unit, MCS_FT_RXPMD_FLEX_FIELD_COUNT,
                                        &num_fids, fids, uc_fids));

    sal_memset(msg, 0, sizeof(mcs_ft_msg_ctrl_rxpmd_flex_format_t));

    variant = bcmlrd_variant_get(unit);
    if (variant == BCMLRD_VARIANT_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }
    SHR_IF_ERR_EXIT(bcmpkt_dev_type_get(unit, &dev_type));
    SHR_IF_ERR_EXIT(
        bcmpkt_rxpmd_flexdata_get(dev_type, rxpmd_flex_ref,
                                  &flexdata_addr, &flexdata_len_words));
    flexdata_len_bytes = flexdata_len_words * 4;

    sal_memset(rxpmd_flex_ref, 0xFF, BCMPKT_RXPMD_SIZE_BYTES);

    SHR_ALLOC(prof_bmp,
            SHR_BITALLOCSIZE(MCS_FT_RXPMD_FLEX_PROFILES_COUNT),
            "bcmcthMonFtRxPmdProfilesBmp");
    sal_memset(prof_bmp, 0, SHR_BITALLOCSIZE(MCS_FT_RXPMD_FLEX_PROFILES_COUNT));
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_rx_flex_profiles_get(variant,prof_bmp));


    SHR_BITCOUNT_RANGE(prof_bmp, profile_count, 0,
                       MCS_FT_RXPMD_FLEX_PROFILES_COUNT);


    SHR_BIT_ITER(prof_bmp, MCS_FT_RXPMD_FLEX_PROFILES_COUNT, profile) {

        for (i = 0; i < num_fids; i++) {
            sal_memset(rxpmd_flex_mask, 0, BCMPKT_RXPMD_SIZE_BYTES);

            /* Get mask value */
            rv = bcmpkt_rxpmd_flex_field_get(variant, rxpmd_flex_ref, profile,
                                             fids[i], &val);
            if (rv != SHR_E_NONE) {
                continue;
            }


            /* Set field mask */
            rv = bcmpkt_rxpmd_flex_field_set(variant, rxpmd_flex_mask,
                                             profile, fids[i], val);
            if (rv != SHR_E_NONE) {
                continue;
            }

            bmp_offset_width_get(rxpmd_flex_mask,
                                 flexdata_len_bytes * 8,
                                 &offset, &width);
            msg->fids[i] = uc_fids[i];
            msg->profiles[profile_idx] = profile;
            msg->offsets[uc_fids[i]][profile_idx] = offset;
            msg->widths[uc_fids[i]][profile_idx] = width;
        }
        profile_idx++;
    }

    msg->num_fields   = num_fids;
    msg->num_profiles = profile_count;

 exit:
    if (prof_bmp != NULL) {
        SHR_FREE(prof_bmp);
    }
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_match_id_msg_fill(int unit, mcs_ft_msg_ctrl_match_id_set_t *msg)
{
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    const bcmlrd_match_id_db_t *info;
    const char *str = NULL;
    int i = 0;

    SHR_FUNC_ENTER(unit);

    ft_drv = bcmcth_mon_ft_drv_get(unit);
    if ((ft_drv == NULL) || (ft_drv->match_id_string_get == NULL)) {
        SHR_EXIT();
    }

    for (i = 0; i < MCS_SHR_FT_MATCH_ID_FIELD_COUNT; i++) {
        str = ft_drv->match_id_string_get(unit, i);
        if (!str) {
            continue;
        }

        SHR_IF_ERR_EXIT
            (bcmlrd_table_match_id_data_get(unit, str, &info));

        msg->fields[msg->num_fields]  = i;
        msg->offsets[msg->num_fields] = info->minbit + info->match_minbit;
        msg->widths[msg->num_fields]  = info->maxbit - info->minbit + 1;
        msg->values[msg->num_fields]  = info->value;
        msg->num_fields++;
    }

    sal_memset(&msg, 0, sizeof(msg));

 exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_uc_init(int unit, bcmcth_mon_ft_control_t *entry)
{
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    mcs_ft_msg_ctrl_init_t msg;
    mcs_ft_msg_ctrl_em_bank_uft_info_t uft_msg;
    mcs_ft_msg_ctrl_sdk_features_t feature_msg;
    mcs_ft_msg_ctrl_hw_learn_opt_msg_t hw_opt_msg;
    mcs_ft_msg_ctrl_rxpmd_flex_format_t rxpmd_msg;
    mcs_ft_msg_ctrl_match_id_set_t match_id_msg;
    bcmcth_mon_ft_info_t *info;
    int uc, num_uc;
    int rv, i;
    uint32_t uc_msg_version = 0;

    SHR_FUNC_ENTER(unit);

    ft_drv = bcmcth_mon_ft_drv_get(unit);
    SHR_NULL_CHECK(ft_drv, SHR_E_UNAVAIL);

    info = bcmcth_mon_ft_info_get(unit);

    info->cfg_mode = MCS_SHR_FT_CFG_MODE_V2;

    if (bcmdrd_feature_is_sim(unit) == true) {
        /* Don't attempt initialization in SIM, simply return without error.
         * Since the init flag is not set, subsequent messages will not be sent
         * to app.
         */
        SHR_EXIT();
    }

    /* Shutdown the application if it is already loaded */
    if (info->ha->init) {
        SHR_IF_ERR_CONT
            (bcmcth_mon_ft_uc_shutdown(unit));
        info->ha->init = false;
        info->ha->run = false;
    }

    /* Init the app, determine which uC is running the app by chooosing
     * the first uC that returns sucessfully to the APPL_INIT message.
     */
    num_uc = bcmbd_mcs_num_uc(unit);
    for (uc = 0; uc < num_uc ; uc++) {
        rv = bcmcth_mon_ft_uc_appl_init(unit, uc, &uc_msg_version);
        if (rv == SHR_E_NONE) {
            break;
        }
    }
    if (uc == num_uc) {
        entry->oper.operational_state =
            BCMLTD_COMMON_MON_FLOWTRACKER_CONTROL_STATE_T_T_APP_NOT_INITIALIZED;
        SHR_EXIT();
    }

    info->ha->uc_num = uc;
    info->uc_msg_version = uc_msg_version;

    /* Set control message data */
    sal_memset(&msg, 0, sizeof(msg));

    msg.max_groups = entry->max_groups;
    msg.num_pipes = ft_drv->num_pipes;
    for (i = 0; i < msg.num_pipes; i++) {
        msg.pipe[i].max_flows = entry->max_flows[i];
    }
    msg.max_export_pkt_length = entry->max_export_length;
    msg.cur_time_secs = sal_time();
    msg.export_interval_msecs = FT_DEFAULT_EXPORT_INTERVAL_MSECS;
    msg.scan_interval_usecs = entry->scan_interval_usecs;
    msg.age_timer_tick_msecs = FT_AGING_INTERVAL_STEP_MSECS;
    msg.num_elph_profiles = FT_MAX_ELEPHANT_PROFILES;

    if (entry->flow_start_timestamp_enable) {
        msg.flags |= MCS_SHR_FT_MSG_INIT_FLAGS_FLOW_START_TS;
    }
    if (entry->host_mem) {
        msg.flags |= MCS_SHR_FT_MSG_INIT_FLAGS_HOSTMEM_ENABLE;
    }
    if (entry->etrap) {
        msg.flags |= MCS_SHR_FT_MSG_INIT_FLAGS_ELEPHANT;
    }
    msg.hw_learn = entry->hw_learn;

    ft_drv = bcmcth_mon_ft_drv_get(unit);
    if (ft_drv && ft_drv->ft_control_init) {
        ft_drv->ft_control_init(unit, entry, &msg);
    }

    if (entry->oper.operational_state !=
        BCMLTD_COMMON_MON_FLOWTRACKER_CONTROL_STATE_T_T_SUCCESS) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_eapp_ctrl_init(unit, &msg));

    feature_msg.cfg_mode = info->cfg_mode;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_eapp_features_exchange(unit , &feature_msg));

   /* Check if soc properties configured aligns with the app features */
    if (info->eleph_mode &&
        !(info->uc_features & MCS_SHR_FT_UC_FEATURE_ELEPH)) {
        entry->oper.operational_state =
            BCMLTD_COMMON_MON_FLOWTRACKER_CONTROL_STATE_T_T_ETRAP_FEATURE_NOT_SUPPORTED;
        SHR_EXIT();
    }

    /* Send the RXPMD flex fields data */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_rxpmd_flex_format_msg_fill(
                                                        unit, &rxpmd_msg));
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_rxmpd_flex_format_set(
                                                        unit, &rxpmd_msg));

    if (ft_drv && ft_drv->ft_em_uft_init) {
        ft_drv->ft_em_uft_init(unit, &uft_msg, entry->hw_learn);
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_eapp_uft_bank_info(unit, &uft_msg));

    if (FLOWTRACKER_HW_LEARN_ENABLED(entry->hw_learn)) {
        SHR_IF_ERR_VERBOSE_EXIT(
                bcmcth_mon_ft_hw_learn_opt_msg_fill(unit, entry, &hw_opt_msg));
        SHR_IF_ERR_VERBOSE_EXIT(
                bcmcth_mon_ft_hw_learn_opt_init(unit, &hw_opt_msg));
    }

    /* Send the match ID information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_match_id_msg_fill(unit, &match_id_msg));
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_match_id_set(unit, &match_id_msg));

    info->ha->init = true;
    info->ha->run = true;

 exit:
    SHR_FUNC_EXIT();
}

static void
bcmcth_mon_ft_util_control_oper_fields_set(
                            int unit,
                            bcmcth_mon_ft_control_t *entry,
                            bcmcth_mon_ft_control_oper_fields_t *oper)
{
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    int i;
    ft_drv = bcmcth_mon_ft_drv_get(unit);

    if (entry->flowtracker == false) {
        /* Do not set oper fields if feature is disabled. */
        return;
    }

    for (i = 0; i < ft_drv->num_pipes; i++) {
        oper->max_flows[i] = entry->max_flows[i];
    }
    if (!FLOWTRACKER_HW_LEARN_WO_EAPP_ENABLED(entry->hw_learn)) {
        oper->max_groups = entry->max_groups;
        oper->max_export_length = entry->max_export_length;
        oper->ctr_ing_flex_action_profile_id = entry->ctr_ing_flex_action_profile_id;
        oper->db_ewide_hb_ctr_ing_flex_action_profile_id = entry->db_ewide_hb_ctr_ing_flex_action_profile_id;
        oper->qd_ewide_hb_ctr_ing_flex_action_profile_id = entry->qd_ewide_hb_ctr_ing_flex_action_profile_id;
        oper->db_ewide_hb_ctr_ing_flex_grp_action = entry->db_ewide_hb_ctr_ing_flex_grp_action;
        oper->qd_ewide_hb_ctr_ing_flex_grp_action = entry->qd_ewide_hb_ctr_ing_flex_grp_action;
        oper->etrap = entry->etrap;
        oper->host_mem = entry->host_mem;
        oper->flow_start_timestamp_enable = entry->flow_start_timestamp_enable;
        oper->scan_interval_usecs = entry->scan_interval_usecs;
    }
    oper->hw_learn = entry->hw_learn;
}

int
bcmcth_mon_ft_control_delete(int unit)
{
    bcmcth_mon_ft_info_t *info;
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    int i;

    SHR_FUNC_ENTER(unit);

    info = bcmcth_mon_ft_info_get(unit);

    if (FLOWTRACKER_HW_LEARN_ENABLED(info->ha->hw_learn)) {
        /* De-init the HW learning */
        ft_drv = bcmcth_mon_ft_drv_get(unit);
        if (ft_drv && ft_drv->ft_hw_learn_deinit) {
            ft_drv->ft_hw_learn_deinit(unit);
        }
    }

    if (info->ha->run == true) {
        /* Shutdown the app */
        SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_uc_shutdown(unit));
    }

    /* Clear the FT info */
    sal_memset(info->ha, 0, sizeof(bcmcth_mon_ft_ha_t));
    for (i=0; i < FT_EM_GRP_ID_MAP_SIZE; i++) {
        info->ha->em_group_id_map[i]= FT_INVALID_EM_GROUP_ID;
    }


exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_control_validate(int unit,
                               bcmcth_mon_ft_control_t *entry,
                               bcmcth_mon_ft_control_oper_fields_t *oper,
                               bool *valid)
{
    uint32_t size = 0;
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    SHR_FUNC_ENTER(unit);

    *valid = true;

    if (!FLOWTRACKER_HW_LEARN_WO_EAPP_ENABLED(entry->hw_learn)) {
        if ((entry->max_export_length < FLOWTRACKER_MIN_EXPORT_LENGTH) ||
                (entry->max_export_length > FLOWTRACKER_MAX_EXPORT_LENGTH)) {
            oper->operational_state =
                BCMLTD_COMMON_MON_FLOWTRACKER_CONTROL_STATE_T_T_INVALID_MAX_EXPORT_LENGTH;
            *valid = false;
            SHR_EXIT();
        }

        if (entry->scan_interval_usecs < FLOWTRACKER_DEF_SCAN_INTERVAL_USECS) {
            oper->operational_state =
                BCMLTD_COMMON_MON_FLOWTRACKER_CONTROL_STATE_T_T_INVALID_SCAN_INTERVAL_USECS;
            *valid = false;
            SHR_EXIT();
        }

        if ((entry->scan_interval_usecs % FLOWTRACKER_SCAN_INTERVAL_STEP_USECS) != 0) {
            oper->operational_state =
                BCMLTD_COMMON_MON_FLOWTRACKER_CONTROL_STATE_T_T_INVALID_SCAN_INTERVAL_USECS;
            *valid = false;
            SHR_EXIT();
        }
    }

    if (FLOWTRACKER_HW_LEARN_WITH_EAPP_ENABLED(entry->hw_learn)) {
        ft_drv = bcmcth_mon_ft_drv_get(unit);

        if (ft_drv && ft_drv->ft_hw_learn_max_grps_get) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ft_drv->ft_hw_learn_max_grps_get(unit, &size));
        }
        if (entry->max_groups > size) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_ft_control_insert(int unit,
                             bcmcth_mon_ft_control_t *insert,
                             bcmcth_mon_ft_control_oper_fields_t *oper)
{
    bcmcth_mon_ft_info_t *info;
    bcmcth_mon_ft_control_t entry;
    bool valid = true;
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    SHR_FUNC_ENTER(unit);

    info = bcmcth_mon_ft_info_get(unit);


    /* Initializations */
    sal_memset(oper, 0, sizeof(*oper));
    sal_memset(&entry, 0, sizeof(entry));

    /* Get the defaults for the fields that are not configured. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_util_control_defaults_get(unit, &entry));
    /* Overwrite the default values with the fields being inserted. */
    bcmcth_mon_ft_util_control_fields_copy(unit, &entry, insert);

    /* Validate the entry and set oper state if applicable */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_control_validate(unit, &entry, oper, &valid));
    if (valid == false) {
        SHR_EXIT();
    }
    ft_drv = bcmcth_mon_ft_drv_get(unit);

    if (entry.flowtracker == true) {
        /* Initialize the app */
        if (!FLOWTRACKER_HW_LEARN_WO_EAPP_ENABLED(entry.hw_learn)) {
            SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_uc_init(unit, &entry));
            if (entry.oper.operational_state !=
                    BCMLTD_COMMON_MON_FLOWTRACKER_CONTROL_STATE_T_T_SUCCESS) {
                oper->operational_state = entry.oper.operational_state;
                SHR_EXIT();
            }
        }
        if (FLOWTRACKER_HW_LEARN_ENABLED(entry.hw_learn)) {
            /* Enable the HW learning */
            if (ft_drv && ft_drv->ft_hw_learn_init) {
                ft_drv->ft_hw_learn_init(unit, &entry);
            }
        }
    }

    if ((info->ha->run == true) ||
        (FLOWTRACKER_HW_LEARN_WO_EAPP_ENABLED(entry.hw_learn))) {
        /* The app has succesfully initialized */
        /* Set the oper field values */
        bcmcth_mon_ft_util_control_oper_fields_set(unit, &entry, oper);
    } else {
        oper->operational_state = BCMLTD_COMMON_MON_FLOWTRACKER_CONTROL_STATE_T_T_APP_NOT_INITIALIZED;
    }

    /* Store the hw learn mechanism in HA. Its used when deleting the entry */
    info->ha->hw_learn = oper->hw_learn;

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_control_run_time_config_update(int unit,
                                  bcmcth_mon_ft_control_t *entry)
{
    bcmcth_mon_ft_info_t *info;

    SHR_FUNC_ENTER(unit);

    info = bcmcth_mon_ft_info_get(unit);

    if (info->ha->run == false) {
        /* App is not running, return */
        SHR_EXIT();
    }


    if (SHR_BITGET(entry->fbmp, MON_FLOWTRACKER_CONTROLt_OBSERVATION_DOMAINf)) {
        SHR_IF_ERR_VERBOSE_EXIT(
            bcmcth_mon_ft_eapp_observation_domain_update(unit,
                                entry->observation_domain));
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_ft_control_update(int unit,
                              bcmcth_mon_ft_control_t *cur,
                              bcmcth_mon_ft_control_t *update,
                              bcmcth_mon_ft_control_oper_fields_t *oper)
{
    bcmcth_mon_ft_info_t *info;
    bcmcth_mon_ft_control_t old_entry, entry;
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;

    SHR_FUNC_ENTER(unit);

    info = bcmcth_mon_ft_info_get(unit);
    /* Initializations */
    sal_memcpy(oper, &(cur->oper), sizeof(*oper));
    sal_memset(&old_entry, 0, sizeof(old_entry));
    sal_memset(&entry, 0, sizeof(entry));
    /* Get the defaults for the fields that are not configured. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_util_control_defaults_get(unit, &old_entry));
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_util_control_defaults_get(unit, &entry));

    /* Overwrite the default fields with the values that are already configured
     * in IMM.
     */
    bcmcth_mon_ft_util_control_fields_copy(unit, &old_entry, cur);
    bcmcth_mon_ft_util_control_fields_copy(unit, &entry, cur);

    /* Overwrite the default/old fields with the new values that are being
     * configured.
     */
    bcmcth_mon_ft_util_control_fields_copy(unit, &entry, update);
    ft_drv = bcmcth_mon_ft_drv_get(unit);

    if (!FLOWTRACKER_HW_LEARN_WO_EAPP_ENABLED(old_entry.hw_learn) &&
        FLOWTRACKER_HW_LEARN_WO_EAPP_ENABLED(entry.hw_learn)) {
        /* Moving from SW_LEARN/HW_LEARN to HW_LEARN_WO_EAPP */
        if (info->ha->run == true) {
            /* App is already running. set oper and return */
            oper->operational_state = BCMLTD_COMMON_MON_FLOWTRACKER_CONTROL_STATE_T_T_APP_RUNNING;
            SHR_EXIT();
        } else {
            if (entry.flowtracker == true) {
                /* Enable the HW learning */
                if (ft_drv && ft_drv->ft_hw_learn_init) {
                    ft_drv->ft_hw_learn_init(unit, &entry);
                }
            } else {
                /* De-init the HW learning */
                if (ft_drv && ft_drv->ft_hw_learn_deinit) {
                    ft_drv->ft_hw_learn_deinit(unit);
                }
            }

            /* Set the oper fields */
            bcmcth_mon_ft_util_control_oper_fields_set(unit, &entry, oper);
            /* Set oper = SUCCESS and return */
            oper->operational_state = BCMLTD_COMMON_MON_FLOWTRACKER_CONTROL_STATE_T_T_SUCCESS;
            SHR_EXIT();
        }
    } else if (FLOWTRACKER_HW_LEARN_WO_EAPP_ENABLED(entry.hw_learn)) {
        /*
         * Moving from HW_LEARN_WO_EAPP to HW_LEARN_WO_EAPP.
         * Change in max_flows.
         * Update the global flow threshold based
         */
        if (sal_memcmp(old_entry.max_flows, entry.max_flows,
                                                sizeof(entry.max_flows))) {
            if (ft_drv && ft_drv->ft_hw_learn_update_glb_flow_threshold) {
                ft_drv->ft_hw_learn_update_glb_flow_threshold(unit, &entry);
            }
        }

        /* Flowtracker feature being enabled/disabled */
        if (entry.flowtracker != old_entry.flowtracker) {
            if (entry.flowtracker == true) {
                /* Enable hw learn */
                if (ft_drv && ft_drv->ft_hw_learn_init) {
                    ft_drv->ft_hw_learn_init(unit, &entry);
                }
            } else {
                /* De-init the HW learning */
                if (ft_drv && ft_drv->ft_hw_learn_deinit) {
                    ft_drv->ft_hw_learn_deinit(unit);
                }
            }
        }
        /* Set the oper fields */
        bcmcth_mon_ft_util_control_oper_fields_set(unit, &entry, oper);
        /* Set oper = SUCCESS and return */
        oper->operational_state = BCMLTD_COMMON_MON_FLOWTRACKER_CONTROL_STATE_T_T_SUCCESS;
        SHR_EXIT();
    }

    /*
     * old_entry now contains the view of the table entry as it currently exists
     * (Defaults + User configured). entry now contains the view of the table
     * entry as it should be once the update is complete.
     */

    if (entry.flowtracker != info->ha->init) {
        /*
         * The app's init status is toggling, check if it is init or shutdown.
         */
        if (entry.flowtracker == true) {
            /* App init requested */
            SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_uc_init(unit, &entry));

            /* Enable the HW learning */
            if (ft_drv && ft_drv->ft_hw_learn_init) {
                ft_drv->ft_hw_learn_init(unit, &entry);
            }

            if (info->ha->run == true) {
                /* Set the oper fields */
                bcmcth_mon_ft_util_control_oper_fields_set(unit, &entry, oper);

                /* The app has succesfully initialized, flip the corresponding
                 * oper state.
                 */
                oper->operational_state = BCMLTD_COMMON_MON_FLOWTRACKER_CONTROL_STATE_T_T_SUCCESS;
            }
        } else {
            /* De-init the HW learning */
            if (ft_drv && ft_drv->ft_hw_learn_deinit) {
                ft_drv->ft_hw_learn_deinit(unit);
            }
            /* Shutdown requested */
            SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_uc_shutdown(unit));

            /* Set the oper fields to 0 as the app is shutdown */
            sal_memset(oper,0,sizeof (bcmcth_mon_ft_control_oper_fields_t));

            /* App is not initalized anymore, set the corresponding
             * oper state
             */
            oper->operational_state = BCMLTD_COMMON_MON_FLOWTRACKER_CONTROL_STATE_T_T_APP_NOT_INITIALIZED;
        }
    }
    if (info->ha->run == true) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_control_run_time_config_update(unit, &entry));
    }

exit:
    SHR_FUNC_EXIT();
}


static int
bcmcth_mon_ft_uc_stats_get(int unit,
                bcmcth_mon_ft_stats_t *entry)
{
    mcs_ft_msg_ctrl_stats_age_t age;
    mcs_ft_msg_ctrl_stats_export_t export;
    mcs_ft_msg_ctrl_stats_learn_t learn;
    mcs_ft_msg_ctrl_stats_elph_t eleph;

    SHR_FUNC_ENTER(unit);

    sal_memset(&age, 0, sizeof(age));
    sal_memset(&export, 0, sizeof(export));
    sal_memset(&learn, 0, sizeof(learn));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_eapp_get_learn_stats(unit, &learn));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_eapp_get_export_stats(unit, &export));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_eapp_get_age_stats(unit, &age));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_eapp_get_elph_stats(unit, &eleph));

    entry->learn_pkt_discard_parse_err_cnt =
        learn.count[MCS_SHR_FT_LEARN_PKT_UNKNOWN_PIPE] +
        learn.count[MCS_SHR_FT_LEARN_PKT_RUNT] +
        learn.count[MCS_SHR_FT_LEARN_PKT_PARSE_ERROR] +
        learn.count[MCS_SHR_FT_LEARN_PKT_NON_IP] +
        learn.count[MCS_SHR_FT_LEARN_PKT_NON_IPV4];

    entry->learn_pkt_discard_invalid_grp_cnt =
        learn.count[MCS_SHR_FT_LEARN_PKT_GROUP_NOT_ENABLED] +
        learn.count[MCS_SHR_FT_LEARN_PKT_INVALID_GROUP];

    entry->learn_pkt_discard_flow_limit_exceed_cnt =
        learn.count[ MCS_SHR_FT_LEARN_PKT_GROUP_THRESHOLD];

    entry->learn_pkt_discard_pipe_limit_exceed_cnt =
        learn.count[MCS_SHR_FT_LEARN_PKT_PIPE_THRESHOLD];

    entry->learn_pkt_discard_em_fail_cnt =
        learn.count[MCS_SHR_FT_LEARN_PKT_EM_INSERT_FAIL];

    entry->learn_pkt_discard_duplicate_cnt =
        learn.count[MCS_SHR_FT_LEARN_PKT_DUPLICATE];

    entry->num_flows_learnt = learn.count[MCS_SHR_FT_LEARN_PKT_NEW_FLOW];

    entry->num_flow_exported = export.num_flows;

    entry->num_packets_exported = export.num_pkts;

    entry->num_flows_aged = age.num_flows;

    entry->num_elephant_flows =
        eleph.trans_count[MCS_SHR_FT_ELPH_FLOW_STATE_NEW][MCS_SHR_FT_ELPH_ACTION_PROMOTE];
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_ft_uc_stats_lookup(int unit, bcmcth_mon_ft_stats_t *entry)
{
    bcmcth_mon_ft_info_t *info = NULL;

    SHR_FUNC_ENTER(unit);

    info = bcmcth_mon_ft_info_get(unit);

    if (info->ha->run == false) {
        sal_memset(entry, 0, sizeof(*entry));
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_uc_stats_get(unit, entry));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_ft_uc_group_status_lookup(
    int unit,
    bcmcth_mon_ft_group_status_t *entry)
{
    bcmcth_mon_ft_info_t *info = NULL;
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    mcs_ft_msg_ctrl_group_get_t msg;

    SHR_FUNC_ENTER(unit);
    sal_memset(&msg, 0, sizeof(msg));

    info = bcmcth_mon_ft_info_get(unit);
    ft_drv = bcmcth_mon_ft_drv_get(unit);

    if (info->ha->run == false &&
            !FLOWTRACKER_HW_LEARN_WO_EAPP_ENABLED(info->ha->hw_learn)) {
        sal_memset(entry, 0, sizeof(*entry));
        SHR_EXIT();
    }

    if (!FLOWTRACKER_HW_LEARN_WO_EAPP_ENABLED(info->ha->hw_learn)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_eapp_flow_group_get(unit, entry->group_id,
                                               &entry->flow_count));
    } else {
        if (ft_drv && ft_drv->ft_grp_hw_learn_flow_cnt_get) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ft_drv->ft_grp_hw_learn_flow_cnt_get(unit, entry->group_id,
                                                      &entry->flow_count));
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static void
bcmcth_mon_ft_elph_filter_copy (bcmcth_mon_flowtracker_elephant_profile_t *entry,
        bcmcth_mon_flowtracker_elephant_filter_t *src,
        mcs_ft_msg_ctrl_elph_profile_filter_t *dst)
{
    FT_COMPILER_64_TO_32_HI(dst->size_threshold_bytes_upper_32,
                         src->size_threshold_bytes);
    FT_COMPILER_64_TO_32_LO(dst->size_threshold_bytes_lower_32,
                         src->size_threshold_bytes);
    dst->rate_low_threshold  = src->rate_low_threshold_kbits_sec / 80;
    dst->rate_high_threshold = src->rate_high_threshold_kbits_sec / 80;
    dst->scan_count = src->monitor_interval_usecs / entry->scan_interval_usecs;
    dst->incr_rate = src->incr_rate;
}


static int
bcmcth_mon_ft_elephant_profile_create(int unit,
        bcmcth_mon_flowtracker_elephant_profile_t *entry)
{
    mcs_ft_msg_ctrl_elph_profile_create_t msg;
    bcmcth_mon_ft_control_t ctrl_entry;

    SHR_FUNC_ENTER(unit);
    msg.num_filters = FLOWTRACKER_PROMO_FILTER_MAX + 1;
    msg.profile_idx = entry->mon_flowtracker_elephant_profile_id;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_control_entry_get(unit, &ctrl_entry));

    entry->scan_interval_usecs = ctrl_entry.scan_interval_usecs;

    bcmcth_mon_ft_elph_filter_copy(entry,&(entry->promotion_filters[0]),
            &(msg.filters[0]));

    bcmcth_mon_ft_elph_filter_copy(entry,&(entry->promotion_filters[1]),
            &(msg.filters[1]));

    bcmcth_mon_ft_elph_filter_copy(entry,&(entry->demotion_filter),
            &(msg.filters[2]));

    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_eapp_elph_profile_create(unit, &msg));

exit:
    SHR_FUNC_EXIT();
}



/* Do a field by field copy into src entry. Only copy those fields which are set
 * in the field bitmap of dst.
 */
static void
bcmcth_mon_ft_elephant_profile_fields_copy(bcmcth_mon_flowtracker_elephant_profile_t *src,
        bcmcth_mon_flowtracker_elephant_profile_t *dst)
{
    int i;
    /* Copy the field bitmap of dst into src. */
    sal_memcpy(&(src->fbmp), &(dst->fbmp), sizeof(src->fbmp));

    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_ELEPHANT_PROFILEt_MON_FLOWTRACKER_ELEPHANT_PROFILE_IDf)) {
        src->mon_flowtracker_elephant_profile_id =
                                    dst->mon_flowtracker_elephant_profile_id;
    }

    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_ELEPHANT_PROFILEt_NUM_PROMOTION_FILTERSf)) {
        src->num_promotion_filters = dst->num_promotion_filters;
    }

    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_ELEPHANT_PROFILEt_PROMOTION_FILTERSu_INCR_RATEf)) {
        for (i = 0; i < FLOWTRACKER_PROMO_FILTER_MAX; i++) {
            if (SHR_BITSET(dst->fbmp_pro_filters, i)) {
                src->promotion_filters[i].incr_rate =
                                        dst->promotion_filters[i].incr_rate;
            }
        }
    }

    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_ELEPHANT_PROFILEt_PROMOTION_FILTERSu_MONITOR_INTERVAL_USECSf)) {
        for (i = 0; i < FLOWTRACKER_PROMO_FILTER_MAX; i++) {
            if (SHR_BITSET(dst->fbmp_pro_filters, i)) {
                src->promotion_filters[i].monitor_interval_usecs =
                            dst->promotion_filters[i].monitor_interval_usecs;
            }
        }
    }

    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_ELEPHANT_PROFILEt_PROMOTION_FILTERSu_RATE_HIGH_THRESHOLD_KBITS_SECf)) {
        for (i = 0; i < FLOWTRACKER_PROMO_FILTER_MAX; i++) {
            if (SHR_BITSET(dst->fbmp_pro_filters, i)) {
                src->promotion_filters[i].rate_high_threshold_kbits_sec =
                    dst->promotion_filters[i].rate_high_threshold_kbits_sec;
            }
        }
    }

    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_ELEPHANT_PROFILEt_PROMOTION_FILTERSu_RATE_LOW_THRESHOLD_KBITS_SECf)) {
        for (i = 0; i < FLOWTRACKER_PROMO_FILTER_MAX; i++) {
            if (SHR_BITSET(dst->fbmp_pro_filters, i)) {
                src->promotion_filters[i].rate_low_threshold_kbits_sec =
                    dst->promotion_filters[i].rate_low_threshold_kbits_sec;
            }
        }
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_ELEPHANT_PROFILEt_PROMOTION_FILTERSu_SIZE_THRESHOLD_BYTESf)) {
        for (i = 0; i < FLOWTRACKER_PROMO_FILTER_MAX; i++) {
            if (SHR_BITSET(dst->fbmp_pro_filters, i)) {
                src->promotion_filters[i].size_threshold_bytes =
                    dst->promotion_filters[i].rate_low_threshold_kbits_sec;
            }
        }
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_ELEPHANT_PROFILEt_DEMOTION_FILTERu_MONITOR_INTERVAL_USECSf)) {
        src->demotion_filter.monitor_interval_usecs =
                                dst->demotion_filter.monitor_interval_usecs;
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_ELEPHANT_PROFILEt_DEMOTION_FILTERu_RATE_HIGH_THRESHOLD_KBITS_SECf)) {
        src->demotion_filter.rate_high_threshold_kbits_sec =
                        dst->demotion_filter.rate_high_threshold_kbits_sec;
    }

    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_ELEPHANT_PROFILEt_DEMOTION_FILTERu_RATE_LOW_THRESHOLD_KBITS_SECf)) {
        src->demotion_filter.rate_low_threshold_kbits_sec =
                        dst->demotion_filter.rate_low_threshold_kbits_sec;
    }

    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_ELEPHANT_PROFILEt_DEMOTION_FILTERu_INCR_RATEf)) {
        src->demotion_filter.incr_rate = dst->demotion_filter.incr_rate;
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_ELEPHANT_PROFILEt_DEMOTION_FILTERu_SIZE_THRESHOLD_BYTESf)) {
        src->demotion_filter.size_threshold_bytes =
                                    dst->demotion_filter.size_threshold_bytes;
    }
}

static int
bcmcth_mon_ft_elephant_profile_defaults_get(int unit,
                bcmcth_mon_flowtracker_elephant_profile_t *entry)
{
    bcmlrd_client_field_info_t *fields_info = NULL;
    size_t fld_num = MON_FLOWTRACKER_ELEPHANT_PROFILEt_FIELD_COUNT;
    size_t actual_fld_num;
    size_t idx;
    int i = 0;

    SHR_FUNC_ENTER(unit);


    SHR_ALLOC(fields_info, sizeof(bcmlrd_client_field_info_t) * fld_num,
            "bcmcthMonFtElephantProfile");
    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmlrd_table_fields_def_get(unit, "MON_FLOWTRACKER_ELEPHANT_PROFILE",
                                     fld_num, fields_info, &actual_fld_num));

    for (idx = 0; idx < actual_fld_num; idx++) {
        if (SHR_BITGET(entry->fbmp, fields_info[idx].id)) {
            /* The field is already configured, don't overwrite with default. */
            continue;
        }

        switch (fields_info[idx].id) {
            case MON_FLOWTRACKER_ELEPHANT_PROFILEt_MON_FLOWTRACKER_ELEPHANT_PROFILE_IDf:
                entry->mon_flowtracker_elephant_profile_id =
                                                fields_info[idx].def.u32;
                break;
            case MON_FLOWTRACKER_ELEPHANT_PROFILEt_NUM_PROMOTION_FILTERSf:
                entry->num_promotion_filters = fields_info[idx].def.u8;
                break;
            case MON_FLOWTRACKER_ELEPHANT_PROFILEt_PROMOTION_FILTERSu_INCR_RATEf:
                for (i = 0; i < FLOWTRACKER_PROMO_FILTER_MAX; i++) {
                    entry->promotion_filters[i].incr_rate =
                                                    fields_info[idx].def.is_true;
                }
                break;
            case MON_FLOWTRACKER_ELEPHANT_PROFILEt_PROMOTION_FILTERSu_MONITOR_INTERVAL_USECSf:
                for (i = 0; i < FLOWTRACKER_PROMO_FILTER_MAX; i++) {
                    entry->promotion_filters[i].monitor_interval_usecs =
                                                        fields_info[idx].def.u32;
                }
                break;
            case MON_FLOWTRACKER_ELEPHANT_PROFILEt_PROMOTION_FILTERSu_RATE_HIGH_THRESHOLD_KBITS_SECf:
                for (i = 0; i < FLOWTRACKER_PROMO_FILTER_MAX; i++) {
                    entry->promotion_filters[i].rate_high_threshold_kbits_sec =
                                                        fields_info[idx].def.u32;
                }
                break;
            case MON_FLOWTRACKER_ELEPHANT_PROFILEt_PROMOTION_FILTERSu_RATE_LOW_THRESHOLD_KBITS_SECf:
                for (i = 0; i < FLOWTRACKER_PROMO_FILTER_MAX; i++) {
                    entry->promotion_filters[i].rate_low_threshold_kbits_sec =
                                                        fields_info[idx].def.u32;
                }
                break;
            case MON_FLOWTRACKER_ELEPHANT_PROFILEt_PROMOTION_FILTERSu_SIZE_THRESHOLD_BYTESf:
                for (i = 0; i < FLOWTRACKER_PROMO_FILTER_MAX; i++) {
                    entry->promotion_filters[i].size_threshold_bytes =
                                                        fields_info[idx].def.u64;
                }
                break;
            case MON_FLOWTRACKER_ELEPHANT_PROFILEt_DEMOTION_FILTERu_MONITOR_INTERVAL_USECSf:
                entry->demotion_filter.monitor_interval_usecs =
                                                    fields_info[idx].def.u32;
                break;
            case MON_FLOWTRACKER_ELEPHANT_PROFILEt_DEMOTION_FILTERu_RATE_HIGH_THRESHOLD_KBITS_SECf:
                entry->demotion_filter.rate_high_threshold_kbits_sec =
                                                    fields_info[idx].def.u32;
                break;
            case MON_FLOWTRACKER_ELEPHANT_PROFILEt_DEMOTION_FILTERu_RATE_LOW_THRESHOLD_KBITS_SECf:
                entry->demotion_filter.rate_low_threshold_kbits_sec =
                                                    fields_info[idx].def.u32;
                break;
            case MON_FLOWTRACKER_ELEPHANT_PROFILEt_DEMOTION_FILTERu_SIZE_THRESHOLD_BYTESf:
                entry->demotion_filter.size_threshold_bytes =
                                                    fields_info[idx].def.u64;
                break;
            case MON_FLOWTRACKER_ELEPHANT_PROFILEt_DEMOTION_FILTERu_INCR_RATEf:
                entry->demotion_filter.incr_rate =
                                                fields_info[idx].def.is_true;
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FREE(fields_info);
    SHR_FUNC_EXIT();
}


int
bcmcth_mon_ft_elephant_profile_insert(int unit,
        bcmcth_mon_flowtracker_elephant_profile_t *insert)
{
    bcmcth_mon_ft_info_t *info;
    bcmcth_mon_flowtracker_elephant_profile_t entry;

    SHR_FUNC_ENTER(unit);

    info = bcmcth_mon_ft_info_get(unit);

    if (info->ha->run == false) {
        /* App is not running, return */
        SHR_EXIT();
    }

    /* Initializations */
    sal_memset(&entry, 0, sizeof(entry));

    /* Get the defaults for the fields that are not configured. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_elephant_profile_defaults_get(unit, &entry));

    /* Overwrite the default values with the fields being inserted. */
    bcmcth_mon_ft_elephant_profile_fields_copy(&entry, insert);

    /* The app has succesfully initialized */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_elephant_profile_create(unit, &entry));

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_ft_elephant_profile_delete(int unit,
        bcmcth_mon_flowtracker_elephant_profile_t *entry)
{
    bcmcth_mon_ft_info_t *info;

    SHR_FUNC_ENTER(unit);
    info = bcmcth_mon_ft_info_get(unit);

    if (info->ha->run == false) {
        /* App is not running, return */
        SHR_EXIT();
    }
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_eapp_eleph_profile_delete(
            unit,
            entry->mon_flowtracker_elephant_profile_id));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_group_util_defaults_get(int unit, bcmcth_mon_ft_group_t *entry)
{
    bcmlrd_client_field_info_t *fields_info = NULL;
    size_t actual_fld_num;
    size_t idx;
    uint32_t field_count = 0;
    int i = 0;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmcth_mon_flowtracker_group_out_fields_count(unit, &field_count));

    SHR_ALLOC(fields_info, sizeof(bcmlrd_client_field_info_t) * field_count,
            "bcmcthMonFtGroupFldInfo");
    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmlrd_table_fields_def_get(unit, "MON_FLOWTRACKER_GROUP",
                                     field_count, fields_info, &actual_fld_num));

    for (idx = 0; idx < actual_fld_num; idx++) {
        if (SHR_BITGET(entry->fbmp, fields_info[idx].id)) {
            /* The field is already configured, don't overwrite with default. */
            continue;
        }
        switch (fields_info[idx].id) {
            case MON_FLOWTRACKER_GROUPt_MON_FLOWTRACKER_GROUP_IDf:
                entry->mon_flowtracker_group_id = fields_info[idx].def.u32;
                break;
            case MON_FLOWTRACKER_GROUPt_LEARNf:
                entry->learn = fields_info[idx].def.is_true;
                break;
            case MON_FLOWTRACKER_GROUPt_DT_EM_GRP_TEMPLATE_IDf:
                entry->dt_em_grp_template_id = fields_info[idx].def.u32;
                break;
            case MON_FLOWTRACKER_GROUPt_NUM_TRACKING_PARAMETERSf:
                entry->num_tp = fields_info[idx].def.u32;
                break;
            case MON_FLOWTRACKER_GROUPt_TRACKING_PARAMETERSu_TYPEf:
                for (i = 0; i < FLOWTRACKER_TRACKING_PARAMETERS_MAX; i++) {
                    entry->tp[i].type = fields_info[idx].def.u16;
                }
                break;
            case MON_FLOWTRACKER_GROUPt_TRACKING_PARAMETERSu_UDF_POLICY_IDf:
                for (i = 0; i < FLOWTRACKER_TRACKING_PARAMETERS_MAX; i++) {
                    entry->tp[i].udf_policy_id = fields_info[idx].def.u16;
                }
                break;
            case MON_FLOWTRACKER_GROUPt_FLOW_LIMITf:
                entry->flow_limit = fields_info[idx].def.u32;
                break;
            case MON_FLOWTRACKER_GROUPt_AGING_INTERVAL_MSf:
                entry->aging_interval_ms = fields_info[idx].def.u32;
                break;
            case MON_FLOWTRACKER_GROUPt_NUM_EXPORT_TRIGGERSf:
                entry->num_export_triggers = fields_info[idx].def.u8;
                break;
            case MON_FLOWTRACKER_GROUPt_EXPORT_TRIGGERSf:
                for (i = 0; i < FLOWTRACKER_EXPORT_TRIGGERS_MAX; i++) {
                    entry->export_triggers[i] = fields_info[idx].def.u16;
                }
                break;
            case MON_FLOWTRACKER_GROUPt_NUM_ACTIONSf:
                entry->num_actions = fields_info[idx].def.u32;
                break;
            case MON_FLOWTRACKER_GROUPt_ACTIONSu_TYPEf:
                for (i = 0; i < FLOWTRACKER_ACTIONS_MAX; i++) {
                    entry->actions[i].type = fields_info[idx].def.u16;
                }
                break;
            case MON_FLOWTRACKER_GROUPt_ACTIONSu_EM_FT_OPAQUE_OBJ0_VALf:
                for (i = 0; i < FLOWTRACKER_ACTIONS_MAX; i++) {
                    entry->actions[i].em_ft_opaque_obj0_val = fields_info[idx].def.u16;
                }
                break;
            case MON_FLOWTRACKER_GROUPt_ACTIONSu_EM_FT_IOAM_GBP_ACTION_VALf:
                for (i = 0; i < FLOWTRACKER_ACTIONS_MAX; i++) {
                    entry->actions[i].em_ft_ioam_gbp_action_val = fields_info[idx].def.u8;
                }
                break;
            case MON_FLOWTRACKER_GROUPt_ACTIONSu_EM_FT_COPY_TO_CPU_VALf:
                for (i = 0; i < FLOWTRACKER_ACTIONS_MAX; i++) {
                    entry->actions[i].em_ft_copy_to_cpu_val = fields_info[idx].def.u8;
                }
                break;
            case MON_FLOWTRACKER_GROUPt_ACTIONSu_EM_FT_DROP_ACTION_VALf:
                for (i = 0; i < FLOWTRACKER_ACTIONS_MAX; i++) {
                    entry->actions[i].em_ft_drop_action_val = fields_info[idx].def.u8;
                }
                break;
            case MON_FLOWTRACKER_GROUPt_ACTIONSu_DESTINATION_VALf:
                for (i = 0; i < FLOWTRACKER_ACTIONS_MAX; i++) {
                    entry->actions[i].destination_val = fields_info[idx].def.u16;
                }
                break;
            case MON_FLOWTRACKER_GROUPt_ACTIONSu_DESTINATION_TYPE_VALf:
                for (i = 0; i < FLOWTRACKER_ACTIONS_MAX; i++) {
                    entry->actions[i].destination_type_val = fields_info[idx].def.u8;
                }
                break;
            case MON_FLOWTRACKER_GROUPt_ACTIONSu_EM_FT_FLEX_STATE_ACTION_VALf:
                for (i = 0; i < FLOWTRACKER_ACTIONS_MAX; i++) {
                    entry->actions[i].em_ft_flex_state_action_val = fields_info[idx].def.u8;
                }
                break;
            case MON_FLOWTRACKER_GROUPt_ACTIONSu_FLEX_CTR_ACTION_VALf:
                for (i = 0; i < FLOWTRACKER_ACTIONS_MAX; i++) {
                    entry->actions[i].flex_ctr_action_val = fields_info[idx].def.u8;
                }
                break;
            case MON_FLOWTRACKER_GROUPt_ACTIONSu_PKT_FLOW_ELIGIBILITY_VALf:
                for (i = 0; i < FLOWTRACKER_ACTIONS_MAX; i++) {
                    entry->actions[i].pkt_flow_eligibility_val = fields_info[idx].def.u8;
                }
                break;
            case MON_FLOWTRACKER_GROUPt_ACTIONSu_L2_IIF_SVP_MIRROR_INDEX_0_VALf:
                for (i = 0; i < FLOWTRACKER_ACTIONS_MAX; i++) {
                    entry->actions[i].l2_iif_svp_mirror_index_0_val = fields_info[idx].def.u8;
                }
                break;
            case MON_FLOWTRACKER_GROUPt_MON_FLOWTRACKER_ELEPHANT_PROFILE_IDf:
                entry->mon_flowtracker_elephant_profile_id = fields_info[idx].def.u32;
                break;
            case MON_FLOWTRACKER_GROUPt_OPERATIONAL_STATEf:
                /* Ignore dynamically calculated fields. */
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
exit:
    SHR_FREE(fields_info);
    SHR_FUNC_EXIT();
}

static void
bcmcth_mon_ft_group_util_fields_copy(bcmcth_mon_ft_group_t *src,
        bcmcth_mon_ft_group_t *dst)
{
    uint32_t i = 0;

    /* Copy the field bitmap of dst into src. */
    sal_memcpy(&(src->fbmp), &(dst->fbmp), sizeof(src->fbmp));

    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUPt_MON_FLOWTRACKER_GROUP_IDf)) {
        src->mon_flowtracker_group_id = dst->mon_flowtracker_group_id;
    }
    if (SHR_BITGET(dst->fbmp, MON_FLOWTRACKER_GROUPt_LEARNf)) {
        src->learn = dst->learn;
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUPt_DT_EM_GRP_TEMPLATE_IDf)) {
        src->dt_em_grp_template_id = dst->dt_em_grp_template_id;
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUPt_NUM_TRACKING_PARAMETERSf)) {
        src->num_tp = dst->num_tp;
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUPt_TRACKING_PARAMETERSu_TYPEf)) {
        for (i = 0; i < src->num_tp; i++) {
            src->tp[i].type = dst->tp[i].type;
        }
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUPt_TRACKING_PARAMETERSu_UDF_POLICY_IDf)) {
        for (i = 0; i < src->num_tp; i++) {
            src->tp[i].udf_policy_id = dst->tp[i].udf_policy_id;
        }
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUPt_FLOW_LIMITf)) {
        src->flow_limit = dst->flow_limit;
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUPt_AGING_INTERVAL_MSf)) {
        src->aging_interval_ms = dst->aging_interval_ms;
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUPt_NUM_EXPORT_TRIGGERSf)) {
        src->num_export_triggers = dst->num_export_triggers;
    }

    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUPt_NUM_ACTIONSf)) {
        src->num_actions = dst->num_actions;
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUPt_ACTIONSu_TYPEf)) {
        for (i = 0; i < src->num_actions; i++) {
            src->actions[i].type = dst->actions[i].type;
        }
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUPt_ACTIONSu_EM_FT_OPAQUE_OBJ0_VALf)) {
        for (i = 0; i < src->num_actions; i++) {
            src->actions[i].em_ft_opaque_obj0_val =
                                        dst->actions[i].em_ft_opaque_obj0_val;
        }
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUPt_ACTIONSu_EM_FT_IOAM_GBP_ACTION_VALf)) {
        for (i = 0; i < src->num_actions; i++) {
            src->actions[i].em_ft_ioam_gbp_action_val =
                                        dst->actions[i].em_ft_ioam_gbp_action_val;
        }
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUPt_ACTIONSu_EM_FT_COPY_TO_CPU_VALf)) {
        for (i = 0; i < src->num_actions; i++) {
            src->actions[i].em_ft_copy_to_cpu_val =
                                        dst->actions[i].em_ft_copy_to_cpu_val;
        }
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUPt_ACTIONSu_EM_FT_DROP_ACTION_VALf)) {
        for (i = 0; i < src->num_actions; i++) {
            src->actions[i].em_ft_drop_action_val =
                                        dst->actions[i].em_ft_drop_action_val;
        }
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUPt_ACTIONSu_DESTINATION_VALf)) {
        for (i = 0; i < src->num_actions; i++) {
            src->actions[i].destination_val =
                                        dst->actions[i].destination_val;
        }
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUPt_ACTIONSu_DESTINATION_TYPE_VALf)) {
        for (i = 0; i < src->num_actions; i++) {
            src->actions[i].destination_type_val =
                                        dst->actions[i].destination_type_val;
        }
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUPt_ACTIONSu_EM_FT_FLEX_STATE_ACTION_VALf)) {
        for (i = 0; i < src->num_actions; i++) {
            src->actions[i].em_ft_flex_state_action_val =
                                        dst->actions[i].em_ft_flex_state_action_val;
        }
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUPt_ACTIONSu_FLEX_CTR_ACTION_VALf)) {
        for (i = 0; i < src->num_actions; i++) {
            src->actions[i].flex_ctr_action_val =
                                        dst->actions[i].flex_ctr_action_val;
        }
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUPt_ACTIONSu_PKT_FLOW_ELIGIBILITY_VALf)) {
        for (i = 0; i < src->num_actions; i++) {
            src->actions[i].pkt_flow_eligibility_val =
                                        dst->actions[i].pkt_flow_eligibility_val;
        }
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUPt_ACTIONSu_L2_IIF_SVP_MIRROR_INDEX_0_VALf)) {
        for (i = 0; i < src->num_actions; i++) {
            src->actions[i].l2_iif_svp_mirror_index_0_val =
                                        dst->actions[i].l2_iif_svp_mirror_index_0_val;
        }
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUPt_EXPORT_TRIGGERSf)) {
        for (i = 0; i < src->num_export_triggers; i++) {
            src->export_triggers[i] = dst->export_triggers[i];
        }
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUPt_MON_FLOWTRACKER_ELEPHANT_PROFILE_IDf)) {
        src->mon_flowtracker_elephant_profile_id =
                                    dst->mon_flowtracker_elephant_profile_id;
    }
}

static int
bcmcth_mon_ft_uc_group_create(int unit,
        bcmcth_mon_ft_group_t *entry)
{
    mcs_ft_msg_ctrl_group_create_t msg={0};
    uint32_t    fid = 0, sid = 0, k = 0;
    bcmltd_fields_t     in_flds = {0}, out_flds = {0};
    const bcmltd_field_t *gen_field;
    uint32_t rv = 0;
    bcmcth_mon_ft_info_t *info;
    uint32_t field_count = 0;

    SHR_FUNC_ENTER(unit);

    info = bcmcth_mon_ft_info_get(unit);

    if (info->ha->run == false) {
        /* App is not running, return */
        SHR_EXIT();
    }
    msg.aging_interval_msecs = entry->aging_interval_ms;
    msg.flow_limit = entry->flow_limit;
    msg.group_idx = entry->mon_flowtracker_group_id;

    sid = MON_FLOWTRACKER_CONTROLt;
    in_flds.count = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_flowtracker_control_out_fields_count(unit, &field_count));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_util_fields_alloc(unit,
                                         field_count,
                                         &out_flds));

    if (bcmimm_entry_lookup(unit, sid, &in_flds, &out_flds) != SHR_E_NONE) {
        rv = SHR_E_INTERNAL;
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    for (k = 0; k < out_flds.count; k++) {
        fid = out_flds.field[k]->id;
        if (fid == MON_FLOWTRACKER_CONTROLt_OBSERVATION_DOMAINf) {
            gen_field = out_flds.field[k];
            msg.domain_id = gen_field->data;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_eapp_flow_group_create(unit, &msg));

exit:
    bcmcth_mon_ft_util_fields_free(unit, field_count, &out_flds);
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_ft_group_insert(int unit,
        bcmcth_mon_ft_group_t *insert)
{
    bcmcth_mon_ft_info_t *info;
    bcmcth_mon_ft_group_t entry;
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    mcs_ft_msg_ctrl_flex_em_group_create_t flex_msg;
    bcmcth_mon_ft_control_t ctrl_entry;
    mcs_ft_msg_ctrl_group_update_t msg;
    int ctrl_rv = SHR_E_NONE;
    
    uint32_t pdd_index = 0;
    uint32_t sbr_index = 0;
    uint8_t  data_format = FLOWTRACKER_HW_LEARN_DATA_FMT_DATA_ONLY;
    int i = 0, j = 0;
    int index = 0;
    int em_group_id_map_status = 0;
    int em_group_idx = 0;

    SHR_FUNC_ENTER(unit);


    ft_drv = bcmcth_mon_ft_drv_get(unit);
    info = bcmcth_mon_ft_info_get(unit);

    /* Initializations */
    sal_memset(&entry, 0, sizeof(entry));
    sal_memset(&flex_msg, 0, sizeof(flex_msg));

    /* Get the defaults for the fields that are not configured. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_group_util_defaults_get(unit, &entry));

    /* Overwrite the default values with the fields being inserted. */
    bcmcth_mon_ft_group_util_fields_copy(&entry, insert);

    if (info->ha->run == true) {
        /* The app has succesfully initialized */
        if (insert->learn) {
            SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_uc_group_create(unit,
                        &entry));
        } else {
            SHR_EXIT();
        }
    } else if (!FLOWTRACKER_HW_LEARN_WO_EAPP_ENABLED(info->ha->hw_learn)) {
        /* Set the oper field values */
        insert->oper =
            BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_STATE_T_T_APP_NOT_INITIALIZED;
        SHR_EXIT();
    }

    if (insert->num_tp && insert->dt_em_grp_template_id &&
        !FLOWTRACKER_HW_LEARN_WO_EAPP_ENABLED(info->ha->hw_learn)) {
        for (i = 0; i < MCS_SHR_FT_MAX_EM_GROUPS; ++i) {
            for (j = 0; j < ft_drv->num_pipes; ++j) {
                index = (i * MCS_SHR_FT_MAX_EM_GROUPS) + j;
                if (info->ha->em_group_id_map[index] == entry.dt_em_grp_template_id) {
                    em_group_id_map_status = 1;
                    em_group_idx = i;
                    break;
                }
            }
        }
        if (em_group_id_map_status) {
            msg.update = MCS_SHR_FT_GROUP_UPDATE_EM_GROUP_IDX;
            msg.param0 = entry.dt_em_grp_template_id;
            SHR_IF_ERR_EXIT
                (bcmcth_mon_ft_eapp_flow_group_update(unit, &msg));
        } else {
            for (i = 0; i < MCS_SHR_FT_MAX_EM_GROUPS; ++i) {
                index = (i * ft_drv->num_pipes);
                if (info->ha->em_group_id_map[index] == FT_INVALID_EM_GROUP_ID) {
                    for (j = 0; j < ft_drv->num_pipes; ++j) {
                        index = (i * MCS_SHR_FT_MAX_EM_GROUPS) + j;
                        info->ha->em_group_id_map[index] = entry.dt_em_grp_template_id;
                    }
                    if (ft_drv && ft_drv->ft_grp_init) {
                        SHR_IF_ERR_VERBOSE_EXIT
                            (ft_drv->ft_grp_init(unit, &entry, &flex_msg, &pdd_index));
                        if (entry.oper !=
                                BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_STATE_T_T_SUCCESS) {
                            insert->oper = entry.oper;
                            SHR_EXIT();
                        }
                    }

                    flex_msg.group_idx = em_group_idx;
                    /* Send em group information */
                    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_eapp_flex_em_group_create(unit,
                                &flex_msg));
                    msg.update = MCS_SHR_FT_GROUP_UPDATE_EM_GROUP_IDX;
                    msg.param0 = em_group_idx;
                    SHR_IF_ERR_EXIT
                        (bcmcth_mon_ft_eapp_flow_group_update(unit, &msg));
                    break;
                }
            }
            if (i >= MCS_SHR_FT_MAX_EM_GROUPS) {
                LOG_ERROR(BSL_LOG_MODULE,
                        (BSL_META_U(unit,
                                    "FT(unit %d) Error: Maximum number of EM Groups created.\n"),
                         unit));
                SHR_ERR_EXIT(SHR_E_RESOURCE);
            }
        }

        /* Hardware init*/
        sal_memset(&ctrl_entry, 0 ,sizeof(bcmcth_mon_ft_control_t));
        ctrl_rv = bcmcth_mon_ft_control_entry_get(unit, &ctrl_entry);
        if (ctrl_rv == SHR_E_NONE) {
            /* Check if Hw learn is set to true */
            if (FLOWTRACKER_HW_LEARN_ENABLED(info->ha->hw_learn)) {
                /* Set FT HW learn configurations in HW */
                if (ft_drv && ft_drv->ft_grp_hw_learn_config) {
                    SHR_IF_ERR_VERBOSE_EXIT(
                            (ft_drv->ft_grp_hw_learn_config(unit,
                                                            &ctrl_entry,
                                                            &entry,
                                                            &flex_msg,
                                                            pdd_index,
                                                            sbr_index,
                                                            data_format)));
                }
            }
        }
    }

    if (FLOWTRACKER_HW_LEARN_WO_EAPP_ENABLED(info->ha->hw_learn)) {
        /* Set FT HW learn configurations in HW */
        if (ft_drv && ft_drv->ft_grp_hw_learn_config_wo_eapp) {
            SHR_IF_ERR_VERBOSE_EXIT(
                    (ft_drv->ft_grp_hw_learn_config_wo_eapp(unit,
                                                    &entry)));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_ft_group_delete(int unit, uint32_t grp_id)
{
    bcmcth_mon_ft_info_t *info;
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    SHR_FUNC_ENTER(unit);

    info = bcmcth_mon_ft_info_get(unit);
    ft_drv = bcmcth_mon_ft_drv_get(unit);

    /* Delete the HW group config */
    if (FLOWTRACKER_HW_LEARN_ENABLED(info->ha->hw_learn)) {
        if (ft_drv && ft_drv->ft_grp_hw_learn_deinit) {
            SHR_IF_ERR_VERBOSE_EXIT(
                    (ft_drv->ft_grp_hw_learn_deinit(unit,
                                                    grp_id)));
        }
    }

    if (info->ha->run == true) {
        /* Delete the group */
        SHR_IF_ERR_VERBOSE_EXIT(
                bcmcth_mon_ft_eapp_flow_group_delete(unit, grp_id));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_ft_group_update(int unit,
        bcmcth_mon_ft_group_t *cur,
        bcmcth_mon_ft_group_t *update)
{
    bcmcth_mon_ft_info_t *info;
    bcmcth_mon_ft_group_t old_entry, entry;
    mcs_ft_msg_ctrl_group_update_t msg;
    uint32_t export_trigger = 0;
    mcs_ft_msg_ctrl_flex_em_group_create_t flex_msg;
    bcmcth_mon_flowtracker_drv_t *ft_drv = NULL;
    mcs_ft_msg_ctrl_group_actions_set_t action_msg;
    int rv = SHR_E_NONE;
    
    uint32_t pdd_index = 0;
    uint32_t sbr_index = 0;
    uint8_t  data_format = FLOWTRACKER_HW_LEARN_DATA_FMT_DATA_ONLY;
    bcmcth_mon_ft_control_t ctrl_entry;
    int em_group_id_map_status = 0;
    int em_group_idx = 0;
    uint32_t index = 0;
    uint32_t i = 0, j = 0;
    bool tp_set = false;

    SHR_FUNC_ENTER(unit);

    info = bcmcth_mon_ft_info_get(unit);

    if (info->ha->run == false &&
        !FLOWTRACKER_HW_LEARN_WO_EAPP_ENABLED(info->ha->hw_learn)) {
        update->oper =
            BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_STATE_T_T_APP_NOT_INITIALIZED;
        SHR_EXIT();
    }

    /* Initializations */
    sal_memset(&old_entry, 0, sizeof(old_entry));
    sal_memset(&entry, 0, sizeof(entry));
    sal_memset(&action_msg, 0 , sizeof(action_msg));
    sal_memset(&flex_msg, 0, sizeof(flex_msg));

    /* Get the defaults for the fields that are not configured. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_group_util_defaults_get(unit,
                &old_entry));
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_group_util_defaults_get(unit,
                &entry));

    /* Overwrite the default fields with the values that are already configured
     * in IMM.
     */
    bcmcth_mon_ft_group_util_fields_copy(&old_entry, cur);
    bcmcth_mon_ft_group_util_fields_copy(&entry, cur);

    /* Overwrite the default/old fields with the new values that are being
     * configured.
     */
    bcmcth_mon_ft_group_util_fields_copy(&entry, update);
    ft_drv = bcmcth_mon_ft_drv_get(unit);

    if (!FLOWTRACKER_HW_LEARN_WO_EAPP_ENABLED(info->ha->hw_learn)) {

        /* old_entry now contains the view of the table entry as it currently exists
         * (Defaults + User configured). entry now contains the view of the table
         * entry as it should be once the update is complete.
         */

        if ((old_entry.learn == 0) && (entry.learn == 1)) {
            SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_uc_group_create(unit, &entry));
        }
        if ((old_entry.learn == 1) && (entry.learn == 0)) {
            SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_eapp_flow_group_delete
                    (unit, entry.mon_flowtracker_group_id));
            SHR_EXIT();
        }
        if ((old_entry.learn == 0) && (entry.learn == 0)) {
            SHR_EXIT();
        }

        /* Check if tracking params are set */
        if (old_entry.num_tp) {
            if (update->num_tp && sal_memcmp(update->tp, old_entry.tp, sizeof(old_entry.tp))) {
                SHR_ERR_EXIT(SHR_E_EXISTS);
            } else {
                tp_set = true;
            }
        }


        if (entry.num_tp && entry.dt_em_grp_template_id && !tp_set) {
            for (i = 0; i < MCS_SHR_FT_MAX_EM_GROUPS; ++i) {
                for (j = 0; j < ft_drv->num_pipes; ++j) {
                    index = (i * MCS_SHR_FT_MAX_EM_GROUPS) + j;
                    if (info->ha->em_group_id_map[index] == entry.dt_em_grp_template_id) {
                        em_group_id_map_status = 1;
                        em_group_idx = i;
                        break;
                    }
                }
            }

            if (em_group_id_map_status) {
                msg.update = MCS_SHR_FT_GROUP_UPDATE_EM_GROUP_IDX;
                msg.param0 = entry.dt_em_grp_template_id;
                SHR_IF_ERR_EXIT
                    (bcmcth_mon_ft_eapp_flow_group_update(unit, &msg));
            } else {
                for (i = 0; i < MCS_SHR_FT_MAX_EM_GROUPS; ++i) {
                    index = (i * ft_drv->num_pipes);
                    if (info->ha->em_group_id_map[index] == FT_INVALID_EM_GROUP_ID) {
                        for (j = 0; j < ft_drv->num_pipes; ++j) {
                            index = (i * MCS_SHR_FT_MAX_EM_GROUPS) + j;
                            info->ha->em_group_id_map[index] = entry.dt_em_grp_template_id;
                            em_group_idx = i;
                        }

                        if (ft_drv && ft_drv->ft_grp_init) {
                            SHR_IF_ERR_VERBOSE_EXIT
                                (ft_drv->ft_grp_init(unit, &entry, &flex_msg, &pdd_index));
                            if (entry.oper !=
                                    BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_STATE_T_T_SUCCESS) {
                                update->oper = entry.oper;
                                SHR_EXIT();
                            }
                        }
                        flex_msg.group_idx = em_group_idx;

                        /* Send em group information */
                        SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_eapp_flex_em_group_create(unit,
                                    &flex_msg));

                        /* Update EM group ID */
                        msg.group_idx = entry.mon_flowtracker_group_id;
                        msg.update = MCS_SHR_FT_GROUP_UPDATE_EM_GROUP_IDX;
                        msg.param0 = em_group_idx;
                        SHR_IF_ERR_EXIT
                            (bcmcth_mon_ft_eapp_flow_group_update(unit, &msg));
                        break;
                    }
                }
                if (i >= MCS_SHR_FT_MAX_EM_GROUPS) {
                    LOG_ERROR(BSL_LOG_MODULE,
                            (BSL_META_U(unit,
                                        "FT(unit %d) Error: Maximum number of EM Groups created.\n"),
                             unit));
                    SHR_ERR_EXIT(SHR_E_RESOURCE);
                }
            }
            sal_memset(&ctrl_entry, 0 ,sizeof(bcmcth_mon_ft_control_t));
            rv = bcmcth_mon_ft_control_entry_get(unit, &ctrl_entry);
            if (rv == SHR_E_NONE) {
                /* Check if Hw learn is set to true */
                if (FLOWTRACKER_HW_LEARN_ENABLED(info->ha->hw_learn)) {
                    /* Set FT HW learn configurations in HW */
                    if (ft_drv && ft_drv->ft_grp_hw_learn_config) {
                        SHR_IF_ERR_VERBOSE_EXIT(
                                (ft_drv->ft_grp_hw_learn_config(unit,
                                                                &ctrl_entry,
                                                                &entry,
                                                                &flex_msg,
                                                                pdd_index,
                                                                sbr_index,
                                                                data_format)));
                    }
                }
            }
        }
    }

    msg.group_idx = entry.mon_flowtracker_group_id;
    if (entry.flow_limit != old_entry.flow_limit) {
        if (!FLOWTRACKER_HW_LEARN_WO_EAPP_ENABLED(info->ha->hw_learn)) {
            msg.update = MCS_SHR_FT_GROUP_UPDATE_FLOW_THRESHOLD ;
            msg.param0 = entry.flow_limit;
            SHR_IF_ERR_EXIT
                (bcmcth_mon_ft_eapp_flow_group_update(unit, &msg));
        }
        /* Check if Hw learn is set to true */
        if (FLOWTRACKER_HW_LEARN_ENABLED(info->ha->hw_learn)) {
            /* Update the flow limit config in HW */
            if (ft_drv && ft_drv->ft_hw_learn_flow_limit_update) {
                SHR_IF_ERR_VERBOSE_EXIT(
                        (ft_drv->ft_hw_learn_flow_limit_update(unit,
                                                               &entry
                                                              )));
            }
        }
    }

    if (!FLOWTRACKER_HW_LEARN_WO_EAPP_ENABLED(info->ha->hw_learn)) {
        if (old_entry.aging_interval_ms != entry.aging_interval_ms) {
            msg.update = MCS_SHR_FT_GROUP_UPDATE_AGING_INTERVAL;
            msg.param0 = entry.aging_interval_ms;
            SHR_IF_ERR_EXIT
                (bcmcth_mon_ft_eapp_flow_group_update(unit, &msg));
        }

        if (old_entry.mon_flowtracker_elephant_profile_id !=
                entry.mon_flowtracker_elephant_profile_id){
            msg.update = MCS_SHR_FT_GROUP_UPDATE_ELPH_PROFILE;
            msg.param0 = entry.mon_flowtracker_elephant_profile_id;
            SHR_IF_ERR_EXIT
                (bcmcth_mon_ft_eapp_flow_group_update(unit, &msg));
        }

        if (entry.num_export_triggers != old_entry.num_export_triggers) {
            if (old_entry.export_triggers[0] != entry.export_triggers[0]) {
                export_trigger |= MCS_SHR_FT_EXPORT_TRIGGER_PERIODIC;
            }
            if (old_entry.export_triggers[1] != entry.export_triggers[1]) {
                export_trigger |= MCS_SHR_FT_EXPORT_TRIGGER_NEW_LEARN;
            }
            if (old_entry.export_triggers[2] != entry.export_triggers[2]) {
                export_trigger |= MCS_SHR_FT_EXPORT_TRIGGER_AGE_OUT;
            }
            if (export_trigger) {
                msg.update = MCS_SHR_FT_GROUP_UPDATE_EXPORT_TRIGGERS;
                msg.param0 = export_trigger;
                SHR_IF_ERR_EXIT(
                        bcmcth_mon_ft_eapp_flow_group_update(unit, &msg));
            }
        }
    }

    if (entry.num_actions &&
            sal_memcmp(entry.actions, old_entry.actions, sizeof(entry.actions))) {
        if (!FLOWTRACKER_HW_LEARN_WO_EAPP_ENABLED(info->ha->hw_learn)) {
            action_msg.group_idx = entry.mon_flowtracker_group_id;
            for (i = 0; i < entry.num_actions; i++) {
                action_msg.actions|= entry.actions[i].type;
            }
            SHR_IF_ERR_EXIT
                (bcmcth_mon_ft_eapp_group_actions_set(unit, &action_msg));

            msg.update = MCS_SHR_FT_GROUP_UPDATE_ELPH_QOS_PROFILE_ID;
            msg.param0 = entry.actions[0].em_ft_opaque_obj0_val &
                MCS_SHR_FT_QOS_PROFILE_MASK;
            SHR_IF_ERR_EXIT
                (bcmcth_mon_ft_eapp_flow_group_update(unit, &msg));
            if (ft_drv && ft_drv->ft_grp_init) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (ft_drv->ft_grp_init(unit, &entry, &flex_msg, &pdd_index));
                if (entry.oper !=
                        BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_STATE_T_T_SUCCESS) {
                    update->oper = entry.oper;
                    SHR_EXIT();
                }
                sal_memset(&ctrl_entry, 0 ,sizeof(bcmcth_mon_ft_control_t));
                rv = bcmcth_mon_ft_control_entry_get(unit, &ctrl_entry);
                if (rv == SHR_E_NONE) {
                    /* Check if Hw learn is set to true */
                    if (FLOWTRACKER_HW_LEARN_ENABLED(ctrl_entry.hw_learn)) {
                        /* Re-configure the FT_GROUP_TABLE config. */
                        if (ft_drv && ft_drv->ft_grp_hw_learn_config) {
                            SHR_IF_ERR_VERBOSE_EXIT(
                                    (ft_drv->ft_grp_hw_learn_config(unit,
                                                                    &ctrl_entry,
                                                                    &entry,
                                                                    &flex_msg,
                                                                    pdd_index,
                                                                    sbr_index,
                                                                    data_format)));
                        }
                    }
                }
            }
        } else {
            /* Set FT HW learn configurations in HW */
            if (ft_drv && ft_drv->ft_grp_hw_learn_config_wo_eapp) {
                SHR_IF_ERR_VERBOSE_EXIT(
                        (ft_drv->ft_grp_hw_learn_config_wo_eapp(unit,
                                                                &entry)));
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_export_template_defaults_get(int unit, bcmcth_mon_ft_export_template_t *entry)
{
    bcmlrd_client_field_info_t *fields_info = NULL;
    size_t idx;
    uint32_t field_count = 0;
    size_t actual_count;
    int i = 0;

    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_flowtracker_export_template_out_fields_count(unit, &field_count));

    SHR_ALLOC(fields_info, sizeof(bcmlrd_client_field_info_t) * field_count,
            "bcmcthMonFtExportTemplateFldInfo");
    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmlrd_table_fields_def_get(unit, "MON_FLOWTRACKER_EXPORT_TEMPLATE",
                                     field_count, fields_info, &actual_count));

    for (idx = 0; idx < actual_count; idx++) {
        switch (fields_info[idx].id) {
            case MON_FLOWTRACKER_EXPORT_TEMPLATEt_MON_FLOWTRACKER_EXPORT_TEMPLATE_IDf:
                entry->mon_ft_export_template_id = fields_info[idx].def.u32;;
                break;
            case MON_FLOWTRACKER_EXPORT_TEMPLATEt_SET_IDf:
                entry->set_id = fields_info[idx].def.u16;
                break;
            case MON_FLOWTRACKER_EXPORT_TEMPLATEt_NUM_EXPORT_ELEMENTSf:
                entry->num_export_elements = fields_info[idx].def.u32;;
                break;
            case MON_FLOWTRACKER_EXPORT_TEMPLATEt_EXPORT_ELEMENTSu_TYPEf:
                for (i = 0; i < FLOWTRACKER_EXPORT_ELEMENTS_MAX; i++) {
                    entry->export_elements[i].type = fields_info[idx].def.u32;
                }
                break;
            case MON_FLOWTRACKER_EXPORT_TEMPLATEt_EXPORT_ELEMENTSu_DATA_SIZEf:
                for (i = 0; i < FLOWTRACKER_EXPORT_ELEMENTS_MAX; i++) {
                    entry->export_elements[i].data_size = fields_info[idx].def.u32;
                }
                break;
            case MON_FLOWTRACKER_EXPORT_TEMPLATEt_EXPORT_ELEMENTSu_ENTERPRISEf:
                for (i = 0; i < FLOWTRACKER_EXPORT_ELEMENTS_MAX; i++) {
                    entry->export_elements[i].enterprise = fields_info[idx].def.is_true;
                }
                break;
            case MON_FLOWTRACKER_EXPORT_TEMPLATEt_EXPORT_ELEMENTSu_ENTERPRISE_IDf:
                for (i = 0; i < FLOWTRACKER_EXPORT_ELEMENTS_MAX; i++) {
                    entry->export_elements[i].id = fields_info[idx].def.u32;
                }
                break;
            case MON_FLOWTRACKER_EXPORT_TEMPLATEt_COLLECTOR_TYPEf:
                entry->collector_type = fields_info[idx].def.u32;
                break;
            case MON_FLOWTRACKER_EXPORT_TEMPLATEt_MON_COLLECTOR_IPV4_IDf:
                entry->collector_ipv4_id = fields_info[idx].def.u32;
                break;
            case MON_FLOWTRACKER_EXPORT_TEMPLATEt_MON_COLLECTOR_IPV6_IDf:
                entry->collector_ipv6_id = fields_info[idx].def.u32;
                break;
            case MON_FLOWTRACKER_EXPORT_TEMPLATEt_TRANSMIT_INTERVALf:
                entry->transmit_interval = fields_info[idx].def.u32;
                break;
            case MON_FLOWTRACKER_EXPORT_TEMPLATEt_INITIAL_BURSTf:
                entry->initial_burst = fields_info[idx].def.u32;
                break;
            case MON_FLOWTRACKER_EXPORT_TEMPLATEt_OPERATIONAL_STATUSf:
                /* Ignore dynamically calculated fields. */
                break;

            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
exit:
    SHR_FREE(fields_info);
    SHR_FUNC_EXIT();
}

static void
bcmcth_mon_ft_export_template_fields_copy(bcmcth_mon_ft_export_template_t *dst,
        bcmcth_mon_ft_export_template_t *src)
{
    int i;

    if (SHR_BITGET(src->fbmp,
                MON_FLOWTRACKER_EXPORT_TEMPLATEt_MON_FLOWTRACKER_EXPORT_TEMPLATE_IDf)) {
        dst->mon_ft_export_template_id = src->mon_ft_export_template_id;
        SHR_BITSET(dst->fbmp,
                MON_FLOWTRACKER_EXPORT_TEMPLATEt_MON_FLOWTRACKER_EXPORT_TEMPLATE_IDf);
    }
    if (SHR_BITGET(src->fbmp,
                MON_FLOWTRACKER_EXPORT_TEMPLATEt_SET_IDf)) {
        dst->set_id = src->set_id;
        SHR_BITSET(dst->fbmp, MON_FLOWTRACKER_EXPORT_TEMPLATEt_SET_IDf);
    }
    if (SHR_BITGET(src->fbmp,
                MON_FLOWTRACKER_EXPORT_TEMPLATEt_NUM_EXPORT_ELEMENTSf)) {
        dst->num_export_elements = src->num_export_elements;
        SHR_BITSET(dst->fbmp,
            MON_FLOWTRACKER_EXPORT_TEMPLATEt_NUM_EXPORT_ELEMENTSf);
    }
    if (SHR_BITGET(src->fbmp,
                MON_FLOWTRACKER_EXPORT_TEMPLATEt_EXPORT_ELEMENTSu_TYPEf)) {
        SHR_BITSET(dst->fbmp,
            MON_FLOWTRACKER_EXPORT_TEMPLATEt_EXPORT_ELEMENTSu_TYPEf);

        for (i = 0; i < FLOWTRACKER_EXPORT_ELEMENTS_MAX; i++) {
            if (SHR_BITGET(src->fbmp_export_ele, i)) {
                dst->export_elements[i].type = src->export_elements[i].type;
                SHR_BITSET(dst->fbmp_export_ele, i);
            }
        }
    }
    if (SHR_BITGET(src->fbmp,
                MON_FLOWTRACKER_EXPORT_TEMPLATEt_EXPORT_ELEMENTSu_DATA_SIZEf)) {
        for (i = 0; i < FLOWTRACKER_EXPORT_ELEMENTS_MAX; i++) {
            if (SHR_BITGET(src->fbmp_export_ele, i)) {
                dst->export_elements[i].data_size = src->export_elements[i].data_size;
                SHR_BITSET(dst->fbmp_export_ele, i);
            }
        }
    }
    if (SHR_BITGET(src->fbmp,
                MON_FLOWTRACKER_EXPORT_TEMPLATEt_EXPORT_ELEMENTSu_ENTERPRISEf)) {
        for (i = 0; i < FLOWTRACKER_EXPORT_ELEMENTS_MAX; i++) {
            if (SHR_BITGET(src->fbmp_export_ele, i)) {
                dst->export_elements[i].enterprise = src->export_elements[i].enterprise;
                SHR_BITSET(dst->fbmp_export_ele, i);
            }
        }
    }
    if (SHR_BITGET(src->fbmp,
                MON_FLOWTRACKER_EXPORT_TEMPLATEt_EXPORT_ELEMENTSu_ENTERPRISE_IDf)) {
        for (i = 0; i < FLOWTRACKER_EXPORT_ELEMENTS_MAX; i++) {
            if (SHR_BITGET(src->fbmp_export_ele, i)) {
                dst->export_elements[i].id = src->export_elements[i].id;
                SHR_BITSET(dst->fbmp_export_ele, i);
            }
        }
    }
    if (SHR_BITGET(src->fbmp,
                MON_FLOWTRACKER_EXPORT_TEMPLATEt_COLLECTOR_TYPEf)) {
        dst->collector_type = src->collector_type;
        SHR_BITSET(dst->fbmp,
            MON_FLOWTRACKER_EXPORT_TEMPLATEt_EXPORT_ELEMENTSu_ENTERPRISE_IDf);
    }
    if (SHR_BITGET(src->fbmp,
                MON_FLOWTRACKER_EXPORT_TEMPLATEt_MON_COLLECTOR_IPV4_IDf)) {
        dst->collector_ipv4_id = src->collector_ipv4_id;
        SHR_BITSET(dst->fbmp,
            MON_FLOWTRACKER_EXPORT_TEMPLATEt_MON_COLLECTOR_IPV4_IDf);
    }
    if (SHR_BITGET(src->fbmp,
                MON_FLOWTRACKER_EXPORT_TEMPLATEt_MON_COLLECTOR_IPV6_IDf)) {
        dst->collector_ipv6_id = src->collector_ipv6_id;
        SHR_BITSET(dst->fbmp,
            MON_FLOWTRACKER_EXPORT_TEMPLATEt_MON_COLLECTOR_IPV6_IDf);
    }
    if (SHR_BITGET(src->fbmp,
                MON_FLOWTRACKER_EXPORT_TEMPLATEt_TRANSMIT_INTERVALf)) {
        dst->transmit_interval = src->transmit_interval;
        SHR_BITSET(dst->fbmp,
            MON_FLOWTRACKER_EXPORT_TEMPLATEt_TRANSMIT_INTERVALf);
    }
    if (SHR_BITGET(src->fbmp,
                MON_FLOWTRACKER_EXPORT_TEMPLATEt_INITIAL_BURSTf)) {
        dst->initial_burst = src->initial_burst;
        SHR_BITSET(dst->fbmp,
            MON_FLOWTRACKER_EXPORT_TEMPLATEt_INITIAL_BURSTf);
    }
}

static int
bcmcth_mon_ft_template_encap_build_pack (int unit,
    bcmcth_mon_ft_export_template_t *entry,
    uint16_t *encap_length, uint8_t *buf)
{
    uint8_t  *cur_ptr = buf;
    uint32_t i = 0, element_id = 0;
    int rv = 0;

    SHR_UTIL_PACK_U32(cur_ptr, FT_INT_TEMPLATE_XMIT_SET_ID);
    /* Skip over length */
    cur_ptr += 2;

    SHR_UTIL_PACK_U16(cur_ptr, entry->set_id);
    SHR_UTIL_PACK_U16(cur_ptr, entry->num_export_elements);

    for (i = 0; i < entry->num_export_elements; i++) {
        if (entry->export_elements[i].enterprise == 0) {
            SHR_UTIL_PACK_U16(cur_ptr, entry->export_elements[i].id);
            SHR_UTIL_PACK_U16(cur_ptr, entry->export_elements[i].data_size);
        } else {
            element_id = (1 << 15) | entry->export_elements[i].id;
            SHR_UTIL_PACK_U16(cur_ptr, element_id);
            SHR_UTIL_PACK_U16(cur_ptr, entry->export_elements[i].data_size);
            if (entry->collector_type ==
                BCMLTD_COMMON_MON_COLLECTOR_TYPE_T_T_IPV4) {
                SHR_UTIL_PACK_U32(cur_ptr,
                                  entry->collector_ipv4.enterprise_number);
            } else {
                SHR_UTIL_PACK_U32(cur_ptr,
                                  entry->collector_ipv6.enterprise_number);
            }
        }
        *encap_length = cur_ptr - buf;
    }

    return rv;
}

static uint8_t
bcmcth_mon_ft_export_template_type_to_msg_info_elems(
        bcmltd_common_flowtracker_export_element_type_t_t type)
{
    switch (type) {
        /*! None.  */
        case BCMLTD_COMMON_FLOWTRACKER_EXPORT_ELEMENT_TYPE_T_T_NONE:
            return MCS_SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS;
            break;
            /*! SRC IPv4 of the flow needs to be exported.  */
        case BCMLTD_COMMON_FLOWTRACKER_EXPORT_ELEMENT_TYPE_T_T_SRC_IPV4:
            return MCS_SHR_FT_TEMPLATE_INFO_ELEM_SRC_IPV4;
            break;
            /*! DST IPV4 of the flow needs to be exported.  */
        case BCMLTD_COMMON_FLOWTRACKER_EXPORT_ELEMENT_TYPE_T_T_DST_IPV4:
            return MCS_SHR_FT_TEMPLATE_INFO_ELEM_DST_IPV4;
            break;
            /*! SRC IPV6 of the flow needs to be exported.  */
        case BCMLTD_COMMON_FLOWTRACKER_EXPORT_ELEMENT_TYPE_T_T_SRC_IPV6:
            /* Not supported yet */
            return MCS_SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS;
            break;
            /*! DST IPV6 of the flow needs to be exported.  */
        case BCMLTD_COMMON_FLOWTRACKER_EXPORT_ELEMENT_TYPE_T_T_DST_IPV6:
            /* Not supported yet */
            return MCS_SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS;
            break;
            /*! L4 SRC PORT of the flow needs to be exported.  */
        case BCMLTD_COMMON_FLOWTRACKER_EXPORT_ELEMENT_TYPE_T_T_SRC_L4_PORT:
            return MCS_SHR_FT_TEMPLATE_INFO_ELEM_L4_SRC_PORT;
            break;
            /*! L4 DST PORT of the flow needs to be exported.  */
        case BCMLTD_COMMON_FLOWTRACKER_EXPORT_ELEMENT_TYPE_T_T_DST_L4_PORT:
            return MCS_SHR_FT_TEMPLATE_INFO_ELEM_L4_DST_PORT;
            break;
            /*! IP PROTOCOL of the flow needs to be exported.  */
        case BCMLTD_COMMON_FLOWTRACKER_EXPORT_ELEMENT_TYPE_T_T_IP_PROTOCOL:
            return MCS_SHR_FT_TEMPLATE_INFO_ELEM_IP_PROTOCOL;
            break;
            /*! Count of packets of the flow needs to be exported.  */
        case BCMLTD_COMMON_FLOWTRACKER_EXPORT_ELEMENT_TYPE_T_T_PKT_COUNT:
            return MCS_SHR_FT_TEMPLATE_INFO_ELEM_PKT_TOTAL_COUNT;
            break;
            /*! Count of bytes of the flow needs to be exported.  */
        case BCMLTD_COMMON_FLOWTRACKER_EXPORT_ELEMENT_TYPE_T_T_BYTE_COUNT:
            return MCS_SHR_FT_TEMPLATE_INFO_ELEM_BYTE_TOTAL_COUNT;
            break;
            /*! Inner Src IPV4 of the flow needs to be exported.  */
        case BCMLTD_COMMON_FLOWTRACKER_EXPORT_ELEMENT_TYPE_T_T_INNER_SRC_IPV4:
            return MCS_SHR_FT_TEMPLATE_INFO_ELEM_INNER_SRC_IPV4;
            break;
            /*! Inner DST IPV4 of the flow needs to be exported.  */
        case BCMLTD_COMMON_FLOWTRACKER_EXPORT_ELEMENT_TYPE_T_T_INNER_DST_IPV4 :
            return MCS_SHR_FT_TEMPLATE_INFO_ELEM_INNER_DST_IPV4;
            break;
            /*! Inner SRC IPV6 of the flow needs to be exported.  */
        case BCMLTD_COMMON_FLOWTRACKER_EXPORT_ELEMENT_TYPE_T_T_INNER_SRC_IPV6:
            return MCS_SHR_FT_TEMPLATE_INFO_ELEM_INNER_SRC_IPV6;
            break;
            /*! Inner DST IPV6 of the flow needs to be exported.  */
        case BCMLTD_COMMON_FLOWTRACKER_EXPORT_ELEMENT_TYPE_T_T_INNER_DST_IPV6:
            return MCS_SHR_FT_TEMPLATE_INFO_ELEM_INNER_DST_IPV6;
            break;
            /*! Inner L4 SRC PORT of the flow needs to be exported.  */
        case BCMLTD_COMMON_FLOWTRACKER_EXPORT_ELEMENT_TYPE_T_T_INNER_SRC_L4_PORT:
            return MCS_SHR_FT_TEMPLATE_INFO_ELEM_INNER_L4_SRC_PORT;
            break;
            /*! Inner L4 DST PORT of the flow needs to be exported.  */
        case BCMLTD_COMMON_FLOWTRACKER_EXPORT_ELEMENT_TYPE_T_T_INNER_DST_L4_PORT:
            return MCS_SHR_FT_TEMPLATE_INFO_ELEM_INNER_L4_DST_PORT;
            break;
            /*! Inner IP PROTOCOL of the flow needs to be exported.  */
        case BCMLTD_COMMON_FLOWTRACKER_EXPORT_ELEMENT_TYPE_T_T_INNER_IP_PROTOCOL:
            return MCS_SHR_FT_TEMPLATE_INFO_ELEM_INNER_IP_PROTOCOL;
            break;
            /*!
             * Custom element tracked as part of the flow
             *  needs to be exported.
             */
        case BCMLTD_COMMON_FLOWTRACKER_EXPORT_ELEMENT_TYPE_T_T_CUSTOM:
            return MCS_SHR_FT_TEMPLATE_INFO_ELEM_CUSTOM;
            break;
        case BCMLTD_COMMON_FLOWTRACKER_EXPORT_ELEMENT_TYPE_T_T_FLOW_START_TIMESTAMP:
            return MCS_SHR_FT_TEMPLATE_INFO_ELEM_FLOW_START_TS_MSEC;
            break;
        case BCMLTD_COMMON_FLOWTRACKER_EXPORT_ELEMENT_TYPE_T_T_OBSERVATION_TIMESTAMP:
            return MCS_SHR_FT_TEMPLATE_INFO_ELEM_OBS_TS_MSEC;
            break;
        case BCMLTD_COMMON_FLOWTRACKER_EXPORT_ELEMENT_TYPE_T_T_FLOWTRACKER_GROUP:
            return MCS_SHR_FT_TEMPLATE_INFO_ELEM_GROUP_ID;
            break;
        case BCMLTD_COMMON_FLOWTRACKER_EXPORT_ELEMENT_TYPE_T_T_VNID:
            return MCS_SHR_FT_TEMPLATE_INFO_ELEM_VNID;
            break;
    }
    return MCS_SHR_FT_TEMPLATE_MAX_INFO_ELEMENTS;
}

int
bcmcth_mon_ft_export_template_insert(int unit,
                               bcmcth_mon_ft_export_template_t *insert)
{
    uint32_t i;
    bcmcth_mon_ft_info_t *info;
    bcmcth_mon_ft_export_template_t entry;
    mcs_ft_msg_ctrl_template_create_t msg;

    SHR_FUNC_ENTER(unit);

    info = bcmcth_mon_ft_info_get(unit);

    if (info->ha->run == false) {
        insert->oper_status =
            BCMLTD_COMMON_MON_FLOWTRACKER_EXPORT_TEMPLATE_STATE_T_T_APP_NOT_INITIALIZED;
        SHR_EXIT();
    }

    /* Initializations */
    sal_memset(&entry, 0, sizeof(entry));

    /* Get the defaults for the fields that are not configured. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_export_template_defaults_get(unit, &entry));

    /* Overwrite the default values with the fields being inserted. */
    bcmcth_mon_ft_export_template_fields_copy(&entry, insert);

    /* Subtract the template ID by -1 as the index is 0 based in FW. */
    msg.id = entry.mon_ft_export_template_id - 1;
    msg.set_id = entry.set_id;
    msg.num_info_elems = entry.num_export_elements;

    for (i = 0;i <= entry.num_export_elements; i++) {
        msg.info_elems[i] =
            bcmcth_mon_ft_export_template_type_to_msg_info_elems(
                                entry.export_elements[i].type);
        msg.data_size[i] = entry.export_elements[i].data_size;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_eapp_template_create(unit, &msg));

exit:
    SHR_FUNC_EXIT();
}


int
bcmcth_mon_ft_export_template_delete(int unit, uint32_t id)
{
    bcmcth_mon_ft_info_t *info;

    SHR_FUNC_ENTER(unit);

    info = bcmcth_mon_ft_info_get(unit);

    if (info->ha->run == false) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_eapp_template_delete(unit, id));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_group_col_map_defaults_get(int unit, bcmcth_mon_ft_group_col_map_t *entry)
{
    bcmlrd_client_field_info_t *fields_info = NULL;
    size_t fld_num = MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_FIELD_COUNT;
    size_t actual_fld_num;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(fields_info, sizeof(bcmlrd_client_field_info_t) * fld_num,
            "bcmcthMonFtGroupColMapFldInfo");
    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (bcmlrd_table_fields_def_get(unit, "MON_FLOWTRACKER_GROUP_COLLECTOR_MAP",
                                     fld_num, fields_info, &actual_fld_num));

    for (idx = 0; idx < actual_fld_num; idx++) {
        if (SHR_BITGET(entry->fbmp, fields_info[idx].id)) {
            /* The field is already configured, don't overwrite with default. */
            continue;
        }
        switch (fields_info[idx].id) {
            case MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_MON_FLOWTRACKER_GROUP_COLLECTOR_MAP_IDf:
                entry->mon_ft_group_col_map_id = fields_info[idx].def.u32;
                break;
            case MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_MON_FLOWTRACKER_GROUP_IDf:
                entry->mon_ft_group_id = fields_info[idx].def.u16;
                break;
            case MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_COLLECTOR_TYPEf:
                entry->collector_type = fields_info[idx].def.u32;
                break;
            case MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_MON_COLLECTOR_IPV4_IDf:
                entry->collector_ipv4_id = fields_info[idx].def.u32;
                break;
            case MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_MON_COLLECTOR_IPV6_IDf:
                entry->collector_ipv6_id = fields_info[idx].def.u32;
                break;
            case MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_MON_EXPORT_PROFILE_IDf:
                entry->export_profile_id = fields_info[idx].def.u32;
                break;
            case MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_MON_FLOWTRACKER_EXPORT_TEMPLATE_IDf:
                entry->export_template_id = fields_info[idx].def.u32;
                break;
            case MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_OPERATIONAL_STATUSf:
                break;
            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
exit:
    SHR_FREE(fields_info);
    SHR_FUNC_EXIT();
}

static void
bcmcth_mon_ft_group_col_map_fields_copy(bcmcth_mon_ft_group_col_map_t *src,
        bcmcth_mon_ft_group_col_map_t *dst)
{
    /* Copy the field bitmap of dst into src. */
    sal_memcpy(&(src->fbmp), &(dst->fbmp), sizeof(src->fbmp));

    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_MON_FLOWTRACKER_GROUP_COLLECTOR_MAP_IDf)) {
        src->mon_ft_group_col_map_id = dst->mon_ft_group_col_map_id;
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_MON_FLOWTRACKER_GROUP_IDf)) {
        src->mon_ft_group_id = dst->mon_ft_group_id;
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_COLLECTOR_TYPEf)) {
        src->collector_type = dst->collector_type;
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_MON_COLLECTOR_IPV4_IDf)) {
        src->collector_ipv4_id = dst->collector_ipv4_id;
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_MON_COLLECTOR_IPV6_IDf)) {
        src->collector_ipv6_id = dst->collector_ipv6_id;
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_MON_EXPORT_PROFILE_IDf)) {
        src->export_profile_id = dst->export_profile_id;
    }
    if (SHR_BITGET(dst->fbmp,
                MON_FLOWTRACKER_GROUP_COLLECTOR_MAPt_MON_FLOWTRACKER_EXPORT_TEMPLATE_IDf)) {
        src->export_template_id = dst->export_template_id;
    }
}

static int
bcmcth_mon_ft_uc_collector_ipv4_create(int unit,
        bcmcth_mon_ft_group_col_map_t *entry)
{
    uint8_t *cur_ptr;
    shr_util_pack_udp_header_t udp;
    shr_util_pack_ipv4_header_t ipv4;
    shr_util_pack_l2_header_t l2;
    mcs_ft_msg_ctrl_collector_create_t msg;
    bcmcth_mon_collector_ipv4_t *collector = &entry->collector_ipv4;
    bcmcth_mon_export_profile_t *export_profile = &entry->export_profile;
    bcmcth_mon_drv_t *mon_drv = bcmcth_mon_drv_get(unit);

    SHR_FUNC_ENTER(unit);

    sal_memset(&msg, 0, sizeof(msg));
    msg.id = entry->mon_ft_group_col_map_id;
    msg.mtu = export_profile->max_packet_length;

    /* L2 header */
    sal_memset(&l2, 0, sizeof(l2));
    sal_memcpy(l2.dst_mac, collector->dst_mac, sizeof(shr_mac_t));
    sal_memcpy(l2.src_mac, collector->src_mac, sizeof(shr_mac_t));

    /* Call chip specific msg fill routine */
    if (mon_drv && mon_drv->collector_ipv4_vlan_info_copy) {
        mon_drv->collector_ipv4_vlan_info_copy(unit, collector, &l2);
    }

    l2.etype = SHR_L2_ETYPE_IPV4;

    /* IPv4 header. */
    sal_memset(&ipv4, 0, sizeof(ipv4));
    ipv4.version = SHR_IPV4_VERSION;
    ipv4.h_length = SHR_IPV4_HEADER_LENGTH / 4;
    ipv4.dscp = collector->tos;
    ipv4.ttl = collector->ttl;
    ipv4.protocol = SHR_IP_PROTO_UDP;
    ipv4.src = collector->src_ip;
    ipv4.dst = collector->dst_ip;

    /* UDP header. */
    sal_memset(&udp, 0, sizeof(udp));
    udp.src = collector->src_port;
    udp.dst = collector->dst_port;

    /* Pack the headers in the message and get the offsets. */
    cur_ptr = msg.encap;

    cur_ptr = shr_util_pack_l2_header_pack(cur_ptr, &l2);

    msg.ip_offset = cur_ptr - msg.encap;
    cur_ptr = shr_util_pack_ipv4_header_pack(cur_ptr, &ipv4);

    msg.udp_offset = cur_ptr - msg.encap;
    cur_ptr = shr_util_pack_udp_header_pack(cur_ptr, &udp);

    msg.ip_base_checksum =
        shr_util_pack_initial_chksum_get(SHR_IPV4_HEADER_LENGTH,
                msg.encap + msg.ip_offset);

    msg.udp_base_checksum =
        shr_util_pack_udp_initial_checksum_get(0, &ipv4, NULL, NULL, &udp);

    msg.system_id_length = collector->system_id_len;
    sal_memcpy(msg.system_id,
            collector->system_id,
            msg.system_id_length * sizeof(uint8_t));

    msg.component_id = collector->component_id;
    msg.encap_length = cur_ptr - msg.encap;

    /* Check if there is space to insert at least the headers  */
    if ((msg.encap_length +
                BCMCTH_INT_FT_IPFIX_MSG_HDR_LENGTH   +
                BCMCTH_INT_FT_SET_HDR_LENGTH         +
                BCMCTH_INT_FT_L2_CRC_LENGTH) > msg.mtu) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_eapp_collector_create(unit, &msg));

    /* Multiply export interval by 1000 to convert usecs to msecs */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_eapp_export_interval_update(
            unit, (export_profile->export_interval/1000)));
exit:
    SHR_FUNC_EXIT();

}

static int
bcmcth_mon_ft_uc_collector_ipv6_create(int unit,
        bcmcth_mon_ft_group_col_map_t * entry)
{
    uint8_t *cur_ptr;
    shr_util_pack_udp_header_t udp;
    shr_util_pack_ipv6_header_t ipv6;
    shr_util_pack_l2_header_t l2;
    mcs_ft_msg_ctrl_collector_create_t msg;
    bcmcth_mon_collector_ipv6_t *collector = &entry->collector_ipv6;
    bcmcth_mon_export_profile_t *export_profile = &entry->export_profile;
    bcmcth_mon_drv_t *mon_drv = bcmcth_mon_drv_get(unit);

    SHR_FUNC_ENTER(unit);

    sal_memset(&msg, 0, sizeof(msg));
    msg.id = entry->mon_ft_group_col_map_id;
    msg.mtu = export_profile->max_packet_length;

    /* L2 header */
    sal_memset(&l2, 0, sizeof(l2));
    sal_memcpy(l2.dst_mac, collector->dst_mac, sizeof(shr_mac_t));
    sal_memcpy(l2.src_mac, collector->src_mac, sizeof(shr_mac_t));

    /* Call chip specific msg fill routine */
    if (mon_drv && mon_drv->collector_ipv6_vlan_info_copy) {
        mon_drv->collector_ipv6_vlan_info_copy(unit, collector, &l2);
    }
    l2.etype = SHR_L2_ETYPE_IPV6;

    /* Ipv6 header. */
    sal_memset(&ipv6, 0, sizeof(ipv6));
    ipv6.version = SHR_IPV6_VERSION;
    ipv6.t_class = collector->tos;
    ipv6.f_label = collector->flow_label;
    ipv6.next_header = SHR_IP_PROTO_UDP;
    ipv6.hop_limit = collector->ttl;
    sal_memcpy(ipv6.src, collector->src_ip6_addr, sizeof(shr_ip6_t));
    sal_memcpy(ipv6.dst, collector->dst_ip6_addr, sizeof(shr_ip6_t));

    /* UDP header. */
    sal_memset(&udp, 0, sizeof(udp));
    udp.src = collector->src_port;
    udp.dst = collector->dst_port;

    /* Pack the headers in the message and get the offsets. */
    cur_ptr = msg.encap;

    cur_ptr = shr_util_pack_l2_header_pack(cur_ptr, &l2);

    msg.ip_offset = cur_ptr - msg.encap;
    cur_ptr = shr_util_pack_ipv6_header_pack(cur_ptr, &ipv6);

    msg.udp_offset = cur_ptr - msg.encap;
    cur_ptr = shr_util_pack_udp_header_pack(cur_ptr, &udp);

    msg.ip_base_checksum =
        shr_util_pack_initial_chksum_get(SHR_IPV6_HEADER_LENGTH,
                msg.encap + msg.ip_offset);


    msg.udp_base_checksum =
        shr_util_pack_udp_initial_checksum_get(1, NULL, &ipv6, NULL, &udp);

    msg.system_id_length = collector->system_id_len;
    sal_memcpy(msg.system_id,
            collector->system_id,
            msg.system_id_length * sizeof(uint8_t));
    msg.component_id = collector->component_id;

    msg.encap_length = cur_ptr - msg.encap;

    /* Check if there is space to insert at least the headers  */
    if ((msg.encap_length +
                BCMCTH_INT_FT_IPFIX_MSG_HDR_LENGTH   +
                BCMCTH_INT_FT_SET_HDR_LENGTH         +
                BCMCTH_INT_FT_L2_CRC_LENGTH)  > msg.mtu) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }
    SHR_IF_ERR_VERBOSE_EXIT(
            bcmcth_mon_ft_eapp_collector_create(unit, &msg ));
exit:
    SHR_FUNC_EXIT();

}

static int
bcmcth_mon_ft_export_profile_defaults_get(int unit,
                            bcmcth_mon_export_profile_t *entry)
{
    bcmlrd_client_field_info_t *fields_info = NULL;
    size_t fld_num = MON_EXPORT_PROFILEt_FIELD_COUNT;
    size_t actual_fld_num;
    size_t idx;

    SHR_FUNC_ENTER(unit);

    SHR_ALLOC(fields_info, sizeof(bcmlrd_client_field_info_t) * fld_num,
              "bcmcthMonExportProfileFldInfo");
    SHR_NULL_CHECK(fields_info, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
       (bcmlrd_table_fields_def_get(unit, "MON_EXPORT_PROFILE",
                                    fld_num, fields_info, &actual_fld_num));

    for (idx = 0; idx < actual_fld_num; idx++) {
        if (SHR_BITGET(entry->fbmp, fields_info[idx].id)) {
            /* The field is already configured, don't overwrite with default. */
            continue;
        }

        switch (fields_info[idx].id) {
            case MON_EXPORT_PROFILEt_WIRE_FORMATf:
                entry->wire_format = fields_info[idx].def.u8;
                break;

            case MON_EXPORT_PROFILEt_PACKET_LEN_INDICATORf:
                entry->packet_len_indicator = fields_info[idx].def.u8;
                break;

            case MON_EXPORT_PROFILEt_MAX_PKT_LENGTHf:
                entry->max_packet_length = fields_info[idx].def.u32;
                break;

            case MON_EXPORT_PROFILEt_NUM_RECORDSf:
                entry->num_records = fields_info[idx].def.u32;
                break;

            default:
                /* Don't care about other fields. */
                break;
        }
    }

exit:
    SHR_FREE(fields_info);
    SHR_FUNC_EXIT();
}

static int
bcmcth_mon_ft_collector_config(int unit,
        bcmcth_mon_ft_group_col_map_t *entry) {

    bool collector_ipv4_valid, collector_ipv6_valid;
    bcmcth_mon_ft_info_t *info;
    SHR_FUNC_ENTER(unit);

    info = bcmcth_mon_ft_info_get(unit);
    collector_ipv4_valid = collector_ipv6_valid = false;

    switch (entry->collector_type) {

        case BCMLTD_COMMON_MON_COLLECTOR_TYPE_T_T_IPV4:
            if (entry->collector_ipv4_found == true) {
                collector_ipv4_valid = true;
            }
            break;

        case BCMLTD_COMMON_MON_COLLECTOR_TYPE_T_T_IPV6:
            if (entry->collector_ipv6_found == true) {
                collector_ipv6_valid = true;
            }
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (entry->export_profile_found == true) {
        /* Get the defaults for values that are not configured. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_export_profile_defaults_get(unit,
                                                       &(entry->export_profile)));
    }

    /* Create the collector in the app if all the conditions are met. */
    if (info->ha->run == true) {
            if (collector_ipv4_valid == true) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_mon_ft_uc_collector_ipv4_create(unit,
                                                            entry));

            } else if (collector_ipv6_valid == true) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmcth_mon_ft_uc_collector_ipv6_create(unit,
                                                            entry));
            }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_ft_group_col_map_insert(int unit,
        bcmcth_mon_ft_group_col_map_t *insert)
{
    bcmcth_mon_ft_info_t *info;
    bcmcth_mon_ft_group_col_map_t entry;
    mcs_ft_msg_ctrl_group_update_t msg;

    SHR_FUNC_ENTER(unit);

    info = bcmcth_mon_ft_info_get(unit);

    if (info->ha->run == false) {
        insert->oper_status =
            BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_COLLECTOR_MAP_STATE_T_T_APP_NOT_INITIALIZED;
        SHR_EXIT();
    }

    /* Initializations */
    sal_memset(&entry, 0, sizeof(entry));

    /* Get the defaults for the fields that are not configured. */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_group_col_map_defaults_get(unit, &entry));

    /* Overwrite the default values with the fields being inserted. */
    bcmcth_mon_ft_group_col_map_fields_copy(&entry, insert);
    if (info->ha->collector_ref_count == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_collector_config(unit, insert));
    }

    sal_memset(&msg, 0, sizeof(msg));
    msg.group_idx = insert->mon_ft_group_id;
    msg.update = MCS_SHR_FT_GROUP_UPDATE_COLLECTOR_SET;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_eapp_flow_group_update(unit, &msg));


    sal_memset(&msg, 0, sizeof(msg));
    msg.group_idx = insert->mon_ft_group_id;
    msg.update = MCS_SHR_FT_GROUP_UPDATE_TEMPLATE_SET;
    /* Send the template ID. Subtract 1 as the index is 0 based in the FW. */
    msg.param0 = entry.export_template_id - 1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_eapp_flow_group_update(unit, &msg));

    info->ha->collector_ref_count++;
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_ft_group_col_map_delete(int unit,
    bcmcth_mon_ft_group_col_map_t *entry)
{
    bcmcth_mon_ft_info_t *info;
    uint32_t collector_id;
    mcs_ft_msg_ctrl_group_update_t msg;

    SHR_FUNC_ENTER(unit);

    info = bcmcth_mon_ft_info_get(unit);

    if (info->ha->run == false) {
        SHR_EXIT();
    }

    sal_memset(&msg, 0, sizeof(msg));
    msg.group_idx = entry->mon_ft_group_id;
    msg.update = MCS_SHR_FT_GROUP_UPDATE_COLLECTOR_UNSET;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_eapp_flow_group_update(unit, &msg));

    sal_memset(&msg, 0, sizeof(msg));
    msg.group_idx = entry->mon_ft_group_id;
    msg.update = MCS_SHR_FT_GROUP_UPDATE_TEMPLATE_UNSET;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_mon_ft_eapp_flow_group_update(unit, &msg));

    info->ha->collector_ref_count--;

    if (entry->collector_ipv4_found) {
        collector_id = entry->collector_ipv4_id;
    } else if (entry->collector_ipv6_found) {
        collector_id = entry->collector_ipv6_id;
    } else {
        entry->oper_status =
            BCMLTD_COMMON_MON_FLOWTRACKER_GROUP_COLLECTOR_MAP_STATE_T_T_COLLECTOR_NOT_EXISTS;
        SHR_EXIT();
    }

    if (info->ha->collector_ref_count == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_eapp_collector_delete(unit, collector_id));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_mon_ft_export_template_update(int unit,
                           bcmcth_mon_ft_export_template_t *cur,
                           bcmcth_mon_ft_export_template_t *update)
{
    bcmcth_mon_ft_info_t *info;
    bcmcth_mon_ft_export_template_t new_entry, old_entry;
    mcs_ft_msg_ctrl_template_xmit_t templ_msg;

    SHR_FUNC_ENTER(unit);

    info = bcmcth_mon_ft_info_get(unit);

    if (info == NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    /* App not initialized */
    if (info->ha->run == false) {
        cur->oper_status =
            BCMLTD_COMMON_MON_FLOWTRACKER_EXPORT_TEMPLATE_STATE_T_T_APP_NOT_INITIALIZED;
        SHR_EXIT();
    }
    /* Initializations */
    sal_memset(&old_entry, 0, sizeof(old_entry));
    sal_memset(&new_entry, 0, sizeof(new_entry));

    /* Get the defaults for all the fields */
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_export_template_defaults_get(unit,
                &new_entry));
    SHR_IF_ERR_VERBOSE_EXIT(bcmcth_mon_ft_export_template_defaults_get(unit,
                &old_entry));

    /* old_entry contains current view of the table and
       default values for not configured fields */
    bcmcth_mon_ft_export_template_fields_copy(&old_entry, cur);
    bcmcth_mon_ft_export_template_fields_copy(&new_entry, cur);

    /* new_entry contains the view of the table entry as it
       should be once the update is complete. */
    bcmcth_mon_ft_export_template_fields_copy(&new_entry, update);

    if (new_entry.transmit_interval != old_entry.transmit_interval) {

        /* Start the template set transmit */
        templ_msg.template_id = new_entry.mon_ft_export_template_id;

        if (new_entry.collector_type == BCMLTD_COMMON_MON_COLLECTOR_TYPE_T_T_IPV4) {
            templ_msg.collector_id = new_entry.collector_ipv4_id;
        } else if (new_entry.collector_type == BCMLTD_COMMON_MON_COLLECTOR_TYPE_T_T_IPV6) {
            templ_msg.collector_id = new_entry.collector_ipv6_id;
        } else {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        templ_msg.interval_secs = new_entry.transmit_interval;
        templ_msg.initial_burst = new_entry.initial_burst;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_template_encap_build_pack(unit, &new_entry,
                                                     &(templ_msg.encap_length),
                                                     templ_msg.encap));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_mon_ft_eapp_template_xmit_start(unit, &templ_msg));
    }

exit:
    SHR_FUNC_EXIT();

}
