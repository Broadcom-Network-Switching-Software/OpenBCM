/*! \file bcmpkt_buf.h
 *
 * Interfaces for packet buffer management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPKT_BUF_H
#define BCMPKT_BUF_H

#ifdef PKTIO_IMPL
#include <pktio_dep.h>
#else
#include <sal/sal_types.h>
#endif
#include <bcmpkt/bcmpkt_net.h>

/*! Shared buffer pool ID.
 *
 * Normal unit is for device specific DMA-capable buffer management.
 * Those buffers are used for zero copy, which request DMA-capable feature,
 * in UNET mode packet I/O. Shared buffer pool ID is used for managing
 * non-DMA-capable buffers for non-DMA cases like KNET mode packet I/O.
 */
#define BCMPKT_BPOOL_SHARED_ID     (-1)

/*! Default packet pool size (number of packets). */
#define BCMPKT_PPOOL_COUNT_DEF      128

/*! Default buffer pool size (number of buffers). */
#define BCMPKT_BPOOL_BCOUNT_DEF     256

/*! Minimum buffer size of the buffer pool to be created. */
#define BCMPKT_BPOOL_BSIZE_MIN      (1536)

/*! Minimum packet size. (not include CRC) */
#define BCMPKT_FRAME_SIZE_MIN       (60)

/*! Buffer operation overhead. */
#define BCMPKT_BUF_SIZE_OVERHEAD    (BCMPKT_RCPU_MAX_ENCAP_SIZE + \
                                     sizeof(bcmpkt_data_buf_t))

/**
 * \name BCMPKT buffer allocation flags.
 * \anchor BCMPKT_BUF_F_XXX
 */
/*! \{ */
/*! Reserve space for TX overhead.
 * This flag is used to reserve a space which size is BCMPKT_TX_HDR_RSV
 * in the front of data buffer for internal transmit overhead.
 */
#define BCMPKT_BUF_F_TX             (0x1 << 0)
/*! \} */

/*! Packet data buffer pool structure.
 *
 * Packet data buffer pool hosts buffers for bcmpkt_packet_t.data_buf
 * usage. Every buffer has same size (\c buf_size) which includes operation
 * overhead (\c BCMPKT_BUF_SIZE_OVERHEAD).
 *
 * Per device DMA-capable buffer is used for UNET mode to get performance from
 * zero-copy.
 *
 * The pool is available only when \c active is true and \c free_count
 * is not 0.
 */
typedef struct bcmpkt_bpool_status_s {
    /*! Active status. */
    bool active;
    /*! Buffer size. */
    uint32_t buf_size;
    /*! Total buffer count. */
    uint32_t buf_count;
    /*! Available buffer count. */
    uint32_t free_count;
} bcmpkt_bpool_status_t;

/*! Packet operation header pool.
 *
 * The pool is available only when \c active is true.
 * If \c free_count is 0, a new allocation of bcmpkt_packet_t will
 * be done from sal_alloc, and the new buffer will be added into
 * the packet pool.
 */
typedef struct bcmpkt_ppool_status_s {
    /*! Active status. */
    bool active;
    /*! Total packet count. */
    uint32_t pkt_count;
    /*! Available packet count. */
    uint32_t free_count;
} bcmpkt_ppool_status_t;

/*!
 * \brief Allocate a packet object.
 *
 * Create packet operation header and allocate data buffer.
 * If \c BCMPKT_BUF_F_TX is set, TX internal operation header space will be
 * created in the front of data_buf.
 *
 * \param [in] unit Switch unit number.
 * \param [in] len The size packet data buffer to be allocated (unit is byte).
 * \param [in] flags Reserved.
 * \param [out] packet Packet handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Input len is too small.
 * \retval SHR_E_MEMORY Allocate failed.
 */
extern int
bcmpkt_alloc(int unit, uint32_t len, uint32_t flags, bcmpkt_packet_t **packet);

/*!
 * \brief Release packet object.
 *
 * Release packet data buffer into bpool, and release packet operation header
 * buffer into ppool.
 *
 * \param [in] unit Switch unit number.
 * \param [in] packet Packet handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Parameter is NULL.
 */
