/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        loopback.h
 */

#ifndef CMICM_EMBEDDED
#include <soc/defs.h>
#endif

#ifndef _SOC_LOOPBACK_H
#define _SOC_LOOPBACK_H

#ifdef BCM_XGS3_SWITCH_SUPPORT
#define START_LBMH_HDR  0xFB
#define COMMON_LBMH_HDR   3
#define TD3X_LBMH_HDR     1
#define VIS_LBMH_HDR      1
#define SRC_TYPE_GPP_LBMH_HDR 1
#define SRC_TYPE_VP_LBMH_HDR  0

typedef struct soc_loopback_hdr_s {
#if defined(LE_HOST)
                                  /* byte # */
    uint32 start:8;             /* 0      */

    uint32 common_hdr0:4;       /* 1      */
    uint32 input_pri:4;         /* 1      */

    uint32 source0:6;           /* 2      */
    uint32 source_type:1;       /* 2      */
    uint32 common_hdr1:1;       /* 2      */

    uint32 source1:8;           /* 3      */

    uint32 reserved0_0:2;       /* 4      */
    uint32 pkt_profile:3;       /* 4      */
    uint32 visibility:1;        /* 4      */
    uint32 source2:2;           /* 4      */

    uint32 reserved0_1:8;       /* 5      */       
    uint32 reserved1_0:8;       /* 6      */
    uint32 reserved_1:5;        /* 7      */
    uint32 zero:1;              /* 7      */
    uint32 reserved1_1:2;       /* 7      */

    uint32 reserved_2:8;        /* 8      */
    uint32 reserved_3:8;        /* 9      */
    uint32 reserved_4:8;        /* 10     */
    uint32 reserved_5:8;        /* 11     */
    uint32 reserved_6:8;        /* 12     */
    uint32 reserved_7:8;        /* 13     */
    uint32 reserved_8:8;        /* 14     */
    uint32 pp_port:8;           /* 15      */
   
#else /* !LE_HOST*/
                                 /* byte # */
    uint32 start:8;             /* 0      */

    uint32 input_pri:4;         /* 1      */
    uint32 common_hdr0:4;       /* 1      */

    uint32 common_hdr1:1;       /* 2      */
    uint32 source_type:1;       /* 2      */
    uint32 source0:6;           /* 2      */

    uint32 source1:8;           /* 3      */

    uint32 source2:2;           /* 4      */
    uint32 visibility:1;        /* 4      */
    uint32 pkt_profile:3;       /* 4      */
    uint32 reserved0_0:2;       /* 4      */

    uint32 reserved0_1:8;       /* 5      */       
    uint32 reserved1_0:8;       /* 6      */
    uint32 reserved1_1:2;       /* 7      */
    uint32 zero:1;              /* 7      */
    uint32 reserved_1:5;        /* 7      */

    uint32 reserved_2:8;        /* 8      */
    uint32 reserved_3:8;        /* 9      */
    uint32 reserved_4:8;        /* 10     */
    uint32 reserved_5:8;        /* 11     */
    uint32 reserved_6:8;        /* 12     */
    uint32 reserved_7:8;        /* 13     */
    uint32 reserved_8:8;        /* 14     */
    uint32 pp_port:8;           /* 15      */
 #endif   
} soc_loopback_hdr_t;


typedef enum {
        LBMH_invalid = -1,
        LBMH_start   =  0,
        LBMH_input_pri,
        LBMH_visibility,
        LBMH_pkt_profile,
        LBMH_src,
        LBMH_src_type,
        LBMH_common_hdr,
        LBMH_pp_port,
        LBMH_cng,
        LBMH_tc,
        LBMH_zero,
        LBMH_COUNT
} soc_loopback_hdr_field_t;


#ifndef CMICM_EMBEDDED

extern soc_loopback_hdr_field_t soc_loopback_hdr_name_to_field(int unit, char *name);
extern char  *soc_loopback_hdr_field_to_name(int unit, soc_loopback_hdr_field_t f);
extern uint32 soc_loopback_hdr_field_get(int unit, soc_loopback_hdr_t *lh,
                                         soc_loopback_hdr_field_t field);
extern void soc_loopback_hdr_field_set(int unit, soc_loopback_hdr_t *lh,
                                       soc_loopback_hdr_field_t field, uint32 val);
extern void soc_loopback_hdr_dump(int unit, char *pfx, soc_loopback_hdr_t *lh);
extern int  soc_loopback_lbport_num_get(int unit, uint32 pipe);
#define soc_looopback_hdr_field_init(unit, mh) \
        sal_memset((mh), 0, sizeof (soc_loopback_hdr_t))

#define LOOPBACK_HDR_SIZE sizeof(soc_loopback_hdr_t)
#endif


#endif /* BCM_XGS3_SWITCH_SUPPORT */
#endif /* _SOC_LOOPBACK_H */

