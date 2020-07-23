/*! \file bcmcth_ctr_eflex_stats.c
 *
 * Custom tabler handler routines for the enhanced flex counter stats LT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmdrd/bcmdrd_pt.h>

#include <bcmptm/bcmptm.h>
#include <bcmcth/bcmcth_ctr_eflex_drv.h>
#include <bcmcth/bcmcth_state_eflex_drv.h>
#include <bcmcth/bcmcth_ctr_eflex_internal.h>
#include <bcmcth/bcmcth_ctr_eflex_stats.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREFLEX

/* Max flex counter value. */
#define CTR_EFLEX_MAX 0xFFFFFFFFFFFFFFFF

/* Counter indices */
#define CTR_EFLEX_INDEX_B 0
#define CTR_EFLEX_INDEX_A 1

/*! Generate CTR_xxx_EFLEX_STATS LT field ids. */
#define CTR_ING_EGR_STATS_FIDS(x) \
    CTR_ING_EFLEX_STATSt_##x, \
    CTR_EGR_EFLEX_STATSt_##x

/*! Generate STATE_xxx_EFLEX_STATS LT field ids. */
#define STATE_ING_EGR_STATS_FIDS(x) \
    FLEX_STATE_ING_STATSt_##x, \
    FLEX_STATE_EGR_STATSt_##x

/*! Check if pipe is enabled */
#define IS_PIPE_ENABLE(map, p) ((map) & (0x01 << p))

/*******************************************************************************
 * Local definitions
 */
static const bcmlrd_sid_t stats_ltid[][2] = {
        {CTR_ING_EGR_LT_IDS(EFLEX_STATSt)},
        {STATE_ING_EGR_LT_IDS(STATSt)},
};

static const bcmlrd_fid_t stats_action_profile_fid[][2] = {
       {CTR_ING_EFLEX_STATSt_CTR_ING_EFLEX_ACTION_PROFILE_IDf,
        CTR_EGR_EFLEX_STATSt_CTR_EGR_EFLEX_ACTION_PROFILE_IDf},
       {FLEX_STATE_ING_STATSt_FLEX_STATE_ING_ACTION_PROFILE_IDf,
        FLEX_STATE_EGR_STATSt_FLEX_STATE_EGR_ACTION_PROFILE_IDf},
};

static const bcmlrd_fid_t index_fid[][2] = {
        {CTR_ING_EGR_STATS_FIDS(CTR_EFLEX_INDEXf)},
        {STATE_ING_EGR_STATS_FIDS(CTR_EFLEX_INDEXf)},
};

static const bcmlrd_fid_t ctr_a_upper_fid[][2] = {
        {CTR_ING_EGR_STATS_FIDS(CTR_Au_UPPERf)},
        {STATE_ING_EGR_STATS_FIDS(CTR_Au_UPPERf)},
};

static const bcmlrd_fid_t ctr_a_lower_fid[][2] = {
        {CTR_ING_EGR_STATS_FIDS(CTR_Au_LOWERf)},
        {STATE_ING_EGR_STATS_FIDS(CTR_Au_LOWERf)},
};

static const bcmlrd_fid_t ctr_b_fid[][2] = {
        {CTR_ING_EGR_STATS_FIDS(CTR_Bf)},
        {STATE_ING_EGR_STATS_FIDS(CTR_Bf)},
};

/**********************************************************************
* Private functions
 */
/*
 * Function to get eflex component and ingress/egress direction
 * based on LT sid
 */
