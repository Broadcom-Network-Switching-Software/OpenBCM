/*! \file bcmtm_tdm_cfg_user.c
 *
 * TDM core functions for user configuration access.
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


/*******************************************************************************
 * Public functions
 */
/*!
 * \brief Get user config data on unit.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Unit.
 */
int
bcmtm_tdm_cfg_unit_get(tdm_mod_t *tdm)
{
    return tdm->user_data.cfg.unit;
}

/*!
 * \brief Get user config data on device ID.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Device ID.
 */
int
bcmtm_tdm_cfg_dev_id_get(tdm_mod_t *tdm)
{
    return tdm->user_data.cfg.dev_id;
}

/*!
 * \brief Get user config data on core clock frequency.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Core clock frequency.
 */
int
bcmtm_tdm_cfg_freq_get(tdm_mod_t *tdm)
{
    return tdm->user_data.cfg.clk_freq_core;
}

/*!
 * \brief Get user config data on number of general ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Nubmer of general ports.
 *
 * General ports include front panel ports + CPU + Management + Loopback.
 */
int
bcmtm_tdm_cfg_num_gports_get(tdm_mod_t *tdm)
{
    return tdm->user_data.cfg.num_gports;
}

/*!
 * \brief Get user config data on flex_en.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Flex operation indicator.
 */
int
bcmtm_tdm_cfg_flex_en_get(tdm_mod_t *tdm)
{
    return tdm->user_data.cfg.flex_en;
}

/*!
 * \brief Get user config data on chk_en.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Checker enable indicator.
 */
int
bcmtm_tdm_cfg_chk_en_get(tdm_mod_t *tdm)
{
    return tdm->user_data.cfg.chk_en;
}

/*!
 * \brief Get user config data on port state.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Calendar mode.
 *
 * \retval Port state.
 */
int
bcmtm_tdm_cfg_port_state_get(tdm_mod_t *tdm, int port, int mode)
{
    if (bcmtm_tdm_gport_chk(tdm, port) == TDM_TRUE) {
        if (mode == TDM_CAL_CURR) {
            return tdm->user_data.cfg.port_states[port - 1];
        } else {
            return tdm->prev_user_data.cfg.port_states[port - 1];
        }
    }

    return TDM_STATE_DISABLED;
}

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
void
bcmtm_tdm_cfg_port_state_set(tdm_mod_t *tdm, int port, int state, int mode)
{
    if (bcmtm_tdm_fport_chk(tdm, port) == TDM_TRUE) {
        if (mode == TDM_CAL_CURR) {
            tdm->user_data.cfg.port_states[port - 1] = state;
        } else {
            tdm->prev_user_data.cfg.port_states[port - 1] = state;
        }
    }
}

/*!
 * \brief Get user config data on port speed.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Calendar mode.
 *
 * \retval Port speed.
 */
int
bcmtm_tdm_cfg_port_speed_get(tdm_mod_t *tdm, int port, int mode)
{
    if (bcmtm_tdm_gport_chk(tdm, port) == TDM_TRUE) {
        if (mode == TDM_CAL_CURR) {
            return tdm->user_data.cfg.port_speeds[port];
        } else {
            return tdm->prev_user_data.cfg.port_speeds[port];
        }
    }

    return 0;
}

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
void
bcmtm_tdm_cfg_port_speed_set(tdm_mod_t *tdm, int port, int speed, int mode)
{
    if (bcmtm_tdm_fport_chk(tdm, port) == TDM_TRUE) {
        if (mode == TDM_CAL_CURR) {
            tdm->user_data.cfg.port_speeds[port] = speed;
        } else {
            tdm->prev_user_data.cfg.port_speeds[port] = speed;
        }
    }
}

/*!
 * \brief Get user config data on port speed in Ethernet speed mode.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 * \param [in] mode Calendar mode.
 *
 * \retval Port speed.
 */
