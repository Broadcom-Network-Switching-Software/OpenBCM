/*! \file bcma_i2ccmd_i2c.c
 *
 *  CLI command for I2C operation user interface.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <bcma/i2c/bcma_i2ccmd_i2c.h>
#include <bcma/i2c/bcma_i2ccmd_op.h>

#define BSL_LOG_MODULE  BSL_LS_APPL_I2C

/*! Device specific attach function type. */
typedef bcma_i2c_device_drv_t *(*bcma_i2c_device_drv_get_f)(int unit);

/*! Array of device specific attach functions */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    { _bc##_i2c_drv_get },

static struct {
    bcma_i2c_device_drv_get_f drv_get;
} bcma_i2c_drv_get[] = {
    { 0 }, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    { 0 } /* end-of-list */
};

bcma_i2c_device_drv_t *bcma_i2c_device_drv[BCMDRD_CONFIG_MAX_UNITS];

/*!
 * \brief Init I2C device driver per board type.
 *
 * \param[in] unit Unit number.
 *
 * \return converted value.
 */
int
bcma_i2c_board_drv_init(int unit)
{
    bcmdrd_dev_type_t dev_type;

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        return SHR_E_UNIT;
    }

    if (!BCMDRD_UNIT_VALID(unit)) {
        return SHR_E_UNIT;
    }

    bcma_i2c_device_drv[unit] = bcma_i2c_drv_get[dev_type].drv_get(unit);;

    return SHR_E_NONE;
}
