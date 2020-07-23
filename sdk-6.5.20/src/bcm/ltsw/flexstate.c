/*! \file flexstate.c
 *
 * Flex State Module Emulator above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bsl/bsl.h>
#include <sal/core/boot.h>

#include <bcm/flexstate.h>
#include <bcm_int/control.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/profile_mgmt.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/flexstate.h>
#include <bcm_int/ltsw/mbcm/flexstate.h>
#include <bcm_int/ltsw/lt_intf.h>

#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <sal/sal_mutex.h>
#include <bcmlt/bcmlt.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
* Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_FLEXSTATE

/*!
 * \brief Composition of return counter id.
 *
 * bits [ 7: 0] index   (maximum 256 indexes)
 * bits [12: 8] pipe    (maximum 32 pipes)
 * bits [15:13] stage   (ing_st0, ing_st1, egr_st)
 *
 * bits [30:28] type    (action, group action, quant, op profile(internal))
 */
#define FLEXSTATE_START_BIT_INDEX     0
#define FLEXSTATE_END_BIT_INDEX       7
#define FLEXSTATE_START_BIT_PIPE      8
#define FLEXSTATE_END_BIT_PIPE        12
#define FLEXSTATE_START_BIT_STAGE     13
#define FLEXSTATE_END_BIT_STAGE       15
#define FLEXSTATE_START_BIT_TYPE      28
#define FLEXSTATE_END_BIT_TYPE        30

#define FLEXSTATE_MASK_INDEX      \
    ((1 << (FLEXSTATE_END_BIT_INDEX - FLEXSTATE_START_BIT_INDEX  + 1)) - 1)
#define FLEXSTATE_MASK_PIPE       \
    ((1 << (FLEXSTATE_END_BIT_PIPE - FLEXSTATE_START_BIT_PIPE + 1)) - 1)
#define FLEXSTATE_MASK_STAGE      \
    ((1 << (FLEXSTATE_END_BIT_STAGE - FLEXSTATE_START_BIT_STAGE + 1)) - 1)
#define FLEXSTATE_MASK_TYPE       \
    ((1 << (FLEXSTATE_END_BIT_TYPE - FLEXSTATE_START_BIT_TYPE + 1)) - 1)

#define FLEXSTATE_ID_INDEX(_id)   \
    (((_id) >> FLEXSTATE_START_BIT_INDEX) & FLEXSTATE_MASK_INDEX)
#define FLEXSTATE_ID_PIPE(_id)    \
    (((_id) >> FLEXSTATE_START_BIT_PIPE) & FLEXSTATE_MASK_PIPE)
#define FLEXSTATE_ID_STAGE(_id)   \
    (((_id) >> FLEXSTATE_START_BIT_STAGE) & FLEXSTATE_MASK_STAGE)
#define FLEXSTATE_ID_TYPE(_id)    \
    (((_id) >> FLEXSTATE_START_BIT_TYPE) & FLEXSTATE_MASK_TYPE)

#define FS_ID_INVALID   BCMI_LTSW_FLEXSTATE_ACTION_INVALID

/*!
 * \brief Flex state key information.
 */
typedef struct ltsw_flexstate_key_s {

    /*! Identification. */
    int id;

    /*! Pipe number. */
    int pipe;

    /*! Stage id. */
    int stage;

#define FLEXSTATE_ACTION          1
#define FLEXSTATE_GRP_ACTION      2
#define FLEXSTATE_QUANT           3
#define FLEXSTATE_OP_PROFILE      4
    /*! Quant, Action, Group action. */
    int type;

} ltsw_flexstate_key_t;

#define FLEXSTATE_ID_COMPOSE(_pkey, _rid)                                           \
    {                                                                               \
        int _id = (_pkey)->id;                                                      \
        int _pipe = (_pkey)->pipe;                                                  \
        int _stage = (_pkey)->stage;                                                \
        int _type = (_pkey)->type;                                                  \
                                                                                    \
        (_rid) = ((((_id) & FLEXSTATE_MASK_INDEX) << FLEXSTATE_START_BIT_INDEX) |   \
                  (((_pipe) & FLEXSTATE_MASK_PIPE) << FLEXSTATE_START_BIT_PIPE) |   \
                  (((_stage) & FLEXSTATE_MASK_STAGE) << FLEXSTATE_START_BIT_STAGE) |\
                  (((_type) & FLEXSTATE_MASK_TYPE) << FLEXSTATE_START_BIT_TYPE));   \
    }

#define FLEXSTATE_ID_DECOMPOSE(_id, _pkey)                                              \
    {                                                                                   \
        (_pkey)->id = (((_id) >> FLEXSTATE_START_BIT_INDEX) & FLEXSTATE_MASK_INDEX);    \
        (_pkey)->pipe = (((_id) >> FLEXSTATE_START_BIT_PIPE) & FLEXSTATE_MASK_PIPE);    \
        (_pkey)->stage = (((_id) >> FLEXSTATE_START_BIT_STAGE) & FLEXSTATE_MASK_STAGE); \
        (_pkey)->type = (((_id) >> FLEXSTATE_START_BIT_TYPE) & FLEXSTATE_MASK_TYPE);    \
    }

#define FS_MIN_STAGE    BCMI_LTSW_FLEXSTATE_STAGE_ING_LKUP
#define FS_MAX_STAGE    BCMI_LTSW_FLEXSTATE_STAGE_COUNT
#define FS_MAX_PIPE     8
#define FS_MAX_OPOBJ    BCM_FLEXSTATE_OPERATION_OBJECT_SIZE

/*!
 * \brief Flex state enumeration map.
 *
 * This type is intended for mapping flex state enum values to
 * SDKLT field strings.
 */
typedef struct ltsw_flexstate_enum_map_s {

    /*! Enum name. */
    const char *name;

    /*! Enum value. */
    int val;

} ltsw_flexstate_enum_map_t;

/*! Flex state get operation. */
#define FLEXSTATE_OPERATION_GET                 (0)

/*! Flex state set operation. */
#define FLEXSTATE_OPERATION_SET                 (1)

/*! Flex state quantization maximum range. */
#define FS_QUANT_RANGE_MAX                      (8)

/*!
 * \brief Quantization information.
 */
typedef struct ltsw_flexstate_quant_s {

    /*! TRUE if quantization has been used. */
    bool used;

    /*! Reference counter. */
    int ref_count;

    /*! Options. */
    uint32_t options;

    /*! Quant object. */
    int object;

    /*! Quant min array. */
    uint32_t range_min[FS_QUANT_RANGE_MAX];

    /*! Quant max array. */
    uint32_t range_max[FS_QUANT_RANGE_MAX];

    /*! Quant range_num. */
    uint32_t range_num;

} ltsw_flexstate_quant_t;

/*!
 * \brief Flex state quantization information.
 */
typedef struct ltsw_flexstate_quant_info_s {

    /*! Min quantization id. */
    uint32_t quant_min[FS_MAX_STAGE];

    /*! Max quantization id. */
    uint32_t quant_max[FS_MAX_STAGE];

    /*! Quantization array. */
    ltsw_flexstate_quant_t *quants[FS_MAX_STAGE][FS_MAX_PIPE];

    /*! Protection mutex. */
    sal_mutex_t mutex;

} ltsw_flexstate_quant_info_t;

/*! Per chip flex state quantization information. */
static
ltsw_flexstate_quant_info_t *flexstate_quant_info[BCM_MAX_NUM_UNITS] = {0};

/*! Flex state quantization operation lock. */
#define FLEXSTATE_QUANT_LOCK(_u)                                          \
    sal_mutex_take(flexstate_quant_info[_u]->mutex, SAL_MUTEX_FOREVER)

/*! Flex state quantization operation unlock. */
#define FLEXSTATE_QUANT_UNLOCK(_u)                                        \
    sal_mutex_give(flexstate_quant_info[_u]->mutex)

/*! Quantization objects. */
#define QUANT_OBJ_ENUM_COUNT                    (4)

/*! Quantization objects enumeration. */
static const
ltsw_flexstate_enum_map_t quant_obj_enum[QUANT_OBJ_ENUM_COUNT] = {
    { USE_RANGE_CHK_1s, 0x0 },
    { USE_RANGE_CHK_2s, 0x1 },
    { USE_RANGE_CHK_3s, 0x2 },
    { USE_RANGE_CHK_4s, 0x3 },
};

/*! Number of counters per section. */
#define NUM_SECT_CTRS                           (128)

/*! Maximum table number supported in LTSW. */
#define FLEXSTATE_NUM_TBLS_MAX                  (32)

/*! Maximum pool section number supported in LTSW. */
#define FLEXSTATE_NUM_SECTION_MAX               (128)

/*!
 * \brief Flex state pool status.
 */
typedef struct ltsw_flexstate_pool_s {

    /*! Bitmap representing tables within the pool. */
    SHR_BITDCL used_by_tbls[SHRi_BITDCLSIZE(FLEXSTATE_NUM_TBLS_MAX)];

#define FS_POOL_NONE      0x0
#define FS_POOL_OUTPUT    0x1

    /*! Pool mode. */
    int mode;

    /*! Hint value. */
    int hint;

    /*! Number of action is using this pool. */
    size_t used_count;

    /*! Bitmap representing sections within the pool. */
    SHR_BITDCL inuse_bitmap[SHRi_BITDCLSIZE(FLEXSTATE_NUM_SECTION_MAX)];

} ltsw_flexstate_pool_t;

/*!
 * \brief Flex state pool info.
 */
typedef struct ltsw_flexstate_pool_info_s {

    /*! Number of pools. */
    size_t npools[FS_MAX_STAGE];

    /*! Number of sections. */
    size_t nsects[FS_MAX_STAGE];

    /*! Number of sections in each pool. */
    size_t nsects_in_pool[FS_MAX_STAGE];

    /*! Number of counters. */
    size_t nctrs[FS_MAX_STAGE];

    /*! Number of counters in each pool. */
    size_t nctrs_in_pool[FS_MAX_STAGE];

    /*! Number of counters in each section. */
    size_t nctrs_in_sect[FS_MAX_STAGE];

    /*! Pool array. */
    ltsw_flexstate_pool_t *pools[FS_MAX_STAGE][FS_MAX_PIPE];

    /*! Protection mutex. */
    sal_mutex_t mutex;

} ltsw_flexstate_pool_info_t;

/*! Per chip flex state pool information. */
static
ltsw_flexstate_pool_info_t *flexstate_pool_info[BCM_MAX_NUM_UNITS] = {0};

/*! Flex state pool operation lock. */
#define FLEXSTATE_POOL_LOCK(_u)                                           \
    sal_mutex_take(flexstate_pool_info[_u]->mutex, SAL_MUTEX_FOREVER)

/*! Flex state pool operation unlock. */
#define FLEXSTATE_POOL_UNLOCK(_u)                                         \
    sal_mutex_give(flexstate_pool_info[_u]->mutex)

/*! Flex state action objects count. */
#define FS_ACTION_OBJ_COUNT                 (4)

/*! Flex state action value A comparison. */
#define FS_VALUE_A_COMP                     (0)

/*! Flex state action value A true. */
#define FS_VALUE_A_TRUE                     (1)

/*! Flex state action value A false. */
#define FS_VALUE_A_FALSE                    (2)

/*! Flex state action value B comparison. */
#define FS_VALUE_B_COMP                     (3)

/*! Flex state action value B true. */
#define FS_VALUE_B_TRUE                     (4)

/*! Flex state action value B false. */
#define FS_VALUE_B_FALSE                    (5)

/*! Flex state action value count. */
#define FS_VALUE_COUNT                      (6)

/*! Flex state action index. */
#define FS_INDEX_OP                         (6)

/*! Flex state action value count. */
#define FS_OP_COUNT                         (7)

/*! Flex state action object count (1 index + 6 value). */
#define FS_ACTION_OPOBJ_COUNT               (FS_MAX_OPOBJ * FS_OP_COUNT)

/*!
 * \brief Action information.
 */
typedef struct ltsw_flexstate_action_s {

    /*! TRUE if action has been used. */
    bool used;

    /*! Reference counter. */
    int ref_count;

    /*! Associated group action id. */
    uint32_t group_action_id;

    /*! Reference counter for group action. */
    int grp_ref_count;

    /*! Action options. */
    uint32_t options;

    /*! Action object array. */
    int objects[FS_ACTION_OBJ_COUNT];

    /*! Allocated first counter pool. */
    uint32_t start_pool_idx;

    /*! Allocated last counter pool. */
    uint32_t end_pool_idx;

    /*! Allocated first counter section. */
    uint32_t start_section_idx;

    /*! Allocated last counter section. */
    uint32_t end_section_idx;

    /*! Index operation profile id. */
    uint32_t index_profile;

    /*! Value A comparison operation profile id. */
    uint32_t value_a_profile_comp;

    /*! Value A true operation profile id. */
    uint32_t value_a_profile;

    /*! Value A false operation profile id. */
    uint32_t value_a_profile_false;

    /*! Value B comparison operation profile id. */
    uint32_t value_b_profile_comp;

    /*! Value B true operation profile id. */
    uint32_t value_b_profile;

    /*! Value B false operation profile id. */
    uint32_t value_b_profile_false;

    /*! Stat counter number. */
    uint32_t index_num;

    /*! Action flags. */
    uint32_t flags;

    /*! Action source. */
    int source;

    /*! Action hint. */
    int hint;

    /*! Action drop count mode. */
    int drop_count_mode;

    /*! Action exception drop count enable. */
    int exception_drop_count_enable;

    /*! Action egress mirror count enable. */
    int egress_mirror_count_enable;

    /*! Action mode. */
    int mode;

    /*! Action object array. */
    int object[FS_ACTION_OPOBJ_COUNT];

    /*! Action quantization array. */
    uint32_t quant_id[FS_ACTION_OPOBJ_COUNT];

    /*! Action mask_size array. */
    uint32_t mask_size[FS_ACTION_OPOBJ_COUNT];

    /*! Action shift array. */
    uint32_t shift[FS_ACTION_OPOBJ_COUNT];

    /*! Action update compare a. */
    int update_compare_a;

    /*! Action update compare b. */
    int update_compare_b;

    /*! Action value select. */
    int value_select[FS_VALUE_COUNT];

    /*! Action value type. */
    int value_type[FS_VALUE_COUNT];

} ltsw_flexstate_action_t;

/*! Action mode enumeration count. */
#define ACTION_MODE_ENUM_COUNT              bcmFlexstateCounterModeCount

/*! Action mode enumeration. */
static const
ltsw_flexstate_enum_map_t action_mode_enum[ACTION_MODE_ENUM_COUNT] = {
    { NORMALs, 0 }, /* bcmFlexstateCounterModeNormal */
    { WIDEs, 1 },   /* bcmFlexstateCounterModeWide */
};

/*! Action source enumeration count. */
#define ACTION_SRC_ENUM_COUNT               bcmFlexstateValueSelectCount

/*! Action mode enumeration. */
static const
ltsw_flexstate_enum_map_t action_src_enum[ACTION_SRC_ENUM_COUNT] = {
    { NOOPs, 0 },               /* bcmFlexstateValueSelectCounterValue */
    { USE_PAIRED_COUNTERs, 1 }, /* bcmFlexstateValueSelectPairedCounter */
};

/*! Action update mode enumeration count. */
#define ACTION_UPDATE_MODE_ENUM_COUNT       bcmFlexstateValueOperationTypeCount

/*! Action update mode enumeration. */
static const
ltsw_flexstate_enum_map_t action_update_mode_enum[ACTION_UPDATE_MODE_ENUM_COUNT] = {
    { NOOPs, 0 },       /* bcmFlexstateValueOperationTypeNoop */
    { SETs, 1 },        /* bcmFlexstateValueOperationTypeSet */
    { INCs, 2 },        /* bcmFlexstateValueOperationTypeInc */
    { DECs, 3 },        /* bcmFlexstateValueOperationTypeDec */
    { MAXs, 4 },        /* bcmFlexstateValueOperationTypeMax */
    { MINs, 5 },        /* bcmFlexstateValueOperationTypeMin */
    { AVGs, 6 },        /* bcmFlexstateValueOperationTypeAverage */
    { SETBITs, 7 },     /* bcmFlexstateValueOperationTypeSetBit */
    { REV_DECs, 8 },    /* bcmFlexstateValueOperationTypeRdec */
    { XORs, 9 },        /* bcmFlexstateValueOperationTypeXor */
};

/*! Action drop enumeration count. */
#define ACTION_DROP_ENUM_COUNT              bcmFlexstateDropCountModeCount

/*! Action mode enumeration. */
static const
ltsw_flexstate_enum_map_t action_drop_enum[ACTION_DROP_ENUM_COUNT] = {
    { NO_DROP_COUNTs, 0 },      /* bcmFlexstateDropCountModeNoDrop */
    { DROP_COUNTs, 1 },         /* bcmFlexstateDropCountModeDrop */
    { ALWAYS_COUNTs, 2 },       /* bcmFlexstateDropCountModeAll */
};

/*! Action update comparison enumeration count. */
#define ACTION_UPDATE_COMP_ENUM_COUNT       bcmFlexstateUpdateCompareCount

/*! Action mode enumeration. */
static const
ltsw_flexstate_enum_map_t action_update_cmp_enum[ACTION_UPDATE_COMP_ENUM_COUNT] = {
    { ALWAYS_TRUEs, 0 },        /* bcmFlexstateUpdateCompareAlwaysTrue */
    { EQUALSs, 1 },             /* bcmFlexstateUpdateCompareEqual */
    { LESSs, 2 },               /* bcmFlexstateUpdateCompareLess */
};

/*! Action scale enumeration count. */
#define ACTION_SCALE_ENUM_COUNT             bcmFlexstateTriggerIntervalCount

/*! Action scale enumeration. */
static const
ltsw_flexstate_enum_map_t action_scale_enum[ACTION_SCALE_ENUM_COUNT] = {
    { SCALE_INFINITEs, 0 },     /* bcmFlexstateTriggerIntervalUnlimited */
    { SCALE_128_NSECs, 1 },     /* bcmFlexstateTriggerInterval100ns */
    { SCALE_512_NSECs, 2 },     /* bcmFlexstateTriggerInterval500ns */
    { SCALE_1024_NSECs, 3 },    /* bcmFlexstateTriggerInterval1us */
    { SCALE_8_USECs, 4 },       /* bcmFlexstateTriggerInterval10us */
    { SCALE_131_USECs, 5 },     /* bcmFlexstateTriggerInterval100us */
    { SCALE_1_MSECs, 6 },       /* bcmFlexstateTriggerInterval1ms */
    { SCALE_8_MSECs, 7 },       /* bcmFlexstateTriggerInterval10ms */
};

/*!
 * \brief Flex state action information.
 */
typedef struct ltsw_flexstate_action_info_s {

    /*! Min action profile id. */
    uint32_t profile_id_min[FS_MAX_STAGE];

    /*! Min action id. */
    uint32_t action_min[FS_MAX_STAGE];

    /*! Max action id. */
    uint32_t action_max[FS_MAX_STAGE];

    /*! Min op profile id. */
    uint32_t op_profile_min[FS_MAX_STAGE];

    /*! Max op profile id. */
    uint32_t op_profile_max[FS_MAX_STAGE];

    /*! Action array. */
    ltsw_flexstate_action_t *actions[FS_MAX_STAGE][FS_MAX_PIPE];

    /*! Protection mutex. */
    sal_mutex_t mutex;

} ltsw_flexstate_action_info_t;

/*! Per chip flex state action information. */
static
ltsw_flexstate_action_info_t *flexstate_action_info[BCM_MAX_NUM_UNITS] = {0};

/*! Flex state action operation lock. */
#define FLEXSTATE_ACTION_LOCK(_u)                                         \
    sal_mutex_take(flexstate_action_info[_u]->mutex, SAL_MUTEX_FOREVER)

/*! Flex state action operation unlock. */
#define FLEXSTATE_ACTION_UNLOCK(_u)                                       \
    sal_mutex_give(flexstate_action_info[_u]->mutex)

/*!
 * \brief Group action information.
 */
typedef struct ltsw_flexstate_grp_action_s {

    /*! TRUE if action has been used. */
    bool used;

    /*! Reference counter. */
    int ref_count;

    /*! Parent action id. */
    uint32_t parent_act_id;

    /*! Action options. */
    uint32_t options;

    /*! Group action source. */
    int source;

    /*! Group action action_id array. */
    uint32_t action_id[BCM_FLEXSTATE_GROUP_ACTION_SIZE];

    /*! Group action number. */
    uint32_t action_num;

} ltsw_flexstate_grp_action_t;

/*!
 * \brief Flex state group action information.
 */
typedef struct ltsw_flexstate_grp_action_info_s {

    /*! Min group action id. */
    uint32_t grp_action_min[FS_MAX_STAGE];

    /*! Max group action id. */
    uint32_t grp_action_max[FS_MAX_STAGE];

    /*! Maximum group map size. */
    uint32_t grp_size[FS_MAX_STAGE];

    /*! Action array. */
    ltsw_flexstate_grp_action_t *grp_actions[FS_MAX_STAGE][FS_MAX_PIPE];

    /*! Protection mutex. */
    sal_mutex_t mutex;

} ltsw_flexstate_grp_action_info_t;

/*! Per chip flex state action information. */
static
ltsw_flexstate_grp_action_info_t *flexstate_grp_action_info[BCM_MAX_NUM_UNITS] = {0};

/*! Flex state action operation lock. */
#define FLEXSTATE_GRP_ACTION_LOCK(_u)     \
    sal_mutex_take(flexstate_grp_action_info[_u]->mutex, SAL_MUTEX_FOREVER)

/*! Flex state action operation unlock. */
#define FLEXSTATE_GRP_ACTION_UNLOCK(_u)   \
    sal_mutex_give(flexstate_grp_action_info[_u]->mutex)

/*! Flex state invalid object. */
#define FLEXSTATE_OBJ_INVALID               (-1)

/*!
 * \brief Operation profile information.
 */