static int
bcmcth_ctr_eflex_stats_comp_get(int unit,
                                bcmltd_sid_t sid,
                                eflex_comp_t *comp,
                                bool *ingress)
{
    SHR_FUNC_ENTER(unit);

    switch (sid) {
        case CTR_ING_EFLEX_STATSt:
            *comp = CTR_EFLEX;
            *ingress = true;
            break;

        case CTR_EGR_EFLEX_STATSt:
            *comp = CTR_EFLEX;
            *ingress = false;
            break;

        case FLEX_STATE_ING_STATSt:
            *comp = STATE_EFLEX;
            *ingress = true;
            break;

        case FLEX_STATE_EGR_STATSt:
            *comp = STATE_EFLEX;
            *ingress = false;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * Function to validate out fields are provided and are large enough
 * to hold all required fields to be returned during lookup,
 * get_first and get_next operations.
 */
static int
bcmcth_ctr_eflex_stats_out_validate(int unit,
                                    bcmltd_sid_t sid,
                                    eflex_comp_t comp,
                                    bcmltd_fields_t *out)
{
    uint32_t t;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    for (t = 0; t < ctrl->lt_info_num; t++) {
        if (ctrl->ctr_eflex_field_info[t].lt_sid == sid) {
            break;
        }
    }

    if (out) {
        if (t == ctrl->lt_info_num) {
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        } else if (out->count <
                   (ctrl->ctr_eflex_field_info[t].num_data_fields +
                    ctrl->ctr_eflex_field_info[t].num_key_fields)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "out fields supplied is too "
                                  "small to hold the desired fields.\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }
    else {
        LOG_ERROR(BSL_LOG_MODULE,
               (BSL_META_U(unit,
                           "out is not supplied.\n")));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to parse data fields in given STATS entry */
static int
bcmcth_ctr_eflex_stats_entry_parse(int unit,
                                   const bcmltd_fields_t *in,
                                   ctr_eflex_stats_data_t *stats_data)
{
    int select;
    uint32_t i;
    bcmltd_field_t *data_field = NULL;
    eflex_comp_t comp;

    SHR_FUNC_ENTER(unit);

    select = stats_data->ingress ? 0 : 1;
    comp = stats_data->comp;

    /* Get all the key and data fields */
    for (i = 0; i < in->count; i++) {
        data_field = in->field[i];
        if (data_field->id == stats_action_profile_fid[comp][select]) {
            stats_data->action_profile_id = data_field->data;
        } else if (data_field->id == index_fid[comp][select]) {
            stats_data->index = data_field->data;
        } else if (data_field->id == ctr_a_upper_fid[comp][select]) {
            stats_data->new_ctr_a_upper = data_field->data;
        } else if (data_field->id == ctr_a_lower_fid[comp][select]) {
            stats_data->new_ctr_a_lower = data_field->data;
        } else if (data_field->id == ctr_b_fid[comp][select]) {
            stats_data->new_ctr_b = data_field->data;
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Function to populate out fields.
 *
 * \param [in] unit Unit number.
 * \param [in] ingress Ingress or Egress direction.
 * \param [in] ctr_mode Counter mode.
 * \param [in] comp Flex component.
 * \param [in] ctr_buff Pointer to buffer containing counter values.
 * \param [out] out Linked list of the output fields in the modified entry.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
static int
bcmcth_ctr_eflex_stats_parse(int unit,
                             uint32_t bit_idx,
                             ctr_eflex_action_profile_data_t *entry,
                             uint64_t *ctr_buff,
                             bcmltd_fields_t *out)
{
    size_t i;
    int select;
    eflex_comp_t comp;

    SHR_FUNC_ENTER(unit);
    select = entry->ingress ? 0 : 1;
    comp = entry->comp;

    /*
     * The counter fields are mapped as follows
     * NORMAL MODE: u64 buffer
     *              ctr_buff[0] - CTR_B
     *              ctr_buff[1] - CTR_A_LOWER
     *
     * WIDE MODE:   u64 buffer
     *              ctr_buff[0] - CTR_A_LOWER
     *              ctr_buff[1] - CTR_A_UPPER
     *
     * SLIM MODE:   u64 buffer
     *              ctr_buff[0] - CTR_A_LOWER
     *
     * The CCI APIs provide the counter buffer in the format above during read.
     */

    for (i = 0; i < out->count; i++) {
        out->field[i]->idx = 0;
        out->field[i]->next = 0;
        out->field[i]->flags = 0;

        /* Set CTR_B logical field value. */
        if (out->field[i]->id == ctr_b_fid[comp][select]) {
            if (entry->ctr_mode == NORMAL_MODE ||
                            entry->ctr_mode == NORMAL_DOUBLE_MODE) {
                out->field[i]->data = ctr_buff[CTR_EFLEX_INDEX_B];
            }
        } else if (out->field[i]->id == ctr_a_lower_fid[comp][select]) {
            /* Set CTR_A_LOWER logical field value. */
            if (entry->ctr_mode == SLIM_MODE) {
                out->field[i]->data = ctr_buff[CTR_EFLEX_INDEX_B];
            } else if (entry->ctr_mode == NORMAL_MODE ||
                                  entry->ctr_mode == NORMAL_DOUBLE_MODE) {
                out->field[i]->data = ctr_buff[CTR_EFLEX_INDEX_A];
            } else if (entry->ctr_mode == WIDE_MODE) {
                out->field[i]->data = ctr_buff[CTR_EFLEX_INDEX_B];
            }
        } else if (out->field[i]->id == ctr_a_upper_fid[comp][select]) {
            /* Set CTR_A_UPPER logical field value. */
            if (entry->ctr_mode == WIDE_MODE) {
                out->field[i]->data = ctr_buff[CTR_EFLEX_INDEX_A];
            }
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Function to populate counter buffer to be written to PT.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Pointer to ACTION_PROFILE LT entry for given stats index.
 * \param [in] stats_data Pointer to stats info struct.
 * \param [in] ctr_buff Pointer to buffer containing counter values.
 * \param [out] out_fields Linked list of the output fields.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */

/* Function to populate counter buffer to write to PT */
static int
bcmcth_ctr_eflex_buffer_populate(int unit,
                                 uint32_t bit_idx,
                                 ctr_eflex_action_profile_data_t *entry,
                                 ctr_eflex_stats_data_t *stats_data,
                                 uint64_t *ctr_buff,
                                 bcmltd_fields_t *out)
{
    int select;
    uint32_t i;
    eflex_comp_t comp;

    SHR_FUNC_ENTER(unit);
    select  = entry->ingress ? 0 : 1;
    comp = entry->comp;

    /*
     * The counter fields are mapped as follows
     * NORMAL MODE: u64 buffer
     *              ctr_buff[0] - CTR_B
     *              ctr_buff[1] - CTR_A_LOWER
     *
     * WIDE MODE:   u64 buffer
     *              ctr_buff[0] - CTR_A_LOWER
     *              ctr_buff[1] - CTR_A_UPPER
     *
     * SLIM MODE:   u64 buffer
     *              ctr_buff[0] - CTR_A_LOWER
     *
     * The CCI APIs expect the counter buffer in the format above during write.
     */

    switch (entry->ctr_mode) {
        case NORMAL_MODE:
        case NORMAL_DOUBLE_MODE:
            /* CTR_B and CTR_A_LOWER fields are valid */
            for (i = 0; i < out->count; i++) {
                if (out->field[i]->id == ctr_b_fid[comp][select]) {
                    /* Use new value */
                    if (stats_data->upd_ctr_b) {
                        ctr_buff[CTR_EFLEX_INDEX_B] =
                            stats_data->new_ctr_b;
                    } else {
                        /* Retain previous value */
                        ctr_buff[CTR_EFLEX_INDEX_B] =
                            out->field[i]->data;
                    }
                } else if (out->field[i]->id == ctr_a_lower_fid[comp][select]){
                    if (stats_data->upd_ctr_a_lower) {
                        ctr_buff[CTR_EFLEX_INDEX_A] =
                            stats_data->new_ctr_a_lower;
                    } else {
                        ctr_buff[CTR_EFLEX_INDEX_A] =
                            out->field[i]->data;
                    }
                }
            } /* end FOR */
            break;

        case WIDE_MODE:
            /* CTR_A_LOWER and CTR_A_UPPER fields are valid */
            for (i = 0; i < out->count; i++) {
                if (out->field[i]->id == ctr_a_lower_fid[comp][select]) {
                    if (stats_data->upd_ctr_a_lower) {
                        ctr_buff[CTR_EFLEX_INDEX_B] =
                            stats_data->new_ctr_a_lower;
                    } else {
                        ctr_buff[CTR_EFLEX_INDEX_B] =
                            out->field[i]->data;
                    }
                } else if (out->field[i]->id == ctr_a_upper_fid[comp][select]){
                    if (stats_data->upd_ctr_a_upper) {
                        ctr_buff[CTR_EFLEX_INDEX_A] =
                            stats_data->new_ctr_a_upper;
                    } else {
                        ctr_buff[CTR_EFLEX_INDEX_A] =
                            out->field[i]->data;
                    }
                }
            } /* end FOR */
            break;

        case SLIM_MODE:
            /* CTR_A_LOWER field is valid */
            for (i = 0; i < out->count; i++) {
                if (out->field[i]->id == ctr_a_lower_fid[comp][select]){
                    if (stats_data->upd_ctr_a_lower) {
                        ctr_buff[CTR_EFLEX_INDEX_B] =
                            stats_data->new_ctr_a_lower;
                    } else {
                        ctr_buff[CTR_EFLEX_INDEX_B] = out->field[i]->data;
                    }
                }
            } /* end FOR */
            break;

        default:
            break;
    } /* end SWITCH */

    SHR_FUNC_EXIT();
}

/*!
 * Function to validate that given STATS LT counter index
 * is within the ACTION_PROFILE counter range.
 */
static int
bcmcth_ctr_eflex_stats_idx_validate(int unit,
                                    uint32_t index,
                                    ctr_eflex_action_profile_data_t *entry)
{
    uint32_t abs_ctr_idx = 0, min_ctr_idx = 0;

    SHR_FUNC_ENTER(unit);

    /* Get absolute counter index */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_abs_idx_get(unit,
                                      index,
                                      entry,
                                      &abs_ctr_idx));

    /* For given entry, get absolute counter index of 1st counter */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_abs_idx_get(unit,
                                      0,
                                      entry,
                                      &min_ctr_idx));

    /* Return error if the given stats counter index is outside the range */
    if ((abs_ctr_idx < min_ctr_idx) ||
        (abs_ctr_idx > (min_ctr_idx + entry->num_ctrs))) {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
            "Invalid counter index %d provided.\n"
            "abs_ctr_idx = %d min_ctr_idx = %d num_ctrs = %d\n"),
            index, abs_ctr_idx, min_ctr_idx, entry->num_ctrs));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Function to determine counter value in global pipe mode
 * based on the update mode.
 */
static int
bcmcth_ctr_eflex_ctr_buff_update(int unit,
                                 uint64_t **ctr_buff_global,
                                 uint64_t *ctr_buff,
                                 ctr_eflex_action_profile_data_t *entry)
{
    uint32_t pipe_idx, ctr_field_num, num_pipes, ctr_field;
    bcmptm_cci_update_mode_t upd_mode;
    uint64_t ctr_avg_total, ctr_avg_num;
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    /* Number of pipes */
    num_pipes = ctrl->ctr_eflex_device_info->num_pipes;

    /* Number of counter fields */
    ctr_field_num = entry->ingress ? ctrl->ing_ctr_field_num :
                                     ctrl->egr_ctr_field_num;

    /*
     * For SLIM and WIDE mode, since only CTR A is valid,
     * use update mode for CTR A only.
     * Within a counter entry, CTR A is the MSB counter.
     */
    upd_mode = entry->ctr_update_mode[ctr_field_num - 1];

    /* For MIN update mode, set all counter values to max value */
    for (ctr_field = 0; ctr_field < ctr_field_num; ctr_field++) {
        if (entry->ctr_update_mode[ctr_field] == MIN_VALUE) {
                ctr_buff[ctr_field] = CTR_EFLEX_MAX;
        }
    }

    /* Determine counter value based on update mode */
    for (ctr_field = 0; ctr_field < ctr_field_num; ctr_field++) {
        ctr_avg_total = ctr_avg_num = 0;

        /*
         * For NORMAL mode, both CTR A and CTR B are valid.
         * Hence use update modes from both.
         */
        if (entry->ctr_mode == NORMAL_MODE || entry->ctr_mode == NORMAL_DOUBLE_MODE) {
            upd_mode = entry->ctr_update_mode[ctr_field];
        }

        for (pipe_idx = 0; pipe_idx < num_pipes; pipe_idx++) {
            switch (upd_mode) {
                case MAX_VALUE:
                    /* Determine max counter value across all pipes */
                    if (ctr_buff_global[ctr_field][pipe_idx] >
                        ctr_buff[ctr_field]) {
                        ctr_buff[ctr_field] =
                            ctr_buff_global[ctr_field][pipe_idx];
                    }
                    break;

                case MIN_VALUE:
                    /* Determine min counter value across all pipes */
                    if (ctr_buff_global[ctr_field][pipe_idx] <
                        ctr_buff[ctr_field]) {
                        ctr_buff[ctr_field] =
                            ctr_buff_global[ctr_field][pipe_idx];
                    }
                    break;

                case SETBIT:
                case XOR:
                case XORBIT:
                    /* Counter value is OR'ed across all pipes */
                    ctr_buff[ctr_field] |= ctr_buff_global[ctr_field][pipe_idx];
                    break;
                case CLRBIT:
                    /* Counter value is AND'ed across all pipes */
                    ctr_buff[ctr_field] &= ctr_buff_global[ctr_field][pipe_idx];
                    break;

                case AVERAGE_VALUE:
                    /*
                     * Average counter value is calculated using
                     * non-zero value across all pipes
                     */
                    if (ctr_buff_global[ctr_field][pipe_idx]) {
                        ctr_avg_total += ctr_buff_global[ctr_field][pipe_idx];
                        ctr_avg_num++;
                    }
                    break;

                case NOOP:
                case SET_TO_CVALUE:
                case INC_BY_CVALUE:
                case DEC_BY_CVALUE:
                case RDEC_FROM_CVALUE:
                    /* Counter value is added across all pipes */
                    ctr_buff[ctr_field] += ctr_buff_global[ctr_field][pipe_idx];
                    break;

                default:
                    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit,
                            "Invalid counter update mode %d. \n"),
                            entry->ctr_update_mode[ctr_field]));
                    SHR_ERR_EXIT(SHR_E_PARAM);
            }
        } /* end inner FOR */

        if (entry->ctr_update_mode[ctr_field] == AVERAGE_VALUE && ctr_avg_num) {
            ctr_buff[ctr_field] = ctr_avg_total / ctr_avg_num;
        }
    }  /* end outter FOR */

exit:
    SHR_FUNC_EXIT();
}

/*!
 * Function to calculate pool_idx, counter index and bit index
 * within the counter for given stats index.
 *
 * In SLIM mode, the counter field number will be returned in bit_idx.
 */
static int
bcmcth_ctr_eflex_stats_idx_calc(int unit,
                                uint32_t index,
                                ctr_eflex_action_profile_data_t *entry,
                                uint32_t *pool_idx,
                                uint32_t *ctr_idx,
                                uint32_t *bit_idx)
{
    uint32_t max_ctr_num, abs_ctr_idx = 0, num_slim_ctrs = 0;
    uint32_t tmp_ctr_idx, counter_factor;
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;

    /* Get max number of counters per pool */
    max_ctr_num = entry->ingress ? ctrl->ing_pool_ctr_num[entry->pool_idx] :
                                   ctrl->egr_pool_ctr_num[entry->pool_idx];

    /* SLIM counter calculation */
    if (entry->ctr_mode == SLIM_MODE) {
        /* Get number of slim counters per pool entry */
        num_slim_ctrs = device_info->num_slim_ctrs;

        /* Get absolute counter index */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_abs_idx_get(unit,
                                          index,
                                          entry,
                                          &abs_ctr_idx));

        /* Calculate pool idx, ctr index and slim ctr field (bit_index) */
        *pool_idx = abs_ctr_idx / (max_ctr_num * num_slim_ctrs);
        tmp_ctr_idx =  abs_ctr_idx -
                  ((*pool_idx) * max_ctr_num * num_slim_ctrs);
        if (device_info->slim_ctr_row == TRUE ) {
            *bit_idx = tmp_ctr_idx % 3 ;
            *ctr_idx = tmp_ctr_idx;

         } else {
            *bit_idx = tmp_ctr_idx / max_ctr_num;
            *ctr_idx = tmp_ctr_idx % max_ctr_num;
         }
    } else {
        /* Get absolute counter index */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_abs_idx_get(unit,
                                          index,
                                          entry,
                                          &abs_ctr_idx));
        if (entry->ctr_mode == NORMAL_DOUBLE_MODE) {
            counter_factor = 2;
        } else {
            counter_factor = 1;
        }
        /* Calculate pool idx and ctr index */
        *pool_idx = abs_ctr_idx / (max_ctr_num * counter_factor);
        *ctr_idx = abs_ctr_idx % (max_ctr_num * counter_factor);
    }

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,
            "index %d\n"
            "pool_idx %d, ctr_idx %d, bit_idx %d\n"),
            index, *pool_idx, *ctr_idx, *bit_idx));

exit:
    SHR_FUNC_EXIT();
}

/*! Function to read STATS entry */
static int
bcmcth_ctr_eflex_stats_read(int unit,
                            uint64_t req_flags,
                            ctr_eflex_action_profile_data_t *entry,
                            uint32_t index,
                            bcmltd_fields_t *out)
{
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    bcmdrd_sid_t drd_sid = {0};
    bcmltd_sid_t rsp_ltid = {0};
    uint32_t rsp_flags = 0, rsp_entry_wsize;
    uint32_t entry_sz, *pt_entry_buff = NULL;
    uint32_t field_num = 0, i;
    int select, n = 0;
    uint64_t *ctr_buff = NULL;
    uint32_t c[2] = {0};
    ctr_eflex_field_t *lt_field = NULL;
    ctr_eflex_control_t *ctrl = NULL;
    uint32_t pipe_idx, num_pipes;
    uint8_t pipe_unique = 0;
    uint64_t **ctr_buff_global = NULL;
    uint32_t inst_idx, num_insts;
    eflex_comp_t comp;
    uint32_t pool_idx, ctr_idx, bit_idx = 0;
    bcmptm_cci_ctr_info_t ctr_info = {0};

    SHR_FUNC_ENTER(unit);
    select  = entry->ingress ? 0 : 1;
    comp = entry->comp;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry->ingress,
                                            entry->comp,
                                            &pipe_unique));

    num_pipes = ctrl->ctr_eflex_device_info->num_pipes;
    num_insts = ctrl->ctr_eflex_device_info->num_pipe_instance;

    /*
     * For device has no instance concept, the value may be initialized as 0,
     * Adjusted to 1 for unified process.
     */
    if (!num_insts) {
        num_insts = 1;
    }

    /* Calculate pool idx, ctr index and bit index within counter index */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_stats_idx_calc(unit,
                                         index,
                                         entry,
                                         &pool_idx,
                                         &ctr_idx,
                                         &bit_idx));
    /* Number of counter fields */
    field_num = entry->ingress ? ctrl->ing_ctr_field_num :
                                 ctrl->egr_ctr_field_num;

    /* Allocate u64 counter buffer for each counter field */
    SHR_ALLOC(ctr_buff, (field_num * sizeof(uint64_t)), "bcmcthCtrEflexStats");
    SHR_NULL_CHECK(ctr_buff, SHR_E_MEMORY);
    sal_memset(ctr_buff, 0, (field_num * sizeof(uint64_t)));

    /* Entry size in words */
    entry_sz = entry->ingress ? ctrl->ing_entry_wsize :
                                ctrl->egr_entry_wsize;
    rsp_entry_wsize = entry_sz;

    /* Allocate PT entry buffer */
    SHR_ALLOC(pt_entry_buff, (entry_sz * sizeof(uint32_t)),
              "bcmcthCtrEflexStats");
    SHR_NULL_CHECK(pt_entry_buff, SHR_E_MEMORY);
    sal_memset(pt_entry_buff, 0, (entry_sz * sizeof(uint32_t)));

    /*
     * In global pipe operating mode, allocate a 2D array with
     * number of flex counters are rows and number of pipes as columns.
     * This array will store the counter values for each pipe.
     */
    if (!pipe_unique) {
        SHR_ALLOC(ctr_buff_global, (field_num * sizeof(uint64_t*)),
                  "bcmcthCtrEflexCtrBuffGlobal");
        SHR_NULL_CHECK(ctr_buff_global, SHR_E_MEMORY);
        sal_memset(ctr_buff_global, 0, (field_num * sizeof(uint64_t*)));

        for (i = 0; i < field_num; i++) {
            SHR_ALLOC(ctr_buff_global[i], (num_pipes * sizeof(uint64_t)),
                      "bcmcthCtrEflexCtrBuffPipe");
            SHR_NULL_CHECK(ctr_buff_global[i], SHR_E_MEMORY);
            sal_memset(ctr_buff_global[i], 0, (num_pipes * sizeof(uint64_t)));
        }
    }

    /* Initialize physical table index */
    pt_dyn_info.index = ctr_idx;

    /* Read counter values for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        pt_dyn_info.tbl_inst = pipe_idx;

        /* Read counter values from all instances in pipe */
        for (inst_idx = 0; inst_idx < num_insts; inst_idx++) {
            /* Get counter table sid */
            SHR_IF_ERR_EXIT
                (bcmcth_eflex_pool_sid_get(unit,
                                           entry->ingress,
                                           entry->comp,
                                           inst_idx,
                                           entry->ingress ?
                                           entry->state_ext.inst
                                                     : entry->state_ext.egr_inst,
                                           pool_idx,
                                           &drd_sid));

            /* Ignore disabled pipes */
            if (!bcmdrd_pt_index_valid(unit, drd_sid, pipe_idx, -1)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Skipping disabled pipe %d. \n"),
                    pipe_idx));
                continue;
            }

            if (entry->ctr_mode == SLIM_MODE) {
                /* Initialize slim counter info struct */
                ctr_info.flags = req_flags;
                ctr_info.sid = drd_sid;
                ctr_info.txfm_fld = bit_idx;
                ctr_info.in_pt_dyn_info = &pt_dyn_info;

                /* Call CCI API to read slim ctr */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_cci_slim_ctr_read(unit,
                                              &ctr_info,
                                              pt_entry_buff,
                                              rsp_entry_wsize));
             } else if (entry->ctr_mode == NORMAL_DOUBLE_MODE) {
                /* Initialize slim counter info struct */
                pt_dyn_info.index = ctr_idx/2;
                ctr_info.flags = req_flags;
                ctr_info.sid = drd_sid;
                ctr_info.txfm_fld = index % 2;
                ctr_info.in_pt_dyn_info = &pt_dyn_info;

                /* Call CCI API to read slim ctr */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_cci_normal_double_ctr_read(unit,
                                              &ctr_info,
                                              pt_entry_buff,
                                              rsp_entry_wsize));
            } else {
                /* Read counter table */
                SHR_IF_ERR_VERBOSE_EXIT(bcmptm_ireq_read(
                    unit,
                    req_flags,
                    drd_sid,
                    &pt_dyn_info,
                    NULL,
                    rsp_entry_wsize,
                    stats_ltid[comp][select],
                    pt_entry_buff,
                    &rsp_ltid,
                    &rsp_flags));
            }

             /*
              * For each counter field,
              * copy 64-bit value from PT entry buffer
              */
             for (i = 0; i < field_num; i++) {
                 lt_field = entry->ingress ? &(ctrl->ing_lt_field[i]) :
                                             &(ctrl->egr_lt_field[i]);

                 /* Calculate number of bits to be copied */
                 n = lt_field->maxbit - lt_field->minbit + 1;

                 /* Get the counter value for field */
                 bcmptm_cci_buff32_field_copy(c, 0,
                                              pt_entry_buff,
                                              lt_field->minbit, n);

                 if (pipe_unique) {
                     /*
                      * In pipe unique mode, update the counter field with
                      * current counter value directly.
                      */
                    ctr_buff[i] += (((uint64_t)c[1] << 32) | c[0]);
                 } else {
                     /* In global pipe mode, store the current counter value. */
                     ctr_buff_global[i][pipe_idx] +=
                                  (((uint64_t)c[1] << 32) | c[0]);
                 }
            }
        }
    }

    /*
     * In global pipe mode, parse the stored counter values for all pipes
     * to determine the counter value based on the update modes.
     */
    if (!pipe_unique) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_ctr_buff_update(unit,
                                              ctr_buff_global,
                                              ctr_buff,
                                              entry));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_stats_parse(unit,
                                      bit_idx,
                                      entry,
                                      ctr_buff,
                                      out));

