/* \file cosq_int.h
 *
 * ltsw COSQ module internal header file.
 * Include structure definitions and function declarations used by COSQ module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BCMI_LTSW_COSQ_INT_H
#define BCMI_LTSW_COSQ_INT_H

#include <bcm/types.h>
#include <sal/sal_types.h>

/*
 * \brief HA subcomponent id for COSQ module.
 */
#define BCMINT_COSQ_WRED_SUB_COMP_ID 0
#define BCMINT_COSQ_PFC_DEADLOCK_SUB_COMP_ID 1
#define BCMINT_COSQ_CPU_LIMIT_SUB_COMP_ID 2
#define BCMINT_COSQ_CPU_SHARED_LIMIT_SUB_COMP_ID 3
#define BCMINT_COSQ_CPU_DYNAMIC_SUB_COMP_ID 4
#define BCMINT_COSQ_CPU_LIMIT_ENABLE_SUB_COMP_ID 5
#define BCMINT_COSQ_CPU_COLOR_LIMIT_SUB_COMP_ID 6
#define BCMINT_COSQ_EBST_ENABLE_SUB_COMP_ID 7
#define BCMINT_COSQ_EBST_SCAN_INTERVAL_SUB_COMP_ID 8
#define BCMINT_COSQ_MOD_PROFILE_BMP_SUB_COMP_ID 9

/* Queue average offset for congestion profile. */
#define COSQ_WRED_CNG_Q_AVG_OFFSET 0
/* Queue minimum offset for congestion profile. */
#define COSQ_WRED_CNG_Q_MIN_OFFSET 1
/* Service pool offset for congestion profile. */
#define COSQ_WRED_CNG_SERVICE_POOL_OFFSET 2
/*
 * Number of entries per congestion profile.
 *  | SP | Q_MIN | Q_AVG
 *   3  2|   1   |   0
 * SP : 00 Low congestion
 *      01 Medium congestion
 *      11 High congestion
 * Q_MIN and Q_AVG: 0 (no congestion) and 1 (congestion).
 */
#define COSQ_WRED_CNG_ENTRY_PER_PROFILE 12

/*
 * \brief The classifier_id used by bcm_cosq_classifier_* APIs
 *        contains two fields, a 6-bit type field, followed by
 *        a 26-bit value field.
 */
#define BCMINT_COSQ_CLASSIFIER_TYPE_FIELD     (1 << 2)
#define BCMINT_COSQ_CLASSIFIER_TYPE_SHIFT     26
#define BCMINT_COSQ_CLASSIFIER_FIELD_SHIFT    (0)
#define BCMINT_COSQ_CLASSIFIER_FIELD_MASK     (0x3ffffff)

#define BCMINT_COSQ_CLASSIFIER_IS_FIELD(_classifier)                   \
        (((_classifier) >> BCMINT_COSQ_CLASSIFIER_TYPE_SHIFT) ==       \
           BCMINT_COSQ_CLASSIFIER_TYPE_FIELD)

#define BCMINT_COSQ_CLASSIFIER_FIELD_SET(_classifier, _id)             \
            ((_classifier) = (BCMINT_COSQ_CLASSIFIER_TYPE_FIELD            \
                              << BCMINT_COSQ_CLASSIFIER_TYPE_SHIFT) |      \
                              (((_id) & BCMINT_COSQ_CLASSIFIER_FIELD_MASK) \
                              << BCMINT_COSQ_CLASSIFIER_FIELD_SHIFT))

#define BCMINT_COSQ_CLASSIFIER_FIELD_GET(_classifier)                  \
            (((_classifier) >> BCMINT_COSQ_CLASSIFIER_FIELD_SHIFT)         \
               & BCMINT_COSQ_CLASSIFIER_FIELD_MASK)

#define BCMINT_COSQ_CLASSIFIER_TYPE_FIELD_HI     (1 << 3)

#define BCMINT_COSQ_CLASSIFIER_IS_FIELD_HI(_classifier)                   \
        (((_classifier) >> BCMINT_COSQ_CLASSIFIER_TYPE_SHIFT) ==       \
           BCMINT_COSQ_CLASSIFIER_TYPE_FIELD_HI)

