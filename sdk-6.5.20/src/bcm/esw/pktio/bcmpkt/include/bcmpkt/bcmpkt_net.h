/*! \file bcmpkt_net.h
 *
 * Interfaces for transmitting and receiving packets for the host CPU.
 *
 * On transmitting, the user calls \ref bcmpkt_tx_f to send out a packet from a
 * network interface. The bcmpkt_txpmd_* interfaces could be used for defining
 * specific destination, e.g. an egress queue of a local port.
 *
 * On receiving, the user registers application's callback onto a netif.
 * When a packet received, the callback will be called by RX handler to receive
 * the packet. Packet structure has a encoded packet metadata for forwarding
 * information. The user calls bcmpkt_rxpmd_* interfaces to parsing forwarding
 * informaiton for the packet. Refer \ref bcmpkt_rxpmd.h for packet metadata
 * APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPKT_NET_H
#define BCMPKT_NET_H

#ifdef PKTIO_IMPL
#include <pktio_dep.h>
#else
#include <sal/sal_types.h>
#include <shr/shr_types.h>
#endif
#include <bcmpkt/bcmpkt_rxpmd.h>
#include <bcmpkt/bcmpkt_rxpmd_flex.h>
#include <bcmpkt/bcmpkt_gih.h>
#include <bcmpkt/bcmpkt_txpmd.h>
#include <bcmpkt/bcmpkt_lbhdr.h>
#include <bcmpkt/bcmpkt_higig3.h>
#include <bcmpkt/bcmpkt_higig_defs.h>
#include <bcmpkt/bcmpkt_socket.h>
#include <bcmpkt/bcmpkt_rcpu_hdr.h>
#include <bcmdrd/bcmdrd_types.h>

/*! Loopback Header maximum size. (words) */
#define BCMPKT_LBHDR_SIZE_WORDS      4

/*!
 * This size is for \ref bcmpkt_packet_t.pmd data.
 * Including RXPMD, TXPMD, Higig, and Loopback Header.
 * RXPMD use RX DCB size, and others use TX DCB Module header size.
 * Real size of them will be take care in API.
 */
#define BCMPKT_PMD_SIZE_BYTES       (BCMPKT_RCPU_RXPMD_SIZE + \
                                     BCMPKT_RCPU_TX_MH_SIZE * 3)

/*! The bcmpkt_packet_t.pmd.data size. (number of words) */
#define BCMPKT_PMD_SIZE_WORDS       (BCMPKT_PMD_SIZE_BYTES / 4)

/*!
 * This space is reserved for (o) optional components.
 *
 *  (o) RCPU header
 *  (o) Tx meta data (Tx PMD)
 *  (o) Loopback header
 *  (o) HiGig header *
 */
#define BCMPKT_TX_HDR_RSV           BCMPKT_RCPU_MAX_ENCAP_SIZE

/*! Packet data handle. */
#define BCMPKT_PACKET_DATA(_pkt)    (_pkt)->data_buf->data

/*! Packet data length. */
#define BCMPKT_PACKET_LEN(_pkt)     (_pkt)->data_buf->data_len

/*!
 * NET driver types.
 */
typedef enum bcmpkt_net_drv_types_e {
    BCMPKT_NET_DRV_T_NONE = BCMPKT_DEV_DRV_T_NONE, /*! No NET driver. */
    BCMPKT_NET_DRV_T_AUTO = BCMPKT_DEV_DRV_T_AUTO, /*! Reserved. */
    BCMPKT_NET_DRV_T_UNET = BCMPKT_DEV_DRV_T_UNET, /*! User land DMA driver. */
    BCMPKT_NET_DRV_T_TPKT = BCMPKT_SOCKET_DRV_T_TPKT,/*! Packet_mmap NET. */
    BCMPKT_NET_DRV_T_RAWSOCK = BCMPKT_SOCKET_DRV_T_RAWSOCK,/*! Raw socket NET. */
    BCMPKT_NET_DRV_T_COUNT /*! Must be end */
} bcmpkt_net_drv_types_t;

/*!
 * Packet forwarding types.
 * The \ref bcmpkt_packet_t.pmd defines normal packet forwarding and
 * encapsulation information. For BCMPKT_PACKET_T_NORMAL type, SDk will refer
 * \ref bcmpkt_packet_t.pmd for specific forwarding and Higig encapsulation.
 * If BCMPKT_FWD_T_RAW type is set, SDK will ignore \ref bcmpkt_packet_t.pmd.
 * In this case, the application user should take care of RCPU header and Higig
 * header because call \ref bcmpkt_net_t.tx to send out the packet.
 * The BCMPKT_FWD_T_RAW is normally used for debugging purpose.
 */