exit:
    SHR_FREE(pt_entry_buff);
    SHR_FREE(ctr_buff);

    if (!pipe_unique) {
        for (i = 0; i < field_num; i++) {
            if (ctr_buff_global) {
                SHR_FREE(ctr_buff_global[i]);
            }
        }
        SHR_FREE(ctr_buff_global);
    }

    SHR_FUNC_EXIT();
}

/*!
 * Function to populate ctr_buff_global[pipe][counter] array with
 * counter value in ctr_buff[counter] based on update mode for each
 * counter and each pipe
 */
static int
bcmcth_ctr_eflex_ctr_buff_global_update(
    int unit,
    uint64_t **ctr_buff_global,
    uint64_t *ctr_buff,
    ctr_eflex_action_profile_data_t *entry)
{
    uint32_t i, field_num, valid_pipemap, pipe_idx;
    uint32_t num_div;
    uint64_t ctr_val;
    int blktype, num_valid_pipes;
    uint8_t pipe_unique;
    ctr_eflex_control_t *ctrl = NULL;
    const char ipipe[] = "ipipe";
    const char epipe[] = "epipe";
    const bcmdrd_chip_info_t *cinfo;

    SHR_FUNC_ENTER(unit);

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, entry->comp, &ctrl));

    /* Number of counter fields */
    field_num = entry->ingress ? ctrl->ing_ctr_field_num :
                                 ctrl->egr_ctr_field_num;

    /* Get blktype */
    cinfo = bcmdrd_dev_chip_info_get(unit);
    if (entry->ingress) {
        blktype = bcmdrd_chip_blktype_get_by_name(cinfo, ipipe);
    } else {
        blktype = bcmdrd_chip_blktype_get_by_name(cinfo, epipe);
    }

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            entry->ingress,
                                            entry->comp,
                                            &pipe_unique));

    /* Get valid pipe bitmap */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmdrd_dev_valid_blk_pipes_get(unit, 0, blktype, &valid_pipemap));

    /* Get number of valid pipes */
    SHR_BITCOUNT_RANGE(&valid_pipemap, num_valid_pipes, 0, 32);

    if (pipe_unique) {
        for (i = 0; i < field_num; i++) {
            /* For pipe unique mode, write ctr value to configured pipe only */
            ctr_buff_global[entry->start_pipe_idx][i] = ctr_buff[i];
        }
    } else {
        for (i = 0; i < field_num; i++) {
            /*
             * In global pipe mode, save number of pipes across which counter
             * value should be divided for DEC and RDEC update modes
             */
            num_div = num_valid_pipes;

            ctr_val = ctr_buff[i];
            for (pipe_idx = entry->start_pipe_idx;
                 pipe_idx < entry->end_pipe_idx;
                 pipe_idx++) {
                /* If not valid pipe, skip it */
                if (!IS_PIPE_ENABLE(valid_pipemap, pipe_idx)) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Skipping disabled pipe %d.\n"),
                        pipe_idx));
                    continue;
                }

                if (entry->ctr_update_mode[i] == NOOP ||
                    entry->ctr_update_mode[i] == SET_TO_CVALUE ||
                    entry->ctr_update_mode[i] == INC_BY_CVALUE ||
                    entry->ctr_update_mode[i] == MAX_VALUE ||
                    entry->ctr_update_mode[i] == AVERAGE_VALUE) {
                    /*
                     * For NOOP, SET, INC, MAX, AVG update modes
                     * write counter value to first valid pipe and skip the rest
                     */
                    ctr_buff_global[pipe_idx][i] = ctr_val;
                    break;
                } else if (entry->ctr_update_mode[i] == MIN_VALUE ||
                           entry->ctr_update_mode[i] == SETBIT ||
                           entry->ctr_update_mode[i] == CLRBIT ||
                           entry->ctr_update_mode[i] == XORBIT ||
                           entry->ctr_update_mode[i] == XOR) {
                    /*
                     * For MIN, SETBIT, XOR  CLRBIT XORBIT update modes
                     * write counter value to all valid pipes
                     */
                    ctr_buff_global[pipe_idx][i] = ctr_val;
                } else if (entry->ctr_update_mode[i] == DEC_BY_CVALUE ||
                           entry->ctr_update_mode[i] == RDEC_FROM_CVALUE) {
                    /*
                     * For DEC, RDEC update modes
                     * divide counter value amongst all valid pipes
                     */
                    if (ctr_val % num_valid_pipes == 0) {
                        /*
                         * Counter value can be equally divided amongst
                         * valid pipes
                         */
                        ctr_buff_global[pipe_idx][i] = ctr_val/num_valid_pipes;
                    } else {
                        /*
                         * If counter value cannot be equally divided amongst
                         * valid pipes, then the last pipe will get remaining
                         * unequal portion
                         */
                        ctr_buff_global[pipe_idx][i] = ctr_val/num_valid_pipes;
                        num_div--;
                        if (num_div == 0) {
                            ctr_buff_global[pipe_idx][i] +=
                                ctr_val % num_valid_pipes;
                        }
                    }
                }
            } /* end inner FOR */
        } /* end outter FOR */
    }

