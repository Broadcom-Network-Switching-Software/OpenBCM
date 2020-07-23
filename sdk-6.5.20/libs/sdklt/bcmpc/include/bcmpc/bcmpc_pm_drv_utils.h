/*! \file bcmpc_pm_drv_utils.h
 *
 * PortMacro Manager (PM) utility APIs.
 *
 * The PortMacro  provides the functions to configure the whole PM, the
 * core within a PM or the lane within a PM. The library provides a common
 * interface for all the PM types. The PM specific implementation is internal
 * to the related PM.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPC_PM_DRV_UTILS_H
#define BCMPC_PM_DRV_UTILS_H

#include <bcmpc/bcmpc_pm.h>
#include <phymod/phymod.h>


/*!
 * \brief Get the port interface type based on the speed,
 * number of lanes of the port.
 *
 * This function is used to validate the PM-specific operating mode.
 *
 * \param [in]  unit Unit number.
 * \param [in]  pm_info Port Macro database information.
 * \param [in]  speed_cfg Port speed config.
 * \param [out] interface_type port interface type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
pm_drv_port_interface_type_get(int unit, pm_info_t *pm_info,
                               pm_speed_config_t *speed_cfg,
                               phymod_interface_t *interface_type);

/*!
 * \brief Update operation status.
 *
 * This function is used to update PM operation status.
 *
 * \param [in] op_st PM operation status data structure.
 * \param [in] st Operation status to be updated.
 */
extern void
pm_port_oper_status_set(pm_oper_status_t *op_st, pm_oper_status_type_t st);

/*!
 * \brief PM port ability to Phymod ability conversion.
 *
 * This function will convert pm_port_ability_t to
 * phymod_autoneg_advert_abilities_t data type.
 *
 * \param [in] unit Unit number.
 * \param [in] num_abilities Number of abilities in input array \c abilities.
 * \param [in] abilities PM type ability array.
 * \param [out] an_abilities Phymod type ability array.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Invalid input for conversion.
 */
extern int
pm_port_ability_to_phy_ability_get(int unit, int num_abilities,
                              pm_port_ability_t *abilities,
                              phymod_autoneg_advert_abilities_t *an_abilities);

/*!
 * \brief Phymod ability to PM port ability conversion.
 *
 * This function will convert phymod_autoneg_advert_abilities_t to
 * pm_port_ability_t data type.
 *
 * \param [in] unit Unit number.
 * \param [in] an_abilities Phymod type ability array.
 * \param [out] num_abilities Number of abilities in output array \c abilities.
 * \param [out] abilities PM type ability array.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_PARAM Invalid input for conversion.
 */
extern int
pm_phy_ability_to_port_ability_get(int unit,
                         const phymod_autoneg_advert_abilities_t *an_abilities,
                         uint32_t *num_abilities, pm_port_ability_t *abilities);
#endif /* BCMPC_PM_DRV_UTILS_H */
