/*! \file bcmtm_imm_mc_group.c
 *
 * BCMTM MC Group in-memory table.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <shr/shr_ha.h>
#include <bcmbd/bcmbd.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_types.h>
#include <bcmtm/multicast/bcmtm_mc_group.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>
#include <bcmtm/bcmtm_utils_internal.h>
#include <bcmtm/bcmtm_ha_internal.h>
#include <bcmissu/issu_api.h>

/*******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE  BSL_LS_BCMTM_IMM

/*******************************************************************************
 * Private functions
 */

/*
 * Create an entry in the TM_MC_GROUP LT.
 */
static int
bcmtm_mc_group_create (int unit, mc_group_cfg_t *lt_cfg,
                       mc_group_cfg_t *old_lt_cfg)
{
    bcmtm_mc_repl_group_info_t repl_group_info;
    bcmtm_mc_ipmc_info_t ipmc_info;
    bcmtm_mc_repl_head_info_t *repl_head_info = NULL;
    bcmtm_mc_repl_head_info_t *mc_head_list;
    int i, mc_group_id, mc_head_id, write_to_pt = 0;
    bool do_not_cut_through = 0;
    bcmtm_mc_dev_info_t *mc_dev;
    int max_repl_count = 0;
    int l2_repl = 0;
    bcmtm_drv_t *drv;
    bcmtm_mc_drv_t mc_drv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_cfg, SHR_E_PARAM);

    sal_memset(&mc_drv, 0, sizeof(bcmtm_mc_drv_t));

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->mc_drv_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_EXIT(drv->mc_drv_get(unit, &mc_drv));

    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_get(unit, &mc_dev));

    mc_head_list = mc_dev->head_info_list;

    mc_group_id = lt_cfg->mc_group_id;

    if ((lt_cfg->port_list_id != -1) &&
        ((lt_cfg->port_list_id + lt_cfg->port_list_num_entries) >
                                        mc_dev->max_repl_head)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memset(&repl_group_info, 0, sizeof(bcmtm_mc_repl_group_info_t));
    if (lt_cfg->port_list_id != -1) {
      for (i = 0; i < lt_cfg->port_list_num_entries; i++) {
           mc_head_id = lt_cfg->port_list_id + i;
           repl_head_info = &mc_head_list[mc_head_id];
           max_repl_count += repl_head_info->num_cfg_repl;
      }
    }
    SHR_BITCOUNT_RANGE(lt_cfg->l2_bitmap, l2_repl, 0, BCMDRD_CONFIG_MAX_PORTS);
    if ((max_repl_count + l2_repl) >
             mc_dev->max_packet_replications) {
         SHR_ERR_EXIT(SHR_E_PARAM);
    }
    mc_dev->repl_count_mc_grp[mc_group_id] = max_repl_count + l2_repl;
    /*
     * If group entry was already configured.
     */
    if ((old_lt_cfg->mc_group_id >= 0) &&
        (old_lt_cfg->port_list_id != -1)) {
        /*
         * Check if any new entry was added or deleted in the TM_MC_PORT_LIST.
         */
        if ((lt_cfg->port_list_id != old_lt_cfg->port_list_id) ||
            (lt_cfg->port_list_num_entries !=
                                    old_lt_cfg->port_list_num_entries)) {
            /*
             * Iterate through old port list and clear mc_group_valid bit.
             */
            for (i = 0; i < old_lt_cfg->port_list_num_entries; i++) {
                mc_head_id = old_lt_cfg->port_list_id + i;
                /*
                 * Skip if Port list entry is used in new config.
                 */
                if ((mc_head_id >= lt_cfg->port_list_id) &&
                    (mc_head_id < (lt_cfg->port_list_id +
                                   lt_cfg->port_list_num_entries))) {
                    continue;
                }
                if (mc_head_id >= mc_dev->max_repl_head) {
                    continue;
                }
                repl_head_info = &mc_head_list[mc_head_id];
                repl_head_info->mc_group_valid = 0;
            }

            /*
             * In case of adding/removing L3-MC port-agg-ID to a group we
             * need to flush out the RQE.
             */
            repl_group_info.rqe_flush = 1;
        }
    }
    if (lt_cfg->port_list_id != -1) {
        repl_group_info.base_ptr = lt_cfg->port_list_id;
        SHR_BITCLR_RANGE(repl_group_info.port_agg_bmp, 0,
                        MAX_PORT_AGG_BMP);
        /*
        * Iterate through all Port list entries.
        * If port_agg_valid is set, the Port list entry has already been
        * configured. Get port_agg_id and add to port_agg_bmp.
        * Else set the mc_group_valid bit and mc_group_id
        * which would help Port List entry addition.
        */
        for (i = 0; i < lt_cfg->port_list_num_entries; i++) {
            mc_head_id = repl_group_info.base_ptr + i;
            repl_head_info = &mc_head_list[mc_head_id];
            if (repl_head_info->port_agg_valid) {
                SHR_BITSET(repl_group_info.port_agg_bmp, repl_head_info->port_agg_id);
            }

            /*
             * If the IPMC group has more than 1 switch copy per egress port,
             * DO_NOT_CUT_THROUGH bit in L3_IPMC table must be set for the
             * ipmc group.
             */
            if (repl_head_info->repl_count > 1) {
                do_not_cut_through = 1;
            }
            repl_head_info->index = mc_head_id;
            repl_head_info->mc_group_valid = 1;
            repl_head_info->mc_group_id = mc_group_id;
            write_to_pt = 1;
        }
        repl_group_info.num_entries = lt_cfg->port_list_num_entries;
        if (repl_head_info && write_to_pt) {
            mc_drv.mc_repl_group_entry_set(unit, mc_group_id, &repl_group_info);
        }
    }
    ipmc_info.l2_mc_disable = lt_cfg->l2_mc_disable;
    mc_drv.mc_ipmc_get(unit, mc_group_id, &ipmc_info);


    SHR_BITCOPY_RANGE(ipmc_info.l3_bitmap, 0, lt_cfg->l3_bitmap, 0, BCMDRD_CONFIG_MAX_PORTS);
    SHR_BITCOPY_RANGE(ipmc_info.l2_bitmap, 0, lt_cfg->l2_bitmap, 0, BCMDRD_CONFIG_MAX_PORTS);
    ipmc_info.do_not_cut_through = do_not_cut_through;
    ipmc_info.valid = 1;

    mc_drv.mc_ipmc_set(unit, mc_group_id, &ipmc_info);
exit:
    SHR_FUNC_EXIT();
}