typedef struct ltsw_flexstate_operation_profile_s {

    /*! CTR_EFLEX_OBJ_SRC_T. */
    const char *op_objects[FS_MAX_OPOBJ];

    /*! Profile object mask size. */
    uint8_t mask_size[FS_MAX_OPOBJ];

    /*! Profile object shift. */
    uint8_t shift[FS_MAX_OPOBJ];

    /*! Profile objects. */
    int objects[FS_MAX_OPOBJ];

    /*! Quantization id. */
    uint32_t quant_id[FS_MAX_OPOBJ];

    /*! Copy of action object array. */
    int action_objects[FS_ACTION_OBJ_COUNT];

    /*! Flex state key identification. */
    ltsw_flexstate_key_t key;

} ltsw_flexstate_operation_profile_t;

/*! Minimum mask number. */
#define FLEXSTATE_OP_MASK_MIN               1

/*! Maximum mask number. */
#define FLEXSTATE_OP_MASK_MAX               16

/*! Maximum shift number. */
#define FLEXSTATE_OP_SHIFT_MAX              16

/*! Operand profile object enumeration count. */
#define OP_OBJ_ENUM_COUNT                   (4)

/*! Operand profile object enumeration. */
static const
ltsw_flexstate_enum_map_t op_obj_enum[OP_OBJ_ENUM_COUNT] = {
    { USE_OBJ_1s, 0x0 },
    { USE_OBJ_2s, 0x1 },
    { USE_OBJ_3s, 0x2 },
    { USE_OBJ_4s, 0x3 },
};

/*! Operand profile object fields. */
static const
char *op_obj_flds[FS_MAX_OPOBJ] = {
    OBJ_1s,
    OBJ_2s,
};

/*! Operand profile maske size fields. */
static const
char *op_mask_size_flds[FS_MAX_OPOBJ] = {
    MASK_SIZE_1s,
    MASK_SIZE_2s,
};

/*! Operand profile shift fields. */
static const
char *op_shift_flds[FS_MAX_OPOBJ] = {
    SHIFT_1s,
    SHIFT_2s,
};

/*! Per chip maximum pipe number. */
static int flexstate_pipe_max[BCM_MAX_NUM_UNITS][FS_MAX_STAGE] = {{0}};

static bcmint_flexstate_stage_info_t *st_info[BCM_MAX_NUM_UNITS];

/*! Per chip flex state initialize flag. */
static bool
flexstate_initialized[BCM_MAX_NUM_UNITS] = {0};

/******************************************************************************
 * Private functions
 */

/*!
 * \breif Encode enum value to string.
 *
 * Encode enum value to string.
 *
 * \param [in] emap Enum map array.
 * \param [in] count Enum map array count.
 * \param [in] evalue Enum value to be resolved.
 * \param [out] sym Resolved symbol string.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_enum_encode(const ltsw_flexstate_enum_map_t *emap,
                           const uint32_t count, int eval, const char **sym)
{
    int rv = SHR_E_NONE;
    uint32_t i;

    for (i = 0; i < count; i++) {
        if (eval == emap[i].val) {
            /* Resolved. */
            *sym = emap[i].name;
            break;
        }
    }

    if (i == count) {
        /* Evalue not found. */
        rv = SHR_E_NOT_FOUND;
    }

    return rv;
}

/*!
 * \breif Decode enum string to value.
 *
 * Decode enum string to value.
 *
 * \param [in] emap Enum map array.
 * \param [in] count Enum map array count.
 * \param [in] sym Symbol string to be resolved.
 * \param [out] evalue Resolved enum value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_enum_decode(const ltsw_flexstate_enum_map_t *emap,
                           const uint32_t count, const char *sym, int *eval)
{
    int rv = SHR_E_NONE;
    uint32_t i;

    *eval = -1;

    for (i = 0; i < count; i++) {
        if (!sal_strcmp(sym, emap[i].name)) {
            /* Resolved. */
            *eval = emap[i].val;
            break;
        }
    }

    if (i == count) {
        /* Evalue not found. */
        rv = SHR_E_NOT_FOUND;
    }

    return rv;
}

/*!
 * \brief Help function to convert a key to logical table index.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex state key identification.
 * \param [out] id Logical table profile id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_key_to_id(int unit, ltsw_flexstate_key_t *key, uint64_t *id)
{
    int type = key->type;
    int max_pipe = bcmi_ltsw_dev_max_pp_pipe_num(unit);
    uint32_t min_pid, min_id, max_id;
    uint64_t per_pipe, per_inst;

    SHR_FUNC_ENTER(unit);

    switch (type) {
        case FLEXSTATE_ACTION:
            min_pid = flexstate_action_info[unit]->profile_id_min[key->stage];
            min_id = flexstate_action_info[unit]->action_min[key->stage];
            max_id = flexstate_action_info[unit]->action_max[key->stage];
            break;

        case FLEXSTATE_GRP_ACTION:
            min_pid = 1;
            min_id = flexstate_grp_action_info[unit]->grp_action_min[key->stage];
            max_id = flexstate_grp_action_info[unit]->grp_action_max[key->stage];
            break;

        case FLEXSTATE_QUANT:
            min_pid = 0;
            min_id = flexstate_quant_info[unit]->quant_min[key->stage];
            max_id = flexstate_quant_info[unit]->quant_max[key->stage];
            break;

        case FLEXSTATE_OP_PROFILE:
            min_pid = 0;
            min_id = flexstate_action_info[unit]->op_profile_min[key->stage];
            max_id = flexstate_action_info[unit]->op_profile_max[key->stage];
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    per_pipe = max_id - min_id;

    /* ST_LKUP & ST_FWD share same LT id scope... */
    if (key->stage == BCMI_LTSW_FLEXSTATE_STAGE_ING_FWD) {
        per_inst = per_pipe * max_pipe;
        *id = per_inst + per_pipe * key->pipe + key->id - min_id + min_pid;
    } else {
        *id = per_pipe * key->pipe + key->id - min_id + min_pid;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Help function to convert ports to pipe.
 *
 * \param [in] unit Unit number.
 * \param [in] ports Port bitmap.
 * \param [out] opipe Pipe id.
 */
static int
ltsw_flexstate_config_port_to_pipe(int unit, bcm_pbmp_t ports, int *opipe)
{
    int pipe = 0, num_pipe;
    bcm_port_config_t port_config;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_port_config_get(unit, &port_config));

    num_pipe = bcmi_ltsw_dev_max_pp_pipe_num(unit);
    for (pipe = 0; pipe < num_pipe; pipe++) {
        if (BCM_PBMP_EQ(ports, port_config.per_pp_pipe[pipe])) {
            break;
        }
    }

    if (pipe >= num_pipe) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid ports in pipe unique mode")));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (opipe) {
        *opipe = pipe;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Help function to convert pipe bitmap to pipe.
 *
 * \param [in] unit Unit number.
 * \param [in] ports Port bitmap.
 * \param [out] opipe Pipe id.
 */
static int
ltsw_flexstate_config_pipe_bitmap_get(int unit, bcm_pbmp_t pipes, int *opipe)
{
    int pipe = 0, num_pipe;

    SHR_FUNC_ENTER(unit);

    num_pipe = bcmi_ltsw_dev_max_pp_pipe_num(unit);
    for (pipe = 0; pipe < num_pipe; pipe++) {
        if (BCM_PBMP_MEMBER(pipes, pipe)) {
            break;
        }
    }

    if (pipe >= num_pipe) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid ports in pipe unique mode")));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (opipe) {
        *opipe = pipe;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Help function to validate logical table/field name.
 *
 * This function retrieves the map state of the specified
 * LT field on a particular device.
 *
 * \param [in] unit Unit number.
 * \param [in] table_name Logic table name.
 * \param [in] field_name Logic table field name.
 * \param [out] valid TRUE/FALSE.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_lt_field_validation(int unit, const char *table_name,
                                   const char *field_name, bool *valid)
{
    int rv = SHR_E_NONE;
    uint32_t ltid;
    uint32_t lfid;

    SHR_FUNC_ENTER(unit);

    rv = bcmlt_table_field_id_get_by_name(unit, table_name, field_name,
                                          &ltid, &lfid);
    if (rv == SHR_E_NOT_FOUND) {
        *valid = FALSE;
        rv = SHR_E_NONE;
    } else if (rv == SHR_E_NONE) {
        *valid = TRUE;
    }
    SHR_IF_ERR_VERBOSE_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state quantization update function.
 *
 * This function is used to insert or update a quantization LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex state key identification.
 * \param [in] quant_cfg Flex state quantization data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_quantization_update(int unit, ltsw_flexstate_key_t *key,
                                   bcm_flexstate_quantization_t *quant_cfg)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmint_flexstate_object_info_t *oi = NULL;
    bcmint_flexstate_stage_info_t *st = &(st_info[unit][key->stage]);
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t data, data_arr[1];
    uint32_t i, count = COUNTOF(data_arr);
    const char *symbol_arr[1];
    bool instance_valid = FALSE;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Quantization lt update %d %d %u\n"),
              key->stage, key->pipe, key->id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->range_chk_profile, &eh));

    /* Check INSTANCE field. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_lt_field_validation
             (unit, st->range_chk_profile, INSTANCEs, &instance_valid));

    for (i = 0; i < FS_QUANT_RANGE_MAX; i++) {
        /* Key fields. */
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_key_to_id(unit, key, &data));
        data = data * FS_QUANT_RANGE_MAX + i;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, st->range_chk_profile_id, data));

        /* Data fields. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PIPEs, key->pipe));

        if (instance_valid) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(eh, INSTANCEs, st->inst));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, OBJ_SELECTs, key->id));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, RANGE_CHECKERs, i));

        if (i < quant_cfg->range_num) {
            data = (uint64_t)quant_cfg->range_check_min[i];
        } else {
            /* Make invalid range checker MIN bigger than MAX. */
            data = 1;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, MINs, data));

        if (i < quant_cfg->range_num) {
            data = (uint64_t)quant_cfg->range_check_max[i];
        } else {
            data = 0;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, MAXs, data));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_clear(eh));
    }

    (void)bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->obj_quant_cfg, &eh));

    /* Key fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PIPEs, key->pipe));

    /* Check INSTANCE field. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_lt_field_validation
             (unit, st->obj_quant_cfg, INSTANCEs, &instance_valid));
    if (instance_valid) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, INSTANCEs, st->inst));
    }

    /* Data fields. */
    data_arr[0] = TRUE;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_add(eh, QUANTIZEs, key->id, data_arr, count));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexstate_object_info_get(unit, quant_cfg->object, &oi));

    symbol_arr[0] = oi->symbol;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_symbol_add(eh, OBJs, key->id,
                                            symbol_arr, count));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(eh));

    (void)bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state quantization destroy function.
 *
 * This function is used to delete a quantization LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex state key identification.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_quantization_delete(int unit, ltsw_flexstate_key_t *key)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmint_flexstate_stage_info_t *st = &(st_info[unit][key->stage]);
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t data, data_arr[1];
    uint32_t i, count = COUNTOF(data_arr);
    bool instance_valid = FALSE;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Quantization lt delete %d %d %u\n"),
              key->stage, key->pipe, key->id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->range_chk_profile, &eh));

    for (i = 0; i < FS_QUANT_RANGE_MAX; i++) {
        /* Key fields. */
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_key_to_id(unit, key, &data));
        data = data * FS_QUANT_RANGE_MAX + i;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, st->range_chk_profile_id, data));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                                  BCMLT_PRIORITY_NORMAL));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_clear(eh));
    }

    (void)bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;


    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->obj_quant_cfg, &eh));

    /* Key fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PIPEs, key->pipe));

    /* Check INSTANCE field. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_lt_field_validation
             (unit, st->obj_quant_cfg, INSTANCEs, &instance_valid));
    if (instance_valid) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, INSTANCEs, st->inst));
    }

    /* Data fields. */
    data_arr[0] = FALSE;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_add(eh, QUANTIZEs, key->id, data_arr, count));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(eh));

    (void)bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup flex state quantization software database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_quantization_db_cleanup(int unit)
{
    ltsw_flexstate_quant_info_t *qi = flexstate_quant_info[unit];

    SHR_FUNC_ENTER(unit);

    if (qi == NULL) {
        SHR_EXIT();
    }

    if (qi->mutex) {
        sal_mutex_destroy(qi->mutex);
    }

    SHR_FREE(qi);
    flexstate_quant_info[unit] = NULL;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize flex state quantization software database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_quantization_db_init(int unit)
{
    ltsw_flexstate_quant_info_t *qi = flexstate_quant_info[unit];
    uint64_t min, max;
    uint32_t ha_alloc_size = 0, ha_req_size = 0, pipe_req_size = 0;
    void *alloc_ptr = NULL;
    int warm = bcmi_warmboot_get(unit), stage, max_pipe, i;

    SHR_FUNC_ENTER(unit);

    if (qi != NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    } else {
        SHR_ALLOC(qi, sizeof(ltsw_flexstate_quant_info_t), "ltswFlexstateQuantInfo");
        SHR_NULL_CHECK(qi, SHR_E_MEMORY);
        sal_memset(qi, 0, sizeof(ltsw_flexstate_quant_info_t));
        flexstate_quant_info[unit] = qi;
    }

    /* Create mutex. */
    qi->mutex = sal_mutex_create("ltswFlexstateQuantInfo");
    SHR_NULL_CHECK(qi->mutex , SHR_E_MEMORY);

    /* Alloc quant buffer from HA. */
    for (stage = FS_MIN_STAGE; stage < FS_MAX_STAGE; stage++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, st_info[unit][stage].range_chk_profile,
                                           OBJ_SELECTs , &min, &max));

        qi->quant_min[stage] = (uint32_t)min;
        qi->quant_max[stage] = (uint32_t)max + 1;

        max_pipe = flexstate_pipe_max[unit][stage];

        pipe_req_size = sizeof(ltsw_flexstate_quant_t) * qi->quant_max[stage];
        ha_req_size = pipe_req_size * max_pipe;
        ha_alloc_size = ha_req_size;
        alloc_ptr = bcmi_ltsw_ha_mem_alloc(unit, BCMI_HA_COMP_ID_FLEXSTATE,
                                           st_info[unit][stage].ha_quant_id,
                                           "bcmFlexStateQuant",
                                           &ha_alloc_size);
        SHR_NULL_CHECK(alloc_ptr, SHR_E_MEMORY);

        if (ha_alloc_size < ha_req_size) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }

        if (!warm) {
            sal_memset(alloc_ptr, 0, ha_alloc_size);
        }

        for (i = 0; i < max_pipe; i++) {
            qi->quants[stage][i] =
                (ltsw_flexstate_quant_t *)(alloc_ptr + pipe_req_size * i);
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        for (stage = FS_MIN_STAGE; stage < FS_MAX_STAGE; stage++) {
            if (qi->quants[stage][0] && !warm) {
                (void)bcmi_ltsw_ha_mem_free(unit, qi->quants[stage][0]);
            }
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state quantization parameter validation function.
 *
 * This function is used to validate quantization parameters.
 *
 * \param [in] unit Unit number.
 * \param [in] options BCM_FLEXSTATE_OPTIONS_XXX options.
 * \param [in] quant_cfg Flex state quantization data structure.
 * \param [out] key Flex state key.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_quantization_params_validate(int unit, int options,
                                            bcm_flexstate_quantization_t *quant_cfg,
                                            ltsw_flexstate_key_t *key)
{
    int pipe = 0;
    bcmint_flexstate_object_info_t *oi = NULL;
    uint32_t i, min, max, last_max = 0;

    SHR_FUNC_ENTER(unit);

    /* Check quantization object. */
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexstate_object_info_get(unit, quant_cfg->object, &oi));

    if ((oi->obj_type != FlexstateObjectTypeMenu)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid quantization object %d\n"),
                   oi->object));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (options & BCMI_LTSW_FLEXSTATE_OPTIONS_PIPE_BMP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_config_pipe_bitmap_get(unit, quant_cfg->ports,
                                                   &pipe));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_config_port_to_pipe(unit, quant_cfg->ports, &pipe));
    }

    if (quant_cfg->range_num <= 0 ||
        quant_cfg->range_num > FS_QUANT_RANGE_MAX) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Check if the range is overlay. */
    for (i = 0; i < quant_cfg->range_num; i++) {
        min = quant_cfg->range_check_min[i];
        max = quant_cfg->range_check_max[i];

        if (min > max) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (i != 0 && last_max >=min) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        last_max = max;
    }

    sal_memset(key, 0, sizeof(ltsw_flexstate_key_t));

    key->pipe = pipe;
    key->stage = oi->stage;
    key->type = FLEXSTATE_QUANT;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state quantization creation function.
 *
 * This function is used to create a flex state quantization.
 *
 * \param [in] unit Unit number.
 * \param [in] options BCM_FLEXSTATE_OPTIONS_XXX options.
 * \param [in] quant_cfg Flex state quantization data structure.
 * \param [in/out] quant_id Flex state quantization ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_quantization_create(int unit, int options,
                                   bcm_flexstate_quantization_t *quant_cfg,
                                   uint32_t *quant_id)
{
    ltsw_flexstate_quant_info_t *qi = flexstate_quant_info[unit];
    ltsw_flexstate_quant_t *quant = NULL;
    ltsw_flexstate_key_t nkey = {0}, okey = {0};
    uint32_t min_id, max_id;
    int i;

    SHR_FUNC_ENTER(unit);

    FLEXSTATE_QUANT_LOCK(unit);

    SHR_NULL_CHECK(quant_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(quant_id, SHR_E_PARAM);

    /* Parameter check. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_quantization_params_validate(unit, options, quant_cfg,
                                                     &nkey));

    min_id = qi->quant_min[nkey.stage];
    max_id = qi->quant_max[nkey.stage];

    if (options & BCM_FLEXSTATE_OPTIONS_REPLACE) {
        FLEXSTATE_ID_DECOMPOSE(*quant_id, &okey);
        nkey.id = okey.id;
        if ((okey.pipe != nkey.pipe) ||
            (okey.stage != nkey.stage) ||
            (okey.type != nkey.type) ||
            (okey.id < min_id || okey.id >= max_id)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid quantization id %x\n"),
                       *quant_id));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        /* Search existing quantization. */
        quant = &(qi->quants[nkey.stage][nkey.pipe][nkey.id]);
        if (!quant->used) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Quantization id %x not found\n"),
                       *quant_id));
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    } else if (options & BCM_FLEXSTATE_OPTIONS_WITH_ID) {
        nkey.id = FLEXSTATE_ID_INDEX(*quant_id);
        if (nkey.id < min_id || nkey.id >= max_id) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid quantization id %x "
                                        "valid id range %d - %d\n"),
                       FLEXSTATE_ID_INDEX(*quant_id), min_id, max_id - 1));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        /* Search existing quantization. */
        quant = &(qi->quants[nkey.stage][nkey.pipe][nkey.id]);
        if (quant->used) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Quantization id %x exists\n"),
                       *quant_id));
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
    } else {
        for (nkey.id = 0; nkey.id < max_id; nkey.id++) {
            quant = &(qi->quants[nkey.stage][nkey.pipe][nkey.id]);
            if (!quant->used) {
                break;
            }
        }

        if (nkey.id >= max_id) {
            SHR_ERR_EXIT(SHR_E_FULL);
        }
    }

    /* Quantization LT update. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_quantization_update(unit, &nkey, quant_cfg));

    quant->used = TRUE;
    quant->options = options;
    quant->ref_count = 0;

    /* Bookkeeping API configuration. */
    quant->object = quant_cfg->object;
    quant->range_num = quant_cfg->range_num;

    for (i = 0; i < quant_cfg->range_num; i++) {
        quant->range_min[i] = quant_cfg->range_check_min[i];
        quant->range_max[i] = quant_cfg->range_check_max[i];
    }

    /* Compose quantization id. */
    FLEXSTATE_ID_COMPOSE(&nkey, *quant_id);

exit:
    FLEXSTATE_QUANT_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state quant id to quant function.
 *
 * This function is used to get a quant from a quant_id.
 *
 * \param [in] unit Unit number.
 * \param [in] quant_id Flex state quantization ID.
 * \param [out] quant Flex state quant.
 * \param [out] okey Flex state key identification.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_quant_id_to_quant(int unit,
                                 uint32_t quant_id,
                                 ltsw_flexstate_quant_t **quant,
                                 ltsw_flexstate_key_t *okey)
{
    ltsw_flexstate_quant_info_t *qi = flexstate_quant_info[unit];
    ltsw_flexstate_quant_t *tmp_quant = NULL;
    ltsw_flexstate_key_t key = {0};
    uint32_t min_id, max_id;

    SHR_FUNC_ENTER(unit);

    FLEXSTATE_ID_DECOMPOSE(quant_id, &key);

    if (key.type != FLEXSTATE_QUANT) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid quantization id %x\n"), quant_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id type %x\n"), key.type));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((key.stage < FS_MIN_STAGE) || (key.stage >= FS_MAX_STAGE)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid quantization id %x\n"), quant_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id stage %x\n"), key.stage));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (key.pipe < 0 || key.pipe >= flexstate_pipe_max[unit][key.stage]) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid quantization id %x\n"), quant_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id pipe %x\n"), key.pipe));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    min_id = qi->quant_min[key.stage];
    max_id = qi->quant_max[key.stage];
    if (key.id < min_id || key.id >= max_id) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid quantization id %x\n"), quant_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id %x\n"), key.id));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    tmp_quant = &(qi->quants[key.stage][key.pipe][key.id]);
    if (!tmp_quant->used) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Quantization id %x not found\n"), quant_id));

        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (quant) {
        *quant = tmp_quant;
    }

    if (okey) {
        sal_memcpy(okey, &key, sizeof(ltsw_flexstate_key_t));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state quantization destroy function.
 *
 * This function is used to destroy a flex state quantization.
 *
 * \param [in] unit Unit number.
 * \param [in] quant_id Flex state quantization ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_quantization_destroy(int unit, uint32_t quant_id)
{
    ltsw_flexstate_quant_t *quant= NULL;
    ltsw_flexstate_key_t key = {0};

    SHR_FUNC_ENTER(unit);

    FLEXSTATE_QUANT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_quant_id_to_quant(unit, quant_id, &quant, &key));

    if (quant->ref_count != 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Quantization id %x is in using\n"),
                   quant_id));
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_quantization_delete(unit, &key));

    /* Reset internal quantization structure. */
    sal_memset(quant, 0, sizeof(ltsw_flexstate_quant_t));

