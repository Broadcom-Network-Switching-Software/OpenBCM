/*! \file bcmpkt_rcpu_hdr.h
 *
 * RCPU header format definition.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPKT_RCPU_HDR_H
#define BCMPKT_RCPU_HDR_H

#ifdef PKTIO_IMPL
#include <pktio_dep.h>
#else
#include <sal/sal_types.h>
#endif

/*! RCPU Header length. */
#define BCMPKT_RCPU_HDR_LEN         32
/*! RX PMD maximum size. */
#define BCMPKT_RCPU_RXPMD_SIZE      96
/*! RX Encapsulation size. */
#define BCMPKT_RCPU_RX_ENCAP_SIZE   (BCMPKT_RCPU_HDR_LEN + BCMPKT_RCPU_RXPMD_SIZE)
/*! TX Module Header size. */
#define BCMPKT_RCPU_TX_MH_SIZE      32
/*! TX Encapsulation size. */
#define BCMPKT_RCPU_TX_ENCAP_SIZE   (BCMPKT_RCPU_HDR_LEN + BCMPKT_RCPU_TX_MH_SIZE)
/*! Maximum Encapsulation size. */
#define BCMPKT_RCPU_MAX_ENCAP_SIZE  BCMPKT_RCPU_RX_ENCAP_SIZE

/*!
 * \name Packet RCPU operation types.
 * \anchor BCMPKT_RCPU_OP_XXX
 */
/*! \{ */
/*! No operation code.   */
#define BCMPKT_RCPU_OP_NONE         0x0
/*! To CPU packet.   */
#define BCMPKT_RCPU_OP_RX           0x10
/*! From CPU packet.   */
#define BCMPKT_RCPU_OP_TX           0x20
/*! \} */

/*!
 * \name Packet RCPU flags.
 * \anchor BCMPKT_RCPU_F_XXX
 */
/*! \{ */
/*! No operation code.   */
#define BCMPKT_RCPU_F_NONE          0x0
/*! To CPU packet.   */
#define BCMPKT_RCPU_F_MODHDR        0x04
/*! \} */

/*! RCPU default VLAN ID with pri and cfi.   */
#define BCMPKT_RCPU_VLAN            0x01

/*! RCPU TPID.   */
#define BCMPKT_RCPU_TPID            0x8100

/*! RCPU Ethertype.   */
#define BCMPKT_RCPU_ETYPE           0xde08

/*!
 * \brief The RCPU header format structure.
 */
typedef struct bcmpkt_rcpu_hdr_s {

    /*! RCPU header DMAC. */
    shr_mac_t dmac;

    /*! RCPU header SMAC. */
    shr_mac_t smac;

    /*! VLAN TPID. */
    uint16_t tpid;

    /*! VLAN TAG with cfi + pri. */
    uint16_t vlan;

    /*! Ether-type. */
    uint16_t ethertype;

    /*! RCPU signature. */
    uint16_t signature;

    /*! RCPU operation code. */
    uint8_t opcode;

    /*! RCPU operation code. */
    uint8_t flags;

    /*! RCPU operation code. */
    uint16_t transid;

    /*! Length of packet data. */
    uint16_t pkt_len;

    /*! Expect reply message length. */
    uint16_t reply_len;

    /*! packet meta data length. (Internal usage) */
    uint8_t meta_len;

    /*! Transmission queue number. (Internal usage) */
    uint8_t queue_id;

    /*! Reserved must be 0 */
    uint16_t reserved;

} bcmpkt_rcpu_hdr_t;

#endif /* BCMPKT_RCPU_HDR_H */