/*
 * Delete an entry in the TM_MC_GROUP LT.
 */
static int
bcmtm_mc_group_delete (int unit, mc_group_cfg_t *lt_cfg)
{
    bcmtm_mc_repl_group_info_t repl_group_info;
    bcmtm_mc_ipmc_info_t ipmc_info;
    int mc_group_id;
    bcmtm_mc_dev_info_t *mc_dev;
    bcmtm_drv_t *drv;
    bcmtm_mc_drv_t mc_drv;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(lt_cfg, SHR_E_PARAM);

    sal_memset(&repl_group_info, 0, sizeof(bcmtm_mc_repl_group_info_t));
    sal_memset(&mc_drv, 0, sizeof(bcmtm_mc_drv_t));

    SHR_IF_ERR_EXIT(bcmtm_drv_get(unit, &drv));
    SHR_NULL_CHECK(drv->mc_drv_get, SHR_E_UNAVAIL);
    SHR_IF_ERR_EXIT(drv->mc_drv_get(unit, &mc_drv));

    SHR_IF_ERR_EXIT
        (bcmtm_mc_dev_info_get(unit, &mc_dev));

    mc_group_id = lt_cfg->mc_group_id;

    /* Get existing TM_MC_GROUP configuration. */
    SHR_IF_ERR_EXIT
        (mc_drv.mc_repl_group_entry_get(unit, mc_group_id, &repl_group_info));

    repl_group_info.base_ptr = 0;
    SHR_BITCLR_RANGE(repl_group_info.port_agg_bmp, 0,
                    MAX_PORT_AGG_BMP);
    SHR_IF_ERR_EXIT
        (mc_drv.mc_repl_group_entry_set(unit, mc_group_id, &repl_group_info));


    sal_memset(&ipmc_info, 0, sizeof(ipmc_info));
    mc_dev->repl_count_mc_grp[mc_group_id] = 0;

    SHR_IF_ERR_EXIT
        (mc_drv.mc_ipmc_set(unit, mc_group_id, &ipmc_info));
exit:
    SHR_FUNC_EXIT();
}

