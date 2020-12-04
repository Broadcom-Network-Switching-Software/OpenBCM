/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        proc_hdr.h
 */

#include <soc/defs.h>
#include <soc/enet.h>

#ifndef _SOC_PROC_HDR_H
#define _SOC_PROC_HDR_H

#if defined(BCM_XGS_SUPPORT) && defined(BCM_CPU_TX_PROC_SUPPORT)

/****************************************************************************
 * CPU TX PROC Header Format
 *
 * ANSI only permits signed or unsigned int for bit field type.  This
 * structure will only work for compilers for which uint32 is unsigned
 * int, and which completely pack bit fields beginning at the MSbit for
 * big-endian machines and at the LSbit for little-endian machines.
 *
 * NOTE: these structures already put the the individual bytes in memory
 * in big endian order for both big- and little-endian machines, so no
 * further swapping is required.
 */

typedef union soc_proc_hdr_u {
    struct {
        uint8   bytes[16];
    } overlay0;

#if defined(LE_HOST)

    struct {                            /* Byte # */
        uint32  start:8;                /* 0 */
        uint32  header_types_4_1:4;     /* 1 */
        uint32  input_pri:4;
        uint32  source_15_10:6;         /* 2 */
        uint32  source_type:1;
        uint32  header_types_0:1;
        uint32  source_9_2:8;           /* 3 */
        uint32  rsvd_9_8:2;             /* 4 */
        uint32  pkt_profile:3;
        uint32  visibility_pkt:1;
        uint32  source_1_0:2;
        uint32  rsvd_7_9:8;             /* 5 */
        uint32  mcast_lb_idx_7_2:6;     /* 6 */
        uint32  mcast_lb_idx_valid:1;
        uint32  rsvd:1;
        uint32  rsvd_4_0:5;             /* 7 */
        uint32  zero:1;
        uint32  mcast_lb_idx_1_0:2;
        uint32  ecmp_member_id_11_4:8;  /* 8 */
        uint32  int_pri_3_2:2;          /* 9 */
        uint32  dp:2;
        uint32  ecmp_member_id_3_0:4;
        uint32  rsvd_11_10:2;           /* 10 */
        uint32  routed_pkt:1;
        uint32  qos_fields_valid:1;
        uint32  int_cn:2;
        uint32  int_pri_1_0:2;
        uint32  rsvd_9_2:8;             /* 11 */
        uint32  destination_15_10:6;     /* 12 */
        uint32  rsvd_1_0:2;
        uint32  destination_9_2:8;      /* 13 */
        uint32  subflow_type:2;         /* 14 */
        uint32  destination_type:4;
        uint32  destination_1_0:2;
        uint32  pp_port:8;              /* 15 */
   } overlay1;

#else /* !LE_HOST */

    struct {                            /* Byte # */
        uint32  start:8;                /* 0 */
        uint32  input_pri:4;            /* 1 */
        uint32  header_types_4_1:4;
        uint32  header_types_0:1;       /* 2 */
        uint32  source_type:1;
        uint32  source_15_10:6;
        uint32  source_9_2:8;           /* 3 */
        uint32  source_1_0:2;           /* 4 */
        uint32  visibility_pkt:1;
        uint32  pkt_profile:3;
        uint32  rsvd_9_8:2;
        uint32  rsvd_7_9:8;             /* 5 */
        uint32  rsvd:1;                 /* 6 */
        uint32  mcast_lb_idx_valid:1;
        uint32  mcast_lb_idx_7_2:6;
        uint32  mcast_lb_idx_1_0:2;     /* 7 */
        uint32  zero:1;
        uint32  rsvd_4_0:5;
        uint32  ecmp_member_id_11_4:8;  /* 8 */
        uint32  ecmp_member_id_3_0:4;   /* 9 */
        uint32  dp:2;
        uint32  int_pri_3_2:2;
        uint32  int_pri_1_0:2;          /* 10 */
        uint32  int_cn:2;
        uint32  qos_fields_valid:1;
        uint32  routed_pkt:1;
        uint32  rsvd_11_10:2;
        uint32  rsvd_9_2:8;             /* 11 */
        uint32  rsvd_1_0:2;             /* 12 */
        uint32  destination_15_10:6;
        uint32  destination_9_2:8;      /* 13 */
        uint32  destination_1_0:2;      /* 14 */
        uint32  destination_type:4;
        uint32  subflow_type:2;
        uint32  pp_port:8;              /* 15 */
   } overlay1;

#endif
} soc_proc_hdr_t;

#define SOC_PROC_HDR_SIZE      ((int) sizeof(soc_proc_hdr_t))

/* default SOP Symbol */
#define SOC_TX_PROC_START        0xfb /* Default Start */

/*
 * TX PROC Header Field Manipulation
 */

typedef enum {
    /* NOTE: strings in soc_proc_hdr_field_names[] must match */
    PH_invalid = -1,
    PH_start = 0,
    PH_input_pri,
    PH_header_types,
    PH_source_type,
    PH_source,
    PH_visibility_pkt,
    PH_pkt_profile,
    PH_mcast_lb_idx_valid,
    PH_mcast_lb_idx,
    PH_zero,
    PH_ecmp_member_id,
    PH_dp,
    PH_int_pri,
    PH_int_cn,
    PH_qos_fields_valid,
    PH_routed_pkt,
    PH_destination,
    PH_destination_type,
    PH_subflow_type,
    PH_pp_port,
    PH_COUNT
} soc_proc_hdr_field_t;

