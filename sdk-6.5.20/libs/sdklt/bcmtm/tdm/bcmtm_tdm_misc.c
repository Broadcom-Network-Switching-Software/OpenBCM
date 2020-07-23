/*! \file bcmtm_tdm_misc.c
 *
 * TDM miscellaneous functions.
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
 * Internal functions.
 */
/*!
 * \brief Initialize TDM core resources.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS  Initilaization passed.
 * \retval TDM_FAIL  Initilaization failed.
 */
int
bcmtm_tdm_core_init(tdm_mod_t *tdm)
{
    int pipe_id, port_lo, port_hi, port, spd_idx, lr_cnt = 0, os_cnt = 0;
    int result = TDM_PASS;

    /* sdklt: tdm_pipe_var_t */
    pipe_id = bcmtm_tdm_pipe_id_get(tdm);
    port_lo = bcmtm_tdm_pipe_port_lo_get(tdm);
    port_hi = bcmtm_tdm_pipe_port_hi_get(tdm);
    TDM_MEMSET(&(tdm->core_data.var), 0, sizeof(tdm_pipe_var_t));
    for (port = port_lo; port <= port_hi; port++) {
        if (bcmtm_tdm_cfg_port_speed_get(tdm, port, 0) > 0) {
            if (bcmtm_tdm_port_lr_chk(tdm, port, 0)) {
                if (lr_cnt < TDM_MAX_PORTS_PER_PIPE) {
                    tdm->core_data.var.lr_buff[lr_cnt++] = port;
                    spd_idx = bcmtm_tdm_port_speed_idx_get(tdm,port, 0);
                    if (spd_idx < TDM_MAX_SPEED_TYPES) {
                        tdm->core_data.var.lr_spd_prt_cnt[spd_idx]++;
                    } else {
                        result = TDM_FAIL;
                        TDM_ERROR4("%s, port %3d, spd_idx %0d, limit %0d\n",
                                   "[tdm_core_init] spd_idx overflow",
                                   port, spd_idx, TDM_MAX_SPEED_TYPES);
                    }
                } else {
                    result = TDM_FAIL;
                    TDM_ERROR5("%s, pipe %d, port %d, cnt %0d, limit %0d.\n",
                               "[tdm_core_init] Linerate port buffer overflow",
                               pipe_id, port, lr_cnt + 1,
                               TDM_MAX_PORTS_PER_PIPE);
                }
            } else if (bcmtm_tdm_port_os_chk(tdm, port, 0)) {
                if (os_cnt < TDM_MAX_PORTS_PER_PIPE) {
                    tdm->core_data.var.os_buff[os_cnt++] = port;
                    spd_idx = bcmtm_tdm_port_speed_idx_get(tdm, port, 0);
                    if (spd_idx < TDM_MAX_SPEED_TYPES) {
                        tdm->core_data.var.os_spd_prt_cnt[spd_idx]++;
                    } else {
                        result = TDM_FAIL;
                        TDM_ERROR4("%s, port %3d, spd_idx %0d, limit %0d\n",
                                   "[tdm_core_init] spd_idx overflow",
                                   port, spd_idx, TDM_MAX_SPEED_TYPES);
                    }
                } else {
                    result = TDM_FAIL;
                    TDM_ERROR5("%s, pipe %d, port %d, cnt %0d, limit %0d.\n",
                               "[tdm_core_init] Oversub port buffer overflow",
                               pipe_id, port, os_cnt + 1,
                               TDM_MAX_PORTS_PER_PIPE);
                }
            }
        }
    }
    tdm->core_data.var.lr_en = (lr_cnt > 0) ? 1 : 0;
    tdm->core_data.var.os_en = (os_cnt > 0) ? 1 : 0;
    tdm->core_data.var.lr_cnt = lr_cnt;
    tdm->core_data.var.os_cnt = os_cnt;
    for (spd_idx = 0; spd_idx < TDM_MAX_SPEED_TYPES; spd_idx++) {
        if (tdm->core_data.var.lr_spd_prt_cnt[spd_idx]) {
            tdm->core_data.var.lr_spd_types++;
        }
    }
    for (spd_idx = 0; spd_idx < TDM_MAX_SPEED_TYPES; spd_idx++) {
        if (tdm->core_data.var.os_spd_prt_cnt[spd_idx]) {
            tdm->core_data.var.os_spd_types++;
        }
    }

    if (result != TDM_PASS) {
        return TDM_FAIL;
    }
    return tdm->core_drv[TDM_CORE_ALLOC](tdm);
}

/*!
 * \brief TDM null funtion.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval TDM_PASS.
 */
int
bcmtm_tdm_core_null(tdm_mod_t *tdm)
{
	TDM_PRINT2("%s, Pipe ID: %0d\n",
               "TDM: Null function was dereferenced",
               bcmtm_tdm_pipe_id_get(tdm));

	return TDM_PASS;
}