int
bcmtm_tdm_cfg_port_speed_eth_get(tdm_mod_t *tdm, int port, int mode)
{
    int port_speed, port_speed_eth;

    port_speed = bcmtm_tdm_cfg_port_speed_get(tdm, port, mode);
    switch (port_speed) {
        case TDM_SPEED_1G:
        case TDM_SPEED_1P25G:
        case TDM_SPEED_2G:
        case TDM_SPEED_2P5G:
        case TDM_SPEED_5G:
            port_speed_eth = port_speed;
            break;
        case TDM_SPEED_10G:
        case TDM_SPEED_11G:
            port_speed_eth = TDM_SPEED_10G;
            break;
        case TDM_SPEED_12P5G:
            port_speed_eth = TDM_SPEED_12P5G;
            break;
        case TDM_SPEED_20G:
        case TDM_SPEED_21G:
            port_speed_eth = TDM_SPEED_20G;
            break;
        case TDM_SPEED_25G:
        case TDM_SPEED_27G:
            port_speed_eth = TDM_SPEED_25G;
            break;
        case TDM_SPEED_40G:
        case TDM_SPEED_42G:
            port_speed_eth = TDM_SPEED_40G;
            break;
        case TDM_SPEED_50G:
        case TDM_SPEED_53G:
            port_speed_eth = TDM_SPEED_50G;
            break;
        case TDM_SPEED_100G:
        case TDM_SPEED_106G:
            port_speed_eth = TDM_SPEED_100G;
            break;
        case TDM_SPEED_120G:
        case TDM_SPEED_127G:
            port_speed_eth = TDM_SPEED_120G;
            break;
        case TDM_SPEED_200G:
            port_speed_eth = TDM_SPEED_200G;
            break;
        case TDM_SPEED_400G:
            port_speed_eth = TDM_SPEED_400G;
            break;
        default:
            port_speed_eth = 0;
            break;
    }

    return port_speed_eth;
}

/*!
 * \brief Print TDM config info.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_cfg_print(tdm_mod_t *tdm)
{
    int pm, lane, lgc_port = 0, phy_port;
    int port_speed;
    int p_num_pm_lanes, p_num_pms;

    p_num_pm_lanes = bcmtm_tdm_num_pm_lanes_get(tdm);
    p_num_pms = bcmtm_tdm_num_pms_get(tdm);

    TDM_PRINT0("TDM: Portmap Summary\n");
    for (pm = 0; pm < p_num_pms; pm++) {
        TDM_PRINT1("\tPM_Inst [%02d] \n", pm);
        for (lane = 0; lane < p_num_pm_lanes; lane++) {
            TDM_PRINT1("\t\t\tPM_Lane [%0d]\t ",lane);
            phy_port = pm * p_num_pm_lanes + lane + 1;
            port_speed = bcmtm_tdm_cfg_port_speed_get(tdm, phy_port, 0);
            if (port_speed > 0) {
                TDM_PRINT2("->\tP_Port [%03d]\t ->\tL_Port [%03d]\t ",
                           phy_port, ++lgc_port);
                TDM_PRINT1("->\tSpeed [%d Gbps] ", port_speed / 1000);
                if (bcmtm_tdm_port_mgmt_chk(tdm, phy_port, 0)) {
                    TDM_PRINT0(" [MGMT]\n");
                } else {
                    if (bcmtm_tdm_port_lr_chk(tdm, phy_port, 0)) {
                        TDM_PRINT0(" [LNRT]");
                    } else {
                        TDM_PRINT0(" [OVSB]");
                    }
                    if (bcmtm_tdm_port_hg_chk(tdm, phy_port, 0)) {
                        TDM_PRINT0(" [HIGIG]\n");
                    } else {
                        TDM_PRINT0(" [ETHN]\n");
                    }
                }
            } else {
                TDM_PRINT0("->\tP_Port [---]\t ->\tL_Port [---] \n");
            }
        }
    }
    TDM_SML_BAR
}