exit:
    FLEXSTATE_QUANT_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state quantization get function.
 *
 * This function is used to get a flex state quantization information.
 *
 * \param [in] unit Unit number.
 * \param [in] quant_id Flex state quantization ID.
 * \param [out] quant_cfg Flex state quantization data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_quantization_get(int unit,
                                uint32_t quant_id,
                                bcm_flexstate_quantization_t *quant_cfg)
{
    ltsw_flexstate_key_t key = {0};
    ltsw_flexstate_quant_t *quant = NULL;
    bcm_port_config_t port_config;
    int i;

    SHR_FUNC_ENTER(unit);

    FLEXSTATE_QUANT_LOCK(unit);

    SHR_NULL_CHECK(quant_cfg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_quant_id_to_quant(unit, quant_id, &quant, &key));

    sal_memset(quant_cfg, 0, sizeof(bcm_flexstate_quantization_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_port_config_get(unit, &port_config));
    BCM_PBMP_ASSIGN(quant_cfg->ports, port_config.per_pp_pipe[key.pipe]);

    quant_cfg->object = quant->object;
    quant_cfg->range_num = quant->range_num;

    for (i = 0; i < quant->range_num; i++) {
        quant_cfg->range_check_min[i] = quant->range_min[i];
        quant_cfg->range_check_max[i] = quant->range_max[i];
    }

exit:
    FLEXSTATE_QUANT_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state quantization traverse function.
 *
 * This function is used to traverse all flex state quantization
 * information.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn Quantization traverse callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
ltsw_flexstate_quantization_traverse(int unit,
                                     bcm_flexstate_quantization_traverse_cb trav_fn,
                                     void *user_data)
{
    ltsw_flexstate_quant_info_t *qi = flexstate_quant_info[unit];
    ltsw_flexstate_quant_t *quant = NULL;
    ltsw_flexstate_key_t key = {0};
    bcm_flexstate_quantization_t quant_cfg = {0};
    uint32_t min_id, max_id;
    int max_pipe, quant_id, rv;

    SHR_FUNC_ENTER(unit);

    FLEXSTATE_QUANT_LOCK(unit);

    SHR_NULL_CHECK(trav_fn, SHR_E_PARAM);

    key.type = FLEXSTATE_QUANT;
    for (key.stage = FS_MIN_STAGE; key.stage < FS_MAX_STAGE; key.stage++) {
        max_pipe = flexstate_pipe_max[unit][key.stage];
        min_id = qi->quant_min[key.stage];
        max_id = qi->quant_max[key.stage];

        for (key.pipe = 0; key.pipe < max_pipe; key.pipe++) {
            for (key.id = min_id; key.id < max_id; key.id++) {
                quant = &(qi->quants[key.stage][key.pipe][key.id]);

                if (!quant->used) {
                    continue;
                }

                /* Skip internal quants. */
                if (quant->options & BCMI_LTSW_FLEXSTATE_OPTIONS_INTERNAL) {
                    continue;
                }

                FLEXSTATE_ID_COMPOSE(&key, quant_id);

                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexstate_quantization_get(unit, quant_id, &quant_cfg));

                rv = trav_fn(unit, quant_id, &quant_cfg, user_data);
                if (SHR_FAILURE(rv)) {
                    break;
                }
            }
        }
    }

exit:
    FLEXSTATE_QUANT_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup flex state pool software database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_pool_db_cleanup(int unit)
{
    ltsw_flexstate_pool_info_t *pi = flexstate_pool_info[unit];

    SHR_FUNC_ENTER(unit);

    if (pi == NULL) {
        SHR_EXIT();
    }

    if (pi->mutex) {
        sal_mutex_destroy(pi->mutex);
    }

    SHR_FREE(pi);
    flexstate_pool_info[unit] = NULL;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize flex state pool software database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_pool_db_init(int unit)
{
    ltsw_flexstate_pool_info_t *pi = flexstate_pool_info[unit];
    uint64_t min, max;
    uint32_t ha_alloc_size = 0, ha_req_size = 0, pipe_req_size = 0;
    void *alloc_ptr = NULL;
    int warm = bcmi_warmboot_get(unit), stage, max_pipe, i;

    SHR_FUNC_ENTER(unit);

    if (pi != NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    } else {
        SHR_ALLOC(pi, sizeof(ltsw_flexstate_pool_info_t), "ltswFlexstatePoolInfo");
        SHR_NULL_CHECK(pi, SHR_E_MEMORY);
        sal_memset(pi, 0, sizeof(ltsw_flexstate_pool_info_t));
        flexstate_pool_info[unit] = pi;
    }

    /* Create mutex. */
    pi->mutex = sal_mutex_create("ltswFlexstatePoolInfo");
    SHR_NULL_CHECK(pi->mutex, SHR_E_MEMORY);

    /* Alloc pool buffer from HA. */
    for (stage = FS_MIN_STAGE; stage < FS_MAX_STAGE; stage++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, st_info[unit][stage].act_profile,
                                           POOL_IDs, &min, &max));
        pi->npools[stage] = (size_t)max + 1;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, st_info[unit][stage].act_profile,
                                           BASE_INDEXs, &min, &max));
        pi->nsects_in_pool[stage] = (size_t)max + 1;

        pi->nsects[stage] = pi->npools[stage] * pi->nsects_in_pool[stage];
        pi->nctrs_in_sect[stage] = NUM_SECT_CTRS;
        pi->nctrs_in_pool[stage] = NUM_SECT_CTRS * pi->nsects_in_pool[stage];
        pi->nctrs[stage] = pi->npools[stage] * pi->nctrs_in_pool[stage];

        max_pipe = flexstate_pipe_max[unit][stage];

        pipe_req_size = sizeof(ltsw_flexstate_pool_t) * pi->npools[stage];
        ha_req_size = pipe_req_size * max_pipe;
        ha_alloc_size = ha_req_size;
        alloc_ptr = bcmi_ltsw_ha_mem_alloc(unit, BCMI_HA_COMP_ID_FLEXSTATE,
                                           st_info[unit][stage].ha_pool_id,
                                           "bcmFlexStatePool",
                                           &ha_alloc_size);
        SHR_NULL_CHECK(alloc_ptr, SHR_E_MEMORY);

        if (ha_alloc_size < ha_req_size) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }

        if (!warm) {
            sal_memset(alloc_ptr, 0, ha_alloc_size);
        }

        for (i = 0; i < max_pipe; i++) {
            pi->pools[stage][i] =
                (ltsw_flexstate_pool_t *)(alloc_ptr + pipe_req_size * i);
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        for (stage = FS_MIN_STAGE; stage < FS_MAX_STAGE; stage++) {
            if (pi->pools[stage][0] && !warm) {
                (void)bcmi_ltsw_ha_mem_free(unit, pi->pools[stage][0]);
            }
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \breif Allocate pool and counter resources for a flex state action.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex state key identification.
 * \param [in] act_cfg Flex state action data structure.
 * \param [out] action Internal flex state action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_pool_alloc(int unit,
                          ltsw_flexstate_key_t *key,
                          bcm_flexstate_action_t *act_cfg,
                          ltsw_flexstate_action_t *action)
{
    ltsw_flexstate_pool_info_t *pi = flexstate_pool_info[unit];
    ltsw_flexstate_pool_t *pool = NULL;
    bcmint_flexstate_source_info_t *si = NULL;
    int pidx, mode;
    uint32_t tbl_bit, num_ctr_req;
    uint32_t num_pools = 0, start_pool_idx = 0, end_pool_idx = 0;
    uint32_t num_sects_pool = 0, start_section_idx = 0, end_section_idx = 0;
    uint32_t num_ctr_section = 0;
    uint32_t num_section_req = 0, section_cnt = 0, section_cnt_rem = 0;
    uint32_t cur_pool_idx = 0, cur_section_idx = 0;
    uint32_t start_bit = 0, num_bits = 0;
    bool done = FALSE;

    SHR_FUNC_ENTER(unit);

    num_pools = pi->npools[key->stage];
    num_sects_pool = pi->nsects_in_pool[key->stage];
    num_ctr_section = pi->nctrs_in_sect[key->stage];

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexstate_source_info_get(unit, act_cfg->source, &si));

    tbl_bit = si->tbl_bit;

    num_ctr_req = act_cfg->index_num;

    num_section_req = (num_ctr_req + num_ctr_section - 1) / num_ctr_section;

    FLEXSTATE_POOL_LOCK(unit);

    if (act_cfg->flags & BCM_FLEXSTATE_ACTION_FLAGS_HINT_OUTPUT) {
        start_pool_idx = act_cfg->hint;
        mode = FS_POOL_OUTPUT;
    } else {
        start_pool_idx = 0;
        mode = FS_POOL_NONE;
    }

    while (start_pool_idx < num_pools) {
        /* Reset. */
        cur_pool_idx = start_pool_idx;
        cur_section_idx = 0;
        section_cnt = 0;
        section_cnt_rem = 0;
        done = FALSE;

        pidx = cur_pool_idx;
        pool = &(pi->pools[key->stage][key->pipe][pidx]);

        if (!SHR_BITNULL_RANGE(pool->used_by_tbls, 0, FLEXSTATE_NUM_TBLS_MAX)) {
            if ((pool->mode != mode) || (pool->hint != act_cfg->hint) ||
                !SHR_BITGET(pool->used_by_tbls, tbl_bit)) {

                if (mode == FS_POOL_OUTPUT) {
                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "Bad pool %d for output\n"),
                                 cur_pool_idx));
                    break;
                } else {
                    /* Pool has been consumed, start from next pool. */
                    start_pool_idx = cur_pool_idx + 1;

                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "Bad pool %d, next %d\n"),
                                 cur_pool_idx, start_pool_idx));
                    continue;
                }
            }
        }

        /* In given pool, determine the start of free sections. */
        for (cur_section_idx = 0;
             cur_section_idx < num_sects_pool;
             cur_section_idx++) {
            if (!SHR_BITGET(pool->inuse_bitmap, cur_section_idx)) {
                section_cnt++;
            } else {
                /* Find next free section in current pool. */
                section_cnt = 0;
                continue;
            }

            /* Requested number of counters are available in the given pool. */
            if (section_cnt == num_section_req) {
                end_pool_idx = cur_pool_idx;
                start_section_idx = cur_section_idx + 1 - num_section_req;
                end_section_idx = cur_section_idx;
                done = TRUE;
                break;
            }
        }

        /* Done check for hint output. */
        if (mode == FS_POOL_OUTPUT && !done) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Pool %d exceeded for output\n"),
                         cur_pool_idx));
            break;
        }

        if (!done) {
            /* Here section_cnt represents the number of
             * free sections at the end of the start pool.
             * Start from next pool.
             */
            if (!section_cnt) {
                start_pool_idx = cur_pool_idx + 1;

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "No free counters in "
                                              "pool %d next %d\n"),
                             cur_pool_idx, start_pool_idx));
                continue;
            }

            start_section_idx = num_sects_pool - section_cnt;

            /* Calculate remaining number of required sections. */
            section_cnt_rem = num_section_req - section_cnt;
        }

        /* Determine if number of counters are available
         * in the next consecutive pool(s).
         */
        while (!done) {
            section_cnt = 0;

            /* Move to the subsequent pool. */
            cur_pool_idx++;

            /* No more pool resource. continue with the max_pool_num. */
            if (cur_pool_idx >= num_pools) {
                start_pool_idx = num_pools;

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Max pool reached %d\n"),
                             start_pool_idx));
                break;
            }

            pidx = cur_pool_idx;
            pool = &(pi->pools[key->stage][key->pipe][pidx]);

            if (!SHR_BITNULL_RANGE(pool->used_by_tbls, 0,
                                   FLEXSTATE_NUM_TBLS_MAX)) {
                if ((pool->mode != mode) || (pool->hint != act_cfg->hint) ||
                    !SHR_BITGET(pool->used_by_tbls, tbl_bit)) {
                    /* Pool has been consumed, start from next pool. */
                    start_pool_idx = cur_pool_idx + 1;

                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "Bad pool %d, next %d\n"),
                                 cur_pool_idx, start_pool_idx));
                    break;
                }
            }

            /* Check for free sections from the beginning of the pool. */
            for (cur_section_idx = 0;
                 cur_section_idx < num_sects_pool;
                 cur_section_idx++) {
                if (!SHR_BITGET(pool->inuse_bitmap, cur_section_idx)) {
                    section_cnt++;

                    /* Enough sections. */
                    if (section_cnt >= section_cnt_rem) {
                        break;
                    }
                } else {
                    /* No more continuous sections, start from current pool. */
                    start_pool_idx = cur_pool_idx;

                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "Bad pool %d, next %d\n"),
                                 cur_pool_idx, start_pool_idx));
                    break;
                }
            }

            if (section_cnt >= section_cnt_rem) {
                end_pool_idx = cur_pool_idx;
                end_section_idx = cur_section_idx;
                done = TRUE;
            } else {
                /* Update remaining number of required sections. */
                section_cnt_rem -= section_cnt;
            }
        } /* end WHILE. */

        if (done) {
            /* All counters in the same pool. */
            if (start_pool_idx == end_pool_idx) {
                /* Set bits in current pool. */
                start_bit = start_section_idx;
                num_bits = num_section_req;
                SHR_BITSET_RANGE(pool->inuse_bitmap, start_bit, num_bits);

                /* Set table bit in current pool. */
                SHR_BITSET(pool->used_by_tbls, tbl_bit);

                pool->mode = mode;

                /* Set hint in current pool. */
                pool->hint = act_cfg->hint;

                pool->used_count++;
            } else {
                /* Counters are spread across multiple pools */
                cur_pool_idx = start_pool_idx;

                while (cur_pool_idx <= end_pool_idx) {
                    pidx = cur_pool_idx;
                    pool = &(pi->pools[key->stage][key->pipe][pidx]);

                    /* Calculate start bit and number of bits in the pool */
                    if (cur_pool_idx == start_pool_idx) {
                        start_bit = start_section_idx;
                        num_bits = num_sects_pool - start_section_idx;
                    } else if (cur_pool_idx == end_pool_idx) {
                        start_bit = 0;
                        num_bits = end_section_idx + 1;
                    } else {
                        start_bit = 0;
                        num_bits = num_sects_pool;
                    }

                    /* Set bits in current pool */
                    SHR_BITSET_RANGE(pool->inuse_bitmap, start_bit, num_bits);

                    /* Set table bit in current pool. */
                    SHR_BITSET(pool->used_by_tbls, tbl_bit);

                    pool->mode = mode;

                    /* Set hint in current pool. */
                    pool->hint = act_cfg->hint;

                    pool->used_count++;

                    cur_pool_idx++;
                }
            }
            break;
        }
    } /* end WHILE */

    FLEXSTATE_POOL_UNLOCK(unit);

    if (done) {
        action->start_pool_idx = start_pool_idx;
        action->end_pool_idx = end_pool_idx;
        action->start_section_idx = start_section_idx;
        action->end_section_idx = end_section_idx;

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Pool assignment success for %s\n"
                                      "Pool mode %d hint %d used_cnt %d\n"
                                      "Pool %d - %d "
                                      "Start sect %d End sect %d\n"),
                     si->table, pool->mode, pool->hint, (int)(pool->used_count),
                     start_pool_idx, end_pool_idx,
                     start_section_idx, end_section_idx));
    } else {
        SHR_ERR_EXIT(SHR_E_RESOURCE);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \breif Free pool and counter resources for a flex state action.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex state key identification.
 * \param [in] action Internal flex state action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_pool_free(int unit,
                         ltsw_flexstate_key_t *key,
                         ltsw_flexstate_action_t *action)
{
    ltsw_flexstate_pool_info_t *pi = flexstate_pool_info[unit];
    ltsw_flexstate_pool_t *pool = NULL;
    bcmint_flexstate_source_info_t *si = NULL;
    int pidx;
    uint32_t tbl_bit;
    uint32_t num_sects_pool;
    uint32_t start_pool_idx = 0, end_pool_idx = 0;
    uint32_t start_section_idx = 0, end_section_idx = 0;
    uint32_t cur_pool_idx = 0;
    uint32_t start_bit = 0, num_bits = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexstate_source_info_get(unit, action->source, &si));

    tbl_bit = si->tbl_bit;

    num_sects_pool = pi->nsects_in_pool[key->stage];
    start_pool_idx = action->start_pool_idx;
    end_pool_idx = action->end_pool_idx;
    start_section_idx = action->start_section_idx;
    end_section_idx = action->end_section_idx;
    cur_pool_idx = start_pool_idx;

    FLEXSTATE_POOL_LOCK(unit);

    /* All counters in the same pool. */
    if (start_pool_idx == end_pool_idx) {
        pidx = cur_pool_idx;
        pool = &(pi->pools[key->stage][key->pipe][pidx]);

        /* Clear bits in current pool. */
        start_bit = start_section_idx;
        num_bits = end_section_idx - start_section_idx + 1;
        SHR_BITCLR_RANGE(pool->inuse_bitmap, start_bit, num_bits);

        pool->used_count--;

        /* Clear table bit in current pool. */
        if (!pool->used_count) {
            SHR_BITCLR(pool->used_by_tbls, tbl_bit);
            pool->mode = FS_POOL_NONE;
            pool->hint = 0;
        }

    } else {
        /* Counters are spread across multiple pools */

        while (cur_pool_idx <= end_pool_idx) {
            pidx = cur_pool_idx;
            pool = &(pi->pools[key->stage][key->pipe][pidx]);

            /* Calculate start bit and number of bits in the pool */
            if (cur_pool_idx == start_pool_idx) {
                start_bit = start_section_idx;
                num_bits = num_sects_pool - start_section_idx;
            } else if (cur_pool_idx == end_pool_idx) {
                start_bit = 0;
                num_bits = end_section_idx + 1;
            } else {
                start_bit = 0;
                num_bits = num_sects_pool;
            }

            /* Clear bits in current pool */
            SHR_BITCLR_RANGE(pool->inuse_bitmap, start_bit, num_bits);

            pool->used_count--;

            /* Clear table bit in current pool. */
            if (!pool->used_count) {
                SHR_BITCLR(pool->used_by_tbls, tbl_bit);
                pool->mode = FS_POOL_NONE;
                pool->hint = 0;
            }

            cur_pool_idx++;
        }
    }

    FLEXSTATE_POOL_UNLOCK(unit);

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert an entry into OP_PROFILE LT.
 *
 * This function is used to insert an entry into OP_PROFILE LT.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex state key identification.
 * \param [in] profile Operation profile entry.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_op_profile_insert(int unit,
                                 ltsw_flexstate_key_t *key,
                                 ltsw_flexstate_operation_profile_t *profile)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmint_flexstate_stage_info_t *st = &(st_info[unit][key->stage]);
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int i;
    uint64_t data;
    const char *symbol = NULL;
    bool instance_valid = FALSE;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Operation profile lt insert %d %d %u\n"),
              key->stage, key->pipe, key->id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->operand_profile, &eh));

    /* Key fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_key_to_id(unit, key, &data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->operand_profile_id, data));

    /* Data fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PROFILEs, key->id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PIPEs, key->pipe));

    /* Check INSTANCE field. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_lt_field_validation
             (unit, st->operand_profile, INSTANCEs, &instance_valid));
    if (instance_valid) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, INSTANCEs, st->inst));
    }

    for (i = 0; i < FS_MAX_OPOBJ; i++) {
        symbol = profile->op_objects[i];
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, op_obj_flds[i], symbol));

        data = (uint64_t)profile->mask_size[i] - 1;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, op_mask_size_flds[i], data));

        data = (uint64_t)profile->shift[i];
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, op_shift_flds[i], data));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(eh));

    (void)bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Delete an entry from OP_PROFILE LT.
 *
 * This function is used to delete an entry from OP_PROFILE LT.
 *
 * \param [in] unit Unit Number.
 * \param [in] key Flex state key identification.
 * \param [in] index OP_PROFILE entry index.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_op_profile_delete(int unit, ltsw_flexstate_key_t *key)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmint_flexstate_stage_info_t *st = &(st_info[unit][key->stage]);
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t data;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Operation profile lt delete %d %d %u\n"),
              key->stage, key->pipe, key->id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->operand_profile, &eh));

    /* Key fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_key_to_id(unit, key, &data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->operand_profile_id, data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(eh));

    (void)bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Lookup an entry from OP_PROFILE LT.
 *
 * This function is used to lookup an entry from OP_PROFILE LT.
 *
 * \param [in] unit Unit Number.
 * \param [in] key Flex state key identification.
 * \param [out] profile Operation profile entry.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_op_profile_lookup(int unit,
                                 ltsw_flexstate_key_t *key,
                                 ltsw_flexstate_operation_profile_t *profile)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmint_flexstate_stage_info_t *st = &(st_info[unit][key->stage]);
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint32_t i;
    uint64_t data;
    const char *symbol = NULL;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Operation profile lt lookup %d %d %u\n"),
              key->stage, key->pipe, key->id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->operand_profile, &eh));

    /* Key fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_key_to_id(unit, key, &data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->operand_profile_id, data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Data fields. */
    for (i = 0; i < FS_MAX_OPOBJ; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_get(eh, op_obj_flds[i], &symbol));
        profile->op_objects[i] = symbol;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, op_mask_size_flds[i], &data));
        profile->mask_size[i] = (uint8_t)data + 1;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, op_shift_flds[i], &data));
        profile->shift[i] = (uint8_t)data;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(eh));

    (void)bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Compare profile set of operation profile.
 *
 * This function is used to compare profile set of operation profile.
 *
 * \param [in]  unit Unit Number.
 * \param [in]  entries Operation profile.
 * \param [in]  entries_per_set Number of entries in the operation profile set.
 * \param [in]  index Entry index of profile table.
 * \param [out] cmp Result of comparison.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_op_profile_entry_cmp_cb(int unit,
                                       void *entries,
                                       int entries_per_set,
                                       int index,
                                       int *cmp)
{
    ltsw_flexstate_operation_profile_t cur_profile, *profile = NULL;
    ltsw_flexstate_key_t key = {0};
    bcmint_flexstate_object_info_t *oi = NULL;
    size_t copy_sz = sizeof(int) * FS_ACTION_OBJ_COUNT;
    int i, array_idx = -1, id;
    const char *object = NULL;
    int tmp_objects[FS_ACTION_OBJ_COUNT] = {0}, action_object;

    SHR_FUNC_ENTER(unit);

    *cmp = 1;

    profile = (ltsw_flexstate_operation_profile_t *)entries;
    SHR_NULL_CHECK(profile, SHR_E_PARAM);

    sal_memcpy(&key, &(profile->key), sizeof(ltsw_flexstate_key_t));
    key.id = index;

    if (entries_per_set != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memcpy(tmp_objects, profile->action_objects, copy_sz);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_op_profile_lookup(unit, &key, &cur_profile));

    for (i = 0; i < FS_MAX_OPOBJ; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexstate_object_info_get(unit, profile->objects[i], &oi));

        object = oi->symbol;

        /* Convert op object to array index. */
        (void)(ltsw_flexstate_enum_decode(op_obj_enum,
                                          OP_OBJ_ENUM_COUNT,
                                          cur_profile.op_objects[i],
                                          &array_idx));

        if ((oi->obj_type == FlexstateObjectTypeMenu) && (array_idx != -1)) {
            /* Both are NPL objects. */
            action_object = tmp_objects[array_idx];

            /* Action object has been occupied and not same. */
            if ((action_object != FLEXSTATE_OBJ_INVALID) &&
                (action_object != (int)(profile->objects[i]))) {
                break;
            }

            tmp_objects[array_idx] = (int)(profile->objects[i]);
        } else {
            /* Non-NPL object and not same. */
            if (oi->obj_type == FlexstateObjectTypeQuant) {
                id = FLEXSTATE_ID_INDEX(profile->quant_id[i]);

                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexstate_enum_encode(quant_obj_enum,
                                                QUANT_OBJ_ENUM_COUNT,
                                                id,
                                                &object));
            }

            if (sal_strcmp(object, cur_profile.op_objects[i])) {
                break;
            }
        }

        /* Mask size and shift should always be matched. */
        if ((cur_profile.mask_size[i] != profile->mask_size[i]) ||
            (cur_profile.shift[i] != profile->shift[i])) {
            break;
        }
    }

    /* Suitable op profile. */
    if (i >= FS_MAX_OPOBJ) {
        /* Update action object array. */
        sal_memcpy(profile->action_objects, tmp_objects, copy_sz);
        *cmp = 0;
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Cleanup the flex state operation profile.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_op_profile_entry_cleanup(int unit)
{
    int stage, pipe;

    SHR_FUNC_ENTER(unit);

    for (stage = FS_MIN_STAGE; stage < FS_MAX_STAGE; stage++) {
        for (pipe = 0; pipe < flexstate_pipe_max[unit][stage]; pipe++) {
            SHR_IF_ERR_EXIT_EXCEPT_IF
                (bcmi_ltsw_profile_unregister(unit, st_info[unit][stage].ph[pipe]),
                 SHR_E_NOT_FOUND);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Initialize the flex state operation profile.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_op_profile_entry_init(int unit)
{
    ltsw_flexstate_action_info_t *ai = flexstate_action_info[unit];
    ltsw_flexstate_action_t *action = NULL;
    uint64_t idx_min = 0, idx_max = 0;
    int ent_idx_min = 0, ent_idx_max = 0, entries_per_set = 1;
    int warm = bcmi_warmboot_get(unit);
    int stage, pipe, i;
    ltsw_flexstate_key_t key = {0};

    SHR_FUNC_ENTER(unit);

    for (stage = FS_MIN_STAGE; stage < FS_MAX_STAGE; stage++) {
        SHR_IF_ERR_EXIT
            (bcmi_lt_field_value_range_get(unit, st_info[unit][stage].operand_profile,
                                           st_info[unit][stage].operand_profile_id,
                                           &idx_min, &idx_max));
        ent_idx_min = (int)idx_min;
        ent_idx_max = (int)idx_max;

        for (pipe = 0; pipe < flexstate_pipe_max[unit][stage]; pipe++) {
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_profile_register(unit, &(st_info[unit][stage].ph[pipe]),
                                            &ent_idx_min, &ent_idx_max,
                                            entries_per_set, NULL,
                                            ltsw_flexstate_op_profile_entry_cmp_cb));

            if (warm) {
                for (i = 0; i < ai->action_max[stage]; i++) {
                    action = &(ai->actions[stage][pipe][i]);

                    if (!action->used) {
                        continue;
                    }

                    FLEXSTATE_ID_DECOMPOSE(action->index_profile, &key);
                    if ((key.type == FLEXSTATE_OP_PROFILE)) {
                        /* increment reference count */
                        SHR_IF_ERR_EXIT
                            (bcmi_ltsw_profile_ref_count_increase(
                                                unit,
                                                st_info[unit][stage].ph[pipe],
                                                entries_per_set,
                                                key.id, 1));
                    }

                    FLEXSTATE_ID_DECOMPOSE(action->value_a_profile_comp, &key);
                    if ((key.type == FLEXSTATE_OP_PROFILE)) {
                        /* increment reference count */
                        SHR_IF_ERR_EXIT
                            (bcmi_ltsw_profile_ref_count_increase(
                                                unit,
                                                st_info[unit][stage].ph[pipe],
                                                entries_per_set,
                                                key.id, 1));
                    }

                    FLEXSTATE_ID_DECOMPOSE(action->value_a_profile, &key);
                    if ((key.type == FLEXSTATE_OP_PROFILE)) {
                        /* increment reference count */
                        SHR_IF_ERR_EXIT
                            (bcmi_ltsw_profile_ref_count_increase(
                                                unit,
                                                st_info[unit][stage].ph[pipe],
                                                entries_per_set,
                                                key.id, 1));
                    }

                    FLEXSTATE_ID_DECOMPOSE(action->value_a_profile_false, &key);
                    if ((key.type == FLEXSTATE_OP_PROFILE)) {
                        /* increment reference count */
                        SHR_IF_ERR_EXIT
                            (bcmi_ltsw_profile_ref_count_increase(
                                                unit,
                                                st_info[unit][stage].ph[pipe],
                                                entries_per_set,
                                                key.id, 1));
                    }

                    FLEXSTATE_ID_DECOMPOSE(action->value_b_profile_comp, &key);
                    if ((key.type == FLEXSTATE_OP_PROFILE)) {
                        /* increment reference count */
                        SHR_IF_ERR_EXIT
                            (bcmi_ltsw_profile_ref_count_increase(
                                                unit,
                                                st_info[unit][stage].ph[pipe],
                                                entries_per_set,
                                                key.id, 1));
                    }

                    FLEXSTATE_ID_DECOMPOSE(action->value_b_profile, &key);
                    if ((key.type == FLEXSTATE_OP_PROFILE)) {
                        /* increment reference count */
                        SHR_IF_ERR_EXIT
                            (bcmi_ltsw_profile_ref_count_increase(
                                                unit,
                                                st_info[unit][stage].ph[pipe],
                                                entries_per_set,
                                                key.id, 1));
                    }

                    FLEXSTATE_ID_DECOMPOSE(action->value_b_profile_false, &key);
                    if ((key.type == FLEXSTATE_QUANT)) {
                        /* increment reference count */
                        SHR_IF_ERR_EXIT
                            (bcmi_ltsw_profile_ref_count_increase(
                                                unit,
                                                st_info[unit][stage].ph[pipe],
                                                entries_per_set,
                                                key.id, 1));
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an entry into operation profile.
 *
 * This function is used to add an entry into operation profile.
 *
 * \param [in] unit Unit number.
 * \param [in] profile Operation profile entry.
 * \param [out] profile_id Profile entry index.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_op_profile_entry_add(int unit,
                                  ltsw_flexstate_operation_profile_t *profile,
                                  uint32_t *profile_id)
{
    ltsw_flexstate_key_t *key = &(profile->key);
    bcmint_flexstate_object_info_t *oi = NULL;
    int rv, entries_per_set = 1, i, array_idx, j, id;
    int action_object;
    bool free_profile = FALSE;

    SHR_FUNC_ENTER(unit);

    rv = bcmi_ltsw_profile_index_allocate(unit, st_info[unit][key->stage].ph[key->pipe],
                                          profile, 0, entries_per_set,
                                          &(key->id));
    if (SHR_FAILURE(rv)) {
        /* Get an existing entry and return the profile index directly. */
        if (rv == SHR_E_EXISTS) {
            FLEXSTATE_ID_COMPOSE(key, *profile_id);

            SHR_EXIT();
        } else {
            SHR_ERR_EXIT(rv);
        }
    }

    free_profile = TRUE;

    /* New profile, need to update op profile lt.*/
    for (i = 0; i < FS_MAX_OPOBJ; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexstate_object_info_get(unit, profile->objects[i], &oi));

        if (oi->obj_type == FlexstateObjectTypeMenu) {
            /* NPL object needs to occupy one action object. */
            array_idx = -1;
            for (j = 0; j < FS_ACTION_OBJ_COUNT; j++) {
                action_object = profile->action_objects[j];

                if (action_object == FLEXSTATE_OBJ_INVALID) {
                    /* Record the first free action object. */
                    if (array_idx == -1) {
                        array_idx = j;
                    }
                    continue;
                }

                /* Reuse the existing action object. */
                if (action_object == (int)(profile->objects[i])) {
                    array_idx = j;
                    break;
                }
            }

            if (array_idx == -1) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "No suitable action object\n")));
                SHR_ERR_EXIT(SHR_E_INTERNAL);
            } else {
                /* Update action object symbol array and operation value. */
                profile->action_objects[array_idx] = (int)(profile->objects[i]);

                /* Convert array index to op object. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexstate_enum_encode(op_obj_enum,
                                              OP_OBJ_ENUM_COUNT,
                                              array_idx,
                                              &(profile->op_objects[i])));
            }
        } else if (oi->obj_type == FlexstateObjectTypeQuant) {
            id = FLEXSTATE_ID_INDEX(profile->quant_id[i]);

            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_enum_encode(quant_obj_enum, QUANT_OBJ_ENUM_COUNT,
                                            id, &(profile->op_objects[i])));

        } else {
            /* Other objects. */
            profile->op_objects[i] = oi->symbol;
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_op_profile_insert(unit, key, profile));

    FLEXSTATE_ID_COMPOSE(key, *profile_id);

exit:
    if (SHR_FUNC_ERR() && free_profile == TRUE) {
        (void)bcmi_ltsw_profile_index_free(unit, st_info[unit][key->stage].ph[key->pipe],
                                           key->id);
    }

    SHR_FUNC_EXIT();
}

/*
 * \brief Remove an entry from operation profile.
 *
 * This function is used to remove an entry from operation profile.
 *
 * \param [in] unit Unit Number.
 * \param [out] profile_id Profile entry index.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_op_profile_entry_remove(int unit, uint32_t profile_id)
{
    ltsw_flexstate_key_t key = {0};
    int rv;

    SHR_FUNC_ENTER(unit);

    FLEXSTATE_ID_DECOMPOSE(profile_id, &key);

    if (key.type != FLEXSTATE_OP_PROFILE) {
        LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Invalid profile id %x\n"), profile_id));
        SHR_EXIT();
    }

    rv = bcmi_ltsw_profile_index_free(unit, st_info[unit][key.stage].ph[key.pipe],
                                      key.id);

    if (SHR_FAILURE(rv)) {
        /* Profile index is still used by others and return directly. */
        if (rv == SHR_E_BUSY) {
            SHR_EXIT();
        } else {
            SHR_ERR_EXIT(rv);
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_op_profile_delete(unit, &key));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief EFLEX_TRIGGER LT operation.
 *
 * This function is used to update/insert or delete an entry from
 * EFLEX_TRIGGER LT.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex state key identification.
 * \param [in] trigger Flex state trigger data structure.
 * \param [in] opcode LT operation code.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_action_trigger_op(int unit,
                               ltsw_flexstate_key_t *key,
                               bcm_flexstate_trigger_t *trigger,
                               bcmlt_opcode_t opcode)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmint_flexstate_stage_info_t *st = &(st_info[unit][key->stage]);
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int evalue;
    uint64_t data;
    const char *symbol = NULL, *stop = NULL;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Counter trigger lt op %d %d %d %u\n"),
              opcode, key->stage, key->pipe, key->id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->trigger, &eh));

    /* Key fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_key_to_id(unit, key, &data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->act_profile_id, data));

    if (opcode == BCMLT_OPCODE_DELETE  || opcode == BCMLT_OPCODE_LOOKUP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, eh, opcode, BCMLT_PRIORITY_NORMAL));

        if (opcode == BCMLT_OPCODE_LOOKUP) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_get(eh, STARTs, &symbol));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_get(eh, STOPs, &stop));

            if(!sal_strcmp(TIMEs, symbol)) {
                trigger->trigger_type = bcmFlexstateTriggerTypeTime;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(eh, NUM_PERIODs, &data));
                trigger->period_num = (uint8_t)data;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_symbol_get(eh, SCALEs, &symbol));
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexstate_enum_decode(action_scale_enum,
                                              ACTION_SCALE_ENUM_COUNT,
                                              symbol, &evalue));
                trigger->interval = (bcm_flexstate_trigger_interval_t)evalue;
            } else if (!sal_strcmp(CONDITIONs, symbol) &&
                       !sal_strcmp(PERIODs, stop)) {
                trigger->trigger_type =
                    bcmFlexstateTriggerTypeConditionStartTimeStop;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(eh, START_VALUEs, &data));
                trigger->object_value_start = (uint16_t)data;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(eh, COND_MASKs, &data));
                trigger->object_value_mask = (uint16_t)data;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(eh, NUM_PERIODs, &data));
                trigger->period_num = (uint8_t)data;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_symbol_get(eh, SCALEs, &symbol));
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexstate_enum_decode(action_scale_enum,
                                              ACTION_SCALE_ENUM_COUNT,
                                              symbol, &evalue));
                trigger->interval = (bcm_flexstate_trigger_interval_t)evalue;
            } else {
                trigger->trigger_type = bcmFlexstateTriggerTypeCondition;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(eh, START_VALUEs, &data));
                trigger->object_value_start = (uint16_t)data;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(eh, STOP_VALUEs, &data));
                trigger->object_value_stop = (uint16_t)data;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(eh, COND_MASKs, &data));
                trigger->object_value_mask = (uint16_t)data;
            }
        }
    } else {
        /* Data fields. */
        data = FALSE;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TRIGGERs, data));

        if (trigger->trigger_type == bcmFlexstateTriggerTypeTime) {
            symbol = TIMEs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(eh, STARTs, symbol));

            symbol = PERIODs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(eh, STOPs, symbol));

            data = 0;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, START_TIME_OFFSETs, data));

            data = trigger->period_num;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, NUM_PERIODs, data));

            evalue = (int)trigger->interval;
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_enum_encode(action_scale_enum,
                                          ACTION_SCALE_ENUM_COUNT,
                                          evalue, &symbol));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(eh, SCALEs, symbol));

        } else if (trigger->trigger_type ==
                   bcmFlexstateTriggerTypeConditionStartTimeStop) {
            symbol = CONDITIONs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(eh, STARTs, symbol));

            symbol = PERIODs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(eh, STOPs, symbol));

            data = trigger->object_value_start;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, START_VALUEs, data));

            data = trigger->object_value_mask;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, COND_MASKs, data));

            data = trigger->period_num;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, NUM_PERIODs, data));

            evalue = (int)trigger->interval;
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_enum_encode(action_scale_enum,
                                          ACTION_SCALE_ENUM_COUNT,
                                          evalue, &symbol));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(eh, SCALEs, symbol));
        } else {
            symbol = CONDITIONs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(eh, STARTs, symbol));

            symbol = CONDITIONs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(eh, STOPs, symbol));

            data = trigger->object_value_start;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, START_VALUEs, data));

            data = trigger->object_value_stop;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, STOP_VALUEs, data));

            data = trigger->object_value_mask;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, COND_MASKs, data));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(eh));

    (void)bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert an entry from EFLEX_ACTION_PROFILE LT.
 * This function is used to insert an entry from
 * CTR_ING/EGR_EFLEX_ACTION_PROFILE LT.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex state key identification.
 * \param [in] action Internal flex state action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_action_insert(int unit,
                             ltsw_flexstate_key_t *key,
                             ltsw_flexstate_action_t *action)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmint_flexstate_stage_info_t *st = &(st_info[unit][key->stage]);
    bcmint_flexstate_object_info_t *oi = NULL;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int evalue;
    uint64_t data;
    const char *symbol = NULL;
    int i;
    ltsw_flexstate_key_t pkey;
    bool instance_valid = FALSE;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Counter action lt insert %d %d %u\n"),
              key->stage, key->pipe, key->id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->act_profile, &eh));

    /* Key fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_key_to_id(unit, key, &data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->act_profile_id, data));

    /* Data fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, ACTIONs, key->id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PIPEs, key->pipe));

    /* Check INSTANCE field. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_lt_field_validation
             (unit, st->act_profile, INSTANCEs, &instance_valid));
    if (instance_valid) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, INSTANCEs, st->inst));
    }

    data = (uint64_t)(action->start_pool_idx);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, POOL_IDs, data));

    data = FALSE;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, BASE_INDEX_AUTOs, data));

    data = (uint64_t)(action->start_section_idx);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, BASE_INDEXs, data));

    data = action->index_num;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, NUM_COUNTERSs, data));

    evalue = action->mode;
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_enum_encode(action_mode_enum, ACTION_MODE_ENUM_COUNT,
                                    evalue, &symbol));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, MODEs, symbol));

    for (i = 0; i < FS_ACTION_OBJ_COUNT; i++) {
        if (action->objects[i] != FLEXSTATE_OBJ_INVALID) {
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexstate_object_info_get(unit, action->objects[i], &oi));

            symbol = oi->symbol;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_symbol_add(eh, OBJs, i, &symbol, 1));
        }
    }

    FLEXSTATE_ID_DECOMPOSE(action->index_profile, &pkey);
    if (pkey.type == FLEXSTATE_OP_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_key_to_id(unit, &pkey, &data));
    } else {
        data = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->act_index, data));

    evalue = action->update_compare_a;
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_enum_encode(action_update_cmp_enum,
                                    ACTION_UPDATE_COMP_ENUM_COUNT,
                                    evalue, &symbol));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, UPDATE_COMPARE_As, symbol));

    /* Comparison A. */
    evalue = action->value_select[FS_VALUE_A_COMP];
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_enum_encode(action_src_enum, ACTION_SRC_ENUM_COUNT,
                                    evalue, &symbol));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, CTR_SRC_COMP_As, symbol));

    FLEXSTATE_ID_DECOMPOSE(action->value_a_profile_comp, &pkey);
    if (pkey.type == FLEXSTATE_OP_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_key_to_id(unit, &pkey, &data));
    } else {
        data = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->act_value_a_comp, data));

    /* Valute A true. */
    evalue = action->value_select[FS_VALUE_A_TRUE];
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_enum_encode(action_src_enum, ACTION_SRC_ENUM_COUNT,
                                    evalue, &symbol));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, CTR_SRC_UPD_TRUE_As, symbol));


    evalue = action->value_type[FS_VALUE_A_TRUE];
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_enum_encode(action_update_mode_enum,
                                    ACTION_UPDATE_MODE_ENUM_COUNT,
                                    evalue, &symbol));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, TRUE_UPDATE_MODE_As, symbol));

    FLEXSTATE_ID_DECOMPOSE(action->value_a_profile, &pkey);
    if (pkey.type == FLEXSTATE_OP_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_key_to_id(unit, &pkey, &data));
    } else {
        data = 0;
    }

    if (action->mode == bcmFlexstateCounterModeWide) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, st->act_value_a_comp, data));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, st->act_value_a_true, data));
    }

    /* Valute A false. */
    evalue = action->value_select[FS_VALUE_A_FALSE];
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_enum_encode(action_src_enum, ACTION_SRC_ENUM_COUNT,
                                    evalue, &symbol));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, CTR_SRC_UPD_FALSE_As, symbol));


    evalue = action->value_type[FS_VALUE_A_FALSE];
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_enum_encode(action_update_mode_enum,
                                    ACTION_UPDATE_MODE_ENUM_COUNT,
                                    evalue, &symbol));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, FALSE_UPDATE_MODE_As, symbol));

    FLEXSTATE_ID_DECOMPOSE(action->value_a_profile_false, &pkey);
    if (pkey.type == FLEXSTATE_OP_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_key_to_id(unit, &pkey, &data));
    } else {
        data = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->act_value_a_false, data));


    evalue = action->update_compare_b;
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_enum_encode(action_update_cmp_enum,
                                    ACTION_UPDATE_COMP_ENUM_COUNT,
                                    evalue, &symbol));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, UPDATE_COMPARE_Bs, symbol));

    /* Comparison B. */
    evalue = action->value_select[FS_VALUE_B_COMP];
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_enum_encode(action_src_enum, ACTION_SRC_ENUM_COUNT,
                                    evalue, &symbol));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, CTR_SRC_COMP_Bs, symbol));

    FLEXSTATE_ID_DECOMPOSE(action->value_b_profile_comp, &pkey);
    if (pkey.type == FLEXSTATE_OP_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_key_to_id(unit, &pkey, &data));
    } else {
        data = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->act_value_b_comp, data));

    /* Valute B true. */
    evalue = action->value_select[FS_VALUE_B_TRUE];
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_enum_encode(action_src_enum, ACTION_SRC_ENUM_COUNT,
                                    evalue, &symbol));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, CTR_SRC_UPD_TRUE_Bs, symbol));


    evalue = action->value_type[FS_VALUE_B_TRUE];
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_enum_encode(action_update_mode_enum,
                                    ACTION_UPDATE_MODE_ENUM_COUNT,
                                    evalue, &symbol));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, TRUE_UPDATE_MODE_Bs, symbol));

    FLEXSTATE_ID_DECOMPOSE(action->value_b_profile, &pkey);
    if (pkey.type == FLEXSTATE_OP_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_key_to_id(unit, &pkey, &data));
    } else {
        data = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->act_value_b_true, data));

    /* Valute B false. */
    evalue = action->value_select[FS_VALUE_B_FALSE];
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_enum_encode(action_src_enum, ACTION_SRC_ENUM_COUNT,
                                    evalue, &symbol));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, CTR_SRC_UPD_FALSE_Bs, symbol));


    evalue = action->value_type[FS_VALUE_B_FALSE];
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_enum_encode(action_update_mode_enum,
                                    ACTION_UPDATE_MODE_ENUM_COUNT,
                                    evalue, &symbol));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, FALSE_UPDATE_MODE_Bs, symbol));

    FLEXSTATE_ID_DECOMPOSE(action->value_b_profile_false, &pkey);
    if (pkey.type == FLEXSTATE_OP_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_key_to_id(unit, &pkey, &data));
    } else {
        data = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->act_value_b_false, data));

    evalue = action->drop_count_mode;
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_enum_encode(action_drop_enum, ACTION_DROP_ENUM_COUNT,
                                    evalue, &symbol));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, COUNT_ON_RULE_DROPs, symbol));

    data = !!action->exception_drop_count_enable;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, COUNT_ON_HW_EXCP_DROPs, data));

    /* Egress side. */
    if (st_info[unit][key->stage].dir == BCMI_LTSW_FLEXSTATE_DIR_EGRESS &&
        action->egress_mirror_count_enable) {
        data = true;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, COUNT_ON_MIRRORs, data));
    }

    data = 1;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, CTR_VAL_DATAs, data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(eh));

    (void)bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from CTR_ING/EGR_EFLEX_ACTION_PROFILE LT.
 * This function is used to delete an entry from
 * CTR_ING/EGR_EFLEX_ACTION_PROFILE LT.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex state key identification.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_action_delete(int unit, ltsw_flexstate_key_t *key)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmint_flexstate_stage_info_t *st = &(st_info[unit][key->stage]);
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t data;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Counter action lt delete %d %d %u\n"),
              key->stage, key->pipe, key->id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->act_profile, &eh));

    /* Key fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_key_to_id(unit, key, &data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->act_profile_id, data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(eh));

    (void)bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup flex state action software database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_action_db_cleanup(int unit)
{
    ltsw_flexstate_action_info_t *ai = flexstate_action_info[unit];

    SHR_FUNC_ENTER(unit);

    if (ai == NULL) {
        SHR_EXIT();
    }

    if (ai->mutex) {
        sal_mutex_destroy(ai->mutex);
    }

    SHR_FREE(ai);
    flexstate_action_info[unit] = NULL;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize flex state action software database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_action_db_init(int unit)
{
    ltsw_flexstate_action_info_t *ai = flexstate_action_info[unit];
    uint64_t min, max;
    uint32_t ha_alloc_size = 0, ha_req_size = 0, pipe_req_size = 0;
    void *alloc_ptr = NULL;
    int warm = bcmi_warmboot_get(unit), stage, max_pipe, i;

    SHR_FUNC_ENTER(unit);

    if (ai != NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    } else {
        SHR_ALLOC(ai, sizeof(ltsw_flexstate_action_info_t), "ltswFlexstateActionInfo");
        SHR_NULL_CHECK(ai, SHR_E_PARAM);
        sal_memset(ai, 0, sizeof(ltsw_flexstate_action_info_t));
        flexstate_action_info[unit] = ai;
    }

    /* Create mutex. */
    ai->mutex = sal_mutex_create("ltswFlexstateActionInfo");
    SHR_NULL_CHECK(ai->mutex, SHR_E_MEMORY);

    /* Alloc action buffer from HA. */
    for (stage = FS_MIN_STAGE; stage < FS_MAX_STAGE; stage++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, st_info[unit][stage].act_profile,
                                           st_info[unit][stage].act_profile_id,
                                           &min, &max));
        ai->profile_id_min[stage] = (uint32_t)min;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, st_info[unit][stage].act_profile,
                                           ACTIONs, &min, &max));
        ai->action_min[stage] = (uint32_t)min;
        ai->action_max[stage] = (uint32_t)max + 1;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, st_info[unit][stage].operand_profile,
                                           PROFILEs, &min, &max));
        ai->op_profile_min[stage] = (uint32_t)min;
        ai->op_profile_max[stage] = (uint32_t)max + 1;

        max_pipe = flexstate_pipe_max[unit][stage];

        pipe_req_size = sizeof(ltsw_flexstate_action_t) * ai->action_max[stage];
        ha_req_size = pipe_req_size * max_pipe;
        ha_alloc_size = ha_req_size;
        alloc_ptr = bcmi_ltsw_ha_mem_alloc(unit, BCMI_HA_COMP_ID_FLEXSTATE,
                                           st_info[unit][stage].ha_action_id,
                                           "bcmFlexStateAction",
                                           &ha_alloc_size);
        SHR_NULL_CHECK(alloc_ptr, SHR_E_MEMORY);

        if (ha_alloc_size < ha_req_size) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }

        if (!warm) {
            sal_memset(alloc_ptr, 0, ha_alloc_size);
        }

        for (i = 0; i < max_pipe; i++) {
            ai->actions[stage][i] =
                (ltsw_flexstate_action_t *)(alloc_ptr + pipe_req_size * i);
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        for (stage = FS_MIN_STAGE; stage < FS_MAX_STAGE; stage++) {
            if (ai->actions[stage][0] && !warm) {
                (void)bcmi_ltsw_ha_mem_free(unit, ai->actions[stage][0]);
            }
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Calculate maximum index number.
 *
 * \param [in] num Index object number.
 * \param [in] oi Flex state object information.
 * \param [in] mask_size Object mask size.
 * \param [in] shift Object shift.
 *
 * \retval Maximum index number.
 */
static uint32_t
ltsw_flexstate_index_max_value_get( int num,
                                   bcmint_flexstate_object_info_t *oi,
                                   uint8_t mask_size,
                                   uint8_t shift)
{
    uint32_t max_value = 0;
    uint16_t mask = (1UL << mask_size) - 1;

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META("num %d, objcet %d, mask %x, shift %u\n"),
              num, oi->object, mask, shift));

    switch (oi->obj_type) {
        case FlexstateObjectTypeConst:
            if (oi->object == bcmFlexstateObjectConstOne) {
                max_value = 1;
            } else {
                max_value = 0;
            }
            break;

        case FlexstateObjectTypeTrigger:
            /* 0 - 255 */
            max_value = 255;
            break;

        case FlexstateObjectTypeQuant:
            /* 0 - 8 */
            max_value = 8;
            break;

        default:
            max_value = mask;
            break;
    }

    /* The second object. */
    if (num) {
        max_value <<= shift;
    }

    return max_value;
}

