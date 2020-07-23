/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File:    trie_util.c
 * Purpose: Trident2 Trie utility functions
 * Requires:
 */

#include <soc/types.h>
#include <soc/drv.h>
#include <shared/bsl.h>
#ifdef ALPM_ENABLE
#ifdef BCM_TRIDENT2_SUPPORT
#ifndef ALPM_IPV6_128_SUPPORT

#include <soc/esw/trie.h>
#include <soc/esw/trie_util.h>

static int _taps_util_debug = FALSE;
/*
 *
 * Function:
 *     taps_show_prefix
 * Input:
 *     key   -- uint32 array head. Only "length" number of bits
 *              is passed in.
 *              for ipv4. Key[0].bit15-0 is key bits 47-32
 *                        Key[1] is key bits 31-0
 *              for ipv6. Key[0].bit15-0 is key bits 143-128
 *                        Key[1-4] is key bits 127-0
 *     length-- number of valid bits in key array. This would be
 *              valid MSB bits of the route. For example, 
 *              (vrf=0x1234, ip=0xf0000000, length=20) would store
 *              as key[0] = 0, key[1]=0x1234F, length=20.
 * Purpose:
 *     print a prefix if "dbm soc verbose"
 */
int taps_show_prefix(uint32 max_key_size, uint32 *key, uint32 length)
{
    int word, max_words, key_words;

    if ((key == NULL) ||
	(length > max_key_size) ||
	((max_key_size != TAPS_IPV4_KEY_SIZE) &&
	 (max_key_size != TAPS_IPV6_KEY_SIZE))) {
	return SOC_E_PARAM;
    }
    
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("prefix length %d key 0x"), length));

    key_words = (length+31)/32;
    max_words = (max_key_size+31)/32;
    for (word = 0; word < max_words; word++) {
	if (word == (max_words-key_words)) {
	    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META("%x"), key[word]));
	} else if (word > (max_words-key_words)) {
	    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META("%08x"), key[word]));
	}
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("\n")));

    return SOC_E_NONE;
}

/*
 *
 * Function:
 *     taps_key_shift 
 * Input:
 *     max_key_size  -- max number of bits in the key
 *                      ipv4 == 48
 *                      ipv4 == 144
 *     key   -- uint32 array head. Only "length" number of bits
 *              is passed in.
 *              for ipv4. Key[0].bit15-0 is key bits 47-32
 *                        Key[1] is key bits 31-0
 *              for ipv6. Key[0].bit15-0 is key bits 143-128
 *                        Key[1-4] is key bits 127-0
 *     length-- number of valid bits in key array. This would be
 *              valid MSB bits of the route. For example, 
 *              (vrf=0x1234, ip=0xf0000000, length=20) would store
 *              as key[0] = 0, key[1]=0x1234F, length=20.
 *     shift -- positive means right shift, negative means left shift
 *              routine will check if the shifted key is out of 
 *              max_key_size boundary.
 */
