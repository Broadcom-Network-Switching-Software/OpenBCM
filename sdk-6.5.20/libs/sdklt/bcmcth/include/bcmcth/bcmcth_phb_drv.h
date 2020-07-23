/*! \file bcmcth_phb_drv.h
 *
 * BCMCTH PHB ingress map Driver Object
 *
 * Declaration of the public structures, enumerations, and functions
 * which interface into the base driver (BD) component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_PHB_DRV_H
#define BCMCTH_PHB_DRV_H

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmcth/bcmcth_phb_ing_map.h>

/*!
 * \brief Get device-specific PHB ingress map register information.
 *
 * \param [in]  unit Unit number.
 * \param [out] ingress PHB map Device-specific protocol information.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int
(*bcmcth_phb_ing_map_get_f)(int unit, bcmcth_phb_ing_map_t *ing_map_info);

/*!
 * \brief Get device-specific PHB ingress map register information.
 *
 * \param [in]  unit Unit number.
 * \param [out] ingress PHB map Device-specific protocol information.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int
(*bcmcth_phb_egr_botp_config_get_f)(int unit,
                                    int *total_entry,
                                    bcmcth_phb_egr_botp_config_t *egr_botp_config_info);

/*!
 * \brief  Register device-specific IMM callback function for ingress PHB.
 *
 * \param [in]  unit Unit number.
 * \param [out] flag Indicates whether to register IMM callback function for ingress PHB.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int
(*bcmcth_phb_ing_imm_register_f)(int unit, bool *flag);

/*!
 * \brief PHB driver object
 *
 * PHB driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH PHB module by
 * \ref bcmcth_phb_drv_init() from the top layer. BCMCTH PHB internally
 * will use this interface to get the corresponding information.
 */
typedef struct bcmcth_phb_drv_s {
    /*! Get PHB Hardware map table information. */
    bcmcth_phb_ing_map_get_f ing_map_get;

    /*! Get PHB egress BOTP configuration */
    bcmcth_phb_egr_botp_config_get_f egr_botp_config_get;

    /*! Register ingress imm call back functions. */
    bcmcth_phb_ing_imm_register_f ing_imm_register;
} bcmcth_phb_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern bcmcth_phb_drv_t *_bc##_cth_phb_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Get the PHB driver of the device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_phb_drv_init(int unit);

/*!
 * \brief Get device-specific PHB protocol information.
 *
 * \param [in] unit Unit number.
 * \param [out] ing_map_info Device-specific PHB information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_phb_ing_map_get(int unit, bcmcth_phb_ing_map_t *ing_map_info);

/*!
 * \brief Get device-specific PHB protocol information.
 *
 * \param [in]  unit         Unit number.
 * \param [in]  total_entry  Total entries needs to be configured.
 * \param [out] egr_botp_config_info  Device-specific PHB information.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_phb_egr_botp_config_get(int unit,
                               int *total_entry,
                               bcmcth_phb_egr_botp_config_t *egr_botp_config_info);

/*!
 * \brief Probe whether IMM call back function needs to be registered.
 *
 * \param [in]  unit         Unit number.
 * \param [out] flag         Flag to indicates wither IMM call back is needed.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_phb_ing_imm_register(int unit, bool *flag);

#endif /* BCMCTH_PHB_DRV_H */
