/*! \file bcmtm_tdm_sched_linerate.c
 *
 * TDM core functions for linerate scheduler calendar.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef _TDM_STANDALONE
    #include <bcmtm_tdm_top.h>
#else
    #include <bcmtm/tdm/bcmtm_tdm_top.h>
#endif


/***********************************************************************
 * Internal functions
 */

/*!
 * \brief Get linerate calendar length by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Linerate calendar length.
 */
int
bcmtm_tdm_lr_cal_len_get(tdm_mod_t *tdm, int cal_id, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        if (mode == TDM_CAL_CURR) {
            return tdm->user_data.cals[cal_id].lr.cal_len;
        } else {
            return tdm->prev_user_data.cals[cal_id].lr.cal_len;
        }
    }

    return 0;
}

/*!
 * \brief Set linerate calendar length by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] cal_len Calendar length.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_lr_cal_len_set(tdm_mod_t *tdm, int cal_id, int cal_len, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        cal_len = cal_len <= TDM_MAX_CAL_LEN ? cal_len : TDM_MAX_CAL_LEN;
        if (mode == TDM_CAL_CURR) {
            tdm->user_data.cals[cal_id].lr.cal_len = cal_len;
        } else {
            tdm->prev_user_data.cals[cal_id].lr.cal_len = cal_len;
        }
    }
}

/*!
 * \brief Get pointer of linerate calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Pointer of linerate calendar.
 */
int
*bcmtm_tdm_lr_cal_get(tdm_mod_t *tdm, int cal_id, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        if (mode == TDM_CAL_CURR) {
            return tdm->user_data.cals[cal_id].lr.cal;
        } else {
            return tdm->prev_user_data.cals[cal_id].lr.cal;
        }
    }

    return 0;
}

/*!
 * \brief Get linerate calendar enable indicator by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Calendar linerate enable indicator.
 */
int
bcmtm_tdm_lr_cal_en_get(tdm_mod_t *tdm, int cal_id, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        if (mode == TDM_CAL_CURR) {
            return tdm->user_data.cals[cal_id].lr.cal_en;
        } else {
            return tdm->prev_user_data.cals[cal_id].lr.cal_en;
        }
    }
    return 0;
}

/*!
 * \brief Set linerate calendar enable indicator by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] val Config value for calendar enable. 0-disable, 1->enable.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_lr_cal_en_set(tdm_mod_t *tdm, int cal_id, int val, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        val = val > 0 ? 1 : 0;
        if (mode == TDM_CAL_CURR) {
            tdm->user_data.cals[cal_id].lr.cal_en = val;
        } else {
            tdm->prev_user_data.cals[cal_id].lr.cal_en = val;
        }
    }
}

/*!
 * \brief Get linerate calendar valid indicator by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Linerate calendar valid indicator.
 */
int
bcmtm_tdm_lr_cal_valid_get(tdm_mod_t *tdm, int cal_id, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        if (mode == TDM_CAL_CURR) {
            return tdm->user_data.cals[cal_id].lr.cal_valid;
        } else {
            return tdm->prev_user_data.cals[cal_id].lr.cal_valid;
        }
    }
    return 0;
}

/*!
 * \brief Set linerate calendar valid indicator by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] val Config value for calendar valid. 0-invalid, 1->valid.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_lr_cal_valid_set(tdm_mod_t *tdm, int cal_id, int val, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        val = val > 0 ? 1 : 0;
        if (mode == TDM_CAL_CURR) {
            tdm->user_data.cals[cal_id].lr.cal_valid = val;
        } else {
            tdm->prev_user_data.cals[cal_id].lr.cal_valid = val;
        }
    }
}

/*!
 * \brief Reserve ancillary bandwidth in linerate calendar.
 *
 * \param [in] tdm Pointer of TDM module address.
 *
 * \retval TDM_PASS Ancillary slot assignment successfully passed.
 * \retval TDM_FAIL Ancillary slot assignment failed.
 */
int
bcmtm_tdm_lr_cal_ancl_assign(tdm_mod_t *tdm)
{
    int i, j, k, k_prev, k_idx, idx_up, idx_dn, result = TDM_PASS;
    int ancl_num, cal_len, token_empty, token_ancl, lr_en;
    int *cal_main;
    int pipe_id;

    pipe_id = bcmtm_tdm_pipe_id_get(tdm);
    ancl_num = bcmtm_tdm_pipe_ancl_limit_get(tdm);
    cal_len = bcmtm_tdm_lr_cal_len_get(tdm, pipe_id, 0);
    token_empty = bcmtm_tdm_token_empty_get(tdm);
    token_ancl = bcmtm_tdm_token_ancl_get(tdm);
    lr_en = bcmtm_tdm_pipe_lr_en_get(tdm);

    /* allocate ANCL slots */
    cal_main = bcmtm_tdm_lr_cal_get(tdm, pipe_id, 0);
    if (ancl_num > 0) {
        TDM_PRINT1("Allocate %d ANCL slots\n\n", ancl_num);
        if (lr_en == TDM_TRUE) {
            k = 0;
            for (j = 0; j < ancl_num; j++) {
                idx_up = 0;
                idx_dn = 0;
                k_idx = 0;
                k_prev = k;
                if (j == (ancl_num - 1)) {
                    k = cal_len - 1;
                } else {
                    k = k_prev + ((cal_len - k_prev) / (ancl_num - j)) - 1;
                }
                for (i = k; i > 0 && i < cal_len; i--) {
                    if (cal_main[i] == token_empty) {
                        idx_up = i;
                        break;
                    }
                }
                for (i = k; i < cal_len; i++) {
                    if (cal_main[i] == token_empty) {
                        idx_dn = i;
                        break;
                    }
                }
                if (idx_up > 0 || idx_dn > 0) {
                    if (idx_up > 0 && idx_dn > 0) {
                        k_idx = ((k - idx_up) < (idx_dn - k)) ?
                                idx_up : idx_dn;
                    } else if (idx_up > 0) {
                        k_idx = idx_up;
                    } else if (idx_dn > 0) {
                        k_idx = idx_dn;
                    }
                }
                if (k_idx == 0) {
                    for (i = cal_len-1; i > 0; i--) {
                        if (cal_main[i] == token_empty) {
                            k_idx = i;
                            break;
                        }
                    }
                }
                if (k_idx > 0 && k_idx < cal_len) {
                    cal_main[k_idx] = token_ancl;
                    TDM_PRINT2("Allocate %d-th ANCL slot at index #%03d\n",
                                j+1, k_idx);
                } else {
                    TDM_WARN1("Failed in %d-th ANCL slot allocation\n",j);
                    result = TDM_FAIL;
                }
            }
        } else {
            k_prev = 0;
            for (j = 0; j < ancl_num; j++) {
                k_idx = k_prev + ((cal_len - k_prev) / (ancl_num - j)) - 1;
                cal_main[k_idx] = token_ancl;
                k_prev = k_idx + 1;
                TDM_PRINT1("Allocate ANCL slot at index #%03d\n", k_idx);
            }
        }
        TDM_SML_BAR
    }

    return result;
}