typedef enum bcmpkt_fwd_types_e {
    /*! Normal Packet. */
    BCMPKT_FWD_T_NORMAL,
    /*! Raw packet, may include RCPU header, TXPMD and/or Higig header. */
    BCMPKT_FWD_T_RAW,
     /*! Must be end */
    BCMPKT_FWD_T_COUNT
} bcmpkt_fwd_types_t;

/*!
 * Packet metadata information.
 */
typedef struct bcmpkt_pmd_s {

    /*! RX Packet metadata handle. */
    uint32_t *rxpmd;

    /*! TX Packet metadata handle. */
    uint32_t *txpmd;

    /*! Higig handle. */
    uint32_t *higig;

    /*! Loopback Header handle. */
    uint32_t *lbhdr;

    /*! Headers' data. */
    uint32_t data[BCMPKT_PMD_SIZE_WORDS];

} bcmpkt_pmd_t;

/*!
 * Packet data buffer information.
 *
 * The 'head' is the packet data buffer's pointer. The space between 'head' and
 * 'data' can be used for packet data content adjustment and/or RCPU header,
 * TXPMD and Higig encapsulation.
 *
 * The \c ref_count is used for clone function. It increases for each packet
 * clone function called, and decreases for each data buffer free called. When
 * \c ref_count = 0, free the buffer.
 */
typedef struct bcmpkt_data_buf_s {

    /*! Packet buffer head pointer. */
    uint8_t *head;

    /*! Packet buffer size (start from head). */
    uint32_t len;

    /*! Packet data pointer. */
    uint8_t *data;

    /*! Packet data size, unit is byte. */
    uint32_t data_len;

    /*! Number of packets using this data buffer. */
    int ref_count;
} bcmpkt_data_buf_t;

/*!
 * \brief Packet structure.
 *
 * \c unit is for per device DMA buffer management.
 * \c type is packet forwarding type.
 * \c pmd saves packet metadata information.
 * \c data saves data buffer information.
 */
typedef struct bcmpkt_packet_s {

    /*! Point to next packet in the list. */
    struct bcmpkt_packet_s *next;

    /*! Point to previous packet in the list. */
    struct bcmpkt_packet_s *prev;

    /*! Switch unit number. */
    int unit;

    /*! Internal used only. */
    uint32_t flags;

    /*! Packet forwarding type, refer to \ref bcmpkt_fwd_types_t. */
    uint32_t type;

    /*! Packet metadata information. */
    bcmpkt_pmd_t pmd;

    /*! Packet data buffer information. */
    bcmpkt_data_buf_t *data_buf;

} bcmpkt_packet_t;

/*!
 * \brief Packet receive callback function type.
 *
 * Callback function type for applications using RX facility.
 *
 * The user defines its callback function and register it onto a network
 * interface to receive its packets.
 *
 * The 'packet' buffer should not be freed in application software. If the
 * would like to use the packet out of its callback, the packet data should be
 * copied in callback function for application usage.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 * \param [in] packet Packet handle.
 * \param [in] cb_data Application-provided context.
 *
 * \return SHR_E_XXX Leave for future.
 */
typedef int (*bcmpkt_rx_cb_f)(int unit, int netif_id, bcmpkt_packet_t *packet,
                               void *cb_data);

/*!
 * \brief Packet receive callback register function.
 *
 * This function is for upper applications to regiter their callback functions
 * onto RX packet handler to receive its packets.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 * \param [in] flags Reserved for future.
 * \param [in] cb_func Packet receive callback function.
 * \param [in] cb_data Application-provided context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID The netif_id is invalid or doesn't support SOCKET.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_MEMORY Allocate buffer failed.
 * \retval SHR_E_EXISTS Callback already exists.
 */
typedef int (*bcmpkt_rx_register_f)(int unit, int netif_id, uint32_t flags,
                                    bcmpkt_rx_cb_f cb_func, void *cb_data);

/*!
 * \brief Packet receive callback deregister function.
 *
 * Remove callback function from the network interface to disable receive
 * packets from it.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 * \param [in] cb_func Packet receive callback function.
 * \param [in] cb_data Application-provided context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Netif_id is invalid or doesn't support SOCKET.
 * \retval SHR_E_PARAM Check parameters failed.
 */
typedef int (*bcmpkt_rx_unregister_f)(int unit, int netif_id,
                                      bcmpkt_rx_cb_f cb_func, void *cb_data);

/*!
 * \brief Packet transmit function.
 *
 * This function is for application to send out a packet through Broadcom Packet
 * API.
 *
 * If a packet is to be sent to a specific local port and/or go through Higig
 * IPIPE, the metadata and/or Higig header should be configured into
 * \ref bcmpkt_packet_t.pmd (\ref BCMPKT_FWD_T_NORMAL type) or encapsulated
 * into RCPU/Higig header (\ref BCMPKT_FWD_T_RAW type).
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface number.
 * \param [in] packet Packet handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Netif_id is invalid or doesn't support SOCKET.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_FAIL Transmit failed.
 */
