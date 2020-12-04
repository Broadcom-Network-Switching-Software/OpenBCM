/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        t_util.h
 * Purpose:     Useful transport macros
 */

#ifndef   _T_UTIL_H_
#define   _T_UTIL_H_


#define PACK_SHORT(buf, val) \
    do {                                               \
        uint16 v2;                                     \
        v2 = bcm_htons(val);                           \
        sal_memcpy(buf, &v2, sizeof(uint16));           \
    } while (0)

#define PACK_LONG(buf, val) \
    do {                                               \
        uint32 v2;                                     \
        v2 = bcm_htonl(val);                           \
        sal_memcpy(buf, &v2, sizeof(uint32));           \
    } while (0)

#define UNPACK_SHORT(buf, val) \
    do {                                               \
        sal_memcpy(&(val), buf, sizeof(uint16));         \
        val = bcm_ntohs(val);                          \
    } while (0)

#define UNPACK_LONG(buf, val) \
    do {                                               \
        sal_memcpy(&(val), buf, sizeof(uint32));         \
        val = bcm_ntohl(val);                          \
    } while (0)


#define	CT_INCR_PACK_U8(_buf, _var) \
		*_buf++ = (_var)
#define	CT_INCR_UNPACK_U8(_buf, _var) \
		_var = *_buf++

#define	CT_INCR_PACK_U16(_buf, _var) \
		*_buf++ = ((_var) >> 8) & 0xff; \
		*_buf++ = (_var) & 0xff;
#define	CT_INCR_UNPACK_U16(_buf, _var) \
		_var  = *_buf++ << 8; \
		_var |= *_buf++; 

#define	CT_INCR_PACK_U32(_buf, _var) \
		*_buf++ = ((_var) >> 24) & 0xff; \
		*_buf++ = ((_var) >> 16) & 0xff; \
		*_buf++ = ((_var) >> 8) & 0xff; \
		*_buf++ = (_var) & 0xff;
#define	CT_INCR_UNPACK_U32(_buf, _var) \
		_var  = *_buf++ << 24; \
		_var |= *_buf++ << 16; \
		_var |= *_buf++ << 8; \
		_var |= *_buf++; 

#endif /* _T_UTIL_H_ */