extern int
bcmpkt_free(int unit, bcmpkt_packet_t *packet);

/*!
 * \brief Reserve buffer size in headroom.
 *
 * Reserve space in the buffer headroom for potential header encapsulation.
 * The reservation should be done prior to adding packet data (when packet len
 * is 0).
 *
 * \param [in,out] dbuf Packet data buffer handle.
 * \param [in] len Number of bytes to reserve.
 *
 * \return A pointer to the updated packet->data_buf->data.
 */
extern uint8_t *
bcmpkt_reserve(bcmpkt_data_buf_t *dbuf, uint32_t len);

/*!
 * \brief Add bytes in the front of packet data.
 *
 * This function extends the used data area in the beginning when the headroom
 * has enough space for the request.
 *
 * \param [in,out] dbuf Packet data buffer handle.
 * \param [in] len Bytes to add.
 *
 * \return A pointer to the first byte of the extra data. NULL for failure.
 */
extern uint8_t *
bcmpkt_push(bcmpkt_data_buf_t *dbuf, uint32_t len);

/*!
 * \brief Add bytes at the end of packet data.
 *
 * This function extends the used data area of the buffer at the buffer end when
 * the tailroom has enough space for the request.
 *
 * \param [in,out] dbuf Packet data buffer handle.
 * \param [in] len Number of bytes to add.
 *
 * \return A pointer to the first byte of the extra data. NULL for failure.
 */
extern uint8_t *
bcmpkt_put(bcmpkt_data_buf_t *dbuf, uint32_t len);

/*!
 * \brief Remove bytes from the start of packet data.
 *
 * This function removes some bytes from the start of a packet data, and returns
 * the memory to the headroom. A pointer to the next data in the packet data
 * buffer is returned. Once the data has been pulled, future pushes will
 * overwrite the old data.
 *
 * \param [in,out] dbuf Packet data buffer handle.
 * \param [in] len Bytes to be removed.
 *
 * \return A pointer to the next data in the buffer.
 */
extern uint8_t *
bcmpkt_pull(bcmpkt_data_buf_t *dbuf, uint32_t len);

/*!
 * \brief Remove end from packet buffer.
 *
 * Cut the length of a buffer down by removing data from the tail. If
 * the buffer is already under the length specified, will return with no action.
 *
 * \param [in,out] dbuf Packet data buffer handle.
 * \param [in] len New data length.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Input len is too small.
 */
extern int
bcmpkt_trim(bcmpkt_data_buf_t *dbuf, uint32_t len);

/*!
 * \brief Get headroom size.
 *
 * Get free space size before packet data in the packet buffer.
 *
 * \param [in] dbuf Packet data buffer handle.
 *
 * \return Available bytes for push.
 */
extern uint32_t
bcmpkt_headroom(bcmpkt_data_buf_t *dbuf);

/*!
 * \brief Get tailroom size.
 *
 *  Get free space after packet data in the packet buffer.
 *
 * \param [in] dbuf Packet data buffer handle.
 *
 * \return Available bytes for put.
 */
extern uint32_t
bcmpkt_tailroom(bcmpkt_data_buf_t *dbuf);

/*!
 * \brief Create a buffer pool.
 *
 * The \c buf_size must be large enough to hold the components show below,
 * where (m) denotes mandatory components (part of all packets) and (o)
 * denotes optional components.
 *
 *  (m) Maximum size ethernet packet
 *  (m) Buffer control (bcmpkt_data_buf_t)
 *  (o) RCPU header
 *  (o) Tx meta data (Tx PMD)
 *  (o) Loopback header
 *  (o) HiGig header
 *
 * The \c unit = \ref BCMPKT_BPOOL_SHARED_ID means the pool shared among units.
 *
 *
 * \param [in] unit Switch unit number.
 * \param [in] buf_size Number of bytes for each buffer.
 * \param [in] buf_count Number of buffers.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Input parameter(s) is(are) invalid.
 * \retval SHR_E_MEMORY Allocate failed.
 */
extern int
bcmpkt_bpool_create(int unit, int buf_size, int buf_count);

