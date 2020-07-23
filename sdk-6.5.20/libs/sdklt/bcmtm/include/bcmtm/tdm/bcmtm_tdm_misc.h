/*! \file bcmtm_tdm_misc.h
 *
 * Miscellaneous functions of TDM core algorithm.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_TDM_MISC_H
#define BCMTM_TDM_MISC_H


#ifdef _TDM_STANDALONE
    #include <bcmtm_tdm_defines.h>
#else
    #include <bcmtm/tdm/bcmtm_tdm_defines.h>
#endif


/***********************************************************************
 * Internal functions
 */
/*!
 * \brief Initialize TDM core resources.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS  Initilaization passed.
 * \retval TDM_FAIL  Initilaization failed.
 */
TDM_EXTERN int
bcmtm_tdm_core_init(tdm_mod_t *tdm);

/*!
 * \brief TDM null funtion.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS.
 */
TDM_EXTERN int
bcmtm_tdm_core_null(tdm_mod_t *tdm);

/*!
 * \brief Populate linerate calendar based on vmap.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS Lineate calendar generation completed successfully.
 * \retval TDM_FAIL Linerate calendar generation failed.
 */
TDM_EXTERN int
bcmtm_tdm_vmap_drv(tdm_mod_t *tdm);


/*!
 * \brief Get speed class by port token.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Port mode.
 *
 * \retval Speed class.
 */
TDM_EXTERN int
bcmtm_tdm_port_speed_idx_get(tdm_mod_t *tdm, int port, int mode);

/*!
 * \brief Get number of slots for a speed.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] speed Port Ethernet speed.
 *
 * \retval Number of slots.
 */
TDM_EXTERN int
bcmtm_tdm_port_slots_get(tdm_mod_t *tdm, int port, int mode);

/*!
 * \brief Get number of slots for a port.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Port mode.
 *
 * \retval Number of slots.
 */
TDM_EXTERN int
bcmtm_tdm_speed_slots_get(tdm_mod_t *tdm, int port_speed);


/***********************************************************************
 * Internal functions: checker
 */
/*!
 * \brief Check sister-port-space restriction among all linerate calendars.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS  Check passed.
 * \retval TDM_FAIL  Check failed.
 */
TDM_EXTERN int
bcmtm_tdm_sister_chk(tdm_mod_t *tdm);

/*!
 * \brief Check same-port-space restriction among all linerate calendars.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS  Check passed.
 * \retval TDM_FAIL  Check failed.
 */
TDM_EXTERN int
bcmtm_tdm_same_chk(tdm_mod_t *tdm);

/*!
 * \brief Get number of slots allocated for a given token in a calendar.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of slots allocated for a given token.
 */
TDM_EXTERN int
bcmtm_tdm_cal_token_count(tdm_mod_t *tdm, int cal_id, int token);

/*!
 * \brief Check linerate port bandwidth in a given calendar.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS  Check passed.
 * \retval TDM_FAIL  Check failed.
 */
TDM_EXTERN int
bcmtm_tdm_lr_port_bw_chk(tdm_mod_t *tdm, int cal_id, int port);

/*!
 * \brief Check if given port is a general port.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 *
 * \retval TDM_TRUE  Port check passed.
 * \retval TDM_FALSE Port check failed.
 *
 * General port = front panel + CPU + MGMT + Loopback
 */
TDM_EXTERN int
bcmtm_tdm_gport_chk(tdm_mod_t *tdm, int port);

/*!
 * \brief Check if given port is a front panel port.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 *
 * \retval TDM_TRUE  Port check passed.
 * \retval TDM_FALSE Port check failed.
 */
TDM_EXTERN int
bcmtm_tdm_fport_chk(tdm_mod_t *tdm, int port);

/*!
 * \brief Check if given port is a linerate port.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Calendar mode.
 *
 * \retval TDM_TRUE  Port check passed.
 * \retval TDM_FALSE Port check failed.
 */
TDM_EXTERN int
bcmtm_tdm_port_lr_chk(tdm_mod_t *tdm, int port, int mode);

/*!
 * \brief Check if given port is an oversub port.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Calendar mode.
 *
 * \retval TDM_TRUE  Port check passed.
 * \retval TDM_FALSE Port check failed.
 */
TDM_EXTERN int
bcmtm_tdm_port_os_chk(tdm_mod_t *tdm, int port, int mode);

/*!
 * \brief Check if given port is an Ethernet port.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Calendar mode.
 *
 * \retval TDM_TRUE  Port check passed.
 * \retval TDM_FALSE Port check failed.
 */
TDM_EXTERN int
bcmtm_tdm_port_eth_chk(tdm_mod_t *tdm, int port, int mode);

/*!
 * \brief Check if given port is a higig port.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Calendar mode.
 *
 * \retval TDM_TRUE  Port check passed.
 * \retval TDM_FALSE Port check failed.
 */
TDM_EXTERN int
bcmtm_tdm_port_hg_chk(tdm_mod_t *tdm, int port, int mode);

/*!
 * \brief Check if given port is a management port.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Calendar mode.
 *
 * \retval TDM_TRUE  Port check passed.
 * \retval TDM_FALSE Port check failed.
 */
TDM_EXTERN int
bcmtm_tdm_port_mgmt_chk(tdm_mod_t *tdm, int port, int mode);

/*!
 * \brief Calculate sister port distance of port at given position.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal Pointer of linerate calendar array.
 * \param [in] cal_len length of linerate calendar array.
 * \param [in] espace Extra space.
 * \param [in] pos Slot position.
 * \param [in] dir Moving direction.
 * \retval Sister port distance if violation detected, otherwise 0.
 */
TDM_EXTERN int
bcmtm_tdm_slot_sister_dist_get(tdm_mod_t *tdm, int *cal, int cal_len,
                               int espace, int pos, int dir);

/*!
 * \brief Calculate same port distance of port at given position.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal Pointer of linerate calendar array.
 * \param [in] cal_len length of linerate calendar array.
 * \param [in] espace Extra space.
 * \param [in] pos Slot position.
 * \param [in] dir Moving direction.
 * \retval Same port distance if violation detected, otherwise 0.
 */
TDM_EXTERN int
bcmtm_tdm_slot_same_dist_get(tdm_mod_t *tdm, int *cal, int cal_len,
                             int espace, int pos, int dir);

/*!
 * \brief Get max and min linerate jitter under given speed.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] speed Port speed in Mbps.
 * \param [in] cal_len Calendar length.
 * \param [out] jitter_min Pointer of min jitter.
 * \param [out] jitter_max Pointer of max jitter.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_lr_speed_jitter_get(tdm_mod_t *tdm, int speed, int cal_len,
                              int *jitter_min, int *jitter_max);

/*!
 * \brief Print TDM pipe config info.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_pipe_config_print(tdm_mod_t *tdm);

/*!
 * \brief Generate pipe oversub ports into pipe oversub speed groups.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS Oversub group generated successfully.
 * \retval TDM_FAIL Oversub group generated failed.
 */
int
bcmtm_tdm_pipe_os_grp_gen(tdm_mod_t *tdm);

/*!
 * \brief Print pipe oversub groups.
 *
 * \param [in] tdm Pointer of TDM module address.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_os_grp_print(tdm_mod_t *tdm);

/*!
 * \brief Print pipe linerate calendars.
 *
 * \param [in] tdm Pointer of TDM module address.
 * \param [in] cal_id Calendar ID.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_lr_cal_print(tdm_mod_t *tdm, int cal_id);

#endif /* BCMTM_TDM_MISC_H */