exit:
    SHR_FUNC_EXIT();
}

/* Function to write STATS for an LT entry */
static int
bcmcth_ctr_eflex_stats_write(int unit,
                             ctr_eflex_action_profile_data_t *entry,
                             ctr_eflex_stats_data_t *stats_data)
{
    int select;
    uint16_t n = 0;
    bcmbd_pt_dyn_info_t pt_dyn_info = {0};
    uint32_t entry_sz, *pt_entry_buff = NULL;
    bcmdrd_sid_t drd_sid = {0};
    bcmltd_sid_t rsp_ltid = {0};
    uint32_t i, rsp_flags = 0, field_num;
    uint64_t *ctr_buff = NULL;
    ctr_eflex_field_t *lt_field = NULL;
    ctr_eflex_control_t *ctrl = NULL;
    uint32_t pipe_idx, inst_idx, num_insts, num_pipes = 0;
    eflex_comp_t comp;
    uint32_t pool_idx, ctr_idx, bit_idx = 0;
    bcmptm_cci_ctr_info_t ctr_info = {0};
    uint64_t **ctr_buff_global = NULL;
    bcmltd_fields_t out = {0};
    size_t num_fields = 0;
    uint32_t *data = NULL;
    bool ctr_inst = true, single_inst = true;
    uint8_t pipe_unique;

    SHR_FUNC_ENTER(unit);
    select  = entry->ingress ? 0 : 1;
    comp = entry->comp;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    num_pipes = ctrl->ctr_eflex_device_info->num_pipes;
    num_insts = ctrl->ctr_eflex_device_info->num_pipe_instance;

    /*
     * For device has no instance concept, the value may be initialized as 0,
     * Adjusted to 1 for unified process.
     */
    if (!num_insts) {
        num_insts = 1;
    }

    if (num_insts > 1) {
        single_inst = false;
    }

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            select,
                                            comp,
                                            &pipe_unique));

    /* Calculate pool idx, ctr index and bit index within counter index */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_stats_idx_calc(unit,
                                         stats_data->index,
                                         entry,
                                         &pool_idx,
                                         &ctr_idx,
                                         &bit_idx));

    /* Number of counter fields */
    field_num = entry->ingress ? ctrl->ing_ctr_field_num :
                                 ctrl->egr_ctr_field_num;

    /* Allocate u64 counter buffer for each counter field */
    SHR_ALLOC(ctr_buff, (field_num * sizeof(uint64_t)), "bcmcthCtrEflexStats");
    SHR_NULL_CHECK(ctr_buff, SHR_E_MEMORY);
    sal_memset(ctr_buff, 0, (field_num * sizeof(uint64_t)));

    /* Entry size in words */
    entry_sz = entry->ingress ? ctrl->ing_entry_wsize :
                                ctrl->egr_entry_wsize;

    /* Allocate PT entry buffer */
    SHR_ALLOC(pt_entry_buff, (entry_sz * sizeof(uint32_t)),
              "bcmcthCtrEflexStats");
    SHR_NULL_CHECK(pt_entry_buff, SHR_E_MEMORY);
    sal_memset(pt_entry_buff, 0, (entry_sz * sizeof(uint32_t)));

    SHR_ALLOC(data, (entry_sz * sizeof(uint32_t)),
              "bcmcthCtrEflexStatsInst");
    SHR_NULL_CHECK(data, SHR_E_MEMORY);
    sal_memset(data, 0, (entry_sz * sizeof(uint32_t)));

    /*
     * Allocate a 2D array with number of pipes as rows and
     * number of flex counters as columns.
     * This array will store the counter values to be written for each pipe.
     */
    SHR_ALLOC(ctr_buff_global, (num_pipes * sizeof(uint64_t *)),
              "bcmcthCtrEflexCtrBuffPipe");
    SHR_NULL_CHECK(ctr_buff_global, SHR_E_MEMORY);
    sal_memset(ctr_buff_global, 0, (num_pipes * sizeof(uint64_t *)));

    for (i = 0; i < num_pipes; i++) {
        SHR_ALLOC(ctr_buff_global[i], (field_num * sizeof(uint64_t)),
                  "bcmcthCtrEflexCtrBuffGlobal");
        SHR_NULL_CHECK(ctr_buff_global[i], SHR_E_MEMORY);
        sal_memset(ctr_buff_global[i], 0, (field_num * sizeof(uint64_t)));
    }

    /* Allocate out fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlrd_field_count_get(unit,
                                stats_ltid[comp][select],
                                &num_fields));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_fields_alloc(unit, &out, num_fields));

    /* Setup out ids and count */
    out.count = 0;

    out.field[out.count++]->id = ctr_b_fid[comp][select];
    out.field[out.count++]->id = ctr_a_lower_fid[comp][select];
    out.field[out.count++]->id = ctr_a_upper_fid[comp][select];

    /* Get current stats for this action_id entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_stats_read(unit,
                                     0,
                                     entry,
                                     stats_data->index,
                                     &out));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_buffer_populate(unit,
                                          bit_idx,
                                          entry,
                                          stats_data,
                                          ctr_buff,
                                          &out));
    /*
     * Populate ctr_buff_global[pipe][counter] array with counter
     * value being written based on update mode for each counter
     * and each pipe
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_ctr_buff_global_update(unit,
                                                 ctr_buff_global,
                                                 ctr_buff,
                                                 entry));

    /* Initialize physical table index */
    pt_dyn_info.index = ctr_idx;

    /* Write counter values for all applicable pipes */
    for (pipe_idx = entry->start_pipe_idx;
         pipe_idx < entry->end_pipe_idx;
         pipe_idx++) {

        pt_dyn_info.tbl_inst = pipe_idx;

        /* Prepare PT entry buffer */
        for (i = 0; i < field_num; i++) {
            uint32_t c[2] = {0};
            lt_field = entry->ingress ? &(ctrl->ing_lt_field[i]) :
                                        &(ctrl->egr_lt_field[i]);

            /* Get number of bits to be copied */
            n = lt_field->maxbit - lt_field->minbit + 1;

            c[0] = (uint32_t)(ctr_buff_global[pipe_idx][i]);
            c[1] = (uint32_t)(ctr_buff_global[pipe_idx][i] >> 32);

            /* Copy to PT entry buffer */
            bcmptm_cci_buff32_field_copy(pt_entry_buff, lt_field->minbit,
                                         c, 0, n);
        }

        /* Write counter values to all instances in pipe */
        for (inst_idx = 0; inst_idx < num_insts; inst_idx++) {
            /* Get counter table sid */
            SHR_IF_ERR_EXIT
                (bcmcth_eflex_pool_sid_get(unit,
                                           entry->ingress,
                                           entry->comp,
                                           inst_idx,
                                           entry->ingress ?
                                           entry->state_ext.inst :
                                              entry->state_ext.egr_inst,
                                           pool_idx,
                                           &drd_sid));

            /* Ignore disabled pipes */
            if (!bcmdrd_pt_index_valid(unit, drd_sid, pipe_idx, -1)) {
                LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Skipping disabled pipe %d. \n"),
                    pipe_idx));
                continue;
            }

            /* Adjust counter value prepare for PT */
            if (single_inst == false) {
                /* The first instance consumes counter value */
                if (pipe_unique) {
                    ctr_inst = (inst_idx == 0) ? true : false;
                } else {
                    ctr_inst = \
                        ((pipe_idx == 0) && (inst_idx == 0)) ? true : false;
                }
            }

            if (entry->ctr_mode == SLIM_MODE) {
                /* Initialize slim counter info struct */
                ctr_info.sid = drd_sid;
                ctr_info.txfm_fld = bit_idx;
                ctr_info.in_pt_dyn_info = &pt_dyn_info;

                /* Call CCI API to write slim ctr */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_cci_slim_ctr_write(unit,
                                              &ctr_info,
                                              ctr_inst ? pt_entry_buff : data));
            } else if (entry->ctr_mode == NORMAL_DOUBLE_MODE) {
                /* Initialize slim counter info struct */
                ctr_info.sid = drd_sid;
                ctr_info.txfm_fld =  stats_data->index % 2;
                ctr_info.in_pt_dyn_info = &pt_dyn_info;

                /* Call CCI API to write slim ctr */
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmptm_cci_normal_double_ctr_write(unit,
                                              &ctr_info,
                                              ctr_inst ? pt_entry_buff : data));
            } else {
                /* Write back to counter table */
                SHR_IF_ERR_VERBOSE_EXIT(bcmptm_ireq_write(
                    unit,
                    0,
                    drd_sid,
                    &pt_dyn_info,
                    NULL,
                    ctr_inst ? pt_entry_buff : data,
                    stats_ltid[comp][select],
                    &rsp_ltid,
                    &rsp_flags));
            }
        } /* end INNER FOR */
    } /* end OUTTER FOR */

