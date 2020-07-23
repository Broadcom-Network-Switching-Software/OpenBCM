/*! \file bcmpkt_buf_internal.h
 *
 * Packet buffer management internal interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPKT_BUF_INTERNAL_H
#define BCMPKT_BUF_INTERNAL_H

/*!
 * \brief Allocate a packet buffer from buffer pools.
 *
 * This function is used for bcmpkt_buf internally use only. The \c unit
 * and buf_size should be assured valid by caller.
 *
 * \param [in] unit Switch unit number.
 * \param [in] size Number of bytes of request.
 * \param [in] buf_size Real buffer size.
 *
 * \retval Buffer handle.
 */
extern void *
bcmpkt_bpool_alloc(int unit, uint32_t size, uint32_t *buf_size);

/*!
 * \brief Release a packet buffer to buffer pools.
 *
 * \param [in] unit Switch unit number.
 * \param [in] buf Buffer handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter(s) is(are) invalid.
 */
extern int
bcmpkt_bpool_free(int unit, void *buf);

/*!
 * \brief Allocate a packet from packet pool.
 *
 * \retval Packet handle.
 */
extern bcmpkt_packet_t *
bcmpkt_ppool_alloc(void);

/*!
 * \brief Release a packet to packet pool.
 *
 * \param [in] packet Packet handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input parameter(s) is(are) invalid.
 */
extern int
bcmpkt_ppool_free(bcmpkt_packet_t *packet);

#endif /* BCMPKT_BUF_INTERNAL_H */
