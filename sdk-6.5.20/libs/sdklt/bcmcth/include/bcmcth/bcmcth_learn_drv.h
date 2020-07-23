/*! \file bcmcth_learn_drv.h
 *
 * BCMCTH Learn Cache driver objects.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_LEARN_DRV_H
#define BCMCTH_LEARN_DRV_H

#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmcth/bcmcth_info_table_entry.h>

/*!
 * \brief Derive variant based table id.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
typedef int (*bcmcth_learn_info_get_data_f)(int unit, bcmcth_info_table_t *data);

/*!
 * \brief LEARN_CACHE driver object
 *
 * LEARN_CACHE driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH L2 module. BCMCTH L2
 * internally will use this interface to get the corresponding information.
 */
typedef struct bcmcth_learn_info_drv_s {

    /*! Derive variant based table id. */
    bcmcth_learn_info_get_data_f get_data;
} bcmcth_learn_info_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
extern bcmcth_learn_info_drv_t *_bd##_vu##_va##_cth_learn_cache_data_info_drv_get(int unit);
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */

#endif /* BCMCTH_LEARN_DRV_H */