exit:
    SHR_FREE(ctr_buff);
    SHR_FREE(pt_entry_buff);
    SHR_FREE(data);
    for (i = 0; i < num_pipes; i++) {
        if (ctr_buff_global) {
            SHR_FREE(ctr_buff_global[i]);
        }
    }
    SHR_FREE(ctr_buff_global);

    /* Restore out.count */
    out.count = num_fields;
    bcmcth_ctr_eflex_fields_free(unit, &out);

    SHR_FUNC_EXIT();
}

/*! Function to lookup STATS for an LT entry. */
static int
bcmcth_ctr_eflex_stats_lt_lookup(int unit,
                                 bool ingress,
                                 eflex_comp_t comp,
                                 uint64_t req_flags,
                                 const bcmltd_fields_t *in,
                                 bcmltd_fields_t *out)
{
    int select;
    uint32_t i;
    ctr_eflex_action_profile_data_t entry = {0};
    ctr_eflex_stats_data_t stats_data = {0};
    ctr_eflex_control_t *ctrl = NULL;
    bcmltd_field_t *data_field = NULL;

    SHR_FUNC_ENTER(unit);
    select  = ingress ? 0 : 1;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    stats_data.ingress = entry.ingress = ingress;
    stats_data.comp = entry.comp = comp;

    /* Setup out fields */
    out->count = 0;
    out->field[out->count++]->id = ctr_b_fid[comp][select];
    out->field[out->count++]->id = ctr_a_lower_fid[comp][select];
    out->field[out->count++]->id = ctr_a_upper_fid[comp][select];

    /* Get all the key fields */
    for (i = 0; i < in->count; i++) {
        data_field = in->field[i];
        if (data_field->id == stats_action_profile_fid[comp][select]) {
            stats_data.action_profile_id = data_field->data;
        } else if (data_field->id == index_fid[comp][select]) {
            stats_data.index = data_field->data;
        }
    }

    entry.action_profile_id = stats_data.action_profile_id;

    /* Lookup ACTION_PROFILE table entry with given action id key */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lookup(unit, &entry));

    /* Validate the stats counter index is within allocated range */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_stats_idx_validate(unit,
                                             stats_data.index,
                                             &entry));
    /* Number of counter fields */
    entry.num_update_mode = entry.ingress ? ctrl->ing_ctr_field_num :
                                            ctrl->egr_ctr_field_num;

    /* Determine first and last pipe number */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_num_pipes_get(unit,
                                        entry.ingress,
                                        entry.comp,
                                        entry.pipe_idx,
                                        &entry.start_pipe_idx,
                                        &entry.end_pipe_idx));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_stats_read(unit,
                                     req_flags,
                                     &entry,
                                     stats_data.index,
                                     out));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmcth_ctr_eflex_stats_lt_update(int unit,
                                 bool ingress,
                                 eflex_comp_t comp,
                                 const bcmltd_fields_t *in)
{
    int select;
    uint32_t i;
    bcmltd_field_t *data_field = NULL;
    ctr_eflex_action_profile_data_t entry = {0};
    ctr_eflex_stats_data_t stats_data = {0};
    ctr_eflex_control_t *ctrl = NULL;

    SHR_FUNC_ENTER(unit);
    select = ingress ? 0 : 1;

    stats_data.ingress = entry.ingress = ingress;
    stats_data.comp = entry.comp = comp;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    /* Parse the key fields */
    for (i = 0; i < in->count; i++) {
        data_field = in->field[i];
        if (data_field->id == stats_action_profile_fid[comp][select]) {
            stats_data.action_profile_id = data_field->data;
        } else if (data_field->id == index_fid[comp][select]) {
            stats_data.index = data_field->data;
        }
    }
    entry.action_profile_id = stats_data.action_profile_id;

    /* Lookup entry with given action profile id key in ACTION_PROFILE LT */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lookup(unit, &entry));

    /* Adjust num_ctrs if flex counter hitbit or SLIM mode is configured */
    if (entry.ctr_mode != NORMAL_DOUBLE_MODE) {
        SHR_IF_ERR_VERBOSE_EXIT
          (bcmcth_ctr_eflex_action_profile_num_ctrs_adjust(unit, &entry));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_stats_idx_validate(unit,
                                             stats_data.index,
                                             &entry));

    /* Determine first and last pipe number */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_num_pipes_get(unit,
                                        entry.ingress,
                                        entry.comp,
                                        entry.pipe_idx,
                                        &entry.start_pipe_idx,
                                        &entry.end_pipe_idx));

    /* Parse the data fields */
    for (i = 0; i < in->count; i++) {
        data_field = in->field[i];
        if (data_field->id == ctr_a_upper_fid[comp][select]) {
            if (entry.ctr_mode == WIDE_MODE) {
                stats_data.new_ctr_a_upper = data_field->data;
                stats_data.upd_ctr_a_upper = true;
            }
        } else if (data_field->id == ctr_a_lower_fid[comp][select]) {
            stats_data.new_ctr_a_lower = data_field->data;
            stats_data.upd_ctr_a_lower = true;
        } else if (data_field->id == ctr_b_fid[comp][select]) {
            if (entry.ctr_mode == NORMAL_MODE) {
                stats_data.new_ctr_b = data_field->data;
                stats_data.upd_ctr_b = true;
            }
        }
    }

    /* Write STATS LT entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_stats_write(unit, &entry, &stats_data));

exit:
    SHR_FUNC_EXIT();
}

/*! Function to get first STATS LT entry. */
static int
bcmcth_ctr_eflex_stats_lt_first(int unit,
                                bool ingress,
                                eflex_comp_t comp,
                                uint64_t req_flags,
                                bcmltd_fields_t *out)
{
    int select;
    ctr_eflex_action_profile_data_t entry = {0};

    SHR_FUNC_ENTER(unit);
    select = ingress ? 0 : 1;

    entry.ingress = ingress;
    entry.comp = comp;

    /* Get first ACTION_PROFILE LT entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_entry_get(unit, true, &entry));

    /* Determine first and last pipe number */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_num_pipes_get(unit,
                                        entry.ingress,
                                        entry.comp,
                                        entry.pipe_idx,
                                        &entry.start_pipe_idx,
                                        &entry.end_pipe_idx));

    /* Setup out fields */
    out->count = 0;
    out->field[out->count++]->id = ctr_b_fid[comp][select];
    out->field[out->count++]->id = ctr_a_lower_fid[comp][select];
    out->field[out->count++]->id = ctr_a_upper_fid[comp][select];

    /* Get first STATS LT entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_stats_read(unit, req_flags, &entry, 0, out));

    /* Setup key fields in out list */
    out->field[out->count]->id = stats_action_profile_fid[comp][select];
    out->field[out->count]->data = entry.action_profile_id;
    out->field[out->count++]->idx = 0;

    out->field[out->count]->id = index_fid[comp][select];
    out->field[out->count]->data = 0;
    out->field[out->count++]->idx = 0;