/*!
 * \brief Flex state value parameter validation function.
 *
 * This function is used to validate value parameters.
 *
 * \param [in] unit Unit number.
 * \param [in] stage Stage id.
 * \param [in] pipe Pipe number.
 * \param [in] value_op Flex state value data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_value_params_validate(int unit, int stage, int pipe,
                                     bcm_flexstate_value_operation_t *value_op)
{
    bcmint_flexstate_object_info_t *oi;
    int object, i;

    SHR_FUNC_ENTER(unit);

    if (value_op->select < bcmFlexstateValueSelectCounterValue ||
        value_op->select >= bcmFlexstateValueSelectCount) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid value op select %d\n"),
                   value_op->select));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (value_op->type < bcmFlexstateValueOperationTypeNoop ||
        value_op->type >= bcmFlexstateValueOperationTypeCount) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid value op type %d\n"),
                   value_op->type));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (value_op->select == bcmFlexstateValueSelectCounterValue) {
        /* Check value objects. */
        for (i = 0; i < FS_MAX_OPOBJ; i++) {
            /* Do more check for mask_size and shift. */
            if ((value_op->mask_size[i] < FLEXSTATE_OP_MASK_MIN) ||
                (value_op->mask_size[i] > FLEXSTATE_OP_MASK_MAX)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid mask size %u for value\n"),
                           value_op->mask_size[i]));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            if (value_op->shift[i] > FLEXSTATE_OP_SHIFT_MAX) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid shift %u for value\n"),
                           value_op->shift[i]));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            object = value_op->object[i];

            if (object == bcmFlexstateObjectQuant) {
                if (FLEXSTATE_ID_STAGE(value_op->quant_id[i]) != stage ||
                    FLEXSTATE_ID_PIPE(value_op->quant_id[i]) != pipe) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }

                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexstate_quant_id_to_quant(unit, value_op->quant_id[i],
                                                    NULL, NULL));
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexstate_object_info_get(unit, object, &oi));

            if (oi->obj_type == FlexstateObjectTypeMenu &&
                (oi->stage != stage)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid value object %d for "
                                            "%s action\n"),
                           object, st_info[unit][stage].name));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state action parameter validation function.
 *
 * This function is used to validate action parameters.
 *
 * \param [in] unit Unit number.
 * \param [in] options BCM_FLEXSTATE_OPTIONS_XXX options.
 * \param [in] act_cfg Flex state action data structure.
 * \param [out] key Flex state key.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_action_params_validate(int unit, int options,
                                      bcm_flexstate_action_t *act_cfg,
                                      ltsw_flexstate_key_t *key)
{
    int stage, pipe = 0;
    ltsw_flexstate_pool_info_t *pi = flexstate_pool_info[unit];
    bcmint_flexstate_source_info_t *si = NULL;
    bcm_flexstate_trigger_t *trigger = NULL;
    bcm_flexstate_index_operation_t *index_op = NULL;
    bcm_flexstate_value_operation_t *value_op = NULL;
    bcmint_flexstate_object_info_t *oi = NULL;
    int object, i;
    uint32_t max_index_val = 0;
    bcm_flexstate_update_compare_t update_compare;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexstate_source_info_get(unit, act_cfg->source, &si));

    stage = si->stage;

    if (options & BCMI_LTSW_FLEXSTATE_OPTIONS_PIPE_BMP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_config_pipe_bitmap_get(unit, act_cfg->ports,
                                                   &pipe));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_config_port_to_pipe(unit, act_cfg->ports, &pipe));
    }

    if (act_cfg->flags & BCM_FLEXSTATE_ACTION_FLAGS_TRIGGER) {
        trigger = &(act_cfg->trigger);

        if (trigger->trigger_type < bcmFlexstateTriggerTypeTime ||
            trigger->trigger_type >= bcmFlexstateTriggerTypeCount) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid trigger type %d\n"),
                       trigger->trigger_type));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (trigger->trigger_type == bcmFlexstateTriggerTypeTime) {
            if (trigger->interval < bcmFlexstateTriggerIntervalUnlimited ||
                trigger->interval >= bcmFlexstateTriggerIntervalCount) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid trigger interval %d\n"),
                           trigger->interval));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }

        /* Check trigger object. */
        if (trigger->trigger_type == bcmFlexstateTriggerTypeCondition) {
            object = trigger->object;

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexstate_object_info_get(unit, object, &oi));

            if ((oi->obj_type != FlexstateObjectTypeMenu) ||
                (oi->stage != stage)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid trigger object %d for "
                                            "%s action\n"),
                           object, st_info[unit][stage].name));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }

        if (trigger->trigger_type ==
            bcmFlexstateTriggerTypeConditionStartTimeStop) {
            object = trigger->object;

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexstate_object_info_get(unit, object, &oi));

            if ((oi->obj_type != FlexstateObjectTypeMenu) ||
                (oi->stage != stage)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid trigger object %d for "
                                            "%s action\n"),
                           object, st_info[unit][stage].name));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            if (trigger->interval < bcmFlexstateTriggerIntervalUnlimited ||
                trigger->interval >= bcmFlexstateTriggerIntervalCount) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid trigger interval %d\n"),
                           trigger->interval));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
    }

    /* Inidicate flexstate output number.
     * Requires:
     *      1. All counters in a single pool.
     *      2. Hint value could not exceed the maximum pool number.
     */
    if (act_cfg->flags & BCM_FLEXSTATE_ACTION_FLAGS_HINT_OUTPUT) {
        if (act_cfg->index_num > pi->nctrs_in_pool[stage] ||
            act_cfg->hint > pi->npools[stage] - 1) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid config for hint output:\n"
                                        "   index_num %d max_index_num %d "
                                        "   hint %d max_hint %d\n"),
                       act_cfg->index_num, (int)pi->nctrs_in_pool[stage],
                       act_cfg->hint, (int)pi->npools[stage] - 1));

            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (act_cfg->drop_count_mode < bcmFlexstateDropCountModeNoDrop ||
        act_cfg->drop_count_mode >= bcmFlexstateDropCountModeCount) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid action drop count mode %d\n"),
                   act_cfg->drop_count_mode));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((st_info[unit][stage].dir == BCMI_LTSW_FLEXSTATE_DIR_INGRESS) &&
        act_cfg->egress_mirror_count_enable) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Ingress flex state action "
                                   "does not support mirror count\n")));
    }

    if (act_cfg->mode < bcmFlexstateCounterModeNormal ||
        act_cfg->mode >= bcmFlexstateCounterModeCount) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid action mode %d\n"), act_cfg->mode));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Check index operation objects. */
    index_op = &act_cfg->index_operation;
    for (i = 0; i < FS_MAX_OPOBJ; i++) {
        /* Do more check for mask_size and shift. */
        if ((index_op->mask_size[i] < FLEXSTATE_OP_MASK_MIN) ||
            (index_op->mask_size[i] > FLEXSTATE_OP_MASK_MAX)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid mask size %u for index operand\n"),
                       index_op->mask_size[i]));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (index_op->shift[i] > FLEXSTATE_OP_SHIFT_MAX) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid shift %u for index operand\n"),
                       index_op->shift[i]));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        object = index_op->object[i];

        if (object == bcmFlexstateObjectQuant) {
            if (FLEXSTATE_ID_STAGE(index_op->quant_id[i]) != stage ||
                FLEXSTATE_ID_PIPE(index_op->quant_id[i]) != pipe) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_quant_id_to_quant(unit, index_op->quant_id[i],
                                                NULL, NULL));
        }


        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexstate_object_info_get(unit, object, &oi));

        if (oi->obj_type == FlexstateObjectTypeMenu &&
            (oi->stage != si->stage)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid index object %d for %s action\n"),
                       object, st_info[unit][stage].name));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        max_index_val |= ltsw_flexstate_index_max_value_get(i, oi,
                                                            index_op->mask_size[i],
                                                            index_op->shift[i]);
    }

    if (max_index_val + 1 > act_cfg->index_num) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Index_num %u is less than possible "
                                   "maximum index %u\n"),
                  act_cfg->index_num, (max_index_val + 1)));
    }

    value_op = &act_cfg->operation_a.operation_true;
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_value_params_validate(unit, stage, pipe, value_op));

    if (act_cfg->mode == bcmFlexstateCounterModeNormal) {
        value_op = &act_cfg->operation_b.operation_true;
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_value_params_validate(unit, stage, pipe, value_op));

        update_compare = act_cfg->operation_a.update_compare;

        if (update_compare < bcmFlexstateUpdateCompareAlwaysTrue ||
            update_compare >= bcmFlexstateUpdateCompareCount) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid value update compare%d\n"),
                       update_compare));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (update_compare != bcmFlexstateUpdateCompareAlwaysTrue) {
            value_op = &act_cfg->operation_a.compare_operation;
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_value_params_validate(unit, stage, pipe, value_op));

            value_op = &act_cfg->operation_a.operation_false;
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_value_params_validate(unit, stage, pipe, value_op));
        }

        update_compare = act_cfg->operation_b.update_compare;

        if (update_compare < bcmFlexstateUpdateCompareAlwaysTrue ||
            update_compare >= bcmFlexstateUpdateCompareCount) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid value update compare%d\n"),
                       update_compare));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (update_compare != bcmFlexstateUpdateCompareAlwaysTrue) {
            value_op = &act_cfg->operation_b.compare_operation;
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_value_params_validate(unit, stage, pipe, value_op));

            value_op = &act_cfg->operation_b.operation_false;
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_value_params_validate(unit, stage, pipe, value_op));
        }
    }

    key->pipe = pipe;
    key->stage = stage;
    key->type = FLEXSTATE_ACTION;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Action trigger process function.
 *
 * This function is used to process action trigger.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex state key identification.
 * \param [in] trigger Flex state trigger data structure.
 * \param [out] action Internal flex state action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_action_trigger_process(int unit,
                                      ltsw_flexstate_key_t *key,
                                      bcm_flexstate_trigger_t *trigger,
                                      ltsw_flexstate_action_t *action)
{
    SHR_FUNC_ENTER(unit);

    if (trigger->trigger_type == bcmFlexstateTriggerTypeCondition ||
        trigger->trigger_type == bcmFlexstateTriggerTypeConditionStartTimeStop) {
        /* Trigger object must be placed at the first action object. */
        if (action->objects[0] != FLEXSTATE_OBJ_INVALID &&
            (action->objects[0] != trigger->object)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Action object 0 is occupied by %d\n"),
                       action->objects[0]));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        action->objects[0] = trigger->object;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Action index operation process function.
 *
 * This function is used to process flex state index operation.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex state key identification.
 * \param [in] index_op Flex state index operation data structure.
 * \param [out] action Internal flex state action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_action_index_operation_process(
                                    int unit,
                                    ltsw_flexstate_key_t *key,
                                    bcm_flexstate_index_operation_t *index_op,
                                    ltsw_flexstate_action_t *action)
{
    ltsw_flexstate_operation_profile_t profile = {{0}};
    size_t copy_sz = sizeof(int) * FS_ACTION_OBJ_COUNT;
    int i, j;
    uint32_t profile_id;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < FS_MAX_OPOBJ; i++) {
        j = FS_INDEX_OP * FS_MAX_OPOBJ + i;

        action->mask_size[j] = profile.mask_size[i] = index_op->mask_size[i];
        action->shift[j] = profile.shift[i] = index_op->shift[i];
        action->object[j] = profile.objects[i] = index_op->object[i];
        if (index_op->object[i] == bcmFlexstateObjectQuant) {
            action->quant_id[j] = profile.quant_id[i] = index_op->quant_id[i];
        } else {
            action->quant_id[j] = FS_ID_INVALID;
        }
    }

    sal_memcpy(profile.action_objects, action->objects, copy_sz);

    profile.key.pipe = key->pipe;
    profile.key.stage = key->stage;
    profile.key.type = FLEXSTATE_OP_PROFILE;

    SHR_IF_ERR_EXIT
        (ltsw_flexstate_op_profile_entry_add(unit, &profile, &profile_id));

    /* Update action information. */
    sal_memcpy(action->objects, profile.action_objects, copy_sz);
    action->index_profile = profile_id;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Action value operation process function.
 *
 * This function is used to process flex state value operation.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex state key identification.
 * \param [in] type Flex state value instance.
 * \param [in] value_op Flex state value operation data structure.
 * \param [out] action Internal flex state action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_action_value_operation_process(
                                    int unit,
                                    ltsw_flexstate_key_t *key,
                                    int type,
                                    bcm_flexstate_value_operation_t *value_op,
                                    ltsw_flexstate_action_t *action)
{
    ltsw_flexstate_operation_profile_t profile = {{0}};
    size_t copy_sz = sizeof(int) * FS_ACTION_OBJ_COUNT;
    int i, j;
    uint32_t profile_id;

    SHR_FUNC_ENTER(unit);

    action->value_select[type] = value_op->select;
    action->value_type[type] = value_op->type;

    for (i = 0; i < FS_MAX_OPOBJ; i++) {
        j = type * FS_MAX_OPOBJ + i;

        action->mask_size[j] = profile.mask_size[i] = value_op->mask_size[i];
        action->shift[j] = profile.shift[i] = value_op->shift[i];
        action->object[j] = profile.objects[i] = value_op->object[i];
        if (value_op->object[i] == bcmFlexstateObjectQuant) {
            action->quant_id[j] = profile.quant_id[i] = value_op->quant_id[i];
        } else {
            action->quant_id[j] = FS_ID_INVALID;
        }
    }

    if (value_op->select != bcmFlexstateValueSelectCounterValue) {
        SHR_EXIT();
    }

    sal_memcpy(profile.action_objects, action->objects, copy_sz);

    profile.key.pipe = key->pipe;
    profile.key.stage = key->stage;
    profile.key.type = FLEXSTATE_OP_PROFILE;

    SHR_IF_ERR_EXIT
        (ltsw_flexstate_op_profile_entry_add(unit, &profile, &profile_id));

    /* Update action information. */
    sal_memcpy(action->objects, profile.action_objects, copy_sz);

    switch (type) {
        case FS_VALUE_A_COMP:
            action->value_a_profile_comp = profile_id;
            break;

        case FS_VALUE_A_TRUE:
            action->value_a_profile = profile_id;
            break;

        case FS_VALUE_A_FALSE:
            action->value_a_profile_false = profile_id;
            break;

        case FS_VALUE_B_COMP:
            action->value_b_profile_comp = profile_id;
            break;

        case FS_VALUE_B_TRUE:
            action->value_b_profile = profile_id;
            break;

        case FS_VALUE_B_FALSE:
            action->value_b_profile_false = profile_id;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state action creation function.
 *
 * This function is used to create a flex state action.
 *
 * \param [in] unit Unit number.
 * \param [in] options BCM_FLEXSTATE_OPTIONS_XXX options.
 * \param [in] act_cfg Flex state action data structure.
 * \param [in/out] action_id Flex state ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_action_create(int unit, int options,
                             bcm_flexstate_action_t *act_cfg,
                             uint32_t *action_id)
{
    ltsw_flexstate_action_info_t *ai = flexstate_action_info[unit];
    ltsw_flexstate_action_t *action = NULL;
    bcm_flexstate_trigger_t *trigger = NULL;
    bcm_flexstate_index_operation_t *index_op = NULL;
    bcm_flexstate_value_operation_t *value_op = NULL;
    ltsw_flexstate_key_t nkey = {0};
    uint32_t min_id, max_id;
    int i;
    bool rollback = FALSE, free_pool= FALSE, free_tr = FALSE;

    SHR_FUNC_ENTER(unit);

    FLEXSTATE_ACTION_LOCK(unit);
    FLEXSTATE_QUANT_LOCK(unit);

    SHR_NULL_CHECK(act_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(action_id, SHR_E_PARAM);

    /* Parameter check. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_action_params_validate(unit, options, act_cfg, &nkey));

    min_id = ai->action_min[nkey.stage];
    max_id = ai->action_max[nkey.stage];

    if (options & BCM_FLEXSTATE_OPTIONS_WITH_ID) {
        nkey.id = FLEXSTATE_ID_INDEX(*action_id);
        if (nkey.id < min_id || nkey.id >= max_id) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid action id %d "
                                        "valid id range %d - %d\n"),
                       FLEXSTATE_ID_INDEX(*action_id),
                       min_id, max_id - 1));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        action = &(ai->actions[nkey.stage][nkey.pipe][nkey.id]);
        if (action->used) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Action id %d exists\n"),
                       *action_id));
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
    } else {
        /* Search an unused counter action. */
        for (nkey.id = min_id; nkey.id < max_id; nkey.id++) {
            action = &(ai->actions[nkey.stage][nkey.pipe][nkey.id]);
            if (!action ->used) {
                break;
            }
        }

        if (nkey.id >= max_id) {
            SHR_ERR_EXIT(SHR_E_FULL);
        }
    }

    /* Set rollback flag. */
    rollback = TRUE;

    /* Reset action objects. */
    action->options = options;
    action->flags = act_cfg->flags;
    action->source = act_cfg->source;
    action->hint = act_cfg->hint;
    action->drop_count_mode = act_cfg->drop_count_mode;
    action->exception_drop_count_enable = act_cfg->exception_drop_count_enable;
    action->egress_mirror_count_enable = act_cfg->egress_mirror_count_enable;
    action->mode = act_cfg->mode;
    /* Logical index number. */
    action->index_num = act_cfg->index_num;

    for (i = 0; i < FS_ACTION_OBJ_COUNT; i++) {
        action->objects[i] = FLEXSTATE_OBJ_INVALID;
    }

    /* Process action trigger. */
    if (act_cfg->flags & BCM_FLEXSTATE_ACTION_FLAGS_TRIGGER) {
        trigger = &act_cfg->trigger;
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_action_trigger_process(unit, &nkey, trigger, action));
    }

    /* Process flex state index operation. */
    index_op = &act_cfg->index_operation;
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_action_index_operation_process(unit, &nkey, index_op,
                                                       action));

    /* Process flex state value a operation. */
    value_op = &act_cfg->operation_a.operation_true;
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_action_value_operation_process(
                                                unit, &nkey, FS_VALUE_A_TRUE,
                                                value_op, action));

    /* Process flex state value b operation. */
    if (act_cfg->mode == bcmFlexstateCounterModeNormal) {
        value_op = &act_cfg->operation_b.operation_true;
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_action_value_operation_process(
                                                unit, &nkey, FS_VALUE_B_TRUE,
                                                value_op, action));

        action->update_compare_a = act_cfg->operation_a.update_compare;
        if (action->update_compare_a != bcmFlexstateUpdateCompareAlwaysTrue) {
            value_op = &act_cfg->operation_a.compare_operation;
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_action_value_operation_process(
                                                unit, &nkey, FS_VALUE_A_COMP,
                                                value_op, action));

            value_op = &act_cfg->operation_a.operation_false;
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_action_value_operation_process(
                                                unit, &nkey, FS_VALUE_A_FALSE,
                                                value_op, action));
        }

        action->update_compare_b = act_cfg->operation_b.update_compare;
        if (action->update_compare_b != bcmFlexstateUpdateCompareAlwaysTrue) {
            value_op = &act_cfg->operation_b.compare_operation;
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_action_value_operation_process(
                                                unit, &nkey, FS_VALUE_B_COMP,
                                                value_op, action));

            value_op = &act_cfg->operation_b.operation_false;
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_action_value_operation_process(
                                                unit, &nkey, FS_VALUE_B_FALSE,
                                                value_op, action));
        }
    }

    /* Assign pool and counter resource. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_pool_alloc(unit, &nkey, act_cfg, action));
    free_pool = TRUE;

    /* Update flex state trigger LT. */
    if (act_cfg->flags & BCM_FLEXSTATE_ACTION_FLAGS_TRIGGER) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_action_trigger_op(unit, &nkey, trigger,
                                              BCMLT_OPCODE_INSERT));
        free_tr = TRUE;
    }

    /* Update flex state action LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_action_insert(unit, &nkey, action));

    action->used = TRUE;

    /* Last update quantization reference count. */
    for (i = 0; i < FS_ACTION_OPOBJ_COUNT; i++) {
        ltsw_flexstate_quant_t *quant = NULL;

        if (action->quant_id[i] == FS_ID_INVALID) {
            continue;
        }

        /* Should not fail here. */
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_quant_id_to_quant(unit, action->quant_id[i],
                                              &quant, NULL));
        quant->ref_count++;
    }

    /* Compose action id. */
    FLEXSTATE_ID_COMPOSE(&nkey, *action_id);

