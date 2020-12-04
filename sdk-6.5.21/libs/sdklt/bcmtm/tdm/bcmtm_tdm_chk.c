/*! \file bcmtm_tdm_chk.c
 *
 * TDM core functions for TDM self check.
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
 * \brief Check sister-port-space restriction among all linerate calendars.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS  Check passed.
 * \retval TDM_FAIL  Check failed.
 */
int
bcmtm_tdm_sister_chk(tdm_mod_t *tdm)
{
    int i, j, k, result = TDM_PASS;
    int cal_id, cal_en, cal_len, *cal;
    int sister_space;
    int pm_curr, pm_next;

    sister_space = bcmtm_tdm_min_sister_space_get(tdm, 0);
    for (cal_id = 0; cal_id < TDM_MAX_NUM_CALS; cal_id++) {
        cal_en = bcmtm_tdm_lr_cal_en_get(tdm, cal_id, 0);
        cal_len = bcmtm_tdm_lr_cal_len_get(tdm, cal_id, 0);
        if (cal_en == TDM_TRUE && cal_len > 0) {
            cal = bcmtm_tdm_lr_cal_get(tdm, cal_id, 0);
            for (i = 0; i < cal_len; i++) {
                if (bcmtm_tdm_fport_chk(tdm, cal[i]) == TDM_FALSE ||
                    bcmtm_tdm_cfg_port_speed_get(tdm, cal[i], 0) == 0) {
                    continue;
                }
                pm_curr = bcmtm_tdm_port_pm_get(tdm, cal[i]);
                for (j = 1; j < sister_space; j++) {
                    k = (i + j) % cal_len;
                    if (bcmtm_tdm_fport_chk(tdm, cal[k]) == TDM_FALSE ||
                        bcmtm_tdm_cfg_port_speed_get(tdm, cal[k], 0) == 0) {
                        continue;
                    }
                    pm_next = bcmtm_tdm_port_pm_get(tdm, cal[k]);
                    if (pm_curr == pm_next) {
                        result = TDM_FAIL;
                        TDM_ERROR5("%s, port[%3d,%3d], pos[%3d,%3d],\n",
                                   "[Sister Port Space]",
                                   cal[i], cal[k], i, k);
                    }
                }
            }
        }
    }

    return result;
}

/*!
 * \brief Check same-port-space restriction among all linerate calendars.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS  Check passed.
 * \retval TDM_FAIL  Check failed.
 */
int
bcmtm_tdm_same_chk(tdm_mod_t *tdm)
{
    int i, j, k, result = TDM_PASS;
    int cal_id, cal_en, cal_len, *cal;
    int same_space, speed;

    same_space = bcmtm_tdm_min_same_space_get(tdm, 0);
    for (cal_id = 0; cal_id < TDM_MAX_NUM_CALS; cal_id++) {
        cal_en = bcmtm_tdm_lr_cal_en_get(tdm, cal_id, 0);
        cal_len = bcmtm_tdm_lr_cal_len_get(tdm, cal_id, 0);
        if (cal_en == TDM_TRUE && cal_len > 0) {
            cal = bcmtm_tdm_lr_cal_get(tdm, cal_id, 0);
            for (i = 0; i < cal_len; i++) {
                if (bcmtm_tdm_fport_chk(tdm, cal[i]) == TDM_FALSE) {
                    continue;
                }
                for (j = 1; j < same_space; j++) {
                    k = (i + j) % cal_len;
                    if (cal[i] == cal[k]) {
                        result = TDM_FAIL;
                        speed = bcmtm_tdm_cfg_port_speed_get(tdm, cal[i], 0);
                        TDM_ERROR5("%s, port %3d, speed %3dG, pos[%3d,%3d]\n",
                                "[Same Port Sapce] ",
                                cal[i], (speed / 1000) , i, k);
                    }
                }
            }
        }
    }

    return result;
}

/*!
 * \brief Get number of slots allocated for a given token in a calendar.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] cal_id Calendar ID.
 * \param [in] token Slot token.
 *
 * \retval Number of slots allocated for a given token.
 */
int
bcmtm_tdm_cal_token_count(tdm_mod_t *tdm, int cal_id, int token)
{
    int cnt = 0;
    int cal_en;

    cal_en = bcmtm_tdm_lr_cal_en_get(tdm, cal_id, 0);
    if (cal_en == TDM_TRUE) {
        int i, cal_len, *cal;
        cal_len = bcmtm_tdm_lr_cal_len_get(tdm, cal_id, 0);
        cal = bcmtm_tdm_lr_cal_get(tdm, cal_id, 0);
        for (i = 0; i < cal_len; i++) {
            if (cal[i] == token) {
                cnt++;
            }
        }
    }

    return cnt;
}

/*!
 * \brief Check linerate port bandwidth in a given calendar.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS  Check passed.
 * \retval TDM_FAIL  Check failed.
 */
