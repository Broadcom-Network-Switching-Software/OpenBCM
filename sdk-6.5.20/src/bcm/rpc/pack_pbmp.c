/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:	pack_pbmp.c
 * Purpose:	BCM API Type Packers - port bitmaps
 */

#include <bcm/types.h>
#include <shared/pbmp.h>
#include <bcm_int/rpc/pack.h>

/*
 * Two different mechanisms for packing bcm_pbmp_t structures are provided.
 *
 * If BCM_RPC_PBMP_64 is not defined then rpc version 3 is in use and port
 * bitmaps are packed as:
 *	<uint8 nword><uint32 word[0]>...<uint32 word[nword-1]>
 * This packing method handles bcm implementations compiled with differing
 * lengths of port bitmaps and is potentially more efficient in packing size
 * (zero value trailing words are not packed)
 *
 * If BCM_RPC_PBMP_64 is defined, then rpc version 2 is in use and
 * port bitmaps are packed as:
 *	<uint32 word[0]><uint32 word[1]>
 * The second packing method is limited to 64 bit port bitmaps but is
 * backwards compatible with sdk-5.3 and sdk-5.4 releases.
 */

#ifndef	BCM_RPC_PBMP_64

uint8 *
_bcm_pack_pbmp(uint8 *buf, bcm_pbmp_t *var)
{
    uint8	nwords;
    int		i;

    /* back up over zero values words */
    for (i = _SHR_PBMP_WORD_MAX-1; i >= 0; i--) {
	if (_SHR_PBMP_WORD_GET(*var, i) != 0) {
	    break;
	}
    }
    nwords = i+1;
    BCM_PACK_U8(buf, nwords);
    for (i = 0; i < nwords; i++) {
	BCM_PACK_U32(buf, _SHR_PBMP_WORD_GET(*var, i));
    }
    return buf;
}

uint8 *
_bcm_unpack_pbmp(uint8 *buf, bcm_pbmp_t *var)
{
    uint8	nwords;
    uint32	pword;
    int		i;

    BCM_UNPACK_U8(buf, nwords);
    for (i = 0; i < nwords; i++) {
	BCM_UNPACK_U32(buf, pword);
	if (i < _SHR_PBMP_WORD_MAX) {
	    _SHR_PBMP_WORD_SET(*var, i, pword);
	}
    }
    /* clear any remaining words */
    for (i = nwords; i < _SHR_PBMP_WORD_MAX; i++) {
	_SHR_PBMP_WORD_SET(*var, i, 0);
    }
    return buf;
}

#else	/* BCM_RPC_PBMP_64 */

uint8 *
_bcm_pack_pbmp(uint8 *buf, bcm_pbmp_t *var)
{
    BCM_PACK_U32(buf, var->pbits[0]);
    BCM_PACK_U32(buf, var->pbits[1]);
    return buf;
}

uint8 *
_bcm_unpack_pbmp(uint8 *buf, bcm_pbmp_t *var)
{
    BCM_UNPACK_U32(buf, var->pbits[0]);
    BCM_UNPACK_U32(buf, var->pbits[1]);
    return buf;
}

#endif	/* BCM_RPC_PBMP_64 */
