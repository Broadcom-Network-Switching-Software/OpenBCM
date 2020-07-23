/*! \file link.h
 *
 * Link module definitions internal to the BCM LTSW library.
 *
 * Note, this file should only contain declarations for cross-moudle usage.
 * Functions/structures used inside Link module should be declared in
 * link_int.h.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_LINK_H
#define BCMI_LTSW_LINK_H

/*!
 * \brief Add/Remove given port to skip_info pbmp.
 *
 * Add/Remove given port to skip_info pbmp to indicate if skip port_info_get
 * during link down event notification or not.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port to set/remove.
 * \param [in] skip Identificator to set or remove.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PORT Invalid port.
 */
extern int
bcmi_ltsw_link_port_info_skip_set(int unit, bcm_port_t port, int skip);

/*!
 * \brief Get status of given port if skip port_info_get or not.
 *
 * Get status of given port to indicate if skip port_info_get during link
 * down event notification or not.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port to set/remove.
 * \param [out] skip Identificator to set or remove.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PORT Invalid port.
 */
extern int
bcmi_ltsw_link_port_info_skip_get(int unit, bcm_port_t port, int *skip);

/*!
 * \brief Get current link status for the given port.
 *
 * Return logical link state if linkscan is enabled on the port. Otherwise,
 * return PHY link state.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] link Current link status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PORT Invalid port.
 * \retval SHR_E_DISABLED Port not being scanned.
 */
extern int
bcmi_ltsw_link_state_get(int unit, bcm_port_t port, int *link);

/*!
 * \brief Get port characteristics from PHY and program MAC to match.
 *
 * This API is called only for ports that are not scaned.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [in] link Link status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PORT Invalid port.
 */
extern int
bcmi_ltsw_link_port_update(int unit, bcm_port_t port, int link);

/*!
 * \brief Initialize linkscan port control for newly added port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PORT Invalid port.
 */
extern int
bcmi_ltsw_link_port_control_attach(int unit, bcm_port_t port);

/*!
 * \brief Clear linkscan port control of given port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PORT Invalid port.
 */
extern int
bcmi_ltsw_link_port_control_detach(int unit, bcm_port_t port);

#endif /* !BCMI_LTSW_LINK_H */

