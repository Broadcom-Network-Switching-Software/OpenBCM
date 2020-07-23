/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SOC_TYPES_H
#define _SOC_TYPES_H

#include <sal/types.h>

#include <soc/defs.h>

#include <shared/types.h>
#include <shared/pbmp.h>
#include <shared/gport.h>
#include <shared/multicast.h>
#include <shared/pkt.h>

/*
 * Port Bitmap Macro Interface
 * See shared/pbmp.h for more details.
 */

typedef _shr_pbmp_t soc_pbmp_t;

/* soc_module_t */
typedef _shr_module_t soc_module_t;
typedef _shr_if_t soc_if_t;

#define pbmp_t      soc_pbmp_t

#define SOC_PBMP_PORT_MAX       _SHR_PBMP_PORT_MAX

#define SOC_MODID_ALL           ((soc_module_t) -2)

/* used for hw registers */
#define SOC_PBMP_WORD_MAX       _SHR_PBMP_WORD_MAX
#define SOC_PBMP_WORD_GET(bmp, word)    _SHR_PBMP_WORD_GET((bmp), (word))
#define SOC_PBMP_WORD_SET(bmp, word, val)   \
    _SHR_PBMP_WORD_SET((bmp), (word), (val))

#define SOC_PBMP_CLEAR(pbm)             _SHR_PBMP_CLEAR(pbm)
#define SOC_PBMP_MEMBER(bmp, port)      _SHR_PBMP_MEMBER((bmp), (port))
#define SOC_PBMP_ITER(bmp, port)        _SHR_PBMP_ITER((bmp), (port))
#define SOC_PBMP_LAST(bmp, port)        _SHR_PBMP_LAST((bmp), (port))
#define SOC_PBMP_COUNT(pbm, count)  _SHR_PBMP_COUNT(pbm, count)

#define SOC_PBMP_IS_NULL(pbm)           _SHR_PBMP_IS_NULL(pbm)
#define SOC_PBMP_NOT_NULL(pbm)          _SHR_PBMP_NOT_NULL(pbm)
#define SOC_PBMP_EQ(pbm_a, pbm_b)       _SHR_PBMP_EQ(pbm_a, pbm_b)
#define SOC_PBMP_NEQ(pbm_a, pbm_b)      _SHR_PBMP_NEQ(pbm_a, pbm_b)

/* Assignment operators */
#define SOC_PBMP_ASSIGN(dst, src)       _SHR_PBMP_ASSIGN(dst, src)
#define SOC_PBMP_AND(pbm_a, pbm_b)      _SHR_PBMP_AND(pbm_a, pbm_b)
#define SOC_PBMP_OR(pbm_a, pbm_b)       _SHR_PBMP_OR(pbm_a, pbm_b)
#define SOC_PBMP_XOR(pbm_a, pbm_b)      _SHR_PBMP_XOR(pbm_a, pbm_b)
#define SOC_PBMP_REMOVE(pbm_a, pbm_b)   _SHR_PBMP_REMOVE(pbm_a, pbm_b)
#define SOC_PBMP_NEGATE(pbm_a, pbm_b)   _SHR_PBMP_NEGATE(pbm_a, pbm_b)

/* Port PBMP operators */
#define SOC_PBMP_PORT_SET(pbm, port)    _SHR_PBMP_PORT_SET(pbm, port)
#define SOC_PBMP_PORT_ADD(pbm, port)    _SHR_PBMP_PORT_ADD(pbm, port)
#define SOC_PBMP_PORT_REMOVE(pbm, port) _SHR_PBMP_PORT_REMOVE(pbm, port)
#define SOC_PBMP_PORT_FLIP(pbm, port)   _SHR_PBMP_PORT_FLIP(pbm, port)
#define SOC_PBMP_PORT_VALID(port)   _SHR_PBMP_PORT_VALID(port)
#define SOC_PBMP_PORTS_RANGE_ADD(bm, first_port, range) _SHR_PBMP_PORTS_RANGE_ADD(bm, first_port, range)
#define SOC_PBMP_PORT_INFO_UPDATE(ptype) \
        do { \
            soc_port_t     _p; \
            ptype.num  = 0; \
            ptype.min = ptype.max = -1; \
            _SHR_PBMP_ITER(ptype.bitmap, _p) { \
                ptype.port[ptype.num++] = _p; \
                if (ptype.min < 0) { \
                    ptype.min = _p; \
                } \
                if (_p > ptype.max) { \
                    ptype.max = _p; \
                } \
            } \
        } while(0)