int taps_key_shift(uint32 max_key_size, uint32 *key, uint32 length, int32 shift)
{
    int word_idx, lsb;

    if ((key == NULL) ||
	(length > max_key_size) ||
	((max_key_size != TAPS_IPV4_KEY_SIZE) &&
	 (max_key_size != TAPS_IPV6_KEY_SIZE))) {
	return SOC_E_PARAM;
    }

    if (((length - shift) > max_key_size) ||
	((shift > 0) && (shift > length))) {
	/* left shift resulted in key longer than max_key_size or
	 * right shift resulted in key shorter than 0
	 */
	return SOC_E_PARAM;
    }

    if (_taps_util_debug) {
	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("Original key before shift:\n")));
	taps_show_prefix(max_key_size, key, length);
    }

    if (shift > 0) {
	/* right shift */
	for (lsb = shift, word_idx=BITS2WORDS(max_key_size)-1;
	     word_idx >=0;
	     lsb+=32, word_idx--) {
	    if (lsb < length) {
		key[word_idx] = _TAPS_GET_KEY_BITS(key, lsb, ((length-lsb)>=32)?32:(length-lsb),
						   max_key_size);
	    } else {
		key[word_idx] = 0;
	    }
	}
    } else if (shift < 0) {
	/* left shift */
	shift = 0 - shift;

	/* whole words shifting first */
	for (word_idx = 0;
	     ((shift/32)!=0) && (word_idx < BITS2WORDS(max_key_size));
	     word_idx++) {
	    if ((word_idx + (shift/32)) >= BITS2WORDS(max_key_size)) {
		key[word_idx]=0;
	    } else {
		key[word_idx] = key[word_idx + (shift/32)];
	    }
	}

	/* shifting remaining bits */
	for (word_idx = 0;
	     ((shift%32)!=0) && (word_idx < BITS2WORDS(max_key_size));
	     word_idx++) {
	    if (word_idx == TP_BITS2IDX(0, max_key_size)) {
		/* at bit 0 word, next word doesn't exist */
		key[word_idx] = TP_SHL(key[word_idx], (shift%32));
	    } else {
		key[word_idx] = TP_SHL(key[word_idx], (shift%32)) | \
		    TP_SHR(key[word_idx+1], 32-(shift%32));
	    }
	}
	
	/* mask off bits higher than max_key_size */
	key[0] &= TP_MASK(max_key_size%32);
    }

    if (_taps_util_debug) {
	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("Resulted key after shift:\n")));
	taps_show_prefix(max_key_size, key, length-shift);
    }

    return SOC_E_NONE;
}

/*
 *
 * Function:
 *     taps_key_match
 * Input:
 *     max_key_size  -- max number of bits in the key
 *                      ipv4 == 48
 *                      ipv4 == 144
 *     key1  -- uint32 array head for first key. Only "length1" number of bits
 *              is passed in.
 *              for ipv4. Key[0].bit15-0 is key bits 47-32
 *                        Key[1] is key bits 31-0
 *              for ipv6. Key[0].bit15-0 is key bits 143-128
 *                        Key[1-4] is key bits 127-0
 *     length1-- number of valid bits in key1 array. This would be
 *              valid MSB bits of the route. For example, 
 *              (vrf=0x1234, ip=0xf0000000, length=20) would store
 *              as key[0] = 0, key[1]=0x1234F, length=20.
 *     key2  -- uint32 array head for second key. Only "length2" number of bits
 *              is passed in.
 *              for ipv4. Key[0].bit15-0 is key bits 47-32
 *                        Key[1] is key bits 31-0
 *              for ipv6. Key[0].bit15-0 is key bits 143-128
 *                        Key[1-4] is key bits 127-0
 *     length2-- number of valid bits in key2 array. This would be
 *              valid MSB bits of the route. For example, 
 *              (vrf=0x1234, ip=0xf0000000, length=20) would store
 *              as key[0] = 0, key[1]=0x1234F, length=20.
 * Purpose:
 *     Return TRUE if the all the valid bits of the shorter key matches
 *     the corresponding bits of the longer key. Otherwise return FALSE.
 *
 *     This function does NOT modify either keys.
 *
 *     max_key_length supported is 256 bits.     
 */
