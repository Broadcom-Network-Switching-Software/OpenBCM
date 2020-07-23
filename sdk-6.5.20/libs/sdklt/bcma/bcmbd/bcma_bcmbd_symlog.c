/*! \file bcma_bcmbd_symlog.c
 *
 * BCMBD symbol log functions for manipulating the logging database.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcma/bcmbd/bcma_bcmbd_config.h>
#include <bcma/bcmbd/bcma_bcmbd_symlog.h>


#if BCMA_CONFIG_INCLUDE_SYMLOG == 1
extern bcma_bcmbd_symlog_drv_t bcma_bcmbd_symlog_drv_default;
#endif


/*******************************************************************************
 * Private functions
 */

static bcma_bcmbd_symlog_drv_t *
bcma_bcmbd_symlog_drv_get(int unit)
{
#if BCMA_CONFIG_INCLUDE_SYMLOG == 1
    return &bcma_bcmbd_symlog_drv_default;
#else
    return NULL;
#endif
}


/*******************************************************************************
 * Public functions
 */

int
bcma_bcmbd_symlog_set(int unit, uint32_t adext, uint32_t addr,
                      uint32_t sym_flags, bcmlt_pt_opcode_t op, int st)
{
    bcma_bcmbd_symlog_drv_t *drv;

    drv = bcma_bcmbd_symlog_drv_get(unit);
    if (!drv) {
        return 0;
    }

    return drv->schan_log_set(unit, adext, addr, sym_flags, op, st);
}

int
bcma_bcmbd_symlog_schan_cmd_set(int unit, uint32_t *schan_cmd, uint32_t wsize,
                                bcma_bcmbd_symlog_schan_cmd_decode_f func,
                                int st)
{
    bcma_bcmbd_symlog_drv_t *drv;
    bcma_bcmbd_symlog_info_t info;

    drv = bcma_bcmbd_symlog_drv_get(unit);
    if (!drv) {
        return 0;
    }

    if (!func) {
        return -1;
    }

    if (func(schan_cmd, wsize, &info) < 0) {
        return -1;
    }

    return drv->schan_log_set(unit, info.adext, info.addr, info.sym_flags,
                              info.op, st);
}

int
bcma_bcmbd_symlog_cmic_set(int unit, uint32_t addr, uint32_t sym_flags,
                           bcmlt_pt_opcode_t op, int st)
{
    bcma_bcmbd_symlog_drv_t *drv;

    drv = bcma_bcmbd_symlog_drv_get(unit);
    if (!drv) {
        return 0;
    }

    return drv->cmic_log_set(unit, addr, sym_flags, op, st);
}

