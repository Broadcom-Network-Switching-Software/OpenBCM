/*! \file bcmfp_types_internal.h
 *
 * All BCMFP generic data types.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_TYPES_INTERNAL_H
#define BCMFP_TYPES_INTERNAL_H

#include <shr/shr_bitop.h>

/*! BCMFP opcodes. */
typedef enum bcmfp_opcode_e {

    /*! Insert opcode. */
    BCMFP_OPCODE_INSERT,

    /*! Delete opcode. */
    BCMFP_OPCODE_DELETE,

    /*! Update opcode. */
    BCMFP_OPCODE_UPDATE,

    /*! Lookup opcode. */
    BCMFP_OPCODE_LOOKUP,

    /*! Invalid opcode. */
    BCMFP_OPCODE_COUNT,

} bcmfp_opcode_t;

/* BCMFP stage ID. */
typedef enum bcmfp_stage_id_e {

    /* Ingress stage ID(IFP/FP_ING). */
    BCMFP_STAGE_ID_INGRESS = 0,

    /* Egress stage ID(EFP/FP_EGR). */
    BCMFP_STAGE_ID_EGRESS,

    /* VLAN stage ID(VFP/FP_VLAN). */
    BCMFP_STAGE_ID_LOOKUP,

    /* Exact match stage ID(EM/FP_EM). */
    BCMFP_STAGE_ID_EXACT_MATCH,

    /* Flow Tracker EM stage ID(EM_FT). */
    BCMFP_STAGE_ID_FLOW_TRACKER,

    /* Last in the list. */
    BCMFP_STAGE_ID_COUNT

} bcmfp_stage_id_t;

/* Flags for qualifier FIDs in rule tables */
typedef enum bcmfp_qualifier_fid_flag_e {

    /*! Qualifier FID is a data. */
    BCMFP_QUALIFIER_FID_FLAGS_DATA = 0,

    /*! Qualifier FID is a mask. */
    BCMFP_QUALIFIER_FID_FLAGS_MASK,

    /*! Qualifier FID s upper 64bits of 128bits. */
    BCMFP_QUALIFIER_FID_FLAGS_UPPER,

    /*! Qualifier FID is lower 64bits of 128bits. */
    BCMFP_QUALIFIER_FID_FLAGS_LOWER,

    /*! This FID is of action type. */
    BCMFP_QUALIFIER_FID_FLAGS_ACTION,

    /*! This FID needs to be in all parts of multi wide. */
    BCMFP_QUALIFIER_FID_FLAGS_ALL_PARTS,

    /*! This FID needs to be set for match ID qualifiers. */
    BCMFP_QUALIFIER_FID_FLAGS_MATCH_ID,

    /* Last in the list. */
    BCMFP_QUALIFIER_FID_FLAGS_COUNT,

} bcmfp_qualifier_fid_flag_t;

typedef struct bcmfp_qualifier_fid_flags_s {
    SHR_BITDCL w[SHRi_BITDCLSIZE(BCMFP_QUALIFIER_FID_FLAGS_COUNT)];
} bcmfp_qualifier_fid_flags_t;

/* BCMFP group ID */
typedef uint32_t bcmfp_group_id_t;

/* BCMFP rule ID. */
typedef uint32_t bcmfp_rule_id_t;

/* BCMFP policy ID. */
typedef uint32_t bcmfp_policy_id_t;

/* BCMFP entry ID. */
typedef uint32_t bcmfp_entry_id_t;

/* BCMFP part ID. */
typedef uint8_t bcmfp_part_id_t;

/* BCMFP PDD profile ID. */
typedef uint32_t bcmfp_pdd_id_t;

/* BCMFP SBR profile ID. */
typedef uint32_t bcmfp_sbr_id_t;

/* BCMFP meter ID. */
typedef uint32_t bcmfp_meter_id_t;

/* BCMFP counter entry ID. */
typedef uint32_t bcmfp_ctr_entry_id_t;

/* BCMFP counter stat ID. */
typedef uint32_t bcmfp_ctr_stat_id_t;

/* BCMFP presel entry ID. */
typedef uint32_t bcmfp_pse_id_t;

/* BCMFP presel group ID. */
typedef uint32_t bcmfp_psg_id_t;

/*!
 * Maximum number of words required for any FP object.
 */
#define BCMFP_MAX_WSIZE 32

/*! Used to get the start pointer of group operational info */
#define BCMFP_GROUP_ID_ALL 0xFFFFFFFF

/*! Maximum number of key fields among all FP LTs in all devices. */
#define BCMFP_LTD_KEY_FIELDS_MAX 0x2

/* Qualifier data/mask buffer. */
typedef uint32_t bcmfp_data_t[BCMFP_MAX_WSIZE];

#endif /* BCMFP_TYPES_INTERNAL_H */

