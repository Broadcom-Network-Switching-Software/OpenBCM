/*! \file bcmptm_rm_tcam_fp.h
 *
 *  APIs, defines for PTRM-TCAM interfaces
 *  This file contains APIs, defines for PTRM-TCAM interfaces
 *  for FP based TCAMs
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_RM_TCAM_FP_H
#define BCMPTM_RM_TCAM_FP_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm.h>
#include <bcmltd/chip/bcmltd_fp_constants.h>

/*******************************************************************************
 * Defines
 */
/*! Number of words for Flags Bitmap*/
#define BCMPTM_FP_FLAGS_BMP_WORDS (BCMPTM_FP_FLAGS_COUNT / 32) + 1

/* ! Max number of parts count for a group */
#define BCMPTM_TCAM_FP_MAX_PARTS  (6)

/* FP ltid_info related macros */
/* Max pipe count */
#define BCMPTM_RM_TCAM_PIPE_COUNT_MAX 4

/* Max slices available for FP stage */
#define BCMPTM_RM_TCAM_MAX_SLICES 12

/* Max depth instances available for FP stage */
#define BCMPTM_RM_TCAM_MAX_DEPTH_INSTANCES 50

/* Max width instances available for FP stage */
#define BCMPTM_RM_TCAM_MAX_WIDTH_INSTANCES 6

/*******************************************************************************
 * Typedefs
 */

/*! Types of Flags */
typedef enum bcmptm_fp_flags_s {

    /*! To indicate that the stage supports preselection */
    BCMPTM_FP_FLAGS_PRESEL_STAGE_SUPPORT,

    /*! To indicate its a preselection entry */
    BCMPTM_FP_FLAGS_PRESEL_ENTRY,

    /*! To indicate its a default preselection entry */
    BCMPTM_FP_FLAGS_PRESEL_ENTRY_DEFAULT,

    /*! To indicate the last entry in a group */
    BCMPTM_FP_FLAGS_LAST_ENTRY_IN_GROUP,

    /*! To indicate the PDD profile per entry */
    BCMPTM_FP_FLAGS_PDD_PROF_PER_ENTRY,

    /*! To indicate the PDD profile per group */
    BCMPTM_FP_FLAGS_PDD_PROF_PER_GROUP,

    /*! To indicate the SBR profile per group */
    BCMPTM_FP_FLAGS_SBR_PROF_PER_GROUP,

    /*! To indicate the SBR profile per entry */
    BCMPTM_FP_FLAGS_SBR_PROF_PER_ENTRY,

    /*! To indicate the PRESEL GROUP */
    BCMPTM_FP_FLAGS_PRESEL_GROUP,

    /*! To indicate the first entry in a group */
    BCMPTM_FP_FLAGS_FIRST_ENTRY_IN_GROUP,

    /*! To indicate to get group info from ptrm. */
    BCMPTM_FP_FLAGS_GET_GROUP_INFO,

    /*! To indicate to use smaller size slice. */
    BCMPTM_FP_FLAGS_USE_SMALL_SLICE,

    /*! To indicate to use large size slice. */
    BCMPTM_FP_FLAGS_USE_LARGE_SLICE,

    /*! To indicate that the group can auto expand. */
    BCMPTM_FP_FLAGS_AUTO_EXPAND_ENABLE,

    /*! To indicate that the group is auto expanding. */
    BCMPTM_FP_FLAGS_AUTO_EXPANDING,

    /*! To indicate to get stage info from ptrm. */
    BCMPTM_FP_FLAGS_GET_STAGE_INFO,

    /*! To indicate to get slice info from ptrm. */
    BCMPTM_FP_FLAGS_GET_SLICE_INFO,

    /*! Total count of flags */
    BCMPTM_FP_FLAGS_COUNT,

} bcmptm_fp_flags_t;

/*! Flags bitmap structure */
typedef struct bcmptm_fp_flags_bmp_s {
    SHR_BITDCL w[BCMPTM_FP_FLAGS_BMP_WORDS];
} bcmptm_fp_flags_bmp_t;

