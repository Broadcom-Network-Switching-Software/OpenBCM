/*! \file bcm56996_a0_testutil_traffic.h
 *
 * Chip-specific test utility for TRAFFIC.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCM56996_A0_TESTUTIL_TRAFFIC_H
#define BCM56996_A0_TESTUTIL_TRAFFIC_H

#include <bcmdrd/bcmdrd_types.h>


/*!
 * \brief Get expected port rate array.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Port bitmap.
 * \param [in] pbmp_oversub Oversub port bitmap.
 * \param [in] pkt_size Packet length.
 * \param [in] array_size Array size for ret_exp_rate.
 * \param [out] ret_exp_rate Ptr to expected port rate array.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcm56996_a0_traffic_get_expeceted_rate(int unit,
                                       bcmdrd_pbmp_t pbmp,
                                       bcmdrd_pbmp_t pbmp_oversub,
                                       int pkt_size,
                                       int array_size,
                                       uint64_t *ret_exp_rate);

/*!
 * \brief Get fixed packet count by packet length and total cell numbers.
 *
 * \param [in] unit Unit number.
 * \param [in] pkt_len Packet length.
 * \param [out] ret_pkt_cnt Ptr to packet count.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcm56996_a0_traffic_get_fixed_packet_list(int unit, uint32_t pkt_len,
                                          uint32_t *ret_pkt_cnt);

/*!
 * \brief Get random packet length and count.
 *
 * \param [in] unit Unit number.
 * \param [in] cellnums_limit Max number of cells per packet.
 * \param [in] array_size Array size for ret_pkt_len and ret_pkt_cnt.
 * \param [out] ret_pkt_len Ptr to packet length array.
 * \param [out] ret_pkt_cnt Ptr to packet count array.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcm56996_a0_traffic_get_random_packet_list(int unit, uint32_t cellnums_limit,
                                           uint32_t array_size,
                                           uint32_t *ret_pkt_len,
                                           uint32_t *ret_pkt_cnt);

/*!
 * \brief Get worst-case packet length.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 * \param [out] ret_pkt_len Ptr to packet length.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcm56996_a0_traffic_get_worstcase_pktlen(int unit, int port,
                                         uint32_t *ret_pkt_len);

/*!
 * \brief Check health of device at end of test.
 *
 * \param [in] unit Unit number.
 * \param [out] test_result Test result.
 *
 * \retval SHR_E_NONE No error.
 */
extern int
bcm56996_a0_check_health(int unit, bool *test_result);


/*!
 * \brief Chip specific help API.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcm56996_a0_power_help(int unit);

/*!
 * \brief Chip specific param init for power test.
 *
 * \param [in] unit Unit number.
 * \param [in] bp Param pointer.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
 extern int
bcm56996_a0_power_init(int unit, void *bp);

/*!
 * \brief Chip specific port config.
 *
 * \param [in] unit Unit number.
 * \param [in] bp Param pointer.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
 extern int
bcm56996_a0_power_port_set(int unit, void *bp);

/*!
 * \brief Chip specific max power scenario setup.
 *
 * \param [in] unit Unit number.
 * \param [in] bp Param pointer.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcm56996_a0_power_config_set(int unit, void *bp);

/*!
 * \brief Chip specific API to run power test.
 *
 * \param [in] unit Unit number.
 * \param [in] bp Param pointer.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcm56996_a0_power_txrx(int unit, void *bp);


#endif /* BCM56996_A0_TESTUTIL_TRAFFIC_H */