exit:
    if (SHR_FUNC_ERR() && rollback) {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Start rollback\n")));

        if (free_tr) {
            (void)ltsw_flexstate_action_trigger_op(unit, &nkey, trigger,
                                                   BCMLT_OPCODE_DELETE);
        }

        (void)ltsw_flexstate_op_profile_entry_remove(unit,
                                                     action->index_profile);

        (void)ltsw_flexstate_op_profile_entry_remove(unit,
                                                     action->value_a_profile);
        (void)
        ltsw_flexstate_op_profile_entry_remove(unit,
                                               action->value_a_profile_comp);
        (void)
        ltsw_flexstate_op_profile_entry_remove(unit,
                                               action->value_a_profile_false);

        (void)ltsw_flexstate_op_profile_entry_remove(unit,
                                                     action->value_b_profile);
        (void)
        ltsw_flexstate_op_profile_entry_remove(unit,
                                               action->value_b_profile_comp);
        (void)
        ltsw_flexstate_op_profile_entry_remove(unit,
                                               action->value_b_profile_false);

        if (free_pool) {
            (void)ltsw_flexstate_pool_free(unit, &nkey, action);
        }

        sal_memset(action, 0, sizeof(ltsw_flexstate_action_t));
    }

    FLEXSTATE_QUANT_UNLOCK(unit);
    FLEXSTATE_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state counter id to action function.
 *
 * This function is used to get an action from a action_id.
 *
 * \param [in] unit Unit number.
 * \param [in] action_id Flex state ID.
 * \param [out] action Flex state action.
 * \param [out] okey Flex state key identification.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_counter_id_to_action(int unit,
                                    uint32_t action_id,
                                    ltsw_flexstate_action_t **action,
                                    ltsw_flexstate_key_t *okey)
{
    ltsw_flexstate_action_info_t *ai = flexstate_action_info[unit];
    ltsw_flexstate_key_t key = {0};
    uint32_t min_id, max_id;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(action, SHR_E_PARAM);

    FLEXSTATE_ID_DECOMPOSE(action_id, &key);

    if (key.type != FLEXSTATE_ACTION) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid counter action %x\n"),
                   action_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id type %x\n"), key.type));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((key.stage < FS_MIN_STAGE) || (key.stage >= FS_MAX_STAGE)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid counter action %x\n"),
                   action_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id stage %x\n"), key.stage));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (key.pipe < 0 || key.pipe >= flexstate_pipe_max[unit][key.stage]) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid counter action %x\n"),
                   action_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id pipe %x\n"), key.pipe));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    min_id = ai->action_min[key.stage];
    max_id = ai->action_max[key.stage];
    if (key.id < min_id || key.id >= max_id) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid counter action %x\n"),
                   action_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id %x\n"), key.id));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *action = &(ai->actions[key.stage][key.pipe][key.id]);
    if (!(*action)->used) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Counter action %x not found\n"),
                  action_id));

        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (okey) {
        sal_memcpy(okey, &key, sizeof(ltsw_flexstate_key_t));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state action destroy function.
 *
 * This function is used to destroy a flex state action.
 *
 * \param [in] unit Unit number.
 * \param [in] action_id Flex state ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_action_destroy(int unit, uint32_t action_id)
{
    ltsw_flexstate_action_t *action = NULL;
    ltsw_flexstate_key_t key = {0};
    int i;

    SHR_FUNC_ENTER(unit);

    FLEXSTATE_ACTION_LOCK(unit);
    FLEXSTATE_QUANT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_counter_id_to_action(unit, action_id,
                                             &action, &key));

    if (action->ref_count != 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Counter action %x is in using\n"),
                   action_id));
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    if (action->grp_ref_count != 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Counter action %x is in using group action\n"),
                   action_id));
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_action_delete(unit, &key));

    if (action->flags & BCM_FLEXSTATE_ACTION_FLAGS_TRIGGER) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_action_trigger_op(unit, &key, NULL,
                                              BCMLT_OPCODE_DELETE));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_op_profile_entry_remove(unit, action->index_profile));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_op_profile_entry_remove(unit, action->value_a_profile));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_op_profile_entry_remove(unit,
                                                action->value_a_profile_comp));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_op_profile_entry_remove(unit,
                                                action->value_a_profile_false));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_op_profile_entry_remove(unit, action->value_b_profile));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_op_profile_entry_remove(unit,
                                                action->value_b_profile_comp));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_op_profile_entry_remove(unit,
                                                action->value_b_profile_false));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_pool_free(unit, &key, action));

    /* Last update quantization reference count. */
    for (i = 0; i < FS_ACTION_OPOBJ_COUNT; i++) {
        ltsw_flexstate_quant_t *quant = NULL;

        if (action->quant_id[i] == FS_ID_INVALID) {
            continue;
        }

        /* Should not fail here. */
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_quant_id_to_quant(unit, action->quant_id[i],
                                              &quant, NULL));
        quant->ref_count--;
    }

    /* Reset internal action structure. */
    sal_memset(action, 0, sizeof(ltsw_flexstate_action_t));

