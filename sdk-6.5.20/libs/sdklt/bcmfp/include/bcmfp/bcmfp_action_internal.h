/*! \file bcmfp_action_internal.h
 *
 * Actions related data structures, defines, enums and APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_ACTION_INTERNAL_H
#define BCMFP_ACTION_INTERNAL_H

#define BCMFP_ACTIONS_COUNT 1024

#define BCMFP_ACTION_NONE (0)

typedef uint32_t bcmfp_action_t;

typedef struct bcmfp_actions_s {
    SHR_BITDCL w[SHRi_BITDCLSIZE(BCMFP_ACTIONS_COUNT)];
} bcmfp_actions_t;

typedef enum bcmfp_action_flags_e {

    /*! Action has to be in one part */
    BCMFP_ACTION_FLAGS_ONE_PART = 0,

    /*! Action has to be in all parts */
    BCMFP_ACTION_FLAGS_ALL_PARTS,

    /*Last in the list */
    BCMFP_ACTION_FLAGS_COUNT,

} bcmfp_action_flags_t;

/*!
 * View type for FP Policy entries
 */
typedef enum bcmfp_action_viewtype_t {
    /*! Default View. */
    BCMFP_ACTION_VIEWTYPE_DEFAULT       = 0,
    /*! SRV6 View. */
    BCMFP_ACTION_VIEWTYPE_SRV6          = 1,
    /*! Vxlan view. */
    BCMFP_ACTION_VIEWTYPE_VXLAN         = 2,
    /*! GSH View */
    BCMFP_ACTION_VIEWTYPE_GSH           = 3,
    /*!
     * Should always be the last in the list.
     * Invalid view type.
 */
    BCMFP_ACTION_VIEWTYPE_COUNT         = 4
} bcmfp_action_viewtype_t;

/*!
 * \brief Action data
 */
typedef struct bcmfp_action_data_s {
    /*! Action */
    bcmfp_action_t action;

    /*! Associated param */
    uint32_t param;

    /*! Flags */
    bcmfp_action_flags_t flags;

    /*! Pointer to next action */
    struct bcmfp_action_data_s *next;
} bcmfp_action_data_t;
#endif /* BCMFP_ACTION_INTERNAL_H */
