/*! \file bcma_board.c
 *
 * Board API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcmdrd/bcmdrd_dev.h>

#include <bcma/board/bcma_board_drv.h>
#include <bcma/board/bcma_board.h>

/*! Device specific attach function type. */
typedef bcma_board_drv_t *(*bcma_board_drv_get_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_board_drv_get },

static struct {
    bcma_board_drv_get_f drv_get;
} board_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

/*******************************************************************************
 * Local functions
 */

static bcma_board_drv_t *
bcma_board_drv_get(int unit)
{
    bcmdrd_dev_type_t dev_type;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return NULL;
    }

    if (!BCMDRD_UNIT_VALID(unit)) {
        return NULL;
    }

    return board_drv_get[dev_type].drv_get(unit);;
}

/*******************************************************************************
 * Public functions
 */

int
bcma_board_init(int unit)
{
    bcma_board_drv_t *drv = bcma_board_drv_get(unit);

    if (drv && drv->board_init) {
        return drv->board_init(unit);
    }

    return SHR_E_NONE;
}
