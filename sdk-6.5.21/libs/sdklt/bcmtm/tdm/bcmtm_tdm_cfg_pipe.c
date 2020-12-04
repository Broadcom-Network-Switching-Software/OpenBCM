/*! \file bcmtm_tdm_cfg_pipe.c
 *
 * TDM core functions for pipe configuration access.
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
 * \brief Get pipe config data on slot granularity.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Slot unit in Mbps.
 *
 * Slot_unit represents the bandwidth per slot of linerate main calendar.
 * Note that the slot unit has the same precision of port speed.
 */
int
bcmtm_tdm_pipe_slot_unit_get(tdm_mod_t *tdm)
{
    int slot_unit;

    slot_unit = tdm->core_data.cfg.slot_unit;
    /* NOTE: add new slot_unit below for backward consistency */
    switch (slot_unit) {
        case TDM_SLOT_UNIT_1G:
        case TDM_SLOT_UNIT_1P25G:
        case TDM_SLOT_UNIT_2P5G:
        case TDM_SLOT_UNIT_5G:
        case TDM_SLOT_UNIT_10G:
        case TDM_SLOT_UNIT_25G:
        case TDM_SLOT_UNIT_50G:
            break;
        default:
            slot_unit = TDM_SLOT_UNIT_2P5G;
            break;
    }

    return slot_unit;
}

/*!
 * \brief Get pipe config data on pipe index.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pipe ID.
 */
int
bcmtm_tdm_pipe_id_get(tdm_mod_t *tdm)
{
    return tdm->core_data.cfg.pipe_id;
}

/*!
 * \brief Get pipe config data on the lowest pipe port.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Lowest port number.
 */
int
bcmtm_tdm_pipe_port_lo_get(tdm_mod_t *tdm)
{
    return tdm->core_data.cfg.pipe_port_lo;
}

/*!
 * \brief Get pipe config data on the highest pipe port.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Highest port number.
 */
int
bcmtm_tdm_pipe_port_hi_get(tdm_mod_t *tdm)
{
    return tdm->core_data.cfg.pipe_port_hi;
}

/*!
 * \brief Get pipe config data on limitiation of linerate slots.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of max linerate slots.
 */
int
bcmtm_tdm_pipe_lr_limit_get(tdm_mod_t *tdm)
{
    return tdm->core_data.cfg.num_lr_limit;
}

/*!
 * \brief Get pipe config data on limitiation of ancilary slots.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of max ancilary slots.
 */
int
bcmtm_tdm_pipe_ancl_limit_get(tdm_mod_t *tdm)
{
    return tdm->core_data.cfg.num_ancl_limit;
}

/*!
 * \brief Get pipe variable on linerate enable.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pipe linerate enable indicator.
 */
char
bcmtm_tdm_pipe_lr_en_get(tdm_mod_t *tdm)
{
    return tdm->core_data.var.lr_en;
}

/*!
 * \brief Get pipe variable on oversub enable.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pipe oversub enable indicator.
 */
char
bcmtm_tdm_pipe_os_en_get(tdm_mod_t *tdm)
{
    return tdm->core_data.var.os_en;
}

/*!
 * \brief Get pipe variable on linerate port count.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of linerate ports in pipe.
 */
int
bcmtm_tdm_pipe_lr_cnt_get(tdm_mod_t *tdm)
{
    return tdm->core_data.var.lr_cnt;
}

/*!
 * \brief Get pipe variable on oversub port count.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of oversub ports in pipe.
 */
int
bcmtm_tdm_pipe_os_cnt_get(tdm_mod_t *tdm)
{
    return tdm->core_data.var.os_cnt;
}

/*!
 * \brief Get number of speed types for pipe linerate ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of speed types of pipe linerate ports.
 */
int
bcmtm_tdm_pipe_lr_spd_types_get(tdm_mod_t *tdm)
{
    return tdm->core_data.var.lr_spd_types;
}

/*!
 * \brief Get number of speed types for pipe oversub ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Number of speed types of pipe oversub ports.
 */
int
bcmtm_tdm_pipe_os_spd_types_get(tdm_mod_t *tdm)
{
    return tdm->core_data.var.os_spd_types;
}

/*!
 * \brief Get pointer of port buffer for pipe linerate ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pointer of pipe linerate port buffer.
 */
int *
bcmtm_tdm_pipe_lr_buff_get(tdm_mod_t *tdm)
{
    return tdm->core_data.var.lr_buff;
}

/*!
 * \brief Get pointer of port count buffer for pipe oversub ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pointer of pipe oversub port buffer.
 */
int *
bcmtm_tdm_pipe_os_buff_get(tdm_mod_t *tdm)
{
    return tdm->core_data.var.os_buff;
}

/*!
 * \brief Get pointer of port count per speed for pipe linerate ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pointer of speed-based oversub port count buffer.
 */
int *
bcmtm_tdm_pipe_lr_spd_prt_cnt_get(tdm_mod_t *tdm)
{
    return tdm->core_data.var.lr_spd_prt_cnt;
}

/*!
 * \brief Get pointer of port count per speed for pipe oversub ports.
 *
 * \param [in] tdm Pointer of TDM module.
 *
 * \retval Pointer of speed-based linerate port count buffer.
 */
int *
bcmtm_tdm_pipe_os_spd_prt_cnt_get(tdm_mod_t *tdm)
{
    return tdm->core_data.var.os_spd_prt_cnt;
}