exit:
    FLEXSTATE_QUANT_UNLOCK(unit);
    FLEXSTATE_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*\
 * brief Flex state action get function.
 *
 * This function is used to get a flex state action information.
 *
 * \param [in] unit Unit number.
 * \param [in] action_id Flex state ID.
 * \param [out] act_cfg Flex state action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_action_get(int unit,
                          uint32_t action_id,
                          bcm_flexstate_action_t *act_cfg)
{
    ltsw_flexstate_key_t key = {0};
    ltsw_flexstate_action_t *action = NULL;
    bcm_flexstate_trigger_t *trigger = NULL;
    bcm_flexstate_index_operation_t *index_op = NULL;
    bcm_flexstate_value_operation_t *value_op = NULL;
    bcm_port_config_t port_config;
    int i, j;

    SHR_FUNC_ENTER(unit);

    FLEXSTATE_ACTION_LOCK(unit);

    SHR_NULL_CHECK(act_cfg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_counter_id_to_action(unit, action_id, &action, &key));

    sal_memset(act_cfg, 0, sizeof(bcm_flexstate_action_t));
    act_cfg->source = action->source;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_port_config_get(unit, &port_config));
    BCM_PBMP_ASSIGN(act_cfg->ports, port_config.per_pp_pipe[key.pipe]);

    act_cfg->flags = action->flags;
    act_cfg->hint = action->hint;
    act_cfg->drop_count_mode = action->drop_count_mode;
    act_cfg->exception_drop_count_enable = action->exception_drop_count_enable;
    act_cfg->egress_mirror_count_enable = action->egress_mirror_count_enable;
    act_cfg->mode = action->mode;
    act_cfg->index_num = action->index_num;

    index_op = &act_cfg->index_operation;
    for (i = 0; i < FS_MAX_OPOBJ; i++) {
        j = FS_INDEX_OP * FS_MAX_OPOBJ + i;

        index_op->object[i] = action->object[j];
        index_op->quant_id[i] = action->quant_id[j];
        index_op->mask_size[i] = action->mask_size[j];
        index_op->shift[i] = action->shift[j];
    }

    value_op = &act_cfg->operation_a.operation_true;
    value_op->select = action->value_select[FS_VALUE_A_TRUE];
    value_op->type = action->value_type[FS_VALUE_A_TRUE];
    for (i = 0; i < FS_MAX_OPOBJ; i++) {
        j = FS_VALUE_A_TRUE * FS_MAX_OPOBJ + i;

        value_op->object[i] = action->object[j];
        value_op->quant_id[i] = action->quant_id[j];
        value_op->mask_size[i] = action->mask_size[j];
        value_op->shift[i] = action->shift[j];
    }

    if (action->mode == bcmFlexstateCounterModeNormal) {
        value_op = &act_cfg->operation_b.operation_true;
        value_op->select = action->value_select[FS_VALUE_B_TRUE];
        value_op->type = action->value_type[FS_VALUE_B_TRUE];
        for (i = 0; i < FS_MAX_OPOBJ; i++) {
            j = FS_VALUE_B_TRUE * FS_MAX_OPOBJ + i;

            value_op->object[i] = action->object[j];
            value_op->quant_id[i] = action->quant_id[j];
            value_op->mask_size[i] = action->mask_size[j];
            value_op->shift[i] = action->shift[j];
        }

        act_cfg->operation_a.update_compare = action->update_compare_a;
        if (action->update_compare_a != bcmFlexstateUpdateCompareAlwaysTrue) {
            value_op = &act_cfg->operation_a.compare_operation;
            value_op->select = action->value_select[FS_VALUE_A_COMP];
            value_op->type = action->value_type[FS_VALUE_A_COMP];
            for (i = 0; i < FS_MAX_OPOBJ; i++) {
                j = FS_VALUE_A_COMP * FS_MAX_OPOBJ + i;

                value_op->object[i] = action->object[j];
                value_op->quant_id[i] = action->quant_id[j];
                value_op->mask_size[i] = action->mask_size[j];
                value_op->shift[i] = action->shift[j];
            }

            value_op = &act_cfg->operation_a.operation_false;
            value_op->select = action->value_select[FS_VALUE_A_FALSE];
            value_op->type = action->value_type[FS_VALUE_A_FALSE];
            for (i = 0; i < FS_MAX_OPOBJ; i++) {
                j = FS_VALUE_A_FALSE * FS_MAX_OPOBJ + i;

                value_op->object[i] = action->object[j];
                value_op->quant_id[i] = action->quant_id[j];
                value_op->mask_size[i] = action->mask_size[j];
                value_op->shift[i] = action->shift[j];
            }
        }

        act_cfg->operation_b.update_compare = action->update_compare_b;
        if (action->update_compare_b != bcmFlexstateUpdateCompareAlwaysTrue) {
            value_op = &act_cfg->operation_b.compare_operation;
            value_op->select = action->value_select[FS_VALUE_B_COMP];
            value_op->type = action->value_type[FS_VALUE_B_COMP];
            for (i = 0; i < FS_MAX_OPOBJ; i++) {
                j = FS_VALUE_B_COMP * FS_MAX_OPOBJ + i;

                value_op->object[i] = action->object[j];
                value_op->quant_id[i] = action->quant_id[j];
                value_op->mask_size[i] = action->mask_size[j];
                value_op->shift[i] = action->shift[j];
            }

            value_op = &act_cfg->operation_b.operation_false;
            value_op->select = action->value_select[FS_VALUE_B_FALSE];
            value_op->type = action->value_type[FS_VALUE_B_FALSE];
            for (i = 0; i < FS_MAX_OPOBJ; i++) {
                j = FS_VALUE_B_FALSE * FS_MAX_OPOBJ + i;

                value_op->object[i] = action->object[j];
                value_op->quant_id[i] = action->quant_id[j];
                value_op->mask_size[i] = action->mask_size[j];
                value_op->shift[i] = action->shift[j];
            }
        }
    }

    if (action->flags & BCM_FLEXSTATE_ACTION_FLAGS_TRIGGER) {
        trigger = &act_cfg->trigger;

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_action_trigger_op(unit, &key, trigger,
                                              BCMLT_OPCODE_LOOKUP));
        if (trigger->trigger_type == bcmFlexstateTriggerTypeCondition ||
            trigger->trigger_type == bcmFlexstateTriggerTypeConditionStartTimeStop) {
            trigger->object = action->objects[0];
        }
    }

exit:
    FLEXSTATE_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state action traverse function.
 *
 * This function is used to traverse all flex state action information.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn Action traverse callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int ltsw_flexstate_action_traverse(int unit,
                                   bcm_flexstate_action_traverse_cb trav_fn,
                                   void *user_data)
{
    ltsw_flexstate_action_info_t *ai = flexstate_action_info[unit];
    ltsw_flexstate_action_t *action = NULL;
    ltsw_flexstate_key_t key = {0};
    bcm_flexstate_action_t act_cfg;
    uint32_t min_id, max_id;
    int max_pipe, action_id, rv;

    SHR_FUNC_ENTER(unit);

    FLEXSTATE_ACTION_LOCK(unit);

    SHR_NULL_CHECK(trav_fn, SHR_E_PARAM);

    key.type = FLEXSTATE_ACTION;
    for (key.stage = FS_MIN_STAGE; key.stage < FS_MAX_STAGE; key.stage++) {
        max_pipe = flexstate_pipe_max[unit][key.stage];
        min_id = ai->action_min[key.stage];
        max_id = ai->action_max[key.stage];

        for (key.pipe = 0; key.pipe < max_pipe; key.pipe++) {
            for (key.id = min_id; key.id < max_id; key.id++) {
                action = &(ai->actions[key.stage][key.pipe][key.id]);

                if (!action->used) {
                    continue;
                }

                /* Skip internal actions. */
                if (action->options & BCMI_LTSW_FLEXSTATE_OPTIONS_INTERNAL) {
                    continue;
                }

                FLEXSTATE_ID_COMPOSE(&key, action_id);

                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexstate_action_get(unit, action_id, &act_cfg));

                rv = trav_fn(unit, action_id, &act_cfg, user_data);
                if (SHR_FAILURE(rv)) {
                    break;
                }
            }
        }
    }

