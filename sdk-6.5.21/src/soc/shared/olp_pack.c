/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        olp_pack.c
 * Purpose:     OLP pack and unpack routines for:
 *              - OLP tx/rx header 
 *
 *
 * OLP control messages
 *
 * OLP messages between the Host CPU and uController are sent
 * using the uc_message module which allows short messages
 * to be passed (see include/soc/shared/mos_msg_common.h)
 *
 * Additional information for a given message (a long message) is passed
 * using DMA.  The OLP control message types defines the format
 * for these long messages.
 *
 * This file is shared between SDK and uKernel.
 */

#include <soc/defs.h>
#include <shared/pack.h>
#include <soc/shared/olp_pkt.h>
#include <soc/shared/olp_pack.h>

/*********************************************************
 * OLP Tx/Rx Network Packet Header Pack/Unpack
 *
 * Functions:
 *      shr_olp_tx/rx_<header>_pack/unpack
 * Purpose:
 *      The following set of routines pack/unpack specified
 *      network packet header to/from given buffer.
 * Parameters:
 *   _pack()
 *      buffer   - (OUT) Buffer where to pack header.
 *      <header> - (IN)  Header to pack.
 *   _unpack()
 *      buffer   - (IN)  Buffer with data to unpack.
 *      <header> - (OUT) Returns unpack header.
 * Returns:
 *      Pointer to next position in buffer.
 * Notes:
 *      Assumes pointers are valid and contain enough memory space.
 */
uint8 *
shr_olp_tx_header_unpack(uint8 *bufp, soc_olp_tx_hdr_t *opt)
{

    _SHR_UNPACK_U32(bufp,opt->u1.h1);
    _SHR_UNPACK_U32(bufp,opt->u2.h2);
    _SHR_UNPACK_U32(bufp,opt->u3.h3);
    _SHR_UNPACK_U32(bufp,opt->u4.h4);
    return bufp;
}

uint8 *
shr_olp_tx_header_pack(uint8 *bufp, soc_olp_tx_hdr_t *opt)
{

    _SHR_PACK_U32(bufp,opt->u1.h1);
    _SHR_PACK_U32(bufp,opt->u2.h2);
    _SHR_PACK_U32(bufp,opt->u3.h3);
    _SHR_PACK_U32(bufp,opt->u4.h4);
    return bufp;
}

uint8 *
shr_olp_rx_header_unpack(uint8 *bufp, soc_olp_rx_hdr_t *opr)
{
    _SHR_UNPACK_U32(bufp,opr->u1.h1);
    _SHR_UNPACK_U32(bufp,opr->u2.h2);
    _SHR_UNPACK_U32(bufp,opr->u3.h3);
    _SHR_UNPACK_U32(bufp,opr->u4.h4);
    _SHR_UNPACK_U32(bufp,opr->u5.h5);
    return bufp;
}

uint8 *
shr_olp_rx_header_pack(uint8 *bufp, soc_olp_rx_hdr_t *opr)
{
    _SHR_PACK_U32(bufp,opr->u1.h1);
    _SHR_PACK_U32(bufp,opr->u2.h2);
    _SHR_PACK_U32(bufp,opr->u3.h3);
    _SHR_PACK_U32(bufp,opr->u4.h4);
    _SHR_PACK_U32(bufp,opr->u5.h5);
    return bufp;
}
