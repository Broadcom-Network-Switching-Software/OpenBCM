/*! \file bcmlm_chip_internal.h
 *
 * Link Manager per-chip driver definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLM_CHIP_INTERNAL_H
#define BCMLM_CHIP_INTERNAL_H

#include <bcmlm/bcmlm_types.h>
#include <bcmlm/bcmlm_drv_internal.h>

/*******************************************************************************
 * Typedefs
 */

/*******************************************************************************
 * Internal public Functions
 */

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_lm_drv_attach(bcmlm_drv_t *drv); \
extern int _bc##_lm_drv_detach(bcmlm_drv_t *drv);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

#endif /* BCMLM_CHIP_INTERNAL_H */
