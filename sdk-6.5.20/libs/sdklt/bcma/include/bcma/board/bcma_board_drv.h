/*! \file bcma_board_drv.h
 *
 * Internal board driver API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BOARD_DRV_H
#define BCMA_BOARD_DRV_H

#include <sal/sal_types.h>

/*! Initialize board. */
typedef int (*bcma_board_init_f)(int unit);

/*!
 * Board driver object.
 */
typedef struct bcma_board_drv_s {

    /*! Intialize board. */
    bcma_board_init_f board_init;

} bcma_board_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern bcma_board_drv_t *_bc##_board_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Initialize board driver.
 *
 * \param[in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcma_board_drv_init(int unit);

#endif /* BCMA_BOARD_DRV_H */
