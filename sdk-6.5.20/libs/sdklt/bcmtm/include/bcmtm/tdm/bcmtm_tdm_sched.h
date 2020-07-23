/*! \file bcmtm_tdm_sched.h
 *
 * Function prototypes for TDM schedulers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_TDM_SCHED_H
#define BCMTM_TDM_SCHED_H


#ifdef _TDM_STANDALONE
    #include <bcmtm_tdm_defines.h>
#else
    #include <bcmtm/tdm/bcmtm_tdm_defines.h>
#endif


/***********************************************************************
 * Internal functions
 */
/*!
 * \brief Get linerate calendar enable indicator by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Calendar linerate enable indicator.
 */
TDM_EXTERN int
bcmtm_tdm_lr_cal_en_get(tdm_mod_t *tdm, int cal_id, int mode);

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
TDM_EXTERN void
bcmtm_tdm_lr_cal_en_set(tdm_mod_t *tdm, int cal_id, int val, int mode);

/*!
 * \brief Get linerate calendar valid indicator by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Linerate calendar valid indicator.
 */
TDM_EXTERN int
bcmtm_tdm_lr_cal_valid_get(tdm_mod_t *tdm, int cal_id, int mode);

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
TDM_EXTERN void
bcmtm_tdm_lr_cal_valid_set(tdm_mod_t *tdm, int cal_id, int val, int mode);

/*!
 * \brief Get linerate calendar length by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Linerate calendar length.
 */
TDM_EXTERN int
bcmtm_tdm_lr_cal_len_get(tdm_mod_t *tdm, int cal_id, int mode);

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
TDM_EXTERN void
bcmtm_tdm_lr_cal_len_set(tdm_mod_t *tdm, int cal_id, int cal_len, int mode);

/*!
 * \brief Get pointer of linerate calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Pointer of linerate calendar.
 */
TDM_EXTERN int *
bcmtm_tdm_lr_cal_get(tdm_mod_t *tdm, int cal_id, int mode);

/*!
 * \brief Reserve ancillary bandwidth in linerate calendar.
 *
 * \param [in] tdm Pointer of TDM module address.
 *
 * \retval TDM_PASS Ancillary slot assignment successfully passed.
 * \retval TDM_FAIL Ancillary slot assignment failed.
 */
int
bcmtm_tdm_lr_cal_ancl_assign(tdm_mod_t *tdm);

/*!
 * \brief Get enable indicator of oversub group calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Oversub group calendar enable indicator.
 */
TDM_EXTERN int
bcmtm_tdm_os_grp_en_get(tdm_mod_t *tdm, int cal_id, int mode);

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
TDM_EXTERN void
bcmtm_tdm_os_grp_en_set(tdm_mod_t *tdm, int cal_id, int val, int mode);

/*!
 * \brief Get valid indicator of oversub group calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Oversub group calendar valid indicator.
 */
TDM_EXTERN int
bcmtm_tdm_os_grp_valid_get(tdm_mod_t *tdm, int cal_id, int mode);

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
TDM_EXTERN void
bcmtm_tdm_os_grp_valid_set(tdm_mod_t *tdm, int cal_id, int val, int mode);

/*!
 * \brief Get number of oversub groups by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Number of oversub groups.
 */
TDM_EXTERN int
bcmtm_tdm_os_grp_num_get(tdm_mod_t *tdm, int cal_id, int mode);

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
TDM_EXTERN void
bcmtm_tdm_os_grp_num_set(tdm_mod_t *tdm, int cal_id, int grp_num, int mode);

/*!
 * \brief Get length of oversub group by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Length of oversub group.
 */
TDM_EXTERN int
bcmtm_tdm_os_grp_len_get(tdm_mod_t *tdm, int cal_id, int mode);

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
TDM_EXTERN void
bcmtm_tdm_os_grp_len_set(tdm_mod_t *tdm, int cal_id, int grp_len, int mode);

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
TDM_EXTERN int *
bcmtm_tdm_os_grp_get(tdm_mod_t *tdm, int cal_id, int grp_id, int mode);



/*!
 * \brief Get packet shaper calendar width by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Packet shaper calendar width.
 */
TDM_EXTERN int
bcmtm_tdm_shp_cal_wid_get(tdm_mod_t *tdm, int cal_id, int mode);

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
TDM_EXTERN void
bcmtm_tdm_shp_cal_wid_set(tdm_mod_t *tdm, int cal_id, int cal_wid, int mode);

/*!
 * \brief Get packet shaper calendar length by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Packet shaper calendar length.
 */
TDM_EXTERN int
bcmtm_tdm_shp_cal_len_get(tdm_mod_t *tdm, int cal_id, int mode);

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
TDM_EXTERN void
bcmtm_tdm_shp_cal_len_set(tdm_mod_t *tdm, int cal_id, int cal_len, int mode);

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
TDM_EXTERN int *
bcmtm_tdm_shp_cal_get(tdm_mod_t *tdm, int cal_id, int sp_id, int mode);

/*!
 * \brief Get enable indicator of packet shaper calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Packet shaper enable indicator.
 */
TDM_EXTERN int
bcmtm_tdm_shp_cal_en_get(tdm_mod_t *tdm, int cal_id, int mode);

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
TDM_EXTERN void
bcmtm_tdm_shp_cal_en_set(tdm_mod_t *tdm, int cal_id, int val, int mode);

/*!
 * \brief Get valid indicator of packet shaper calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] mode Calendar mode.
 *
 * \retval Packet shaper valid indicator.
 */
TDM_EXTERN int
bcmtm_tdm_shp_cal_valid_get(tdm_mod_t *tdm, int cal_id, int mode);

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
TDM_EXTERN void bcmtm_tdm_shp_cal_valid_set(tdm_mod_t *tdm, int cal_id,
                                            int val, int mode);

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
TDM_EXTERN int
bcmtm_tdm_shp_pm2pblk_map_len_get(tdm_mod_t *tdm, int cal_id, int sp_id,
                                  int mode);

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
TDM_EXTERN void
bcmtm_tdm_shp_pm2pblk_map_len_set(tdm_mod_t *tdm, int cal_id, int sp_id,
                                  int cal_len, int mode);

/*!
 * \brief Set valid indicator of packet shaper calendar by calendar ID.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] val Value of valid indicator. 0->invalid, 1->valid.
 * \param [in] len Length of pm2pblk_map.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
TDM_EXTERN int *
bcmtm_tdm_shp_pm2pblk_map_get(tdm_mod_t *tdm, int cal_id, int sp_id, int mode);

#endif /* BCMTM_TDM_SCHED_H */
