/*! \file bcmcth_flex_qos_drv.h
 *
 * BCMCTH Flex QoS driver objects.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_FLEX_QOS_DRV_H
#define BCMCTH_FLEX_QOS_DRV_H

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
typedef int (*bcmcth_flex_qos_info_get_data_f)(int unit, bcmcth_info_table_t *data);

/*!
 * \brief FLEX_QOS driver object
 *
 * FLEX_QOS driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH FLEX_QOS module by
 * \ref bcmcth_flex_qos_drv_init() from the top layer. BCMCTH FLEX_QOS
 * internally will use this interface to get the corresponding information.
 */
typedef struct bcmcth_flex_qos_info_drv_s {

    /*! Derive variant based table id. */
    bcmcth_flex_qos_info_get_data_f          get_data;

} bcmcth_flex_qos_info_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
extern bcmcth_flex_qos_info_drv_t *_bd##_vu##_va##_cth_flex_qos_ing_info_drv_get(int unit);
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */

/*! \cond  Externs for the required functions. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
extern bcmcth_flex_qos_info_drv_t *_bd##_vu##_va##_cth_flex_qos_egr_info_drv_get(int unit);
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */

/*!
 * \brief Get the FLEX_QOS driver of the device.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_flex_qos_drv_init(int unit, bool warm);

/*!
 * \brief Register Flex QoS Ing Info IMM LTs callback functions to IMM.
 *
 * \param [in] unit Unit number.
 * \param [in] table_id Logical table ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
extern int
bcmcth_flex_qos_ing_info_imm_register(int unit, int table_id);

/*!
 * \brief Register Flex QoS Egr Info IMM LTs callback functions to IMM.
 *
 * \param [in] unit Unit number.
 * \param [in] table_id Logical table ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Unable to allocate required resources.
 */
extern int
bcmcth_flex_qos_egr_info_imm_register(int unit, int table_id);

#endif /* BCMCTH_FLEX_QOS_DRV_H */
