/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PORTMOD_DEFS_H__H_
#define _PORTMOD_DEFS_H__H_

#include <soc/debug.h>
#include <soc/portmode.h>
#include <soc/port_ability.h>
#include <soc/timesync.h>
#include <soc/mcm/allenum.h>

#ifdef PHYMOD_SUPPORT 
#include <phymod/phymod.h>
#endif /* PHYMOD_SUPPORT */


#define MAX_LANES_PER_PORT (24)
#define MAX_PHYN (3)
/* ILKN PM doesn't manage phymod objects directly, 
    instead it take over other PMs and control them using portmod APIs.
    Each ILKN PM controls up to 6 PM*/
#define PORTMOD_MAX_ILKN_AGGREGATED_PMS (6)

#define MAX_NUM_CORES PORTMOD_MAX_ILKN_AGGREGATED_PMS

typedef int ilkn_retransmit_config_t;
typedef int phymod_firmware_mode_t;
typedef soc_port_ability_t portmod_port_ability_t;
typedef soc_port_if_t portmod_port_if_t;
typedef soc_port_mode_t portmod_port_mode_t;

typedef  unsigned int portmod_pa_encap_t;


typedef struct portmod_lanes_assign_info_s{
    uint32 valid_entries;
    uint32 lane_assign[MAX_LANES_PER_PORT];
}portmod_lanes_assign_info_t;


#if (defined BCM_ESW_SUPPORT) && (512 > _SHR_PBMP_PORT_MAX)
#define PORTMOD_PBMP_PORT_MAX	512
#else
#define PORTMOD_PBMP_PORT_MAX      _SHR_PBMP_PORT_MAX
#endif

#define PORTMOD_PBMP_WORD_MAX        \
    ((PORTMOD_PBMP_PORT_MAX + _SHR_PBMP_WORD_WIDTH-1) / _SHR_PBMP_WORD_WIDTH)

typedef struct portmod_pbmp {
    uint32 pbits[PORTMOD_PBMP_WORD_MAX];
} portmod_pbmp_t;

extern int portmod_pbmp_bmnull(portmod_pbmp_t *bmp);
extern int portmod_pbmp_bmeq(portmod_pbmp_t *bmp1, portmod_pbmp_t *bmp2);

#define PORTMOD_PBMP_WORDS(bm) \
    (sizeof((bm).pbits) / sizeof((bm).pbits[0]))

#define PORTMOD_PBMP_COUNT(bm, count) \
    do { \
        int    _w; \
        count = 0; \
        for (_w = 0; _w < PORTMOD_PBMP_WORD_MAX; _w++) { \
            count += _shr_popcount(_SHR_PBMP_WORD_GET(bm, _w)); \
        } \
    } while(0)

#define PORTMOD_PBMP_BMOP(bma, bmb, op) \
    do { \
            int _w, _l; \
            _l = ((PORTMOD_PBMP_WORDS(bma) < PORTMOD_PBMP_WORDS(bmb)) ? \
                   PORTMOD_PBMP_WORDS(bma) : PORTMOD_PBMP_WORDS(bmb)); \
            for (_w = 0; _w < _l; _w++) { \
                _SHR_PBMP_WORD_GET(bma, _w) op _SHR_PBMP_WORD_GET(bmb, _w); \
            } \
        } while (0)

#define PORTMOD_PBMP_CLEAR(bm) \
    do { \
        int _w; \
        for (_w = 0; _w < PORTMOD_PBMP_WORD_MAX; _w++) { \
            _SHR_PBMP_WORD_GET(bm, _w) = 0; \
        } \
    } while (0)



#define PORTMOD_PBMP_ITER(bm, port)    \
            for ((port) = 0; (port) < PORTMOD_PBMP_PORT_MAX; (port)++) \
                if (PORTMOD_PBMP_MEMBER((bm), (port)))