int taps_key_match(uint32 max_key_size, uint32 *key1, uint32 length1,
		   uint32 *key2, uint32 length2)
{
#define _TAPS_UTIL_MAX_MATCH_KEY_SIZE (256)

    uint32 tmp_key1[BITS2WORDS(_TAPS_UTIL_MAX_MATCH_KEY_SIZE)];
    uint32 tmp_key2[BITS2WORDS(_TAPS_UTIL_MAX_MATCH_KEY_SIZE)];
    int rv = SOC_E_NONE;
    int index;

    if (_taps_util_debug) {
	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("key1:\n")));
	taps_show_prefix(max_key_size, key1, length1);

	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("key2:\n")));
	taps_show_prefix(max_key_size, key2, length2);
    }

    /* copy key bits */
    for (index=0; index<BITS2WORDS(max_key_size); index++) {
	tmp_key1[index] = key1[index];
	tmp_key2[index] = key2[index];
    }

    /* shift out the LSBs of the longer key */
    if (length1 > length2) {
	rv = taps_key_shift(max_key_size, tmp_key1, length1, (length1-length2));
    } else if (length2 > length1) {
	rv = taps_key_shift(max_key_size, tmp_key2, length2, (length2-length1));
    }
    if (rv != SOC_E_NONE) {
      LOG_VERBOSE(BSL_LS_SOC_COMMON,
                  (BSL_META("taps key shift failed 0x%x\n"),rv));
    }

    for (index=0; index<BITS2WORDS(max_key_size); index++) {
	if (tmp_key1[index] != tmp_key2[index]) {
	    /* some bits not matching */
	    if (_taps_util_debug) {
		LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META("Key1 and key2 not matching:\n")));
	    }
	    return FALSE;
	}
    }
    
    /* all matched */
    if (_taps_util_debug) {
	LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META("Key1 and key2 matched:\n")));
    }
    return TRUE;
}

/*
 *
 * Function:
 *    taps_get_lsb
 * Input:
 *     max_mask_size  -- number of bits in the mask
 *                      ipv4 == 48
 *                      ipv6 == 144
 *     mask  -- uint32 array head.
 *              for ipv4. Mask[0].bit15-0 is mask bits 47-32
 *                        Mask[1] is mask bits 31-0
 *              for ipv6. Mask[0].bit15-0 is mask bits 143-128
 *                        Mask[1-4] is mask bits 127-0
 *     lsb   -- -1 if no bit is set. bit position of the least
 *              significant bit that is set to 1
 * Purpose:
 *     get the bit position of the least significant bit that is set in mask.
 *     for example:
 *         for ipv4, max_mask_size == 48
 *         mask[0]=0, mask[1]=0x30 will return lsb=4
 *         mask[0]=3, mask[1]=0 will return lsb=32
 */
int taps_get_lsb(uint32 max_mask_size, uint32 *mask, int32 *lsb)
{
    int word_idx, bit_idx;

    if (!mask || !lsb) {
	return SOC_E_PARAM;
    }

    *lsb = -1;
    for (word_idx = (BITS2WORDS(max_mask_size)-1); word_idx >=0; word_idx--) {
	if (mask[word_idx]!=0) {
	    for (bit_idx=0; bit_idx<32; bit_idx++) {
		if ((mask[word_idx] & (1<<bit_idx)) &&
		    (((BITS2WORDS(max_mask_size) - 1 - word_idx) * 32 + bit_idx) < max_mask_size)) {
		    *lsb = (BITS2WORDS(max_mask_size) - 1 - word_idx) * 32 + bit_idx;
		    return SOC_E_NONE;
		}
	    }
	}
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *     taps_get_bpm_pfx
 * Purpose:
 *     finds best prefix match length given a bpm bitmap & key
 */
int taps_get_bpm_pfx(unsigned int *bpm, 
		     unsigned int key_len,
		     unsigned int max_key_len,
		     /* OUT */
		     unsigned int *pfx_len)
{
    int rv = SOC_E_NONE, pos=0;

    if (!bpm || !pfx_len || (key_len > max_key_len)) {
        return SOC_E_PARAM;
    }

    *pfx_len = 0;

    rv = taps_get_lsb(max_key_len, bpm, &pos);
    if (SOC_SUCCESS(rv)) {
        *pfx_len = (pos < 0)?0:(key_len - pos);
    }
    return rv;
}
#endif /* ALPM_IPV6_128_SUPPORT */
#endif /* BCM_TRIDENT2_SUPPORT */
#endif /* ALPM_ENABLE */