#define BCMINT_COSQ_CLASSIFIER_FIELD_HI_SET(_classifier, _id)             \
            ((_classifier) = (BCMINT_COSQ_CLASSIFIER_TYPE_FIELD_HI            \
                              << BCMINT_COSQ_CLASSIFIER_TYPE_SHIFT) |      \
                              (((_id) & BCMINT_COSQ_CLASSIFIER_FIELD_MASK) \
                              << BCMINT_COSQ_CLASSIFIER_FIELD_SHIFT))

#define BCMINT_COSQ_CLASSIFIER_FIELD_HI_GET(_classifier)                  \
            (((_classifier) >> BCMINT_COSQ_CLASSIFIER_FIELD_SHIFT)         \
               & BCMINT_COSQ_CLASSIFIER_FIELD_MASK)

/*
 * \brief TM_Q_ASSIGNMENT_INFO_ID_T type enum.
 */
typedef enum bcmint_tm_q_assignment_info_id_type_s {
    /* Enumeration for TM_UNICAST_VIEW. */
    BCMINT_TM_UNICAST_VIEW = 0,

    /* Enumeration for TM_NON_UNICAST_VIEW. */
    BCMINT_TM_NON_UNICAST_VIEW = 1,

    /* Enumeration for TM_IFP_UNICAST_VIEW. */
    BCMINT_TM_IFP_UNICAST_VIEW = 2,

    /* Enumeration for TM_IFP_NON_UNICAST_VIEW. */
    BCMINT_TM_IFP_NON_UNICAST_VIEW = 3,

    /* Enumeration for TM_DST_FP_UNICAST_VIEW. */
    BCMINT_TM_DST_FP_UNICAST_VIEW = 4,

    /* Enumeration for TM_DST_FP_NON_UNICAST_VIEW. */
    BCMINT_TM_DST_FP_NON_UNICAST_VIEW = 5,

    /* Enumeration for TM_IFP_UNICAST_HI_VIEW_INDEX. */
    BCMINT_TM_IFP_UNICAST_HI_VIEW = 6,

    /* Enumeration for TM_IFP_NON_UNICAST_HI_VIEW_INDEX. */
    BCMINT_TM_IFP_NON_UNICAST_HI_VIEW = 7,

    /* The last one, not valid. */
    BCMINT_TM_Q_ASSIGN_ID_TYPE_COUNT = 8

} bcmint_tm_q_assignment_info_id_type_t;

/*
 * \brief Cosq string map structure.
 */
typedef struct bcmint_cosq_str_map_s {

    /*! Index of the mapped object. */
    int idx;

    /*! Map string. */
    const char * str;

    /*! Profile id. */
    int profile_id;

    /*! base index. */
    int base_index;

    /*! Enable to use multicast COS from mirroring. */
    bool mc_cos_mirror;
} bcmint_cosq_str_map_t;

/*
 * \brief Cosq string map handle.
 */
/* TM_Q_ASSIGNMENT_INFO_ID_T type handle. */
#define BCMINT_TM_Q_ASSIGNMENT_INFO_ID_T 0

/*
 * \brief Cosq string map database structure.
 */
typedef struct bcmint_cosq_str_map_db_s {

    /* Cosq string map handle(BCMINT_L2_MAP_SH_XXX). */
    int hdl;

    /* Map info. */
    const bcmint_cosq_str_map_t *info;

    /* Num of elements in map info. */
    int num;

} bcmint_cosq_str_map_db_t;

/*
 * \brief Cosq map database structure.
 */
typedef struct bcmint_cosq_map_db_s {

    /*! String database. */
    const bcmint_cosq_str_map_db_t *str_db;

    /*! Num of elements in string database. */
    int str_num;

} bcmint_cosq_map_db_t;

#define TM_Q_ASSIGNMENT_INFO_MAX_POLICY 4

typedef struct bcmint_cosq_assignment_info_s {

    /* ipost_scratch_bus_unicast_flow[0:0], no overlay. */
    const char *unicast;

    /* ipost_scratch_bus_switch_to_cpu[0:0], no overlay. */
    const char *switchtocpu;

    /* number of policy indices. */
    uint32_t num_tm_q_assignment_profile_id;

    /* tm_q_assignment_policy logical table indices. */
    const char * tm_q_assignment_profile_id[TM_Q_ASSIGNMENT_INFO_MAX_POLICY];
} bcmint_cosq_assignment_info_t;

#endif /* !BCMI_LTSW_COSQ_INT_H */
