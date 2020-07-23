/*! \file bcmfp_em_internal.h
 *
 * Init/Cleanup APIs for Exact Match Field Processor.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_EM_INTERNAL_H
#define BCMFP_EM_INTERNAL_H

typedef enum bcmfp_em_pt_types_e {
    BCMFP_EM_PRESEL_PTID = 0,
    BCMFP_EM_LTS_TCAM_PTID,
    BCMFP_EM_LTS_DATA_FLOP_PTID,
    BCMFP_EM_LTS_DATA_PTID,
    BCMFP_EM_LTS_DATA_EXT_PTID,
    BCMFP_EM_KEY_ATTRIBUTES_PTID,
    BCMFP_EM_PDD_PTID,
    BCMFP_EM_SBR_PTID,
    BCMFP_EM_KEY_MASK_PTID,
    BCMFP_EM_LKUP_MISS_PTID,
    BCMFP_EM_LKUP_MISS_LTPR_PTID,
    BCMFP_EM_HIT_INDEX_PTID,
    BCMFP_EM_HASH_SINGLE_PTID,
    BCMFP_EM_HASH_DOUBLE_PTID,
    BCMFP_EM_HASH_QUAD_PTID,
    BCMFP_EM_PTID_COUNT,
} bcmfp_em_pt_types_t;

#define BCMFP_EM_PTID_MIN       BCMFP_EM_PRESEL_PTID

/*! Initialize the s/w state for exact match field processor. */
extern int
bcmfp_em_init(int unit, bool warm);

/*! Register call back function for exact match field processor. */
extern int
bcmfp_em_imm_register(int unit);

/*! Register call back function for exact match field processor. */
extern int
bcmfp_em_imm_register(int unit);

/*! Cleanup the s/w state for exact match field processor. */
extern int
bcmfp_em_cleanup(int unit);

extern void
bcmfp_em_grp_change_event_cb(int unit, const char *event, uint64_t ev_data);

#endif /* BCMFP_EM_INTERNAL_H */