/*!
 * \brief Destroy a buffer pool.
 *
 * The \c unit = \ref BCMPKT_BPOOL_SHARED_ID means destroy the shared pool.
 *
 * \param [in] unit Switch unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmpkt_bpool_destroy(int unit);

/*!
 * \brief Destroy all buffer pools.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpkt_bpool_cleanup(void);

/*!
 * \brief Fetch a buffer pool status.
 *
 * The \c unit = \ref BCMPKT_BPOOL_SHARED_ID is for the shared pool.
 *
 * \param [in] unit Switch unit number.
 * \param [out] status Buffer pool status handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM The \c status is a NULL pointer.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmpkt_bpool_status_get(int unit, bcmpkt_bpool_status_t *status);

/*!
 * \brief Dump buffer pool information.
 *
 * Dump a buffer pool's configuration and status.
 * The \c unit = \ref BCMPKT_BPOOL_SHARED_ID is for shared pool.
 *
 * \param [in] unit Switch unit number.
 * \param [out] pb Print buffer handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM The \c pb is a NULL pointer.
 */
extern int
bcmpkt_bpool_info_dump(int unit, shr_pb_t *pb);

/*!
 * \brief Allocate a packet data buffer.
 *
 * \param [in] unit Switch unit number.
 * \param [in] size Number of bytes for each buffer.
 * \param [in] dbuf Data buffer handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Input parameter(s) is(are) invalid.
 * \retval SHR_E_MEMORY Out of memory.
 */
extern int
bcmpkt_data_buf_alloc(int unit, uint32_t size, bcmpkt_data_buf_t **dbuf);

/*!
 * \brief Release a packet data buffer.
 *
 * \param [in] unit Switch unit number.
 * \param [in] dbuf Data buffer handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Input parameter(s) is(are) invalid.
 * \retval SHR_E_MEMORY The dbuf does not belong to the unit's buffer pool.
 * \retval SHR_E_FAIL Could not get lock.
 */
extern int
bcmpkt_data_buf_free(int unit, bcmpkt_data_buf_t *dbuf);

/*!
 * \brief Clone a packet.
 *
 * Create a new packet, copy meta data content and refer to the same data_buf.
 *
 * \param [in] unit Switch number.
 * \param [in] pkt Packet handle.
 * \param [out] new_pkt New packet handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Input parameter(s) is(are) invalid.
 * \retval SHR_E_MEMORY Allocate failed.
 */
extern int
bcmpkt_packet_clone(int unit, bcmpkt_packet_t *pkt, bcmpkt_packet_t **new_pkt);

/*!
 * \brief Claim the packet.
 *
 * Create a new packet. Copy metadata. Try to take over\c pkt->data_buf. If
 * can't take over it because of different unit, will create a new data_buf
 * and copy content.
 *
 * \param [in] unit Switch number, from which buffer pool allocate new buffer.
 * \param [in] pkt Packet handle.
 * \param [out] new_pkt New packet handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Input parameter(s) is(are) invalid.
 * \retval SHR_E_MEMORY Allocate failed.
 */
extern int
bcmpkt_packet_claim(int unit, bcmpkt_packet_t *pkt, bcmpkt_packet_t **new_pkt);

/*!
 * \brief Create a new similar data buffer with same content.
 *
 * Allocate a new data buffer, and copy \c dbuf content to it.
 *
 * \param [in] unit Switch number, from which buffer pool allocate new buffer.
 * \param [in] dbuf Packet data buffer for copy.
 * \param [out] new_dbuf New packet data buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Input parameter(s) is(are) invalid.
 * \retval SHR_E_MEMORY Allocate failed.
 */
extern int
bcmpkt_data_buf_copy(int unit, bcmpkt_data_buf_t *dbuf,
                     bcmpkt_data_buf_t **new_dbuf);

