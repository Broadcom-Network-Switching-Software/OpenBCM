/*! \file bcmtm_tdm_sched_oversub.c
 *
 * TDM core functions for oversub scheduler calendar.
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
 * \brief Get oversub group calendar by calendar ID and group ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] grp_id Group ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Pointer of one oversub group.
 */
int *
bcmtm_tdm_os_grp_get(tdm_mod_t *tdm, int cal_id, int grp_id, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS &&
        grp_id < TDM_MAX_GRP_WID) {
        if (mode == TDM_CAL_CURR) {
            return ((int *) &(tdm->user_data.cals[cal_id].ovsb.cal[grp_id]));
        } else {
            return ((int *) &(tdm->prev_user_data.cals[cal_id].ovsb.
                              cal[grp_id]));
        }
    }

    return 0;
}

/*!
 * \brief Get number of oversub groups by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Number of oversub groups.
 */
int
bcmtm_tdm_os_grp_num_get(tdm_mod_t *tdm, int cal_id, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        if (mode == TDM_CAL_CURR) {
            return tdm->user_data.cals[cal_id].ovsb.grp_num;
        } else {
            return tdm->prev_user_data.cals[cal_id].ovsb.grp_num;
        }
    }

    return 0;
}

/*!
 * \brief Set number of oversub groups by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] grp_num Number of oversub groups.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_os_grp_num_set(tdm_mod_t *tdm, int cal_id, int grp_num, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        grp_num = (grp_num <= TDM_MAX_GRP_WID) ? grp_num : TDM_MAX_GRP_WID;
        if (mode == TDM_CAL_CURR) {
            tdm->user_data.cals[cal_id].ovsb.grp_num = grp_num;
        } else {
            tdm->prev_user_data.cals[cal_id].ovsb.grp_num = grp_num;
        }
    }
}

/*!
 * \brief Get length of oversub group by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Length of oversub group.
 */
int
bcmtm_tdm_os_grp_len_get(tdm_mod_t *tdm, int cal_id, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        if (mode == TDM_CAL_CURR) {
            return tdm->user_data.cals[cal_id].ovsb.grp_len;
        } else {
            return tdm->prev_user_data.cals[cal_id].ovsb.grp_len;
        }
    }

    return 0;
}

/*!
 * \brief Set length of oversub group by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] grp_len Length of oversub groups.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_os_grp_len_set(tdm_mod_t *tdm, int cal_id, int grp_len, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        grp_len = (grp_len <= TDM_MAX_GRP_LEN) ? grp_len : TDM_MAX_GRP_LEN;
        if (mode == TDM_CAL_CURR) {
            tdm->user_data.cals[cal_id].ovsb.grp_len = grp_len;
        } else {
            tdm->prev_user_data.cals[cal_id].ovsb.grp_len = grp_len;
        }
    }
}

/*!
 * \brief Get enable indicator of oversub group calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Oversub group calendar enable indicator.
 */
int
bcmtm_tdm_os_grp_en_get(tdm_mod_t *tdm, int cal_id, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        if (mode == TDM_CAL_CURR) {
            return tdm->user_data.cals[cal_id].ovsb.cal_en;
        } else {
            return tdm->prev_user_data.cals[cal_id].ovsb.cal_en;
        }
    }
    return 0;
}

/*!
 * \brief Set enable indicator of oversub group calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] val Value of enable indicator. 0->invalid, 1->valid.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_os_grp_en_set(tdm_mod_t *tdm, int cal_id, int val, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        val = val > 0 ? 1 : 0;
        if (mode == TDM_CAL_CURR) {
            tdm->user_data.cals[cal_id].ovsb.cal_en = val;
        } else {
            tdm->prev_user_data.cals[cal_id].ovsb.cal_en = val;
        }
    }
}

/*!
 * \brief Get valid indicator of oversub group calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Oversub group calendar valid indicator.
 */
int
bcmtm_tdm_os_grp_valid_get(tdm_mod_t *tdm, int cal_id, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        if (mode == TDM_CAL_CURR) {
            return tdm->user_data.cals[cal_id].ovsb.cal_valid;
        } else {
            return tdm->prev_user_data.cals[cal_id].ovsb.cal_valid;
        }
    }
    return 0;
}

/*!
 * \brief Set valid indicator of oversub group calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] val Value of valid indicator. 0->invalid, 1->valid.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_os_grp_valid_set(tdm_mod_t *tdm, int cal_id, int val, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        val = val > 0 ? 1 : 0;
        if (mode == TDM_CAL_CURR) {
            tdm->user_data.cals[cal_id].ovsb.cal_valid = val;
        } else {
            tdm->prev_user_data.cals[cal_id].ovsb.cal_valid = val;
        }
    }
}
