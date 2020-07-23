/*! \file bcmtm_tdm_cfg.h
 *
 * Function prototypes for TDM user/chip/pipe configurations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_TDM_CFG_H
#define BCMTM_TDM_CFG_H


#ifdef _TDM_STANDALONE
    #include <bcmtm_tdm_defines.h>
#else
    #include <bcmtm/tdm/bcmtm_tdm_defines.h>
#endif


/***********************************************************************
 * Internal functions: user config info
 */
/*!
 * \brief Get user config data on unit.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Unit.
 */
TDM_EXTERN int
bcmtm_tdm_cfg_unit_get(tdm_mod_t *tdm);

/*!
 * \brief Get user config data on device ID.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Device ID.
 */
TDM_EXTERN int
bcmtm_tdm_cfg_dev_id_get(tdm_mod_t *tdm);

/*!
 * \brief Get user config data on core clock frequency.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Core clock frequency.
 */
TDM_EXTERN int
bcmtm_tdm_cfg_freq_get(tdm_mod_t *tdm);

/*!
 * \brief Get user config data on number of general ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Nubmer of general ports.
 *
 * General ports include front panel ports + CPU + Management + Loopback.
 */
TDM_EXTERN int
bcmtm_tdm_cfg_num_gports_get(tdm_mod_t *tdm);

/*!
 * \brief Get user config data on flex_en.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Flex operation indicator.
 */
TDM_EXTERN int
bcmtm_tdm_cfg_flex_en_get(tdm_mod_t *tdm);

/*!
 * \brief Get user config data on chk_en.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Checker enable indicator.
 */
TDM_EXTERN int
bcmtm_tdm_cfg_chk_en_get(tdm_mod_t *tdm);

/*!
 * \brief Get user config data on port state.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Calendar mode.
 *
 * \retval Port state.
 */
TDM_EXTERN int
bcmtm_tdm_cfg_port_state_get(tdm_mod_t *tdm, int port, int mode);

/*!
 * \brief Set user config data on port state.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] state Port state.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
TDM_EXTERN void
bcmtm_tdm_cfg_port_state_set(tdm_mod_t *tdm, int port, int state, int mode);

/*!
 * \brief Get user config data on port speed.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Calendar mode.
 *
 * \retval Port speed.
 */
TDM_EXTERN int
bcmtm_tdm_cfg_port_speed_get(tdm_mod_t *tdm, int port, int mode);

/*!
 * \brief Set user config data on port speed.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] speed Port speed.
 * \param [in] mode Calendar mode.
 *
 * \return Nothing.
 */
TDM_EXTERN void
bcmtm_tdm_cfg_port_speed_set(tdm_mod_t *tdm, int port, int speed, int mode);

/*!
 * \brief Get user config data on port speed in Ethernet speed mode.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Calendar mode.
 *
 * \retval Port speed.
 */
TDM_EXTERN int
bcmtm_tdm_cfg_port_speed_eth_get(tdm_mod_t *tdm, int port, int mode);

/*!
 * \brief Print TDM config info.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \return Nothing.
 */
TDM_EXTERN void
bcmtm_tdm_cfg_print(tdm_mod_t *tdm);

/***********************************************************************
 * Internal functions: chip config info
 */
/*!
 * \brief Get the lowest front panel port.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Lowest front panel port.
 */
TDM_EXTERN int
bcmtm_tdm_fport_lo_get(tdm_mod_t *tdm);

/*!
 * \brief Get the highest front panel port.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Highest front panel port.
 */
TDM_EXTERN int
bcmtm_tdm_fport_hi_get(tdm_mod_t *tdm);

/*!
 * \brief Get number of pipes.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of pipes.
 */
TDM_EXTERN int
bcmtm_tdm_num_pipes_get(tdm_mod_t *tdm);

/*!
 * \brief Get number of PMs.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of PMs.
 */
TDM_EXTERN int
bcmtm_tdm_num_pms_get(tdm_mod_t *tdm);

/*!
 * \brief Get number of lanes per PM.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of lanes per PM.
 */
TDM_EXTERN int
bcmtm_tdm_num_pm_lanes_get(tdm_mod_t *tdm);

/*!
 * \brief Get same port spacing.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 *
 * \retval Same port spacing.
 */
TDM_EXTERN int
bcmtm_tdm_min_same_space_get(tdm_mod_t *tdm, int port);

/*!
 * \brief Get sister port spacing.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 *
 * \retval Sister port spacing.
 */
TDM_EXTERN int
bcmtm_tdm_min_sister_space_get(tdm_mod_t *tdm, int port);

/*!
 * \brief Get port PM number.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 *
 * \retval Port PM number.
 */
TDM_EXTERN int
bcmtm_tdm_port_pm_get(tdm_mod_t *tdm, int port);

/*!
 * \brief Set port PM number.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 *
 * \return Nothing.
 */
TDM_EXTERN void
bcmtm_tdm_port_pm_set(tdm_mod_t *tdm, int port, int pm);


/*!
 * \brief Get empty port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Empty port token.
 */
TDM_EXTERN int
bcmtm_tdm_token_empty_get(tdm_mod_t *tdm);

/*!
 * \brief Get oversub port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Oversub port token.
 */