/*! Stages supported */
typedef enum  bcmptm_rm_tcam_fp_stage_id_e {
    /*! IFP stage */
    BCMPTM_RM_TCAM_FP_STAGE_ID_INGRESS,
    /*! EFP stage */
    BCMPTM_RM_TCAM_FP_STAGE_ID_EGRESS,
    /*! VFP Stage */
    BCMPTM_RM_TCAM_FP_STAGE_ID_LOOKUP,
    /*! EM stage */
    BCMPTM_RM_TCAM_FP_STAGE_ID_EXACT_MATCH,
    /*! Stage count */
    BCMPTM_RM_TCAM_FP_STAGE_ID_COUNT,
} bcmptm_rm_tcam_fp_stage_id_t;

/*! To pass selcode from FP to PTM */
typedef struct bcmptm_rm_tcam_fp_group_selcode_s {
    int8_t intraslice;            /* Intraslice double wide selection.    */
    int8_t fpf2;                  /* FPF2 field(s) select.                */
    int8_t fpf3;                  /* FPF3 field(s) select.                */
    int8_t src_sel;               /* Src port/trunk entity selection.     */
    int8_t ip_header_sel;         /* Inner/Outer Ip header selection.     */
    int8_t egr_key1_classid_sel;  /* EFP KEY1 Class ID Selector           */
    int8_t egr_key2_classid_sel;  /* EFP KEY2 Class ID Selector           */
    int8_t egr_key3_classid_sel;  /* EFP KEY3 Class ID Selector           */
    int8_t egr_key4_classid_sel;  /* EFP KEY4 Class ID Selector           */
    int8_t egr_key6_classid_sel;  /* EFP KEY6 Class ID Selector           */
    int8_t egr_key7_classid_sel;  /* EFP KEY7 Class ID Selector           */
    int8_t egr_key8_classid_sel;  /* EFP KEY8 Class ID Selector           */
    int8_t egr_key4_dvp_sel;      /* EFP KEY4 DVP Secondary selector      */
    int8_t egr_key8_dvp_sel;      /* EFP KEY8 DVP Secondary selector      */
    int8_t egr_key4_mdl_sel;      /* EFP KEY4 MDL Secondary selector      */
    int8_t egr_ip_addr_sel;       /* Ip6 address format selection.        */
} bcmptm_rm_tcam_fp_group_selcode_t;

typedef struct bcmptm_rm_tcam_group_hw_info_s {
    uint16_t num_entries_tentative;
    uint8_t slice[BCMPTM_TCAM_FP_MAX_PARTS][FP_SLICE_ID_MAX];
    uint8_t virtual_slice[BCMPTM_TCAM_FP_MAX_PARTS][FP_SLICE_ID_MAX];
    uint8_t num_slices_depth_wise;
} bcmptm_rm_tcam_group_hw_info_t;

typedef struct bcmptm_rm_tcam_stage_hw_info_s {
    uint8_t num_slices;
} bcmptm_rm_tcam_stage_hw_info_t;

typedef struct bcmptm_rm_tcam_slice_hw_info_s {
    uint16_t num_entries_total;
    uint16_t num_entries_created;
} bcmptm_rm_tcam_slice_hw_info_t;

/*! Group Modes supported */
typedef enum  bcmptm_rm_tcam_group_mode_e {
    /*! Group Mode Single */
    BCMPTM_RM_TCAM_GRP_MODE_SINGLE,
    /*! Group Mode Intra Slice Double */
    BCMPTM_RM_TCAM_GRP_MODE_DBLINTRA,
    /*! Group Mode Inter Slice Double */
    BCMPTM_RM_TCAM_GRP_MODE_DBLINTER,
    /*! Group Mode Triple */
    BCMPTM_RM_TCAM_GRP_MODE_TRIPLE,
    /*! Group Mode Quad */
    BCMPTM_RM_TCAM_GRP_MODE_QUAD,
    /*! Group Mode Half */
    BCMPTM_RM_TCAM_GRP_MODE_HALF,
    /*! Group Mode Count */
    BCMPTM_RM_TCAM_GRP_MODE_COUNT,
} bcmptm_rm_tcam_group_mode_t;