int
bcmtm_tdm_lr_port_bw_chk(tdm_mod_t *tdm, int cal_id, int port)
{
    int result = TDM_PASS;
    int slot_req, slot_cnt, speed;

    slot_req = bcmtm_tdm_port_slots_get(tdm, port, 0);
    slot_cnt = bcmtm_tdm_cal_token_count(tdm, cal_id, port);
    if (slot_cnt < slot_req) {
        result = TDM_FAIL;
        speed = bcmtm_tdm_cfg_port_speed_get(tdm, port, 0);
        TDM_ERROR6("%s cal %0d, port %0d, speed %0dG, slots %0d/%0d\n",
                  "[Linerate Bandwidth] insufficient port bandwidth",
                  cal_id, port, (speed / 1000), slot_cnt, slot_req);
    }

    return result;
}

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
int
bcmtm_tdm_gport_chk(tdm_mod_t *tdm, int port)
{
    return ((port >= 0 && port < tdm->user_data.cfg.num_gports) ?
            TDM_TRUE : TDM_FALSE);
}

/*!
 * \brief Check if given port is a front panel port.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 *
 * \retval TDM_TRUE  Port check passed.
 * \retval TDM_FALSE Port check failed.
 */
int
bcmtm_tdm_fport_chk(tdm_mod_t *tdm, int port)
{
    return ((port >= tdm->chip_data.cfg.fp_port_lo &&
             port <= tdm->chip_data.cfg.fp_port_hi) ?
            TDM_TRUE : TDM_FALSE);
}

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
int
bcmtm_tdm_port_lr_chk(tdm_mod_t *tdm, int port, int mode)
{
    int port_state;

    if (bcmtm_tdm_cfg_port_speed_get(tdm, port, mode) > 0) {
        port_state = bcmtm_tdm_cfg_port_state_get(tdm, port, mode);
        if (port_state == TDM_STATE_LINERATE   ||
            port_state == TDM_STATE_LINERATE_HG) {
            return TDM_TRUE;
        }
    }

    return TDM_FALSE;
}

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
int
bcmtm_tdm_port_os_chk(tdm_mod_t *tdm, int port, int mode)
{
    int port_state;

    if (bcmtm_tdm_cfg_port_speed_get(tdm, port, mode) > 0) {
        port_state = bcmtm_tdm_cfg_port_state_get(tdm, port, mode);
        if (port_state == TDM_STATE_OVERSUB   ||
            port_state == TDM_STATE_OVERSUB_HG) {
            return TDM_TRUE;
        }
    }

    return TDM_FALSE;
}

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
int
bcmtm_tdm_port_eth_chk(tdm_mod_t *tdm, int port, int mode)
{
    int port_state;

    port_state = bcmtm_tdm_cfg_port_state_get(tdm, port, mode);
    if (port_state == TDM_STATE_LINERATE ||
        port_state == TDM_STATE_OVERSUB  ||
        port_state == TDM_STATE_MGMT) {
        return TDM_TRUE;
    }

    return TDM_FALSE;
}

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
int
bcmtm_tdm_port_hg_chk(tdm_mod_t *tdm, int port, int mode)
{
    int port_state;

    if (bcmtm_tdm_cfg_port_speed_get(tdm, port, mode) > 0) {
        port_state = bcmtm_tdm_cfg_port_state_get(tdm, port, mode);
        if (port_state == TDM_STATE_LINERATE_HG ||
            port_state == TDM_STATE_OVERSUB_HG  ||
            port_state == TDM_STATE_MGMT_HG) {
            return TDM_TRUE;
        }
    }

    return TDM_FALSE;
}

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
int
bcmtm_tdm_port_mgmt_chk(tdm_mod_t *tdm, int port, int mode)
{
    int port_state;

    port_state = bcmtm_tdm_cfg_port_state_get(tdm, port, mode);
    if (port_state == TDM_STATE_MGMT ||
        port_state == TDM_STATE_MGMT_HG) {
        return TDM_TRUE;
    }

    return TDM_FALSE;
}

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
                              int *jitter_min, int *jitter_max)
{
    int slot_unit, slot_req, space_frac, jrange, jmin = 0, jmax = 0;

    slot_unit = bcmtm_tdm_pipe_slot_unit_get(tdm);
    if (slot_unit > 0 && speed > TDM_SPEED_1G) {
        slot_req = speed / slot_unit;
        if (slot_req > 0) {
            space_frac = ((cal_len * 10) / slot_req) % 10;
            jrange = (2 * cal_len) / (slot_req * 5);
            jrange = ((((2 * cal_len * 10) / (slot_req * 5)) % 10) < 5)?
                         jrange : (jrange + 1);

            if (space_frac < 5) {
                jmin = cal_len / slot_req - jrange / 2;
                jmax = cal_len / slot_req + jrange / 2;
                jmax = ((jrange % 2) == 0) ? jmax : (jmax + 1);
            } else {
                jmin = cal_len / slot_req - jrange / 2;
                jmin = ((cal_len % slot_req) == 0) ? jmin : (jmin + 1);
                jmin = ((jrange % 2) == 0) ? jmin : (jmin - 1);
                jmax = cal_len / slot_req + jrange / 2;
                jmax = ((cal_len % slot_req) == 0) ? jmax : (jmax + 1);
            }
            jmin = (jmin < 1) ? 1 : jmin;
            jmax = (jmax < 1) ? 1 : jmax;
        }
    }

    if (jitter_min != NULL) {
        (*jitter_min) = jmin;
    }
    if (jitter_max != NULL) {
        (*jitter_max) = jmax;
    }
}