TDM_EXTERN int
bcmtm_tdm_token_ovsb_get(tdm_mod_t *tdm);

/*!
 * \brief Get ancilary port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Ancilary port token.
 */
TDM_EXTERN int
bcmtm_tdm_token_ancl_get(tdm_mod_t *tdm);

/*!
 * \brief Get idle-1 port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Idle1 port token.
 */
TDM_EXTERN int
tdm_cmn_get_token_idl1(tdm_mod_t *tdm);

/*!
 * \brief Get idle-2 port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Idle2 port token.
 */
TDM_EXTERN int
tdm_cmn_get_token_idl2(tdm_mod_t *tdm);

/*!
 * \brief Get idle port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Idle port token.
 */
TDM_EXTERN int
tdm_cmn_get_token_idle(tdm_mod_t *tdm);

/*!
 * \brief Get opportunistic-1 port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval opportunistic-1 port token.
 */
TDM_EXTERN int
tdm_cmn_get_token_opp1(tdm_mod_t *tdm);

/*!
 * \brief Get opportunistic-2 port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval opportunistic-2 port token.
 */
TDM_EXTERN int
tdm_cmn_get_token_opp2(tdm_mod_t *tdm);

/*!
 * \brief Get null port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval null port token.
 */
TDM_EXTERN int
tdm_cmn_get_token_null(tdm_mod_t *tdm);


/***********************************************************************
 * Internal functions: pipe config info
 */
/*!
 * \brief Get pipe config data on slot granularity.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Slot unit in Mbps.
 *
 * Slot_unit represents the bandwidth per slot of linerate main calendar.
 * Note that the slot unit has the same precision of port speed.
 */
TDM_EXTERN int
bcmtm_tdm_pipe_slot_unit_get(tdm_mod_t *tdm);

/*!
 * \brief Get pipe config data on pipe index.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pipe ID.
 */
TDM_EXTERN int
bcmtm_tdm_pipe_id_get(tdm_mod_t *tdm);

/*!
 * \brief Get pipe config data on the lowest pipe port.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Lowest port number.
 */
TDM_EXTERN int
bcmtm_tdm_pipe_port_lo_get(tdm_mod_t *tdm);

/*!
 * \brief Get pipe config data on the highest pipe port.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Highest port number.
 */
TDM_EXTERN int
bcmtm_tdm_pipe_port_hi_get(tdm_mod_t *tdm);

/*!
 * \brief Get pipe config data on limitiation of linerate slots.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of max linerate slots.
 */
TDM_EXTERN int
bcmtm_tdm_pipe_lr_limit_get(tdm_mod_t *tdm);

/*!
 * \brief Get pipe config data on limitiation of ancilary slots.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of max ancilary slots.
 */
TDM_EXTERN int
bcmtm_tdm_pipe_ancl_limit_get(tdm_mod_t *tdm);



/*!
 * \brief Get pipe variable on linerate enable.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pipe linerate enable indicator.
 */
TDM_EXTERN char
bcmtm_tdm_pipe_lr_en_get(tdm_mod_t *tdm);

/*!
 * \brief Get pipe variable on oversub enable.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pipe oversub enable indicator.
 */
TDM_EXTERN char bcmtm_tdm_pipe_os_en_get(tdm_mod_t *tdm);

/*!
 * \brief Get pipe variable on linerate port count.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of linerate ports in pipe.
 */
TDM_EXTERN int
bcmtm_tdm_pipe_lr_cnt_get(tdm_mod_t *tdm);

/*!
 * \brief Get pipe variable on oversub port count.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of oversub ports in pipe.
 */
TDM_EXTERN int
bcmtm_tdm_pipe_os_cnt_get(tdm_mod_t *tdm);

/*!
 * \brief Get number of speed types for pipe linerate ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of speed types of pipe linerate ports.
 */
TDM_EXTERN int
bcmtm_tdm_pipe_lr_spd_types_get(tdm_mod_t *tdm);

/*!
 * \brief Get number of speed types for pipe oversub ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of speed types of pipe oversub ports.
 */
TDM_EXTERN int
bcmtm_tdm_pipe_os_spd_types_get(tdm_mod_t *tdm);

/*!
 * \brief Get pointer of port buffer for pipe linerate ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pointer of pipe linerate port buffer.
 */
TDM_EXTERN int *
bcmtm_tdm_pipe_lr_buff_get(tdm_mod_t *tdm);

/*!
 * \brief Get pointer of port count buffer for pipe oversub ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pointer of pipe oversub port buffer.
 */
TDM_EXTERN int *
bcmtm_tdm_pipe_os_buff_get(tdm_mod_t *tdm);

/*!
 * \brief Get pointer of port count per speed for pipe linerate ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pointer of speed-based oversub port count buffer.
 */
TDM_EXTERN int *
bcmtm_tdm_pipe_lr_spd_prt_cnt_get(tdm_mod_t *tdm);

/*!
 * \brief Get pointer of port count per speed for pipe oversub ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pointer of speed-based linerate port count buffer.
 */
TDM_EXTERN int *
bcmtm_tdm_pipe_os_spd_prt_cnt_get(tdm_mod_t *tdm);


#endif /* BCMTM_TDM_CFG_H */