typedef int (*bcmpkt_tx_f)(int unit, int netif_id, bcmpkt_packet_t *packet);

/*!
 * \brief NET operation vector.
 */
typedef struct bcmpkt_net_s {

    /*! initialized flag: 0 - uninitialized 1 - initialized. */
    int initialized;

    /*! Driver name, such as "TPacket". */
    char driver_name[128];

    /*! SOCKET driver type. */
    bcmpkt_net_drv_types_t driver_type;

    /*! Register RX callback. */
    bcmpkt_rx_register_f rx_register;

    /*! Unregister RX callback. */
    bcmpkt_rx_unregister_f rx_unregister;

    /*! Transmit function. */
    bcmpkt_tx_f tx;

} bcmpkt_net_t;

/*!
 * \brief NET driver register function.
 *
 *
 * \param [in] net_drv Network driver handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_CONFIG Invalid driver.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcmpkt_net_drv_register(bcmpkt_net_t *net_drv);

/*!
 * \brief Unregister NET driver.
 *
 * \param [in] type NET driver type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_BUSY The driver is in using.
 */
extern int
bcmpkt_net_drv_unregister(bcmpkt_net_drv_types_t type);

/*!
 * \brief Get NET driver's type.
 *
 * \param [in] unit Switch unit number.
 * \param [out] type NET driver type.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 */
extern int
bcmpkt_net_drv_type_get(int unit, bcmpkt_net_drv_types_t *type);

/*!
 * \brief Direct forwarding port set function.
 *
 * Configure a destination port for bypass CPU port IPIPE and forwarding the
 * packet to the specific port directly. This function will set TXPMD (SOBMH)
 * for the forwarding.
 *
 * \param [in] dev_type Switch device type.
 * \param [in] port Packet destination port.
 * \param [in,out] packet Packet handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_INTERNAL Internal failure.
 */
extern int
bcmpkt_fwd_port_set(bcmdrd_dev_type_t dev_type, int port,
                    bcmpkt_packet_t *packet);

/*!
 * \brief Packet forwarding type set function.
 *
 * Packet's default type is NORMAL. If the user want to change the type, this
 * API is used for the purpose.
 *
 * \param [in] type Packet forward type.
 * \param [in,out] packet Packet handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcmpkt_fwd_type_set(bcmpkt_fwd_types_t type, bcmpkt_packet_t *packet);

/*!
 * \brief Packet receive callback register function.
 *
 * This function is for upper applications to regiter their callback functions
 * onto RX packet handler to receive its packets.
 *
 * The 'netif_id' is not used for UNET driver and would be ignored.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 * \param [in] flags Reserved for future.
 * \param [in] cb_func Packet receive callback function.
 * \param [in] cb_data Application-provided context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID The netif_id is invalid or doesn't support SOCKET.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_MEMORY Allocate buffer failed.
 * \retval SHR_E_EXISTS Callback already exists.
 */
extern int
bcmpkt_rx_register(int unit, int netif_id, uint32_t flags,
                   bcmpkt_rx_cb_f cb_func, void *cb_data);

/*!
 * \brief Packet receive callback deregister function.
 *
 * Remove callback function from the network interface to disable receive
 * packets from it.
 *
 * The 'netif_id' is not used for UNET driver and would be ignored.
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface ID.
 * \param [in] cb_func Packet receive callback function.
 * \param [in] cb_data Application-provided context.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Netif_id is invalid or doesn't support SOCKET.
 * \retval SHR_E_PARAM Check parameters failed.
 */
extern int
bcmpkt_rx_unregister(int unit, int netif_id, bcmpkt_rx_cb_f cb_func,
                     void *cb_data);

/*!
 * \brief Packet transmit function.
 *
 * This function is for application to send out a packet through Broadcom Packet
 * API.
 *
 * If a packet is to be sent to a specific local port and/or go through Higig
 * IPIPE, the metadata and/or Higig header should be configured into
 * \ref bcmpkt_packet_t.pmd (\ref BCMPKT_FWD_T_NORMAL type) or encapsulated
 * into RCPU/Higig header (\ref BCMPKT_FWD_T_RAW type).
 *
 * For UNET driver mode, 'netif_id' is TX DMA queue index (starts from 1).
 *
 * \param [in] unit Switch unit number.
 * \param [in] netif_id Network interface number.
 * \param [in] packet Packet handle.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 * \retval SHR_E_BADID Netif_id is invalid or doesn't support SOCKET.
 * \retval SHR_E_PARAM Check parameters failed.
 * \retval SHR_E_FAIL Transmit failed.
 */
extern int
bcmpkt_tx(int unit, int netif_id, bcmpkt_packet_t *packet);

#endif /* BCMPKT_NET_H */