#define SOC_PBMP_FMT_LEN        _SHR_PBMP_FMT_LEN
#define SOC_PBMP_FMT(pbm, buf)      _SHR_PBMP_FMT(pbm, buf)

/* backwards compatibility */
#define PBMP_MEMBER(bmp, port)          SOC_PBMP_MEMBER((bmp), (port))
#define PBMP_ITER(bmp, port)            SOC_PBMP_ITER((bmp), (port))

#define KILO(x)                 ((x) * 1024)
#define MEGA(x)                 ((x) * 1024 * 1024)
#define GIGA(x)                 ((x) * 1024 * 1024 * 1024)
#define TERA(x)                 ((x) * 1024 * 1024 * 1024 * 1024)

typedef int     soc_cos_t;
typedef int     soc_port_t;
typedef int     soc_block_t;        /* see soc/chip.h */
typedef int     *soc_block_types_t;
typedef int     soc_multicast_t;

#define SOC_PORT_INVALID  _SHR_PORT_INVALID

typedef uint32  soc_page_t;

typedef uint32  soc_scache_handle_t;

/*
 * VLAN Types
 */

typedef uint16  vlan_id_t;

#define VLAN_ID_NONE                    ((vlan_id_t) 0x0000)
#define VLAN_ID_MIN                     ((vlan_id_t) 1)
#define VLAN_ID_MAX                     ((vlan_id_t) 4095)
#define VLAN_ID_DEFAULT                 ((vlan_id_t) 1)
#define VLAN_ID_INVALID                 ((vlan_id_t) (VLAN_ID_MAX + 1))
#define VLAN_ID_VALID(id)               ((id) >= VLAN_ID_MIN && \
                                         (id) <= VLAN_ID_MAX)

#define VLAN_CTRL(prio, cfi, id)        (((prio) & 0x007) << 13 | \
                                         ((cfi ) & 0x001) << 12 | \
                                         ((id  ) & 0xfff) << 0)

#define VLAN_CTRL_PRIO(c)               ((c) >> 13 & 0x007)
#define VLAN_CTRL_CFI(c)                ((c) >> 12 & 0x001)
#define VLAN_CTRL_ID(c)                 ((c) >>  0 & 0xfff)

/* 
 * STG definition.
 */
typedef int stg_id_t;

#define STG_ID_DEFAULT         ((stg_id_t) 1) 
#define STG_ID_INVALID         ((stg_id_t) -1) 


/* 
 * Trunk definition.
 */
typedef int trunk_id_t;


/*
 * GPORT (generic port) definitions
 * See shared/gport.h for more details.
 */

typedef int    soc_gport_t;

#define SOC_GPORT_NONE        _SHR_GPORT_NONE
#define SOC_GPORT_INVALID     _SHR_GPORT_INVALID
#define SOC_GPORT_BLACK_HOLE  _SHR_GPORT_BLACK_HOLE
#define SOC_GPORT_LOCAL_CPU   _SHR_GPORT_LOCAL_CPU