exit:
    SHR_FUNC_EXIT();
}

/*! Function to get next STATS LT entry. */
static int
bcmcth_ctr_eflex_stats_lt_next(int unit,
                               bool ingress,
                               eflex_comp_t comp,
                               uint64_t req_flags,
                               const bcmltd_fields_t *in,
                               bcmltd_fields_t *out)
{
    int select;
    ctr_eflex_action_profile_data_t entry = {0};
    ctr_eflex_stats_data_t stats_data = {0};

    SHR_FUNC_ENTER(unit);
    select = ingress ? 0 : 1;

    /* Parse entry key and data fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_stats_entry_parse(unit, in, &stats_data));

    entry.ingress = ingress;
    entry.comp = comp;
    entry.action_profile_id = stats_data.action_profile_id;

    /* Lookup ACTION_PROFILE table entry with given action profile id key */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_action_profile_lookup(unit, &entry));

    /* Determine first and last pipe number */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_num_pipes_get(unit,
                                        entry.ingress,
                                        entry.comp,
                                        entry.pipe_idx,
                                        &entry.start_pipe_idx,
                                        &entry.end_pipe_idx));

    /* Setup out fields */
    out->count = 0;
    out->field[out->count++]->id = ctr_b_fid[comp][select];
    out->field[out->count++]->id = ctr_a_lower_fid[comp][select];
    out->field[out->count++]->id = ctr_a_upper_fid[comp][select];

    /*
     * For a given action profile id, the counter index range
     * is from 0 to (NUM_COUNTERS - 1).
     */

    /*
     * Check if next counter index is within 0 to (NUM_COUNTERS - 1) range
     * for current action profile id
     */
    if ((stats_data.index + 1) < entry.num_ctrs) {
        /* Get next counter index STATS LT entry */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_stats_read(unit,
                                         req_flags,
                                         &entry,
                                         (stats_data.index + 1),
                                         out));

        /* Setup key fields  in out list */
        out->field[out->count]->id = stats_action_profile_fid[comp][select];
        out->field[out->count]->data = entry.action_profile_id;
        out->field[out->count++]->idx = 0;

        out->field[out->count]->id = index_fid[comp][select];
        out->field[out->count]->data = (stats_data.index + 1);
        out->field[out->count++]->idx = 0;
    } else {
        /* Get next ACTION_PROFILE LT entry */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_action_profile_entry_get(unit, false, &entry));

        /* Get first counter index of next action profile id in STATS LT */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmcth_ctr_eflex_stats_read(unit, req_flags, &entry, 0, out));

        /* Setup key fields  in out list */
        out->field[out->count]->id = stats_action_profile_fid[comp][select];
        out->field[out->count]->data = entry.action_profile_id;
        out->field[out->count++]->idx = 0;

        out->field[out->count]->id = index_fid[comp][select];
        out->field[out->count]->data = 0;
        out->field[out->count++]->idx = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/*! Function to get STATS for an LT entry limit value. */
