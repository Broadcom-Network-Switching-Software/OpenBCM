/*! \file port.h
 *
 * Port Module Emulator above SDKLT
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM_LTSW_ID_MGMT_H
#define BCM_LTSW_ID_MGMT_H

#include <bcm/types.h>

#include <sal/sal_types.h>


#define BCMI_L2_IF_INVALID      0xFFFF

/*!
 * \brief Initialize ID Management module.
 *
 * \param [in] unit Unit Number.
 * \param [in] size Size of ID pool.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_id_mgmt_init(int unit, int size);

/*!
 * \brief De-initialize ID Management module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_id_mgmt_deinit(int unit);

/*!
 * \brief Recover ID Management module.
 *
 * \param [in] unit Unit Number.
 * \param [in] size Size of L2_IF pool.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_id_mgmt_recover(int unit);

/*!
 * \brief Reserve a range of IDs from L2_IF pool. Must be called before alloc.
 *
 * \param [in] unit Unit Number.
 * \param [in] l2_if Start ID.
 * \param [in] num Number of IDs to reserve.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_id_l2_if_reserve(int unit, int l2_if, int num);

/*!
 * \brief Allocate an ID from L2_IF pool.
 *
 * \param [in] unit Unit number.
 * \param [out] l2_if Allocated ID.
 *
 * \retval SHR_E_EMPTY No available ID to allocacte.
 */
extern int
bcmi_ltsw_id_l2_if_alloc(int unit, int *l2_if);

/*!
 * \brief Free an ID to L2_IF pool.
 *
 * \param [in] unit Unit number.
 * \param [in] l2_if ID to free.
 *
 * \retval SHR_E_FULL The ID is already free.
 */
extern int
bcmi_ltsw_id_l2_if_free(int unit, int l2_if);

/*!
 * \brief Allocate an ID for port or gport. The ID is not exposed to caller.
 *
 * \param [in] unit Unit number.
 * \param [in] gport Gport number.
 *
 * \retval SHR_E_EMPTY No available ID to allocacte.
 */
extern int
bcmi_ltsw_id_port_reserve(int unit, int gport);

/*!
 * \brief Free an ID for port or gport.
 *
 * \param [in] unit Unit number.
 * \param [in] gport Gport number.
 *
 * \retval SHR_E_FULL The ID is already free.
 */
extern int
bcmi_ltsw_id_port_free(int unit, int gport);

/*!
 * \brief Map L2 interface with Gport.
 *
 * \param [in] unit Unit number.
 * \param [in] l2_if L2 interface.
 * \param [in] gport Gport number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_id_l2_if_map_port(int unit, int l2_if, int gport);

/*!
 * \brief Demap L2 interface with Gport.
 *
 * \param [in] unit Unit number.
 * \param [in] l2_if L2 interface.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_id_l2_if_demap_port(int unit, int l2_if);

/*!
 * \brief Get gport from L2 interface.
 *
 * \param [in] unit Unit number.
 * \param [in] l2_if L2 interface.
 * \param [out] port Gport.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_id_l2_if_to_port(int unit, int l2_if, int *port);

/*!
 * \brief Get L2 interface from port or gport.
 *
 * \param [in] unit Unit number.
 * \param [in] port Local logical port or gport.
 * \param [out] l2_if L2 interface.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_id_port_to_l2_if(int unit, int port, int *l2_if);

/*!
 * \brief Set LAG of the port or gport.
 *
 * \param [in] unit Unit number.
 * \param [in] gport Local logical port or gport.
 * \param [in] gport_lag Gport number of LAG.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_id_port_lag_set(int unit, int gport, int gport_lag);

/*!
 * \brief Get LAG of the port or gport.
 *
 * \param [in] unit Unit number.
 * \param [in] gport Local logical port or gport.
 * \param [out] gport_lag Gport number of LAG.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmi_ltsw_id_port_lag_get(int unit, int gport, int *gport_lag);

/*!
 * \brief Dump L2_IF info.
 *
 * \param [in] unit Unit Number.
 */
extern int
bcmi_ltsw_id_l2_if_dump_sw(int unit);

/*!
 * \brief Dump gport info.
 *
 * \param [in] unit Unit Number.
 */
extern int
bcmi_ltsw_id_port_dump_sw(int unit);

#endif /* BCM_LTSW_ID_MGMT_H */

