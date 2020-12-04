/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains multicast definitions.
 */

#ifndef _SHR_PKT_H
#define _SHR_PKT_H
#include <shared/types.h>
#include <shared/pbmp.h>
#include <shared/rx.h>
#include <shared/port.h>



#define _SHR_PKT_NOF_DNX_HEADERS 9
#define _SHR_PKT_DNX_RAW_SIZE_MAX    20  


/* Pkt DNX types. */
typedef enum  {
    _SHR_PKT_DNX_TYPE_ITMH = 0,  /* ITMH Header. */
    _SHR_PKT_DNX_TYPE_FTMH = 1,  /* FMTH Header. */
    _SHR_PKT_DNX_TYPE_OTMH = 2,  /* OTMH Header. */
    _SHR_PKT_DNX_TYPE_OTSH = 3,  /* OAM-TS Header (OTSH). */
    _SHR_PKT_DNX_TYPE_INTERNALS = 4  /* DNX Internal fields */

} _shr_pkt_dnx_type_t;
 
/* itmh dest type. */
typedef enum  {
    _SHR_PKT_DNX_ITMH_DEST_TYPE_MULTICAST = 0, /* ITMH destination type is multicast. */
    _SHR_PKT_DNX_ITMH_DEST_TYPE_FLOW = 1,      /* ITMH destination type is flow. */
    _SHR_PKT_DNX_ITMH_DEST_TYPE_INGRESS_SHAPING_FLOW = 2, /* ITMH destination type is ingress
                                           shaping flow. */
    _SHR_PKT_DNX_ITMH_DEST_TYPE_VPORT = 3,     /* ITMH destination type is out lif. */
    _SHR_PKT_DNX_ITMH_DEST_TYPE_SYSTEM_PORT = 4 /* ITMH destination type is system port. */
} _shr_pkt_dnx_itmh_dest_type_t;


/* ftmh action type. */
typedef enum  {
    _SHR_PKT_DNX_FTMH_ACTION_TYPE_FORWARD = 0, /* TM action is forward */
    _SHR_PKT_DNX_FTMH_ACTION_TYPE_SNOOP = 1,   /* TM action is snoop */
    _SHR_PKT_DNX_FTMH_ACTION_TYPE_INBOUND_MIRROR = 2, /* TM action is inbound mirror. */
    _SHR_PKT_DNX_FTMH_ACTION_TYPE_OUTBOUND_MIRROR = 3, /* TM action is outbound mirror. */
    _SHR_PKT_DNX_FTMH_ACTION_TYPE_MIRROR = 4,  /* TM action is mirror. */
    _SHR_PKT_DNX_FTMH_ACTION_TYPE_STATISTICAL_SAMPLING = 5 /* TM action is statistical sampling. */
} _shr_pkt_dnx_ftmh_action_type_t;
  
/* otsh type */
typedef enum  {
    _SHR_PKT_DNX_OTSH_TYPE_OAM = 0,       /* OAM-TS type is OAM */
    _SHR_PKT_DNX_OTSH_TYPE_1588v2 = 1,    /* OAM-TS type is 1588v2 */
    _SHR_PKT_DNX_OTSH_TYPE_RESERVED = 2,  /* OAM-TS type reseved */
    _SHR_PKT_DNX_OTSH_TYPE_LATENCY = 3    /* OAM-TS type is packet latency */
} _shr_pkt_dnx_otsh_type_t;

/* otsh oam sutype */
typedef enum  {
    _SHR_PKT_DNX_OTSH_OAM_SUBTYPE_NONE = 0,    /* None */
    _SHR_PKT_DNX_OTSH_OAM_SUBTYPE_LM = 1,      /* Loss Measurement (LM) */
    _SHR_PKT_DNX_OTSH_OAM_SUBTYPE_DM1588 = 2,  /* Delay Measurement (DM) - 1588 ToD */
    _SHR_PKT_DNX_OTSH_OAM_SUBTYPE_DM_NTP = 3,   /* Delay Measurement (DM) - NTP ToD */
    _SHR_PKT_DNX_OTSH_OAM_SUBTYPE_OAM_DEFAULT = 4, /* Default OAM type */
    _SHR_PKT_DNX_OTSH_OAM_SUBTYPE_LOOPBACK = 5, /* Loopback */
    _SHR_PKT_DNX_OTSH_OAM_SUBTYPE_ECN = 7      /* ECN */
} _shr_pkt_dnx_otsh_oam_subtype_t;

  
#endif /* _SHR_PKT_H */
