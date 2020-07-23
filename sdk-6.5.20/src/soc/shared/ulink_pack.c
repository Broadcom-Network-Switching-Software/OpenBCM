/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ulink_pack.c
 * Purpose:     ULINK pack and unpack routines for:
 *              - ukernel link module 
 *
 *
 * ULINK control messages
 *
 * ULINK messages between the Host CPU and uController are sent
 * using the uc_message module which allows short messages
 * to be passed (see include/soc/shared/mos_msg_common.h)
 *
 * Additional information for a given message (a long message) is passed
 * using DMA.  The ULINK control message types defines the format
 * for these long messages.
 *
 * This file is shared between SDK and uKernel.
 */

#include <soc/defs.h>
#include <shared/pack.h>
#include <soc/shared/ulink_pack.h>

/*********************************************************
 * ULINK port bitmap Pack/Unpack
 *
 * Functions:
 *      shr_ulink_pbm_pack/unpack
 * Purpose:
 *      The following set of routines pack/unpack link  
 *      bitmap to/from given buffer.
 * Parameters:
 *   _pack()
 *      buffer   - (OUT) Buffer where to pack bitmap.
 *      <header> - (IN)  bitmap to pack.
 *   _unpack()
 *      buffer   - (IN)  Buffer with data to unpack.
 *      <header> - (OUT) Returns unpack header.
 * Returns:
 *      Pointer to next position in buffer.
 * Notes:
 *      Assumes pointers are valid and contain enough memory space.
 */
uint8 *
shr_ulink_pbm_unpack(uint8 *bufp, soc_ulink_pbm_msg_t *msg)
{
    int i, word_max;

    _SHR_UNPACK_U8(bufp, msg->flags);
    _SHR_UNPACK_U8(bufp, msg->words);
    word_max = (msg->words <= UL_PBMP_WORD_MAX) ? msg->words : UL_PBMP_WORD_MAX;
    
    for(i = 0; i < word_max; i++) {
        _SHR_UNPACK_U32(bufp,msg->pbits[i]);
    }
    return bufp;
}

uint8 *
shr_ulink_pbm_pack(uint8 *bufp, soc_ulink_pbm_msg_t *msg)
{
    int i;
    _SHR_PACK_U8(bufp, msg->flags);
    _SHR_PACK_U8(bufp, msg->words);
    for(i = 0; i < msg->words; i++) {
        _SHR_PACK_U32(bufp,msg->pbits[i]);
    }
    return bufp;
}

