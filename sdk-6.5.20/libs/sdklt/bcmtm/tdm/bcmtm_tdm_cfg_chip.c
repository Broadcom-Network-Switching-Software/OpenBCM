/*! \file bcmtm_tdm_cfg_chip.c
 *
 * TDM core functions for chip configuration access.
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
 * \brief Get the lowest front panel port.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Lowest front panel port.
 */
int
bcmtm_tdm_fport_lo_get(tdm_mod_t *tdm)
{
    return tdm->chip_data.cfg.fp_port_lo;
}

/*!
 * \brief Get the highest front panel port.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Highest front panel port.
 */
int
bcmtm_tdm_fport_hi_get(tdm_mod_t *tdm)
{
    return tdm->chip_data.cfg.fp_port_hi;
}

/*!
 * \brief Get number of pipes.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of pipes.
 */
int
bcmtm_tdm_num_pipes_get(tdm_mod_t *tdm)
{
    return tdm->chip_data.cfg.num_pipes;
}

/*!
 * \brief Get number of PMs.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of PMs.
 */
int
bcmtm_tdm_num_pms_get(tdm_mod_t *tdm)
{
    return tdm->chip_data.cfg.num_pms;
}

/*!
 * \brief Get number of lanes per PM.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of lanes per PM.
 */
int
bcmtm_tdm_num_pm_lanes_get(tdm_mod_t *tdm)
{
    return tdm->chip_data.cfg.num_pm_lanes;
}

/*!
 * \brief Get same port spacing.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 *
 * \retval Same port spacing.
 */
int
bcmtm_tdm_min_same_space_get(tdm_mod_t *tdm, int port)
{
    /* For device with diff sister port spaces, retrieve space limit
     * which is pre-determined by chip side.
     */
    return tdm->chip_data.cfg.min_space_same;
}

/*!
 * \brief Get sister port spacing.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 *
 * \retval Sister port spacing.
 */
int
bcmtm_tdm_min_sister_space_get(tdm_mod_t *tdm, int port)
{
    /* For device with diff sister port spaces, retrieve space limit
     * which is pre-determined by chip side.
     */
    return tdm->chip_data.cfg.min_space_sister;
}

/*!
 * \brief Get port PM number.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 *
 * \retval Port PM number.
 */
int
bcmtm_tdm_port_pm_get(tdm_mod_t *tdm, int port)
{
    return tdm->chip_data.cfg.port2pm_map[port % TDM_MAX_NUM_GPORTS];
}

/*!
 * \brief Set port PM number.
 *
 * \param [in] tdm Pointer of TDM module.
 * \param [in] port Port token.
 *
 * \return Nothing.
 */
void
bcmtm_tdm_port_pm_set(tdm_mod_t *tdm, int port, int pm)
{
    if (port < TDM_MAX_NUM_GPORTS) {
        if (pm < TDM_MAX_NUM_PMS) {
            tdm->chip_data.cfg.port2pm_map[port] = pm;
        } else {
            TDM_ERROR1("[port_pm_set] Invalid PM number %0d\n", pm);
        }
    } else {
        TDM_ERROR1("[port_pm_set] Invalid port number %0d\n", port);
    }
}



/*!
 * \brief Get empty port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Empty port token.
 */
int
bcmtm_tdm_token_empty_get(tdm_mod_t *tdm)
{
    return tdm->chip_data.token.token_empty;
}

/*!
 * \brief Get oversub port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Oversub port token.
 */
int
bcmtm_tdm_token_ovsb_get(tdm_mod_t *tdm)
{
    return tdm->chip_data.token.token_ovsb;
}

/*!
 * \brief Get ancilary port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Ancilary port token.
 */
int
bcmtm_tdm_token_ancl_get(tdm_mod_t *tdm)
{
    return tdm->chip_data.token.token_ancl;
}

/*!
 * \brief Get idle-1 port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Idle-1 port token.
 */
int
tdm_cmn_get_token_idl1(tdm_mod_t *tdm)
{
    return tdm->chip_data.token.token_idl1;
}

/*!
 * \brief Get idle-2 port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Idle2 port token.
 */
int
tdm_cmn_get_token_idl2(tdm_mod_t *tdm)
{
    return tdm->chip_data.token.token_idl2;
}

/*!
 * \brief Get idle port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Idle port token.
 */
int
tdm_cmn_get_token_idle(tdm_mod_t *tdm)
{
    return tdm->chip_data.token.token_idle;
}

/*!
 * \brief Get opportunistic-1 port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval opportunistic-1 port token.
 */
int
tdm_cmn_get_token_opp1(tdm_mod_t *tdm)
{
    return tdm->chip_data.token.token_opp1;
}

/*!
 * \brief Get opportunistic-2 port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval opportunistic-2 port token.
 */
int
tdm_cmn_get_token_opp2(tdm_mod_t *tdm)
{
    return tdm->chip_data.token.token_opp2;
}

/*!
 * \brief Get null port token.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval null port token.
 */
int
tdm_cmn_get_token_null(tdm_mod_t *tdm)
{
    return tdm->chip_data.token.token_null;
}