#define SOC_GPORT_IS_LOCAL(_gport)       _SHR_GPORT_IS_LOCAL(_gport)
#define SOC_GPORT_IS_MODPORT(_gport)     _SHR_GPORT_IS_MODPORT(_gport)
#define SOC_GPORT_IS_TRUNK(_gport)       _SHR_GPORT_IS_TRUNK(_gport)
#define SOC_GPORT_IS_BLACK_HOLE(_gport)  _SHR_GPORT_IS_BLACK_HOLE(_gport)
#define SOC_GPORT_IS_LOCAL_CPU(_gport)   _SHR_GPORT_IS_LOCAL_CPU(_gport)
#define SOC_GPORT_IS_MPLS_PORT(_gport)   _SHR_GPORT_IS_MPLS_PORT(_gport)
#define SOC_GPORT_IS_SUBPORT_GROUP(_gport) _SHR_GPORT_IS_SUBPORT_GROUP(_gport)
#define SOC_GPORT_IS_SUBPORT_PORT(_gport) _SHR_GPORT_IS_SUBPORT_PORT(_gport)
#define SOC_GPORT_IS_UCAST_QUEUE_GROUP(_gport) _SHR_GPORT_IS_UCAST_QUEUE_GROUP(_gport)
#define SOC_GPORT_IS_SCHEDULER(_gport)   _SHR_GPORT_IS_SCHEDULER(_gport)
#define SOC_GPORT_IS_DEVPORT(_gport)     _SHR_GPORT_IS_DEVPORT(_gport)
#define SOC_GPORT_IS_MIM_PORT(_gport)    _SHR_GPORT_IS_MIM_PORT(_gport)
#define SOC_GPORT_IS_WLAN_PORT(_gport)   _SHR_GPORT_IS_WLAN_PORT(_gport)
#define SOC_GPORT_IS_TRILL_PORT(_gport)  _SHR_GPORT_IS_TRILL_PORT(_gport)
#define SOC_GPORT_IS_VLAN_PORT(_gport)   _SHR_GPORT_IS_VLAN_PORT(_gport)
#define SOC_GPORT_IS_NIV_PORT(_gport)    _SHR_GPORT_IS_NIV_PORT(_gport)
#define SOC_GPORT_IS_L2GRE_PORT(_gport)    _SHR_GPORT_IS_L2GRE_PORT(_gport)
#define SOC_GPORT_IS_VXLAN_PORT(_gport)    _SHR_GPORT_IS_VXLAN_PORT(_gport)
#define SOC_GPORT_IS_EXTENDER_PORT(_gport) _SHR_GPORT_IS_EXTENDER_PORT(_gport)

#define SOC_GPORT_LOCAL_SET(_gport, _port) _SHR_GPORT_LOCAL_SET(_gport, _port)
#define SOC_GPORT_LOCAL_GET(_gport)        _SHR_GPORT_LOCAL_GET(_gport)

#define SOC_GPORT_MODPORT_SET(_gport, _module, _port)    \
        _SHR_GPORT_MODPORT_SET(_gport, _module, _port)
#define SOC_GPORT_MODPORT_MODID_GET(_gport)    \
        _SHR_GPORT_MODPORT_MODID_GET(_gport)
#define SOC_GPORT_MODPORT_PORT_GET(_gport)    \
        _SHR_GPORT_MODPORT_PORT_GET(_gport)

#define SOC_GPORT_TRUNK_SET(_gport, _trunk_id)    \
        _SHR_GPORT_TRUNK_SET(_gport, _trunk_id)
#define SOC_GPORT_TRUNK_GET(_gport)    \
        _SHR_GPORT_TRUNK_GET(_gport)

#define SOC_GPORT_MPLS_PORT_ID_SET(_gport, _id)    \
        _SHR_GPORT_MPLS_PORT_ID_SET(_gport, _id)
#define SOC_GPORT_MPLS_PORT_ID_GET(_gport)           \
        _SHR_GPORT_MPLS_PORT_ID_GET(_gport))

#define SOC_GPORT_SUBPORT_GROUP_SET(_gport, _id)    \
        _SHR_GPORT_SUBPORT_GROUP_SET(_gport, _id)
#define SOC_GPORT_SUBPORT_GROUP_GET(_gport)           \
        _SHR_GPORT_SUBPORT_GROUP_GET(_gport))

#define SOC_GPORT_SUBPORT_PORT_SET(_gport, _id)    \
        _SHR_GPORT_SUBPORT_PORT_SET(_gport, _id)
#define SOC_GPORT_SUBPORT_PORT_GET(_gport)           \
        _SHR_GPORT_SUBPORT_PORT_GET(_gport))

#define SOC_GPORT_UCAST_QUEUE_GROUP_SET(_gport, _id)    \
        _SHR_GPORT_UCAST_QUEUE_GROUP_SET(_gport, _id)
#define SOC_GPORT_UCAST_QUEUE_GROUP_GET(_gport)           \
        _SHR_GPORT_UCAST_QUEUE_GROUP_GET(_gport))

#define SOC_GPORT_SCHEDULER_SET(_gport, _id)    \
        _SHR_GPORT_SCHEDULER_SET(_gport, _id)
#define SOC_GPORT_SCHEDULER_GET(_gport)           \
        _SHR_GPORT_SCHEDULER_GET(_gport))

#define SOC_GPORT_SCHEDULER_CORE_GET(_gport)  \
    _SHR_GPORT_SCHEDULER_CORE_GET(_gport) 