static int
bcmtm_mc_group_lt_cfg_get (int unit, int mc_group_id,
                            mc_group_cfg_t *mc_group_cfg)
{
    bcmltd_fields_t in;
    bcmltd_fields_t out;
    bcmltd_field_t *data_field;
    size_t num_fid;
    uint64_t fval;
    uint32_t i, idx_count;
    int idx, data, rv;

    SHR_FUNC_ENTER(unit);

    sal_memset(&in, 0, sizeof(bcmltd_fields_t));
    sal_memset(&out, 0, sizeof(bcmltd_fields_t));
    SHR_NULL_CHECK(mc_group_cfg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit, TM_MC_GROUPt, &num_fid));

    idx_count = bcmlrd_field_idx_count_get(unit, TM_MC_GROUPt,
                                           TM_MC_GROUPt_L3_PORTf);

    idx_count += bcmlrd_field_idx_count_get(unit, TM_MC_GROUPt,
                                            TM_MC_GROUPt_L2_PORTf);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_field_list_alloc(unit, num_fid, &in));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_field_list_alloc(unit, num_fid + idx_count - 1, &out));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_default_entry_array_construct(unit,
                                TM_MC_GROUPt, &in));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_field_list_set(unit, &in,
                TM_MC_GROUPt_TM_MC_GROUP_IDf, 0, mc_group_id));

    rv = bcmimm_entry_lookup(unit, TM_MC_GROUPt, &in, &out);

    if (rv == SHR_E_NOT_FOUND) {
        out.count = 0;
    } else {
        SHR_IF_ERR_EXIT(rv);
        mc_group_cfg->mc_group_id = mc_group_id;
        rv = bcmtm_field_list_get(unit, &out,
                        TM_MC_GROUPt_TM_MC_PORT_AGG_LIST_IDf, 0, &fval);
        if (SHR_SUCCESS(rv)) {
            mc_group_cfg->port_list_id = (uint32_t)fval;
        }
        rv = bcmtm_field_list_get(unit, &out,
                        TM_MC_GROUPt_NUM_PORT_LIST_ENTRIESf, 0, &fval);
        if (SHR_SUCCESS(rv)) {
            mc_group_cfg->port_list_num_entries = (uint32_t)fval;
        }
    }
    for (i = 0; i < out.count; i++) {
        data_field = out.field[i];
        if (data_field->id == TM_MC_GROUPt_L3_PORTf) {
            idx = (int)data_field->idx;
            data = (int)data_field->data;
            if (data) {
                SHR_BITSET(mc_group_cfg->l3_bitmap, idx);
            } else {
                SHR_BITCLR(mc_group_cfg->l3_bitmap, idx);
            }
        }
        if (data_field->id == TM_MC_GROUPt_L2_PORTf) {
            idx = (int)data_field->idx;
            data = (int)data_field->data;
            if (data) {
                SHR_BITSET(mc_group_cfg->l2_bitmap, idx);
            } else {
                SHR_BITCLR(mc_group_cfg->l2_bitmap, idx);
            }
        }
    }
