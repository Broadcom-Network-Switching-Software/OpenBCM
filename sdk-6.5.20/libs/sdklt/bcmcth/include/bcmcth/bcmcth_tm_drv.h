/*! \file bcmcth_tm_drv.h
 *
 * BCMCTH TM driver objects.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TM_DRV_H
#define BCMCTH_TM_DRV_H

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
typedef int (*bcmcth_tm_info_get_data_f)(int unit, bcmcth_info_table_t *data);

/*!
 * \brief TM driver object
 *
 * TM driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH TM module.
 * BCMCTH TM internally will use this interface to get the
 * corresponding information.
 */
typedef struct bcmcth_tm_info_drv_s {

    /*! Derive variant based table id. */
    bcmcth_tm_info_get_data_f          get_data;

} bcmcth_tm_info_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
extern bcmcth_tm_info_drv_t *_bd##_vu##_va##_cth_tm_q_assignment_info_drv_get(int unit);
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */


/*! \cond  Externs for the required functions. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
extern bcmcth_tm_info_drv_t *_bd##_vu##_va##_cth_tm_cos_q_cpu_map_info_drv_get(int unit);
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */

#endif /* BCMCTH_TM_DRV_H */