#define SOC_GPORT_SCHEDULER_CORE_SET(_gport, _scheduler_id, _core_id)  \
    _SHR_GPORT_SCHEDULER_SET(_gport, _scheduler_id) 


#define SOC_GPORT_DEVPORT_SET(_gport, _device, _port)    \
        _SHR_GPORT_DEVPORT_SET(_gport, _device, _port)
#define SOC_GPORT_DEVPORT_DEVID_GET(_gport)    \
        _SHR_GPORT_DEVPORT_DEVID_GET(_gport)
#define SOC_GPORT_DEVPORT_PORT_GET(_gport)    \
        _SHR_GPORT_DEVPORT_PORT_GET(_gport)

#define SOC_GPORT_MIM_PORT_ID_SET(_gport, _id)    \
        _SHR_GPORT_MIM_PORT_ID_SET(_gport, _id)
#define SOC_GPORT_MIM_PORT_ID_GET(_gport)           \
        _SHR_GPORT_MIM_PORT_ID_GET(_gport))

#define SOC_GPORT_WLAN_PORT_ID_SET(_gport, _id)    \
        _SHR_GPORT_WLAN_PORT_ID_SET(_gport, _id)
#define SOC_GPORT_WLAN_PORT_ID_GET(_gport)           \
        _SHR_GPORT_WLAN_PORT_ID_GET(_gport))

#define SOC_GPORT_TRILL_PORT_ID_SET(_gport, _id)    \
         _SHR_GPORT_TRILL_PORT_ID_SET(_gport, _id)
#define SOC_GPORT_TRILL_PORT_ID_GET(_gport)           \
         _SHR_GPORT_TRILL_PORT_ID_GET(_gport))

#define SOC_GPORT_VLAN_PORT_ID_SET(_gport, _id)    \
        _SHR_GPORT_VLAN_PORT_ID_SET(_gport, _id)
#define SOC_GPORT_VLAN_PORT_ID_GET(_gport)           \
        _SHR_GPORT_VLAN_PORT_ID_GET(_gport))

#define SOC_GPORT_NIV_PORT_ID_SET(_gport, _id)    \
        _SHR_GPORT_NIV_PORT_ID_SET(_gport, _id)
#define SOC_GPORT_NIV_PORT_ID_GET(_gport)           \
        _SHR_GPORT_NIV_PORT_ID_GET(_gport))

#define SOC_GPORT_L2GRE_PORT_ID_SET(_gport, _id)    \
        _SHR_GPORT_L2GRE_PORT_ID_SET(_gport, _id)
#define SOC_GPORT_L2GRE_PORT_ID_GET(_gport)           \
        _SHR_GPORT_L2GRE_PORT_ID_GET(_gport))

#define SOC_GPORT_VXLAN_PORT_ID_SET(_gport, _id)    \
        _SHR_GPORT_VXLAN_PORT_ID_SET(_gport, _id)
#define SOC_GPORT_VXLAN_PORT_ID_GET(_gport)           \
        _SHR_GPORT_VXLAN_PORT_ID_GET(_gport))

#define SOC_GPORT_EXTENDER_PORT_ID_SET(_gport, _id)    \
        _SHR_GPORT_EXTENDER_PORT_ID_SET(_gport, _id)
#define SOC_GPORT_EXTENDER_PORT_ID_GET(_gport)           \
        _SHR_GPORT_EXTENDER_PORT_ID_GET(_gport))

/*
 * Multicast replication structure and flags for usage in soc code that must be the same as bcm_multicast_replication_t
 */

/* values for the flags field of soc_multicast_replication_t */
typedef struct { /* the structure represents a multicast replication */
    uint32 flags;       /* information on the replication */
    soc_gport_t port;   /* destination */
    soc_if_t encap1;    /* first encapsulation. */
    soc_if_t encap2;    /* second encapsulation. */
} soc_multicast_replication_t;
#define SOC_MUTICAST_REPLICATION_ENCAP2_VALID _SHR_MUTICAST_REPLICATION_ENCAP2_VALID /* determines if encap2 is used. */
#define SOC_MUTICAST_REPLICATION_ENCAP1_L3_INTF _SHR_MUTICAST_REPLICATION_ENCAP1_L3_INTF /* when encap2 is used, determines if encap1 is a routing interface or not. */
#endif  /* !_SOC_TYPES_H */