#define PORTMOD_PBMP_IS_NULL(bm)        (portmod_pbmp_bmnull(&bm))
#define PORTMOD_PBMP_NOT_NULL(bm)       (!portmod_pbmp_bmnull(&bm))
#define PORTMOD_PBMP_EQ(bma, bmb)       (portmod_pbmp_bmeq(&bma, &bmb))
#define PORTMOD_PBMP_NEQ(bma, bmb)      (!portmod_pbmp_bmeq(&bma, &bmb))
/* Assignment operators */
#define PORTMOD_PBMP_ASSIGN(dst, src)   (dst) = (src)
#define PORTMOD_PBMP_AND(bma, bmb)      PORTMOD_PBMP_BMOP(bma, bmb, &=)
#define PORTMOD_PBMP_OR(bma, bmb)       PORTMOD_PBMP_BMOP(bma, bmb, |=)
#define PORTMOD_PBMP_XOR(bma, bmb)      PORTMOD_PBMP_BMOP(bma, bmb, ^=)
#define PORTMOD_PBMP_REMOVE(bma, bmb)   PORTMOD_PBMP_BMOP(bma, bmb, &= ~)
#define PORTMOD_PBMP_NEGATE(bma, bmb)   PORTMOD_PBMP_BMOP(bma, bmb, = ~)
/* Port PBMP operators */
#define PORTMOD_PBMP_MEMBER(bm, port)    \
    ((_SHR_PBMP_ENTRY(bm, port) & _SHR_PBMP_WBIT(port)) != 0)
#define PORTMOD_PBMP_PORT_SET(bm, port)    do { \
        PORTMOD_PBMP_CLEAR(bm); \
        PORTMOD_PBMP_PORT_ADD(bm, port); \
    } while(0)
#define PORTMOD_PBMP_PORT_ADD(bm, port)    \
    (_SHR_PBMP_ENTRY(bm, port) |= _SHR_PBMP_WBIT(port))
#define PORTMOD_PBMP_PORT_REMOVE(bm, port)    \
    (_SHR_PBMP_ENTRY(bm, port) &= ~_SHR_PBMP_WBIT(port))
#define PORTMOD_PBMP_PORT_FLIP(bm, port)    \
    (_SHR_PBMP_ENTRY(bm, port) ^= _SHR_PBMP_WBIT(port))
#define PORTMOD_PBMP_PORTS_RANGE_ADD(bm, first_port, range) \
    do {\
        uint32 _mask_;\
        int _first_port_, _range_;\
        _first_port_ = first_port; _range_ = range;\
        while (_range_ > 0) {\
            _mask_ = ~0;\
            if (_range_ < _SHR_PBMP_WORD_WIDTH) _mask_ >>= (_SHR_PBMP_WORD_WIDTH - _range_);\
            _mask_ <<= (_first_port_ % _SHR_PBMP_WORD_WIDTH);\
            _SHR_PBMP_ENTRY(bm, _first_port_) |= _mask_; \
            _range_ += (_first_port_ % _SHR_PBMP_WORD_WIDTH) - _SHR_PBMP_WORD_WIDTH;\
            _first_port_ +=  _SHR_PBMP_WORD_WIDTH - (_first_port_ % _SHR_PBMP_WORD_WIDTH);\
        }\
    } while (0); 


#define PORTMOD_VSD_PBMP_WORD_WIDTH                           32       /* 32 bits */
#define PORTMOD_VSD_PBMP_WORD_GET(bm,word)             (bm[(word)])
#define PORTMOD_VSD_PBMP_WENT(idx)                     ((idx)/PORTMOD_VSD_PBMP_WORD_WIDTH)
#define PORTMOD_VSD_PBMP_WBIT(idx)                     \
                                        (1U << ((idx) % PORTMOD_VSD_PBMP_WORD_WIDTH))

#define PORTMOD_VSD_PBMP_WORD_ENTRY(bm, idx)                \
                                    (PORTMOD_VSD_PBMP_WORD_GET(bm,PORTMOD_VSD_PBMP_WENT(idx)))

#define PORTMOD_VSD_PBMP_GROUP_ACTIVE(bm, idx)               \
                        ((PORTMOD_VSD_PBMP_WORD_ENTRY(bm, idx) & PORTMOD_VSD_PBMP_WBIT(idx)) != 0)

#define PORTMOD_VSD_PBMP_GROUP_SET(bm, idx)             \
                            ( PORTMOD_VSD_PBMP_WORD_ENTRY(bm, idx) |=  PORTMOD_VSD_PBMP_WBIT(idx))

#define PORTMOD_VSD_PBMP_GROUP_CLEAR(bm, idx)             \
                            ( PORTMOD_VSD_PBMP_WORD_ENTRY(bm, idx) &= ~PORTMOD_VSD_PBMP_WBIT(idx))



#endif /*_PORTMOD_DEFS_H__H_*/