/* NOTE: strings must match soc_proc_hdr_field_t */
#define SOC_PROC_HDR_FIELD_NAMES_INIT                   \
    "start",                                            \
    "input_pri",                                        \
    "header_types",                                     \
    "source_type",                                      \
    "source",                                           \
    "visibility_pkt",                                   \
    "pkt_profile",                                      \
    "mcast_lb_idx_valid",                               \
    "mcast_lb_idx",                                     \
    "zero",                                             \
    "ecmp_member_id",                                   \
    "dp",                                               \
    "int_pri",                                          \
    "int_cn",                                           \
    "qos_fields_valid",                                 \
    "routed_pkt",                                       \
    "destination",                                      \
    "destination_type",                                 \
    "subflow_type",                                     \
    "pp_port",                                          \
    NULL

/*
 * Function Name:
 *         soc_proc_hdr_name_to_field
 * Purpose:
 *         To convert field name to the field
 * Parameters:
 *         [IN] unit - unit number
 *         [IN] name - ascii name of the field
 * Return:
 *         Proc HDR Field or invalid code
 */
extern soc_proc_hdr_field_t soc_proc_hdr_name_to_field(int unit, char *name);

/*
 * Function Name:
 *         soc_proc_hdr_field_to_name
 * Purpose:
 *         To convert field to field name
 * Parameters:
 *         [IN] unit - unit number
 *         [IN] name - field in the proc hdr
 * Return:
 *         Field character name or ?? on ERROR
 */
extern char *soc_proc_hdr_field_to_name(int unit, soc_proc_hdr_field_t f);

/*
 * Function Name:
 *         soc_proc_hdr_field_get
 * Purpose:
 *         To get the value of a field in the proc header
 * Parameters:
 *         [IN] unit  - unit number
 *         [INOUT] ph    - proc header to be modified
 *         [IN] field - field to be modified
 * Return:
 *         value of the field, if field is not found then Log Warning
 */
extern uint32 soc_proc_hdr_field_get(int unit, soc_proc_hdr_t *ph,
                                  soc_proc_hdr_field_t field);

/*
 * Function Name:
 *         soc_proc_hdr_field_set
 * Purpose:
 *         To set the value of a field in the proc header
 * Parameters:
 *         [IN] unit  - unit number
 *         [IN] ph    - proc header to be modified
 *         [IN] field - field to be modified
 *         [IN] val   - value of the field
 * Return:
 *         None
 */

extern void soc_proc_hdr_field_set(int unit, soc_proc_hdr_t *ph,
                                soc_proc_hdr_field_t field, uint32 val);

/*
 * Function Name:
 *         soc_proc_hdr_field_dump
 * Purpose:
 *         to output the entire proc header in a user friendly format
 * Parameters:
 *         [IN] unit - unit number
 *         [IN] pfx  - character prefix to be appended to the header output.
 *         [IN] ph   - proc header to be modified
 * Return:
 *         None
 */
extern void soc_proc_hdr_dump(int unit, char *pfx, soc_proc_hdr_t *ph);

/*
 * Function:
 *     soc_lbport_num_get
 * Description:
 *      return FIRST loopback port of the chip
 * Parameters:
 *      unit         device number
 * Return:
 *      first loopback port on the given device
 */
extern int soc_lbport_num_get(int unit, uint32 pipe);

/*
 * Function:
 *     soc_pipe_get
 * Description:
 *      return the pipe of the chip
 * Parameters:
 *      unit             device number
 *      logical_src_port source port
 * Return:
 *      pipe number based on logical src port
 */
extern int soc_pipe_get(int unit, uint32 logical_src_port);

/* CPU TX PROC header Op-Codes */
#define SOC_TX_PROC_OP_DGPP        0x00 /* Destination GPP */
#define SOC_TX_PROC_OP_NHI         0x01 /* Next Hop */
#define SOC_TX_PROC_OP_ECMP        0x02 /* ECMP group */
#define SOC_TX_PROC_OP_ECMP_MEMBER 0x03 /* ECMP member */
#define SOC_TX_PROC_OP_IPMC        0x04 /* IP Multicast Frame */
#define SOC_TX_PROC_OP_L2MC        0x05 /* L2 Multicast Frame */
#define SOC_TX_PROC_OP_VLAN_FLOOD  0x06 /* VLAN Flood */

#define SOC_TX_PROC_ETHERNET_HEADER_TYPE    0x03 /* Ethernet */
#define SOC_TX_PROC_SOURCE_TYPE             0x01 /* Source type for SGPP */
#define SOC_TX_PROC_SUBFLOW_TYPE            0x01 /* Subflow type for CPU_TX_PROC */
#define SOC_TX_PROC_VISIBILITY_PKT          0    /* CPU TX PROC is not a visibility packet */
#define SOC_TX_PROC_ROUTED_PKT              1    /* L3 routed packet */

#endif /* BCM_XGS_SUPPORT && BCM_CPU_TX_PROC_SUPPORT */

#endif  /* !_SOC_PROC_HDR_H */
