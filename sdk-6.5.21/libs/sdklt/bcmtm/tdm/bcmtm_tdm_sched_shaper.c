/*! \file bcmtm_tdm_sched_shaper.c
 *
 * TDM core functions for packet shaper scheduler calendar.
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
 * \brief Get packet shaper calendar width by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Packet shaper calendar width.
 */
int
bcmtm_tdm_shp_cal_wid_get(tdm_mod_t *tdm, int cal_id, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        if (mode == TDM_CAL_CURR) {
            return tdm->user_data.cals[cal_id].shp.cal_wid;
        } else {
            return tdm->prev_user_data.cals[cal_id].shp.cal_wid;
        }
    }

    return 0;
}

/*!
 * \brief Set packet shaper calendar width by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] grp_wid Calendar width.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_shp_cal_wid_set(tdm_mod_t *tdm, int cal_id, int cal_wid, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        cal_wid = (cal_wid <= TDM_MAX_SHP_WID) ? cal_wid : TDM_MAX_SHP_WID;
        if (mode == TDM_CAL_CURR) {
            tdm->user_data.cals[cal_id].shp.cal_wid = cal_wid;
        } else {
            tdm->prev_user_data.cals[cal_id].shp.cal_wid = cal_wid;
        }
    }
}

/*!
 * \brief Get packet shaper calendar length by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Packet shaper calendar length.
 */
int
bcmtm_tdm_shp_cal_len_get(tdm_mod_t *tdm, int cal_id, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        if (mode == TDM_CAL_CURR) {
            return tdm->user_data.cals[cal_id].shp.cal_len;
        } else {
            return tdm->prev_user_data.cals[cal_id].shp.cal_len;
        }
    }

    return 0;
}

/*!
 * \brief Set packet shaper calendar length by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] grp_len Calendar length.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_shp_cal_len_set(tdm_mod_t *tdm, int cal_id, int cal_len, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        cal_len = cal_len <= TDM_MAX_SHP_WID ? cal_len : TDM_MAX_SHP_WID;
        if (mode == TDM_CAL_CURR) {
            tdm->user_data.cals[cal_id].shp.cal_len = cal_len;
        } else {
            tdm->prev_user_data.cals[cal_id].shp.cal_len = cal_len;
        }
    }
}

/*!
 * \brief Get packet shaper calendar by calendar ID and subpipe ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] sp_id Subpipe ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Pionter of packet shaper calendar.
 */
int *
bcmtm_tdm_shp_cal_get(tdm_mod_t *tdm, int cal_id, int sp_id, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        if (mode == TDM_CAL_CURR) {
            return ((int *) &(tdm->user_data.cals[cal_id].shp.cal[sp_id]));
        } else {
            return ((int *) &(tdm->prev_user_data.cals[cal_id].shp.
                              cal[sp_id]));
        }
    }

    return 0;
}

/*!
 * \brief Get enable indicator of packet shaper calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Packet shaper enable indicator.
 */
int
bcmtm_tdm_shp_cal_en_get(tdm_mod_t *tdm, int cal_id, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        if (mode == TDM_CAL_CURR) {
            return tdm->user_data.cals[cal_id].shp.cal_en;
        } else {
            return tdm->prev_user_data.cals[cal_id].shp.cal_en;
        }
    }
    return 0;
}

/*!
 * \brief Set enable indicator of packet shaper calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] val Value of enable indicator.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_shp_cal_en_set(tdm_mod_t *tdm, int cal_id, int val, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        val = val > 0 ? 1 : 0;
        if (mode == TDM_CAL_CURR) {
            tdm->user_data.cals[cal_id].shp.cal_en = val;
        } else {
            tdm->prev_user_data.cals[cal_id].shp.cal_en = val;
        }
    }
}

/*!
 * \brief Get valid indicator of packet shaper calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Packet shaper valid indicator.
 */
int
bcmtm_tdm_shp_cal_valid_get(tdm_mod_t *tdm, int cal_id, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        if (mode == TDM_CAL_CURR) {
            return tdm->user_data.cals[cal_id].shp.cal_valid;
        } else {
            return tdm->prev_user_data.cals[cal_id].shp.cal_valid;
        }
    }
    return 0;
}

/*!
 * \brief Set valid indicator of packet shaper calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] val Value of valid indicator. 0->invalid, 1->valid.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_shp_cal_valid_set(tdm_mod_t *tdm, int cal_id, int val, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        val = val > 0 ? 1 : 0;
        if (mode == TDM_CAL_CURR) {
            tdm->user_data.cals[cal_id].shp.cal_valid = val;
        } else {
            tdm->prev_user_data.cals[cal_id].shp.cal_valid = val;
        }
    }
}

/*!
 * \brief Get pm2pblk_map length by calendar ID and subpipe ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] sp_id Subpipe ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Packet shaper enable indicator.
 */
int
bcmtm_tdm_shp_pm2pblk_map_len_get(tdm_mod_t *tdm, int cal_id, int sp_id,
int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        /* Note: always use cals[0]. */
        if (mode == TDM_CAL_CURR) {
            return tdm->user_data.cals[0].shp.pm2pblk_len;
        } else {
            return tdm->prev_user_data.cals[0].shp.pm2pblk_len;
        }
    }

    return 0;
}

/*!
 * \brief Set pm2pblk_map length by calendar ID and subpipe ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] sp_id Subpipe ID.
 * \param [in] cal_len Calendar length.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_shp_pm2pblk_map_len_set(tdm_mod_t *tdm, int cal_id, int sp_id,
int cal_len, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        cal_len = cal_len <= TDM_MAX_NUM_PMS ? cal_len : TDM_MAX_NUM_PMS;
        /* Note: always use cals[0]. */
        if (mode == TDM_CAL_CURR) {
            tdm->user_data.cals[0].shp.pm2pblk_len = cal_len;
        } else {
            tdm->prev_user_data.cals[0].shp.pm2pblk_len = cal_len;
        }
    }
}

/*!
 * \brief Get pm2pblk_map by calendar ID and subpipe ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] sp_id Subpipe ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Pointer of pm2pblk_map.
 */
int *
bcmtm_tdm_shp_pm2pblk_map_get(tdm_mod_t *tdm, int cal_id, int sp_id, int mode)
{
    if (cal_id < TDM_MAX_NUM_CALS) {
        /* Note: always use cals[0]. */
        if (mode == TDM_CAL_CURR) {
            return tdm->user_data.cals[0].shp.pm2pblk_map;
        } else {
            return tdm->prev_user_data.cals[0].shp.pm2pblk_map;
        }
    }

    return 0;
}