/*!
 * \brief Create packet pool.
 *
 * This API is for creating a bcmpkt_packet_t object pool for allocation with
 * low allocation overhead.

 * If the packet pool is not active, bcmpkt_alloc() will allocate
 * bcmpkt_packet_t object from sal_alloc(), and bcmpkt_free() will release
 * the buffer through sal_free().
 *
 * If packet pool is active, bcmpkt_alloc() will allocate bcmpkt_packet_t
 * object from the packet pool. When there is not available packet buffer,
 * a new buffer will be allocated from sal_alloc() internally. And,
 * bcmpkt_free() is to return every packet buffer into packet pool, when
 * packet pool free count is under \c pkt_count. If free count reached
 * \c pkt_count, the buffer will be released by sal_free().
 *
 * \param [in] pkt_count Number of buffers.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Invalid packet count.
 * \retval SHR_E_MEMORY Allocate failed.
 */
extern int
bcmpkt_ppool_create(int pkt_count);

/*!
 * \brief Destroy packet pool.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmpkt_ppool_destroy(void);

/*!
 * \brief Dump packet pool information.
 *
 * \retval SHR_E_NONE No errors.
 */

/*!
 * \brief Fetch the packet pool status.
 *
 * \param [out] status Packet pool status handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM The \c status is a NULL pointer.
 */
extern int
bcmpkt_ppool_status_get(bcmpkt_ppool_status_t *status);

/*!
 * \brief Dump packet pool information.
 *
 * \param [out] pb Print buffer handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM The \c pb is a NULL pointer.
 */
extern int
bcmpkt_ppool_info_dump(shr_pb_t *pb);

/*!
 * \brief Format packet metadata structure.
 *
 * Link packet metadata pointers to their data without touching the data content.
 *
 * \param [in] packet Packet handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid packet handle.
 */
extern int
bcmpkt_pmd_format(bcmpkt_packet_t *packet);

/*!
 * \brief Get Higig handle from a packet handle.
 *
 * This function is for getting HiGig handle from a packet handle.
 *
 * \param [in] packet Packet handle.
 * \param [out] hg_hdr HiGig header handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid packet handle or invalid HiGig header handle.
 */
extern int
bcmpkt_higig_get(bcmpkt_packet_t *packet, uint32_t **hg_hdr);

/*!
 * \brief Get RXPMD handle from a packet handle.
 *
 * This function is for getting RXPMD handle from a packet handle.
 *
 * \param [in] packet Packet handle.
 * \param [out] rxpmd RX Packet MetaData handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid packet handle or invalid RXPMD handle.
 */
extern int
bcmpkt_rxpmd_get(bcmpkt_packet_t *packet, uint32_t **rxpmd);

/*!
 * \brief Get TXPMD handle from a packet handle.
 *
 * This function is for getting TXPMD handle from a packet handle.
 *
 * \param [in] packet Packet handle.
 * \param [out] txpmd TXPMD handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid packet handle or invalid TXPMD handle.
 */
extern int
bcmpkt_txpmd_get(bcmpkt_packet_t *packet, uint32_t **txpmd);

/*!
 * \brief Get loopback header handle from a packet handle.
 *
 * This function is for getting loopback header handle from a packet handle.
 *
 * \param [in] packet Packet handle.
 * \param [out] lbhdr Loopback header handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM nvalid packet handle or invalid loopback header handle.
 */
extern int
bcmpkt_lbhdr_get(bcmpkt_packet_t *packet, uint32_t **lbhdr);

/*!
 * \brief Set minimum packet size (not includng CRC).
 *
 * This function is used for setting allowable minimum packet size for packet
 * transmission (not includng CRC).
 *
 * The valid range for the minimum packet size is from [1 to 60]. The default
 * value is \ref BCMPKT_FRAME_SIZE_MIN.
 *
 * Note:
 * This is a debug function and should keep default value for normal case.
 *
 * \param [in] size The minimum packet size to use (unit is bytes).
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Requested minimum packet size is out of valid range.
 */
extern int
bcmpkt_framesize_min_set(uint32_t size);

/*!
 * \brief Get current minimum packet size (not includng CRC).
 *
 * This function is used for getting the current minimum packet size for packet
 * transmission. Default is \ref BCMPKT_FRAME_SIZE_MIN.
 *
 * \retval Current minimum packet size.
 */
extern uint32_t
bcmpkt_framesize_min_get(void);

#endif /* BCMPKT_BUF_H */