/*!
 * \brief Get speed class by port speed.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] speed Port Ethernet speed.
 *
 * \retval Speed class.
 */
static int
bcmtm_tdm_speed_idx_get(tdm_mod_t *tdm, int speed)
{
    int speed_idx = 0;

    switch (speed) {
        case TDM_SPEED_1G:
            speed_idx = TDM_SPEED_IDX_1G;
            break;
        case TDM_SPEED_1P25G:
            speed_idx = TDM_SPEED_IDX_1P25G;
            break;
        case TDM_SPEED_2P5G:
            speed_idx = TDM_SPEED_IDX_2P5G;
            break;
        case TDM_SPEED_5G:
            speed_idx = TDM_SPEED_IDX_5G;
            break;
        case TDM_SPEED_10G:
            speed_idx = TDM_SPEED_IDX_10G;
            break;
        case TDM_SPEED_12P5G:
            speed_idx = TDM_SPEED_IDX_12P5G;
            break;
        case TDM_SPEED_20G:
            speed_idx = TDM_SPEED_IDX_20G;
            break;
        case TDM_SPEED_25G:
            speed_idx = TDM_SPEED_IDX_25G;
            break;
        case TDM_SPEED_40G:
            speed_idx = TDM_SPEED_IDX_40G;
            break;
        case TDM_SPEED_50G:
            speed_idx = TDM_SPEED_IDX_50G;
            break;
        case TDM_SPEED_100G:
            speed_idx = TDM_SPEED_IDX_100G;
            break;
        case TDM_SPEED_120G:
            speed_idx = TDM_SPEED_IDX_120G;
            break;
        case TDM_SPEED_200G:
            speed_idx = TDM_SPEED_IDX_200G;
            break;
        case TDM_SPEED_400G:
            speed_idx = TDM_SPEED_IDX_400G;
            break;
        default:
            speed_idx = TDM_SPEED_IDX_0;
            break;
    }

    return speed_idx;
}

/*!
 * \brief Get speed class by port token.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Port mode.
 *
 * \retval Speed class.
 */
int
bcmtm_tdm_port_speed_idx_get(tdm_mod_t *tdm, int port, int mode)
{
    int port_speed, speed_idx = 0;

    port_speed = bcmtm_tdm_cfg_port_speed_eth_get(tdm, port, mode);
    speed_idx = bcmtm_tdm_speed_idx_get(tdm, port_speed);

    return speed_idx;
}

/*!
 * \brief Get number of slots for a speed.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] speed Port Ethernet speed.
 *
 * \retval Number of slots.
 */
int
bcmtm_tdm_speed_slots_get(tdm_mod_t *tdm, int port_speed)
{
    int speed_slots = 0;
    int slot_unit;

    slot_unit = bcmtm_tdm_pipe_slot_unit_get(tdm);
    if (slot_unit > 0 && port_speed > 0) {
        speed_slots = port_speed / slot_unit;
        if (speed_slots == 0) speed_slots = 1;
    }

    return speed_slots;
}

/*!
 * \brief Get number of slots for a port.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Port mode.
 *
 * \retval Number of slots.
 */
int
bcmtm_tdm_port_slots_get(tdm_mod_t *tdm, int port, int mode)
{
    int port_slots, port_speed;

    port_speed = bcmtm_tdm_cfg_port_speed_eth_get(tdm, port, mode);
    port_slots = bcmtm_tdm_speed_slots_get(tdm, port_speed);

    return port_slots;
}

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
int
bcmtm_tdm_slot_sister_dist_get(tdm_mod_t *tdm, int *cal, int cal_len,
                               int espace, int pos, int dir)
{
    int n, k, space, pm, distance = 0;

    if (cal != NULL && cal_len > 0 && pos >= 0 && pos < cal_len) {
        if (bcmtm_tdm_fport_chk(tdm, cal[pos])) {
            space = bcmtm_tdm_min_sister_space_get(tdm, cal[pos]) + espace;
            pm = bcmtm_tdm_port_pm_get(tdm, cal[pos]);
            for (n = 1; n < space; n++) {
                k = (dir == TDM_DIR_DN) ? ((pos + n) % cal_len) :
                                          ((cal_len + pos - n) % cal_len);
                if (pm == bcmtm_tdm_port_pm_get(tdm, cal[k])) {
                    distance = n;
                    break;
                }
            }
        }
    }

    return distance;
}

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
int
bcmtm_tdm_slot_same_dist_get(tdm_mod_t *tdm, int *cal, int cal_len,
                             int espace, int pos, int dir)
{
    int n, k, space, distance = 0;

    if (cal != NULL && pos >= 0 && pos < cal_len) {
        if (bcmtm_tdm_fport_chk(tdm, cal[pos])) {
            space = bcmtm_tdm_min_same_space_get(tdm, cal[pos]) + espace;
            for (n = 1; n < space; n++) {
                k = (dir == TDM_DIR_DN) ? ((pos + n) % cal_len) :
                                          ((cal_len + pos - n) % cal_len);
                if (cal[pos] == cal[k]) {
                    distance = n;
                    break;
                }
            }
        }
    }

    return distance;
}