static int
bcmcth_ctr_eflex_stats_lt_ent_limit_get(int unit,
                                        bool ingress,
                                        eflex_comp_t comp,
                                        uint64_t *count)
{
    ctr_eflex_control_t *ctrl = NULL;
    bcmcth_ctr_eflex_device_info_t *device_info = NULL;
    const bcmlrd_map_t *map = NULL;
    uint8_t pipe_unique;
    uint32_t num_pipes = 1, k;
    size_t ent_limit;
    int rv, select;

    SHR_FUNC_ENTER(unit);
    select = ingress ? 0 : 1;

    /* Get flex control struct */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_eflex_ctrl_get(unit, comp, &ctrl));

    device_info = ctrl->ctr_eflex_device_info;

    /* Get pipe operating mode */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_pipe_opermode_get(unit,
                                            ingress,
                                            comp,
                                            &pipe_unique));

    if (device_info) {
        num_pipes = pipe_unique ? 1: device_info->num_pipes;
    }

    /* Check if LT is supported for given device */
    rv = bcmlrd_map_get(unit, stats_ltid[comp][select], &map);

    if (SHR_SUCCESS(rv) && map) {
        for (k = 0; k < map->table_attr->attributes; k++) {
            if (map->table_attr->attr[k].key ==
                    BCMLRD_MAP_TABLE_ATTRIBUTE_ENTRY_LIMIT) {
                ent_limit = map->table_attr->attr[k].value;
                *count = ent_limit / num_pipes;
                break;
            }
        }
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
            (BSL_META_U(unit,"STATS LT not supported. Set limit to 0\n")));
        *count = 0;

    }

    LOG_VERBOSE(BSL_LOG_MODULE,
        (BSL_META_U(unit,"comp %d ingress %d entry_limit 0x%" PRIx64 "\n"),
        comp, ingress, *count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief CTR_xxx_EFLEX_STATS / FLEX_STATE_xxx_STATS LT
 *        validation handler function.
 *
 * \param [in] unit Unit Number.
 * \param [in] opcode LT opcode.
 * \param [out] in Input field values.
 * \param [in] arg Transform arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_ctr_eflex_stats_validate(int unit,
                                bcmlt_opcode_t opcode,
                                const bcmltd_fields_t *in,
                                const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    /* Insert and Delete not supported */
    if ((opcode == BCMLT_OPCODE_INSERT) ||
        (opcode == BCMLT_OPCODE_DELETE)) {
        SHR_ERR_EXIT(SHR_E_DISABLED);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief CTR_xxx_EFLEX_STATS / FLEX_STATE_xxx_STATS LT
 *        insert handler function.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_arg Operation arguments.
 * \param [in] in Input field values.
 * \param [out] out Output field values (not applicable).
 * \param [in] arg Transform arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_ctr_eflex_stats_insert(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief CTR_xxx_EFLEX_STATS / FLEX_STATE_xxx_STATS LT
 *        lookup handler function.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_arg Operation arguments.
 * \param [in] in Input field values.
 * \param [out] out Output field values.
 * \param [in] arg Transform arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_ctr_eflex_stats_lookup(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_generic_arg_t *arg)
{
    bool ingress;
    eflex_comp_t comp;
    uint64_t req_flags;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);
    SHR_NULL_CHECK(in, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    /* Get eflex comp */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_stats_comp_get(unit, arg->sid, &comp, &ingress));

    /* Check if out fields provided are large enough */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_stats_out_validate(unit, arg->sid, comp, out));

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_stats_lt_lookup(unit,
                                          ingress,
                                          comp,
                                          req_flags,
                                          in,
                                          out));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief CTR_xxx_EFLEX_STATS / FLEX_STATE_xxx_STATS LT
 *        delete handler function.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_arg Operation arguments.
 * \param [in] in Input field values.
 * \param [out] out Output field values.
 * \param [in] arg Transform arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmcth_ctr_eflex_stats_delete(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_generic_arg_t *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief CTR_xxx_EFLEX_STATS / FLEX_STATE_xxx_STATS LT
 *        update handler function.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_arg Operation arguments.
 * \param [in] in Input field values.
 * \param [out] out Output field values.
 * \param [in] arg Transform arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_ctr_eflex_stats_update(int unit,
                              const bcmltd_op_arg_t *op_arg,
                              const bcmltd_fields_t *in,
                              bcmltd_fields_t *out,
                              const bcmltd_generic_arg_t *arg)
{
    bool ingress;
    eflex_comp_t comp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    /* Get eflex comp */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_stats_comp_get(unit, arg->sid, &comp, &ingress));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_stats_lt_update(unit, ingress, comp, in));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief CTR_xxx_EFLEX_STATS / FLEX_STATE_xxx_STATS LT
 *        get first handler function.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_arg Operation arguments.
 * \param [in] in Input field values.
 * \param [out] out Output field values.
 * \param [in] arg Transform arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_ctr_eflex_stats_first(int unit,
                             const bcmltd_op_arg_t *op_arg,
                             const bcmltd_fields_t *in,
                             bcmltd_fields_t *out,
                             const bcmltd_generic_arg_t *arg)
{
    bool ingress;
    eflex_comp_t comp;
    uint64_t req_flags;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    /* Get eflex comp */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_stats_comp_get(unit, arg->sid, &comp, &ingress));

    /* Check if out fields provided are large enough */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_stats_out_validate(unit, arg->sid, comp, out));

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    /* Get first STATS LT entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_stats_lt_first(unit, ingress, comp, req_flags, out));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief CTR_xxx_EFLEX_STATS / FLEX_STATE_xxx_STATS LT
 *        get next handler function.
 *
 * \param [in] unit Unit Number.
 * \param [in] op_arg Operation arguments.
 * \param [in] in Input field values.
 * \param [out] out Output field values.
 * \param [in] arg Transform arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_ctr_eflex_stats_next(int unit,
                            const bcmltd_op_arg_t *op_arg,
                            const bcmltd_fields_t *in,
                            bcmltd_fields_t *out,
                            const bcmltd_generic_arg_t *arg)
{
    bool ingress;
    eflex_comp_t comp;
    uint64_t req_flags;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);
    SHR_NULL_CHECK(out, SHR_E_PARAM);

    /* Get eflex comp */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_stats_comp_get(unit, arg->sid, &comp, &ingress));

    /* Check if out fields provided are large enough */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_stats_out_validate(unit, arg->sid, comp, out));

    req_flags = bcmptm_lt_entry_attrib_to_ptm_req_flags(op_arg->attrib);

    /* Get next STATS LT entry */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_stats_lt_next(unit,
                                        ingress,
                                        comp,
                                        req_flags,
                                        in,
                                        out));

exit:
    SHR_FUNC_EXIT();
}
/*!
 * \brief CTR_xxx_EFLEX_STATS / FLEX_STATE_xxx_STATS LT
 *        get actual entry limit.
 *
 * \param [in] unit Unit Number.
 * \param [in] trans_id LT transaction ID.
 * \param [in] sid logical table ID.
 * \param [in] table_arg Table arguments.
 * \param [out] table_data The actual entry limit.
 * \param [in] arg Component arguments.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_ctr_eflex_stats_table_entry_limit_get(
        int unit,
        uint32_t trans_id,
        bcmltd_sid_t sid,
        const bcmltd_table_entry_limit_arg_t *table_arg,
        bcmltd_table_entry_limit_t *table_data,
        const bcmltd_generic_arg_t *arg)
{
    bool ingress;
    eflex_comp_t comp;

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(arg, SHR_E_PARAM);

    /* Get eflex comp */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_stats_comp_get(unit, arg->sid, &comp, &ingress));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmcth_ctr_eflex_stats_lt_ent_limit_get(unit,
                                                 ingress,
                                                 comp,
                                                 &table_data->entry_limit));

exit:
    SHR_FUNC_EXIT();
}