/*! This structure is passed from FP to PTM on
 * every INSERT/DELETE/UPDATE operations of
 * entries in FP_INGRESS_ENTRY table. FP will
 * pass this as a void pointer to PTM and FP
 * subcomponent in PTM module will typecast
 * the void pointer to this structure type.
 */
typedef struct bcmptm_rm_tcam_entry_attrs_s {
       /*! Transaction Id of the operation. */
       uint32_t trans_id;

       /*! BCMPTM_FP_XXX flags */
       bcmptm_fp_flags_bmp_t  flags;


       /*! Group Id to which the entry belongs to. */
       uint32_t group_id;

       /*! priority of the group_id.This information
        * is required only when first entry in the group
        * is created.
        */
       uint32_t group_prio;


       /*! ltid  of the group. This information
        * is required to resolve the action priority when
        * conflicting actions are processed when multiple
        * rules are to hit from different groups
        */
       uint8_t group_ltid;

       /*! Lookup Id information needed
        * for DEFIP urpf enabled LT's
        */
       uint8_t lookup_id;

       /*! Mode(SINGLE/DBLINTRA/DBLINTER/TRIPLE/QUAD) of
        * the group.This information is
        *  required only when first entry in the group is created.
        */
       bcmptm_rm_tcam_group_mode_t group_mode;


       /*! Number of preset Ids associated with group_id.
       * This information is required only when first
       * entry in the group is created.
       */
       uint32_t num_presel_ids;

       /*! Stage Id(INGRESS/EGRESS/VLAN) to which the group belongs to. */
       bcmptm_rm_tcam_fp_stage_id_t stage_id;

       /*! Pipe Id(Global pipe (or) pipe 0 (or) Pipe 1 etc.)
        * to which the group belongs to.
        */
       int pipe_id;

       /*! Selcode associated with the group_id. */
       bcmptm_rm_tcam_fp_group_selcode_t selcode[BCMPTM_TCAM_FP_MAX_PARTS];

       /*! Slice id of the group id. */
       uint8_t slice_id;

       /*!
        * LT may occupy multiple slices but entry management
        * should happen per slice.
        */
       bool per_slice_entry_mgmt;

       /*! PDD profile information. */
       uint32_t pdd_profile_info[BCMPTM_TCAM_FP_MAX_PARTS]
                                [BCMPTM_MAX_PT_FIELD_WORDS];

       /*! SBR profile information. */
       uint32_t sbr_profile_info[BCMPTM_TCAM_FP_MAX_PARTS]
                                [BCMPTM_MAX_PT_FIELD_WORDS];

       /*! Presel group profile information. */
       uint32_t presel_group_info[BCMPTM_TCAM_FP_MAX_PARTS]
                                 [BCMPTM_MAX_PT_FIELD_WORDS];

       /*! Physical indexes allocated for PDD proflies. */
       uint8_t pdd_profile_indexes[BCMPTM_TCAM_FP_MAX_PARTS];

       /*! Physical indexes allocated for SBR proflies. */
       uint8_t sbr_profile_indexes[BCMPTM_TCAM_FP_MAX_PARTS];

       /*! Physical indexes allocated for presel groups. */
       uint8_t presel_group_indexes[BCMPTM_TCAM_FP_MAX_PARTS];

       /*! Group hardware information. */
       void *grp_hw_info;

       /*! Stage hardware information. */
       void *stage_hw_info;

       /*! Slice hardware information. */
       void *slice_hw_info;

       /* Slice number for the slice hardware information. */
       uint8_t slice_num;

} bcmptm_rm_tcam_entry_attrs_t;
#endif /* BCMPTM_RM_TCAM_FP_H */