/*!
 * \brief Print TDM pipe config info.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_pipe_config_print(tdm_mod_t *tdm)
{
    int port, port_speed;
    int port_lo, port_hi, pipe_id;

    port_lo = bcmtm_tdm_pipe_port_lo_get(tdm);
    port_hi = bcmtm_tdm_pipe_port_hi_get(tdm);
    pipe_id = bcmtm_tdm_pipe_id_get(tdm);

    if (bcmtm_tdm_fport_chk(tdm, port_lo) == TDM_TRUE) {
        TDM_BIG_BAR
        TDM_PRINT3("TDM: Pipe Config: Pipe %d, Port [%d - %d] \n\n",
                   pipe_id, port_lo, port_hi);
        TDM_PRINT5("%8s%8s%8s%8s%8s\n",
                   "port", "pm", "speed", "LR/OS", "EN/HG");
        for (port = port_lo; port <= port_hi; port++) {
            port_speed = bcmtm_tdm_cfg_port_speed_get(tdm, port, 0);
            if (port_speed > 0) {
                TDM_PRINT1("%8d", port);
                TDM_PRINT1("%8d", bcmtm_tdm_port_pm_get(tdm, port));
                TDM_PRINT1("%8d", (port_speed / 1000));
                if (TDM_TRUE == bcmtm_tdm_port_lr_chk(tdm, port, 0)) {
                    TDM_PRINT1("%8s", "LR");
                } else {
                    TDM_PRINT1("%8s", "OS");
                }
                if (TDM_TRUE == bcmtm_tdm_port_hg_chk(tdm, port, 0)) {
                    TDM_PRINT1("%8s", "HG");
                } else {
                    TDM_PRINT1("%8s", "EN");
                }
                TDM_PRINT0("\n");
            }
        }
        TDM_BIG_BAR
    }
}

/*!
 * \brief Print pipe oversub groups.
 *
 * \param [in] tdm Pointer of TDM module address.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_os_grp_print(tdm_mod_t *tdm)
{
    int i, j, n, pipe_id, grp_num = 0, grp_len = 0, *cal_grp = NULL;

    pipe_id = bcmtm_tdm_pipe_id_get(tdm);
    grp_num = bcmtm_tdm_os_grp_num_get(tdm, pipe_id, 0);
    grp_len = bcmtm_tdm_os_grp_len_get(tdm, pipe_id, 0);

    if (grp_num > 0 && grp_len > 0) {
        TDM_PRINT1("\nTDM: PIPE %d, OVSB GROUPs 0:11\n", pipe_id);
        for (i = 0; i < grp_num; i++) {
            n = grp_num / 2;
            if (n > 0) {
                if (i % n == 0) {
                    TDM_PRINT2("\n%12s%2d\t", "HALF PIPE", (i / n));
                    for (j = 0; j < grp_len; j++) {
                        TDM_PRINT1("%3d\t", j + 1);
                    }
                    TDM_PRINT0("\n");
                }
            }
            TDM_PRINT1("OVSB_GROUP[%2d]:\t", i);
            cal_grp = bcmtm_tdm_os_grp_get(tdm, pipe_id, i, 0);
            for (j = 0; j < grp_len; j++) {
                TDM_PRINT1("%3d\t", cal_grp[j]);
            }
            TDM_PRINT0("\n");
        }
        TDM_SML_BAR
    }

    TDM_SML_BAR
}

/*!
 * \brief Print pipe linerate calendars.
 *
 * \param [in] tdm Pointer of TDM module address.
 * \param [in] cal_id Calendar ID.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_lr_cal_print(tdm_mod_t *tdm, int cal_id)
{
    int j, cal_len, *cal = NULL;

    cal_len = bcmtm_tdm_lr_cal_len_get(tdm, cal_id, 0);
    cal = bcmtm_tdm_lr_cal_get(tdm, cal_id, 0);

    if (cal != NULL && cal_len > 0) {
        TDM_PRINT1("LINERATE CALENDAR -- %0d\n", cal_id);
        TDM_PRINT1("%12s", "");
        for (j = 0; j < TDM_LR_CAL_PRINT_WID; j++) {
            TDM_PRINT1("%3d ", j);
        }
        for (j = 0; j < cal_len; j++) {
            if (j % TDM_LR_CAL_PRINT_WID == 0) {
                TDM_PRINT2("\n[%03d, %03d]: ", j,
                           (j + TDM_LR_CAL_PRINT_WID - 1));
            }
            TDM_PRINT1("%3d ", cal[j]);
        }
        TDM_PRINT0("\n");
        TDM_SML_BAR
    }
}