exit:
    FLEXSTATE_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief EFLEX_GROUP_ACTION LT operation.
 *
 * This function is used to update/insert or delete an entry from
 * EFLEX_GROUP_ACTION LT.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex state key identification.
 * \param [in] grp_act Flex state group action data structure.
 * \param [in] opcode LT operation code.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_group_action_op(int unit,
                             ltsw_flexstate_key_t *key,
                             ltsw_flexstate_grp_action_t *grp_act,
                             bcmlt_opcode_t opcode)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmint_flexstate_stage_info_t *st = &(st_info[unit][key->stage]);
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t data;
    int i;
    bool instance_valid = FALSE;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Group action lt op %d %d %d %u\n"),
              opcode, key->stage, key->pipe, key->id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->grp_act_profile, &eh));

    /* Key fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_key_to_id(unit, key, &data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->grp_act_profile_id, data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, GROUPs, key->id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PIPEs, key->pipe));

    /* Check INSTANCE field. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_lt_field_validation
             (unit, st->grp_act_profile, INSTANCEs, &instance_valid));
    if (instance_valid) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, INSTANCEs, st->inst));
    }

    if ((opcode == BCMLT_OPCODE_DELETE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, eh, opcode, BCMLT_PRIORITY_NORMAL));
    } else {
        /* Data fields. */
        ltsw_flexstate_key_t ikey = {0};

        FLEXSTATE_ID_DECOMPOSE(grp_act->parent_act_id, &ikey);
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_key_to_id(unit, &ikey, &data));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, st->act_profile_id, data));

        for (i = 1; i < grp_act->action_num; i++) {
            FLEXSTATE_ID_DECOMPOSE(grp_act->action_id[i], &ikey);
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_key_to_id(unit, &ikey, &data));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_add(eh, GROUP_MAPs, i - 1, &data, 1));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(eh));

    (void)bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup flex state group action software database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_group_action_db_cleanup(int unit)
{
    ltsw_flexstate_grp_action_info_t *gai = flexstate_grp_action_info[unit];

    SHR_FUNC_ENTER(unit);

    if (gai == NULL) {
        SHR_EXIT();
    }

    if (gai->mutex) {
        sal_mutex_destroy(gai->mutex);
    }

    SHR_FREE(gai);
    flexstate_grp_action_info[unit] = NULL;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize flex state group action software database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_group_action_db_init(int unit)
{
    ltsw_flexstate_grp_action_info_t *gai = flexstate_grp_action_info[unit];
    uint64_t min, max;
    bcmlt_field_def_t fld_def;
    uint32_t ha_alloc_size = 0, ha_req_size = 0, pipe_req_size = 0;
    void *alloc_ptr = NULL;
    int warm = bcmi_warmboot_get(unit), stage, max_pipe, i;

    SHR_FUNC_ENTER(unit);

    if (gai != NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    } else {
        SHR_ALLOC(gai, sizeof(ltsw_flexstate_grp_action_info_t),
                  "ltswFlexstateGrpActionInfo");
        SHR_NULL_CHECK(gai, SHR_E_PARAM);
        sal_memset(gai, 0, sizeof(ltsw_flexstate_grp_action_info_t));
        flexstate_grp_action_info[unit] = gai;
    }

    /* Create mutex. */
    gai->mutex = sal_mutex_create("ltswFlexstateGrpActionInfo");
    SHR_NULL_CHECK(gai->mutex, SHR_E_MEMORY);

    /* Alloc group action buffer from HA. */
    for (stage = FS_MIN_STAGE; stage < FS_MAX_STAGE; stage++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, st_info[unit][stage].grp_act_profile,
                                           GROUPs,
                                           &min, &max));
        gai->grp_action_min[stage] = (uint32_t)min;
        gai->grp_action_max[stage] = (uint32_t)max + 1;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit, st_info[unit][stage].grp_act_profile,
                                   GROUP_MAPs, &fld_def));
        gai->grp_size[stage] = fld_def.depth;

        max_pipe = flexstate_pipe_max[unit][stage];

        pipe_req_size = sizeof(ltsw_flexstate_action_t) * gai->grp_action_max[stage];
        ha_req_size = pipe_req_size * max_pipe;
        ha_alloc_size = ha_req_size;
        alloc_ptr = bcmi_ltsw_ha_mem_alloc(unit, BCMI_HA_COMP_ID_FLEXSTATE,
                                           st_info[unit][stage].ha_grp_act_id,
                                           "bcmFlexStateGrpAction",
                                           &ha_alloc_size);
        SHR_NULL_CHECK(alloc_ptr, SHR_E_MEMORY);
        if (ha_alloc_size < ha_req_size) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }

        if (!warm) {
            sal_memset(alloc_ptr, 0, ha_alloc_size);
        }

        for (i = 0; i < max_pipe; i++) {
            gai->grp_actions[stage][i] =
                (ltsw_flexstate_grp_action_t *)(alloc_ptr + pipe_req_size * i);
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        for (stage = FS_MIN_STAGE; stage < FS_MAX_STAGE; stage++) {
            if (gai->grp_actions[stage][0] && !warm) {
                (void)bcmi_ltsw_ha_mem_free(unit, gai->grp_actions[stage][0]);
            }
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state group action parameter validation function.
 *
 * This function is used to validate group action parameters.
 *
 * \param [in] unit Unit number.
 * \param [in] options BCM_FLEXSTATE_OPTIONS_XXX options.
 * \param [in] group_action Flex state group action data structure.
 * \param [out] key Flex state key.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_grp_action_params_validate(int unit, int options,
                                    bcm_flexstate_group_action_t *group_action,
                                    ltsw_flexstate_key_t *key)
{
    int stage, pipe = 0;
    ltsw_flexstate_grp_action_info_t *gai = flexstate_grp_action_info[unit];
    bcmint_flexstate_source_info_t *si = NULL;
    int i;
    ltsw_flexstate_action_t *action = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexstate_source_info_get(unit, group_action->source, &si));

    stage = si->stage;

    if (options & BCMI_LTSW_FLEXSTATE_OPTIONS_PIPE_BMP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_config_pipe_bitmap_get(unit, group_action->ports,
                                                   &pipe));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_config_port_to_pipe(unit, group_action->ports, &pipe));
    }

    if (group_action->action_num <= 0 ||
        group_action->action_num > gai->grp_size[stage] + 1) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid group map num %u min 1 max %d\n"),
                   group_action->action_num, gai->grp_size[stage] + 1));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Check each group action member. */
    for (i = 0; i < group_action->action_num; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_counter_id_to_action(unit,
                                               group_action->action_id[i],
                                               &action, NULL));

        if (FLEXSTATE_ID_PIPE(group_action->action_id[i]) != pipe) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid action[%d] %u for group action\n"),
                       i, group_action->action_id[i]));

            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (action->source != group_action->source) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid action[%d] %u for group action\n"),
                       i, group_action->action_id[i]));

            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    key->pipe = pipe;
    key->stage = stage;
    key->type = FLEXSTATE_GRP_ACTION;

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \breif Flex state group action creation function.
 *
 * This function is used to create a flex state group action.
 *
 * \param [in] unit Unit number.
 * \param [in] options BCM_FLEXSTATE_OPTIONS_XXX options.
 * \param [in] group_action Flex state group action data structure.
 * \param [in/out] group_action_id Flex state group ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_group_action_create(int unit, int options,
                                   bcm_flexstate_group_action_t *group_action,
                                   uint32_t *group_action_id)
{
    ltsw_flexstate_grp_action_info_t *gai = flexstate_grp_action_info[unit];
    ltsw_flexstate_grp_action_t *grp_act = NULL;
    ltsw_flexstate_action_t *action = NULL, *tmp_action = NULL;
    ltsw_flexstate_key_t nkey = {0}, akey = {0};
    uint32_t min_id, max_id;
    int i;
    bool rollback = FALSE;

    SHR_FUNC_ENTER(unit);

    FLEXSTATE_GRP_ACTION_LOCK(unit);
    FLEXSTATE_ACTION_LOCK(unit);

    SHR_NULL_CHECK(group_action, SHR_E_PARAM);
    SHR_NULL_CHECK(group_action_id, SHR_E_PARAM);

    /* Parameter check. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_grp_action_params_validate(unit, options, group_action, &nkey));

    min_id = gai->grp_action_min[nkey.stage];
    max_id = gai->grp_action_max[nkey.stage];

    if (options & BCM_FLEXSTATE_OPTIONS_WITH_ID) {
        nkey.id = FLEXSTATE_ID_INDEX(*group_action_id);
        if (nkey.id < min_id || nkey.id >= max_id) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid group action id %d "
                                        "valid id range %d - %d\n"),
                        FLEXSTATE_ID_INDEX(*group_action_id),
                        min_id, max_id - 1));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        grp_act = &(gai->grp_actions[nkey.stage][nkey.pipe][nkey.id]);
        if (grp_act->used) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Group action id %d exists\n"),
                       *group_action_id));
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
    } else {
        /* Search an unused group counter action. */
        for (nkey.id = min_id; nkey.id < max_id; nkey.id++) {
            grp_act =
                &(gai->grp_actions[nkey.stage][nkey.pipe][nkey.id]);
            if (!grp_act ->used) {
                break;
            }
        }

        if (nkey.id >= max_id) {
            SHR_ERR_EXIT(SHR_E_FULL);
        }
    }

    rollback = TRUE;

    /* First member. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_counter_id_to_action(unit,
                                             group_action->action_id[0],
                                             &action, &akey));

    if (action->group_action_id) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "First member [%x] is used in group_action[%x]\n"),
                   group_action->action_id[0],
                   action->group_action_id));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (action->ref_count > 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "First member [%x] should not be attached\n"),
                   group_action->action_id[0]));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Compose action id. */
    FLEXSTATE_ID_COMPOSE(&akey, grp_act->parent_act_id);

    /* Bookkeeping API configuration. */
    grp_act->source = group_action->source;
    grp_act->action_num = group_action->action_num;

    for (i = 0; i < group_action->action_num; i++) {
        grp_act->action_id[i] = group_action->action_id[i];
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_group_action_op(unit, &nkey, grp_act, BCMLT_OPCODE_INSERT));

    grp_act->options = options;
    grp_act->used = TRUE;

    /* Last update action reference count. */
    for (i = 0; i < group_action->action_num; i++) {
        /* Should not fail here. */
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_counter_id_to_action(unit,
                                                 group_action->action_id[i],
                                                 &tmp_action, NULL));
        tmp_action->grp_ref_count++;
    }

    /* Compose group action id. */
    FLEXSTATE_ID_COMPOSE(&nkey, *group_action_id);

    action->group_action_id = *group_action_id;

exit:
    if (SHR_FUNC_ERR() && rollback) {
        if (grp_act) {
            sal_memset(grp_act, 0, sizeof(ltsw_flexstate_grp_action_t));
        }
    }

    FLEXSTATE_ACTION_UNLOCK(unit);
    FLEXSTATE_GRP_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state group counter id to group action function.
 *
 * This function is used to get an action from a group_action_id.
 *
 * \param [in] unit Unit number.
 * \param [in] group_action_id Flex state group ID.
 * \param [out] grp_action Flex state group action.
 * \param [out] okey Flex state key identification.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_counter_id_to_grp_action(int unit,
                                        uint32_t group_action_id,
                                        ltsw_flexstate_grp_action_t **grp_action,
                                        ltsw_flexstate_key_t *okey)
{
    ltsw_flexstate_grp_action_info_t *gai = flexstate_grp_action_info[unit];
    ltsw_flexstate_key_t key = {0};
    uint32_t min_id, max_id;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(grp_action, SHR_E_PARAM);

    FLEXSTATE_ID_DECOMPOSE(group_action_id, &key);

    if (key.type != FLEXSTATE_GRP_ACTION) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid group counter action %x\n"),
                   group_action_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id type %x\n"), key.type));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((key.stage < FS_MIN_STAGE) || (key.stage >= FS_MAX_STAGE)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid group counter action %x\n"),
                   group_action_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id stage %x\n"), key.stage));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (key.pipe < 0 || key.pipe >= flexstate_pipe_max[unit][key.stage]) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid group counter action %x\n"),
                   group_action_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id pipe %x\n"), key.pipe));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    min_id = gai->grp_action_min[key.stage];
    max_id = gai->grp_action_max[key.stage];
    if (key.id < min_id || key.id >= max_id) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid group counter action %x\n"),
                   group_action_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id %x\n"), key.id));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *grp_action = &(gai->grp_actions[key.stage][key.pipe][key.id]);
    if (!(*grp_action)->used) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Counter group action %x not found\n"),
                  group_action_id));

        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (okey) {
        sal_memcpy(okey, &key, sizeof(ltsw_flexstate_key_t));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \breif Flex state group action destroy function.
 *
 * This function is used to destroy a flex state group action.
 *
 * \param [in] unit Unit number.
 * \param [in] group_action_id Flex state group ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_group_action_destroy(int unit, uint32_t group_action_id)
{
    ltsw_flexstate_grp_action_t *grp_act = NULL;
    ltsw_flexstate_action_t *par_action = NULL, *action = NULL;
    ltsw_flexstate_key_t key = {0}, akey = {0};
    int i;

    SHR_FUNC_ENTER(unit);

    FLEXSTATE_GRP_ACTION_LOCK(unit);
    FLEXSTATE_ACTION_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_counter_id_to_grp_action(unit, group_action_id,
                                                 &grp_act, &key));

    if (grp_act->ref_count != 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Counter action %x is in using\n"),
                   group_action_id));
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_counter_id_to_action(unit, grp_act->parent_act_id,
                                             &par_action, &akey));
    par_action->group_action_id = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_group_action_op(unit, &key, grp_act, BCMLT_OPCODE_DELETE));

    /* Last update action reference count. */
    for (i = 0; i < grp_act->action_num; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_counter_id_to_action(unit, grp_act->action_id[i],
                                                 &action, NULL));
        action->grp_ref_count--;
    }

    /* Reset group action structure. */
    sal_memset(grp_act, 0, sizeof(ltsw_flexstate_grp_action_t));

