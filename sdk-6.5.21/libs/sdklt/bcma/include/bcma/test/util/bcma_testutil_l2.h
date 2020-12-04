/*! \file bcma_testutil_l2.h
 *
 * L2 operation utility.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_TESTUTIL_L2_H
#define BCMA_TESTUTIL_L2_H

#include <shr/shr_types.h>
#include <bcmdrd/bcmdrd_types.h>

/*!
 * \brief Add a L2 unicast forward entity.
 *
 * \param [in] unit Unit number.
 * \param [in] port The port number.
 * \param [in] mac The destination mac address.
 * \param [in] vid The Vlan ID.
 * \param [in] copy2cpu Copy to CPU.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_l2_unicast_add(int unit, int port, const shr_mac_t mac, int vid,
                             int copy2cpu);

/*!
 * \brief Update a L2 unicast forward entity.
 *
 * \param [in] unit Unit number.
 * \param [in] port The port number.
 * \param [in] mac The destination mac address.
 * \param [in] vid The Vlan ID.
 * \param [in] copy2cpu Copy to CPU.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_l2_unicast_update(int unit, int port, const shr_mac_t mac,
                                int vid, int copy2cpu);

/*!
 * \brief Delete a L2 unicast forward entity.
 *
 * \param [in] unit Unit number.
 * \param [in] port The port number.
 * \param [in] mac The destination mac address.
 * \param [in] vid The Vlan ID.
 * \param [in] copy2cpu Copy to CPU.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_l2_unicast_delete(int unit, int port, const shr_mac_t mac,
                                int vid, int copy2cpu);

/*!
 * \brief Add a L2 multicast forward entity.
 *
 * \param [in] unit Unit number.
 * \param [in] group_id The multicast group id.
 * \param [in] pbmp The destination port bitmap.
 * \param [in] mac The destination mac address.
 * \param [in] vid The Vlan ID.
 * \param [in] copy2cpu Copy to CPU.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_l2_multicast_add(int unit, int group_id, bcmdrd_pbmp_t pbmp,
                               const shr_mac_t mac, int vid, int copy2cpu);

/*!
 * \brief Delete a L2 multicast forward entity.
 *
 * \param [in] unit Unit number.
 * \param [in] group_id The multicast group id.
 * \param [in] pbmp The destination port bitmap.
 * \param [in] mac The destination mac address.
 * \param [in] vid The Vlan ID.
 * \param [in] copy2cpu Copy to CPU.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_l2_multicast_delete(int unit, int group_id, bcmdrd_pbmp_t pbmp,
                                  const shr_mac_t mac, int vid, int copy2cpu);


/*!
 * \brief Update destination of a L2 multicast forward entity.
 *
 * \param [in] unit Unit number.
 * \param [in] group_id The multicast group id.
 * \param [in] pbmp The destination port bitmap.
 * \param [in] mac The destination mac address.
 * \param [in] vid The Vlan ID.
 * \param [in] copy2cpu Copy to CPU.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcma_testutil_l2_multicast_update(int unit, int group_id, bcmdrd_pbmp_t pbmp,
                                  const shr_mac_t mac, int vid, int copy2cpu);

/*!
 * \brief Add a L2 unicast forward entity.
 *
 * \param [in] unit Unit number.
 * \param [in] port The port number.
 * \param [in] mac The destination mac address.
 * \param [in] vid The Vlan ID.
 * \param [in] copy2cpu Copy to CPU.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int
(*bcma_testutil_l2_unicast_add_f)(int unit, int port, const shr_mac_t mac,
                                  int vid, int copy2cpu);

/*!
 * \brief Update a L2 unicast forward entity.
 *
 * \param [in] unit Unit number.
 * \param [in] port The port number.
 * \param [in] mac The destination mac address.
 * \param [in] vid The Vlan ID.
 * \param [in] copy2cpu Copy to CPU.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int
(*bcma_testutil_l2_unicast_update_f)(int unit, int port, const shr_mac_t mac,
                                     int vid, int copy2cpu);

/*!
 * \brief Delete a L2 unicast forward entity.
 *
 * \param [in] unit Unit number.
 * \param [in] port The port number.
 * \param [in] mac The destination mac address.
 * \param [in] vid The Vlan ID.
 * \param [in] copy2cpu Copy to CPU.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int
(*bcma_testutil_l2_unicast_delete_f)(int unit, int port, const shr_mac_t mac,
                                     int vid, int copy2cpu);


/*!
 * \brief Add a L2 multicast forward entity.
 *
 * \param [in] unit Unit number.
 * \param [in] group_id The multicast group id.
 * \param [in] pbmp The destination port bitmap.
 * \param [in] mac The destination mac address.
 * \param [in] vid The Vlan ID.
 * \param [in] copy2cpu Copy to CPU.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (*bcma_testutil_l2_multicast_add_f)(int unit, int group_id,
                                                bcmdrd_pbmp_t pbmp,
                                                const shr_mac_t mac, int vid,
                                                int copy2cpu);

/*!
 * \brief Delete a L2 multicast forward entity.
 *
 * \param [in] unit Unit number.
 * \param [in] group_id The multicast group id.
 * \param [in] pbmp The destination port bitmap.
 * \param [in] mac The destination mac address.
 * \param [in] vid The Vlan ID.
 * \param [in] copy2cpu Copy to CPU.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (*bcma_testutil_l2_multicast_delete_f)(int unit, int group_id,
                                                   bcmdrd_pbmp_t pbmp,
                                                   const shr_mac_t mac, int vid,
                                                   int copy2cpu);

/*!
 * \brief Update destination a L2 multicast forward entity.
 *
 * \param [in] unit Unit number.
 * \param [in] group_id The multicast group id.
 * \param [in] pbmp The destination port bitmap.
 * \param [in] mac The destination mac address.
 * \param [in] vid The Vlan ID.
 * \param [in] copy2cpu Copy to CPU.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
typedef int (*bcma_testutil_l2_multicast_update_f)(int unit, int group_id,
                                                   bcmdrd_pbmp_t pbmp,
                                                   const shr_mac_t mac, int vid,
                                                   int copy2cpu);


/*! L2 operation functions. */
typedef struct bcma_testutil_l2_op_s {
    /*! L2 unicast forward entry add function. */
    bcma_testutil_l2_unicast_add_f unicast_add;

    /*! L2 unicast forward entry update function. */
    bcma_testutil_l2_unicast_update_f unicast_update;

    /*! L2 unicast forward entry delete function. */
    bcma_testutil_l2_unicast_delete_f unicast_delete;

    /*! L2 multicast forward entry add function. */
    bcma_testutil_l2_multicast_add_f multicast_add;

    /*! L2 multicast forward entry delete function. */
    bcma_testutil_l2_multicast_delete_f multicast_delete;

    /*! L2 multicast forward entry destination pbmp update function. */
    bcma_testutil_l2_multicast_update_f multicast_update;
} bcma_testutil_l2_op_t;

/*! XGS device L2 operation functions. */
extern bcma_testutil_l2_op_t *bcma_testutil_xgs_l2_op_get(int unit);

/*! XGS device VFI L2 operation functions. */
extern bcma_testutil_l2_op_t *bcma_testutil_xgs_vfi_l2_op_get(int unit);

/*! XFS device L2 operation functions. */
extern bcma_testutil_l2_op_t *bcma_testutil_xfs_l2_op_get(int unit);

#endif /* BCMA_TESTUTIL_L2_H */