exit:
    bcmtm_field_list_free(&in);
    bcmtm_field_list_free(&out);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Parse the key and data fields
 *
 * \param [in] unit Unit number.
 * \param [in] key Array of Key fields
 * \param [in] Data Array of Data fields
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcmtm_mc_group_lt_fields_parse(int unit,
                       const bcmltd_field_t *key,
                       const bcmltd_field_t *data,
                       mc_group_cfg_t *ltcfg,
                       mc_group_cfg_t *old_ltcfg)
{
    const bcmltd_field_t *gen_field;
    uint32_t fid, idx;
    uint64_t fval;

    SHR_FUNC_ENTER(unit);

    /* Parse key field */
    gen_field = key;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case TM_MC_GROUPt_TM_MC_GROUP_IDf:
            ltcfg->mc_group_id = fval;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_mc_group_lt_cfg_get(unit, ltcfg->mc_group_id, old_ltcfg));

    if (old_ltcfg->mc_group_id >= 0) {
        SHR_BITCOPY_RANGE(ltcfg->l3_bitmap, 0, old_ltcfg->l3_bitmap, 0, BCMDRD_CONFIG_MAX_PORTS);
        SHR_BITCOPY_RANGE(ltcfg->l2_bitmap, 0, old_ltcfg->l2_bitmap, 0, BCMDRD_CONFIG_MAX_PORTS);
    }

    /* Parse data field */
    gen_field = data;
    while (gen_field) {
        fid = gen_field->id;
        fval = gen_field->data;

        switch (fid) {
        case TM_MC_GROUPt_TM_MC_PORT_AGG_LIST_IDf:
            ltcfg->port_list_id = fval;
            break;
        case TM_MC_GROUPt_NUM_PORT_LIST_ENTRIESf:
            ltcfg->port_list_num_entries = fval;
            break;
        case TM_MC_GROUPt_L2_MCf:
            ltcfg->l2_mc_disable = !fval;
            break;
        case TM_MC_GROUPt_L3_PORTf:
            idx = gen_field->idx;
            if (fval) {
                SHR_BITSET(ltcfg->l3_bitmap, idx);
            } else {
                SHR_BITCLR(ltcfg->l3_bitmap, idx);
            }
            break;
        case TM_MC_GROUPt_L2_PORTf:
            idx = gen_field->idx;
            if (fval) {
                SHR_BITSET(ltcfg->l2_bitmap, idx);
            } else {
                SHR_BITCLR(ltcfg->l2_bitmap, idx);
            }
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
        gen_field = gen_field->next;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Multicast group callback function for staging.
 *
 * Handle the MC Group creation/modification parameters
 *
 * \param [in] unit Unit number.
 * \param [in] sid This is the logical table ID.
 * \param [in] action This is the desired action for the entry.
 * \param [in] key This is a linked list of the key fields identifying
 * the entry.
 * \param [in] data This is a linked list of the data fields in the
 * modified entry.
 * \param [in] context Is a pointer that was given during registration.
 * The callback can use this pointer to retrieve some context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL MC Group failure.
 */
static int
bcmtm_mc_group_stage_callback(int unit,
                             bcmltd_sid_t sid,
                             uint32_t trans_id,
                             bcmimm_entry_event_t event_reason,
                             const bcmltd_field_t *key,
                             const bcmltd_field_t *data,
                             void *context,
                             bcmltd_fields_t *output_fields)
{
    mc_group_cfg_t lt_cfg;
    mc_group_cfg_t old_lt_cfg;

    SHR_FUNC_ENTER(unit);
    if (output_fields) {
        output_fields->count = 0;
    }
    sal_memset(&lt_cfg, 0, sizeof(mc_group_cfg_t));
    sal_memset(&old_lt_cfg, 0, sizeof(mc_group_cfg_t));

    lt_cfg.port_list_id = -1;
    old_lt_cfg.port_list_id = -1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmtm_mc_group_lt_fields_parse (unit, key, data, &lt_cfg, &old_lt_cfg));

    switch (event_reason) {
    case BCMIMM_ENTRY_INSERT:
    case BCMIMM_ENTRY_UPDATE:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmtm_mc_group_create(unit, &lt_cfg, &old_lt_cfg));
        break;
    case BCMIMM_ENTRY_DELETE:
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmtm_mc_group_delete(unit, &lt_cfg));
        break;
    default:
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief TM multicast group callback structure.
 *
 * This structure contains callback functions that will be conresponding
 * to TM_MC_GROUP logical table entry commit stages.
 */
static bcmimm_lt_cb_t bcmtm_mc_group_imm_callback = {

    /*! Validate function. */
    .validate = NULL,

    /*! Staging function. */
    .stage = bcmtm_mc_group_stage_callback,

    /*! Commit function. */
    .commit = NULL,

    /*! Abort function. */
    .abort = NULL
};

/*!
 * \brief MC Group IMM registration function.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Memory allocation failed.
 */
int
bcmtm_imm_mc_group_register(int unit)
{
    const bcmlrd_map_t *map = NULL;
    int rv;

    SHR_FUNC_ENTER(unit);

    /*
     * To register callback for TM_MC_GROUP LT.
     */
    rv = bcmlrd_map_get(unit, TM_MC_GROUPt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT
            (bcmimm_lt_event_reg(unit,
                                 TM_MC_GROUPt,
                                 &bcmtm_mc_group_imm_callback,
                                 NULL));
    }

exit:
    SHR_FUNC_EXIT();
}