exit:
    FLEXSTATE_ACTION_UNLOCK(unit);
    FLEXSTATE_GRP_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \breif Flex state group action get function.
 *
 * This function is used to get a flex state group action information.
 *
 * \param [in] unit Unit number.
 * \param [in] group_action_id Flex state group ID.
 * \param [out] group_action Flex state group action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_group_action_get(int unit,
                                uint32_t group_action_id,
                                bcm_flexstate_group_action_t *group_action)
{
    ltsw_flexstate_key_t key = {0};
    ltsw_flexstate_grp_action_t *grp_act = NULL;
    bcm_port_config_t port_config;
    int i;

    SHR_FUNC_ENTER(unit);

    FLEXSTATE_GRP_ACTION_LOCK(unit);

    SHR_NULL_CHECK(group_action, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_counter_id_to_grp_action(unit, group_action_id,
                                                 &grp_act, &key));

    sal_memset(group_action, 0, sizeof(bcm_flexstate_group_action_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_port_config_get(unit, &port_config));
    BCM_PBMP_ASSIGN(group_action->ports, port_config.per_pp_pipe[key.pipe]);

    group_action->source = grp_act->source;
    group_action->action_num = grp_act->action_num;

    for (i = 0; i < grp_act->action_num; i++) {
        group_action->action_id[i] = grp_act->action_id[i];
    }

exit:
    FLEXSTATE_GRP_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state group action traverse function.
 *
 * This function is used to traverse all flex state group action information.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn Group action traverse callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_group_action_traverse(int unit,
                                     bcm_flexstate_group_action_traverse_cb trav_fn,
                                     void *user_data)
{
    ltsw_flexstate_grp_action_info_t *gai = flexstate_grp_action_info[unit];
    ltsw_flexstate_grp_action_t *grp_act = NULL;
    ltsw_flexstate_key_t key = {0};
    bcm_flexstate_group_action_t grp_action;
    uint32_t min_id, max_id;
    int max_pipe, group_action_id, rv;

    SHR_FUNC_ENTER(unit);

    FLEXSTATE_GRP_ACTION_LOCK(unit);

    SHR_NULL_CHECK(trav_fn, SHR_E_PARAM);

    key.type = FLEXSTATE_GRP_ACTION;
    for (key.stage = FS_MIN_STAGE; key.stage < FS_MAX_STAGE; key.stage++) {
        max_pipe = flexstate_pipe_max[unit][key.stage];
        min_id = gai->grp_action_min[key.stage];
        max_id = gai->grp_action_max[key.stage];

        for (key.pipe = 0; key.pipe < max_pipe; key.pipe++) {
            for (key.id = min_id; key.id < max_id; key.id++) {
                grp_act= &(gai->grp_actions[key.stage][key.pipe][key.id]);

                if (!grp_act->used) {
                    continue;
                }

                /* Skip internal actions. */
                if (grp_act->options & BCMI_LTSW_FLEXSTATE_OPTIONS_INTERNAL) {
                    continue;
                }

                FLEXSTATE_ID_COMPOSE(&key, group_action_id);

                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexstate_group_action_get(unit, group_action_id,
                                                     &grp_action));

                rv = trav_fn(unit, group_action_id, &grp_action, user_data);
                if (SHR_FAILURE(rv)) {
                    break;
                }
            }
        }
    }

exit:
    FLEXSTATE_GRP_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state trigger enable update function.
 *
 * This function is used to update trigger field of
 * CTR_ING/EGR_EFLEX_TRIGGER LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex state key identification.
 * \param [in] enable Enable/disable action trigger.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_trigger_enable_update(int unit, ltsw_flexstate_key_t *key, int enable)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmint_flexstate_stage_info_t *st = &(st_info[unit][key->stage]);
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t data;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Counter trigger lt update %d %d %u\n"),
              key->stage, key->pipe, key->id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->trigger, &eh));

    /* Key fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_key_to_id(unit, key, &data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->act_profile_id, data));

    data = enable ? 1 : 0;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, TRIGGERs, data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(eh));

    (void)bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state trigger enable lookup function.
 *
 * This function is used to lookup trigger field of
 * CTR_ING/EGR_EFLEX_TRIGGER LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex state key identification.
 * \param [out] enable Enable/disable action trigger.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_trigger_enable_lookup(int unit, ltsw_flexstate_key_t *key, int *enable)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmint_flexstate_stage_info_t *st = &(st_info[unit][key->stage]);
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t data;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Counter trigger lt lookup %d %d %u\n"),
              key->stage, key->pipe, key->id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->trigger, &eh));

    /* Key fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_key_to_id(unit, key, &data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->act_profile_id, data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, TRIGGERs, &data));
    *enable = (int)data;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(eh));

    (void)bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state trigger enable get set function.
 *
 * This function is used to get or set a trigger enable/disable status.
 *
 * \param [in] unit Unit number.
 * \param [in] operation Flex state operation.
 * \param [in] action_id Flex state ID.
 * \param [in/out] enable Enable/disable action trigger.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_trigger_enable_get_set(int unit,
                                      int operation,
                                      uint32_t action_id,
                                      int *enable)
{
    ltsw_flexstate_action_t *action = NULL;
    ltsw_flexstate_key_t key = {0};
    int trigger;

    SHR_FUNC_ENTER(unit);

    FLEXSTATE_ACTION_LOCK(unit);

    SHR_NULL_CHECK(enable, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_counter_id_to_action(unit, action_id, &action, &key));

    trigger = action->flags & BCM_FLEXSTATE_ACTION_FLAGS_TRIGGER;

    if (!trigger) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Trigger is unavailable in action %x\n"),
                   action_id));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    switch (operation) {
        case FLEXSTATE_OPERATION_GET:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_trigger_enable_lookup(unit, &key, enable));
            break;

        case FLEXSTATE_OPERATION_SET:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_trigger_enable_update(unit, &key, *enable));
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    FLEXSTATE_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Update an entry from LT.
 *
 * This function is used to update an entry from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex state key identification.
 * \param [in] counter_index Counter index of the given counter action.
 * \param [in] mode Flex state counter mode.
 * \param [in] counter_value Statistic for the specific counter entry.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_stats_update(int unit,
                            ltsw_flexstate_key_t *key,
                            uint32_t counter_index,
                            bcm_flexstate_counter_mode_t mode,
                            bcm_flexstate_counter_value_t *counter_value)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmint_flexstate_stage_info_t *st = &(st_info[unit][key->stage]);
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t data;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Counter stats lt update %d action %d %u index %u\n"),
              key->stage, key->pipe, key->id, counter_index));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->stats, &eh));

    /* Key fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_key_to_id(unit, key, &data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->act_profile_id, data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, CTR_EFLEX_INDEXs, counter_index));

    /* Data fields */
    switch (mode) {
        case bcmFlexstateCounterModeNormal:
            /* CTR_B and CTR_A_LOWER fields are valid */
            data = counter_value->value[0];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, CTR_A_LOWERs, data));

            data = counter_value->value[1];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, CTR_Bs, data));
            break;

        case bcmFlexstateCounterModeWide:
            /* CTR_A_LOWER and CTR_A_UPPER fields are valid */
            data = counter_value->value[0];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, CTR_A_UPPERs, data));

            data = counter_value->value[1];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, CTR_A_LOWERs, data));
            break;

        default:
            break;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_UPDATE,
                              BCMLT_PRIORITY_NORMAL));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(eh));

    (void)bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup an entry from LT.
 *
 * This function is used to lookup an entry from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex state key identification.
 * \param [in] counter_index Counter index of the given counter action.
 * \param [in] sync_mode Sync mode.
 * \param [in] mode Flex state counter mode.
 * \param [out] counter_value Statistic for the specific counter entry.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_stats_lookup(int unit,
                            ltsw_flexstate_key_t *key,
                            uint32_t counter_index,
                            bool sync_mode,
                            bcm_flexstate_counter_mode_t mode,
                            bcm_flexstate_counter_value_t *counter_value)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmint_flexstate_stage_info_t *st = &(st_info[unit][key->stage]);
    uint32_t entry_attr;
    uint64_t data;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Counter stats lt lookup %d action %d %u index %u\n"),
              key->stage, key->pipe, key->id, counter_index));

    sal_memset(counter_value, 0, sizeof(bcm_flexstate_counter_value_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->stats, &eh));

    /* Key fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_key_to_id(unit, key, &data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->act_profile_id, data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, CTR_EFLEX_INDEXs, counter_index));

    entry_attr = sync_mode ? BCMLT_ENT_ATTR_GET_FROM_HW : 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_attrib_set(eh, entry_attr));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Data fields */
    switch (mode) {
        case bcmFlexstateCounterModeNormal:
            /* CTR_B and CTR_A_LOWER fields are valid */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, CTR_A_LOWERs, &data));
            counter_value->value[0] = data;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, CTR_Bs, &data));
            counter_value->value[1] = data;
            break;

        case bcmFlexstateCounterModeWide:
            /* CTR_A_LOWER and CTR_A_UPPER fields are valid */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, CTR_A_UPPERs, &data));
            counter_value->value[0] = data;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, CTR_A_LOWERs, &data));
            counter_value->value[1] = data;
            break;

        default:
            break;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(eh));

    (void)bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state stats parameter validation function.
 *
 * This function is used to validate stats parameter.
 *
 * \param [in] unit Unit number.
 * \param [in] action_id Flex state ID.
 * \param [in] num_entries Total number of counter entries need to get or set.
 * \param [in] counter_indexes Each counter entry index of the given counter action.
 * \param [in] counter_values Statistic for the specific counter entry.
 * \param [out] mode Flex state counter mode.
 * \param [out] key Flex state key identification.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_stats_params_validate(int unit,
                                     uint32_t action_id,
                                     uint32_t num_entries,
                                     uint32_t *counter_indexes,
                                     bcm_flexstate_counter_value_t *counter_values,
                                     bcm_flexstate_counter_mode_t *mode,
                                     ltsw_flexstate_key_t *key)
{
    ltsw_flexstate_action_t *action = NULL;
    int i;

    SHR_FUNC_ENTER(unit);

    FLEXSTATE_ACTION_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_counter_id_to_action(unit, action_id, &action, key));

    *mode = action->mode;

    for (i = 0; i < num_entries; i++) {
        if (counter_indexes[i] >= action->index_num) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "counter_index[%d] = %u "
                                        "exceeds total indexes %u\n"),
                       i, counter_indexes[i], action->index_num));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    FLEXSTATE_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state statistic get set function.
 *
 * This function is used to get or set the specific statistic
 * of a given counter action.
 *
 * \param [in] unit Unit number.
 * \param [in] operation Flex state operation.
 * \param [in] action_id Flex state ID.
 * \param [in] sync Sync mode.
 * \param [in] num_entries Total number of counter entries need to get or set.
 * \param [in] counter_indexes Each counter entry index of the given counter action.
 * \param [in/out] counter_values Statistic for the specific counter entry.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_stats_get_set(int unit,
                             int operation,
                             uint32_t action_id,
                             bool sync,
                             uint32_t num_entries,
                             uint32_t *counter_indexes,
                             bcm_flexstate_counter_value_t *counter_values)
{
    bcm_flexstate_counter_value_t *value = NULL;
    ltsw_flexstate_key_t key = {0};
    bcm_flexstate_counter_mode_t mode;
    int i;
    uint32_t index;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(counter_indexes, SHR_E_PARAM);
    SHR_NULL_CHECK(counter_values, SHR_E_PARAM);

    /* Parameter check. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_stats_params_validate(unit,
                                              action_id,
                                              num_entries,
                                              counter_indexes,
                                              counter_values,
                                              &mode, &key));

    for (i = 0; i < num_entries; i++) {
        index = counter_indexes[i];
        value = &counter_values[i];

        switch (operation) {
            case FLEXSTATE_OPERATION_GET:
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexstate_stats_lookup(unit, &key, index,
                                                 sync, mode, value));
                break;

            case FLEXSTATE_OPERATION_SET:
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexstate_stats_update(unit, &key, index, mode, value));
                break;

            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize flex state pipe mode.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_pipe_mode_init(int unit)
{
    int stage;

    SHR_FUNC_ENTER(unit);

    for (stage = FS_MIN_STAGE; stage < FS_MAX_STAGE; stage++) {
        /* Flex state only supports pipe unique mode. */
        flexstate_pipe_max[unit][stage] = bcmi_ltsw_dev_max_pp_pipe_num(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize flex state misc.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_misc_init(int unit)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    int stage, max_pipe, pipe;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int warm = bcmi_warmboot_get(unit);
    bool instance_valid = FALSE;

    SHR_FUNC_ENTER(unit);

    if (warm) {
        SHR_EXIT();
    }

    for (stage = FS_MIN_STAGE; stage < FS_MAX_STAGE; stage++) {
        max_pipe = flexstate_pipe_max[unit][stage];

        for (pipe = 0; pipe < max_pipe; pipe++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_allocate(dunit, st_info[unit][stage].err_stats, &eh));

            /* Key fields */
            /* Check INSTANCE field. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_lt_field_validation
                     (unit, st_info[unit][stage].err_stats, INSTANCEs,
                      &instance_valid));
            if (instance_valid) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_symbol_add(eh, INSTANCEs,
                                                  st_info[unit][stage].inst));
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, PIPEs, pipe));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

            (void)bcmlt_entry_free(eh);
            eh = BCMLT_INVALID_HDL;
        }
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach the flex state module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_detach(int unit)
{

    SHR_FUNC_ENTER(unit);

    /* Set initialized flag. */
    flexstate_initialized[unit] = FALSE;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_pool_db_cleanup(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_quantization_db_cleanup(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_action_db_cleanup(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_op_profile_entry_cleanup(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_group_action_db_cleanup(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initializes the flex state module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_init(int unit)
{
    int warm = bcmi_warmboot_get(unit), stage;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexstate_stage_info_get(unit, &st_info[unit]));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_detach(unit));

    if (!warm) {
        for (stage = FS_MIN_STAGE; stage < FS_MAX_STAGE; stage++) {
            /* Reset all LTs. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_clear(unit, st_info[unit][stage].obj_quant_cfg));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_clear(unit, st_info[unit][stage].range_chk_profile));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_clear(unit, st_info[unit][stage].act_profile));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_clear(unit, st_info[unit][stage].grp_act_profile));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_clear(unit, st_info[unit][stage].operand_profile));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_clear(unit, st_info[unit][stage].stats));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_clear(unit, st_info[unit][stage].trigger));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_clear(unit, st_info[unit][stage].err_stats));
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_pipe_mode_init(unit));

    /* Initialize pool information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_pool_db_init(unit));

    /* Initialize quantization information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_quantization_db_init(unit));

    /* Initialize action information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_action_db_init(unit));

    /* Initialize operation profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_op_profile_entry_init(unit));

    /* Initialize group action information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_group_action_db_init(unit));

    /* Initialize misc. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexstate_misc_init(unit));

    /* Set initialized flag. */
    flexstate_initialized[unit] = TRUE;

exit:
    if (SHR_FUNC_ERR()) {
        (void)ltsw_flexstate_detach(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state info get function.
 *
 * This function is used to get a counter action info.
 *
 * \param [in] unit Unit number.
 * \param [in] action_id Flex state ID.
 * \param [out] info Flex state action information.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_counter_info_get(int unit,
                             uint32_t action_id,
                             bcmi_ltsw_flexstate_counter_info_t *info)
{
    ltsw_flexstate_grp_action_t *grp_act = NULL;
    ltsw_flexstate_action_t *action = NULL;
    ltsw_flexstate_key_t key = {0};
    bcmint_flexstate_source_info_t *si = NULL;
    int type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    type = FLEXSTATE_ID_TYPE(action_id);

    switch (type) {
        case FLEXSTATE_GRP_ACTION:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_counter_id_to_grp_action(unit, action_id,
                                                         &grp_act, &key));

            info->stage = key.stage;
            info->action_index = FLEXSTATE_ID_INDEX(grp_act->parent_act_id);

            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_key_to_id(unit, &key, &(info->action_profile_id)));

            info->source = grp_act->source;

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexstate_source_info_get(unit, info->source, &si));
            info->table_name = si->table;

            break;

        case FLEXSTATE_ACTION:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_counter_id_to_action(unit, action_id,
                                                     &action, &key));

            if (action->group_action_id) {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit, "Attaching a group action %x first member %x\n"),
                          action->group_action_id, action_id));
            }

            info->stage = key.stage;
            info->action_index = key.id;

            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_key_to_id(unit, &key, &(info->action_profile_id)));

            info->source = action->source;

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexstate_source_info_get(unit, info->source, &si));
            info->table_name = si->table;

            info->start_pool = action->start_pool_idx;
            info->end_pool = action->end_pool_idx;

            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state action id get function.
 *
 * This function is used to get a counter action info.
 *
 * \param [in] unit Unit number.
 * \param [out] info Flex state action information.
 * \param [out] action_id Flex state ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_action_id_get(int unit,
                             bcmi_ltsw_flexstate_counter_info_t *info,
                             uint32_t *action_id)
{
    ltsw_flexstate_action_info_t *ai = flexstate_action_info[unit];
    ltsw_flexstate_action_t *action = NULL;
    ltsw_flexstate_key_t key = {0};
    uint32_t min_id, max_id;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(action_id, SHR_E_PARAM);

    key.id = info->action_index;
    key.pipe = info->pipe;
    key.stage = info->stage;

    /* Check input info. */
    if ((key.stage < FS_MIN_STAGE) || (key.stage >= FS_MAX_STAGE)) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id stage %x\n"), key.stage));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (key.pipe < 0 || key.pipe >= flexstate_pipe_max[unit][key.stage]) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id pipe %x\n"), key.pipe));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    min_id = ai->action_min[key.stage];
    max_id = ai->action_max[key.stage];
    if (key.id < min_id || key.id >= max_id) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id %x\n"), key.id));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    action = &(ai->actions[key.stage][key.pipe][key.id]);

    if (!action->used) {
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (action->group_action_id) {
        /* Used by a group action */
        *action_id = action->group_action_id;
    } else {
        key.type = FLEXSTATE_ACTION;

        /* Compose action id. */
        FLEXSTATE_ID_COMPOSE(&key, *action_id);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state attach action status update function.
 *
 * This function is used to update an attached action status.
 *
 * \param [in] unit Unit number.
 * \param [in] action_id Flex state ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_attach_action_status_update(int unit, uint32_t action_id)
{
    ltsw_flexstate_grp_action_t *grp_act = NULL;
    ltsw_flexstate_action_t *action = NULL;
    int type = 0;

    SHR_FUNC_ENTER(unit);

    FLEXSTATE_GRP_ACTION_LOCK(unit);
    FLEXSTATE_ACTION_LOCK(unit);

    type = FLEXSTATE_ID_TYPE(action_id);

    switch (type) {
        case FLEXSTATE_GRP_ACTION:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_counter_id_to_grp_action(unit, action_id,
                                                         &grp_act, NULL));
            grp_act->ref_count++;
            break;

        case FLEXSTATE_ACTION:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_counter_id_to_action(unit, action_id,
                                                     &action, NULL));

            action->ref_count++;
            break;
    }

exit:
    FLEXSTATE_ACTION_UNLOCK(unit);
    FLEXSTATE_GRP_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state detach action status update function.
 *
 * This function is used to update a detached action status.
 *
 * \param [in] unit Unit number.
 * \param [in] action_id Flex state ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_detach_action_status_update(int unit, uint32_t action_id)
{
    ltsw_flexstate_grp_action_t *grp_act = NULL;
    ltsw_flexstate_action_t *action = NULL;
    int type = 0;

    SHR_FUNC_ENTER(unit);

    FLEXSTATE_GRP_ACTION_LOCK(unit);
    FLEXSTATE_ACTION_LOCK(unit);

    type = FLEXSTATE_ID_TYPE(action_id);

    switch (type) {
        case FLEXSTATE_GRP_ACTION:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_counter_id_to_grp_action(unit, action_id,
                                                         &grp_act, NULL));
            grp_act->ref_count--;
            break;

        case FLEXSTATE_ACTION:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_counter_id_to_action(unit, action_id,
                                                     &action, NULL));

            action->ref_count--;
            break;
    }

exit:
    FLEXSTATE_ACTION_UNLOCK(unit);
    FLEXSTATE_GRP_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state software info dump function.
 *
 * This function is used to dump flex state software info.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexstate_sw_dump(int unit)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    int stage, max_pipe, pipe, pidx, i;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t misconfig, too_many;
    ltsw_flexstate_pool_info_t *pi = flexstate_pool_info[unit];
    ltsw_flexstate_pool_t *pool = NULL;
    bool instance_valid = FALSE;

    SHR_FUNC_ENTER(unit);

    LOG_CLI((BSL_META_U(unit, "Flex state information:\n\n")));

    for (stage = FS_MIN_STAGE; stage < FS_MAX_STAGE; stage++) {
        max_pipe = flexstate_pipe_max[unit][stage];

        LOG_CLI((BSL_META_U(unit, "==%s==\n"), st_info[unit][stage].name));

        for (pipe = 0; pipe < max_pipe; pipe++) {
            LOG_CLI((BSL_META_U(unit, "== Pipe %d ==\n"), pipe));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_allocate(dunit, st_info[unit][stage].err_stats, &eh));

            /* Key fields */
            /* Check INSTANCE field. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexstate_lt_field_validation
                     (unit, st_info[unit][stage].err_stats, INSTANCEs,
                      &instance_valid));
            if (instance_valid) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_symbol_add(eh, INSTANCEs,
                                                  st_info[unit][stage].inst));
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, PIPEs, pipe));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                                      BCMLT_PRIORITY_NORMAL));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, ACTION_MISCONFIGs, &misconfig));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, TOO_MANY_ACTIONSs, &too_many));

            (void)bcmlt_entry_free(eh);
            eh = BCMLT_INVALID_HDL;

            LOG_CLI((BSL_META_U(unit, "\tMisconfig Action: %"PRIu64"\n"),
                    misconfig));
            LOG_CLI((BSL_META_U(unit, "\tToo Many Action: %"PRIu64"\n"),
                    too_many));

            LOG_CLI((BSL_META_U(unit, "  == Pool info ==\n")));
            LOG_CLI((BSL_META_U(unit, "\tTotal Pool: %u Pool Counter: %u"
                                      " Pool Section: %u Section Counter: %u\n"),
                    (uint32_t)pi->npools[stage],
                    (uint32_t)pi->nctrs_in_pool[stage],
                    (uint32_t)pi->nsects[stage],
                    (uint32_t)pi->nctrs_in_sect[stage]));

            for (pidx = 0; pidx < pi->npools[stage]; pidx++) {
                pool = &(pi->pools[stage][pipe][pidx]);

                LOG_CLI((BSL_META_U(unit,
                                    "\t\tPool: %2d Mode: %d"
                                    " Hint: %d Use_count: %u\n"),
                        pidx, pool->mode, pool->hint,
                        (uint32_t)pool->used_count));

                LOG_CLI((BSL_META_U(unit, "\t\t         Used by: ")));
                SHR_BIT_ITER(pool->used_by_tbls, FLEXSTATE_NUM_TBLS_MAX, i) {
                    LOG_CLI((BSL_META_U(unit, "%2d "), i));
                }
                LOG_CLI((BSL_META_U(unit, "\n")));

                LOG_CLI((BSL_META_U(unit, "\t\t         Inuse section: ")));
                SHR_BIT_ITER(pool->inuse_bitmap, FLEXSTATE_NUM_SECTION_MAX, i) {
                    LOG_CLI((BSL_META_U(unit, "%2d "), i));
                }
                LOG_CLI((BSL_META_U(unit, "\n")));
            }
            LOG_CLI((BSL_META_U(unit, "\n")));
        }
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

/*!
 * \brief Initializes the flex state module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexstate_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXSTATE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_init(unit));
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief De-initializes the flex state module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexstate_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXSTATE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_detach(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state quantization creation function.
 *
 * This function is used to create a flex state quantization.
 *
 * \param [in] unit Unit number.
 * \param [in] options BCM_FLEXSTATE_OPTIONS_XXX options.
 * \param [in] quantization Flex state quantization data structure.
 * \param [in/out] quant_id Flex state quantization ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexstate_quantization_create(int unit,
                                       int options,
                                       bcm_flexstate_quantization_t *quantization,
                                       uint32_t *quant_id)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXSTATE)) {

        if (flexstate_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_quantization_create(unit,
                                                options,
                                                quantization,
                                                quant_id));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state quantization destroy function.
 *
 * This function is used to destroy a flex state quantization.
 *
 * \param [in] unit Unit number.
 * \param [in] quant_id Flex state quantization ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexstate_quantization_destroy(int unit, uint32_t quant_id)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXSTATE)) {

        if (flexstate_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }


        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_quantization_destroy(unit, quant_id));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state quantization get function.
 *
 * This function is used to get a flex state quantization information.
 *
 * \param [in] unit Unit number.
 * \param [in] quant_id Flex state quantization ID.
 * \param [out] quantization Flex state quantization data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexstate_quantization_get(int unit,
                                    uint32_t quant_id,
                                    bcm_flexstate_quantization_t *quantization)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXSTATE)) {

        if (flexstate_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_quantization_get(unit, quant_id, quantization));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state quantization traverse function.
 *
 * This function is used to traverse all flex state quantization
 * information.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn Quantization traverse callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int bcm_ltsw_flexstate_quantization_traverse(
                                int unit,
                                bcm_flexstate_quantization_traverse_cb trav_fn,
                                void *user_data)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXSTATE)) {

        if (flexstate_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_quantization_traverse(unit, trav_fn, user_data));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state action creation function.
 *
 * This function is used to create a flex state action.
 *
 * \param [in] unit Unit number.
 * \param [in] options BCM_FLEXSTATE_OPTIONS_XXX options.
 * \param [in] action Flex state action data structure.
 * \param [in/out] action_id Flex state ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexstate_action_create(int unit,
                                 int options,
                                 bcm_flexstate_action_t *action,
                                 uint32_t *action_id)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXSTATE)) {

        if (flexstate_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_action_create(unit,
                                          options,
                                          action,
                                          action_id));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state action destroy function.
 *
 * This function is used to destroy a flex state action.
 *
 * \param [in] unit Unit number.
 * \param [in] action_id Flex state ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexstate_action_destroy(int unit,
                                  uint32_t action_id)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXSTATE)) {

        if (flexstate_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_action_destroy(unit, action_id));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state action get function.
 *
 * This function is used to get a flex state action information.
 *
 * \param [in] unit Unit number.
 * \param [in] action_id Flex state ID.
 * \param [out] action Flex state action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexstate_action_get(int unit,
                              uint32_t action_id,
                              bcm_flexstate_action_t *action)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXSTATE)) {

        if (flexstate_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_action_get(unit, action_id, action));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state action traverse function.
 *
 * This function is used to traverse all flex state action information.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn Action traverse callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int bcm_ltsw_flexstate_action_traverse(int unit,
                                       bcm_flexstate_action_traverse_cb trav_fn,
                                       void *user_data)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXSTATE)) {

        if (flexstate_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_action_traverse(unit, trav_fn, user_data));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state group action creation function.
 *
 * This function is used to create a flex state group action.
 *
 * \param [in] unit Unit number.
 * \param [in] options BCM_FLEXSTATE_OPTIONS_XXX options.
 * \param [in] group_action Flex state group action data structure.
 * \param [in/out] group_action_id Flex state group action ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexstate_group_action_create(int unit,
                                       int options,
                                       bcm_flexstate_group_action_t *group_action,
                                       uint32_t *group_action_id)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXSTATE)) {

        if (flexstate_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_group_action_create(unit,
                                                options,
                                                group_action,
                                                group_action_id));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state group action destroy function.
 *
 * This function is used to destroy a flex state group action.
 *
 * \param [in] unit Unit number.
 * \param [in] group_action_id Flex state group action ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexstate_group_action_destroy(int unit,
                                        uint32_t group_action_id)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXSTATE)) {

        if (flexstate_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_group_action_destroy(unit, group_action_id));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state group action get function.
 *
 * This function is used to get a flex state group action information.
 *
 * \param [in] unit Unit number.
 * \param [in] group_action_id Flex state group action ID.
 * \param [out] group_action Flex state group action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexstate_group_action_get(int unit,
                                    uint32_t group_action_id,
                                    bcm_flexstate_group_action_t *group_action)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXSTATE)) {

        if (flexstate_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_group_action_get(unit,
                                             group_action_id,
                                             group_action));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state group action traverse function.
 *
 * This function is used to traverse all flex state group action information.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn Group action traverse callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int bcm_ltsw_flexstate_group_action_traverse(
                                    int unit,
                                    bcm_flexstate_group_action_traverse_cb trav_fn,
                                    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXSTATE)) {

        if (flexstate_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_group_action_traverse(unit, trav_fn, user_data));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state trigger enable set function.
 *
 * This function is used to enable/disable a trigger from an existing
 * flex state action.
 *
 * \param [in] unit Unit number.
 * \param [in] action_id Flex state ID.
 * \param [in] enable Enable/disable action trigger.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexstate_trigger_enable_set(int unit,
                                      uint32_t action_id,
                                      int enable)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXSTATE)) {

        if (flexstate_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_trigger_enable_get_set(unit,
                                                   FLEXSTATE_OPERATION_SET,
                                                   action_id,
                                                   &enable));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state trigger enable get function.
 *
 * This function is used to get a trigger enable/disable status from
 * a flex state action.
 *
 * \param [in] unit Unit number.
 * \param [in] action_id Flex state ID.
 * \param [out] enable Enable/disable action trigger.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexstate_trigger_enable_get(int unit,
                                      uint32_t action_id,
                                      int *enable)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXSTATE)) {

        if (flexstate_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_trigger_enable_get_set(unit,
                                                   FLEXSTATE_OPERATION_GET,
                                                   action_id,
                                                   enable));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state statistic get function.
 *
 * This function is used to get the specific statistic of a given counter action.
 *
 * \param [in] unit Unit number.
 * \param [in] action_id Flex state ID.
 * \param [in] num_entries Total number of counter entries need to get.
 * \param [in] counter_indexes Each counter entry index of the given counter action.
 * \param [out] counter_values Statistic for the specific counter entry.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int bcm_ltsw_flexstate_stat_get(int unit,
                                uint32_t action_id,
                                uint32_t num_entries,
                                uint32_t *counter_indexes,
                                bcm_flexstate_counter_value_t *counter_values)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXSTATE)) {

        if (flexstate_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_stats_get_set(unit,
                                          FLEXSTATE_OPERATION_GET,
                                          action_id,
                                          FALSE,
                                          num_entries,
                                          counter_indexes,
                                          counter_values));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state statistic sync get function.
 *
 * This function is used to get the specific statistic of a given counter action.
 *
 * \param [in] unit Unit number.
 * \param [in] action_id Flex state ID.
 * \param [in] num_entries Total number of counter entries need to get.
 * \param [in] counter_indexes Each counter entry index of the given counter action.
 * \param [out] counter_values Statistic for the specific counter entry.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int bcm_ltsw_flexstate_stat_sync_get(int unit,
                                     uint32_t action_id,
                                     uint32_t num_entries,
                                     uint32_t *counter_indexes,
                                     bcm_flexstate_counter_value_t *counter_values)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXSTATE)) {

        if (flexstate_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_stats_get_set(unit,
                                          FLEXSTATE_OPERATION_GET,
                                          action_id,
                                          TRUE,
                                          num_entries,
                                          counter_indexes,
                                          counter_values));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex state statistic set function.
 *
 * This function is used to set the specific statistic of a given counter action.
 *
 * \param [in] unit Unit number.
 * \param [in] action_id Flex state ID.
 * \param [in] num_entries Total number of counter entries need to get.
 * \param [in] counter_indexes Each counter entry index of the given counter action.
 * \param [in] counter_values Statistic for the specific counter entry.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int bcm_ltsw_flexstate_stat_set(int unit,
                                uint32_t action_id,
                                uint32_t num_entries,
                                uint32_t *counter_indexes,
                                bcm_flexstate_counter_value_t *counter_values)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXSTATE)) {

        if (flexstate_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_stats_get_set(unit,
                                          FLEXSTATE_OPERATION_SET,
                                          action_id,
                                          FALSE,
                                          num_entries,
                                          counter_indexes,
                                          counter_values));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_flexstate_counter_id_info_get(int unit,
                                        uint32_t action_id,
                                        bcmi_ltsw_flexstate_counter_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXSTATE)) {

        if (flexstate_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_counter_info_get(unit, action_id, info));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_flexstate_counter_id_get(int unit,
                                   bcmi_ltsw_flexstate_counter_info_t *info,
                                   uint32_t *action_id)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXSTATE)) {

        if (flexstate_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_action_id_get(unit, info, action_id));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_flexstate_attach_counter_id_status_update(int unit,
                                                    uint32_t action_id)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXSTATE)) {

        if (flexstate_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_attach_action_status_update(unit, action_id));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_flexstate_detach_counter_id_status_update(int unit,
                                                    uint32_t action_id)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXSTATE)) {

        if (flexstate_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_detach_action_status_update(unit, action_id));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_flexstate_sw_dump(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXSTATE)) {

        if (flexstate_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexstate_sw_dump(unit));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:

    SHR_FUNC_EXIT();
}
