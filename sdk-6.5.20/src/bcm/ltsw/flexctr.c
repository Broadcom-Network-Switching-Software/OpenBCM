/*! \file flexctr.c
 *
 * Flex Counter Module Emulator above SDKLT.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bsl/bsl.h>
#include <sal/core/boot.h>

#include <bcm/types.h>
#include <bcm/flexctr.h>
#include <bcm_int/control.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/profile_mgmt.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/issu.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/field.h>
#include <bcm_int/ltsw/flexctr.h>
#include <bcm_int/ltsw/qos.h>
#include <bcm_int/ltsw/mbcm/flexctr.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/generated/flexctr_ha.h>

#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <sal/sal_mutex.h>
#include <bcmlt/bcmlt.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
* Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_FLEXCTR

/*!
 * \brief Composition of return counter id.
 *
 * bits [ 7: 0] index   (maximum 256 indexes)
 * bits [12: 8] pipe    (maximum 32 pipes)
 * bits [15:13] stage   (ing_ctr, egr_ctr)
 *
 * bits [30:28] type    (action, group action, quant, op profile(internal))
 */
#define FLEXCTR_START_BIT_INDEX     0
#define FLEXCTR_END_BIT_INDEX       7
#define FLEXCTR_START_BIT_PIPE      8
#define FLEXCTR_END_BIT_PIPE        12
#define FLEXCTR_START_BIT_STAGE     13
#define FLEXCTR_END_BIT_STAGE       15
#define FLEXCTR_START_BIT_TYPE      28
#define FLEXCTR_END_BIT_TYPE        30

#define FLEXCTR_MASK_INDEX      \
    ((1 << (FLEXCTR_END_BIT_INDEX - FLEXCTR_START_BIT_INDEX  + 1)) - 1)
#define FLEXCTR_MASK_PIPE       \
    ((1 << (FLEXCTR_END_BIT_PIPE - FLEXCTR_START_BIT_PIPE + 1)) - 1)
#define FLEXCTR_MASK_STAGE      \
    ((1 << (FLEXCTR_END_BIT_STAGE - FLEXCTR_START_BIT_STAGE + 1)) - 1)
#define FLEXCTR_MASK_TYPE       \
    ((1 << (FLEXCTR_END_BIT_TYPE - FLEXCTR_START_BIT_TYPE + 1)) - 1)

#define FLEXCTR_ID_INDEX(_id)   \
    (((_id) >> FLEXCTR_START_BIT_INDEX) & FLEXCTR_MASK_INDEX)
#define FLEXCTR_ID_PIPE(_id)    \
    (((_id) >> FLEXCTR_START_BIT_PIPE) & FLEXCTR_MASK_PIPE)
#define FLEXCTR_ID_STAGE(_id)   \
    (((_id) >> FLEXCTR_START_BIT_STAGE) & FLEXCTR_MASK_STAGE)
#define FLEXCTR_ID_TYPE(_id)    \
    (((_id) >> FLEXCTR_START_BIT_TYPE) & FLEXCTR_MASK_TYPE)

/*!
 * \brief Flex counter key information.
 */
typedef struct ltsw_flexctr_key_s {

    /*! Identification. */
    int id;

    /*! Pipe number. */
    int pipe;

    /*! Stage id. */
    int stage;

#define FLEXCTR_ACTION          1
#define FLEXCTR_GRP_ACTION      2
#define FLEXCTR_QUANT           3
#define FLEXCTR_OP_PROFILE      4
#define FLEXCTR_PKT_ATTR        5
    /*! Quant, Action, Group action, Packet attribute. */
    int type;

} ltsw_flexctr_key_t;

#define FLEXCTR_ID_COMPOSE(_pkey, _rid)                                         \
    {                                                                           \
        int _id = (_pkey)->id;                                                  \
        int _pipe = (_pkey)->pipe;                                              \
        int _stage = (_pkey)->stage;                                            \
        int _type = (_pkey)->type;                                              \
                                                                                \
        (_rid) = ((((_id) & FLEXCTR_MASK_INDEX) << FLEXCTR_START_BIT_INDEX) |   \
                  (((_pipe) & FLEXCTR_MASK_PIPE) << FLEXCTR_START_BIT_PIPE) |   \
                  (((_stage) & FLEXCTR_MASK_STAGE) << FLEXCTR_START_BIT_STAGE) |\
                  (((_type) & FLEXCTR_MASK_TYPE) << FLEXCTR_START_BIT_TYPE));   \
    }

#define FLEXCTR_ID_DECOMPOSE(_id, _pkey)                                            \
    {                                                                               \
        (_pkey)->id = (((_id) >> FLEXCTR_START_BIT_INDEX) & FLEXCTR_MASK_INDEX);    \
        (_pkey)->pipe = (((_id) >> FLEXCTR_START_BIT_PIPE) & FLEXCTR_MASK_PIPE);    \
        (_pkey)->stage = (((_id) >> FLEXCTR_START_BIT_STAGE) & FLEXCTR_MASK_STAGE); \
        (_pkey)->type = (((_id) >> FLEXCTR_START_BIT_TYPE) & FLEXCTR_MASK_TYPE);    \
    }

#define FC_ID_INVALID   BCMI_LTSW_FLEXCTR_ACTION_INVALID

#define FC_MIN_STAGE    BCMI_LTSW_FLEXCTR_STAGE_ING_CTR
#define FC_MAX_STAGE    BCMI_LTSW_FLEXCTR_STAGE_COUNT
#define FC_MAX_PIPE     8

/*! Flex counter get operation. */
#define FLEXCTR_OPERATION_GET               (0)

/*! Flex counter set operation. */
#define FLEXCTR_OPERATION_SET               (1)

/*! Flex counter quantization range check maximum value for 16-bit mode. */
#define FC_QUANT_RANGE_VALUE_MAX_16BIT 0xFFFF

/*! Flex counter quantization range check maximum value for 32-bit mode. */
#define FC_QUANT_RANGE_VALUE_MAX_32BIT 0xFFFFFFFF

/*! Flex counter quantization range check mask value for the lower 16 bits. */
#define FC_QUANT_RANGE_VALUE_MASK_16BIT_LOWER 0x0000FFFF

/*! Flex counter quantization range check mask value for the upper 16 bits. */
#define FC_QUANT_RANGE_VALUE_MASK_16BIT_UPPER 0xFFFF0000

/*! Flex counter quantization range check shift value for the upper 16 bits. */
#define FC_QUANT_RANGE_VALUE_SHIFT_16BIT_UPPER (16)

/*!
 * \brief Flex counter quantization information.
 */
typedef struct ltsw_flexctr_quant_info_s {

    /*! Min quantization id. */
    uint32_t quant_min[FC_MAX_STAGE];

    /*! Max quantization id. */
    uint32_t quant_max[FC_MAX_STAGE];

    /*! Quantization array. */
    ltsw_flexctr_quant_t *quants[FC_MAX_STAGE][FC_MAX_PIPE];

    /*! Protection mutex. */
    sal_mutex_t mutex;

    /*! Num of quantization reserved for global action in pipe unique mode. */
    int rsvd_num[FC_MAX_STAGE];

} ltsw_flexctr_quant_info_t;

/*! Per chip flex counter quantization information. */
static
ltsw_flexctr_quant_info_t *flexctr_quant_info[BCM_MAX_NUM_UNITS] = {0};

/*! Flex counter quantization operation lock. */
#define FLEXCTR_QUANT_LOCK(_u)                                          \
    sal_mutex_take(flexctr_quant_info[_u]->mutex, SAL_MUTEX_FOREVER)

/*! Flex counter quantization operation unlock. */
#define FLEXCTR_QUANT_UNLOCK(_u)                                        \
    sal_mutex_give(flexctr_quant_info[_u]->mutex)

/*! Quantization objects. */
#define QUANT_OBJ_ENUM_COUNT                (4)

/*! Quantization objects enumeration. */
static const
bcmint_flexctr_enum_map_t quant_obj_enum[QUANT_OBJ_ENUM_COUNT] = {
    { USE_RANGE_CHK_1s, 0x0 },
    { USE_RANGE_CHK_2s, 0x1 },
    { USE_RANGE_CHK_3s, 0x2 },
    { USE_RANGE_CHK_4s, 0x3 },
};

/*! Number of counters per section. */
#define NUM_SECT_CTRS                       (128)

/*!
 * \brief Flex counter pool info.
 */
typedef struct ltsw_flexctr_pool_info_s {

    /*! Number of pools. */
    size_t npools[FC_MAX_STAGE];

    /*! Number of sections. */
    size_t nsects[FC_MAX_STAGE];

    /*! Number of sections in each pool. */
    size_t nsects_in_pool[FC_MAX_STAGE];

    /*! Number of counters. */
    size_t nctrs[FC_MAX_STAGE];

    /*! Number of counters in each pool. */
    size_t nctrs_in_pool[FC_MAX_STAGE];

    /*! Number of counters in each section. */
    size_t nctrs_in_sect[FC_MAX_STAGE];

    /*! Pool array. */
    ltsw_flexctr_pool_t *pools[FC_MAX_STAGE][FC_MAX_PIPE];

    /*! Protection mutex. */
    sal_mutex_t mutex;

    /*! Num of pool reserved for global action in pipe unique mode. */
    int rsvd_num[FC_MAX_STAGE];

} ltsw_flexctr_pool_info_t;

/*! Per chip flex counter pool information. */
static
ltsw_flexctr_pool_info_t *flexctr_pool_info[BCM_MAX_NUM_UNITS] = {0};

/*! Flex counter pool operation lock. */
#define FLEXCTR_POOL_LOCK(_u)                                           \
    sal_mutex_take(flexctr_pool_info[_u]->mutex, SAL_MUTEX_FOREVER)

/*! Flex counter pool operation unlock. */
#define FLEXCTR_POOL_UNLOCK(_u)                                         \
    sal_mutex_give(flexctr_pool_info[_u]->mutex)

/*! Action mode enumeration count. */
#define ACTION_MODE_ENUM_COUNT              bcmFlexctrCounterModeCount

/*! Action mode enumeration. */
static const
bcmint_flexctr_enum_map_t action_mode_enum[ACTION_MODE_ENUM_COUNT] = {
    { NORMALs, 0 }, /* bcmFlexctrCounterModeNormal */
    { WIDEs, 1 },   /* bcmFlexctrCounterModeWide */
    { SLIMs, 2 },   /* bcmFlexctrCounterModeSlim */
};

/*! Action source enumeration count. */
#define ACTION_SRC_ENUM_COUNT               bcmFlexctrValueSelectCount

/*! Action mode enumeration. */
static const
bcmint_flexctr_enum_map_t action_src_enum[ACTION_SRC_ENUM_COUNT] = {
    { NOOPs, 0 },               /* bcmFlexctrValueSelectCounterValue */
    { USE_PKTLENs, 1 },         /* bcmFlexctrValueSelectPacketLength */
    { USE_PAIRED_COUNTERs, 2 }, /* bcmFlexctrValueSelectPairedCounter */
};

/*! Action update mode enumeration count. */
#define ACTION_UPDATE_MODE_ENUM_COUNT       bcmFlexctrValueOperationTypeCount

/*! Action update mode enumeration. */
static const
bcmint_flexctr_enum_map_t action_update_mode_enum[ACTION_UPDATE_MODE_ENUM_COUNT] = {
    { NOOPs, 0 },       /* bcmFlexctrValueOperationTypeNoop */
    { SETs, 1 },        /* bcmFlexctrValueOperationTypeSet */
    { INCs, 2 },        /* bcmFlexctrValueOperationTypeInc */
    { DECs, 3 },        /* bcmFlexctrValueOperationTypeDec */
    { MAXs, 4 },        /* bcmFlexctrValueOperationTypeMax */
    { MINs, 5 },        /* bcmFlexctrValueOperationTypeMin */
    { AVGs, 6 },        /* bcmFlexctrValueOperationTypeAverage */
    { SETBITs, 7 },     /* bcmFlexctrValueOperationTypeSetBit */
    { REV_DECs, 8 },    /* bcmFlexctrValueOperationTypeRdec */
    { XORs, 9 },        /* bcmFlexctrValueOperationTypeXor */
};

/*! Action drop enumeration count. */
#define ACTION_DROP_ENUM_COUNT              bcmFlexctrDropCountModeCount

/*! Action mode enumeration. */
static const
bcmint_flexctr_enum_map_t action_drop_enum[ACTION_DROP_ENUM_COUNT] = {
    { NO_DROP_COUNTs, 0 },      /* bcmFlexctrDropCountModeNoDrop */
    { DROP_COUNTs, 1 },         /* bcmFlexctrDropCountModeDrop */
    { ALWAYS_COUNTs, 2 },       /* bcmFlexctrDropCountModeAll */
};

/*! Flex counter action type. */
#define FC_ACTION_TYPE_SIMPLE  (0)  /* Simple (fixed) action. */
#define FC_ACTION_TYPE_FLEX    (1)  /* Flex action. */

/*!
 * \brief Flex counter action information.
 */
typedef struct ltsw_flexctr_action_info_s {

    /*! Min action profile id. */
    uint32_t profile_id_min[FC_MAX_STAGE];

    /*! Min action id. */
    uint32_t action_min[FC_MAX_STAGE];

    /*! Max action id. */
    uint32_t action_max[FC_MAX_STAGE];

    /*! Min op profile id. */
    uint32_t op_profile_min[FC_MAX_STAGE];

    /*! Max op profile id. */
    uint32_t op_profile_max[FC_MAX_STAGE];

    /*! Action array. */
    ltsw_flexctr_action_t *actions[FC_MAX_STAGE][FC_MAX_PIPE];

    /*! Min flex action id. */
    uint32_t action_flex_min[FC_MAX_STAGE];

    /*! Max flex action id. */
    uint32_t action_flex_max[FC_MAX_STAGE];

    /*! Protection mutex. */
    sal_mutex_t mutex;

    /*! Num of action reserved for global action in pipe unique mode. */
    int rsvd_num[FC_MAX_STAGE];

    /*! Per chip maximum op profile object selection size. */
    int op_obj_max[FC_MAX_STAGE];

    /*! Per chip maximum op profile shift selection size. */
    int op_shift_max[FC_MAX_STAGE];
} ltsw_flexctr_action_info_t;

/*! Per chip flex counter action information. */
static
ltsw_flexctr_action_info_t *flexctr_action_info[BCM_MAX_NUM_UNITS] = {0};

/*! Flex counter action operation lock. */
#define FLEXCTR_ACTION_LOCK(_u)                                         \
    sal_mutex_take(flexctr_action_info[_u]->mutex, SAL_MUTEX_FOREVER)

/*! Flex counter action operation unlock. */
#define FLEXCTR_ACTION_UNLOCK(_u)                                       \
    sal_mutex_give(flexctr_action_info[_u]->mutex)

/*!
 * \brief Flex counter group action information.
 */
typedef struct ltsw_flexctr_grp_action_info_s {

    /*! Min group action id. */
    uint32_t grp_action_min[FC_MAX_STAGE];

    /*! Max group action id. */
    uint32_t grp_action_max[FC_MAX_STAGE];

    /*! Maximum group map size. */
    uint32_t grp_size[FC_MAX_STAGE];

    /*! Action array. */
    ltsw_flexctr_grp_action_t *grp_actions[FC_MAX_STAGE][FC_MAX_PIPE];

    /*! Protection mutex. */
    sal_mutex_t mutex;

    /*! Num of group reserved for global action in pipe unique mode. */
    int rsvd_num[FC_MAX_STAGE];

} ltsw_flexctr_grp_action_info_t;

/*! Per chip flex counter action information. */
static
ltsw_flexctr_grp_action_info_t *flexctr_grp_action_info[BCM_MAX_NUM_UNITS] = {0};

/*! Flex counter action operation lock. */
#define FLEXCTR_GRP_ACTION_LOCK(_u)     \
    sal_mutex_take(flexctr_grp_action_info[_u]->mutex, SAL_MUTEX_FOREVER)

/*! Flex counter action operation unlock. */
#define FLEXCTR_GRP_ACTION_UNLOCK(_u)   \
    sal_mutex_give(flexctr_grp_action_info[_u]->mutex)

/*! Invalid packet attribute selector type. */
#define FC_PKT_ATTR_SELECTOR_TYPE_INVALID    (-1)

/*! Invalid packet attribute selector bit position. */
#define FC_PKT_ATTR_SELECTOR_BIT_POS_INVALID (-1)

/*! Invalid packet attribute object value. */
#define FC_PKT_ATTR_OBJECT_VALUE_INVALID    (-1)

/*! Packet attribute selector type enumeration count. */
#define FC_PKT_ATTR_TYPE_ENUM_COUNT  bcmFlexctrPacketAttributeTypeCount

/*!
 * \brief Packet attribute information.
 */
typedef struct ltsw_flexctr_pkt_attr_s {

    /*! TRUE if action has been used. */
    bool used;

    /*! Reference counter. */
    int ref_count;

    /*! Number of packet attribute selectors. */
    uint32 num_selectors;

    /*! Packet attribute selector type array. */
    int selector_type[BCMI_LTSW_FLEXCTR_PKT_ATTR_SELECTOR_NUM_MAX];

    /*! Packet attribute selector mask array. */
    uint32_t selector_mask[BCMI_LTSW_FLEXCTR_PKT_ATTR_SELECTOR_NUM_MAX];

    /*! Packet attribute selector bit position array. */
    int selector_bit_pos[BCMI_LTSW_FLEXCTR_PKT_ATTR_SELECTOR_NUM_MAX];

    /*! Packet attribute object instance array. */
    bcmi_ltsw_flexctr_pkt_attr_obj_inst_t
        obj_inst[BCMI_LTSW_FLEXCTR_PKT_ATTR_OBJECT_INSTANCE_NUM_MAX];

    /*! Bitmap representing attribute type within the profile. */
    SHR_BITDCL used_by_attrs[SHRi_BITDCLSIZE(FC_PKT_ATTR_TYPE_ENUM_COUNT)];

} ltsw_flexctr_pkt_attr_t;

/*!
 * \brief Flex counter packet attribute information.
 */
typedef struct ltsw_flexctr_pkt_attr_info_s {

    /*! Per chip flex counter packet attribute initialize flag. */
    bool pkt_attr_initialized;

    /*! Minimum packet attribute profile id. */
    uint32_t pkt_attr_min[FC_MAX_STAGE];

    /*! Maximum packet attribute profile id. */
    uint32_t pkt_attr_max[FC_MAX_STAGE];

    /*! Maximum packet attribute size. */
    uint32_t pkt_attr_size[FC_MAX_STAGE];

    /*! Maximum packet attribute object instance id. */
    uint32_t object_instance_max[FC_MAX_STAGE];

    /*! Maximum packet attribute object value. */
    uint32_t object_value_max[FC_MAX_STAGE];

    /*! Packet attribute array. */
    ltsw_flexctr_pkt_attr_t *pkt_attrs[FC_MAX_STAGE][FC_MAX_PIPE];

    /*! Protection mutex. */
    sal_mutex_t mutex;

} ltsw_flexctr_pkt_attr_info_t;

/*! Per chip flex counter packet attribute information. */
static
ltsw_flexctr_pkt_attr_info_t *flexctr_pkt_attr_info[BCM_MAX_NUM_UNITS] = {0};

/*! Flex counter packet attribute operation lock. */
#define FLEXCTR_PKT_ATTR_LOCK(_u)     \
    sal_mutex_take(flexctr_pkt_attr_info[_u]->mutex, SAL_MUTEX_FOREVER)

/*! Flex counter packet attribute operation unlock. */
#define FLEXCTR_PKT_ATTR_UNLOCK(_u)   \
    sal_mutex_give(flexctr_pkt_attr_info[_u]->mutex)

/*! Check flex counter packet attribute options for creating a profile. */
#define FLEXCTR_PKT_ATTR_OPTIONS_CHECK(unit, options)                 \
    do {                                                              \
        if (options < 0 ||                                            \
            options > BCM_FLEXCTR_PACKET_ATTRIBUTE_OPTIONS_REPLACE) { \
            SHR_ERR_EXIT(SHR_E_PARAM);                                \
        }                                                             \
    } while(0)

/*! Flex counter invalid object. */
#define FLEXCTR_OBJ_INVALID         (-1)

/*!
 * \brief Operation profile information.
 */
typedef struct ltsw_flexctr_operation_profile_s {

    /*! CTR_EFLEX_OBJ_SRC_T. */
    const char *op_objects[FC_MAX_OPOBJ];

    /*! Profile object mask size. */
    uint8_t mask_size[FC_MAX_OPOBJ];

    /*! Profile object shift. */
    uint8_t shift[FC_MAX_OPOBJ];

    /*! Profile objects. */
    int objects[FC_MAX_OPOBJ];

    /*! Profile objects id. */
    uint32_t objects_id[FC_MAX_OPOBJ];

    /*! Quantization id. */
    uint32_t quant_id[FC_MAX_OPOBJ];

    /*! Copy of action object array. */
    int action_objects[FC_ACTION_OBJ_COUNT];

    /*! Copy of action object id array. */
    uint32_t action_objects_id[FC_ACTION_OBJ_COUNT];

    /*! Flex counter key identification. */
    ltsw_flexctr_key_t key;

    /*! Used for global action in pipe unique mode. */
    bool global_rsvd;
} ltsw_flexctr_operation_profile_t;

/*!
 * \brief Flexctr operation profile information.
 */
typedef struct ltsw_flexctr_operation_profile_info_s {

    /*! Min operation profile id. */
    uint32_t op_profile_min[FC_MAX_STAGE];

    /*! Max operation profile id. */
    uint32_t op_profile_max[FC_MAX_STAGE];

    /*! Operation profile array. */
    ltsw_flexctr_operation_profile_t *op_profile[FC_MAX_STAGE][FC_MAX_PIPE];

    /*! Num of operation profile reserved for global action in pipe unique mode. */
    int rsvd_num[FC_MAX_STAGE];

} ltsw_flexctr_operation_profile_info_t;

/*! Per chip flex counter operation profile information. */
static
ltsw_flexctr_operation_profile_info_t *flexctr_op_profile_info[BCM_MAX_NUM_UNITS] = {0};

/*! Minimum mask number. */
#define FLEXCTR_OP_MASK_MIN         1

/*! Maximum mask number. */
#define FLEXCTR_OP_MASK_MAX         16

/*! Maximum shift number. */
#define FLEXCTR_OP_SHIFT_MAX        16

/*! Operand profile object enumeration count. */
#define OP_OBJ_ENUM_COUNT                   (4)

/*! Operand profile object enumeration. */
static const
bcmint_flexctr_enum_map_t op_obj_enum[OP_OBJ_ENUM_COUNT] = {
    { USE_OBJ_1s, 0x0 },
    { USE_OBJ_2s, 0x1 },
    { USE_OBJ_3s, 0x2 },
    { USE_OBJ_4s, 0x3 },
};

/*! Operand profile object fields. */
static const
char *op_obj_flds[FC_MAX_OPOBJ] = {
    OBJ_1s,
    OBJ_2s,
    OBJ_3s,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

/*! Operand profile maske size fields. */
static const
char *op_mask_size_flds[FC_MAX_OPOBJ] = {
    MASK_SIZE_1s,
    MASK_SIZE_2s,
    MASK_SIZE_3s,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

/*! Operand profile shift fields. */
static const
char *op_shift_flds[FC_MAX_OPOBJ] = {
    SHIFT_1s,
    SHIFT_2s,
    SHIFT_3s,
    SHIFT_4s,
    SHIFT_5s,
    NULL,
    NULL,
    NULL,
};

/*! Flex counter slim count. */
#define FLEXCTR_SLIM_COUNT                  (3)

/*! Per chip pipe unique info. */
static bool flexctr_pipe_unique[BCM_MAX_NUM_UNITS][FC_MAX_STAGE] = {{0}};

/*! Per chip maximum pipe number. */
static int flexctr_pipe_max[BCM_MAX_NUM_UNITS][FC_MAX_STAGE] = {{0}};

/*! Static stage properties. */
typedef struct ltsw_flexctr_stage_info_s {

    int stage;

    const char *name;

    int dir;

    uint8_t ha_pool_id;

    uint8_t ha_quant_id;

    uint8_t ha_action_id;

    uint8_t ha_grp_act_id;

    bcmi_ltsw_profile_hdl_t ph[FC_MAX_PIPE];

    const char *config;

    const char *pipe_unique;

    const char *obj_quant_cfg;

    const char *range_chk_profile;

    const char *range_chk_profile_id;

    const char *act_profile;

    const char *act_profile_id;

    const char *act_index;

    const char *act_value_a_true;

    const char *act_value_b_true;

    const char *grp_act_profile;

    const char *grp_act_profile_id;

    const char *operand_profile;

    const char *operand_profile_id;

    const char *stats;

    const char *trigger;

    const char *err_stats;

    const char *hit_ctrl;

    const char *hit_dst_grp;

    const char *hit_src_grp;

    const char *hit_dst_act;

    const char *hit_src_act;

    uint8_t ha_pkt_attr_id;

    const char *pkt_attribute;

    const char *pkt_attribute_id;

    const char *pkt_attribute_object;

    const char *pkt_attribute_object_id;

    const char *pkt_attribute_object_value;

    const char *pool_ctrl;

    const char *pool_ctrl_pool_id;

} ltsw_flexctr_stage_info_t;

static ltsw_flexctr_stage_info_t st_info[FC_MAX_STAGE] = {
    {
        .stage = BCMI_LTSW_FLEXCTR_STAGE_ING_CTR,
        .name = "ing flexctr",
        .dir = BCMI_LTSW_FLEXCTR_DIR_INGRESS,
        .ha_pool_id  = BCMINT_FLEXCTR_ING_POOL_SUB_COMP_ID,
        .ha_quant_id = BCMINT_FLEXCTR_ING_QUANT_SUB_COMP_ID,
        .ha_action_id = BCMINT_FLEXCTR_ING_ACTION_SUB_COMP_ID,
        .ha_grp_act_id = BCMINT_FLEXCTR_ING_GRP_ACTION_SUB_COMP_ID,
        .ph = {
            BCMI_LTSW_PROFILE_FLEXCTR_ING_PIPE0,
            BCMI_LTSW_PROFILE_FLEXCTR_ING_PIPE1,
            BCMI_LTSW_PROFILE_FLEXCTR_ING_PIPE2,
            BCMI_LTSW_PROFILE_FLEXCTR_ING_PIPE3,
            BCMI_LTSW_PROFILE_FLEXCTR_ING_PIPE4,
            BCMI_LTSW_PROFILE_FLEXCTR_ING_PIPE5,
            BCMI_LTSW_PROFILE_FLEXCTR_ING_PIPE6,
            BCMI_LTSW_PROFILE_FLEXCTR_ING_PIPE7,
        },
        .config = CTR_EFLEX_CONFIGs,
        .pipe_unique = CTR_ING_EFLEX_OPERMODE_PIPEUNIQUEs,
        .obj_quant_cfg = CTR_ING_EFLEX_OBJ_QUANTIZATIONs,
        .range_chk_profile = CTR_ING_EFLEX_RANGE_CHK_PROFILEs,
        .range_chk_profile_id = CTR_ING_EFLEX_RANGE_CHK_PROFILE_IDs,
        .act_profile = CTR_ING_EFLEX_ACTION_PROFILEs,
        .act_profile_id = CTR_ING_EFLEX_ACTION_PROFILE_IDs,
        .act_index = INDEX_CTR_ING_EFLEX_OPERAND_PROFILE_IDs,
        .act_value_a_true = VAL_A_CTR_ING_EFLEX_OPERAND_PROFILE_IDs,
        .act_value_b_true = VAL_B_CTR_ING_EFLEX_OPERAND_PROFILE_IDs,
        .grp_act_profile = CTR_ING_EFLEX_GROUP_ACTION_PROFILEs,
        .grp_act_profile_id = CTR_ING_EFLEX_GROUP_ACTION_PROFILE_IDs,
        .operand_profile = CTR_ING_EFLEX_OPERAND_PROFILEs,
        .operand_profile_id = CTR_ING_EFLEX_OPERAND_PROFILE_IDs,
        .stats = CTR_ING_EFLEX_STATSs,
        .trigger = CTR_ING_EFLEX_TRIGGERs,
        .err_stats = CTR_ING_EFLEX_ERROR_STATSs,
        .hit_ctrl = CTR_ING_EFLEX_HITBIT_CONTROLs,
        .hit_dst_grp = DST_CTR_ING_EFLEX_GROUP_ACTION_PROFILE_IDs,
        .hit_src_grp = SRC_CTR_ING_EFLEX_GROUP_ACTION_PROFILE_IDs,
        .hit_dst_act = DST_CTR_ING_EFLEX_ACTION_PROFILE_IDs,
        .hit_src_act = SRC_CTR_ING_EFLEX_ACTION_PROFILE_IDs,
        .ha_pkt_attr_id = BCMINT_FLEXCTR_ING_PKT_ATTR_SUB_COMP_ID,
        .pkt_attribute = CTR_ING_EFLEX_PKT_ATTRIBUTEs,
        .pkt_attribute_id = CTR_ING_EFLEX_PKT_ATTRIBUTE_IDs,
        .pkt_attribute_object = CTR_ING_EFLEX_PKT_ATTRIBUTE_OBJECTs,
        .pkt_attribute_object_id = CTR_ING_EFLEX_PKT_ATTRIBUTE_OBJECT_IDs,
        .pkt_attribute_object_value = CTR_ING_EFLEX_OBJECTs,
        .pool_ctrl = CTR_ING_FLEX_POOL_CONTROLs,
        .pool_ctrl_pool_id = CTR_ING_FLEX_POOL_IDs,
    },
    {
        .stage = BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR,
        .name = "egr flexctr",
        .dir = BCMI_LTSW_FLEXCTR_DIR_EGRESS,
        .ha_pool_id = BCMINT_FLEXCTR_EGR_POOL_SUB_COMP_ID,
        .ha_quant_id = BCMINT_FLEXCTR_EGR_QUANT_SUB_COMP_ID,
        .ha_action_id = BCMINT_FLEXCTR_EGR_ACTION_SUB_COMP_ID,
        .ha_grp_act_id = BCMINT_FLEXCTR_EGR_GRP_ACTION_SUB_COMP_ID,
        .ph = {
            BCMI_LTSW_PROFILE_FLEXCTR_EGR_PIPE0,
            BCMI_LTSW_PROFILE_FLEXCTR_EGR_PIPE1,
            BCMI_LTSW_PROFILE_FLEXCTR_EGR_PIPE2,
            BCMI_LTSW_PROFILE_FLEXCTR_EGR_PIPE3,
            BCMI_LTSW_PROFILE_FLEXCTR_EGR_PIPE4,
            BCMI_LTSW_PROFILE_FLEXCTR_EGR_PIPE5,
            BCMI_LTSW_PROFILE_FLEXCTR_EGR_PIPE6,
            BCMI_LTSW_PROFILE_FLEXCTR_EGR_PIPE7,
        },
        .config = CTR_EFLEX_CONFIGs,
        .pipe_unique = CTR_EGR_EFLEX_OPERMODE_PIPEUNIQUEs,
        .obj_quant_cfg = CTR_EGR_EFLEX_OBJ_QUANTIZATIONs,
        .range_chk_profile = CTR_EGR_EFLEX_RANGE_CHK_PROFILEs,
        .range_chk_profile_id = CTR_EGR_EFLEX_RANGE_CHK_PROFILE_IDs,
        .act_profile = CTR_EGR_EFLEX_ACTION_PROFILEs,
        .act_profile_id = CTR_EGR_EFLEX_ACTION_PROFILE_IDs,
        .act_index = INDEX_CTR_EGR_EFLEX_OPERAND_PROFILE_IDs,
        .act_value_a_true = VAL_A_CTR_EGR_EFLEX_OPERAND_PROFILE_IDs,
        .act_value_b_true = VAL_B_CTR_EGR_EFLEX_OPERAND_PROFILE_IDs,
        .grp_act_profile = CTR_EGR_EFLEX_GROUP_ACTION_PROFILEs,
        .grp_act_profile_id = CTR_EGR_EFLEX_GROUP_ACTION_PROFILE_IDs,
        .operand_profile = CTR_EGR_EFLEX_OPERAND_PROFILEs,
        .operand_profile_id = CTR_EGR_EFLEX_OPERAND_PROFILE_IDs,
        .stats = CTR_EGR_EFLEX_STATSs,
        .trigger = CTR_EGR_EFLEX_TRIGGERs,
        .err_stats = CTR_EGR_EFLEX_ERROR_STATSs,
        .hit_ctrl = CTR_EGR_EFLEX_HITBIT_CONTROLs,
        .hit_dst_grp = DST_CTR_EGR_EFLEX_GROUP_ACTION_PROFILE_IDs,
        .hit_src_grp = SRC_CTR_EGR_EFLEX_GROUP_ACTION_PROFILE_IDs,
        .hit_dst_act = DST_CTR_EGR_EFLEX_ACTION_PROFILE_IDs,
        .hit_src_act = SRC_CTR_EGR_EFLEX_ACTION_PROFILE_IDs,
        .ha_pkt_attr_id = BCMINT_FLEXCTR_EGR_PKT_ATTR_SUB_COMP_ID,
        .pkt_attribute = CTR_EGR_EFLEX_PKT_ATTRIBUTEs,
        .pkt_attribute_id = CTR_EGR_EFLEX_PKT_ATTRIBUTE_IDs,
        .pkt_attribute_object = CTR_EGR_EFLEX_PKT_ATTRIBUTE_OBJECTs,
        .pkt_attribute_object_id = CTR_EGR_EFLEX_PKT_ATTRIBUTE_OBJECT_IDs,
        .pkt_attribute_object_value = CTR_EGR_EFLEX_OBJECTs,
        .pool_ctrl = CTR_EGR_FLEX_POOL_CONTROLs,
        .pool_ctrl_pool_id = CTR_EGR_FLEX_POOL_IDs,
    },
};

/*! Per chip flex counter initialize flag. */
static bool
flexctr_initialized[BCM_MAX_NUM_UNITS] = {0};

/*! Per chip counter eviction enabled flag. */
static bool
flexctr_eviction_enabled[BCM_MAX_NUM_UNITS] = {0};

/* Flexctr eviction threshold default percent. */
#define FLEXCTR_EVICT_THRESHOLD_PERCENT 90

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
ltsw_flexctr_enum_encode(const bcmint_flexctr_enum_map_t *emap,
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
ltsw_flexctr_enum_decode(const bcmint_flexctr_enum_map_t *emap,
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
 * \param [in] key Flex counter key identification.
 * \param [out] id Logical table profile id.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_key_to_id(int unit, ltsw_flexctr_key_t *key, uint64_t *id)
{
    int type = key->type;
    uint32_t min_pid, min_id, max_id;
    uint64_t per_pipe;

    SHR_FUNC_ENTER(unit);

    switch (type) {
        case FLEXCTR_ACTION:
            min_pid = flexctr_action_info[unit]->profile_id_min[key->stage];
            min_id = flexctr_action_info[unit]->action_min[key->stage];
            max_id = flexctr_action_info[unit]->action_max[key->stage];
            break;

        case FLEXCTR_GRP_ACTION:
            min_pid = 1;
            min_id = flexctr_grp_action_info[unit]->grp_action_min[key->stage];
            max_id = flexctr_grp_action_info[unit]->grp_action_max[key->stage];
            break;

        case FLEXCTR_QUANT:
            min_pid = 0;
            min_id = flexctr_quant_info[unit]->quant_min[key->stage];
            max_id = flexctr_quant_info[unit]->quant_max[key->stage];
            break;

        case FLEXCTR_OP_PROFILE:
            min_pid = 0;
            min_id = flexctr_action_info[unit]->op_profile_min[key->stage];
            max_id = flexctr_action_info[unit]->op_profile_max[key->stage];
            break;

        case FLEXCTR_PKT_ATTR:
            min_pid = 0;
            min_id = flexctr_pkt_attr_info[unit]->pkt_attr_min[key->stage];
            max_id = flexctr_pkt_attr_info[unit]->pkt_attr_max[key->stage];
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    per_pipe = max_id - min_id;

    *id = per_pipe * key->pipe + key->id - min_id + min_pid;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check if global indicator and reserved resource indicator mismatch.
 *
 * \param [in] unit Unit number.
 * \param [in] mode Flexctr mode.
 * \param [in] global Global action or not.
 * \param [in] rsvd Reserved resource.
 *
 * \retval 1 indicates mismatch, 0 indicates match.
 */
static int
ltsw_flexctr_resource_mismatch(int unit, bcmi_ltsw_flexctr_mode_t mode,
                               bool global, bool rsvd)
{
    return (mode == bcmiFlexctrModePipeUnique) ? (global ^ rsvd) : false;
}

/*!
 * \brief Help function to convert ports to pipe.
 *
 * \param [in] unit Unit number.
 * \param [in] ports Port bitmap.
 * \param [out] opipe Pipe id.
 */
static int
ltsw_flexctr_config_port_to_pipe(int unit, bcm_pbmp_t ports, int *opipe)
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
ltsw_flexctr_lt_field_validation(int unit, const char *table_name,
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
 * \brief Flex counter packet attribute profile id to packet attribute profile
 * function.
 *
 * This function is used to get a pkt_attr from a profile_id.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Flex counter packet attribute profile ID.
 * \param [out] pkt_attr Flex counter packet attribute profile.
 * \param [out] okey Flex counter key identification.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_profile_id_to_pkt_attr(int unit,
                                    int profile_id,
                                    ltsw_flexctr_pkt_attr_t **pkt_attr,
                                    ltsw_flexctr_key_t *okey)
{
    ltsw_flexctr_pkt_attr_info_t *pai = flexctr_pkt_attr_info[unit];
    ltsw_flexctr_pkt_attr_t *tmp_pkt_attr = NULL;
    ltsw_flexctr_key_t key = {0};
    uint32_t min_id, max_id;

    SHR_FUNC_ENTER(unit);

    FLEXCTR_ID_DECOMPOSE(profile_id, &key);

    if (key.type != FLEXCTR_PKT_ATTR) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid packet attribute profile id "
                                    "%x\n"),
                   profile_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id type %x\n"), key.type));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((key.stage < FC_MIN_STAGE) || (key.stage >= FC_MAX_STAGE)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid packet attribute profile id "
                                    "%x\n"),
                   profile_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id stage %x\n"), key.stage));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (key.pipe < 0 || key.pipe >= flexctr_pipe_max[unit][key.stage]) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid packet attribute profile id "
                                    "%x\n"),
                   profile_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id pipe %x\n"), key.pipe));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    min_id = pai->pkt_attr_min[key.stage];
    max_id = pai->pkt_attr_max[key.stage];
    if (key.id < min_id || key.id >= max_id) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid packet attribute profile id "
                                    "%x\n"),
                   profile_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id %x\n"), key.id));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    tmp_pkt_attr = &(pai->pkt_attrs[key.stage][key.pipe][key.id]);
    if (!tmp_pkt_attr->used) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Packet attribute profile id %x "
                                   "not found\n"),
                  profile_id));

        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (pkt_attr) {
        *pkt_attr = tmp_pkt_attr;
    }

    if (okey) {
        sal_memcpy(okey, &key, sizeof(ltsw_flexctr_key_t));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter quantization data clean up.
 *
 * \param [in] unit Unit number.
 * \param [in|out] quant Flex counter quantization data structure.
 */
static void
ltsw_flexctr_quantization_cleanup(int unit, ltsw_flexctr_quant_t *quant)
{
    bool global_rsvd = quant->global_rsvd;
    sal_memset(quant, 0, sizeof(ltsw_flexctr_quant_t));
    quant->global_rsvd = global_rsvd;
}

/*!
 * \brief Flex counter quantization update function.
 *
 * This function is used to insert or update a quantization LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex counter key identification.
 * \param [in] quant_cfg Flex counter quantization data structure.
 * \param [in] quant_32bit_mode Flex counter quantization 32-bit mode.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_quantization_update(int unit, ltsw_flexctr_key_t *key,
                                 bcm_flexctr_quantization_t *quant_cfg,
                                 bool quant_32bit_mode)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bool pipe_unique = flexctr_pipe_unique[unit][key->stage];
    bcmint_flexctr_object_info_t *oi = NULL;
    ltsw_flexctr_stage_info_t *st = &(st_info[key->stage]);
    ltsw_flexctr_key_t okey = {0};
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t data, data_arr[1];
    uint32_t i, count = COUNTOF(data_arr);
    uint32_t object_id, value, mask;
    const char *symbol_arr[1];
    const char *object = NULL;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Quantization lt update %d %d %u\n"),
              key->stage, key->pipe, key->id));

    /* Configure range_chk_profile LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->range_chk_profile, &eh));

    for (i = 0; i < FC_QUANT_RANGE_MAX; i++) {
        /* Key fields: key->id (range checks for the lower 16 bits). */
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_key_to_id(unit, key, &data));
        data = data * FC_QUANT_RANGE_MAX + i;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, st->range_chk_profile_id, data));

        /* Data fields. */
        if (pipe_unique) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, PIPEs, key->pipe));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, OBJ_SELECTs, key->id));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, RANGE_CHECKERs, i));

        if (i < quant_cfg->range_num) {
            mask = FC_QUANT_RANGE_VALUE_MASK_16BIT_LOWER;
            value = quant_cfg->range_check_min[i] & mask;
            data = (uint64_t)value;
        } else {
            /* Make invalid range checker MIN bigger than MAX. */
            data = 1;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, MINs, data));

        if (i < quant_cfg->range_num) {
            mask = FC_QUANT_RANGE_VALUE_MASK_16BIT_LOWER;
            value = quant_cfg->range_check_max[i] & mask;
            data = (uint64_t)value;
        } else {
            data = 0;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, MAXs, data));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_clear(eh));

        /* Flex counter quantization 32-bit mode. */
        if (quant_32bit_mode) {
            /* Key fields: key->id + 1 (range checks for the upper 16 bits). */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_key_to_id(unit, key, &data));
            data = (data + 1) * FC_QUANT_RANGE_MAX + i;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, st->range_chk_profile_id, data));

            /* Data fields. */
            if (pipe_unique) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(eh, PIPEs, key->pipe));
            }

            data = (key->id + 1);
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, OBJ_SELECTs, data));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, RANGE_CHECKERs, i));

            if (i < quant_cfg->range_num) {
                mask = FC_QUANT_RANGE_VALUE_MASK_16BIT_UPPER;
                value = quant_cfg->range_check_min[i] & mask;
                value >>= FC_QUANT_RANGE_VALUE_SHIFT_16BIT_UPPER;
                data = (uint64_t)value;
            } else {
                /* Make invalid range checker MIN bigger than MAX. */
                data = 1;
            }
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, MINs, data));

            if (i < quant_cfg->range_num) {
                mask = FC_QUANT_RANGE_VALUE_MASK_16BIT_UPPER;
                value = quant_cfg->range_check_max[i] & mask;
                value >>= FC_QUANT_RANGE_VALUE_SHIFT_16BIT_UPPER;
                data = (uint64_t)value;
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
    }

    (void)bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

    /* Configure obj_quant_cfg LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->obj_quant_cfg, &eh));

    /* Key fields. */
    if (pipe_unique) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PIPEs, key->pipe));
    }

    /* Data fields. */
    /* key->id (range checks for the lower 16 bits). */
    data_arr[0] = TRUE;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_add(eh, QUANTIZEs, key->id, data_arr, count));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexctr_object_info_get(unit, quant_cfg->object, &oi));

    if (oi->obj_type == FlexctrObjectTypeMenuId) {
        if ((quant_cfg->object == bcmFlexctrObjectStaticIngPktAttribute) ||
            (quant_cfg->object == bcmFlexctrObjectStaticEgrPktAttribute)) {
            FLEXCTR_ID_DECOMPOSE(quant_cfg->object_id, &okey);
            object_id = okey.id;
        } else {
            object_id = quant_cfg->object_id;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexctr_object_enum_get(unit, quant_cfg->object,
                                               object_id, &object));
    } else {
        object = oi->symbol;
    }

    symbol_arr[0] = object;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_symbol_add(eh, OBJs, key->id,
                                            symbol_arr, count));

    /* Flex counter quantization 32-bit mode. */
    if (quant_32bit_mode) {
        /* Data fields. */
        /* key->id + 1 (range checks for the upper 16 bits). */
        data_arr[0] = TRUE;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add(eh, QUANTIZEs, (key->id + 1),
                                         data_arr, count));

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexctr_object_info_get(unit, quant_cfg->object_upper,
                                               &oi));

        if (oi->obj_type == FlexctrObjectTypeMenuId) {
            if ((quant_cfg->object_upper ==
                 bcmFlexctrObjectStaticIngPktAttribute) ||
                (quant_cfg->object_upper ==
                 bcmFlexctrObjectStaticEgrPktAttribute)) {
                FLEXCTR_ID_DECOMPOSE(quant_cfg->object_id_upper, &okey);
                object_id = okey.id;
            } else {
                object_id = quant_cfg->object_id_upper;
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexctr_object_enum_get(unit,
                                                   quant_cfg->object_upper,
                                                   object_id, &object));
        } else {
            object = oi->symbol;
        }

        symbol_arr[0] = object;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_symbol_add(eh, OBJs, (key->id + 1),
                                                symbol_arr, count));

        /* Enable OBJ_0_1_MODE or OBJ_2_3_MODE for 32-bit mode. */
        data = TRUE;
        if (key->id == 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, OBJ_0_1_MODEs, data));
        } else if (key->id == 2) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, OBJ_2_3_MODEs, data));
        }
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

/*!
 * \brief Flex counter quantization destroy function.
 *
 * This function is used to delete a quantization LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex counter key identification.
 * \param [in] quant_32bit_mode Flex counter quantization 32-bit mode.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_quantization_delete(int unit, ltsw_flexctr_key_t *key,
                                 bool quant_32bit_mode)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bool pipe_unique = flexctr_pipe_unique[unit][key->stage];
    ltsw_flexctr_stage_info_t *st = &(st_info[key->stage]);
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t data, data_arr[1];
    uint32_t i, count = COUNTOF(data_arr);

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Quantization lt delete %d %d %u\n"),
              key->stage, key->pipe, key->id));

    /* Configure range_chk_profile LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->range_chk_profile, &eh));

    for (i = 0; i < FC_QUANT_RANGE_MAX; i++) {
        /* Key fields: key->id (range checks for the lower 16 bits). */
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_key_to_id(unit, key, &data));
        data = data * FC_QUANT_RANGE_MAX + i;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, st->range_chk_profile_id, data));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                                  BCMLT_PRIORITY_NORMAL));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_clear(eh));

        /* Flex counter quantization 32-bit mode. */
        if (quant_32bit_mode) {
            /* Key fields: key->id + 1 (range checks for the upper 16 bits). */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_key_to_id(unit, key, &data));
            data = (data + 1) * FC_QUANT_RANGE_MAX + i;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, st->range_chk_profile_id, data));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                                      BCMLT_PRIORITY_NORMAL));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_clear(eh));
        }
    }

    (void)bcmlt_entry_free(eh);
    eh = BCMLT_INVALID_HDL;

    /* Configure obj_quant_cfg LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->obj_quant_cfg, &eh));

    /* Key fields. */
    if (pipe_unique) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PIPEs, key->pipe));
    }

    /* Data fields. */
    /* key->id (range checks for the lower 16 bits). */
    data_arr[0] = FALSE;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_array_add(eh, QUANTIZEs, key->id, data_arr, count));

    /* Flex counter quantization 32-bit mode. */
    if (quant_32bit_mode) {
        /* Data fields. */
        /* key->id + 1 (range checks for the upper 16 bits). */
        data_arr[0] = FALSE;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_array_add(eh, QUANTIZEs, (key->id + 1),
                                         data_arr, count));

        /* Disable OBJ_0_1_MODE or OBJ_2_3_MODE for 32-bit mode. */
        data = FALSE;
        if (key->id == 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, OBJ_0_1_MODEs, data));
        } else if (key->id == 2) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, OBJ_2_3_MODEs, data));
        }
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

/*!
 * \brief Cleanup flex counter quantization software database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_quantization_db_cleanup(int unit)
{
    ltsw_flexctr_quant_info_t *qi = flexctr_quant_info[unit];

    SHR_FUNC_ENTER(unit);

    if (qi == NULL) {
        SHR_EXIT();
    }

    if (qi->mutex) {
        sal_mutex_destroy(qi->mutex);
    }

    SHR_FREE(qi);
    flexctr_quant_info[unit] = NULL;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize flex counter quantization software database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_quantization_db_init(int unit)
{
    ltsw_flexctr_quant_info_t *qi = flexctr_quant_info[unit];
    uint64_t min, max;
    uint32_t ha_alloc_size = 0, ha_req_size = 0, pipe_req_size = 0;
    uint32_t ha_instance_size = 0, ha_array_size = 0;
    void *alloc_ptr = NULL;
    int warm = bcmi_warmboot_get(unit), stage, max_pipe, i;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (qi != NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    } else {
        SHR_ALLOC(qi, sizeof(ltsw_flexctr_quant_info_t), "ltswFlexctrQuantInfo");
        SHR_NULL_CHECK(qi, SHR_E_MEMORY);
        sal_memset(qi, 0, sizeof(ltsw_flexctr_quant_info_t));
        flexctr_quant_info[unit] = qi;
    }

    /* Create mutex. */
    qi->mutex = sal_mutex_create("ltswFlexctrQuantInfo");
    SHR_NULL_CHECK(qi->mutex , SHR_E_MEMORY);

    /* Alloc quant buffer from HA. */
    for (stage = FC_MIN_STAGE; stage < FC_MAX_STAGE; stage++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, st_info[stage].range_chk_profile,
                                           OBJ_SELECTs , &min, &max));

        qi->quant_min[stage] = (uint32_t)min;
        qi->quant_max[stage] = (uint32_t)max + 1;

        max_pipe = flexctr_pipe_max[unit][stage];

        ha_instance_size = sizeof(ltsw_flexctr_quant_t);
        ha_array_size = qi->quant_max[stage] * max_pipe;
        pipe_req_size = ha_instance_size * qi->quant_max[stage];
        ha_req_size = ha_instance_size * ha_array_size;
        ha_alloc_size = ha_req_size;
        alloc_ptr = bcmi_ltsw_ha_mem_alloc(unit, BCMI_HA_COMP_ID_FLEXCTR,
                                           st_info[stage].ha_quant_id,
                                           "bcmFlexCtrQuant",
                                           &ha_alloc_size);
        SHR_NULL_CHECK(alloc_ptr, SHR_E_MEMORY);

        if (ha_alloc_size < ha_req_size) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }

        if (!warm) {
            sal_memset(alloc_ptr, 0, ha_alloc_size);
        }

        rv = bcmi_ltsw_issu_struct_info_report(unit,
                 BCMI_HA_COMP_ID_FLEXCTR,
                 st_info[stage].ha_quant_id,
                 0, ha_instance_size, ha_array_size,
                 LTSW_FLEXCTR_QUANT_T_ID);
        if (rv != SHR_E_EXISTS) {
           SHR_IF_ERR_VERBOSE_EXIT(rv);
        }

        for (i = 0; i < max_pipe; i++) {
            qi->quants[stage][i] =
                (ltsw_flexctr_quant_t *)(alloc_ptr + pipe_req_size * i);
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        for (stage = FC_MIN_STAGE; stage < FC_MAX_STAGE; stage++) {
            if (qi->quants[stage][0] && !warm) {
                (void)bcmi_ltsw_ha_mem_free(unit, qi->quants[stage][0]);
            }
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter quantization parameter validation function.
 *
 * This function is used to validate quantization parameters.
 *
 * \param [in] unit Unit number.
 * \param [in] quant_cfg Flex counter quantization data structure.
 * \param [out] key Flex counter key.
 * \param [out] quant_32bit_mode Flex counter quantization 32-bit mode.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_quantization_params_validate(int unit,
                                          bcm_flexctr_quantization_t *quant_cfg,
                                          ltsw_flexctr_key_t *key,
                                          bool *quant_32bit_mode)
{
    int pipe = 0;
    bcmint_flexctr_object_info_t *oi = NULL;
    ltsw_flexctr_key_t okey = {0};
    uint32_t i, min, max, last_max = 0;
    bcm_flexctr_object_t object;
    uint32_t object_id;
    int tmp_stage = 0;
    int quant_obj_cnt = 1; /* Default object count for 16-bit quantization. */
    bool quant_32bit_en = FALSE; /* Default is 16-bit quantization. */

    SHR_FUNC_ENTER(unit);

    /* Check quantization 32-bit mode support. */
    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_QUANT_32BIT)) {
        if (quant_cfg->flags & BCM_FLEXCTR_QUANTIZATION_FLAGS_32BIT_MODE) {
            quant_32bit_en = TRUE;
            quant_obj_cnt = 2;
        }
    } else {
        /* !LTSW_FT_FLEXCTR_QUANT_32BIT */
        if ((quant_cfg->flags & BCM_FLEXCTR_QUANTIZATION_FLAGS_32BIT_MODE) ||
            (quant_cfg->object_upper != 0) ||
            (quant_cfg->object_id_upper != 0)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Flags and object for upper 16 bits " \
                                  "should be all zero if device doesn't " \
                                  "support 32-bit mode quantization!\n")));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    for (i = 0; i < quant_obj_cnt; i++) {
        /* Check quantization object. */
        if (i == 0) {
            /* Object for lower 16 bits. */
            object = quant_cfg->object;
            object_id = quant_cfg->object_id;
        } else {
            /* Object for upper 16 bits when quant_32bit_en is TRUE. */
            object = quant_cfg->object_upper;
            object_id = quant_cfg->object_id_upper;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexctr_object_info_get(unit, object, &oi));

        if ((oi->obj_type != FlexctrObjectTypeMenu) &&
            (oi->obj_type != FlexctrObjectTypeMenuId)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid quantization object %d\n"),
                       object));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if ((object == bcmFlexctrObjectStaticIngPktAttribute) ||
            (object == bcmFlexctrObjectStaticEgrPktAttribute)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_profile_id_to_pkt_attr(unit, object_id,
                                                     NULL, &okey));

            if (okey.stage != oi->stage) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid quantization " \
                                      "object id %d\n"),
                           object_id));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }

        if (i == 0) {
            /* Only check chip pipe unique info once. */
            if (flexctr_pipe_unique[unit][oi->stage]) {
                if (quant_cfg->flags & BCM_FLEXCTR_QUANTIZATION_FLAGS_GLOBAL) {
                    /* Skip ports for global quantization. */
                    pipe = 0;
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (ltsw_flexctr_config_port_to_pipe(unit,
                                                          quant_cfg->ports,
                                                          &pipe));
                }
            } else {
                if (BCM_PBMP_NOT_NULL(quant_cfg->ports)) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit, "Ports should be NULL in " \
                                          "global mode")));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            }
            tmp_stage = oi->stage;
        } else {
            /* Check stages of object and object_upper should be the same. */
            if (oi->stage != tmp_stage) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Mismatch quantization " \
                                      "object %d (for lower 16 bits) and " \
                                      "object %d (for upper 16 bits\n"),
                           quant_cfg->object, quant_cfg->object_upper));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
    }

    if (quant_cfg->range_num <= 0 ||
        quant_cfg->range_num > FC_QUANT_RANGE_MAX) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Check if the range is overlay. */
    for (i = 0; i < quant_cfg->range_num; i++) {
        min = quant_cfg->range_check_min[i];
        max = quant_cfg->range_check_max[i];

        if (min > max) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (i != 0 && last_max >= min) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (quant_32bit_en) {
            /* Quantization range check value is 32 bits. */
            if ((min > FC_QUANT_RANGE_VALUE_MAX_32BIT) ||
                (max > FC_QUANT_RANGE_VALUE_MAX_32BIT)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        } else {
            /* Quantization range check value is 16 bits. */
            if ((min > FC_QUANT_RANGE_VALUE_MAX_16BIT) ||
                (max > FC_QUANT_RANGE_VALUE_MAX_16BIT)) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }

        last_max = max;
    }

    sal_memset(key, 0, sizeof(ltsw_flexctr_key_t));

    key->pipe = pipe;
    key->stage = oi->stage;
    key->type = FLEXCTR_QUANT;

    *quant_32bit_mode = quant_32bit_en;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter quantization creation function.
 *
 * This function is used to create a flex counter quantization.
 *
 * \param [in] unit Unit number.
 * \param [in] options BCM_FLEXCTR_OPTIONS_XXX options.
 * \param [in] quant_cfg Flex counter quantization data structure.
 * \param [in] global Global action in pipe unique mode.
 * \param [in] pipe Pipe id, only valid when global is true.
 * \param [in/out] quant_id Flex counter quantization ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_quantization_single_create(int unit, int options,
                                        bcm_flexctr_quantization_t *quant_cfg,
                                        bool global,
                                        int pipe,
                                        uint32_t *quant_id)
{
    ltsw_flexctr_quant_info_t *qi = flexctr_quant_info[unit];
    ltsw_flexctr_quant_t *quant = NULL, *quant_next = NULL;
    ltsw_flexctr_key_t nkey = {0}, okey = {0};
    uint32_t min_id, max_id;
    bcmi_ltsw_flexctr_mode_t mode;
    bool quant_32bit_mode = FALSE;
    int i;

    SHR_FUNC_ENTER(unit);

    FLEXCTR_QUANT_LOCK(unit);

    SHR_NULL_CHECK(quant_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(quant_id, SHR_E_PARAM);

    /* Parameter check. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_quantization_params_validate(unit, quant_cfg, &nkey,
                                                   &quant_32bit_mode));
    if (global) {
        nkey.pipe = pipe;
    }
    mode = flexctr_pipe_unique[unit][nkey.stage] ?
           bcmiFlexctrModePipeUnique : bcmiFlexctrModeGlobal;

    min_id = qi->quant_min[nkey.stage];
    max_id = qi->quant_max[nkey.stage];

    if (options & BCM_FLEXCTR_OPTIONS_REPLACE) {
        FLEXCTR_ID_DECOMPOSE(*quant_id, &okey);
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

        /*
         * The quant id should be either 0 or 2 (total four 16-bit quantizers)
         * - each will occupy two 16-bit quantizers for 32-bit mode.
         */
        if (quant_32bit_mode) {
            if ((nkey.id % 2) != 0) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid quantization id %x " \
                                      "for 32-bit mode\n"),
                           *quant_id));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }

        /* Search existing quantization. */
        quant = &(qi->quants[nkey.stage][nkey.pipe][nkey.id]);
        if (!quant->used) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Quantization id %x not found\n"),
                       *quant_id));
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        } else {
            if (quant_32bit_mode) {
                quant_next = &(qi->quants[nkey.stage][nkey.pipe][nkey.id + 1]);
                if (!quant_next->used) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit, "Quantization id %x not " \
                                          "found for 32-bit mode\n"),
                               *quant_id));
                    SHR_ERR_EXIT(SHR_E_NOT_FOUND);
                }
            }
        }

        if (ltsw_flexctr_resource_mismatch(unit, mode, global,
                                           quant->global_rsvd)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid quantization id %x "
                                        "Mismatch one\n"),
                       FLEXCTR_ID_INDEX(*quant_id)));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else if (options & BCM_FLEXCTR_OPTIONS_WITH_ID) {
        nkey.id = FLEXCTR_ID_INDEX(*quant_id);
        if (nkey.id < min_id || nkey.id >= max_id) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid quantization id %x "
                                        "valid id range %d - %d\n"),
                       FLEXCTR_ID_INDEX(*quant_id), min_id, max_id - 1));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        /*
         * The quant id should be either 0 or 2 (total four 16-bit quantizers)
         * - each will occupy two 16-bit quantizers for 32-bit mode.
         */
        if (quant_32bit_mode) {
            if ((nkey.id % 2) != 0) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid quantization id %x " \
                                      "for 32-bit mode\n"),
                           *quant_id));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }

        /* Search existing quantization. */
        quant = &(qi->quants[nkey.stage][nkey.pipe][nkey.id]);
        if (quant->used) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Quantization id %x exists\n"),
                       *quant_id));
            SHR_ERR_EXIT(SHR_E_EXISTS);
        } else {
            if (quant_32bit_mode) {
                quant_next = &(qi->quants[nkey.stage][nkey.pipe][nkey.id + 1]);
                if (quant_next->used) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit, "Quantization id %x exists "
                                          "for 32-bit mode\n"),
                               *quant_id));
                    SHR_ERR_EXIT(SHR_E_EXISTS);
                }
            }
        }

        if (ltsw_flexctr_resource_mismatch(unit, mode, global,
                                           quant->global_rsvd)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid quantization id %x "
                                        "Mismatch one\n"),
                       FLEXCTR_ID_INDEX(*quant_id)));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        for (nkey.id = 0; nkey.id < max_id; nkey.id++) {
            /*
             * The quant id should be either 0 or 2 (total four 16-bit quantizers)
             * - each will occupy two 16-bit quantizers for 32-bit mode.
             */
            if (quant_32bit_mode) {
                if ((nkey.id % 2) != 0) {
                    continue;
                }
            }

            quant = &(qi->quants[nkey.stage][nkey.pipe][nkey.id]);
            if (!quant->used) {
                if (quant_32bit_mode) {
                    quant_next =
                        &(qi->quants[nkey.stage][nkey.pipe][nkey.id + 1]);
                    if (quant_next->used) {
                        continue;
                    }
                }

                if (ltsw_flexctr_resource_mismatch(unit, mode, global,
                                                   quant->global_rsvd)) {
                    continue;
                }
                break;
            }
        }

        if (nkey.id >= max_id) {
            SHR_ERR_EXIT(SHR_E_FULL);
        }
    }

    /* Quantization LT update. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_quantization_update(unit, &nkey, quant_cfg,
                                          quant_32bit_mode));

    quant->used = TRUE;
    quant->options = options;
    quant->ref_count = 0;

    /* Bookkeeping API configuration. */
    quant->object = quant_cfg->object;
    quant->object_id = quant_cfg->object_id;
    quant->range_num = quant_cfg->range_num;

    if (quant_32bit_mode) {
        quant->object_upper = quant_cfg->object_upper;
        quant->object_id_upper = quant_cfg->object_id_upper;
        quant_next->used = TRUE;
        quant_next->used_for_32bit = TRUE;
    }

    for (i = 0; i < quant_cfg->range_num; i++) {
        quant->range_min[i] = quant_cfg->range_check_min[i];
        quant->range_max[i] = quant_cfg->range_check_max[i];
    }

    /* Last update packet attribute profile reference count. */
    if ((quant->object == bcmFlexctrObjectStaticIngPktAttribute) ||
        (quant->object == bcmFlexctrObjectStaticEgrPktAttribute)) {
        ltsw_flexctr_pkt_attr_t *pkt_attr = NULL;

        /* Should not fail here. */
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_profile_id_to_pkt_attr(unit, quant->object_id,
                                                 &pkt_attr, NULL));
        pkt_attr->ref_count++;
    }

    if (quant_32bit_mode) {
        if ((quant->object_upper == bcmFlexctrObjectStaticIngPktAttribute) ||
            (quant->object_upper == bcmFlexctrObjectStaticEgrPktAttribute)) {
            ltsw_flexctr_pkt_attr_t *pkt_attr_upper = NULL;

            /* Should not fail here. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_profile_id_to_pkt_attr(unit,
                                                     quant->object_id_upper,
                                                     &pkt_attr_upper, NULL));
            pkt_attr_upper->ref_count++;
        }
    }

    /* Compose quantization id. */
    FLEXCTR_ID_COMPOSE(&nkey, *quant_id);

exit:
    FLEXCTR_QUANT_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter quant id to quant function.
 *
 * This function is used to get a quant from a quant_id.
 *
 * \param [in] unit Unit number.
 * \param [in] quant_id Flex counter quantization ID.
 * \param [out] quant Flex counter quant.
 * \param [out] okey Flex counter key identification.
 * \param [out] quant_32bit_mode Flex counter quantization 32-bit mode.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_quant_id_to_quant(int unit,
                               uint32_t quant_id,
                               ltsw_flexctr_quant_t **quant,
                               ltsw_flexctr_key_t *okey,
                               bool *quant_32bit_mode)
{
    ltsw_flexctr_quant_info_t *qi = flexctr_quant_info[unit];
    ltsw_flexctr_quant_t *tmp_quant = NULL, *quant_next = NULL;
    ltsw_flexctr_key_t key = {0};
    uint32_t min_id, max_id;
    bool quant_32bit_en = FALSE; /* Default is 16-bit quantization. */

    SHR_FUNC_ENTER(unit);

    FLEXCTR_ID_DECOMPOSE(quant_id, &key);

    if (key.type != FLEXCTR_QUANT) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid quantization id %x\n"), quant_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id type %x\n"), key.type));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((key.stage < FC_MIN_STAGE) || (key.stage >= FC_MAX_STAGE)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid quantization id %x\n"), quant_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id stage %x\n"), key.stage));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (key.pipe < 0 || key.pipe >= flexctr_pipe_max[unit][key.stage]) {
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

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_QUANT_32BIT)) {
        if (tmp_quant->used_for_32bit) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Quantization id %x not found " \
                                 "(used for 32-bit mode)\n"), quant_id));
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        /*
         * The quant id should be either 0 or 2 (total four 16-bit quantizers)
         * - each will occupy two 16-bit quantizers for 32-bit mode.
         */
        if ((key.id % 2) == 0) {
            quant_next = &(qi->quants[key.stage][key.pipe][key.id + 1]);
            if (quant_next->used_for_32bit) {
                quant_32bit_en = TRUE;
            } else {
                quant_32bit_en = FALSE;
            }
        }
    }

    if (quant) {
        *quant = tmp_quant;
    }

    if (okey) {
        sal_memcpy(okey, &key, sizeof(ltsw_flexctr_key_t));
    }

    if (quant_32bit_mode) {
        *quant_32bit_mode = quant_32bit_en;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter quantization destroy function.
 *
 * This function is used to destroy a flex counter quantization.
 *
 * \param [in] unit Unit number.
 * \param [in] quant_id Flex counter quantization ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_quantization_single_destroy(int unit, uint32_t quant_id)
{
    ltsw_flexctr_quant_info_t *qi = flexctr_quant_info[unit];
    ltsw_flexctr_quant_t *quant= NULL, *quant_next = NULL;
    ltsw_flexctr_key_t key = {0};
    bool quant_32bit_mode = FALSE;

    SHR_FUNC_ENTER(unit);

    FLEXCTR_QUANT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_quant_id_to_quant(unit, quant_id, &quant, &key,
                                        &quant_32bit_mode));

    if (quant->ref_count != 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Quantization id %x is in using\n"),
                   quant_id));
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_quantization_delete(unit, &key, quant_32bit_mode));

    /* Last update packet attribute profile reference count. */
    if ((quant->object == bcmFlexctrObjectStaticIngPktAttribute) ||
        (quant->object == bcmFlexctrObjectStaticEgrPktAttribute)) {
        ltsw_flexctr_pkt_attr_t *pkt_attr = NULL;

        /* Should not fail here. */
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_profile_id_to_pkt_attr(unit, quant->object_id,
                                                 &pkt_attr, NULL));
        pkt_attr->ref_count--;
    }

    if (quant_32bit_mode) {
        if ((quant->object_upper == bcmFlexctrObjectStaticIngPktAttribute) ||
            (quant->object_upper == bcmFlexctrObjectStaticEgrPktAttribute)) {
            ltsw_flexctr_pkt_attr_t *pkt_attr_upper = NULL;

            /* Should not fail here. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_profile_id_to_pkt_attr(unit,
                                                     quant->object_id_upper,
                                                     &pkt_attr_upper, NULL));
            pkt_attr_upper->ref_count--;
        }

        quant_next = &(qi->quants[key.stage][key.pipe][key.id + 1]);
        /* Reset internal quantization structure for upper 16-bit object. */
        ltsw_flexctr_quantization_cleanup(unit, quant_next);
    }

    /* Reset internal quantization structure. */
    ltsw_flexctr_quantization_cleanup(unit, quant);

exit:
    FLEXCTR_QUANT_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter quantization get function.
 *
 * This function is used to get a flex counter quantization information.
 *
 * \param [in] unit Unit number.
 * \param [in] quant_id Flex counter quantization ID.
 * \param [out] quant_cfg Flex counter quantization data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_quantization_get(int unit,
                              uint32_t quant_id,
                              bcm_flexctr_quantization_t *quant_cfg)
{
    ltsw_flexctr_key_t key = {0};
    ltsw_flexctr_quant_t *quant = NULL;
    bcm_port_config_t port_config;
    bool quant_32bit_mode = FALSE;
    int i;

    SHR_FUNC_ENTER(unit);

    FLEXCTR_QUANT_LOCK(unit);

    SHR_NULL_CHECK(quant_cfg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_quant_id_to_quant(unit, quant_id, &quant, &key,
                                        &quant_32bit_mode));

    sal_memset(quant_cfg, 0, sizeof(bcm_flexctr_quantization_t));
    if (flexctr_pipe_unique[unit][key.stage]) {
        if (quant->global_rsvd) {
            /* Skip ports for global quantization in pipe unique mode. */
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_port_config_get(unit, &port_config));
            BCM_PBMP_ASSIGN(quant_cfg->ports, port_config.per_pp_pipe[key.pipe]);
        }
    }
    quant_cfg->object = quant->object;
    quant_cfg->object_id = quant->object_id;
    quant_cfg->range_num = quant->range_num;

    for (i = 0; i < quant->range_num; i++) {
        quant_cfg->range_check_min[i] = quant->range_min[i];
        quant_cfg->range_check_max[i] = quant->range_max[i];
    }

    if (quant_32bit_mode) {
        quant_cfg->object_upper = quant->object_upper;
        quant_cfg->object_id_upper = quant->object_id_upper;
        quant_cfg->flags |= BCM_FLEXCTR_QUANTIZATION_FLAGS_32BIT_MODE;
    }

exit:
    FLEXCTR_QUANT_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter quantization traverse function.
 *
 * This function is used to traverse all flex counter quantization
 * information.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn Quantization traverse callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
ltsw_flexctr_quantization_traverse(int unit,
                                   bcm_flexctr_quantization_traverse_cb trav_fn,
                                   void *user_data)
{
    ltsw_flexctr_quant_info_t *qi = flexctr_quant_info[unit];
    ltsw_flexctr_quant_t *quant = NULL;
    ltsw_flexctr_key_t key = {0};
    bcm_flexctr_quantization_t quant_cfg = {0};
    uint32_t min_id, max_id;
    int max_pipe, quant_id, rv;

    SHR_FUNC_ENTER(unit);

    FLEXCTR_QUANT_LOCK(unit);

    SHR_NULL_CHECK(trav_fn, SHR_E_PARAM);

    key.type = FLEXCTR_QUANT;
    for (key.stage = FC_MIN_STAGE; key.stage < FC_MAX_STAGE; key.stage++) {
        max_pipe = flexctr_pipe_max[unit][key.stage];
        min_id = qi->quant_min[key.stage];
        max_id = qi->quant_max[key.stage];

        for (key.pipe = 0; key.pipe < max_pipe; key.pipe++) {
            for (key.id = min_id; key.id < max_id; key.id++) {
                quant = &(qi->quants[key.stage][key.pipe][key.id]);

                if (!quant->used) {
                    continue;
                }

                /* Skip internal quants. */
                if (quant->options & BCMI_LTSW_FLEXCTR_OPTIONS_INTERNAL) {
                    continue;
                }

                /* Only report once for global quantization in pipe mode. */
                if (quant->global_rsvd && (key.pipe != 0)) {
                    continue;
                }

                /* Skip quants used for upper 16-bit object in 32-bit mode. */
                if (quant->used_for_32bit) {
                    continue;
                }

                FLEXCTR_ID_COMPOSE(&key, quant_id);
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexctr_quantization_get(unit, quant_id, &quant_cfg));

                rv = trav_fn(unit, quant_id, &quant_cfg, user_data);
                if (SHR_FAILURE(rv)) {
                    break;
                }
            }
        }
    }

exit:
    FLEXCTR_QUANT_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter global quantization creation function.
 *
 * This function is used to create a flex counter global quantization in pipe
 * unique mode. The quantization on each pipe has the same configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] options BCM_FLEXCTR_OPTIONS_XXX options.
 * \param [in] quant_cfg Flex counter quantization data structure.
 * \param [in/out] quant_id Flex counter quantization ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_quantization_global_create(int unit, int options,
                                        bcm_flexctr_quantization_t *quant_cfg,
                                        uint32_t *quant_id)
{
    int i, pipe_num, stage, rollback = false;
    bcmint_flexctr_object_info_t *oi = NULL;
    uint32_t *quant_id_tmp = NULL, size;
    ltsw_flexctr_key_t key = {0};
    int options_tmp = options;
    bool with_id = false;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(quant_id, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexctr_object_info_get(unit, quant_cfg->object, &oi));
    stage = oi->stage;

    if (!flexctr_pipe_unique[unit][stage]) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (options & BCM_FLEXCTR_OPTIONS_WITH_ID) {
        FLEXCTR_ID_DECOMPOSE(*quant_id, &key);
        with_id = true;
    }

    pipe_num = flexctr_pipe_max[unit][stage];
    size = sizeof(uint32_t) * pipe_num;
    SHR_ALLOC(quant_id_tmp, size, "ltswFlexctrQuantId");
    SHR_NULL_CHECK(quant_id_tmp, SHR_E_MEMORY);
    sal_memset(quant_id_tmp, 0, size);

    for (i = 0; i < pipe_num; i++) {
        if (with_id) {
            /* Create with id. */
            key.pipe = i;
            FLEXCTR_ID_COMPOSE(&key, quant_id_tmp[i]);
            options_tmp |= BCM_FLEXCTR_OPTIONS_WITH_ID;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_quantization_single_create(unit, options_tmp,
                                                     quant_cfg,
                                                     true, i,
                                                     &quant_id_tmp[i]));
        if (i == 0) {
            FLEXCTR_ID_DECOMPOSE(quant_id_tmp[i], &key);
        }
        with_id = true;
        rollback = true;
    }
    /* Return the counter id with pipe 0. */
    *quant_id = quant_id_tmp[0];

exit:
    if (SHR_FUNC_ERR() && rollback) {
        /* Any create fails, destroy all associated ones. */
        for (i = 0; i < pipe_num; i++) {
            if (quant_id_tmp[i] != 0) {
                (void)ltsw_flexctr_quantization_single_destroy(unit,
                                                               quant_id_tmp[i]);
            }
        }
    }
    SHR_FREE(quant_id_tmp);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter global quantization destroy function.
 *
 * This function is used to destroy a flex counter global quantization in pipe
 * unique mode.
 *
 * \param [in] unit Unit number.
 * \param [in] quant_id Flex counter quantization ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_quantization_global_destroy(int unit, uint32_t quant_id)
{
    int i, pipe_num;
    ltsw_flexctr_key_t key = {0};
    uint32_t quant_id_tmp;
    SHR_FUNC_ENTER(unit);

    FLEXCTR_ID_DECOMPOSE(quant_id, &key);
    pipe_num = flexctr_pipe_max[unit][key.stage];

    for (i = 0; i < pipe_num; i++) {
        key.pipe = i;
        FLEXCTR_ID_COMPOSE(&key, quant_id_tmp);
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_quantization_single_destroy(unit, quant_id_tmp));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter quantization creation function.
 *
 * This function is used to create a flex counter quantization, which can be
 * a global quantization in pipe unique mode or normal one.
 *
 * \param [in] unit Unit number.
 * \param [in] options BCM_FLEXCTR_OPTIONS_XXX options.
 * \param [in] quant_cfg Flex counter quantization data structure.
 * \param [in/out] quant_id Flex counter quantization ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_quantization_create(int unit, int options,
                                 bcm_flexctr_quantization_t *quant_cfg,
                                 uint32_t *quant_id)
{
    SHR_FUNC_ENTER(unit);

    FLEXCTR_QUANT_LOCK(unit);

    SHR_NULL_CHECK(quant_cfg, SHR_E_PARAM);

    if (quant_cfg->flags & BCM_FLEXCTR_QUANTIZATION_FLAGS_GLOBAL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_quantization_global_create(unit,
                                                     options,
                                                     quant_cfg,
                                                     quant_id));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_quantization_single_create(unit,
                                                     options,
                                                     quant_cfg,
                                                     false,
                                                     0,
                                                     quant_id));
    }

exit:
    FLEXCTR_QUANT_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter quantization destroy function.
 *
 * This function is used to destroy a flex counter quantization.
 *
 * \param [in] unit Unit number.
 * \param [in] quant_id Flex counter quantization ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_quantization_destroy(int unit, uint32_t quant_id)
{
    ltsw_flexctr_quant_t *quant= NULL;
    ltsw_flexctr_key_t key = {0};

    SHR_FUNC_ENTER(unit);
    FLEXCTR_QUANT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_quant_id_to_quant(unit, quant_id, &quant, &key, NULL));

    if (quant->global_rsvd) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_quantization_global_destroy(unit, quant_id));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_quantization_single_destroy(unit, quant_id));
    }

exit:
    FLEXCTR_QUANT_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup flex counter pool software database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_pool_db_cleanup(int unit)
{
    ltsw_flexctr_pool_info_t *pi = flexctr_pool_info[unit];

    SHR_FUNC_ENTER(unit);

    if (pi == NULL) {
        SHR_EXIT();
    }

    if (pi->mutex) {
        sal_mutex_destroy(pi->mutex);
    }

    SHR_FREE(pi);
    flexctr_pool_info[unit] = NULL;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize flex counter pool software database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_pool_db_init(int unit)
{
    ltsw_flexctr_pool_info_t *pi = flexctr_pool_info[unit];
    uint64_t min, max;
    uint32_t ha_alloc_size = 0, ha_req_size = 0, pipe_req_size = 0;
    uint32_t ha_instance_size = 0, ha_array_size = 0;
    void *alloc_ptr = NULL;
    int warm = bcmi_warmboot_get(unit), stage, max_pipe, i;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (pi != NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    } else {
        SHR_ALLOC(pi, sizeof(ltsw_flexctr_pool_info_t), "ltswFlexctrPoolInfo");
        SHR_NULL_CHECK(pi, SHR_E_MEMORY);
        sal_memset(pi, 0, sizeof(ltsw_flexctr_pool_info_t));
        flexctr_pool_info[unit] = pi;
    }

    /* Create mutex. */
    pi->mutex = sal_mutex_create("ltswFlexctrPoolInfo");
    SHR_NULL_CHECK(pi->mutex, SHR_E_MEMORY);

    /* Alloc pool buffer from HA. */
    for (stage = FC_MIN_STAGE; stage < FC_MAX_STAGE; stage++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, st_info[stage].act_profile,
                                           POOL_IDs, &min, &max));
        pi->npools[stage] = (size_t)max + 1;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, st_info[stage].act_profile,
                                           BASE_INDEXs, &min, &max));
        pi->nsects_in_pool[stage] = (size_t)max + 1;

        pi->nsects[stage] = pi->npools[stage] * pi->nsects_in_pool[stage];
        pi->nctrs_in_sect[stage] = NUM_SECT_CTRS;
        pi->nctrs_in_pool[stage] = NUM_SECT_CTRS * pi->nsects_in_pool[stage];
        pi->nctrs[stage] = pi->npools[stage] * pi->nctrs_in_pool[stage];

        max_pipe = flexctr_pipe_max[unit][stage];

        ha_instance_size = sizeof(ltsw_flexctr_pool_t);
        ha_array_size = pi->npools[stage] * max_pipe;
        pipe_req_size = ha_instance_size * pi->npools[stage];
        ha_req_size = ha_instance_size * ha_array_size;
        ha_alloc_size = ha_req_size;
        alloc_ptr = bcmi_ltsw_ha_mem_alloc(unit, BCMI_HA_COMP_ID_FLEXCTR,
                                           st_info[stage].ha_pool_id,
                                           "bcmFlexCtrPool",
                                           &ha_alloc_size);
        SHR_NULL_CHECK(alloc_ptr, SHR_E_MEMORY);

        if (ha_alloc_size < ha_req_size) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }

        if (!warm) {
            sal_memset(alloc_ptr, 0, ha_alloc_size);
        }

        rv = bcmi_ltsw_issu_struct_info_report(unit,
                 BCMI_HA_COMP_ID_FLEXCTR,
                 st_info[stage].ha_pool_id,
                 0, ha_instance_size, ha_array_size,
                 LTSW_FLEXCTR_POOL_T_ID);
        if (rv != SHR_E_EXISTS) {
           SHR_IF_ERR_VERBOSE_EXIT(rv);
        }

        for (i = 0; i < max_pipe; i++) {
            pi->pools[stage][i] =
                (ltsw_flexctr_pool_t *)(alloc_ptr + pipe_req_size * i);
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        for (stage = FC_MIN_STAGE; stage < FC_MAX_STAGE; stage++) {
            if (pi->pools[stage][0] && !warm) {
                (void)bcmi_ltsw_ha_mem_free(unit, pi->pools[stage][0]);
            }
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \breif Allocate pool and counter resources for a flex counter action.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex counter key identification.
 * \param [in] act_cfg Flex counter action data structure.
 * \param [out] action Internal flex counter action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_pool_alloc(int unit,
                        ltsw_flexctr_key_t *key,
                        bcm_flexctr_action_t *act_cfg,
                        ltsw_flexctr_action_t *action)
{
    ltsw_flexctr_pool_info_t *pi = flexctr_pool_info[unit];
    ltsw_flexctr_pool_t *pool = NULL;
    bcmint_flexctr_source_info_t *si = NULL;
    int phint = 0, pidx, mode;
    uint32_t tbl_bit, num_ctr_req;
    uint32_t num_pools = 0, start_pool_idx = 0, end_pool_idx = 0;
    uint32_t num_sects_pool = 0, start_section_idx = 0, end_section_idx = 0;
    uint32_t num_ctr_section = 0, num_ctr_pool = 0;
    uint32_t num_section_req = 0, section_cnt = 0, section_cnt_rem = 0;
    uint32_t cur_pool_idx = 0, cur_section_idx = 0;
    uint32_t start_bit = 0, num_bits = 0;
    bool done = FALSE, l2_hit_alloc = FALSE, global;
    bcmi_ltsw_flexctr_mode_t flexctr_mode;

    SHR_FUNC_ENTER(unit);

    num_pools = pi->npools[key->stage];
    num_sects_pool = pi->nsects_in_pool[key->stage];
    num_ctr_section = pi->nctrs_in_sect[key->stage];
    num_ctr_pool = pi->nctrs_in_pool[key->stage];
    flexctr_mode = flexctr_pipe_unique[unit][key->stage] ?
                   bcmiFlexctrModePipeUnique : bcmiFlexctrModeGlobal;
    global = (act_cfg->flags & BCM_FLEXCTR_ACTION_FLAGS_GLOBAL) ?
             true : false;

    if (flexctr_mode == bcmiFlexctrModePipeUnique) {
        if (global) {
            start_pool_idx = pi->npools[key->stage] - pi->rsvd_num[key->stage];
        } else {
            num_pools = pi->npools[key->stage] - pi->rsvd_num[key->stage];
        }
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexctr_source_info_get(unit, act_cfg->source, &si));

    tbl_bit = si->tbl_bit;

    if (act_cfg->mode == bcmFlexctrCounterModeSlim) {
        int pools, offset;

        pools = act_cfg->index_num / num_ctr_pool;
        offset = act_cfg->index_num % num_ctr_pool;

        /*
         *  slim mode address : (ingress)
         *
         *  counter_table_0        : pool-2, pool-1, pool-0
         *                          ------------------------
         *  entry:0    ==> address :  16k     8k       0
         *                          ------------------------
         *  entry:1    ==> address :  16k+1   8k+1     1
         *                          ------------------------
         *  entry:n    ==> address :  16k+n   8k+n     n
         *                          ------------------------
         *  entry:8k-1 ==> address :  24k-1   16k-1    8k-1
         */
        switch (pools % FLEXCTR_SLIM_COUNT) {
            case 0: /* Pool A0 */
                num_ctr_req =
                    num_ctr_pool * (pools / FLEXCTR_SLIM_COUNT) + offset;
                break;

            default: /* Pool A1,A2 */
                num_ctr_req =
                    num_ctr_pool * (pools / FLEXCTR_SLIM_COUNT) + num_ctr_pool;
                break;
        }
        mode = FC_POOL_SLIM;
    } else {
        num_ctr_req = act_cfg->index_num;
        mode = FC_POOL_NONE;
    }

    num_section_req = (num_ctr_req + num_ctr_section - 1) / num_ctr_section;

    if (act_cfg->source == (bcm_flexctr_source_t)bcmiFlexctrSourceL2Dst ||
        act_cfg->source == (bcm_flexctr_source_t)bcmiFlexctrSourceL2Src) {
        l2_hit_alloc = TRUE;
    }

    if ((act_cfg->source == bcmFlexctrSourceIfp ||
         act_cfg->source == bcmFlexctrSourceVfp ||
         act_cfg->source == bcmFlexctrSourceEfp ||
         act_cfg->source == bcmFlexctrSourceExactMatch) &&
        (act_cfg->hint_type == bcmFlexctrHintFieldGroupId)) {
        bcmi_field_flexctr_group_info_t grp_info;

        /* Get group priority. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_field_flexctr_group_info_get(unit, act_cfg->source,
                                               act_cfg->hint, &grp_info));

        phint = grp_info.priority;
    } else if (act_cfg->hint_type == bcmFlexctrHintPool) {
        phint = act_cfg->hint;
    }

    FLEXCTR_POOL_LOCK(unit);

    while (start_pool_idx < num_pools) {
        /* Reset. */
        cur_pool_idx = start_pool_idx;
        cur_section_idx = 0;
        section_cnt = 0;
        section_cnt_rem = 0;
        done = FALSE;

        pidx = cur_pool_idx;
        pool = &(pi->pools[key->stage][key->pipe][pidx]);

        if (!SHR_BITNULL_RANGE(pool->used_by_tbls, 0, FLEXCTR_NUM_TBLS_MAX)) {
            /* Trick operation for l2 hit */
            if (l2_hit_alloc && phint == 1) {
                if (SHR_BITGET(pool->used_by_tbls, tbl_bit)) {
                    /* Found the pool for UFT table. */
                    start_pool_idx = cur_pool_idx;
                    end_pool_idx = cur_pool_idx + 1;
                    start_section_idx = 16;
                    end_section_idx = num_sects_pool - 1;
                    done = TRUE;
                    break;
                } else {
                    start_pool_idx = cur_pool_idx + 1;
                    continue;
                }
            }

            if ((pool->mode != mode) || (pool->hint != phint) ||
                !SHR_BITGET(pool->used_by_tbls, tbl_bit)) {
                /* Pool has been consumed, start from next pool. */
                start_pool_idx = cur_pool_idx + 1;

                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Bad pool %d, next %d\n"),
                             cur_pool_idx, start_pool_idx));
                continue;
            }
        }

        /* In given pool, determine the start of free sections. */
        for (cur_section_idx = 0;
             cur_section_idx < num_sects_pool;
             cur_section_idx++) {
            if (!SHR_BITGET(pool->inuse_bitmap, cur_section_idx)) {
                section_cnt++;
            } else { /* Section idx is used. */
                if (mode == FC_POOL_SLIM) {
                    /* Slim mode should start from pool section 0. */
                    section_cnt = 0;

                    LOG_VERBOSE(BSL_LOG_MODULE,
                                (BSL_META_U(unit, "Sect idx %d is used, "
                                                  "break for slim\n"),
                                 cur_section_idx));
                    break;
                } else {
                    /* Find next free section in current pool. */
                    section_cnt = 0;
                    continue;
                }
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
                                   FLEXCTR_NUM_TBLS_MAX)) {
                if ((pool->mode != mode) || (pool->hint != phint) ||
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
                pool->hint = phint;

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
                    pool->hint = phint;

                    pool->used_count++;

                    cur_pool_idx++;
                }
            }
            break;
        }
    } /* end WHILE */

    FLEXCTR_POOL_UNLOCK(unit);

    if (done) {
        action->start_pool_idx = start_pool_idx;
        action->end_pool_idx = end_pool_idx;
        action->start_section_idx = start_section_idx;
        action->end_section_idx = end_section_idx;

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Pool assignment success %s\n"
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
 * \breif Free pool and counter resources for a flex counter action.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex counter key identification.
 * \param [in] action Internal flex counter action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_pool_free(int unit,
                       ltsw_flexctr_key_t *key,
                       ltsw_flexctr_action_t *action)
{
    ltsw_flexctr_pool_info_t *pi = flexctr_pool_info[unit];
    ltsw_flexctr_pool_t *pool = NULL;
    bcmint_flexctr_source_info_t *si = NULL;
    int pidx;
    uint32_t tbl_bit;
    uint32_t num_sects_pool;
    uint32_t start_pool_idx = 0, end_pool_idx = 0;
    uint32_t start_section_idx = 0, end_section_idx = 0;
    uint32_t cur_pool_idx = 0;
    uint32_t start_bit = 0, num_bits = 0;
    bool l2_hit_alloc = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexctr_source_info_get(unit, action->source, &si));

    tbl_bit = si->tbl_bit;

    num_sects_pool = pi->nsects_in_pool[key->stage];
    start_pool_idx = action->start_pool_idx;
    end_pool_idx = action->end_pool_idx;
    start_section_idx = action->start_section_idx;
    end_section_idx = action->end_section_idx;
    cur_pool_idx = start_pool_idx;

    if (action->source == (bcm_flexctr_source_t)bcmiFlexctrSourceL2Dst ||
        action->source == (bcm_flexctr_source_t)bcmiFlexctrSourceL2Src) {
        l2_hit_alloc = TRUE;
    }

    /* Trick operatino for l2 hit */
    if (l2_hit_alloc && action->hint == 1) {
        SHR_EXIT();
    }

    FLEXCTR_POOL_LOCK(unit);

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
            pool->mode = FC_POOL_NONE;
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
                pool->mode = FC_POOL_NONE;
                pool->hint = 0;
            }

            cur_pool_idx++;
        }
    }

    FLEXCTR_POOL_UNLOCK(unit);

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \breif Update pool info for a flex counter action.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex counter key identification.
 * \param [in] phint Pool hint.
 * \param [in] action Internal flex counter action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_pool_update(int unit,
                         ltsw_flexctr_key_t *key,
                         int phint,
                         ltsw_flexctr_action_t *action)
{
    ltsw_flexctr_pool_info_t *pi = flexctr_pool_info[unit];
    ltsw_flexctr_pool_t *pool = NULL;
    int pidx;
    uint32_t start_pool_idx = 0, end_pool_idx = 0;
    uint32_t cur_pool_idx = 0;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    start_pool_idx = action->start_pool_idx;
    end_pool_idx = action->end_pool_idx;
    cur_pool_idx = start_pool_idx;

    FLEXCTR_POOL_LOCK(unit);
    locked = true;

    while (cur_pool_idx <= end_pool_idx) {
        pidx = cur_pool_idx;
        pool = &(pi->pools[key->stage][key->pipe][pidx]);

        /* Pool is used by multi actions, not support pool update */
        if (pool->used_count > 1) {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "Pool %d is used by multi actions.\n"),
                         pidx));

            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        cur_pool_idx++;
    }

    cur_pool_idx = start_pool_idx;
    while (cur_pool_idx <= end_pool_idx) {
        pidx = cur_pool_idx;
        pool = &(pi->pools[key->stage][key->pipe][pidx]);

        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "Update pool %d hint from %d to %d.\n"),
                     pidx, pool->hint, phint));

        pool->hint = phint;

        cur_pool_idx++;
    }

exit:
    if (locked) {
        FLEXCTR_POOL_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set pool control LT for flexctr action.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex counter key identification.
 * \param [in] action Internal flex counter action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_pool_control_set(int unit, ltsw_flexctr_key_t *key,
                              ltsw_flexctr_action_t *action)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    int stage, pool_id;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t min, max, thresh, seed_val;

    SHR_FUNC_ENTER(unit);

    stage = key->stage;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, st_info[stage].pool_ctrl,
                                       EVICTION_THD_CTR_As,
                                       &min, &max));
    thresh = ((max * FLEXCTR_EVICT_THRESHOLD_PERCENT) + (100 - 1)) / 100;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st_info[stage].pool_ctrl, &eh));
    for (pool_id = action->start_pool_idx;
         pool_id <= action->end_pool_idx; pool_id++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, st_info[stage].pool_ctrl_pool_id,
                                   pool_id));

        /* Set threshold based eviction by default. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, EVICTION_MODEs, RANDOMs));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, EVICTION_THD_CTR_As, thresh));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, EVICTION_THD_CTR_Bs, thresh));

        /* Arbitrarily pick a different seed for each pool. */
        seed_val = ((pool_id + 1) << 16);
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, EVICTION_SEEDs, seed_val));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Insert an entry into OP_PROFILE LT.
 *
 * This function is used to insert an entry into OP_PROFILE LT.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex counter key identification.
 * \param [in] profile Operation profile entry.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_op_profile_insert(int unit,
                               ltsw_flexctr_key_t *key,
                               ltsw_flexctr_operation_profile_t *profile)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bool pipe_unique = flexctr_pipe_unique[unit][key->stage];
    ltsw_flexctr_stage_info_t *st = &(st_info[key->stage]);
    ltsw_flexctr_action_info_t *ai = flexctr_action_info[unit];
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int i;
    uint64_t data;
    const char *symbol = NULL;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Operation profile lt insert %d %d %u\n"),
              key->stage, key->pipe, key->id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->operand_profile, &eh));

    /* Key fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_key_to_id(unit, key, &data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->operand_profile_id, data));

    /* Data fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PROFILEs, key->id));

    if (pipe_unique) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PIPEs, key->pipe));
    }

    for (i = 0; i < ai->op_obj_max[key->stage]; i++) {
        symbol = profile->op_objects[i];
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, op_obj_flds[i], symbol));

        if ((profile->mask_size[i] == 0) &&
            (profile->objects[i] == bcmFlexctrObjectConstZero)) {
            data = (uint64_t)profile->mask_size[i];
        } else {
            data = (uint64_t)profile->mask_size[i] - 1;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, op_mask_size_flds[i], data));
    }

    for (i = 0; i < ai->op_shift_max[key->stage]; i++) {
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
 * \param [in] key Flex counter key identification.
 * \param [in] index OP_PROFILE entry index.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_op_profile_delete(int unit, ltsw_flexctr_key_t *key)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    ltsw_flexctr_stage_info_t *st = &(st_info[key->stage]);
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
        (ltsw_flexctr_key_to_id(unit, key, &data));

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
 * \param [in] key Flex counter key identification.
 * \param [out] profile Operation profile entry.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_op_profile_lookup(int unit,
                               ltsw_flexctr_key_t *key,
                               ltsw_flexctr_operation_profile_t *profile)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    ltsw_flexctr_stage_info_t *st = &(st_info[key->stage]);
    ltsw_flexctr_action_info_t *ai = flexctr_action_info[unit];
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
        (ltsw_flexctr_key_to_id(unit, key, &data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->operand_profile_id, data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));

    /* Data fields. */
    for (i = 0; i < ai->op_obj_max[key->stage]; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_get(eh, op_obj_flds[i], &symbol));
        profile->op_objects[i] = symbol;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, op_mask_size_flds[i], &data));
        profile->mask_size[i] = (uint8_t)data + 1;
    }

    for (i = 0; i < ai->op_shift_max[key->stage]; i++) {
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
ltsw_flexctr_op_profile_entry_cmp_cb(int unit,
                                     void *entries,
                                     int entries_per_set,
                                     int index,
                                     int *cmp)
{
    ltsw_flexctr_action_info_t *ai = flexctr_action_info[unit];
    ltsw_flexctr_operation_profile_t cur_profile, *profile = NULL;
    ltsw_flexctr_key_t key = {0}, okey = {0};
    bcmint_flexctr_object_info_t *oi = NULL;
    size_t copy_sz = sizeof(int) * FC_ACTION_OBJ_COUNT;
    int i, j, array_idx = -1, id;
    const char *object;
    int tmp_objects[FC_ACTION_OBJ_COUNT] = {0}, action_object;
    uint32_t tmp_objects_id[FC_ACTION_OBJ_COUNT] = {0}, action_object_id;
    uint32_t object_id;
    ltsw_flexctr_operation_profile_info_t *opi = flexctr_op_profile_info[unit];
    ltsw_flexctr_operation_profile_t *profile_tmp = NULL;
    bcmi_ltsw_flexctr_mode_t mode;

    SHR_FUNC_ENTER(unit);

    *cmp = 1;

    profile = (ltsw_flexctr_operation_profile_t *)entries;
    SHR_NULL_CHECK(profile, SHR_E_PARAM);

    sal_memcpy(&key, &(profile->key), sizeof(ltsw_flexctr_key_t));
    key.id = index;

    if (entries_per_set != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memcpy(tmp_objects, profile->action_objects, copy_sz);
    sal_memcpy(tmp_objects_id, profile->action_objects_id,
               (sizeof(uint32_t) * FC_ACTION_OBJ_COUNT));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_op_profile_lookup(unit, &key, &cur_profile));

    for (i = 0; i < ai->op_obj_max[key.stage]; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexctr_object_info_get(unit, profile->objects[i], &oi));

        if (oi->obj_type == FlexctrObjectTypeMenuId) {
            if ((profile->objects[i] ==
                     bcmFlexctrObjectStaticIngPktAttribute) ||
                (profile->objects[i] ==
                     bcmFlexctrObjectStaticEgrPktAttribute)) {
                FLEXCTR_ID_DECOMPOSE(profile->objects_id[i], &okey);
                object_id = okey.id;
            } else {
                object_id = profile->objects_id[i];
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexctr_object_enum_get(unit, profile->objects[i],
                                                   object_id, &object));
        } else {
            object = oi->symbol;
        }

        /* Convert op object to array index. */
        (void)(ltsw_flexctr_enum_decode(op_obj_enum,
                                        OP_OBJ_ENUM_COUNT,
                                        cur_profile.op_objects[i],
                                        &array_idx));

        if (((oi->obj_type == FlexctrObjectTypeMenu) ||
            (oi->obj_type == FlexctrObjectTypeMenuId)) && (array_idx != -1)) {
            /* Both are NPL objects. */
            action_object = tmp_objects[array_idx];
            action_object_id = tmp_objects_id[array_idx];

            /* Action object has been occupied and not same. */
            if (oi->obj_type == FlexctrObjectTypeMenu) {
                if ((action_object != FLEXCTR_OBJ_INVALID) &&
                    (action_object != (int)(profile->objects[i]))) {
                    break;
                }
            } else if (oi->obj_type == FlexctrObjectTypeMenuId) {
                if ((action_object != FLEXCTR_OBJ_INVALID) &&
                    (action_object != (int)(profile->objects[i])) &&
                    (action_object_id != profile->objects_id[i])) {
                    break;
                }
            }

            tmp_objects[array_idx] = (int)(profile->objects[i]);
            tmp_objects_id[array_idx] = profile->objects_id[i];
        } else {
            /* Non-NPL object and not same. */
            if (oi->obj_type == FlexctrObjectTypeQuant) {
                id = FLEXCTR_ID_INDEX(profile->quant_id[i]);

                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexctr_enum_encode(quant_obj_enum,
                                              QUANT_OBJ_ENUM_COUNT,
                                              id,
                                              &object));
            }

            if (sal_strcmp(object, cur_profile.op_objects[i])) {
                break;
            }
        }

        if ((profile->mask_size[i] == 0) &&
            (profile->objects[i] == bcmFlexctrObjectConstZero)) {
            /* Shift should always be matched. */
            if (cur_profile.shift[i] != profile->shift[i]) {
                break;
            }
        } else {
            /* Mask size and shift should always be matched. */
            if ((cur_profile.mask_size[i] != profile->mask_size[i]) ||
                (cur_profile.shift[i] != profile->shift[i])) {
                break;
            }
        }
    }

    /* Suitable op profile. */
    if (i >= ai->op_obj_max[key.stage]) {
        for (j = i; j < ai->op_shift_max[key.stage]; j++) {
            /* shift should always be matched. */
            if (cur_profile.shift[j] != profile->shift[j]) {
                break;
            }
        }

        if (j >= ai->op_shift_max[key.stage]) {
            /* Update action object array. */
            sal_memcpy(profile->action_objects, tmp_objects, copy_sz);
            sal_memcpy(profile->action_objects_id, tmp_objects_id,
                       (sizeof(uint32_t) * FC_ACTION_OBJ_COUNT));
            *cmp = 0;
        }
    }

    /* Alloc op profile id for global action from reserved resources. */
    mode = flexctr_pipe_unique[unit][profile->key.stage] ?
           bcmiFlexctrModePipeUnique : bcmiFlexctrModeGlobal;
    if (mode == bcmiFlexctrModePipeUnique && opi != NULL) {
        profile_tmp = opi->op_profile[profile->key.stage][profile->key.pipe];
        if(ltsw_flexctr_resource_mismatch(unit, mode, profile->global_rsvd,
                                          profile_tmp[index].global_rsvd)) {
            *cmp = 1;
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Cleanup the flex counter operation profile.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_op_profile_entry_cleanup(int unit)
{
    int stage, pipe;

    SHR_FUNC_ENTER(unit);

    for (stage = FC_MIN_STAGE; stage < FC_MAX_STAGE; stage++) {
        for (pipe = 0; pipe < flexctr_pipe_max[unit][stage]; pipe++) {
            SHR_IF_ERR_EXIT_EXCEPT_IF
                (bcmi_ltsw_profile_unregister(unit, st_info[stage].ph[pipe]),
                 SHR_E_NOT_FOUND);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Initialize the flex counter operation profile database.
 *
 * This database is mainly used for global action in pipe unique mode.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_op_profile_db_init(int unit)
{
    ltsw_flexctr_operation_profile_info_t *oi = flexctr_op_profile_info[unit];
    void *alloc_ptr = NULL;
    int stage, max_pipe, size, op_profile_num;
    uint64_t idx_min = 0, idx_max = 0;
    int ent_idx_min = 0, ent_idx_max = 0, i, pipe_req_size;
    SHR_FUNC_ENTER(unit);

    if (oi != NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    } else {
        SHR_ALLOC(oi, sizeof(ltsw_flexctr_operation_profile_info_t),
                  "ltswFlexctrOpProfileInfo");
        SHR_NULL_CHECK(oi, SHR_E_PARAM);
        sal_memset(oi, 0, sizeof(ltsw_flexctr_operation_profile_info_t));
        flexctr_op_profile_info[unit] = oi;
    }

    for (stage = FC_MIN_STAGE; stage < FC_MAX_STAGE; stage++) {
        SHR_IF_ERR_EXIT
            (bcmi_lt_field_value_range_get(unit, st_info[stage].operand_profile,
                                           st_info[stage].operand_profile_id,
                                           &idx_min, &idx_max));
        ent_idx_min = (int)idx_min;
        ent_idx_max = (int)idx_max;
        op_profile_num = ent_idx_max - ent_idx_min + 1;

        oi->op_profile_min[stage] = ent_idx_min;
        oi->op_profile_max[stage] = ent_idx_max + 1;
        max_pipe = flexctr_pipe_max[unit][stage];
        pipe_req_size = op_profile_num *
                        sizeof(ltsw_flexctr_operation_profile_t);
        size = max_pipe * pipe_req_size;
        SHR_ALLOC(alloc_ptr, size, "ltswFlexctrOpProfile");
        SHR_NULL_CHECK(alloc_ptr, SHR_E_PARAM);
        sal_memset(alloc_ptr, 0, size);

        for (i = 0; i < max_pipe; i++) {
            oi->op_profile[stage][i] = (ltsw_flexctr_operation_profile_t *)
                                       (alloc_ptr + pipe_req_size * i);
        }
        alloc_ptr = NULL;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clean up reserved resources info.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_op_profile_db_cleanup(int unit)
{
    ltsw_flexctr_operation_profile_info_t *oi = flexctr_op_profile_info[unit];
    int stage;
    SHR_FUNC_ENTER(unit);

    if (oi == NULL) {
        SHR_EXIT();
    }

    for (stage = FC_MIN_STAGE; stage < FC_MAX_STAGE; stage++) {
        SHR_FREE(oi->op_profile[stage][0]);
    }
    SHR_FREE(oi);
    flexctr_op_profile_info[unit] = NULL;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Initialize the flex counter operation profile.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_op_profile_entry_init(int unit)
{
    ltsw_flexctr_action_info_t *ai = flexctr_action_info[unit];
    ltsw_flexctr_action_t *action = NULL;
    uint64_t idx_min = 0, idx_max = 0;
    int ent_idx_min = 0, ent_idx_max = 0, entries_per_set = 1;
    int warm = bcmi_warmboot_get(unit);
    int stage, pipe, i;
    ltsw_flexctr_key_t key = {0};

    SHR_FUNC_ENTER(unit);

    for (stage = FC_MIN_STAGE; stage < FC_MAX_STAGE; stage++) {
        SHR_IF_ERR_EXIT
            (bcmi_lt_field_value_range_get(unit, st_info[stage].operand_profile,
                                           st_info[stage].operand_profile_id,
                                           &idx_min, &idx_max));
        ent_idx_min = (int)idx_min;
        ent_idx_max = (int)idx_max;

        for (pipe = 0; pipe < flexctr_pipe_max[unit][stage]; pipe++) {
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_profile_register(unit, &(st_info[stage].ph[pipe]),
                                            &ent_idx_min, &ent_idx_max,
                                            entries_per_set, NULL,
                                            ltsw_flexctr_op_profile_entry_cmp_cb));

            if (warm) {
                for (i = 0; i < ai->action_max[stage]; i++) {
                    action = &(ai->actions[stage][pipe][i]);

                    if (!action->used) {
                        continue;
                    }

                    FLEXCTR_ID_DECOMPOSE(action->index_profile, &key);
                    if ((key.type == FLEXCTR_OP_PROFILE)) {
                        /* increment reference count */
                        SHR_IF_ERR_EXIT
                            (bcmi_ltsw_profile_ref_count_increase(
                                                unit,
                                                st_info[stage].ph[pipe],
                                                entries_per_set,
                                                key.id, 1));
                    }

                    FLEXCTR_ID_DECOMPOSE(action->value_a_profile, &key);
                    if ((key.type == FLEXCTR_OP_PROFILE)) {
                        /* increment reference count */
                        SHR_IF_ERR_EXIT
                            (bcmi_ltsw_profile_ref_count_increase(
                                                unit,
                                                st_info[stage].ph[pipe],
                                                entries_per_set,
                                                key.id, 1));
                    }

                    FLEXCTR_ID_DECOMPOSE(action->value_b_profile, &key);
                    if ((key.type == FLEXCTR_OP_PROFILE)) {
                        /* increment reference count */
                        SHR_IF_ERR_EXIT
                            (bcmi_ltsw_profile_ref_count_increase(
                                                unit,
                                                st_info[stage].ph[pipe],
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
ltsw_flexctr_op_profile_entry_add(int unit,
                                  ltsw_flexctr_operation_profile_t *profile,
                                  uint32_t *profile_id)
{
    ltsw_flexctr_action_info_t *ai = flexctr_action_info[unit];
    ltsw_flexctr_key_t *key = &(profile->key);
    ltsw_flexctr_key_t okey = {0};
    bcmint_flexctr_object_info_t *oi = NULL;
    int rv, entries_per_set = 1, i, array_idx, j, id;
    int action_object;
    uint32_t object_id, action_object_id = 0;
    const char *object = NULL;
    bool free_profile = FALSE;
    bool existing_profile = FALSE;

    SHR_FUNC_ENTER(unit);

    rv = bcmi_ltsw_profile_index_allocate(unit, st_info[key->stage].ph[key->pipe],
                                          profile, 0, entries_per_set,
                                          &(key->id));
    if (SHR_FAILURE(rv)) {
        /* Get an existing entry and return the profile index directly. */
        if (rv == SHR_E_EXISTS) {
            FLEXCTR_ID_COMPOSE(key, *profile_id);

            existing_profile = TRUE;
        } else {
            SHR_ERR_EXIT(rv);
        }
    }

    free_profile = TRUE;

    /* New profile, need to update op profile lt.*/
    for (i = 0; i < ai->op_obj_max[key->stage]; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexctr_object_info_get(unit, profile->objects[i], &oi));

        /*
         * Existing profile, need to update profile action object array when
         * object type is FlexctrObjectTypeMenuId.
         */
        if ((existing_profile) && (oi->obj_type != FlexctrObjectTypeMenuId)) {
            continue;
        }

        if (oi->obj_type == FlexctrObjectTypeMenuId) {
            if ((profile->objects[i] ==
                     bcmFlexctrObjectStaticIngPktAttribute) ||
                (profile->objects[i] ==
                     bcmFlexctrObjectStaticEgrPktAttribute)) {
                FLEXCTR_ID_DECOMPOSE(profile->objects_id[i], &okey);
                object_id = okey.id;
            } else {
                object_id = profile->objects_id[i];
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexctr_object_enum_get(unit, profile->objects[i],
                                                   object_id, &object));
        } else {
            object = oi->symbol;
        }

        if ((oi->obj_type == FlexctrObjectTypeMenu) ||
            (oi->obj_type == FlexctrObjectTypeMenuId)) {
            /* NPL object needs to occupy one action object. */
            array_idx = -1;
            for (j = 0; j < FC_ACTION_OBJ_COUNT; j++) {
                action_object = profile->action_objects[j];
                action_object_id = profile->action_objects_id[j];

                if (action_object == FLEXCTR_OBJ_INVALID) {
                    /* Record the first free action object. */
                    if (array_idx == -1) {
                        array_idx = j;
                    }
                    continue;
                }

                /* Reuse the existing action object. */
                if ((action_object == (int)(profile->objects[i])) &&
                    (action_object_id == profile->objects_id[i])) {
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
                profile->action_objects[array_idx] =
                    (int)(profile->objects[i]);
                profile->action_objects_id[array_idx] =
                    profile->objects_id[i];

                /* Convert array index to op object. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexctr_enum_encode(op_obj_enum,
                                              OP_OBJ_ENUM_COUNT,
                                              array_idx,
                                              &(profile->op_objects[i])));
            }
        } else if (oi->obj_type == FlexctrObjectTypeQuant) {
            id = FLEXCTR_ID_INDEX(profile->quant_id[i]);

            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_enum_encode(quant_obj_enum, QUANT_OBJ_ENUM_COUNT,
                                          id, &(profile->op_objects[i])));

        } else {
            /* Other objects. */
            profile->op_objects[i] = object;
        }
    }

    /* Existing profile entry and return the profile index directly. */
    if (existing_profile) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_op_profile_insert(unit, key, profile));

    FLEXCTR_ID_COMPOSE(key, *profile_id);

exit:
    if (SHR_FUNC_ERR() && free_profile == TRUE) {
        (void)bcmi_ltsw_profile_index_free(unit, st_info[key->stage].ph[key->pipe],
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
ltsw_flexctr_op_profile_entry_remove(int unit, uint32_t profile_id)
{
    ltsw_flexctr_key_t key = {0};
    int rv;

    SHR_FUNC_ENTER(unit);

    FLEXCTR_ID_DECOMPOSE(profile_id, &key);

    if (key.type != FLEXCTR_OP_PROFILE) {
        LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Invalid profile id %x\n"), profile_id));
        SHR_EXIT();
    }

    rv = bcmi_ltsw_profile_index_free(unit, st_info[key.stage].ph[key.pipe],
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
        (ltsw_flexctr_op_profile_delete(unit, &key));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter action data clean up.
 *
 * \param [in] unit Unit number.
 * \param [in|out] action Flex counter action data structure.
 */
static void
ltsw_flexctr_action_cleanup(int unit, ltsw_flexctr_action_t *action)
{
    bool global_rsvd = action->global_rsvd;
    sal_memset(action, 0, sizeof(ltsw_flexctr_action_t));
    action->global_rsvd = global_rsvd;
}

/*!
 * \brief EFLEX_TRIGGER LT operation.
 *
 * This function is used to update/insert or delete an entry from
 * EFLEX_TRIGGER LT.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex counter key identification.
 * \param [in] trigger Flex counter trigger data structure.
 * \param [in] opcode LT operation code.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_action_trigger_op(int unit,
                               ltsw_flexctr_key_t *key,
                               bcm_flexctr_trigger_t *trigger,
                               bcmlt_opcode_t opcode)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    ltsw_flexctr_stage_info_t *st = &(st_info[key->stage]);
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int evalue, enable;
    uint64_t data;
    const char *symbol = NULL, *stop = NULL;
    bool lt_field_valid = FALSE;
    const bcmint_flexctr_enum_map_t *map;
    uint32_t count;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Counter trigger lt op %d %d %d %u\n"),
              opcode, key->stage, key->pipe, key->id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->trigger, &eh));

    /* Key fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_key_to_id(unit, key, &data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->act_profile_id, data));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexctr_action_scale_enum_map_get(unit, &map, &count));

    if (opcode == BCMLT_OPCODE_DELETE || opcode == BCMLT_OPCODE_LOOKUP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, eh, opcode, BCMLT_PRIORITY_NORMAL));

        if (opcode == BCMLT_OPCODE_LOOKUP) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_get(eh, STARTs, &symbol));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_get(eh, STOPs, &stop));

            if(!sal_strcmp(TIMEs, symbol) &&
               !sal_strcmp(PERIODs, stop)) {
                trigger->trigger_type = bcmFlexctrTriggerTypeTime;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(eh, NUM_PERIODs, &data));
                trigger->period_num = (uint8_t)data;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_symbol_get(eh, SCALEs, &symbol));
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexctr_enum_decode(map,
                                              count,
                                              symbol, &evalue));
                trigger->interval = (bcm_flexctr_trigger_interval_t)evalue;
            } else if (!sal_strcmp(CONDITIONs, symbol) &&
                       !sal_strcmp(PERIODs, stop)) {
                trigger->trigger_type = bcmFlexctrTriggerTypeConditionStartTimeStop;

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
                    (ltsw_flexctr_enum_decode(map,
                                              count,
                                              symbol, &evalue));
                trigger->interval = (bcm_flexctr_trigger_interval_t)evalue;

                /* Check INTERVAL_SHIFT field. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexctr_lt_field_validation(unit, st->trigger,
                                                      INTERVAL_SHIFTs,
                                                      &lt_field_valid));
                if (lt_field_valid) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_get(eh, INTERVAL_SHIFTs, &data));
                    trigger->interval_shift = (uint16_t)data;
                }

                /* Check INTERVAL_SIZE field. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexctr_lt_field_validation(unit, st->trigger,
                                                      INTERVAL_SIZEs,
                                                      &lt_field_valid));
                if (lt_field_valid) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_get(eh, INTERVAL_SIZEs, &data));
                    trigger->interval_size = (uint16_t)data;
                }

                /* Check COMPARE_START field. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexctr_lt_field_validation(unit, st->trigger,
                                                      COMPARE_STARTs,
                                                      &lt_field_valid));
                if (lt_field_valid) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_get(eh, COMPARE_STARTs, &data));

                    enable = (int)data;
                    if (enable) {
                        trigger->flags |=
                        BCM_FLEXCTR_TRIGGER_CONDITION_COMPARE_NOT_EQUAL_START;
                    }
                }
            } else if (!sal_strcmp(TIMEs, symbol) &&
                       !sal_strcmp(ACTION_COUNTs, stop)) {
                trigger->trigger_type =
                    bcmFlexctrTriggerTypeTimeStartActionCountStop;

                /* Check ACTION_COUNT field. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexctr_lt_field_validation(unit, st->trigger,
                                                      NUM_ACTIONSs,
                                                      &lt_field_valid));
                if (lt_field_valid) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_get(eh, NUM_ACTIONSs, &data));
                    trigger->action_count_stop = (uint8_t)data;
                }
            } else if (!sal_strcmp(CONDITIONs, symbol) &&
                       !sal_strcmp(ACTION_COUNTs, stop)) {
                trigger->trigger_type =
                    bcmFlexctrTriggerTypeConditionStartActionCountStop;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(eh, START_VALUEs, &data));
                trigger->object_value_start = (uint16_t)data;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(eh, COND_MASKs, &data));
                trigger->object_value_mask = (uint16_t)data;

                /* Check ACTION_COUNT field. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexctr_lt_field_validation(unit, st->trigger,
                                                      NUM_ACTIONSs,
                                                      &lt_field_valid));
                if (lt_field_valid) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_get(eh, NUM_ACTIONSs, &data));
                    trigger->action_count_stop = (uint8_t)data;
                }

                /* Check COMPARE_START field. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexctr_lt_field_validation(unit, st->trigger,
                                                      COMPARE_STARTs,
                                                      &lt_field_valid));
                if (lt_field_valid) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_get(eh, COMPARE_STARTs, &data));

                    enable = (int)data;
                    if (enable) {
                        trigger->flags |=
                        BCM_FLEXCTR_TRIGGER_CONDITION_COMPARE_NOT_EQUAL_START;
                    }
                }
            } else {
                trigger->trigger_type = bcmFlexctrTriggerTypeCondition;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(eh, START_VALUEs, &data));
                trigger->object_value_start = (uint16_t)data;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(eh, STOP_VALUEs, &data));
                trigger->object_value_stop = (uint16_t)data;

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_get(eh, COND_MASKs, &data));
                trigger->object_value_mask = (uint16_t)data;

                /* Check COMPARE_START field. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexctr_lt_field_validation(unit, st->trigger,
                                                      COMPARE_STARTs,
                                                      &lt_field_valid));
                if (lt_field_valid) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_get(eh, COMPARE_STARTs, &data));

                    enable = (int)data;
                    if (enable) {
                        trigger->flags |=
                        BCM_FLEXCTR_TRIGGER_CONDITION_COMPARE_NOT_EQUAL_START;
                    }
                }

                /* Check COMPARE_STOP field. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexctr_lt_field_validation(unit, st->trigger,
                                                      COMPARE_STOPs,
                                                      &lt_field_valid));
                if (lt_field_valid) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (bcmlt_entry_field_get(eh, COMPARE_STOPs, &data));

                    enable = (int)data;
                    if (enable) {
                        trigger->flags |=
                        BCM_FLEXCTR_TRIGGER_CONDITION_COMPARE_NOT_EQUAL_STOP;
                    }
                }
            }
        }
    } else {
        /* Data fields. */
        data = FALSE;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, TRIGGERs, data));

        if (trigger->trigger_type == bcmFlexctrTriggerTypeTime) {
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
                (ltsw_flexctr_enum_encode(map,
                                          count,
                                          evalue, &symbol));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(eh, SCALEs, symbol));

            /* Check INTERVAL_SHIFT field. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_lt_field_validation(unit, st->trigger,
                                                  INTERVAL_SHIFTs,
                                                  &lt_field_valid));
            if (lt_field_valid) {
                data = trigger->interval_shift;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(eh, INTERVAL_SHIFTs, data));
            }

            /* Check INTERVAL_SIZE field. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_lt_field_validation(unit, st->trigger,
                                                  INTERVAL_SIZEs,
                                                  &lt_field_valid));
            if (lt_field_valid) {
                data = trigger->interval_size;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(eh, INTERVAL_SIZEs, data));
            }

        } else if (trigger->trigger_type ==
                   bcmFlexctrTriggerTypeConditionStartTimeStop) {
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
                (ltsw_flexctr_enum_encode(map,
                                          count,
                                          evalue, &symbol));
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(eh, SCALEs, symbol));

            /* Check INTERVAL_SHIFT field. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_lt_field_validation(unit, st->trigger,
                                                  INTERVAL_SHIFTs,
                                                  &lt_field_valid));
            if (lt_field_valid) {
                data = trigger->interval_shift;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(eh, INTERVAL_SHIFTs, data));
            }

            /* Check INTERVAL_SIZE field. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_lt_field_validation(unit, st->trigger,
                                                  INTERVAL_SIZEs,
                                                  &lt_field_valid));
            if (lt_field_valid) {
                data = trigger->interval_size;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(eh, INTERVAL_SIZEs, data));
            }

            /* Check COMPARE_START field. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_lt_field_validation(unit, st->trigger,
                                                  COMPARE_STARTs,
                                                  &lt_field_valid));
            if (lt_field_valid) {
                if (trigger->flags &
                    BCM_FLEXCTR_TRIGGER_CONDITION_COMPARE_NOT_EQUAL_START) {
                    data = 1;
                } else {
                    data = 0;
                }

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(eh, COMPARE_STARTs, data));
            }
        } else if (trigger->trigger_type ==
                   bcmFlexctrTriggerTypeTimeStartActionCountStop) {
            symbol = TIMEs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(eh, STARTs, symbol));

            symbol = ACTION_COUNTs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(eh, STOPs, symbol));

            data = 0;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, START_TIME_OFFSETs, data));

            /* Check NUM_ACTIONS field. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_lt_field_validation(unit, st->trigger,
                                                  NUM_ACTIONSs,
                                                  &lt_field_valid));
            if (lt_field_valid) {
                data = trigger->action_count_stop;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(eh, NUM_ACTIONSs, data));
            }
        } else if (trigger->trigger_type ==
                   bcmFlexctrTriggerTypeConditionStartActionCountStop) {
            symbol = CONDITIONs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(eh, STARTs, symbol));

            symbol = ACTION_COUNTs;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_symbol_add(eh, STOPs, symbol));

            data = trigger->object_value_start;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, START_VALUEs, data));

            data = trigger->object_value_mask;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, COND_MASKs, data));

            /* Check NUM_ACTIONS field. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_lt_field_validation(unit, st->trigger,
                                                  NUM_ACTIONSs,
                                                  &lt_field_valid));
            if (lt_field_valid) {
                data = trigger->action_count_stop;
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(eh, NUM_ACTIONSs, data));
            }

            /* Check COMPARE_START field. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_lt_field_validation(unit, st->trigger,
                                                  COMPARE_STARTs,
                                                  &lt_field_valid));
            if (lt_field_valid) {
                if (trigger->flags &
                    BCM_FLEXCTR_TRIGGER_CONDITION_COMPARE_NOT_EQUAL_START) {
                    data = 1;
                } else {
                    data = 0;
                }

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(eh, COMPARE_STARTs, data));
            }
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

            /* Check COMPARE_START field. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_lt_field_validation(unit, st->trigger,
                                                  COMPARE_STARTs,
                                                  &lt_field_valid));
            if (lt_field_valid) {
                if (trigger->flags &
                    BCM_FLEXCTR_TRIGGER_CONDITION_COMPARE_NOT_EQUAL_START) {
                    data = 1;
                } else {
                    data = 0;
                }

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(eh, COMPARE_STARTs, data));
            }

            /* Check COMPARE_STOP field. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_lt_field_validation(unit, st->trigger,
                                                  COMPARE_STOPs,
                                                  &lt_field_valid));
            if (lt_field_valid) {
                if (trigger->flags &
                    BCM_FLEXCTR_TRIGGER_CONDITION_COMPARE_NOT_EQUAL_STOP) {
                    data = 1;
                } else {
                    data = 0;
                }

                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(eh, COMPARE_STOPs, data));
            }
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
 * \param [in] key Flex counter key identification.
 * \param [in] action Internal flex counter action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_action_insert(int unit,
                           ltsw_flexctr_key_t *key,
                           ltsw_flexctr_action_t *action)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bool pipe_unique = flexctr_pipe_unique[unit][key->stage];
    ltsw_flexctr_stage_info_t *st = &(st_info[key->stage]);
    bcmint_flexctr_object_info_t *oi = NULL;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int evalue;
    uint64_t data;
    uint32_t object_id;
    const char *symbol = NULL;
    int i;
    ltsw_flexctr_key_t pkey, okey = {0};

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Counter action lt insert %d %d %u\n"),
              key->stage, key->pipe, key->id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->act_profile, &eh));

    /* Key fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_key_to_id(unit, key, &data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->act_profile_id, data));

    /* Data fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, ACTIONs, key->id));

    if (pipe_unique) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PIPEs, key->pipe));
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
        (ltsw_flexctr_enum_encode(action_mode_enum, ACTION_MODE_ENUM_COUNT,
                                  evalue, &symbol));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, MODEs, symbol));

    for (i = 0; i < FC_ACTION_OBJ_COUNT; i++) {
        if (action->objects[i] != FLEXCTR_OBJ_INVALID) {
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexctr_object_info_get(unit,
                                                   action->objects[i], &oi));

            if (oi->obj_type == FlexctrObjectTypeMenuId) {
                if ((action->objects[i] ==
                         bcmFlexctrObjectStaticIngPktAttribute) ||
                    (action->objects[i] ==
                         bcmFlexctrObjectStaticEgrPktAttribute)) {
                    FLEXCTR_ID_DECOMPOSE(action->objects_id[i], &okey);
                    object_id = okey.id;
                } else {
                    object_id = action->objects_id[i];
                }

                SHR_IF_ERR_VERBOSE_EXIT
                    (mbcm_ltsw_flexctr_object_enum_get(unit,
                                                       action->objects[i],
                                                       object_id, &symbol));
            } else {
                symbol = oi->symbol;
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_symbol_add(eh, OBJs, i, &symbol, 1));
        }
    }

    FLEXCTR_ID_DECOMPOSE(action->index_profile, &pkey);
    if (pkey.type == FLEXCTR_OP_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_key_to_id(unit, &pkey, &data));
    } else {
        data = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->act_index, data));

    /* Value A. */
    evalue = action->value_select[FC_VALUE_A_TRUE];
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_enum_encode(action_src_enum, ACTION_SRC_ENUM_COUNT,
                                  evalue, &symbol));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, CTR_SRC_As, symbol));

    evalue = action->value_type[FC_VALUE_A_TRUE];
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_enum_encode(action_update_mode_enum,
                                  ACTION_UPDATE_MODE_ENUM_COUNT,
                                  evalue, &symbol));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, UPDATE_MODE_As, symbol));

    FLEXCTR_ID_DECOMPOSE(action->value_a_profile, &pkey);
    if (pkey.type == FLEXCTR_OP_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_key_to_id(unit, &pkey, &data));
    } else {
        data = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->act_value_a_true, data));

    /* Value B. */
    evalue = action->value_select[FC_VALUE_B_TRUE];
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_enum_encode(action_src_enum, ACTION_SRC_ENUM_COUNT,
                                  evalue, &symbol));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, CTR_SRC_Bs, symbol));

    evalue = action->value_type[FC_VALUE_B_TRUE];
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_enum_encode(action_update_mode_enum,
                                  ACTION_UPDATE_MODE_ENUM_COUNT,
                                  evalue, &symbol));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, UPDATE_MODE_Bs, symbol));

    FLEXCTR_ID_DECOMPOSE(action->value_b_profile, &pkey);
    if (pkey.type == FLEXCTR_OP_PROFILE) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_key_to_id(unit, &pkey, &data));
    } else {
        data = 0;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->act_value_b_true, data));

    evalue = action->drop_count_mode;
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_enum_encode(action_drop_enum, ACTION_DROP_ENUM_COUNT,
                                  evalue, &symbol));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, COUNT_ON_RULE_DROPs, symbol));

    data = !!action->exception_drop_count_enable;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, COUNT_ON_HW_EXCP_DROPs, data));

    /* Egress side. */
    if (st_info[key->stage].dir == BCMI_LTSW_FLEXCTR_DIR_EGRESS) {
        data = !!action->egress_mirror_count_enable;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, COUNT_ON_MIRRORs, data));
    }

    data = 1;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, CTR_VAL_DATAs, data));

    symbol = NOOPs;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, EVICT_COMPAREs, symbol));

    data = FALSE;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, EVICT_RESETs, data));

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
 * \param [in] key Flex counter key identification.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_action_delete(int unit, ltsw_flexctr_key_t *key)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    ltsw_flexctr_stage_info_t *st = &(st_info[key->stage]);
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
        (ltsw_flexctr_key_to_id(unit, key, &data));

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
 * \brief Cleanup flex counter action software database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_action_db_cleanup(int unit)
{
    ltsw_flexctr_action_info_t *ai = flexctr_action_info[unit];

    SHR_FUNC_ENTER(unit);

    if (ai == NULL) {
        SHR_EXIT();
    }

    if (ai->mutex) {
        sal_mutex_destroy(ai->mutex);
    }

    SHR_FREE(ai);
    flexctr_action_info[unit] = NULL;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize flex counter action software database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_action_db_init(int unit)
{
    ltsw_flexctr_action_info_t *ai = flexctr_action_info[unit];
    uint64_t min, max;
    uint32_t flex_min = 0, flex_max = 0;
    uint32_t ha_alloc_size = 0, ha_req_size = 0, pipe_req_size = 0;
    uint32_t ha_instance_size = 0, ha_array_size = 0;
    void *alloc_ptr = NULL;
    int warm = bcmi_warmboot_get(unit), stage, max_pipe, i;
    int rv = SHR_E_NONE;
    bool lt_field_valid = FALSE;

    SHR_FUNC_ENTER(unit);

    if (ai != NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    } else {
        SHR_ALLOC(ai, sizeof(ltsw_flexctr_action_info_t), "ltswFlexctrActionInfo");
        SHR_NULL_CHECK(ai, SHR_E_PARAM);
        sal_memset(ai, 0, sizeof(ltsw_flexctr_action_info_t));
        flexctr_action_info[unit] = ai;
    }

    /* Create mutex. */
    ai->mutex = sal_mutex_create("ltswFlexctrActionInfo");
    SHR_NULL_CHECK(ai->mutex, SHR_E_MEMORY);

    /* Alloc action buffer from HA. */
    for (stage = FC_MIN_STAGE; stage < FC_MAX_STAGE; stage++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, st_info[stage].act_profile,
                                           st_info[stage].act_profile_id,
                                           &min, &max));
        ai->profile_id_min[stage] = (uint32_t)min;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, st_info[stage].act_profile,
                                           ACTIONs, &min, &max));
        ai->action_min[stage] = (uint32_t)min;
        ai->action_max[stage] = (uint32_t)max + 1;

        /*
         * Need to check flex action range if device's actions are
         * partitioned into two types - simple (fixed) and flex action.
         */
        rv = mbcm_ltsw_flexctr_flex_action_range_get(unit, stage,
                                                     &flex_min, &flex_max);
        if (SHR_FAILURE(rv)) {
            /* All are fully flex actions. */
            ai->action_flex_min[stage] = ai->action_min[stage];
            ai->action_flex_max[stage] = ai->action_max[stage];
        } else {
            /* Flex action range. */
            ai->action_flex_min[stage] = flex_min;
            ai->action_flex_max[stage] = flex_max + 1;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, st_info[stage].operand_profile,
                                           PROFILEs, &min, &max));
        ai->op_profile_min[stage] = (uint32_t)min;
        ai->op_profile_max[stage] = (uint32_t)max + 1;

        /* Check per chip maximum op profile object selection size. */
        ai->op_obj_max[stage] = 0;
        for (i = 0; i < FC_MAX_OPOBJ; i++) {
            if (op_obj_flds[i] != NULL) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexctr_lt_field_validation
                        (unit, st_info[stage].operand_profile,
                         op_obj_flds[i], &lt_field_valid));
                if (lt_field_valid) {
                    ai->op_obj_max[stage]++;
                }
            }
        }
        /* Check per chip maximum op profile shift selection size. */
        ai->op_shift_max[stage] = 0;
        for (i = 0; i < FC_MAX_OPOBJ; i++) {
            if (op_shift_flds[i] != NULL) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexctr_lt_field_validation
                        (unit, st_info[stage].operand_profile,
                         op_shift_flds[i], &lt_field_valid));
                if (lt_field_valid) {
                    ai->op_shift_max[stage]++;
                }
            }
        }

        max_pipe = flexctr_pipe_max[unit][stage];

        ha_instance_size = sizeof(ltsw_flexctr_action_t);
        ha_array_size = ai->action_max[stage] * max_pipe;
        pipe_req_size = ha_instance_size * ai->action_max[stage];
        ha_req_size = ha_instance_size * ha_array_size;
        ha_alloc_size = ha_req_size;
        alloc_ptr = bcmi_ltsw_ha_mem_alloc(unit, BCMI_HA_COMP_ID_FLEXCTR,
                                           st_info[stage].ha_action_id,
                                           "bcmFlexCtrAction",
                                           &ha_alloc_size);
        SHR_NULL_CHECK(alloc_ptr, SHR_E_MEMORY);

        if (ha_alloc_size < ha_req_size) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }

        if (!warm) {
            sal_memset(alloc_ptr, 0, ha_alloc_size);
        }

        rv = bcmi_ltsw_issu_struct_info_report(unit,
                 BCMI_HA_COMP_ID_FLEXCTR,
                 st_info[stage].ha_action_id,
                 0, ha_instance_size, ha_array_size,
                 LTSW_FLEXCTR_ACTION_T_ID);
        if (rv != SHR_E_EXISTS) {
           SHR_IF_ERR_VERBOSE_EXIT(rv);
        }

        for (i = 0; i < max_pipe; i++) {
            ai->actions[stage][i] =
                (ltsw_flexctr_action_t *)(alloc_ptr + pipe_req_size * i);
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        for (stage = FC_MIN_STAGE; stage < FC_MAX_STAGE; stage++) {
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
 * \param [in] oi Flex counter object information.
 * \param [in] mask_size Object mask size.
 * \param [in] shift Object shift.
 *
 * \retval Maximum index number.
 */
static uint32_t
ltsw_flexctr_index_max_value_get(int num,
                                 bcmint_flexctr_object_info_t *oi,
                                 uint8_t mask_size,
                                 uint8_t shift)
{
    uint32_t max_value = 0;
    uint16_t mask = (1UL << mask_size) - 1;

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META("num %d, objcet %d, mask %x, shift %u\n"),
              num, oi->object, mask, shift));

    switch (oi->obj_type) {
        case FlexctrObjectTypeConst:
            if (oi->object == bcmFlexctrObjectConstOne) {
                max_value = 1;
            } else {
                max_value = 0;
            }
            break;

        case FlexctrObjectTypeTrigger:
            /* 0 - 255 */
            max_value = 255;
            break;

        case FlexctrObjectTypeQuant:
            /* 0 - 8 */
            max_value = 8;
            break;

        default:
            max_value = mask;
            break;
    }

    /* The second or third object. */
    if (num) {
        max_value <<= shift;
    }

    return max_value;
}

/*!
 * \brief Flex counter value parameter validation function.
 *
 * This function is used to validate value parameters.
 *
 * \param [in] unit Unit number.
 * \param [in] stage Stage id.
 * \param [in] pipe Pipe number.
 * \param [in] value_op Flex counter value data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_value_params_validate(int unit, int stage, int pipe,
                                   bcm_flexctr_value_operation_t *value_op)
{
    ltsw_flexctr_action_info_t *ai = flexctr_action_info[unit];
    bcmint_flexctr_object_info_t *oi;
    ltsw_flexctr_key_t okey = {0};
    bool pipe_unique = flexctr_pipe_unique[unit][stage];
    int object, i, j;

    SHR_FUNC_ENTER(unit);

    if (value_op->select < bcmFlexctrValueSelectCounterValue ||
        value_op->select >= bcmFlexctrValueSelectCount) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid value op select %d\n"),
                   value_op->select));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (value_op->type < bcmFlexctrValueOperationTypeNoop ||
        value_op->type >= bcmFlexctrValueOperationTypeCount) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid value op type %d\n"),
                   value_op->type));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    switch (value_op->type) {
        case bcmFlexctrValueOperationTypeAverage:
        case bcmFlexctrValueOperationTypeDec:
        case bcmFlexctrValueOperationTypeRdec:
            if (!pipe_unique) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Value op type %d is applicable in "
                                            "pipe unique mode\n"),
                           value_op->type));

                SHR_ERR_EXIT(SHR_E_UNAVAIL);
            }
            break;

        default:
            break;
    }

    if (value_op->select == bcmFlexctrValueSelectCounterValue) {
        /* Check value objects. */
        for (i = 0; i < ai->op_obj_max[stage]; i++) {
            /* Do more check for mask_size and shift. */
            if (((value_op->mask_size[i] < FLEXCTR_OP_MASK_MIN) &&
                 (value_op->object[i] != bcmFlexctrObjectConstZero)) ||
                (value_op->mask_size[i] > FLEXCTR_OP_MASK_MAX)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid mask size %u for value\n"),
                           value_op->mask_size[i]));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            if (value_op->shift[i] > FLEXCTR_OP_SHIFT_MAX) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid shift %u for value\n"),
                           value_op->shift[i]));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            object = value_op->object[i];

            if (object == bcmFlexctrObjectQuant) {
                if (FLEXCTR_ID_STAGE(value_op->quant_id[i]) != stage ||
                    FLEXCTR_ID_PIPE(value_op->quant_id[i]) != pipe) {
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }

                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexctr_quant_id_to_quant(unit,
                                                    value_op->quant_id[i],
                                                    NULL, NULL, NULL));
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexctr_object_info_get(unit, object, &oi));

            if (((oi->obj_type == FlexctrObjectTypeMenu) ||
                (oi->obj_type == FlexctrObjectTypeMenuId)) &&
                (oi->stage != stage)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid value object %d for "
                                            "%s action\n"),
                           object, st_info[stage].name));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            if ((object == bcmFlexctrObjectStaticIngPktAttribute) ||
                (object == bcmFlexctrObjectStaticEgrPktAttribute)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexctr_profile_id_to_pkt_attr
                        (unit, value_op->object_id[i], NULL, &okey));

                if (okey.stage != oi->stage) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit, "Invalid value object id %d "
                                                "for object %d\n"),
                               value_op->object_id[i], object));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            }
        }

        for (j = i; j < ai->op_shift_max[stage]; j++) {
            /* Do more check for shift. */
            if (value_op->shift[j] > FLEXCTR_OP_SHIFT_MAX) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid shift %u for value\n"),
                           value_op->shift[j]));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter action parameter validation function.
 *
 * This function is used to validate action parameters.
 *
 * \param [in] unit Unit number.
 * \param [in] act_cfg Flex counter action data structure.
 * \param [out] key Flex counter key.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_action_params_validate(int unit,
                                    bcm_flexctr_action_t *act_cfg,
                                    ltsw_flexctr_key_t *key)
{
    ltsw_flexctr_action_info_t *ai = flexctr_action_info[unit];
    int pipe = 0, stage;
    bcmint_flexctr_source_info_t *si = NULL;
    bcm_flexctr_trigger_t *trigger = NULL;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_op = NULL;
    bcmint_flexctr_object_info_t *oi = NULL;
    ltsw_flexctr_key_t okey = {0};
    int object, i, j;
    uint32_t max_index_val = 0;
    uint64_t min_val, max_val;
    bool lt_field_valid = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexctr_source_info_get(unit, act_cfg->source, &si));

    stage = si->stage;

    if (flexctr_pipe_unique[unit][stage]) {
        if (act_cfg->flags & BCM_FLEXCTR_ACTION_FLAGS_GLOBAL) {
            /* Ignore ports if creating a unique action. */
            pipe = 0;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_config_port_to_pipe(unit, act_cfg->ports, &pipe));
        }
    } else {
        if (BCM_PBMP_NOT_NULL(act_cfg->ports)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Ports should be NULL in global mode")));

            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (act_cfg->hint_type >= bcmFlexctrHintCount) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    } else if (act_cfg->hint_type == bcmFlexctrHintFieldGroupId &&
               !(act_cfg->source == bcmFlexctrSourceIfp ||
                 act_cfg->source == bcmFlexctrSourceVfp ||
                 act_cfg->source == bcmFlexctrSourceEfp ||
                 act_cfg->source == bcmFlexctrSourceExactMatch)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (act_cfg->flags & BCM_FLEXCTR_ACTION_FLAGS_TRIGGER) {
        trigger = &(act_cfg->trigger);

        if (trigger->trigger_type < bcmFlexctrTriggerTypeTime ||
            trigger->trigger_type >= bcmFlexctrTriggerTypeCount) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid trigger type %d\n"),
                       trigger->trigger_type));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (trigger->flags &
            BCM_FLEXCTR_TRIGGER_CONDITION_COMPARE_NOT_EQUAL_START) {
            /* Check COMPARE_START field. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_lt_field_validation(unit,
                                                  st_info[si->stage].trigger,
                                                  COMPARE_STARTs,
                                                  &lt_field_valid));
            if (!lt_field_valid) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid trigger flags 0x%x\n"),
                           trigger->flags));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Must be condition based start type. */
            if (trigger->trigger_type != bcmFlexctrTriggerTypeCondition &&
                trigger->trigger_type !=
                bcmFlexctrTriggerTypeConditionStartTimeStop &&
                trigger->trigger_type !=
                bcmFlexctrTriggerTypeConditionStartActionCountStop) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid trigger flags 0x%x\n"),
                           trigger->flags));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }

        if (trigger->flags &
            BCM_FLEXCTR_TRIGGER_CONDITION_COMPARE_NOT_EQUAL_STOP) {
            /* Check COMPARE_STOP field. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_lt_field_validation(unit,
                                                  st_info[si->stage].trigger,
                                                  COMPARE_STOPs,
                                                  &lt_field_valid));
            if (!lt_field_valid) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid trigger flags 0x%x\n"),
                           trigger->flags));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            /* Must be condition based stop type. */
            if (trigger->trigger_type != bcmFlexctrTriggerTypeCondition) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid trigger flags 0x%x\n"),
                           trigger->flags));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }

        if (trigger->trigger_type == bcmFlexctrTriggerTypeTime) {
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexctr_trigger_interval_validate
                     (unit, trigger->interval));

            /* Check INTERVAL_SHIFT field. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_lt_field_validation(unit,
                                                  st_info[si->stage].trigger,
                                                  INTERVAL_SHIFTs,
                                                  &lt_field_valid));
            if (lt_field_valid) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_field_value_range_get(unit,
                                                   st_info[si->stage].trigger,
                                                   INTERVAL_SHIFTs,
                                                   &min_val, &max_val));
                if ((trigger->interval_shift < (uint16_t)min_val) ||
                    (trigger->interval_shift > (uint16_t)max_val)) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit, "Invalid interval shift %u\n"),
                               trigger->interval_shift));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            }

            /* Check INTERVAL_SHIFT field. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_lt_field_validation(unit,
                                                  st_info[si->stage].trigger,
                                                  INTERVAL_SIZEs,
                                                  &lt_field_valid));
            if (lt_field_valid) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_field_value_range_get(unit,
                                                   st_info[si->stage].trigger,
                                                   INTERVAL_SIZEs,
                                                   &min_val, &max_val));
                if ((trigger->interval_size < (uint16_t)min_val) ||
                    (trigger->interval_size > (uint16_t)max_val)) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit, "Invalid interval size %u\n"),
                               trigger->interval_size));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            }
        }

        /* Check trigger object. */
        if (trigger->trigger_type == bcmFlexctrTriggerTypeCondition) {
            object = trigger->object;

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexctr_object_info_get(unit, object, &oi));

            if (((oi->obj_type != FlexctrObjectTypeMenu) &&
                (oi->obj_type != FlexctrObjectTypeMenuId)) ||
                (oi->stage != stage)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid trigger object %d for "
                                            "%s action\n"),
                           object, st_info[stage].name));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            if ((object == bcmFlexctrObjectStaticIngPktAttribute) ||
                (object == bcmFlexctrObjectStaticEgrPktAttribute)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexctr_profile_id_to_pkt_attr(unit,
                                                         trigger->object_id,
                                                         NULL, &okey));

                if (okey.stage != oi->stage) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit, "Invalid trigger object id %d "
                                                "for object %d\n"),
                               trigger->object_id, object));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            }
        }

        if (trigger->trigger_type ==
            bcmFlexctrTriggerTypeConditionStartTimeStop) {
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexctr_trigger_interval_validate
                     (unit, trigger->interval));

            /* Check INTERVAL_SHIFT field. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_lt_field_validation(unit,
                                                  st_info[si->stage].trigger,
                                                  INTERVAL_SHIFTs,
                                                  &lt_field_valid));
            if (lt_field_valid) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_field_value_range_get(unit,
                                                   st_info[si->stage].trigger,
                                                   INTERVAL_SHIFTs,
                                                   &min_val, &max_val));
                if ((trigger->interval_shift < (uint16_t)min_val) ||
                    (trigger->interval_shift > (uint16_t)max_val)) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit, "Invalid interval shift %u\n"),
                               trigger->interval_shift));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            }

            /* Check INTERVAL_SIZE field. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_lt_field_validation(unit,
                                                  st_info[si->stage].trigger,
                                                  INTERVAL_SIZEs,
                                                  &lt_field_valid));
            if (lt_field_valid) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_field_value_range_get(unit,
                                                   st_info[si->stage].trigger,
                                                   INTERVAL_SIZEs,
                                                   &min_val, &max_val));
                if ((trigger->interval_size < (uint16_t)min_val) ||
                    (trigger->interval_size > (uint16_t)max_val)) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit, "Invalid interval size %u\n"),
                               trigger->interval_size));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            }

            object = trigger->object;

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexctr_object_info_get(unit, object, &oi));

            if (((oi->obj_type != FlexctrObjectTypeMenu) &&
                (oi->obj_type != FlexctrObjectTypeMenuId)) ||
                (oi->stage != stage)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid trigger object %d for "
                                            "%s action\n"),
                           object, st_info[stage].name));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            if ((object == bcmFlexctrObjectStaticIngPktAttribute) ||
                (object == bcmFlexctrObjectStaticEgrPktAttribute)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexctr_profile_id_to_pkt_attr(unit,
                                                         trigger->object_id,
                                                         NULL, &okey));

                if (okey.stage != oi->stage) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit, "Invalid trigger object id %d "
                                                "for object %d\n"),
                               trigger->object_id, object));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            }
        }

        /* Time based start and action count based stop type. */
        if (trigger->trigger_type ==
            bcmFlexctrTriggerTypeTimeStartActionCountStop) {
            /* Check NUM_ACTIONS field. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_lt_field_validation(unit,
                                                  st_info[si->stage].trigger,
                                                  NUM_ACTIONSs,
                                                  &lt_field_valid));
            if (!lt_field_valid) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid trigger type %d\n"),
                           trigger->trigger_type));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }

        /* Condition based start and action count based stop type. */
        if (trigger->trigger_type ==
            bcmFlexctrTriggerTypeConditionStartActionCountStop) {
            /* Check NUM_ACTIONS field. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_lt_field_validation(unit,
                                                  st_info[si->stage].trigger,
                                                  NUM_ACTIONSs,
                                                  &lt_field_valid));
            if (!lt_field_valid) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid trigger type %d\n"),
                           trigger->trigger_type));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            object = trigger->object;
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexctr_object_info_get(unit, object, &oi));

            if (((oi->obj_type != FlexctrObjectTypeMenu) &&
                (oi->obj_type != FlexctrObjectTypeMenuId)) ||
                (oi->stage != stage)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid trigger object %d for "
                                            "%s action\n"),
                           object, st_info[stage].name));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            if ((object == bcmFlexctrObjectStaticIngPktAttribute) ||
                (object == bcmFlexctrObjectStaticEgrPktAttribute)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexctr_profile_id_to_pkt_attr(unit,
                                                         trigger->object_id,
                                                         NULL, &okey));

                if (okey.stage != oi->stage) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit, "Invalid trigger object id %d "
                                                "for object %d\n"),
                               trigger->object_id, object));
                    SHR_ERR_EXIT(SHR_E_PARAM);
                }
            }
        }
    }

    if (act_cfg->drop_count_mode < bcmFlexctrDropCountModeNoDrop ||
        act_cfg->drop_count_mode >= bcmFlexctrDropCountModeCount) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid action drop count mode %d\n"),
                   act_cfg->drop_count_mode));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((st_info[stage].dir == BCMI_LTSW_FLEXCTR_DIR_INGRESS) &&
        act_cfg->egress_mirror_count_enable) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Ingress flex counter action "
                                   "does not support mirror count\n")));
    }

    if (act_cfg->mode < bcmFlexctrCounterModeNormal ||
        act_cfg->mode >= bcmFlexctrCounterModeCount) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid action mode %d\n"), act_cfg->mode));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Check index operation objects. */
    index_op = &act_cfg->index_operation;
    for (i = 0; i < ai->op_obj_max[stage]; i++) {
        /* Do more check for mask_size and shift. */
        if (((index_op->mask_size[i] < FLEXCTR_OP_MASK_MIN) &&
             (index_op->object[i] != bcmFlexctrObjectConstZero)) ||
            (index_op->mask_size[i] > FLEXCTR_OP_MASK_MAX)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid mask size %u for index operand\n"),
                       index_op->mask_size[i]));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (index_op->shift[i] > FLEXCTR_OP_SHIFT_MAX) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid shift %u for index operand\n"),
                       index_op->shift[i]));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        object = index_op->object[i];

        if (object == bcmFlexctrObjectQuant) {
            if (FLEXCTR_ID_STAGE(index_op->quant_id[i]) != stage ||
                FLEXCTR_ID_PIPE(index_op->quant_id[i]) != pipe) {
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_quant_id_to_quant(unit, index_op->quant_id[i],
                                                NULL, NULL, NULL));
        }


        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexctr_object_info_get(unit, object, &oi));

        if (((oi->obj_type == FlexctrObjectTypeMenu) ||
            (oi->obj_type == FlexctrObjectTypeMenuId)) &&
            (oi->stage != stage)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid index object %d for %s action\n"),
                       object, st_info[stage].name));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if ((object == bcmFlexctrObjectStaticIngPktAttribute) ||
            (object == bcmFlexctrObjectStaticEgrPktAttribute)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_profile_id_to_pkt_attr(unit,
                                                     index_op->object_id[i],
                                                     NULL, &okey));

            if (okey.stage != oi->stage) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid index object id %d "
                                            "for object %d\n"),
                           index_op->object_id[i], object));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }

        if (i < 2) {
            /* Handle the first and second object case. */
            max_index_val |=
                ltsw_flexctr_index_max_value_get(i, oi,
                                                 index_op->mask_size[i],
                                                 index_op->shift[i]);
        } else {
            /* Handle the third object case. */
            if ((i + 1) < ai->op_shift_max[stage]) {
                max_index_val |=
                    ltsw_flexctr_index_max_value_get(i, oi,
                                                     index_op->mask_size[i],
                                                     index_op->shift[i + 1]);
            }
        }
    }

    for (j = i; j < ai->op_shift_max[stage]; j++) {
        /* Do more check for shift. */
        if (index_op->shift[j] > FLEXCTR_OP_SHIFT_MAX) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit,
                                  "Invalid shift %u for index operand\n"),
                       index_op->shift[j]));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (max_index_val + 1 > act_cfg->index_num) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Index_num %u is less than possible "
                                   "maximum index %u\n"),
                  act_cfg->index_num, (max_index_val + 1)));
    }

    value_op = &act_cfg->operation_a;
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_value_params_validate(unit, stage, pipe, value_op));

    if (act_cfg->mode == bcmFlexctrCounterModeNormal) {
        value_op = &act_cfg->operation_b;
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_value_params_validate(unit, stage, pipe, value_op));
    }

    key->pipe = pipe;
    key->stage = stage;
    key->type = FLEXCTR_ACTION;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter action type check function.
 *
 * This function is used to check the action type is
 * flex action or simple (fixed) action.
 *
 * \param [in] unit Unit number.
 * \param [in] act_cfg Flex counter action data structure.
 * \param [in] key Flex counter key identification.
 * \param [out] action_type 1: flex action, 0: simple (fixed) action.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_action_type_check(int unit,
                               bcm_flexctr_action_t *act_cfg,
                               ltsw_flexctr_key_t *key,
                               int *action_type)
{
    int rv = SHR_E_NONE;
    bcm_flexctr_value_operation_t *value_op = NULL;
    ltsw_flexctr_action_info_t *ai = flexctr_action_info[unit];
    int i;
    int action_type_value = FC_ACTION_TYPE_SIMPLE;

    /* Get flex counter value a operation. */
    value_op = &act_cfg->operation_a;
    for (i = 0; i < ai->op_obj_max[key->stage]; i++) {
        if ((value_op->object[i] != bcmFlexctrObjectConstZero) &&
            (value_op->object[i] != bcmFlexctrObjectConstOne)) {
            action_type_value = FC_ACTION_TYPE_FLEX;
            break;
        }
    }

    /* Get flex counter value b operation. */
    if (act_cfg->mode == bcmFlexctrCounterModeNormal) {
        if (action_type_value != FC_ACTION_TYPE_FLEX) {
            value_op = &act_cfg->operation_b;
            for (i = 0; i < ai->op_obj_max[key->stage]; i++) {
                if ((value_op->object[i] != bcmFlexctrObjectConstZero) &&
                    (value_op->object[i] != bcmFlexctrObjectConstOne)) {
                    action_type_value = FC_ACTION_TYPE_FLEX;
                    break;
                }
            }
        }
    }

    *action_type = action_type_value;

    return rv;
}

/*!
 * \brief Action trigger process function.
 *
 * This function is used to process action trigger.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex counter key identification.
 * \param [in] trigger Flex counter trigger data structure.
 * \param [out] action Internal flex counter action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_action_trigger_process(int unit,
                                    ltsw_flexctr_key_t *key,
                                    bcm_flexctr_trigger_t *trigger,
                                    ltsw_flexctr_action_t *action)
{
    SHR_FUNC_ENTER(unit);

    if (trigger->trigger_type == bcmFlexctrTriggerTypeCondition ||
        trigger->trigger_type == bcmFlexctrTriggerTypeConditionStartTimeStop ||
        trigger->trigger_type ==
        bcmFlexctrTriggerTypeConditionStartActionCountStop) {
        /* Trigger object must be placed at the first action object. */
        if (action->objects[0] != FLEXCTR_OBJ_INVALID &&
            (action->objects[0] != trigger->object) &&
            (action->objects_id[0] != trigger->object_id)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Action object 0 is occupied by %d\n"),
                       action->objects[0]));
            SHR_ERR_EXIT(SHR_E_INTERNAL);
        }

        action->objects[0] = trigger->object;
        action->objects_id[0] = trigger->object_id;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Action index operation process function.
 *
 * This function is used to process flex counter index operation.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex counter key identification.
 * \param [in] index_op Flex counter index operation data structure.
 * \param [out] action Internal flex counter action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_action_index_operation_process(
                                    int unit,
                                    ltsw_flexctr_key_t *key,
                                    bcm_flexctr_index_operation_t *index_op,
                                    ltsw_flexctr_action_t *action)
{
    bcmi_field_flexctr_group_info_t grp_info = {0};
    ltsw_flexctr_action_info_t *ai = flexctr_action_info[unit];
    ltsw_flexctr_operation_profile_t profile = {{0}};
    size_t copy_sz = sizeof(int) * FC_ACTION_OBJ_COUNT;
    int i, j;
    uint32_t profile_id;
    uint32_t ohint = 0, object_id;
    int object;

    SHR_FUNC_ENTER(unit);

    if ((action->source == bcmFlexctrSourceIfp ||
         action->source == bcmFlexctrSourceVfp ||
         action->source == bcmFlexctrSourceEfp ||
         action->source == bcmFlexctrSourceExactMatch) &&
        (action->hint_type == BCMI_FLEXCTR_HINT_FIELD_GROUP_ID)) {

        /* Get FP group flex counter object ID. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_field_flexctr_group_info_get(unit, action->source,
                                               action->hint, &grp_info));

        ohint = grp_info.flex_ctr_obj_id;
    }

    /* Process for mask_size and object. */
    for (i = 0; i < ai->op_obj_max[key->stage]; i++) {
        j = FC_INDEX_OP * ai->op_obj_max[key->stage] + i;

        action->mask_size[j] = profile.mask_size[i] = index_op->mask_size[i];
        action->object[j] = profile.objects[i] = index_op->object[i];

        object = index_op->object[i];

        if ((object == bcmFlexctrObjectStaticIngFieldStageIngress ||
             object == bcmFlexctrObjectStaticIngFieldStageLookup ||
             object == bcmFlexctrObjectStaticEgrFieldStageEgress ||
             object == bcmFlexctrObjectStaticIngExactMatch) &&
            (action->hint_type == BCMI_FLEXCTR_HINT_FIELD_GROUP_ID)) {
            /*
             * Use internal object ID needed for FP and ignore user's input.
             */
            object_id = ohint;
        } else {
            object_id = index_op->object_id[i];
        }

        action->object_id[j] = profile.objects_id[i] = object_id;
        if (index_op->object[i] == bcmFlexctrObjectQuant) {
            action->quant_id[j] = profile.quant_id[i] = index_op->quant_id[i];
        } else {
            action->quant_id[j] = FC_ID_INVALID;
        }
    }

    /* Process for shift. */
    for (i = 0; i < ai->op_shift_max[key->stage]; i++) {
        j = FC_INDEX_OP * ai->op_shift_max[key->stage] + i;

        action->shift[j] = profile.shift[i] = index_op->shift[i];
    }

    sal_memcpy(profile.action_objects, action->objects, copy_sz);
    sal_memcpy(profile.action_objects_id, action->objects_id,
               (sizeof(uint32_t) * FC_ACTION_OBJ_COUNT));

    profile.key.pipe = key->pipe;
    profile.key.stage = key->stage;
    profile.key.type = FLEXCTR_OP_PROFILE;
    profile.global_rsvd = action->global_rsvd;

    SHR_IF_ERR_EXIT
        (ltsw_flexctr_op_profile_entry_add(unit, &profile, &profile_id));

    /* Update action information. */
    sal_memcpy(action->objects, profile.action_objects, copy_sz);
    sal_memcpy(action->objects_id, profile.action_objects_id,
               (sizeof(uint32_t) * FC_ACTION_OBJ_COUNT));
    action->index_profile = profile_id;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Action value operation process function.
 *
 * This function is used to process flex counter value operation.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex counter key identification.
 * \param [in] type Flex counter value instance.
 * \param [in] value_op Flex counter value operation data structure.
 * \param [out] action Internal flex counter action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_action_value_operation_process(
                                    int unit,
                                    ltsw_flexctr_key_t *key,
                                    int type,
                                    bcm_flexctr_value_operation_t *value_op,
                                    ltsw_flexctr_action_t *action)
{
    ltsw_flexctr_action_info_t *ai = flexctr_action_info[unit];
    ltsw_flexctr_operation_profile_t profile = {{0}};
    size_t copy_sz = sizeof(int) * FC_ACTION_OBJ_COUNT;
    int i, j;
    uint32_t profile_id;

    SHR_FUNC_ENTER(unit);

    action->value_select[type] = value_op->select;
    action->value_type[type] = value_op->type;

    /* Process for mask_size and object. */
    for (i = 0; i < ai->op_obj_max[key->stage]; i++) {
        j = type * ai->op_obj_max[key->stage] + i;

        action->mask_size[j] = profile.mask_size[i] = value_op->mask_size[i];
        action->object[j] = profile.objects[i] = value_op->object[i];
        action->object_id[j] = profile.objects_id[i] = value_op->object_id[i];
        if (value_op->object[i] == bcmFlexctrObjectQuant) {
            action->quant_id[j] = profile.quant_id[i] = value_op->quant_id[i];
        } else {
            action->quant_id[j] = FC_ID_INVALID;
        }
    }

    /* Process for shift. */
    for (i = 0; i < ai->op_shift_max[key->stage]; i++) {
        j = type * ai->op_shift_max[key->stage] + i;

        action->shift[j] = profile.shift[i] = value_op->shift[i];
    }

    if (value_op->select != bcmFlexctrValueSelectCounterValue) {
        SHR_EXIT();
    }

    sal_memcpy(profile.action_objects, action->objects, copy_sz);
    sal_memcpy(profile.action_objects_id, action->objects_id,
               (sizeof(uint32_t) * FC_ACTION_OBJ_COUNT));

    profile.key.pipe = key->pipe;
    profile.key.stage = key->stage;
    profile.key.type = FLEXCTR_OP_PROFILE;

    SHR_IF_ERR_EXIT
        (ltsw_flexctr_op_profile_entry_add(unit, &profile, &profile_id));

    /* Update action information. */
    sal_memcpy(action->objects, profile.action_objects, copy_sz);
    sal_memcpy(action->objects_id, profile.action_objects_id,
               (sizeof(uint32_t) * FC_ACTION_OBJ_COUNT));

    switch (type) {
        case FC_VALUE_A_TRUE:
            action->value_a_profile = profile_id;
            break;

        case FC_VALUE_B_TRUE:
            action->value_b_profile = profile_id;
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter action creation function.
 *
 * This function is used to create a flex counter action.
 *
 * \param [in] unit Unit number.
 * \param [in] options BCM_FLEXCTR_OPTIONS_XXX options.
 * \param [in] act_cfg Flex counter action data structure.
 * \param [in] global Global action in pipe unique mode.
 * \param [in] pipe Pipe id, only valid when global is true.
 * \param [in/out] stat_counter_id Flex counter ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_action_single_create(int unit, int options,
                                  bcm_flexctr_action_t *act_cfg,
                                  bool global,
                                  int pipe,
                                  uint32_t *stat_counter_id)
{
    ltsw_flexctr_action_info_t *ai = flexctr_action_info[unit];
    ltsw_flexctr_action_t *action = NULL;
    bcm_flexctr_trigger_t *trigger = NULL;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_op = NULL;
    ltsw_flexctr_key_t nkey = {0};
    uint32_t min_id, max_id;
    int i;
    bool rollback = FALSE, free_pool= FALSE, free_tr = FALSE;
    int action_type = FC_ACTION_TYPE_SIMPLE;
    bcmi_ltsw_flexctr_mode_t mode;
    bool evict_ena = false;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(act_cfg, SHR_E_PARAM);
    SHR_NULL_CHECK(stat_counter_id, SHR_E_PARAM);

    /* Parameter check. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_action_params_validate(unit, act_cfg, &nkey));
    if (global) {
        nkey.pipe = pipe;
    }
    mode = flexctr_pipe_unique[unit][nkey.stage] ?
           bcmiFlexctrModePipeUnique : bcmiFlexctrModeGlobal;

    /* Flex action check. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_action_type_check(unit, act_cfg, &nkey, &action_type));

    if (action_type == FC_ACTION_TYPE_FLEX) {
        min_id = ai->action_flex_min[nkey.stage];
        max_id = ai->action_flex_max[nkey.stage];
    } else {
        min_id = ai->action_min[nkey.stage];
        max_id = ai->action_max[nkey.stage];
    }

    if (options & BCM_FLEXCTR_OPTIONS_WITH_ID) {
        nkey.id = FLEXCTR_ID_INDEX(*stat_counter_id);
        if (nkey.id < min_id || nkey.id >= max_id) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid action id %d "
                                        "valid id range %d - %d\n"),
                       FLEXCTR_ID_INDEX(*stat_counter_id),
                       min_id, max_id - 1));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        action = &(ai->actions[nkey.stage][nkey.pipe][nkey.id]);
        if (action->used) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Action id %d exists\n"),
                       *stat_counter_id));
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }

        if (ltsw_flexctr_resource_mismatch(unit, mode, global,
                                           action->global_rsvd)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid action id %d "
                                        "Mismatch one\n"),
                       FLEXCTR_ID_INDEX(*stat_counter_id)));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        /* Search an unused counter action. */
        for (nkey.id = min_id; nkey.id < max_id; nkey.id++) {
            action = &(ai->actions[nkey.stage][nkey.pipe][nkey.id]);
            if (!action ->used) {
                if (ltsw_flexctr_resource_mismatch(unit, mode, global,
                                                   action->global_rsvd)) {
                    continue;
                }
                break;
            }
        }

        if (nkey.id >= max_id) {
            SHR_ERR_EXIT(SHR_E_FULL);
        }
    }

    /* Set rollback flag. */
    rollback = TRUE;

    /* Reset action information. */
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
    action->hint_type = act_cfg->hint_type;

    for (i = 0; i < FC_ACTION_OBJ_COUNT; i++) {
        action->objects[i] = FLEXCTR_OBJ_INVALID;
        action->objects_id[i] = 0;
    }

    /* Process action trigger. */
    if (act_cfg->flags & BCM_FLEXCTR_ACTION_FLAGS_TRIGGER) {
        trigger = &act_cfg->trigger;
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_action_trigger_process(unit, &nkey, trigger, action));
    }

    /* Process flex counter index operation. */
    index_op = &act_cfg->index_operation;
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_action_index_operation_process(unit, &nkey, index_op,
                                                     action));

    /* Process flex counter value a operation. */
    value_op = &act_cfg->operation_a;
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_action_value_operation_process(unit, &nkey,
                                                     FC_VALUE_A_TRUE,
                                                     value_op, action));

    /* Process flex counter value b operation. */
    if (act_cfg->mode == bcmFlexctrCounterModeNormal) {
        value_op = &act_cfg->operation_b;
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_action_value_operation_process(unit, &nkey,
                                                         FC_VALUE_B_TRUE,
                                                         value_op, action));
    }

    /* Assign pool and counter resource. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_pool_alloc(unit, &nkey, act_cfg, action));
    free_pool = TRUE;

    if (flexctr_eviction_enabled[unit]) {
        evict_ena = true;
        if (act_cfg->source == (bcm_flexctr_source_t)bcmiFlexctrSourceL2Dst ||
            act_cfg->source == (bcm_flexctr_source_t)bcmiFlexctrSourceL2Src ||
            act_cfg->source == (bcm_flexctr_source_t)bcmiFlexctrSourceEM_FT ||
            act_cfg->source == bcmFlexctrSourceIpmc) {
            /*
             * Don't enable eviction for hit bit counter pools for the
             * sake of API(L2/IPMC traverse) performance.
             */
            evict_ena = false;
        }
    }
    if (evict_ena) {
        /* Set pool control LT. */
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_pool_control_set(unit, &nkey, action));
        action->evict_ena = evict_ena;
    }

    /* Update flex counter trigger LT. */
    if (act_cfg->flags & BCM_FLEXCTR_ACTION_FLAGS_TRIGGER) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_action_trigger_op(unit, &nkey, trigger,
                                            BCMLT_OPCODE_INSERT));
        free_tr = TRUE;
    }

    /* Update flex counter action LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_action_insert(unit, &nkey, action));

    action->used = TRUE;

    /* Last update quantization reference count. */
    for (i = 0; i < FC_ACTION_OPOBJ_COUNT; i++) {
        ltsw_flexctr_quant_t *quant = NULL;

        if (action->quant_id[i] == FC_ID_INVALID) {
            continue;
        }

        /* Should not fail here. */
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_quant_id_to_quant(unit, action->quant_id[i],
                                            &quant, NULL, NULL));
        quant->ref_count++;
    }

    /* Last update packet attribute profile reference count. */
    for (i = 0; i < FC_ACTION_OBJ_COUNT; i++) {
        if ((action->objects[i] == bcmFlexctrObjectStaticIngPktAttribute) ||
            (action->objects[i] == bcmFlexctrObjectStaticEgrPktAttribute)) {
            ltsw_flexctr_pkt_attr_t *pkt_attr = NULL;

            /* Should not fail here. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_profile_id_to_pkt_attr(unit,
                                                     action->objects_id[i],
                                                     &pkt_attr, NULL));
            pkt_attr->ref_count++;
        }
    }

    /* Compose action id. */
    FLEXCTR_ID_COMPOSE(&nkey, *stat_counter_id);

exit:
    if (SHR_FUNC_ERR() && rollback) {
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Start rollback\n")));

        if (free_tr) {
            (void)ltsw_flexctr_action_trigger_op(unit, &nkey, trigger,
                                                 BCMLT_OPCODE_DELETE);
        }

        (void)ltsw_flexctr_op_profile_entry_remove(unit,
                                                   action->index_profile);

        (void)ltsw_flexctr_op_profile_entry_remove(unit,
                                                   action->value_a_profile);

        (void)ltsw_flexctr_op_profile_entry_remove(unit,
                                                   action->value_b_profile);
        if (free_pool) {
            (void)ltsw_flexctr_pool_free(unit, &nkey, action);
        }

        ltsw_flexctr_action_cleanup(unit, action);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter counter id to action function.
 *
 * This function is used to get an action from a stat_counter_id.
 *
 * \param [in] unit Unit number.
 * \param [in] stat_counter_id Flex counter ID.
 * \param [out] action Flex counter action.
 * \param [out] okey Flex counter key identification.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_counter_id_to_action(int unit,
                                  uint32_t stat_counter_id,
                                  ltsw_flexctr_action_t **action,
                                  ltsw_flexctr_key_t *okey)
{
    ltsw_flexctr_action_info_t *ai = flexctr_action_info[unit];
    ltsw_flexctr_key_t key = {0};
    uint32_t min_id, max_id;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(action, SHR_E_PARAM);

    FLEXCTR_ID_DECOMPOSE(stat_counter_id, &key);

    if (key.type != FLEXCTR_ACTION) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid counter action %x\n"),
                   stat_counter_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id type %x\n"), key.type));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((key.stage < FC_MIN_STAGE) || (key.stage >= FC_MAX_STAGE)){
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid counter action %x\n"),
                   stat_counter_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id stage %x\n"), key.stage));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (key.pipe < 0 || key.pipe >= flexctr_pipe_max[unit][key.stage]) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid counter action %x\n"),
                   stat_counter_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id pipe %x\n"), key.pipe));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    min_id = ai->action_min[key.stage];
    max_id = ai->action_max[key.stage];
    if (key.id < min_id || key.id >= max_id) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid counter action %x\n"),
                   stat_counter_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id %x\n"), key.id));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *action = &(ai->actions[key.stage][key.pipe][key.id]);
    if (!(*action)->used) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Counter action %x not found\n"),
                  stat_counter_id));

        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (okey) {
        sal_memcpy(okey, &key, sizeof(ltsw_flexctr_key_t));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter action destroy function.
 *
 * This function is used to destroy a flex counter action.
 *
 * \param [in] unit Unit number.
 * \param [in] stat_counter_id Flex counter ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_action_single_destroy(int unit, uint32_t stat_counter_id)
{
    ltsw_flexctr_action_t *action = NULL;
    ltsw_flexctr_key_t key = {0};
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_counter_id_to_action(unit, stat_counter_id, &action, &key));

    if (action->ref_count != 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Counter action %x is in using\n"),
                   stat_counter_id));
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    if (action->grp_ref_count != 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Counter action %x is in using group action\n"),
                   stat_counter_id));
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_action_delete(unit, &key));

    if (action->flags & BCM_FLEXCTR_ACTION_FLAGS_TRIGGER) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_action_trigger_op(unit, &key, NULL,
                                            BCMLT_OPCODE_DELETE));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_op_profile_entry_remove(unit, action->index_profile));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_op_profile_entry_remove(unit, action->value_a_profile));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_op_profile_entry_remove(unit, action->value_b_profile));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_pool_free(unit, &key, action));

    /* Last update quantization reference count. */
    for (i = 0; i < FC_ACTION_OPOBJ_COUNT; i++) {
        ltsw_flexctr_quant_t *quant = NULL;

        if (action->quant_id[i] == FC_ID_INVALID) {
            continue;
        }

        /* Should not fail here. */
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_quant_id_to_quant(unit, action->quant_id[i],
                                            &quant, NULL, NULL));
        quant->ref_count--;
    }

    /* Last update packet attribute profile reference count. */
    for (i = 0; i < FC_ACTION_OBJ_COUNT; i++) {
        if ((action->objects[i] == bcmFlexctrObjectStaticIngPktAttribute) ||
            (action->objects[i] == bcmFlexctrObjectStaticEgrPktAttribute)) {
            ltsw_flexctr_pkt_attr_t *pkt_attr = NULL;

            /* Should not fail here. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_profile_id_to_pkt_attr(unit,
                                                     action->objects_id[i],
                                                     &pkt_attr, NULL));
            pkt_attr->ref_count--;
        }
    }

    /* Reset internal action structure. */
    ltsw_flexctr_action_cleanup(unit, action);

exit:
    SHR_FUNC_EXIT();
}

/*\
 * brief Flex counter action get function.
 *
 * This function is used to get a flex counter action information.
 *
 * \param [in] unit Unit number.
 * \param [in] stat_counter_id Flex counter ID.
 * \param [out] act_cfg Flex counter action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_action_get(int unit,
                        uint32_t stat_counter_id,
                        bcm_flexctr_action_t *act_cfg)
{
    ltsw_flexctr_key_t key = {0};
    ltsw_flexctr_action_info_t *ai = flexctr_action_info[unit];
    ltsw_flexctr_action_t *action = NULL;
    bcm_flexctr_trigger_t *trigger = NULL;
    bcm_flexctr_index_operation_t *index_op = NULL;
    bcm_flexctr_value_operation_t *value_op = NULL;
    bcm_port_config_t port_config;
    int i, j;

    SHR_FUNC_ENTER(unit);

    FLEXCTR_ACTION_LOCK(unit);

    SHR_NULL_CHECK(act_cfg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_counter_id_to_action(unit, stat_counter_id, &action, &key));

    sal_memset(act_cfg, 0, sizeof(bcm_flexctr_action_t));
    act_cfg->source = action->source;
    if (flexctr_pipe_unique[unit][key.stage]) {
        if (action->global_rsvd) {
            /* global action skip ports. */
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_port_config_get(unit, &port_config));
            BCM_PBMP_ASSIGN(act_cfg->ports, port_config.per_pp_pipe[key.pipe]);
        }
    }
    act_cfg->flags = action->flags;
    act_cfg->hint = action->hint;
    act_cfg->drop_count_mode = action->drop_count_mode;
    act_cfg->exception_drop_count_enable = action->exception_drop_count_enable;
    act_cfg->egress_mirror_count_enable = action->egress_mirror_count_enable;
    act_cfg->mode = action->mode;
    act_cfg->index_num = action->index_num;
    act_cfg->hint_type = action->hint_type;

    index_op = &act_cfg->index_operation;
    /* index: object and mask_size. */
    for (i = 0; i < ai->op_obj_max[key.stage]; i++) {
        j = FC_INDEX_OP * ai->op_obj_max[key.stage] + i;

        index_op->object[i] = action->object[j];
        index_op->object_id[i] = action->object_id[j];
        index_op->quant_id[i] = action->quant_id[j];
        index_op->mask_size[i] = action->mask_size[j];
    }
    /* index: shift. */
    for (i = 0; i < ai->op_shift_max[key.stage]; i++) {
        j = FC_INDEX_OP * ai->op_shift_max[key.stage] + i;

        index_op->shift[i] = action->shift[j];
    }

    value_op = &act_cfg->operation_a;
    value_op->select = action->value_select[FC_VALUE_A_TRUE];
    value_op->type = action->value_type[FC_VALUE_A_TRUE];
    /* value A: object and mask_size. */
    for (i = 0; i < ai->op_obj_max[key.stage]; i++) {
        j = FC_VALUE_A_TRUE * ai->op_obj_max[key.stage] + i;

        value_op->object[i] = action->object[j];
        value_op->object_id[i] = action->object_id[j];
        value_op->quant_id[i] = action->quant_id[j];
        value_op->mask_size[i] = action->mask_size[j];
    }
    /* value A: shift. */
    for (i = 0; i < ai->op_shift_max[key.stage]; i++) {
        j = FC_VALUE_A_TRUE * ai->op_shift_max[key.stage] + i;

        value_op->shift[i] = action->shift[j];
    }

    if (action->mode == bcmFlexctrCounterModeNormal) {
        value_op = &act_cfg->operation_b;
        value_op->select = action->value_select[FC_VALUE_B_TRUE];
        value_op->type = action->value_type[FC_VALUE_B_TRUE];
        /* value B: object and mask_size. */
        for (i = 0; i < ai->op_obj_max[key.stage]; i++) {
            j = FC_VALUE_B_TRUE * ai->op_obj_max[key.stage] + i;

            value_op->object[i] = action->object[j];
            value_op->object_id[i] = action->object_id[j];
            value_op->quant_id[i] = action->quant_id[j];
            value_op->mask_size[i] = action->mask_size[j];
        }
        /* value B: shift. */
        for (i = 0; i < ai->op_shift_max[key.stage]; i++) {
            j = FC_VALUE_B_TRUE * ai->op_shift_max[key.stage] + i;

            value_op->shift[i] = action->shift[j];
        }
    }

    if (action->flags & BCM_FLEXCTR_ACTION_FLAGS_TRIGGER) {
        trigger = &act_cfg->trigger;

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_action_trigger_op(unit, &key, trigger,
                                            BCMLT_OPCODE_LOOKUP));
        if (trigger->trigger_type == bcmFlexctrTriggerTypeCondition ||
            trigger->trigger_type ==
            bcmFlexctrTriggerTypeConditionStartTimeStop ||
            trigger->trigger_type ==
            bcmFlexctrTriggerTypeConditionStartActionCountStop) {
            trigger->object = action->objects[0];
            trigger->object_id = action->objects_id[0];
        }
    }

exit:
    FLEXCTR_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter action traverse function.
 *
 * This function is used to traverse all flex counter action information.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn Action traverse callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int ltsw_flexctr_action_traverse(int unit,
                                 bcm_flexctr_action_traverse_cb trav_fn,
                                 void *user_data)
{
    ltsw_flexctr_action_info_t *ai = flexctr_action_info[unit];
    ltsw_flexctr_action_t *action = NULL;
    ltsw_flexctr_key_t key = {0};
    bcm_flexctr_action_t act_cfg;
    uint32_t min_id, max_id;
    int max_pipe, stat_counter_id, rv;

    SHR_FUNC_ENTER(unit);

    FLEXCTR_ACTION_LOCK(unit);

    SHR_NULL_CHECK(trav_fn, SHR_E_PARAM);

    key.type = FLEXCTR_ACTION;
    for (key.stage = FC_MIN_STAGE; key.stage < FC_MAX_STAGE; key.stage++) {
        max_pipe = flexctr_pipe_max[unit][key.stage];
        min_id = ai->action_min[key.stage];
        max_id = ai->action_max[key.stage];

        for (key.pipe = 0; key.pipe < max_pipe; key.pipe++) {
            for (key.id = min_id; key.id < max_id; key.id++) {
                action = &(ai->actions[key.stage][key.pipe][key.id]);

                if (!action->used) {
                    continue;
                }

                /* Skip internal actions. */
                if (action->options & BCMI_LTSW_FLEXCTR_OPTIONS_INTERNAL) {
                    continue;
                }

                /* Only report once for global actions in unique mode. */
                if (action->global_rsvd && (key.pipe != 0)) {
                    continue;
                }

                FLEXCTR_ID_COMPOSE(&key, stat_counter_id);

                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexctr_action_get(unit, stat_counter_id, &act_cfg));

                rv = trav_fn(unit, stat_counter_id, &act_cfg, user_data);
                if (SHR_FAILURE(rv)) {
                    break;
                }
            }
        }
    }

exit:
    FLEXCTR_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*\
 * brief Flex counter action update function.
 *
 * This function is used to update a flex counter action information.
 *
 * \param [in] unit Unit number.
 * \param [in] stat_counter_id Flex counter ID.
 * \param [out] act_cfg Flex counter action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_action_update(int unit,
                           uint32_t stat_counter_id,
                           bcm_flexctr_action_t *act_cfg)
{
    ltsw_flexctr_key_t key = {0};
    ltsw_flexctr_action_t *action = NULL;
    int phint = 0;

    SHR_FUNC_ENTER(unit);

    FLEXCTR_ACTION_LOCK(unit);

    SHR_NULL_CHECK(act_cfg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_counter_id_to_action(unit, stat_counter_id, &action, &key));

    if (action->source != act_cfg->source) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    if (action->source == (bcm_flexctr_source_t)bcmiFlexctrSourceL2Dst ||
        action->source == (bcm_flexctr_source_t)bcmiFlexctrSourceL2Src) {
        SHR_EXIT();
    }

    if ((act_cfg->source == bcmFlexctrSourceIfp ||
         act_cfg->source == bcmFlexctrSourceVfp ||
         act_cfg->source == bcmFlexctrSourceEfp ||
         act_cfg->source == bcmFlexctrSourceExactMatch) &&
        (act_cfg->hint_type == bcmFlexctrHintFieldGroupId)) {
        bcmi_field_flexctr_group_info_t grp_info;

        /* Get group priority. */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_field_flexctr_group_info_get(unit, act_cfg->source,
                                               act_cfg->hint, &grp_info));

        phint = grp_info.priority;
    } else if (act_cfg->hint_type == bcmFlexctrHintPool) {
        phint = act_cfg->hint;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_pool_update(unit, &key, phint, action));
    action->hint = act_cfg->hint;

exit:
    FLEXCTR_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter global action creation function.
 *
 * This function is used to create a flex counter global action in pipe
 * unique mode, and the action on each pipe shares the same configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] options BCM_FLEXCTR_OPTIONS_XXX options.
 * \param [in] act_cfg Flex counter action data structure.
 * \param [in/out] stat_counter_id Flex counter ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_action_global_create(int unit, int options,
                                  bcm_flexctr_action_t *act_cfg,
                                  uint32_t *stat_counter_id)
{
    int i, pipe_num, stage, rollback = false;
    bcmint_flexctr_source_info_t *si = NULL;
    uint32_t *counter_id = NULL, size;
    ltsw_flexctr_key_t key = {0};
    int options_tmp = options;
    bool with_id = false;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(stat_counter_id, SHR_E_PARAM);
    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexctr_source_info_get(unit, act_cfg->source, &si));
    stage = si->stage;

    if (!flexctr_pipe_unique[unit][stage]) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (options & BCM_FLEXCTR_OPTIONS_WITH_ID) {
        with_id = true;
        FLEXCTR_ID_DECOMPOSE(*stat_counter_id, &key);
    }

    pipe_num = flexctr_pipe_max[unit][stage];
    size = sizeof(uint32_t) * pipe_num;
    SHR_ALLOC(counter_id, size, "ltswFlexctrCounterId");
    SHR_NULL_CHECK(counter_id, SHR_E_MEMORY);
    sal_memset(counter_id, 0, size);

    for (i = 0; i < pipe_num; i++) {
        if (with_id) {
            /* Create with id. */
            key.pipe = i;
            FLEXCTR_ID_COMPOSE(&key, counter_id[i]);
            options_tmp |= BCM_FLEXCTR_OPTIONS_WITH_ID;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_action_single_create(unit, options_tmp, act_cfg,
                                               true, i, &counter_id[i]));
        if (i == 0) {
            FLEXCTR_ID_DECOMPOSE(counter_id[i], &key);
        }
        with_id = true;
        rollback = true;
    }
    /* Return the counter id with pipe 0. */
    *stat_counter_id = counter_id[0];

exit:
    if (SHR_FUNC_ERR() && rollback) {
        /* Any action create fails, destroy all associated ones. */
        for (i = 0; i < pipe_num; i++) {
            if (counter_id[i] != 0) {
                (void)ltsw_flexctr_action_single_destroy(unit, counter_id[i]);
            }
        }
    }
    SHR_FREE(counter_id);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter global action destroy function.
 *
 * \param [in] unit Unit number.
 * \param [in] stat_counter_id Flex counter ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_action_global_destroy(int unit, uint32_t stat_counter_id)
{
    int i, pipe_num;
    ltsw_flexctr_key_t key = {0};
    uint32_t counter_id;
    SHR_FUNC_ENTER(unit);

    FLEXCTR_ID_DECOMPOSE(stat_counter_id, &key);
    pipe_num = flexctr_pipe_max[unit][key.stage];

    for (i = 0; i < pipe_num; i++) {
        key.pipe = i;
        FLEXCTR_ID_COMPOSE(&key, counter_id);
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_action_single_destroy(unit, counter_id));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter action creation function.
 *
 * This function is used to create a flex counter action, which can be a
 * global action in pipe unique mode or normal one.
 *
 * \param [in] unit Unit number.
 * \param [in] options BCM_FLEXCTR_OPTIONS_XXX options.
 * \param [in] act_cfg Flex counter action data structure.
 * \param [in/out] stat_counter_id Flex counter ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_action_create(int unit, int options,
                           bcm_flexctr_action_t *act_cfg,
                           uint32_t *stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    FLEXCTR_ACTION_LOCK(unit);
    FLEXCTR_QUANT_LOCK(unit);

    SHR_NULL_CHECK(act_cfg, SHR_E_PARAM);

    if (act_cfg->flags & BCM_FLEXCTR_ACTION_FLAGS_GLOBAL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_action_global_create(unit,
                                               options,
                                               act_cfg,
                                               stat_counter_id));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_action_single_create(unit,
                                               options,
                                               act_cfg,
                                               false,
                                               0,
                                               stat_counter_id));
    }

exit:
    FLEXCTR_QUANT_UNLOCK(unit);
    FLEXCTR_ACTION_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter action destroy function.
 *
 * This function is used to destroy a flex counter action.
 *
 * \param [in] unit Unit number.
 * \param [in] stat_counter_id Flex counter ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_action_destroy(int unit, uint32_t stat_counter_id)
{
    ltsw_flexctr_action_t *action = NULL;
    ltsw_flexctr_key_t key = {0};
    SHR_FUNC_ENTER(unit);

    FLEXCTR_ACTION_LOCK(unit);
    FLEXCTR_QUANT_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_counter_id_to_action(unit, stat_counter_id,
                                           &action, &key));
    if (action->flags & BCM_FLEXCTR_ACTION_FLAGS_GLOBAL) {
        /* Global action. */
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_action_global_destroy(unit, stat_counter_id));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_action_single_destroy(unit, stat_counter_id));
    }

exit:
    FLEXCTR_QUANT_UNLOCK(unit);
    FLEXCTR_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Update flex counter info for field group.
 *
 * \param [in] unit Unit Number.
 * \param [in] source Flex counter source.
 * \param [in] group_id Field group id.
 * \param [in] old_pri Old priority.
 * \param [in] new_pri New priority.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_flexctr_field_group_update(int unit, bcm_flexctr_source_t source,
                                int group_id, int old_pri, int new_pri)
{
    bcmint_flexctr_source_info_t *si = NULL;
    ltsw_flexctr_action_info_t *ai = flexctr_action_info[unit];
    ltsw_flexctr_action_t *action = NULL;
    ltsw_flexctr_key_t key = {0};
    uint32_t min_id, max_id;
    int max_pipe, phint;

    SHR_FUNC_ENTER(unit);

    FLEXCTR_ACTION_LOCK(unit);

    if (old_pri == new_pri) {
        SHR_EXIT();
    }

    if (source == bcmFlexctrSourceIfp ||
        source == bcmFlexctrSourceVfp ||
        source == bcmFlexctrSourceEfp ||
        source == bcmFlexctrSourceExactMatch) {
        phint = new_pri;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexctr_source_info_get(unit, source, &si));
    key.stage = si->stage;

    key.type = FLEXCTR_ACTION;
    max_pipe = flexctr_pipe_max[unit][key.stage];
    min_id = ai->action_min[key.stage];
    max_id = ai->action_max[key.stage];

    for (key.pipe = 0; key.pipe < max_pipe; key.pipe++) {
        for (key.id = min_id; key.id < max_id; key.id++) {
            action = &(ai->actions[key.stage][key.pipe][key.id]);

            /* Skip used one. */
            if (!action->used) {
                continue;
            }

            /* Skip internal one. */
            if (action->options & BCMI_LTSW_FLEXCTR_OPTIONS_INTERNAL) {
                continue;
            }

            /* Check source. */
            if (action->source != source) {
                continue;
            }

            /* Check hint. */
            if (action->hint_type != BCMI_FLEXCTR_HINT_FIELD_GROUP_ID ||
                action->hint != group_id) {
                continue;
            }

            /* Update pool hint. */
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_pool_update(unit, &key, phint, action));
        }
    }

exit:
    FLEXCTR_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter group action data clean up.
 *
 * \param [in] unit Unit number.
 * \param [in|out] grp_act Flex counter group action data structure.
 */
static void
ltsw_flexctr_group_action_cleanup(int unit, ltsw_flexctr_grp_action_t *grp_act)
{
    bool global_rsvd = grp_act->global_rsvd;
    sal_memset(grp_act, 0, sizeof(ltsw_flexctr_grp_action_t));
    grp_act->global_rsvd = global_rsvd;
}

/*!
 * \brief EFLEX_GROUP_ACTION LT operation.
 *
 * This function is used to update/insert or delete an entry from
 * EFLEX_GROUP_ACTION LT.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex counter key identification.
 * \param [in] grp_act Flex counter group action data structure.
 * \param [in] opcode LT operation code.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_group_action_op(int unit,
                             ltsw_flexctr_key_t *key,
                             ltsw_flexctr_grp_action_t *grp_act,
                             bcmlt_opcode_t opcode)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bool pipe_unique = flexctr_pipe_unique[unit][key->stage];
    ltsw_flexctr_stage_info_t *st = &(st_info[key->stage]);
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t data;
    int i;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Group action lt op %d %d %d %u\n"),
              opcode, key->stage, key->pipe, key->id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->grp_act_profile, &eh));

    /* Key fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_key_to_id(unit, key, &data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->grp_act_profile_id, data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, GROUPs, key->id));

    if (pipe_unique) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PIPEs, key->pipe));
    }

    if ((opcode == BCMLT_OPCODE_DELETE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, eh, opcode, BCMLT_PRIORITY_NORMAL));
    } else {
        /* Data fields. */
        ltsw_flexctr_key_t ikey = {0};

        FLEXCTR_ID_DECOMPOSE(grp_act->parent_act_id, &ikey);
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_key_to_id(unit, &ikey, &data));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, st->act_profile_id, data));

        for (i = 1; i < grp_act->action_num; i++) {
            FLEXCTR_ID_DECOMPOSE(grp_act->stat_counter_id[i], &ikey);
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_key_to_id(unit, &ikey, &data));

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
 * \brief Cleanup flex counter group action software database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_group_action_db_cleanup(int unit)
{
    ltsw_flexctr_grp_action_info_t *gai = flexctr_grp_action_info[unit];

    SHR_FUNC_ENTER(unit);

    if (gai == NULL) {
        SHR_EXIT();
    }

    if (gai->mutex) {
        sal_mutex_destroy(gai->mutex);
    }

    SHR_FREE(gai);
    flexctr_grp_action_info[unit] = NULL;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize flex counter group action software database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_group_action_db_init(int unit)
{
    ltsw_flexctr_grp_action_info_t *gai = flexctr_grp_action_info[unit];
    uint64_t min, max;
    bcmlt_field_def_t fld_def;
    uint32_t ha_alloc_size = 0, ha_req_size = 0, pipe_req_size = 0;
    uint32_t ha_instance_size = 0, ha_array_size = 0;
    void *alloc_ptr = NULL;
    int warm = bcmi_warmboot_get(unit), stage, max_pipe, i;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (gai != NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    } else {
        SHR_ALLOC(gai, sizeof(ltsw_flexctr_grp_action_info_t), "ltswFlexctrGrpActionInfo");
        SHR_NULL_CHECK(gai, SHR_E_PARAM);
        sal_memset(gai, 0, sizeof(ltsw_flexctr_grp_action_info_t));
        flexctr_grp_action_info[unit] = gai;
    }

    /* Create mutex. */
    gai->mutex = sal_mutex_create("ltswFlexctrGrpActionInfo");
    SHR_NULL_CHECK(gai->mutex, SHR_E_MEMORY);

    /* Alloc group action buffer from HA. */
    for (stage = FC_MIN_STAGE; stage < FC_MAX_STAGE; stage++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, st_info[stage].grp_act_profile,
                                           GROUPs,
                                           &min, &max));
        gai->grp_action_min[stage] = (uint32_t)min;
        gai->grp_action_max[stage] = (uint32_t)max + 1;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit, st_info[stage].grp_act_profile,
                                   GROUP_MAPs, &fld_def));
        gai->grp_size[stage] = fld_def.depth;

        max_pipe = flexctr_pipe_max[unit][stage];

        ha_instance_size = sizeof(ltsw_flexctr_grp_action_t);
        ha_array_size = gai->grp_action_max[stage] * max_pipe;
        pipe_req_size = ha_instance_size * gai->grp_action_max[stage];
        ha_req_size = ha_instance_size * ha_array_size;
        ha_alloc_size = ha_req_size;
        alloc_ptr = bcmi_ltsw_ha_mem_alloc(unit, BCMI_HA_COMP_ID_FLEXCTR,
                                           st_info[stage].ha_grp_act_id,
                                           "bcmFlexCtrGrpAction",
                                           &ha_alloc_size);
        SHR_NULL_CHECK(alloc_ptr, SHR_E_MEMORY);
        if (ha_alloc_size < ha_req_size) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }

        if (!warm) {
            sal_memset(alloc_ptr, 0, ha_alloc_size);
        }

        rv = bcmi_ltsw_issu_struct_info_report(unit,
                 BCMI_HA_COMP_ID_FLEXCTR,
                 st_info[stage].ha_grp_act_id,
                 0, ha_instance_size, ha_array_size,
                 LTSW_FLEXCTR_GRP_ACTION_T_ID);
        if (rv != SHR_E_EXISTS) {
           SHR_IF_ERR_VERBOSE_EXIT(rv);
        }

        for (i = 0; i < max_pipe; i++) {
            gai->grp_actions[stage][i] =
                (ltsw_flexctr_grp_action_t *)(alloc_ptr + pipe_req_size * i);
        }
    }

exit:
    if (SHR_FUNC_ERR()) {
        for (stage = FC_MIN_STAGE; stage < FC_MAX_STAGE; stage++) {
            if (gai->grp_actions[stage][0] && !warm) {
                (void)bcmi_ltsw_ha_mem_free(unit, gai->grp_actions[stage][0]);
            }
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter group action parameter validation function.
 *
 * This function is used to validate group action parameters.
 *
 * \param [in] unit Unit number.
 * \param [in] group_action Flex counter group action data structure.
 * \param [out] key Flex counter key.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_grp_action_params_validate(int unit,
                                    bcm_flexctr_group_action_t *group_action,
                                    ltsw_flexctr_key_t *key)
{
    int pipe = 0;
    ltsw_flexctr_grp_action_info_t *gai = flexctr_grp_action_info[unit];
    bcmint_flexctr_source_info_t *si = NULL;
    int i, global = false;
    ltsw_flexctr_action_t *action = NULL;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexctr_source_info_get(unit, group_action->source, &si));

    if (flexctr_pipe_unique[unit][si->stage]) {
        if (group_action->flags & BCM_FLEXCTR_GROUP_ACTION_FLAGS_GLOBAL) {
            /* Ignore ports if creating a unique action. */
            pipe = 0;
            global = true;
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_config_port_to_pipe(unit, group_action->ports,
                                                  &pipe));
        }
    } else {
        if (BCM_PBMP_NOT_NULL(group_action->ports)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Ports should be NULL in global mode")));

            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    if (group_action->action_num <= 0 ||
        group_action->action_num > gai->grp_size[si->stage] + 1) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid group map num %u min 1 max %d\n"),
                   group_action->action_num, gai->grp_size[si->stage] + 1));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Check each group action member. */
    for (i = 0; i < group_action->action_num; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_counter_id_to_action(unit,
                                               group_action->stat_counter_id[i],
                                               &action, NULL));

        if ((FLEXCTR_ID_PIPE(group_action->stat_counter_id[i]) != pipe) &&
            !global) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid action[%d] %u for group action\n"),
                       i, group_action->stat_counter_id[i]));

            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        if (action->source != group_action->source) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid action[%d] %u for group action\n"),
                       i, group_action->stat_counter_id[i]));

            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    key->pipe = pipe;
    key->stage = si->stage;
    key->type = FLEXCTR_GRP_ACTION;

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \breif Flex counter group action creation function.
 *
 * This function is used to create a flex counter group action.
 *
 * \param [in] unit Unit number.
 * \param [in] options BCM_FLEXCTR_OPTIONS_XXX options.
 * \param [in] group_action Flex counter group action data structure.
 * \param [in] global Global action in pipe unique mode.
 * \param [in] pipe Pipe id, only valid when global is true.
 * \param [in/out] group_stat_counter_id Flex counter group ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_group_action_single_create(int unit, int options,
                                        bcm_flexctr_group_action_t *group_action,
                                        bool global,
                                        int pipe,
                                        uint32_t *group_stat_counter_id)
{
    ltsw_flexctr_grp_action_info_t *gai = flexctr_grp_action_info[unit];
    ltsw_flexctr_grp_action_t *grp_act = NULL;
    ltsw_flexctr_action_t *action = NULL, *tmp_action = NULL;
    ltsw_flexctr_key_t nkey = {0}, akey = {0};
    uint32_t min_id, max_id;
    int i;
    bool rollback = FALSE;
    bcmi_ltsw_flexctr_mode_t mode;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(group_action, SHR_E_PARAM);
    SHR_NULL_CHECK(group_stat_counter_id, SHR_E_PARAM);

    /* Parameter check. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_grp_action_params_validate(unit, group_action, &nkey));
    if (global) {
        nkey.pipe = pipe;
    }
    mode = flexctr_pipe_unique[unit][nkey.stage] ?
           bcmiFlexctrModePipeUnique : bcmiFlexctrModeGlobal;

    min_id = gai->grp_action_min[nkey.stage];
    max_id = gai->grp_action_max[nkey.stage];

    if (options & BCM_FLEXCTR_OPTIONS_WITH_ID) {
        nkey.id = FLEXCTR_ID_INDEX(*group_stat_counter_id);
        if (nkey.id < min_id || nkey.id >= max_id) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid group action id %d "
                                        "valid id range %d - %d\n"),
                        FLEXCTR_ID_INDEX(*group_stat_counter_id),
                        min_id, max_id - 1));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        grp_act = &(gai->grp_actions[nkey.stage][nkey.pipe][nkey.id]);
        if (grp_act->used) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Group action id %d exists\n"),
                       *group_stat_counter_id));
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
        if (ltsw_flexctr_resource_mismatch(unit, mode, global,
                                           grp_act->global_rsvd)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid Group action id %d "
                                        "Mismatch one\n"),
                       FLEXCTR_ID_INDEX(*group_stat_counter_id)));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    } else {
        /* Search an unused group counter action. */
        for (nkey.id = min_id; nkey.id < max_id; nkey.id++) {
            grp_act =
                &(gai->grp_actions[nkey.stage][nkey.pipe][nkey.id]);
            if (!grp_act ->used) {
                if (ltsw_flexctr_resource_mismatch(unit, mode, global,
                                                   grp_act->global_rsvd)) {
                    continue;
                }
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
        (ltsw_flexctr_counter_id_to_action(unit,
                                           group_action->stat_counter_id[0],
                                           &action, &akey));

    if (action->group_stat_counter_id) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "First member [%x] is used in group_action[%x]\n"),
                   group_action->stat_counter_id[0],
                   action->group_stat_counter_id));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (action->ref_count > 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "First member [%x] should not be attached\n"),
                   group_action->stat_counter_id[0]));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Compose action id. */
    FLEXCTR_ID_COMPOSE(&akey, grp_act->parent_act_id);

    /* Bookkeeping API configuration. */
    grp_act->source = group_action->source;
    grp_act->action_num = group_action->action_num;

    for (i = 0; i < group_action->action_num; i++) {
        grp_act->stat_counter_id[i] = group_action->stat_counter_id[i];
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_group_action_op(unit, &nkey, grp_act, BCMLT_OPCODE_INSERT));

    grp_act->options = options;
    grp_act->used = TRUE;

    /* Last update action reference count. */
    for (i = 0; i < group_action->action_num; i++) {
        /* Should not fail here. */
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_counter_id_to_action(unit,
                                               group_action->stat_counter_id[i],
                                               &tmp_action, NULL));
        tmp_action->grp_ref_count++;
    }

    /* Compose group action id. */
    FLEXCTR_ID_COMPOSE(&nkey, *group_stat_counter_id);

    action->group_stat_counter_id = *group_stat_counter_id;

exit:
    if (SHR_FUNC_ERR() && rollback) {
        if (grp_act) {
            ltsw_flexctr_group_action_cleanup(unit, grp_act);
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter group counter id to group action function.
 *
 * This function is used to get an action from a group_stat_counter_id.
 *
 * \param [in] unit Unit number.
 * \param [in] group_stat_counter_id Flex counter group ID.
 * \param [out] grp_action Flex counter group action.
 * \param [out] okey Flex counter key identification.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_counter_id_to_grp_action(int unit,
                                      uint32_t group_stat_counter_id,
                                      ltsw_flexctr_grp_action_t **grp_action,
                                      ltsw_flexctr_key_t *okey)
{
    ltsw_flexctr_grp_action_info_t *gai = flexctr_grp_action_info[unit];
    ltsw_flexctr_key_t key = {0};
    uint32_t min_id, max_id;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(grp_action, SHR_E_PARAM);

    FLEXCTR_ID_DECOMPOSE(group_stat_counter_id, &key);

    if (key.type != FLEXCTR_GRP_ACTION) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid group counter action %x\n"),
                   group_stat_counter_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id type %x\n"), key.type));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((key.stage < FC_MIN_STAGE) || (key.stage >= FC_MAX_STAGE)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid group counter action %x\n"),
                   group_stat_counter_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id stage %x\n"), key.stage));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (key.pipe < 0 || key.pipe >= flexctr_pipe_max[unit][key.stage]) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid group counter action %x\n"),
                   group_stat_counter_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id pipe %x\n"), key.pipe));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    min_id = gai->grp_action_min[key.stage];
    max_id = gai->grp_action_max[key.stage];
    if (key.id < min_id || key.id >= max_id) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid group counter action %x\n"),
                   group_stat_counter_id));

        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id %x\n"), key.id));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    *grp_action = &(gai->grp_actions[key.stage][key.pipe][key.id]);
    if (!(*grp_action)->used) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Counter group action %x not found\n"),
                  group_stat_counter_id));

        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    if (okey) {
        sal_memcpy(okey, &key, sizeof(ltsw_flexctr_key_t));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \breif Flex counter group action destroy function.
 *
 * This function is used to destroy a flex counter group action.
 *
 * \param [in] unit Unit number.
 * \param [in] group_stat_counter_id Flex counter group ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_group_action_single_destroy(int unit, uint32_t group_stat_counter_id)
{
    ltsw_flexctr_grp_action_t *grp_act = NULL;
    ltsw_flexctr_action_t *par_action = NULL, *action = NULL;
    ltsw_flexctr_key_t key = {0}, akey = {0};
    int i;

    SHR_FUNC_ENTER(unit);

    FLEXCTR_GRP_ACTION_LOCK(unit);
    FLEXCTR_ACTION_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_counter_id_to_grp_action(unit, group_stat_counter_id,
                                               &grp_act, &key));

    if (grp_act->ref_count != 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Counter action %x is in using\n"),
                   group_stat_counter_id));
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_counter_id_to_action(unit, grp_act->parent_act_id,
                                           &par_action, &akey));
    par_action->group_stat_counter_id = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_group_action_op(unit, &key, grp_act, BCMLT_OPCODE_DELETE));

    /* Last update action reference count. */
    for (i = 0; i < grp_act->action_num; i++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_counter_id_to_action(unit,
                                               grp_act->stat_counter_id[i],
                                               &action, NULL));
        action->grp_ref_count--;
    }

    /* Reset group action structure. */
    ltsw_flexctr_group_action_cleanup(unit, grp_act);

exit:
    FLEXCTR_ACTION_UNLOCK(unit);
    FLEXCTR_GRP_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \breif Flex counter group action get function.
 *
 * This function is used to get a flex counter group action information.
 *
 * \param [in] unit Unit number.
 * \param [in] group_stat_counter_id Flex counter group ID.
 * \param [out] group_action Flex counter group action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_group_action_get(int unit,
                              uint32_t group_stat_counter_id,
                              bcm_flexctr_group_action_t *group_action)
{
    ltsw_flexctr_key_t key = {0};
    ltsw_flexctr_grp_action_t *grp_act = NULL;
    bcm_port_config_t port_config;
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(group_action, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_counter_id_to_grp_action(unit, group_stat_counter_id,
                                               &grp_act, &key));

    sal_memset(group_action, 0, sizeof(bcm_flexctr_group_action_t));
    if (flexctr_pipe_unique[unit][key.stage]) {
        if (grp_act->global_rsvd) {
            /* global action skip ports. */
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcm_ltsw_port_config_get(unit, &port_config));
            BCM_PBMP_ASSIGN(group_action->ports,
                            port_config.per_pp_pipe[key.pipe]);
        }
    }
    group_action->source = grp_act->source;
    group_action->action_num = grp_act->action_num;

    for (i = 0; i < grp_act->action_num; i++) {
        group_action->stat_counter_id[i] = grp_act->stat_counter_id[i];
    }

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter group action traverse function.
 *
 * This function is used to traverse all flex counter group action information.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn Group action traverse callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_group_action_traverse(int unit,
                                   bcm_flexctr_group_action_traverse_cb trav_fn,
                                   void *user_data)
{
    ltsw_flexctr_grp_action_info_t *gai = flexctr_grp_action_info[unit];
    ltsw_flexctr_grp_action_t *grp_act = NULL;
    ltsw_flexctr_key_t key = {0};
    bcm_flexctr_group_action_t grp_action;
    uint32_t min_id, max_id;
    int max_pipe, group_stat_counter_id, rv;

    SHR_FUNC_ENTER(unit);

    FLEXCTR_GRP_ACTION_LOCK(unit);

    SHR_NULL_CHECK(trav_fn, SHR_E_PARAM);

    key.type = FLEXCTR_GRP_ACTION;
    for (key.stage = FC_MIN_STAGE; key.stage < FC_MAX_STAGE; key.stage++) {
        max_pipe = flexctr_pipe_max[unit][key.stage];
        min_id = gai->grp_action_min[key.stage];
        max_id = gai->grp_action_max[key.stage];

        for (key.pipe = 0; key.pipe < max_pipe; key.pipe++) {
            for (key.id = min_id; key.id < max_id; key.id++) {
                grp_act= &(gai->grp_actions[key.stage][key.pipe][key.id]);

                if (!grp_act->used) {
                    continue;
                }

                /* Skip internal actions. */
                if (grp_act->options & BCMI_LTSW_FLEXCTR_OPTIONS_INTERNAL) {
                    continue;
                }

                /* Only report once for global actions in unique mode. */
                if (grp_act->global_rsvd && (key.pipe != 0)) {
                    continue;
                }

                FLEXCTR_ID_COMPOSE(&key, group_stat_counter_id);

                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexctr_group_action_get(unit, group_stat_counter_id,
                                                   &grp_action));

                rv = trav_fn(unit, group_stat_counter_id, &grp_action, user_data);
                if (SHR_FAILURE(rv)) {
                    break;
                }
            }
        }
    }

exit:
    FLEXCTR_GRP_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Adjust stat_counter_id of global group action in pipe unique mode.
 *
 * \param [in] unit Unit number.
 * \param [in] pipe Pipe number.
 * \param [in|out] group_action Flex counter group action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static void
ltsw_flexctr_group_action_global_counter_id_adjust(int unit, int pipe,
    bcm_flexctr_group_action_t *group_action)
{
    ltsw_flexctr_key_t key = {0};
    int i;

    for (i = 0; i < group_action->action_num; i++) {
        FLEXCTR_ID_DECOMPOSE(group_action->stat_counter_id[i], &key);
        key.pipe = pipe;
        FLEXCTR_ID_COMPOSE(&key, group_action->stat_counter_id[i]);
    }
}

/*!
 * \brief Flex counter global group action creation function.
 *
 * This function is used to create a flex counter global group action in pipe
 * unique mode, and the action on each pipe shares the same configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] options BCM_FLEXCTR_OPTIONS_XXX options.
 * \param [in] group_action Flex counter group action data structure.
 * \param [in/out] group_stat_counter_id Flex counter group ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_group_action_global_create(int unit, int options,
                                        bcm_flexctr_group_action_t *group_action,
                                        uint32_t *group_stat_counter_id)
{
    int i, pipe_num, stage, rollback = false;
    bcmint_flexctr_source_info_t *si = NULL;
    bcm_flexctr_group_action_t action_tmp;
    uint32_t *counter_id = NULL, size;
    ltsw_flexctr_key_t key = {0};
    int options_tmp = options;
    bool with_id = false;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexctr_source_info_get(unit, group_action->source, &si));
    stage = si->stage;

    if (!flexctr_pipe_unique[unit][stage]) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (options & BCM_FLEXCTR_OPTIONS_WITH_ID) {
        FLEXCTR_ID_DECOMPOSE(*group_stat_counter_id, &key);
        with_id = true;
    }

    pipe_num = flexctr_pipe_max[unit][stage];
    size = sizeof(uint32_t) * pipe_num;
    SHR_ALLOC(counter_id, size, "ltswFlexctrGroupCounterId");
    SHR_NULL_CHECK(counter_id, SHR_E_MEMORY);
    sal_memset(counter_id, 0, size);
    sal_memcpy(&action_tmp, group_action, sizeof(bcm_flexctr_group_action_t));

    for (i = 0; i < pipe_num; i++) {
        if (with_id) {
            /* Create with id from the second pipe. */
            key.pipe = i;
            FLEXCTR_ID_COMPOSE(&key, counter_id[i]);
            options_tmp |= BCM_FLEXCTR_OPTIONS_WITH_ID;
            ltsw_flexctr_group_action_global_counter_id_adjust(unit, i,
                                                               &action_tmp);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_group_action_single_create(unit, options_tmp,
                                                     &action_tmp,
                                                     true, i, &counter_id[i]));
        if (i == 0) {
            FLEXCTR_ID_DECOMPOSE(counter_id[i], &key);
        }
        with_id = true;
        rollback = true;
    }
    /* Return the counter id with pipe 0. */
    *group_stat_counter_id = counter_id[0];

exit:
    if (SHR_FUNC_ERR() && rollback) {
        /* Any action create fails, destroy all associated ones. */
        for (i = 0; i < pipe_num; i++) {
            if (counter_id[i] != 0) {
                (void)ltsw_flexctr_group_action_single_destroy(unit,
                                                               counter_id[i]);
            }
        }
    }
    SHR_FREE(counter_id);
    SHR_FUNC_EXIT();
}

/*!
 * \breif Flex counter global group action destroy function.
 *
 * This function is used to destroy a flex counter global group action in
 * pipe unique mode.
 *
 * \param [in] unit Unit number.
 * \param [in] group_stat_counter_id Flex counter group ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_group_action_global_destroy(int unit,
                                         uint32_t group_stat_counter_id)
{
    int i, pipe_num;
    ltsw_flexctr_key_t key = {0};
    uint32_t counter_id;
    SHR_FUNC_ENTER(unit);

    FLEXCTR_ID_DECOMPOSE(group_stat_counter_id, &key);
    pipe_num = flexctr_pipe_max[unit][key.stage];

    for (i = 0; i < pipe_num; i++) {
        key.pipe = i;
        FLEXCTR_ID_COMPOSE(&key, counter_id);
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_group_action_single_destroy(unit, counter_id));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \breif Flex counter group action creation function.
 *
 * This function is used to create a flex counter group action.
 *
 * \param [in] unit Unit number.
 * \param [in] options BCM_FLEXCTR_OPTIONS_XXX options.
 * \param [in] group_action Flex counter group action data structure.
 * \param [in/out] group_stat_counter_id Flex counter group ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_group_action_create(int unit, int options,
                                 bcm_flexctr_group_action_t *group_action,
                                 uint32_t *group_stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    FLEXCTR_GRP_ACTION_LOCK(unit);
    FLEXCTR_ACTION_LOCK(unit);

    SHR_NULL_CHECK(group_action, SHR_E_PARAM);
    SHR_NULL_CHECK(group_stat_counter_id, SHR_E_PARAM);

    if (group_action->flags & BCM_FLEXCTR_GROUP_ACTION_FLAGS_GLOBAL) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_group_action_global_create(unit,
                                                     options,
                                                     group_action,
                                                     group_stat_counter_id));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_group_action_single_create(unit,
                                                     options,
                                                     group_action,
                                                     false, 0,
                                                     group_stat_counter_id));
    }

exit:
    FLEXCTR_ACTION_UNLOCK(unit);
    FLEXCTR_GRP_ACTION_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \breif Flex counter group action destroy function.
 *
 * \param [in] unit Unit number.
 * \param [in] group_stat_counter_id Flex counter group ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_group_action_destroy(int unit, uint32_t group_stat_counter_id)
{
    ltsw_flexctr_grp_action_t *grp_act = NULL;
    ltsw_flexctr_key_t key = {0};
    SHR_FUNC_ENTER(unit);

    FLEXCTR_GRP_ACTION_LOCK(unit);
    FLEXCTR_ACTION_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_counter_id_to_grp_action(unit, group_stat_counter_id,
                                               &grp_act, &key));
    if (grp_act->global_rsvd) {
        /* Global action. */
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_group_action_global_destroy(unit,
                                                      group_stat_counter_id));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_group_action_single_destroy(unit,
                                                      group_stat_counter_id));
    }

exit:
    FLEXCTR_ACTION_UNLOCK(unit);
    FLEXCTR_GRP_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter trigger enable update function.
 *
 * This function is used to update trigger field of
 * CTR_ING/EGR_EFLEX_TRIGGER LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex counter key identification.
 * \param [in] enable Enable/disable action trigger.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_trigger_enable_single_update(int unit, ltsw_flexctr_key_t *key,
                                          int enable)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    ltsw_flexctr_stage_info_t *st = &(st_info[key->stage]);
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
        (ltsw_flexctr_key_to_id(unit, key, &data));

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
 * \brief Flex counter trigger enable update function for global action.
 *
 * This function is used to update trigger field of
 * CTR_ING/EGR_EFLEX_TRIGGER LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex counter key identification.
 * \param [in] enable Enable/disable action trigger.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_trigger_enable_global_update(int unit, ltsw_flexctr_key_t *key,
                                          int enable)
{
    int i, pipe_num;
    ltsw_flexctr_key_t key_tmp;

    SHR_FUNC_ENTER(unit);

    pipe_num = flexctr_pipe_max[unit][key->stage];
    sal_memcpy(&key_tmp, key, sizeof(ltsw_flexctr_key_t));

    for (i = 0; i < pipe_num; i++) {
        key_tmp.pipe = i;

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_trigger_enable_single_update(unit, &key_tmp, enable));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter trigger enable lookup function.
 *
 * This function is used to lookup trigger field of
 * CTR_ING/EGR_EFLEX_TRIGGER LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex counter key identification.
 * \param [out] enable Enable/disable action trigger.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_trigger_enable_lookup(int unit, ltsw_flexctr_key_t *key, int *enable)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    ltsw_flexctr_stage_info_t *st = &(st_info[key->stage]);
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
        (ltsw_flexctr_key_to_id(unit, key, &data));

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
 * \brief Flex counter trigger enable get set function.
 *
 * This function is used to get or set a trigger enable/disable status.
 *
 * \param [in] unit Unit number.
 * \param [in] operation Flex counter operation.
 * \param [in] stat_counter_id Flex counter ID.
 * \param [in/out] enable Enable/disable action trigger.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_trigger_enable_get_set(int unit,
                                    int operation,
                                    uint32_t stat_counter_id,
                                    int *enable)
{
    ltsw_flexctr_action_t *action = NULL;
    ltsw_flexctr_key_t key = {0};
    int trigger;

    SHR_FUNC_ENTER(unit);

    FLEXCTR_ACTION_LOCK(unit);

    SHR_NULL_CHECK(enable, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_counter_id_to_action(unit, stat_counter_id, &action, &key));

    trigger = action->flags & BCM_FLEXCTR_ACTION_FLAGS_TRIGGER;

    if (!trigger) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Trigger is unavailable in action %x\n"),
                   stat_counter_id));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    switch (operation) {
        case FLEXCTR_OPERATION_GET:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_trigger_enable_lookup(unit, &key, enable));
            break;

        case FLEXCTR_OPERATION_SET:
            if (action->global_rsvd) {
                /* Update all pipes for global action. */
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexctr_trigger_enable_global_update(unit, &key,
                                                               *enable));
            } else {
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexctr_trigger_enable_single_update(unit, &key,
                                                               *enable));
            }
            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    FLEXCTR_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Force a sync op to stats table.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_stats_force_sync(int unit)
{
    int rv;

    /*
     * In case some pools are set to eviction mode and some pools are set to
     * pool mode, it needs an explicit sync op.
     */
    rv = bcm_ltsw_stat_sync(unit);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Force sync fails(%d)\n"), rv));
    }
    return rv;
}

/*!
 * \brief Update an entry from LT.
 *
 * This function is used to update an entry from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex counter key identification.
 * \param [in] counter_index Counter index of the given counter action.
 * \param [in] mode Flex counter counter mode.
 * \param [in] counter_value Statistic for the specific counter entry.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_stats_update(int unit,
                          ltsw_flexctr_key_t *key,
                          uint32_t counter_index,
                          bcm_flexctr_counter_mode_t mode,
                          bcm_flexctr_counter_value_t *counter_value)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    ltsw_flexctr_stage_info_t *st = &(st_info[key->stage]);
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
        (ltsw_flexctr_key_to_id(unit, key, &data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->act_profile_id, data));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, CTR_EFLEX_INDEXs, counter_index));

    /* Data fields */
    switch (mode) {
        case bcmFlexctrCounterModeNormal:
            /* CTR_B and CTR_A_LOWER fields are valid */
            data = counter_value->value[0];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, CTR_A_LOWERs, data));

            data = counter_value->value[1];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, CTR_Bs, data));
            break;

        case bcmFlexctrCounterModeWide:
            /* CTR_A_LOWER and CTR_A_UPPER fields are valid */
            data = counter_value->value[0];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, CTR_A_UPPERs, data));

            data = counter_value->value[1];
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, CTR_A_LOWERs, data));
            break;

        case bcmFlexctrCounterModeSlim:
            /* CTR_C, CTR_B and CTR_A_LOWER fields are valid */
            data = counter_value->value[0];
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
 * \param [in] key Flex counter key identification.
 * \param [in] counter_index Counter index of the given counter action.
 * \param [in] sync_mode Sync mode.
 * \param [in] mode Flex counter counter mode.
 * \param [out] counter_value Statistic for the specific counter entry.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_stats_lookup(int unit,
                          ltsw_flexctr_key_t *key,
                          uint32_t counter_index,
                          bool sync_mode,
                          bcm_flexctr_counter_mode_t mode,
                          bcm_flexctr_counter_value_t *counter_value)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    ltsw_flexctr_stage_info_t *st = &(st_info[key->stage]);
    uint32_t entry_attr;
    uint64_t data;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Counter stats lt lookup %d action %d %u index %u\n"),
              key->stage, key->pipe, key->id, counter_index));

    sal_memset(counter_value, 0, sizeof(bcm_flexctr_counter_value_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->stats, &eh));

    /* Key fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_key_to_id(unit, key, &data));

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
        case bcmFlexctrCounterModeNormal:
            /* CTR_B and CTR_A_LOWER fields are valid */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, CTR_A_LOWERs, &data));
            counter_value->value[0] = data;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, CTR_Bs, &data));
            counter_value->value[1] = data;
            break;

        case bcmFlexctrCounterModeWide:
            /* CTR_A_LOWER and CTR_A_UPPER fields are valid */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, CTR_A_UPPERs, &data));
            counter_value->value[0] = data;

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, CTR_A_LOWERs, &data));
            counter_value->value[1] = data;
            break;

        case bcmFlexctrCounterModeSlim:
            /* CTR_C, CTR_B and CTR_A_LOWER fields are valid */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, CTR_A_LOWERs, &data));
            counter_value->value[0] = data;
            counter_value->value[1] = 0;
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

/* Max flex counter value. */
#define CTR_EFLEX_MAX 0xFFFFFFFFFFFFFFFF

/*!
 * \brief Collect global counter value from per-pipe ones, or update per-pipe
 * counter based on global one.
 *
 * This function refers to bcmcth_ctr_eflex_ctr_buff_update and
 * bcmcth_ctr_eflex_ctr_buff_global_update in SDKLT lib.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex counter key identification.
 * \param [in] mode Flex counter counter mode.
 * \param [in] pipe_num Pipe number.
 * \param [in] dir Direction, 0 indicates collect global counter from per-pipe,
 *                 1 indicates update per-pipe counter from global.
 * \param [in|out] global_value Global counter value pointer.
 * \param [in|out] pipe_value Pipe counter value pointer.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_stats_transform(int unit, ltsw_flexctr_key_t *key,
                             bcm_flexctr_counter_mode_t mode,
                             int pipe_num, bool dir,
                             bcm_flexctr_counter_value_t *global_value,
                             bcm_flexctr_counter_value_t *pipe_value)
{
    uint32_t stat_counter_id;
    ltsw_flexctr_action_t *action = NULL;
    int i, update_mode, pipe, avg_num, num_valid_pipes, num_div;
    uint64_t avg_total, ctr_val;

    SHR_FUNC_ENTER(unit);

    FLEXCTR_ID_COMPOSE(key, stat_counter_id);
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_counter_id_to_action(unit, stat_counter_id,
                                           &action, NULL));

    if (dir == 0) {
        /* Collect global counter value from per-pipe ones. */
        sal_memset(global_value, 0, sizeof(bcm_flexctr_counter_value_t));
        /*
         * For SLIM and WIDE mode, since only CTR A is valid,
         * use update mode for CTR A only.
         * Within a counter entry, CTR A is the MSB counter.
         */
        update_mode = action->value_type[0];

        /* For MIN update mode, set all counter values to max value */
        for (i = 0; i < FC_VALUE_COUNT; i++) {
            if (update_mode == bcmFlexctrValueOperationTypeMin) {
                global_value->value[i] = CTR_EFLEX_MAX;
            }
        }

        for (i = 0; i < FC_VALUE_COUNT; i++) {
            avg_total = avg_num = 0;
            /*
             * For NORMAL mode, both CTR A and CTR B are valid.
             * Hence use update modes from both.
             */
            if (mode == bcmFlexctrCounterModeNormal) {
                update_mode = action->value_type[i];
            }

            for (pipe = 0; pipe < pipe_num; pipe++) {
                switch (update_mode) {
                    case bcmFlexctrValueOperationTypeMax:
                        /* Determine max counter value across all pipes */
                        if (pipe_value[pipe].value[i] >
                            global_value->value[i]) {
                            global_value->value[i] =
                                pipe_value[pipe].value[i];
                        }
                        break;

                    case bcmFlexctrValueOperationTypeMin:
                        /* Determine min counter value across all pipes */
                        if (pipe_value[pipe].value[i] <
                            global_value->value[i]) {
                            global_value->value[i] =
                                pipe_value[pipe].value[i];
                        }
                        break;

                    case bcmFlexctrValueOperationTypeSetBit:
                    case bcmFlexctrValueOperationTypeXor:
                        /* Counter value is OR'ed across all pipes */
                        global_value->value[i] |= pipe_value[pipe].value[i];
                        break;

                    case bcmFlexctrValueOperationTypeAverage:
                        /*
                         * Average counter value is calculated using
                         * non-zero value across all pipes
                         */
                        if (pipe_value[pipe].value[i]) {
                            avg_total += pipe_value[pipe].value[i];
                            avg_num++;
                        }
                        break;

                    case bcmFlexctrValueOperationTypeNoop:
                    case bcmFlexctrValueOperationTypeSet:
                    case bcmFlexctrValueOperationTypeInc:
                    case bcmFlexctrValueOperationTypeDec:
                    case bcmFlexctrValueOperationTypeRdec:
                        /* Counter value is added across all pipes */
                        global_value->value[i] += pipe_value[pipe].value[i];
                        break;

                    default:
                        SHR_ERR_EXIT(SHR_E_PARAM);
                }
            }

            if (update_mode == bcmFlexctrValueOperationTypeAverage && avg_num) {
                global_value->value[i] = avg_total / avg_num;
            }
        }
    } else {
        num_div = num_valid_pipes = pipe_num;
        /* update per-pipe counter based on global one.*/
        for (i = 0; i < FC_VALUE_COUNT; i++) {
            ctr_val = global_value->value[i];

            for (pipe = 0; pipe < pipe_num; pipe++) {
                update_mode = action->value_type[i];

                if (update_mode == bcmFlexctrValueOperationTypeNoop ||
                    update_mode == bcmFlexctrValueOperationTypeSet ||
                    update_mode == bcmFlexctrValueOperationTypeInc ||
                    update_mode == bcmFlexctrValueOperationTypeMax ||
                    update_mode == bcmFlexctrValueOperationTypeAverage) {
                    /*
                     * For NOOP, SET, INC, MAX, AVG update modes
                     * write counter value to first valid pipe and skip the rest
                     */
                    pipe_value[pipe].value[i] = ctr_val;
                    break;
                } else if (update_mode == bcmFlexctrValueOperationTypeMin ||
                           update_mode == bcmFlexctrValueOperationTypeSetBit ||
                           update_mode == bcmFlexctrValueOperationTypeXor) {
                    /*
                     * For MIN, SETBIT, XOR update modes
                     * write counter value to all valid pipes
                     */
                    pipe_value[pipe].value[i] = ctr_val;
                } else if (update_mode == bcmFlexctrValueOperationTypeDec ||
                           update_mode == bcmFlexctrValueOperationTypeRdec) {
                    /*
                     * For DEC, RDEC update modes
                     * divide counter value amongst all valid pipes
                     */
                    if (ctr_val % num_valid_pipes == 0) {
                        /*
                         * Counter value can be equally divided amongst
                         * valid pipes
                         */
                        pipe_value[pipe].value[i] = ctr_val/num_valid_pipes;
                    } else {
                        /*
                         * If counter value cannot be equally divided amongst
                         * valid pipes, then the last pipe will get remaining
                         * unequal portion
                         */
                        pipe_value[pipe].value[i] = ctr_val/num_valid_pipes;
                        num_div--;
                        if (num_div == 0) {
                            pipe_value[pipe].value[i] +=
                                ctr_val % num_valid_pipes;
                        }
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update an entry from LT for global action in pipe unique.
 *
 * This function is used to update an entry from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex counter key identification.
 * \param [in] counter_index Counter index of the given counter action.
 * \param [in] mode Flex counter counter mode.
 * \param [in] counter_value Statistic for the specific counter entry.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_stats_global_update(int unit,
                                 ltsw_flexctr_key_t *key,
                                 uint32_t counter_index,
                                 bcm_flexctr_counter_mode_t mode,
                                 bcm_flexctr_counter_value_t *counter_value)
{
    int i, pipe_num;
    ltsw_flexctr_key_t key_tmp;
    bcm_flexctr_counter_value_t *pipe_value = NULL;
    uint32_t size;

    SHR_FUNC_ENTER(unit);

    pipe_num = flexctr_pipe_max[unit][key->stage];
    size = sizeof(bcm_flexctr_counter_value_t) * pipe_num;
    SHR_ALLOC(pipe_value, size, "ltswFlexctrCounterValue");
    SHR_NULL_CHECK(pipe_value, SHR_E_MEMORY);
    sal_memset(pipe_value, 0, size);
    sal_memcpy(&key_tmp, key, sizeof(ltsw_flexctr_key_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_stats_transform(unit, key, mode, pipe_num, 1,
                                      counter_value, pipe_value));

    for (i = 0; i < pipe_num; i++) {
        key_tmp.pipe = i;

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_stats_update(unit, &key_tmp, counter_index,
                                       mode, &pipe_value[i]));
    }

exit:
    SHR_FREE(pipe_value);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Lookup an entry from LT for global action in pipe unique mode.
 *
 * This function is used to lookup an entry from LT.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex counter key identification.
 * \param [in] counter_index Counter index of the given counter action.
 * \param [in] sync_mode Sync mode.
 * \param [in] mode Flex counter counter mode.
 * \param [out] counter_value Statistic for the specific counter entry.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_stats_global_lookup(int unit,
                                 ltsw_flexctr_key_t *key,
                                 uint32_t counter_index,
                                 bool sync_mode,
                                 bcm_flexctr_counter_mode_t mode,
                                 bcm_flexctr_counter_value_t *counter_value)
{
    int i, pipe_num;
    ltsw_flexctr_key_t key_tmp;
    bcm_flexctr_counter_value_t *pipe_value = NULL;
    uint32_t size;

    SHR_FUNC_ENTER(unit);

    pipe_num = flexctr_pipe_max[unit][key->stage];
    size = sizeof(bcm_flexctr_counter_value_t) * pipe_num;
    SHR_ALLOC(pipe_value, size, "ltswFlexctrCounterValue");
    SHR_NULL_CHECK(pipe_value, SHR_E_MEMORY);
    sal_memset(pipe_value, 0, size);
    sal_memcpy(&key_tmp, key, sizeof(ltsw_flexctr_key_t));

    for (i = 0; i < pipe_num; i++) {
        key_tmp.pipe = i;

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_stats_lookup(unit, &key_tmp, counter_index,
                                       sync_mode, mode, &pipe_value[i]));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_stats_transform(unit, key, mode, pipe_num, 0,
                                      counter_value, pipe_value));

exit:
    SHR_FREE(pipe_value);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter stats parameter validation function.
 *
 * This function is used to validate stats parameter.
 *
 * \param [in] unit Unit number.
 * \param [in] stat_counter_id Flex counter ID.
 * \param [in] num_entries Total number of counter entries need to get or set.
 * \param [in] counter_indexes Each counter entry index of the given counter action.
 * \param [in] counter_values Statistic for the specific counter entry.
 * \param [out] mode Flex counter counter mode.
 * \param [out] key Flex counter key identification.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_stats_params_validate(int unit,
                                   uint32_t stat_counter_id,
                                   uint32_t num_entries,
                                   uint32_t *counter_indexes,
                                   bcm_flexctr_counter_value_t *counter_values,
                                   bcm_flexctr_counter_mode_t *mode,
                                   ltsw_flexctr_key_t *key)
{
    ltsw_flexctr_action_t *action = NULL;
    int i;

    SHR_FUNC_ENTER(unit);

    FLEXCTR_ACTION_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_counter_id_to_action(unit, stat_counter_id, &action, key));

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
    FLEXCTR_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

static int
ltsw_flexctr_hit_index_get(int unit,
                           uint32_t stat_counter_id,
                           uint32_t num_entries,
                           uint32_t *counter_indexes)
{
    int i;
    uint32_t lt_index = 0;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < num_entries; i++) {
        lt_index = counter_indexes[i];
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_field_entry_hit_index_get(unit,
                                            lt_index,
                                            &counter_indexes[i]));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter statistic get set function.
 *
 * This function is used to get or set the specific statistic
 * of a given counter action.
 *
 * \param [in] unit Unit number.
 * \param [in] operation Flex counter operation.
 * \param [in] stat_counter_id Flex counter ID.
 * \param [in] sync Sync mode.
 * \param [in] num_entries Total number of counter entries need to get or set.
 * \param [in] counter_indexes Each counter entry index of the given counter action.
 * \param [in/out] counter_values Statistic for the specific counter entry.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_stats_get_set(int unit,
                           int operation,
                           uint32_t stat_counter_id,
                           bool sync,
                           uint32_t num_entries,
                           uint32_t *counter_indexes,
                           bcm_flexctr_counter_value_t *counter_values)
{
    bcm_flexctr_counter_value_t *value = NULL;
    ltsw_flexctr_key_t key = {0};
    bcm_flexctr_counter_mode_t mode;
    bcm_flexctr_action_t action;
    int i;
    uint32_t index;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(counter_indexes, SHR_E_PARAM);
    SHR_NULL_CHECK(counter_values, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_flexctr_action_get(unit,
                                     stat_counter_id,
                                     &action));

    if ((action.source == bcmFlexctrSourceExactMatch) &&
        (action.index_operation.object[0] == bcmFlexctrObjectIngEmFtHitIndex0)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_hit_index_get(unit,
                                        stat_counter_id,
                                        num_entries,
                                        counter_indexes));
    }

    /* Parameter check. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_stats_params_validate(unit,
                                            stat_counter_id,
                                            num_entries,
                                            counter_indexes,
                                            counter_values,
                                            &mode, &key));

    if ((operation == FLEXCTR_OPERATION_GET) && sync &&
        flexctr_eviction_enabled[unit]) {
        ltsw_flexctr_action_t *act = NULL;

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_counter_id_to_action(unit,
                                               stat_counter_id,
                                               &act, NULL));
        if (act->evict_ena) {
            /* Try a sync op, but do not want it block API calling. */
            (void)ltsw_flexctr_stats_force_sync(unit);
        }
    }

    for (i = 0; i < num_entries; i++) {
        index = counter_indexes[i];
        value = &counter_values[i];

        switch (operation) {
            case FLEXCTR_OPERATION_GET:
                if (action.flags & BCM_FLEXCTR_ACTION_FLAGS_GLOBAL) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (ltsw_flexctr_stats_global_lookup(unit, &key, index,
                                                          sync, mode, value));
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (ltsw_flexctr_stats_lookup(unit, &key, index,
                                                   sync, mode, value));
                }
                break;

            case FLEXCTR_OPERATION_SET:
                if (action.flags & BCM_FLEXCTR_ACTION_FLAGS_GLOBAL) {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (ltsw_flexctr_stats_global_update(unit, &key, index,
                                                          mode, value));
                } else {
                    SHR_IF_ERR_VERBOSE_EXIT
                        (ltsw_flexctr_stats_update(unit, &key, index, mode,
                                                   value));
                }
                break;

            default:
                SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter packet attribute profile insert function.
 *
 * This function is used to insert a packet attribute profile LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex counter key identification.
 * \param [in] pkt_attr Internal flex counter packet attribute data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_packet_attribute_profile_insert(int unit,
                                             ltsw_flexctr_key_t *key,
                                             ltsw_flexctr_pkt_attr_t *pkt_attr)
{
    ltsw_flexctr_pkt_attr_info_t *pai = flexctr_pkt_attr_info[unit];
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bool pipe_unique = flexctr_pipe_unique[unit][key->stage];
    ltsw_flexctr_stage_info_t *st = &(st_info[key->stage]);
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmint_flexctr_pkt_attr_bus_info_t *bi = NULL;
    const char *symbol = NULL;
    uint64_t data;
    uint32_t i;
    int bit_pos;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Packet attribute lt insert %d %d %u\n"),
              key->stage, key->pipe, key->id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->pkt_attribute, &eh));

    /* Key fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->pkt_attribute_id, key->id));

    if (pipe_unique) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PIPEs, key->pipe));
    }

    /* Data fields. */
    for (i = 0; i < pai->pkt_attr_size[key->stage]; i++) {
        bit_pos = pkt_attr->selector_bit_pos[i];

        if (bit_pos != FC_PKT_ATTR_SELECTOR_BIT_POS_INVALID) {
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexctr_pkt_attr_bus_info_get(unit, key->stage,
                                                         bit_pos, &bi));

            symbol = bi->symbol;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_symbol_add(eh, PKT_ATTRIBUTE_MAPs, i,
                                                    &symbol, 1));

            data = TRUE;
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_array_add(eh, PKT_ATTRIBUTEs, i, &data, 1));
        }
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

/*!
 * \brief Flex counter packet attribute profile delete function.
 *
 * This function is used to delete a packet attribute profile LT entry.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex counter key identification.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_packet_attribute_profile_delete(int unit,
                                             ltsw_flexctr_key_t *key)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bool pipe_unique = flexctr_pipe_unique[unit][key->stage];
    ltsw_flexctr_stage_info_t *st = &(st_info[key->stage]);
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Packet attribute lt delete %d %d %u\n"),
              key->stage, key->pipe, key->id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->pkt_attribute, &eh));

    /* Key fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->pkt_attribute_id, key->id));

    if (pipe_unique) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PIPEs, key->pipe));
    }

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
 * \brief Cleanup flex counter packet attribute software database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_packet_attribute_db_cleanup(int unit)
{
    ltsw_flexctr_pkt_attr_info_t *pai = flexctr_pkt_attr_info[unit];

    SHR_FUNC_ENTER(unit);

    if (pai == NULL) {
        SHR_EXIT();
    }

    if (pai->mutex) {
        sal_mutex_destroy(pai->mutex);
    }

    SHR_FREE(pai);
    flexctr_pkt_attr_info[unit] = NULL;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize flex counter packet attribute software database.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_packet_attribute_db_init(int unit)
{
    ltsw_flexctr_pkt_attr_info_t *pai = flexctr_pkt_attr_info[unit];
    uint64_t min, max;
    bcmlt_field_def_t fld_def;
    uint32_t ha_alloc_size = 0, ha_req_size = 0, pipe_req_size = 0;
    void *alloc_ptr = NULL;
    int warm = bcmi_warmboot_get(unit), stage, max_pipe, i;
    bool pkt_attr_support = FALSE;

    SHR_FUNC_ENTER(unit);

    if (pai != NULL) {
        SHR_ERR_EXIT(SHR_E_INIT);
    } else {
        SHR_ALLOC(pai, sizeof(ltsw_flexctr_pkt_attr_info_t),
                  "ltswFlexctrPktAttrInfo");
        SHR_NULL_CHECK(pai, SHR_E_PARAM);
        sal_memset(pai, 0, sizeof(ltsw_flexctr_pkt_attr_info_t));
        flexctr_pkt_attr_info[unit] = pai;
    }

    /* Create mutex. */
    pai->mutex = sal_mutex_create("ltswFlexctrPktAttrInfo");
    SHR_NULL_CHECK(pai->mutex, SHR_E_MEMORY);

    /* Clear packet attribute initialized flag. */
    pai->pkt_attr_initialized = FALSE;

    /* Alloc packet attribute buffer from HA. */
    for (stage = FC_MIN_STAGE; stage < FC_MAX_STAGE; stage++) {
        /* Check PKT_ATTRIBUTE LT support. */
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_lt_field_validation(unit,
                                              st_info[stage].pkt_attribute,
                                              st_info[stage].pkt_attribute_id,
                                              &pkt_attr_support));
        if (!pkt_attr_support) {
            break;
        }

        if (!warm) {
            /* Reset all LTs. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_clear(unit, st_info[stage].pkt_attribute));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_clear(unit, st_info[stage].pkt_attribute_object));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get(unit, st_info[stage].pkt_attribute,
                                           st_info[stage].pkt_attribute_id,
                                           &min, &max));
        pai->pkt_attr_min[stage] = (uint32_t)min;
        pai->pkt_attr_max[stage] = (uint32_t)max + 1;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_def_get(unit, st_info[stage].pkt_attribute,
                                   PKT_ATTRIBUTEs, &fld_def));
        pai->pkt_attr_size[stage] = fld_def.depth;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get
                (unit, st_info[stage].pkt_attribute_object,
                 PKT_ATTRIBUTE_OBJECT_INSTANCE_IDs, &min, &max));
        pai->object_instance_max[stage] = (uint32_t)max + 1;

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_field_value_range_get
                (unit, st_info[stage].pkt_attribute_object,
                 st_info[stage].pkt_attribute_object_value, &min, &max));
        pai->object_value_max[stage] = (uint32_t)max + 1;

        max_pipe = flexctr_pipe_max[unit][stage];

        pipe_req_size =
            sizeof(ltsw_flexctr_pkt_attr_t) * pai->pkt_attr_max[stage];
        ha_req_size = pipe_req_size * max_pipe;
        ha_alloc_size = ha_req_size;
        alloc_ptr = bcmi_ltsw_ha_mem_alloc(unit, BCMI_HA_COMP_ID_FLEXCTR,
                                           st_info[stage].ha_pkt_attr_id,
                                           "bcmFlexCtrPktAttr",
                                           &ha_alloc_size);
        SHR_NULL_CHECK(alloc_ptr, SHR_E_MEMORY);
        if (ha_alloc_size < ha_req_size) {
            SHR_ERR_EXIT(SHR_E_MEMORY);
        }

        if (!warm) {
            sal_memset(alloc_ptr, 0, ha_alloc_size);
        }

        for (i = 0; i < max_pipe; i++) {
            pai->pkt_attrs[stage][i] =
                (ltsw_flexctr_pkt_attr_t *)(alloc_ptr + pipe_req_size * i);
        }
    }

    if (!pkt_attr_support) {
        SHR_EXIT();
    }

    /* Set packet attribute initialized flag. */
    pai->pkt_attr_initialized = TRUE;

exit:
    if (SHR_FUNC_ERR()) {
        for (stage = FC_MIN_STAGE; stage < FC_MAX_STAGE; stage++) {
            if (pai->pkt_attrs[stage][0] && !warm) {
                (void)bcmi_ltsw_ha_mem_free(unit, pai->pkt_attrs[stage][0]);
            }
        }
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter packet attribute profile parameter validation function.
 *
 * This function is used to validate packet attribute profile parameters.
 *
 * \param [in] unit Unit number.
 * \param [in] attr_profile Flex counter packet attribute profile data
 *                          structure.
 * \param [out] key Flex counter key.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_packet_attribute_profile_params_validate(
    int unit,
    bcm_flexctr_packet_attribute_profile_t *attr_profile,
    ltsw_flexctr_key_t *key)
{
    ltsw_flexctr_pkt_attr_info_t *pai = flexctr_pkt_attr_info[unit];
    SHR_BITDCL used_by_attrs[SHRi_BITDCLSIZE(FC_PKT_ATTR_TYPE_ENUM_COUNT)];
    bcmint_flexctr_pkt_attr_info_t *pkt_attr_info = NULL;
    bcm_flexctr_packet_attribute_type_t attr_type;
    bcm_flexctr_packet_attribute_selector_t *attr_selectors;
    uint32 num_selectors, attr_mask;
    uint8_t total_bit_count;
    int i, first_stage = -1, pipe = 0;

    SHR_FUNC_ENTER(unit);

    SHR_BITCLR_RANGE(used_by_attrs, 0, FC_PKT_ATTR_TYPE_ENUM_COUNT);

    /* Check the number of packet attribute selectors. */
    num_selectors = attr_profile->num_selectors;
    if (num_selectors <= 0 ||
        num_selectors > BCMI_LTSW_FLEXCTR_PKT_ATTR_SELECTOR_NUM_MAX) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid packet attribute "
                                    "selector number %d\n"),
                   num_selectors));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    total_bit_count = 0;

    /* Check packet attribute selectors. */
    attr_selectors = attr_profile->attr_selectors;
    for (i = 0; i < num_selectors; i++) {
        attr_type = attr_selectors[i].attr_type;
        attr_mask = attr_selectors[i].attr_mask;

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexctr_pkt_attr_info_get(unit, attr_type,
                                                 &pkt_attr_info));

        /* Check if the packet attribute selector type is overlay. */
        if (SHR_BITGET(used_by_attrs, attr_type)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid packet attribute "
                                        "selector type %d: overlay!\n"),
                       attr_type));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        /* Check the stage for each packet attribute selector. */
        if (first_stage == -1) {
            first_stage = pkt_attr_info->stage;
        } else {
            /* All attribute selector types should belong to the same stage. */
            if (pkt_attr_info->stage != first_stage) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid packet attribute "
                                            "selector type %d with different "
                                            "stage %d\n"),
                           attr_type, pkt_attr_info->stage));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }
        }

        /* Check packet attribute selector mask. */
        if (attr_mask <= 0 || attr_mask > pkt_attr_info->mask) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid packet attribute "
                                        "selector mask 0x%x (valid 0x%x)\n"),
                       attr_mask, pkt_attr_info->mask));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        /* Calculate the total packet attribute selector bit count. */
        while (attr_mask) {
            if (attr_mask & 0x1) {
                total_bit_count += 1;

                if (total_bit_count > pai->pkt_attr_size[first_stage]) {
                    break;
                }
            }

            attr_mask >>= 1;
        } /* end while. */

        if (total_bit_count > pai->pkt_attr_size[first_stage]) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Current packet attribute selector "
                                        "bit count %d is out of range (%d)\n"),
                       total_bit_count, pai->pkt_attr_size[first_stage]));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        /* Set packet attribute selector type bit is in used. */
        SHR_BITSET(used_by_attrs, attr_type);
    }

    if (flexctr_pipe_unique[unit][first_stage]) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_config_port_to_pipe(unit, attr_profile->ports,
                                              &pipe));
    } else {
        if (BCM_PBMP_NOT_NULL(attr_profile->ports)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Ports should be NULL in global mode")));

            SHR_ERR_EXIT(SHR_E_PARAM);
        }
    }

    sal_memset(key, 0, sizeof(ltsw_flexctr_key_t));

    key->pipe = pipe;
    key->stage = first_stage;
    key->type = FLEXCTR_PKT_ATTR;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter packet attribute profile process function.
 *
 * This function is used to process flex counter packet attribute profile.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex counter key identification.
 * \param [in] attr_profile Flex counter packet attribute profile data
 *                          structure.
 * \param [out] pkt_attr Internal flex counter packet attribute data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_packet_attribute_profile_process(
    int unit,
    ltsw_flexctr_key_t *key,
    bcm_flexctr_packet_attribute_profile_t *attr_profile,
    ltsw_flexctr_pkt_attr_t *pkt_attr)
{
    ltsw_flexctr_pkt_attr_info_t *pai = flexctr_pkt_attr_info[unit];
    bcmint_flexctr_pkt_attr_info_t *pkt_attr_info = NULL;
    bcm_flexctr_packet_attribute_type_t attr_type;
    bcm_flexctr_packet_attribute_selector_t *attr_selectors;
    uint32 attr_mask, tmp_attr_mask;
    uint8_t current_bit_position, start_bit, bit_count;
    int i;

    SHR_FUNC_ENTER(unit);

    SHR_BITCLR_RANGE(pkt_attr->used_by_attrs, 0, FC_PKT_ATTR_TYPE_ENUM_COUNT);

    /* Get current packet attribute selector bit position. */
    current_bit_position = 0;

    pkt_attr->num_selectors = attr_profile->num_selectors;
    attr_selectors = attr_profile->attr_selectors;
    for (i = 0; i < pkt_attr->num_selectors; i++) {
        attr_type = attr_selectors[i].attr_type;
        attr_mask = attr_selectors[i].attr_mask;

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexctr_pkt_attr_info_get(unit, attr_type,
                                                 &pkt_attr_info));

        /* Calculate each packet attribute selector bit position with mask. */
        start_bit = pkt_attr_info->start_bit_pos;
        tmp_attr_mask = attr_mask;
        bit_count = 0;
        while (tmp_attr_mask) {
            if (tmp_attr_mask & 0x1) {
                if (current_bit_position >= pai->pkt_attr_size[key->stage]) {
                    LOG_ERROR(BSL_LOG_MODULE,
                              (BSL_META_U(unit, "Current packet attribute "
                                                "selector bit position %d is "
                                                "out of range (0 ~ %d)\n"),
                               current_bit_position,
                               (pai->pkt_attr_size[key->stage] - 1)));
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }

                pkt_attr->selector_bit_pos[current_bit_position] = start_bit;
                current_bit_position += 1;
                bit_count += 1;
            }

            start_bit += 1;
            tmp_attr_mask >>= 1;
        } /* end while. */

        pkt_attr->selector_type[i] = attr_type;
        pkt_attr->selector_mask[i] = attr_mask;

        /* Set packet attribute selector type bit is in used. */
        SHR_BITSET(pkt_attr->used_by_attrs, attr_type);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter packet attribute profile creation function.
 *
 * This function is used to create a flex counter packet attribute profile
 * for given packet attributes.
 *
 * \param [in] unit Unit number.
 * \param [in] options BCM_FLEXCTR_PACKET_ATTRIBUTE_OPTIONS_XXX options.
 * \param [in] attr_profile Flex counter packet attribute profile data
 *                          structure.
 * \param [in/out] profile_id Created flex counter packet attribute profile ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_packet_attribute_profile_create(
    int unit,
    int options,
    bcm_flexctr_packet_attribute_profile_t *attr_profile,
    int *profile_id)
{
    ltsw_flexctr_pkt_attr_info_t *pai = flexctr_pkt_attr_info[unit];
    ltsw_flexctr_pkt_attr_t *pkt_attr = NULL;
    bcmi_ltsw_flexctr_pkt_attr_obj_inst_t *obj_inst = NULL;
    ltsw_flexctr_key_t nkey = {0}, okey = {0};
    uint32_t min_id, max_id;
    int i;

    SHR_FUNC_ENTER(unit);

    FLEXCTR_PKT_ATTR_LOCK(unit);

    SHR_NULL_CHECK(attr_profile, SHR_E_PARAM);
    SHR_NULL_CHECK(profile_id, SHR_E_PARAM);

    /* Parameter check. */
    FLEXCTR_PKT_ATTR_OPTIONS_CHECK(unit, options);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_packet_attribute_profile_params_validate(unit,
                                                               attr_profile,
                                                               &nkey));

    min_id = pai->pkt_attr_min[nkey.stage];
    max_id = pai->pkt_attr_max[nkey.stage];

    if (options & BCM_FLEXCTR_PACKET_ATTRIBUTE_OPTIONS_REPLACE) {
        FLEXCTR_ID_DECOMPOSE(*profile_id, &okey);
        nkey.id = okey.id;
        if ((okey.pipe != nkey.pipe) ||
            (okey.stage != nkey.stage) ||
            (okey.type != nkey.type) ||
            (okey.id < min_id || okey.id >= max_id)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid packet attribute profile id "
                                        "%x\n"),
                       *profile_id));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        /* Search existing packet attribute profile. */
        pkt_attr = &(pai->pkt_attrs[nkey.stage][nkey.pipe][nkey.id]);
        if (!pkt_attr->used) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Packet attribute profile id %x "
                                        "not found\n"),
                       *profile_id));
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }

        /* Check if the packet attribute profile is in using. */
        if (pkt_attr->ref_count != 0) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Packet attribute profile id %x "
                                        "is in using\n"),
                       *profile_id));
            SHR_ERR_EXIT(SHR_E_BUSY);
        }
    } else if (options & BCM_FLEXCTR_PACKET_ATTRIBUTE_OPTIONS_WITH_ID) {
        nkey.id = FLEXCTR_ID_INDEX(*profile_id);
        if (nkey.id < min_id || nkey.id >= max_id) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid packet attribute profile id "
                                        "%x valid id range %d - %d\n"),
                       FLEXCTR_ID_INDEX(*profile_id), min_id, max_id - 1));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        /* Search existing packet attribute profile. */
        pkt_attr = &(pai->pkt_attrs[nkey.stage][nkey.pipe][nkey.id]);
        if (pkt_attr->used) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Packet attribute profile id %x "
                                        "exists\n"),
                       *profile_id));
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
    } else {
        for (nkey.id = 0; nkey.id < max_id; nkey.id++) {
            pkt_attr = &(pai->pkt_attrs[nkey.stage][nkey.pipe][nkey.id]);
            if (!pkt_attr->used) {
                break;
            }
        }

        if (nkey.id >= max_id) {
            SHR_ERR_EXIT(SHR_E_FULL);
        }
    }

    /* Initialize packet attribute selector arrays. */
    for (i = 0; i < pai->pkt_attr_size[nkey.stage]; i++) {
        pkt_attr->selector_type[i] = FC_PKT_ATTR_SELECTOR_TYPE_INVALID;
        pkt_attr->selector_mask[i] = 0;
        pkt_attr->selector_bit_pos[i] = FC_PKT_ATTR_SELECTOR_BIT_POS_INVALID;
    }

    for (i = 0; i < pai->object_instance_max[nkey.stage]; i++) {
        obj_inst = &pkt_attr->obj_inst[i];
        obj_inst->object_value = FC_PKT_ATTR_OBJECT_VALUE_INVALID;
    }

    /* Process flex counter packet attribute profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_packet_attribute_profile_process(unit, &nkey,
                                                       attr_profile,
                                                       pkt_attr));

    /* Update flex counter packet attribute LT. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_packet_attribute_profile_insert(unit, &nkey, pkt_attr));

    pkt_attr->used = TRUE;
    pkt_attr->ref_count = 0;

    /* Compose packet attribute profile id. */
    FLEXCTR_ID_COMPOSE(&nkey, *profile_id);

exit:
    FLEXCTR_PKT_ATTR_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter packet attribute profile destroy function.
 *
 * This function is used to destroy a flex counter packet attribute profile
 * for given flex counter packet attribute profile ID.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Flex counter packet attribute profile ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_packet_attribute_profile_destroy(int unit, int profile_id)
{
    ltsw_flexctr_pkt_attr_t *pkt_attr = NULL;
    ltsw_flexctr_key_t key = {0};

    SHR_FUNC_ENTER(unit);

    FLEXCTR_PKT_ATTR_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_profile_id_to_pkt_attr(unit, profile_id,
                                             &pkt_attr, &key));

    if (pkt_attr->ref_count != 0) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Packet attribute profile id %x "
                                    "is in using\n"),
                   profile_id));
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_packet_attribute_profile_delete(unit, &key));

    /* Reset internal packet attribute structure. */
    sal_memset(pkt_attr, 0, sizeof(ltsw_flexctr_pkt_attr_t));

exit:
    FLEXCTR_PKT_ATTR_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter packet attribute profile get function.
 *
 * This function is used to get a flex counter packet attribute profile
 * information for given flex counter packet attributes profile ID.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Flex counter packet attribute profile ID.
 * \param [out] attr_profile Flex counter packet attribute profile data
 *                           structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_packet_attribute_profile_get(
    int unit,
    int profile_id,
    bcm_flexctr_packet_attribute_profile_t *attr_profile)
{
    ltsw_flexctr_key_t key = {0};
    ltsw_flexctr_pkt_attr_t *pkt_attr = NULL;
    bcm_port_config_t port_config;
    bcm_flexctr_packet_attribute_selector_t *attr_selectors;
    int i;

    SHR_FUNC_ENTER(unit);

    FLEXCTR_PKT_ATTR_LOCK(unit);

    SHR_NULL_CHECK(attr_profile, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_profile_id_to_pkt_attr(unit, profile_id,
                                             &pkt_attr, &key));

    sal_memset(attr_profile, 0,
               sizeof(bcm_flexctr_packet_attribute_profile_t));

    if (flexctr_pipe_unique[unit][key.stage]) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_port_config_get(unit, &port_config));
        BCM_PBMP_ASSIGN(attr_profile->ports,
                        port_config.per_pp_pipe[key.pipe]);
    }

    /* Get the number of packet attribute selectors. */
    attr_profile->num_selectors = pkt_attr->num_selectors;

    /* Get the content of packet attribute selectors. */
    attr_selectors = attr_profile->attr_selectors;

    for (i = 0; i < pkt_attr->num_selectors; i++) {
        attr_selectors[i].attr_type = pkt_attr->selector_type[i];
        attr_selectors[i].attr_mask = pkt_attr->selector_mask[i];
    }

exit:
    FLEXCTR_PKT_ATTR_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter packet attribute profile traverse function.
 *
 * This function is used to traverse all flex counter packet attribute profiles
 * information.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn Packet attribute profile traverse callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
ltsw_flexctr_packet_attribute_profile_traverse(
    int unit,
    bcm_flexctr_packet_attribute_profile_traverse_cb trav_fn,
    void *user_data)
{
    ltsw_flexctr_pkt_attr_info_t *pai = flexctr_pkt_attr_info[unit];
    ltsw_flexctr_pkt_attr_t *pkt_attr = NULL;
    ltsw_flexctr_key_t key = {0};
    bcm_flexctr_packet_attribute_profile_t attr_profile;
    uint32_t min_id, max_id;
    int max_pipe, profile_id, rv;

    SHR_FUNC_ENTER(unit);

    FLEXCTR_PKT_ATTR_LOCK(unit);

    SHR_NULL_CHECK(trav_fn, SHR_E_PARAM);

    key.type = FLEXCTR_PKT_ATTR;
    for (key.stage = FC_MIN_STAGE; key.stage < FC_MAX_STAGE; key.stage++) {
        max_pipe = flexctr_pipe_max[unit][key.stage];
        min_id = pai->pkt_attr_min[key.stage];
        max_id = pai->pkt_attr_max[key.stage];

        for (key.pipe = 0; key.pipe < max_pipe; key.pipe++) {
            for (key.id = min_id; key.id < max_id; key.id++) {
                pkt_attr = &(pai->pkt_attrs[key.stage][key.pipe][key.id]);

                if (!pkt_attr->used) {
                    continue;
                }

                FLEXCTR_ID_COMPOSE(&key, profile_id);
                SHR_IF_ERR_VERBOSE_EXIT
                    (ltsw_flexctr_packet_attribute_profile_get(unit,
                                                               profile_id,
                                                               &attr_profile));

                rv = trav_fn(unit, profile_id, &attr_profile, user_data);
                if (SHR_FAILURE(rv)) {
                    break;
                }
            }
        }
    }

exit:
    FLEXCTR_PKT_ATTR_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter packet attribute map parameter validation function.
 *
 * This function is used to validate packet attribute map parameters.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Flex counter packet attribute profile ID.
 * \param [in] attr_map Flex counter packet attribute map data structure.
 * \param [in] offset_value Offset value for this set of packet attribute
 *                          map entries.
 * \param [out] key Flex counter key.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_packet_attribute_map_params_validate(
    int unit,
    int profile_id,
    bcm_flexctr_packet_attribute_map_t *attr_map,
    uint32 offset_value,
    ltsw_flexctr_key_t *key)
{
    ltsw_flexctr_pkt_attr_info_t *pai = flexctr_pkt_attr_info[unit];
    ltsw_flexctr_pkt_attr_t *pkt_attr = NULL;
    bcmint_flexctr_pkt_attr_info_t *pkt_attr_info = NULL;
    bcm_flexctr_packet_attribute_type_t attr_type;
    bcm_flexctr_packet_attribute_map_entry_t *attr_map_entries = NULL;
    uint32 num_entries, attr_value, selector_mask, value;
    int i, selector, pkt_type = 0, vlan_tag = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_profile_id_to_pkt_attr(unit, profile_id,
                                             &pkt_attr, key));

    /* Check the number of packet attribute map entries. */
    num_entries = attr_map->num_entries;
    if (num_entries != pkt_attr->num_selectors) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid packet attribute "
                                    "map entries number %d\n"),
                   num_entries));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Check the the packet attribute offset value. */
    if (offset_value >= pai->object_value_max[key->stage]) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "Invalid packet attribute "
                                    "map offset value 0x%x\n"),
                   offset_value));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Check packet attribute map entries. */
    attr_map_entries = attr_map->attr_map_entries;
    for (i = 0; i < num_entries; i++) {
        attr_type = attr_map_entries[i].attr_type;
        attr_value = attr_map_entries[i].attr_value;

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexctr_pkt_attr_info_get(unit, attr_type,
                                                 &pkt_attr_info));

        /* Check the stage for each packet attribute selector. */
        if (pkt_attr_info->stage != key->stage) {
            /* All attribute selector types should belong to the same stage. */
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid packet attribute "
                                        "selector type %d with different "
                                        "stage %d\n"),
                       attr_type, pkt_attr_info->stage));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        /* Check if the packet attribute selector type is selected. */
        if (!SHR_BITGET(pkt_attr->used_by_attrs, attr_type)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid packet attribute "
                                        "selector type %d: not found!\n"),
                       attr_type));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        /* Check packet attribute selector value with mask. */
        selector_mask = 0;
        for (selector = 0; selector < pkt_attr->num_selectors; selector++) {
            if (attr_type == pkt_attr->selector_type[selector]) {
                selector_mask = pkt_attr->selector_mask[selector];
                break;
            }
        }

        /* Not found. */
        if (selector >= pkt_attr->num_selectors) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Mismatch packet attribute "
                                       "selector type %d in profule\n"),
                      attr_type));

            return SHR_E_INTERNAL;
        }

        if (attr_type == bcmFlexctrPacketAttributeTypeIngPktType) {
            pkt_type = (int)attr_value;
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexctr_pkt_type_value_get(unit, pkt_type,
                                                      &value));
        } else if (attr_type == bcmFlexctrPacketAttributeTypeIngVlanTag ||
                   attr_type == bcmFlexctrPacketAttributeTypeEgrVlanTag) {
            vlan_tag = (int)attr_value;
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexctr_vlan_tag_value_get(unit, vlan_tag,
                                                      &value));
        } else if ((attr_type == bcmFlexctrPacketAttributeTypeIngColor) ||
                   (attr_type ==
                        bcmFlexctrPacketAttributeTypeIngFieldIngressColor) ||
                   (attr_type == bcmFlexctrPacketAttributeTypeEgrColor)) {

            if ((attr_value < 0) || (attr_value >= bcmColorCount)) {
                LOG_ERROR(BSL_LOG_MODULE,
                          (BSL_META_U(unit, "Invalid packet attribute "
                                            "selector value 0x%x for "
                                            "selector type %d\n"),
                           attr_value, attr_type));
                SHR_ERR_EXIT(SHR_E_PARAM);
            }

            value = BCMI_LTSW_QOS_COLOR_ENCODING(attr_value);
        } else {
            value = attr_value;
        }

        if (value > selector_mask) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "Invalid packet attribute "
                                        "selector value 0x%x for "
                                        "selector type %d (mask 0x%x)\n"),
                       value, attr_type, selector_mask));
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief EFLEX_PKT_ATTRIBUTE_OBJECT LT operation.
 *
 * This function is used to update/insert or delete an entry from
 * EFLEX_PKT_ATTRIBUTE_OBJECT LT.
 *
 * \param [in] unit Unit number.
 * \param [in] key Flex counter key identification.
 * \param [in] attr_map Flex counter packet attribute map data structure.
 * \param [in/out] offset_value Offset value for this set of packet attribute
 *                             map entries.
 * \param [in] opcode LT operation code.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_packet_attribute_map_op(int unit,
    ltsw_flexctr_key_t *key,
    bcm_flexctr_packet_attribute_map_t *attr_map,
    uint32 *offset_value,
    bcmlt_opcode_t opcode)
{
    ltsw_flexctr_pkt_attr_info_t *pai = flexctr_pkt_attr_info[unit];
    int dunit = bcmi_ltsw_dev_dunit(unit);
    bool pipe_unique = flexctr_pipe_unique[unit][key->stage];
    ltsw_flexctr_stage_info_t *st = &(st_info[key->stage]);
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    ltsw_flexctr_pkt_attr_t *pkt_attr = NULL;
    bcmi_ltsw_flexctr_pkt_attr_obj_inst_t *obj_inst = NULL;
    bcm_flexctr_packet_attribute_type_t attr_type;
    bcm_flexctr_packet_attribute_map_entry_t *attr_map_entries = NULL;
    uint64_t data;
    uint32 num_entries, attr_mask, attr_value, value, bit_value, offset_index;
    uint8_t current_bit_position;
    int i, selector, pkt_type, vlan_tag;

    SHR_FUNC_ENTER(unit);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Packet attribute object lt op %d %d %d %u\n"),
              opcode, key->stage, key->pipe, key->id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->pkt_attribute_object, &eh));

    /* Key fields. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, st->pkt_attribute_object_id, key->id));

    if (pipe_unique) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PIPEs, key->pipe));
    }

    /* Get packet attribute selector type/mask by sequence. */
    pkt_attr = &(pai->pkt_attrs[key->stage][key->pipe][key->id]);
    offset_index = 0;
    current_bit_position = 0;
    num_entries = attr_map->num_entries;
    attr_map_entries = attr_map->attr_map_entries;
    for (i = 0; i < pkt_attr->num_selectors; i++) {
        attr_type = pkt_attr->selector_type[i];
        attr_mask = pkt_attr->selector_mask[i];

        /* Get packet attribute selector value from attr_map_entries. */
        attr_value = 0;
        for (selector = 0; selector < num_entries; selector++) {
            if (attr_type == attr_map_entries[selector].attr_type) {
                attr_value = attr_map_entries[selector].attr_value;
                break;
            }
        }

        if (attr_type == bcmFlexctrPacketAttributeTypeIngPktType) {
            pkt_type = (int)attr_value;
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexctr_pkt_type_value_get(unit, pkt_type, &value));
        } else if (attr_type == bcmFlexctrPacketAttributeTypeIngVlanTag ||
                   attr_type == bcmFlexctrPacketAttributeTypeEgrVlanTag) {
            vlan_tag = (int)attr_value;
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexctr_vlan_tag_value_get(unit, vlan_tag, &value));
        } else if ((attr_type == bcmFlexctrPacketAttributeTypeIngColor) ||
                   (attr_type ==
                        bcmFlexctrPacketAttributeTypeIngFieldIngressColor) ||
                   (attr_type == bcmFlexctrPacketAttributeTypeEgrColor)) {
            value = BCMI_LTSW_QOS_COLOR_ENCODING(attr_value);
        } else {
            value = attr_value;
        }

        /* Calculate offset index for current packet attribute selectors. */
        while (attr_mask) {
            if (attr_mask & 0x1) {
                bit_value = (value & 0x1);
                offset_index |= (bit_value << current_bit_position);
                current_bit_position += 1;
            }

            attr_mask >>= 1;
            value >>= 1;
        } /* end while. */
    }

    data = offset_index;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, PKT_ATTRIBUTE_OBJECT_INSTANCE_IDs, data));

    obj_inst = &pkt_attr->obj_inst[offset_index];
    if (opcode == BCMLT_OPCODE_DELETE || opcode == BCMLT_OPCODE_LOOKUP) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, eh, opcode, BCMLT_PRIORITY_NORMAL));

        if (opcode == BCMLT_OPCODE_LOOKUP) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_get(eh, st->pkt_attribute_object_value,
                                       &data));
            *offset_value = (uint32_t)data;
        } else {
            /* Update bookkeeping API configuration (BCMLT_OPCODE_DELETE). */
            obj_inst->object_value = FC_PKT_ATTR_OBJECT_VALUE_INVALID;
            obj_inst->num_entries = 0;
            sal_memset(obj_inst->map_entry_type, 0,
                       sizeof(obj_inst->map_entry_type));
            sal_memset(obj_inst->map_entry_value, 0,
                       sizeof(obj_inst->map_entry_value));
        }
    } else {
        /* Data fields. */
        data = *offset_value;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, st->pkt_attribute_object_value, data));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, eh, opcode, BCMLT_PRIORITY_NORMAL));

        /* Update bookkeeping API configuration (BCMLT_OPCODE_INSERT). */
        obj_inst->object_value = *offset_value;
        obj_inst->num_entries = num_entries;
        for (i = 0; i < obj_inst->num_entries; i++) {
            obj_inst->map_entry_type[i] = attr_map_entries[i].attr_type;
            obj_inst->map_entry_value[i] = attr_map_entries[i].attr_value;
        }
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
 * \brief Flex counter packet attribute map entry add function.
 *
 * This function is used to add an offset/object entry to a
 * packet attribute profile for given packet attributes.
 * Note: the num_entries and related attribute map entry array
 * (in bcm_flexctr_packet_attribute_map_t) should be
 * the same as num_selectors and attr_selectors
 * (in bcm_flexctr_packet_attribute_profile_t) in the flex counter
 * packet attribute profile creation function
 * (bcm_flexctr_packet_attribute_profile_create()) for the same profile_id.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Flex counter packet attribute profile ID.
 * \param [in] attr_map Flex counter packet attribute map data structure.
 * \param [in] offset_value Offset value for this set of packet attribute
 *                          map entries.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_packet_attribute_map_add(
    int unit,
    int profile_id,
    bcm_flexctr_packet_attribute_map_t *attr_map,
    uint32 offset_value)
{
    ltsw_flexctr_key_t nkey = {0};

    SHR_FUNC_ENTER(unit);

    FLEXCTR_PKT_ATTR_LOCK(unit);

    SHR_NULL_CHECK(attr_map, SHR_E_PARAM);

    /* Parameter check. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_packet_attribute_map_params_validate(unit,
                                                           profile_id,
                                                           attr_map,
                                                           offset_value,
                                                           &nkey));

    /* Process flex counter packet attribute map add operation. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_packet_attribute_map_op(unit, &nkey,
                                              attr_map, &offset_value,
                                              BCMLT_OPCODE_INSERT));

exit:
    FLEXCTR_PKT_ATTR_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter packet attribute map entry delete function.
 *
 * This function is used to delete an offset/object entry from a
 * packet attribute profile for given packet attributes.
 * Note: the num_entries and related attribute map entry array
 * (in bcm_flexctr_packet_attribute_map_t) should be
 * the same as num_selectors and attr_selectors
 * (in bcm_flexctr_packet_attribute_profile_t) in the flex counter
 * packet attribute profile creation function
 * (bcm_flexctr_packet_attribute_profile_create()) for the same profile_id.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Flex counter packet attribute profile ID.
 * \param [in] attr_map Flex counter packet attribute map data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_packet_attribute_map_delete(
    int unit,
    int profile_id,
    bcm_flexctr_packet_attribute_map_t *attr_map)
{
    ltsw_flexctr_key_t nkey = {0};

    SHR_FUNC_ENTER(unit);

    FLEXCTR_PKT_ATTR_LOCK(unit);

    SHR_NULL_CHECK(attr_map, SHR_E_PARAM);

    /* Parameter check. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_packet_attribute_map_params_validate(unit,
                                                           profile_id,
                                                           attr_map,
                                                           0,
                                                           &nkey));

    /* Process flex counter packet attribute map delete operation. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_packet_attribute_map_op(unit, &nkey,
                                              attr_map, NULL,
                                              BCMLT_OPCODE_DELETE));

exit:
    FLEXCTR_PKT_ATTR_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter packet attribute map entry delete all function.
 *
 * This function is used to delete all offset/object entries from a
 * packet attribute profile for given flex counter packet attribute profile ID.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Flex counter packet attribute profile ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_packet_attribute_map_delete_all(int unit, int profile_id)
{
    ltsw_flexctr_pkt_attr_info_t *pai = flexctr_pkt_attr_info[unit];
    ltsw_flexctr_pkt_attr_t *pkt_attr = NULL;
    ltsw_flexctr_key_t key = {0};
    bcmi_ltsw_flexctr_pkt_attr_obj_inst_t *obj_inst = NULL;
    bcm_flexctr_packet_attribute_map_t attr_map;
    bcm_flexctr_packet_attribute_map_entry_t *attr_map_entries = NULL;
    int i, selector;

    SHR_FUNC_ENTER(unit);

    FLEXCTR_PKT_ATTR_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_profile_id_to_pkt_attr(unit, profile_id,
                                             &pkt_attr, &key));

    for (i = 0; i < pai->object_instance_max[key.stage]; i++) {
        obj_inst = &pkt_attr->obj_inst[i];

        if (obj_inst->object_value == FC_PKT_ATTR_OBJECT_VALUE_INVALID) {
            continue;
        }

        sal_memset(&attr_map, 0, sizeof(bcm_flexctr_packet_attribute_map_t));

        attr_map.num_entries = obj_inst->num_entries;
        attr_map_entries = attr_map.attr_map_entries;

        for (selector = 0; selector < obj_inst->num_entries; selector++) {
            attr_map_entries[selector].attr_type =
                obj_inst->map_entry_type[selector];
            attr_map_entries[selector].attr_value=
                obj_inst->map_entry_value[selector];
        }

        /* Process flex counter packet attribute map delete operation. */
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_packet_attribute_map_op(unit, &key,
                                                  &attr_map, NULL,
                                                  BCMLT_OPCODE_DELETE));
    }

exit:
    FLEXCTR_PKT_ATTR_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter packet attribute map entry get function.
 *
 * This function is used to get an offset/object entry from a
 * packet attribute profile for given packet attributes.
 * Note: the num_entries and related attribute map entry array
 * (in bcm_flexctr_packet_attribute_map_t) should be
 * the same as num_selectors and attr_selectors
 * (in bcm_flexctr_packet_attribute_profile_t) in the flex counter
 * packet attribute profile creation function
 * (bcm_flexctr_packet_attribute_profile_create()) for the same profile_id.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Flex counter packet attribute profile ID.
 * \param [in] attr_map Flex counter packet attribute map data structure.
 * \param [out] offset_value Offset value for this set of packet attribute
 *                           map entries.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_packet_attribute_map_get(
    int unit,
    int profile_id,
    bcm_flexctr_packet_attribute_map_t *attr_map,
    uint32 *offset_value)
{
    ltsw_flexctr_key_t nkey = {0};

    SHR_FUNC_ENTER(unit);

    FLEXCTR_PKT_ATTR_LOCK(unit);

    SHR_NULL_CHECK(attr_map, SHR_E_PARAM);
    SHR_NULL_CHECK(offset_value, SHR_E_PARAM);

    /* Parameter check. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_packet_attribute_map_params_validate(unit,
                                                           profile_id,
                                                           attr_map,
                                                           0,
                                                           &nkey));

    /* Process flex counter packet attribute map get operation. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_packet_attribute_map_op(unit, &nkey,
                                              attr_map, offset_value,
                                              BCMLT_OPCODE_LOOKUP));

exit:
    FLEXCTR_PKT_ATTR_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter packet attribute map entry traverse function.
 *
 * This function is used to traverse all flex counter packet attribute
 * map entries information for given flex counter packet attribute profile ID.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Flex counter packet attribute profile ID.
 * \param [in] trav_fn Packet attribute map entry traverse callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
ltsw_flexctr_packet_attribute_map_traverse(
    int unit,
    int profile_id,
    bcm_flexctr_packet_attribute_map_traverse_cb trav_fn,
    void *user_data)
{
    ltsw_flexctr_pkt_attr_info_t *pai = flexctr_pkt_attr_info[unit];
    ltsw_flexctr_pkt_attr_t *pkt_attr = NULL;
    ltsw_flexctr_key_t key = {0};
    bcmi_ltsw_flexctr_pkt_attr_obj_inst_t *obj_inst = NULL;
    bcm_flexctr_packet_attribute_map_t attr_map;
    bcm_flexctr_packet_attribute_map_entry_t *attr_map_entries = NULL;
    int i, selector, rv;

    SHR_FUNC_ENTER(unit);

    FLEXCTR_PKT_ATTR_LOCK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_profile_id_to_pkt_attr(unit, profile_id,
                                             &pkt_attr, &key));

    for (i = 0; i < pai->object_instance_max[key.stage]; i++) {
        obj_inst = &pkt_attr->obj_inst[i];

        if (obj_inst->object_value == FC_PKT_ATTR_OBJECT_VALUE_INVALID) {
            continue;
        }

        sal_memset(&attr_map, 0, sizeof(bcm_flexctr_packet_attribute_map_t));

        attr_map.num_entries = obj_inst->num_entries;
        attr_map_entries = attr_map.attr_map_entries;

        for (selector = 0; selector < obj_inst->num_entries; selector++) {
            attr_map_entries[selector].attr_type =
                obj_inst->map_entry_type[selector];
            attr_map_entries[selector].attr_value=
                obj_inst->map_entry_value[selector];
        }

        rv = trav_fn(unit, &attr_map, obj_inst->object_value, user_data);
        if (SHR_FAILURE(rv)) {
            break;
        }
    }

exit:
    FLEXCTR_PKT_ATTR_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter packet attribute info get function.
 *
 * This function is used to get a packet attribute info.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Flex counter packet attribute profile ID.
 * \param [out] info Flex counter packet attribute information.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_packet_attribute_info_get(
    int unit,
    int profile_id,
    bcmi_ltsw_flexctr_packet_attribute_info_t *info)
{
    ltsw_flexctr_pkt_attr_info_t *pai = flexctr_pkt_attr_info[unit];
    ltsw_flexctr_pkt_attr_t *pkt_attr = NULL;
    ltsw_flexctr_key_t key = {0};
    bcmi_ltsw_flexctr_pkt_attr_obj_inst_t *obj_inst = NULL;
    bcmi_ltsw_flexctr_pkt_attr_obj_inst_t *obj_inst_info = NULL;
    int type = 0;
    int i, selector;

    SHR_FUNC_ENTER(unit);

    FLEXCTR_PKT_ATTR_LOCK(unit);

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    type = FLEXCTR_ID_TYPE(profile_id);

    if (type != FLEXCTR_PKT_ATTR) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_profile_id_to_pkt_attr(unit, profile_id,
                                             &pkt_attr, &key));

    info->stage = key.stage;
    info->direction = st_info[key.stage].dir;
    info->pipe = key.pipe;
    info->pkt_attr_index = key.id;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_key_to_id(unit, &key,
                                &(info->pkt_attr_profile_id)));

    info->obj_inst_max = pai->object_instance_max[key.stage];

    for (i = 0; i < info->obj_inst_max; i++) {
        obj_inst = &pkt_attr->obj_inst[i];
        obj_inst_info = &info->obj_inst[i];

        obj_inst_info->object_value = obj_inst->object_value;
        obj_inst_info->num_entries = obj_inst->num_entries;

        for (selector = 0; selector < obj_inst->num_entries; selector++) {
            obj_inst_info->map_entry_type[selector] =
                obj_inst->map_entry_type[selector];
            obj_inst_info->map_entry_value[selector] =
                obj_inst->map_entry_value[selector];
        }
    }

exit:
    FLEXCTR_PKT_ATTR_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize flex counter pipe mode.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_pipe_mode_init(int unit)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    int stage;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t data;

    SHR_FUNC_ENTER(unit);

    for (stage = FC_MIN_STAGE; stage < FC_MAX_STAGE; stage++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, st_info[stage].config, &eh));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                                  BCMLT_PRIORITY_NORMAL));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_get(eh, st_info[stage].pipe_unique, &data));

        if (data) {
            flexctr_pipe_max[unit][stage] = bcmi_ltsw_dev_max_pp_pipe_num(unit);
        } else {
            flexctr_pipe_max[unit][stage] = 1;
        }

        flexctr_pipe_unique[unit][stage] = (bool)data;

        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize flex counter misc.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_misc_init(int unit)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    int stage, max_pipe, pipe;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int warm = bcmi_warmboot_get(unit);

    SHR_FUNC_ENTER(unit);

    if (warm) {
        SHR_EXIT();
    }

    for (stage = FC_MIN_STAGE; stage < FC_MAX_STAGE; stage++) {
        max_pipe = flexctr_pipe_max[unit][stage];

        for (pipe = 0; pipe < max_pipe; pipe++) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_allocate(dunit, st_info[stage].err_stats, &eh));

            /* Key fields */
            if (flexctr_pipe_unique[unit][stage]) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(eh, PIPEs, pipe));
            }

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
 * \brief Initialize reserved resources info.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_rsvd_init(int unit)
{
    ltsw_flexctr_pool_info_t *pi = flexctr_pool_info[unit];
    ltsw_flexctr_pool_t *pool = NULL;
    ltsw_flexctr_quant_info_t *qi = flexctr_quant_info[unit];
    ltsw_flexctr_quant_t *quant = NULL;
    ltsw_flexctr_action_info_t *ai = flexctr_action_info[unit];
    ltsw_flexctr_action_t *action = NULL;
    ltsw_flexctr_grp_action_info_t *gai = flexctr_grp_action_info[unit];
    ltsw_flexctr_grp_action_t *grp_act = NULL;
    ltsw_flexctr_operation_profile_info_t *oi = flexctr_op_profile_info[unit];
    ltsw_flexctr_operation_profile_t *op_profile = NULL;
    int stage, base, pipe, max_pipe;
    int pool_id, max_pool;
    int quant_id, max_quant;
    int act_id, max_action;
    int grp_id, max_grp;
    int op_id, max_op_id;
    const char *properties[FC_MAX_STAGE][5]
        = {{BCMI_CPN_GLOBAL_FLEXCTR_ING_POOL_NUM_RESERVED,
            BCMI_CPN_GLOBAL_FLEXCTR_ING_QUANT_NUM_RESERVED,
            BCMI_CPN_GLOBAL_FLEXCTR_ING_ACTION_NUM_RESERVED,
            BCMI_CPN_GLOBAL_FLEXCTR_ING_GROUP_NUM_RESERVED,
            BCMI_CPN_GLOBAL_FLEXCTR_ING_OP_PROFILE_NUM_RESERVED},
           {BCMI_CPN_GLOBAL_FLEXCTR_EGR_POOL_NUM_RESERVED,
            BCMI_CPN_GLOBAL_FLEXCTR_EGR_QUANT_NUM_RESERVED,
            BCMI_CPN_GLOBAL_FLEXCTR_EGR_ACTION_NUM_RESERVED,
            BCMI_CPN_GLOBAL_FLEXCTR_EGR_GROUP_NUM_RESERVED,
            BCMI_CPN_GLOBAL_FLEXCTR_EGR_OP_PROFILE_NUM_RESERVED}};

    for (stage = FC_MIN_STAGE; stage < FC_MAX_STAGE; stage++) {
        /* Skip in global mode. */
        if (flexctr_pipe_unique[unit][stage] != 1) {
            continue;
        }

        pi->rsvd_num[stage] =  bcmi_ltsw_property_get(unit,
            properties[stage][0], 0);
        qi->rsvd_num[stage] = bcmi_ltsw_property_get(unit,
            properties[stage][1], 0);
        ai->rsvd_num[stage] = bcmi_ltsw_property_get(unit,
            properties[stage][2], 0);
        gai->rsvd_num[stage] = bcmi_ltsw_property_get(unit,
            properties[stage][3], 0);
        oi->rsvd_num[stage] = bcmi_ltsw_property_get(unit,
            properties[stage][4], 0);
    }

    for (stage = FC_MIN_STAGE; stage < FC_MAX_STAGE; stage++) {
        /* Skip in global mode. */
        if (flexctr_pipe_unique[unit][stage] != 1) {
            continue;
        }

        max_pipe = flexctr_pipe_max[unit][stage];

        /* Mark pool rsvd. */
        if (pi->rsvd_num[stage]) {
            max_pool = pi->npools[stage];
            base = max_pool - pi->rsvd_num[stage];
            for (pipe = 0; pipe < max_pipe; pipe++) {
                for (pool_id = base; pool_id < max_pool; pool_id++) {
                    if (pi->pools[stage][pipe] != NULL) {
                        pool = &(pi->pools[stage][pipe][pool_id]);
                        pool->global_rsvd = 1;
                    }
                }
            }
        }

        /* Mark quantization rsvd. */
        if (qi->rsvd_num[stage]) {
            max_quant = qi->quant_max[stage];
            base = max_quant - qi->rsvd_num[stage];
            for (pipe = 0; pipe < max_pipe; pipe++) {
                for (quant_id = base; quant_id < max_quant; quant_id++) {
                    if (qi->quants[stage][pipe] != NULL) {
                        quant = &(qi->quants[stage][pipe][quant_id]);
                        quant->global_rsvd = 1;
                    }
                }
            }
        }

        /* Mark action rsvd. */
        if (ai->rsvd_num[stage]) {
            max_action = ai->action_max[stage];
            base = max_action - ai->rsvd_num[stage];
            for (pipe = 0; pipe < max_pipe; pipe++) {
                for (act_id = base; act_id < max_action; act_id++) {
                    if (ai->actions[stage][pipe] != NULL) {
                        action = &(ai->actions[stage][pipe][act_id]);
                        action->global_rsvd = 1;
                    }
                }
            }
        }

        /* Mark group rsvd. */
        if (gai->rsvd_num[stage]) {
            max_grp = gai->grp_action_max[stage];
            base = max_grp - gai->rsvd_num[stage];
            for (pipe = 0; pipe < max_pipe; pipe++) {
                for (grp_id = base; grp_id < max_grp; grp_id++) {
                    if (gai->grp_actions[stage][pipe] != NULL) {
                        grp_act = &(gai->grp_actions[stage][pipe][grp_id]);
                        grp_act->global_rsvd = 1;
                    }
                }
            }
        }

        /* Mark op profile rsvd. */
        if (oi->rsvd_num[stage]) {
            max_op_id = oi->op_profile_max[stage];
            base = max_op_id - oi->rsvd_num[stage];
            for (pipe = 0; pipe < max_pipe; pipe++) {
                for (op_id = base; op_id < max_op_id; op_id++) {
                    if (oi->op_profile[stage][pipe] != NULL) {
                        op_profile = &(oi->op_profile[stage][pipe][op_id]);
                        op_profile->global_rsvd = 1;
                    }
                }
            }
        }
    }

    return SHR_E_NONE;
}

/*!
 * \brief Initialize flex counter eviction enable flag.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_eviction_enable_init(int unit)
{
    bool is_sim = SAL_BOOT_BCMSIM;

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_EVICTION) &&
        bcmi_ltsw_property_get(unit, BCMI_CPN_CTR_EVICT_ENABLE, 1)) {
        if (is_sim) {
            flexctr_eviction_enabled[unit] = false;

            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Counter eviction is not supported on "
                                       "BCMSIM!\n")));
        } else {
            flexctr_eviction_enabled[unit] = true;
        }
    } else {
        flexctr_eviction_enabled[unit] = false;
    }

    return SHR_E_NONE;
}

/*!
 * \brief Detach the flex counter module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_gen2_detach(int unit)
{

    SHR_FUNC_ENTER(unit);

    /* Set initialized flag. */
    flexctr_initialized[unit] = FALSE;

    flexctr_eviction_enabled[unit] = false;

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_op_profile_db_cleanup(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_pool_db_cleanup(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_quantization_db_cleanup(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_action_db_cleanup(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_op_profile_entry_cleanup(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_group_action_db_cleanup(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_packet_attribute_db_cleanup(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initializes the flex counter module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_gen2_init(int unit)
{
    int warm = bcmi_warmboot_get(unit), stage;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_gen2_detach(unit));

    /* Initializes flex counter eviction enable flag. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_eviction_enable_init(unit));

    if (!warm) {
        for (stage = FC_MIN_STAGE; stage < FC_MAX_STAGE; stage++) {
            /* Reset all LTs. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_clear(unit, st_info[stage].obj_quant_cfg));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_clear(unit, st_info[stage].range_chk_profile));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_clear(unit, st_info[stage].act_profile));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_clear(unit, st_info[stage].grp_act_profile));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_clear(unit, st_info[stage].operand_profile));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_clear(unit, st_info[stage].stats));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_clear(unit, st_info[stage].trigger));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_clear(unit, st_info[stage].err_stats));

            
            if (ltsw_feature(unit, LTSW_FT_FLEXCTR_L2_HITBIT)) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_clear(unit, st_info[stage].hit_ctrl));
            }

            if (flexctr_eviction_enabled[unit]) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmi_lt_clear(unit, st_info[stage].pool_ctrl));
            }
        }
    }

    /* Initialize pipe mode information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_pipe_mode_init(unit));

    /* Init op profile database. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_op_profile_db_init(unit));

    /* Initialize pool information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_pool_db_init(unit));

    /* Initialize quantization information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_quantization_db_init(unit));

    /* Initialize action information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_action_db_init(unit));

    /* Initialize operation profile. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_op_profile_entry_init(unit));

    /* Initialize group action information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_group_action_db_init(unit));

    /* Initialize packet attribute information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_packet_attribute_db_init(unit));

    /* Initialize misc. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_misc_init(unit));

    /* Initialize software info of reserved resources. */
    SHR_IF_ERR_VERBOSE_EXIT
        (ltsw_flexctr_rsvd_init(unit));

    /* Set initialized flag. */
    flexctr_initialized[unit] = TRUE;

exit:
    if (SHR_FUNC_ERR()) {
        (void)ltsw_flexctr_gen2_detach(unit);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter info get function.
 *
 * This function is used to get a counter action info.
 *
 * \param [in] unit Unit number.
 * \param [in] stat_counter_id Flex counter ID.
 * \param [out] info Flex counter action information.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_counter_info_get(int unit,
                             uint32_t stat_counter_id,
                             bcmi_ltsw_flexctr_counter_info_t *info)
{
    ltsw_flexctr_grp_action_t *grp_act = NULL;
    ltsw_flexctr_action_t *action = NULL;
    ltsw_flexctr_key_t key = {0};
    bcmint_flexctr_source_info_t *si = NULL;
    int type = 0;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    sal_memset(info, 0x0, sizeof(bcmi_ltsw_flexctr_counter_info_t));

    type = FLEXCTR_ID_TYPE(stat_counter_id);

    switch (type) {
        case FLEXCTR_GRP_ACTION:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_counter_id_to_grp_action(unit, stat_counter_id,
                                                       &grp_act, &key));

            info->stage = key.stage;
            info->direction = st_info[key.stage].dir;
            info->action_index = FLEXCTR_ID_INDEX(grp_act->parent_act_id);

            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_key_to_id(unit, &key, &(info->action_profile_id)));

            info->source = grp_act->source;

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexctr_source_info_get(unit, info->source, &si));

            info->table_name = si->table;
            info->group = true;

            break;

        case FLEXCTR_ACTION:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_counter_id_to_action(unit, stat_counter_id,
                                                   &action, &key));

            if (action->group_stat_counter_id) {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit, "Attaching a group action %x first member %x\n"),
                          action->group_stat_counter_id, stat_counter_id));
            }

            info->stage = key.stage;
            info->direction = st_info[key.stage].dir;
            info->action_index = key.id;

            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_key_to_id(unit, &key, &(info->action_profile_id)));

            info->source = action->source;

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_flexctr_source_info_get(unit, info->source, &si));
            info->table_name = si->table;

            info->start_pool = action->start_pool_idx;
            info->end_pool = action->end_pool_idx;

            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_action_get(unit, stat_counter_id, &info->act_cfg));

            if (flexctr_pipe_unique[unit][info->stage]) {
                info->single_pipe = (info->act_cfg.flags &
                                     BCM_FLEXCTR_ACTION_FLAGS_GLOBAL) ? false : true;
                info->pipe = key.pipe;
            } else {
                info->single_pipe = false;
            }

            break;

        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

exit:

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter action id get function.
 *
 * This function is used to get a counter action info.
 *
 * \param [in] unit Unit number.
 * \param [out] info Flex counter action information.
 * \param [out] stat_counter_id Flex counter ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_action_id_get(int unit,
                           bcmi_ltsw_flexctr_counter_info_t *info,
                           uint32_t *stat_counter_id)
{
    ltsw_flexctr_action_info_t *ai = flexctr_action_info[unit];
    ltsw_flexctr_action_t *action = NULL;
    ltsw_flexctr_key_t key = {0};
    uint32_t min_id, max_id;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(stat_counter_id, SHR_E_PARAM);

    key.id = info->action_index;
    key.pipe = info->pipe;
    key.stage = info->stage;

    /* Check input info. */
    if ((key.stage < FC_MIN_STAGE) || (key.stage >= FC_MAX_STAGE)) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id stage %x\n"), key.stage));

        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (key.pipe < 0 || key.pipe >= flexctr_pipe_max[unit][key.stage]) {
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

    if (action->group_stat_counter_id) {
        /* Used by a group action */
        *stat_counter_id = action->group_stat_counter_id;
    } else {
        key.type = FLEXCTR_ACTION;

        /* Compose action id. */
        FLEXCTR_ID_COMPOSE(&key, *stat_counter_id);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter attach action status update function.
 *
 * This function is used to update an attached action status.
 *
 * \param [in] unit Unit number.
 * \param [in] stat_counter_id Flex counter ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_attach_action_status_update(int unit, uint32_t stat_counter_id)
{
    ltsw_flexctr_grp_action_t *grp_act = NULL;
    ltsw_flexctr_action_t *action = NULL;
    int type = 0;

    SHR_FUNC_ENTER(unit);

    FLEXCTR_GRP_ACTION_LOCK(unit);
    FLEXCTR_ACTION_LOCK(unit);

    type = FLEXCTR_ID_TYPE(stat_counter_id);

    switch (type) {
        case FLEXCTR_GRP_ACTION:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_counter_id_to_grp_action(unit, stat_counter_id,
                                                       &grp_act, NULL));
            grp_act->ref_count++;
            break;

        case FLEXCTR_ACTION:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_counter_id_to_action(unit, stat_counter_id,
                                                   &action, NULL));

            action->ref_count++;
            break;
    }

exit:
    FLEXCTR_ACTION_UNLOCK(unit);
    FLEXCTR_GRP_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter detach action status update function.
 *
 * This function is used to update a detached action status.
 *
 * \param [in] unit Unit number.
 * \param [in] stat_counter_id Flex counter ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_detach_action_status_update(int unit, uint32_t stat_counter_id)
{
    ltsw_flexctr_grp_action_t *grp_act = NULL;
    ltsw_flexctr_action_t *action = NULL;
    int type = 0;

    SHR_FUNC_ENTER(unit);

    FLEXCTR_GRP_ACTION_LOCK(unit);
    FLEXCTR_ACTION_LOCK(unit);

    type = FLEXCTR_ID_TYPE(stat_counter_id);

    switch (type) {
        case FLEXCTR_GRP_ACTION:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_counter_id_to_grp_action(unit, stat_counter_id,
                                                       &grp_act, NULL));
            grp_act->ref_count--;
            break;

        case FLEXCTR_ACTION:
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_counter_id_to_action(unit, stat_counter_id,
                                                   &action, NULL));

            action->ref_count--;
            break;
    }

exit:
    FLEXCTR_ACTION_UNLOCK(unit);
    FLEXCTR_GRP_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Update hit bit control table in pipe unique mode.
 *
 * This function is used to update hit control LT for all pipes.
 *
 * \param [in] unit Unit number.
 * \param [in] ctrl Flex counter hit control info.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_hit_control_global_update(int unit,
                                       bcmi_ltsw_flexctr_hit_ctrl_t *ctrl)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    ltsw_flexctr_stage_info_t *st = NULL;
    ltsw_flexctr_grp_action_t *grp_act = NULL;
    ltsw_flexctr_action_t *action = NULL;
    ltsw_flexctr_key_t dkey = {0}, skey = {0};
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t data;
    bool delete = TRUE;
    int pipe, pipe_max;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ctrl, SHR_E_PARAM);
    if (flexctr_pipe_unique[unit][ctrl->stage] != 1) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (ctrl->dst_grp_id != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_counter_id_to_grp_action(unit, ctrl->dst_grp_id,
                                                   &grp_act, &dkey));

        delete = FALSE;
    }


    if (ctrl->src_grp_id != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_counter_id_to_grp_action(unit, ctrl->src_grp_id,
                                                   &grp_act, &skey));

        delete = FALSE;
    }

    if (ctrl->dst_act_profile_id != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_counter_id_to_action(unit, ctrl->dst_act_profile_id,
                                               &action, &dkey));

        delete = FALSE;
    }


    if (ctrl->src_act_profile_id != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_counter_id_to_action(unit, ctrl->src_act_profile_id,
                                               &action, &skey));

        delete = FALSE;
    }

    st = &(st_info[ctrl->stage]);
    pipe_max = flexctr_pipe_max[unit][ctrl->stage];

    for (pipe = 0; pipe < pipe_max; pipe++) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_allocate(dunit, st->hit_ctrl, &eh));

        /* Key fields */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_symbol_add(eh, TABLE_IDs, ctrl->tbl_name));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, PIPEs, pipe));

        /* Data fields */
        if (ctrl->dst_grp_id != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
            dkey.pipe = pipe;
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_key_to_id(unit, &dkey, &data));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, st->hit_dst_grp, data));
        }

        if (ctrl->src_grp_id != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
            skey.pipe = pipe;
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_key_to_id(unit, &skey, &data));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, st->hit_src_grp, data));
        }

        if (ctrl->dst_act_profile_id != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
            dkey.pipe = pipe;
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_key_to_id(unit, &dkey, &data));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, st->hit_dst_act, data));
        }

        if (ctrl->src_act_profile_id != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
            skey.pipe = pipe;
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_key_to_id(unit, &skey, &data));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_field_add(eh, st->hit_src_act, data));
        }

        if (delete) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                                      BCMLT_PRIORITY_NORMAL));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_clear(eh));
        (void)bcmlt_entry_free(eh);
        eh = BCMLT_INVALID_HDL;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter hit control update function.
 *
 * This function is used to update hit control LT.
 *
 * \param [in] unit Unit number.
 * \param [in] ctrl Flex counter hit control info.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_hit_control_update(int unit, bcmi_ltsw_flexctr_hit_ctrl_t *ctrl)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    ltsw_flexctr_stage_info_t *st = NULL;
    ltsw_flexctr_grp_action_t *grp_act = NULL;
    ltsw_flexctr_action_t *action = NULL;
    ltsw_flexctr_key_t dkey = {0}, skey = {0};
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    uint64_t data;
    bool delete = TRUE;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ctrl, SHR_E_PARAM);

    LOG_INFO(BSL_LOG_MODULE,
             (BSL_META_U(unit, "Hit control lt update %u %u %u %u %u\n"),
              ctrl->stage, ctrl->dst_grp_id, ctrl->src_grp_id,
              ctrl->dst_act_profile_id, ctrl->src_act_profile_id));

    FLEXCTR_GRP_ACTION_LOCK(unit);

    if (ctrl->global) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_hit_control_global_update(unit, ctrl));
        SHR_EXIT();
    }

    if (ctrl->dst_grp_id != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_counter_id_to_grp_action(unit, ctrl->dst_grp_id,
                                                   &grp_act, &dkey));

        delete = FALSE;
    }


    if (ctrl->src_grp_id != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_counter_id_to_grp_action(unit, ctrl->src_grp_id,
                                                   &grp_act, &skey));

        delete = FALSE;
    }

    if (ctrl->dst_act_profile_id != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_counter_id_to_action(unit, ctrl->dst_act_profile_id,
                                               &action, &dkey));

        delete = FALSE;
    }

    if (ctrl->src_act_profile_id != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_counter_id_to_action(unit, ctrl->src_act_profile_id,
                                               &action, &skey));

        delete = FALSE;
    }

    st = &(st_info[ctrl->stage]);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, st->hit_ctrl, &eh));

    /* Key fields */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_symbol_add(eh, TABLE_IDs, ctrl->tbl_name));

    /* Data fields */
    if (ctrl->dst_grp_id != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_key_to_id(unit, &dkey, &data));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, st->hit_dst_grp, data));
    }

    if (ctrl->src_grp_id != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_key_to_id(unit, &skey, &data));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, st->hit_src_grp, data));
    }

    if (ctrl->dst_act_profile_id != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_key_to_id(unit, &dkey, &data));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, st->hit_dst_act, data));
    }

    if (ctrl->src_act_profile_id != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_key_to_id(unit, &skey, &data));

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmlt_entry_field_add(eh, st->hit_src_act, data));
    }

    if (delete) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                                  BCMLT_PRIORITY_NORMAL));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_entry_set_commit(unit, eh, BCMLT_PRIORITY_NORMAL));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_clear(eh));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }

    FLEXCTR_GRP_ACTION_UNLOCK(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter stat info get.
 *
 * \param [in] unit Unit Number.
 * \param [in] source Flexctr source.
 * \param [in] pipe Pipe id. -1 indicates global.
 * \param [out] info Flex counter stat info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
ltsw_flexctr_stat_info_get(int unit, bcm_flexctr_source_t source, int pipe,
                           bcmi_ltsw_flexctr_stat_info_t *info)
{
    uint32_t total_entries, free_entries;
    ltsw_flexctr_pool_info_t *pi = flexctr_pool_info[unit];
    ltsw_flexctr_pool_t *pool = NULL;
    bcmint_flexctr_source_info_t *si = NULL;
    int pipe_num, pool_num, pipe_id, start, end, pool_id;
    uint32_t num_sects_pool = 0, num_ctr_section = 0, nctrs_in_pool = 0;
    uint32_t tbl_bit, section_id;
    int stage;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(info, SHR_E_PARAM);
    total_entries = free_entries = 0;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_flexctr_source_info_get(unit, source, &si));
    stage = si->stage;
    tbl_bit = si->tbl_bit;

    if (stage > BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR || stage < 0) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    pipe_num = flexctr_pipe_max[unit][stage];
    if (!((pipe >=0 || pipe < pipe_num) || (pipe != -1))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    pool_num = pi->npools[stage];
    num_sects_pool = pi->nsects_in_pool[stage];
    nctrs_in_pool = pi->nctrs_in_pool[stage];
    num_ctr_section = pi->nctrs_in_sect[stage];

    if (flexctr_pipe_unique[unit][stage]) {
        if (pipe == -1) {
            start = 0;
            end = pipe_num - 1;
        } else {
            start = end = pipe;
        }
    } else {
        start = end = 0;
    }

    for (pipe_id = start; pipe_id <= end; pipe_id++) {
        for (pool_id = 0; pool_id < pool_num; pool_id++) {
            pool = &(pi->pools[stage][pipe_id][pool_id]);

            if (!SHR_BITNULL_RANGE(pool->used_by_tbls, 0,
                                   FLEXCTR_NUM_TBLS_MAX)) {
                if (!SHR_BITGET(pool->used_by_tbls, tbl_bit)) {
                    /* Pool has been consumed by others. */
                    continue;
                }

                for (section_id = 0;
                     section_id < num_sects_pool;
                     section_id++) {
                    if (!SHR_BITGET(pool->inuse_bitmap, section_id)) {
                        /* Sum up empty sections. */
                        free_entries += num_ctr_section;
                    }
                }
                total_entries += nctrs_in_pool;
            } else {
                /* Sum up empty pools. */
                free_entries += nctrs_in_pool;
                total_entries += nctrs_in_pool;
            }
        }
    }

    info->total_entries = total_entries;
    info->free_entries = free_entries;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter software info dump function.
 *
 * This function is used to dump flex counter software info.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
static int
ltsw_flexctr_sw_dump(int unit)
{
    int dunit = bcmi_ltsw_dev_dunit(unit);
    int stage, max_pipe, pipe, pidx, i;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bool pipe_unique;
    uint64_t misconfig, too_many;
    ltsw_flexctr_pool_info_t *pi = flexctr_pool_info[unit];
    ltsw_flexctr_pool_t *pool = NULL;

    SHR_FUNC_ENTER(unit);

    LOG_CLI((BSL_META_U(unit, "Flex counter information:\n\n")));
    LOG_CLI((BSL_META_U(unit, "Counter eviction enable: %s\n"),
            flexctr_eviction_enabled[unit] ? "True" : "False"));

    for (stage = FC_MIN_STAGE; stage < FC_MAX_STAGE; stage++) {
        pipe_unique = flexctr_pipe_unique[unit][stage];
        max_pipe = flexctr_pipe_max[unit][stage];

        LOG_CLI((BSL_META_U(unit, "==%s== Pipe unique: %s\n"),
                st_info[stage].name,
                pipe_unique ? "True" : "False"));

        for (pipe = 0; pipe < max_pipe; pipe++) {
            if (pipe_unique) {
                LOG_CLI((BSL_META_U(unit, "== Pipe %d ==\n"), pipe));
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmlt_entry_allocate(dunit, st_info[stage].err_stats, &eh));

            /* Key fields */
            if (pipe_unique) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmlt_entry_field_add(eh, PIPEs, pipe));
            }

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

                LOG_CLI((BSL_META_U(unit, "\t\tPool: %2d Mode: %d"
                                    " Hint: %d Use_count: %u\n"),
                        pidx, pool->mode, pool->hint,
                        (uint32_t)pool->used_count));

                LOG_CLI((BSL_META_U(unit, "\t\t         Used by: ")));
                SHR_BIT_ITER(pool->used_by_tbls, FLEXCTR_NUM_TBLS_MAX, i) {
                    LOG_CLI((BSL_META_U(unit, "%2d "), i));
                }
                LOG_CLI((BSL_META_U(unit, "\n")));

                LOG_CLI((BSL_META_U(unit, "\t\t         Inuse section: ")));
                SHR_BIT_ITER(pool->inuse_bitmap, FLEXCTR_NUM_SECTION_MAX, i) {
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

/*!
 * \brief Flex counter action is a single pipe action.
 *
 * \param [in] unit Unit Number.
 * \param [in] stage Flex counter stage id.
 * \param [in] action_index Hardware action index.
 *
 * \retval 1 for yes, 0 for no, others indicate error.
 */
int
ltsw_flexctr_action_is_single_pipe(int unit, int stage, uint32_t action_index)
{
    ltsw_flexctr_action_info_t *ai = flexctr_action_info[unit];
    uint32_t min_id, max_id, pipe_max_id;

    if ((stage < FC_MIN_STAGE) || (stage >= FC_MAX_STAGE)) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Invalid id stage %x\n"), stage));

        return SHR_E_PARAM;
    }

    min_id = ai->action_min[stage];
    max_id = ai->action_max[stage];
    pipe_max_id = max_id - ai->rsvd_num[stage];

    if (flexctr_pipe_unique[unit][stage]) {
        /* Pipe unique mode. */
        if ((action_index >= min_id) && (action_index < pipe_max_id)) {
            return true;
        } else if ((action_index >= pipe_max_id) && (action_index < max_id)) {
            return false;
        } else {
            return SHR_E_PARAM;
        }
    } else {
        /* Global mode. */
        if ((action_index >= min_id) && (action_index < max_id)) {
            return false;
        } else {
            return SHR_E_PARAM;
        }
    }
}

/******************************************************************************
 * Public Functions
 */

/*!
 * \brief Initializes the flex counter module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexctr_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_gen2_init(unit));
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief De-initializes the flex counter module.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexctr_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_gen2_detach(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter quantization creation function.
 *
 * This function is used to create a flex counter quantization.
 *
 * \param [in] unit Unit number.
 * \param [in] options BCM_FLEXCTR_OPTIONS_XXX options.
 * \param [in] quantization Flex counter quantization data structure.
 * \param [in/out] quant_id Flex counter quantization ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexctr_quantization_create(int unit,
                                     int options,
                                     bcm_flexctr_quantization_t *quantization,
                                     uint32_t *quant_id)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_quantization_create(unit,
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
 * \brief Flex counter quantization destroy function.
 *
 * This function is used to destroy a flex counter quantization.
 *
 * \param [in] unit Unit number.
 * \param [in] quant_id Flex counter quantization ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexctr_quantization_destroy(int unit, uint32_t quant_id)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }


        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_quantization_destroy(unit, quant_id));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter quantization get function.
 *
 * This function is used to get a flex counter quantization information.
 *
 * \param [in] unit Unit number.
 * \param [in] quant_id Flex counter quantization ID.
 * \param [out] quantization Flex counter quantization data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexctr_quantization_get(int unit,
                                  uint32_t quant_id,
                                  bcm_flexctr_quantization_t *quantization)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_quantization_get(unit, quant_id, quantization));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter quantization traverse function.
 *
 * This function is used to traverse all flex counter quantization
 * information.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn Quantization traverse callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int bcm_ltsw_flexctr_quantization_traverse(
                                int unit,
                                bcm_flexctr_quantization_traverse_cb trav_fn,
                                void *user_data)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_quantization_traverse(unit, trav_fn, user_data));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter action creation function.
 *
 * This function is used to create a flex counter action.
 *
 * \param [in] unit Unit number.
 * \param [in] options BCM_FLEXCTR_OPTIONS_XXX options.
 * \param [in] action Flex counter action data structure.
 * \param [in/out] stat_counter_id Flex counter ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexctr_action_create(int unit,
                               int options,
                               bcm_flexctr_action_t *action,
                               uint32_t *stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_action_create(unit,
                                        options,
                                        action,
                                        stat_counter_id));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter action destroy function.
 *
 * This function is used to destroy a flex counter action.
 *
 * \param [in] unit Unit number.
 * \param [in] stat_counter_id Flex counter ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexctr_action_destroy(int unit,
                                uint32_t stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_action_destroy(unit, stat_counter_id));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter action get function.
 *
 * This function is used to get a flex counter action information.
 *
 * \param [in] unit Unit number.
 * \param [in] stat_counter_id Flex counter ID.
 * \param [out] action Flex counter action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexctr_action_get(int unit,
                            uint32_t stat_counter_id,
                            bcm_flexctr_action_t *action)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_action_get(unit,
                                     stat_counter_id,
                                     action));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter action traverse function.
 *
 * This function is used to traverse all flex counter action information.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn Action traverse callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int bcm_ltsw_flexctr_action_traverse(
                                    int unit,
                                    bcm_flexctr_action_traverse_cb trav_fn,
                                    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_action_traverse(unit, trav_fn, user_data));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter group action creation function.
 *
 * This function is used to create a flex counter group action.
 *
 * \param [in] unit Unit number.
 * \param [in] options BCM_FLEXCTR_OPTIONS_XXX options.
 * \param [in] group_action Flex counter group action data structure.
 * \param [in/out] group_stat_counter_id Flex counter group action ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexctr_group_action_create(int unit,
                                     int options,
                                     bcm_flexctr_group_action_t *group_action,
                                     uint32_t *group_stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_group_action_create(unit,
                                              options,
                                              group_action,
                                              group_stat_counter_id));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter group action destroy function.
 *
 * This function is used to destroy a flex counter group action.
 *
 * \param [in] unit Unit number.
 * \param [in] group_stat_counter_id Flex counter group action ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexctr_group_action_destroy(int unit,
                                      uint32_t group_stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_group_action_destroy(unit, group_stat_counter_id));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter group action get function.
 *
 * This function is used to get a flex counter group action information.
 *
 * \param [in] unit Unit number.
 * \param [in] group_stat_counter_id Flex counter group action ID.
 * \param [out] group_action Flex counter group action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexctr_group_action_get(int unit,
                                  uint32_t group_stat_counter_id,
                                  bcm_flexctr_group_action_t *group_action)
{
    SHR_FUNC_ENTER(unit);

    FLEXCTR_GRP_ACTION_LOCK(unit);
    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_group_action_get(unit,
                                           group_stat_counter_id,
                                           group_action));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    FLEXCTR_GRP_ACTION_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter group action traverse function.
 *
 * This function is used to traverse all flex counter group action information.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn Group action traverse callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int bcm_ltsw_flexctr_group_action_traverse(
                                    int unit,
                                    bcm_flexctr_group_action_traverse_cb trav_fn,
                                    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_group_action_traverse(unit, trav_fn, user_data));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter trigger enable set function.
 *
 * This function is used to enable/disable a trigger from an existing
 * flex counter action.
 *
 * \param [in] unit Unit number.
 * \param [in] stat_counter_id Flex counter ID.
 * \param [in] enable Enable/disable action trigger.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexctr_trigger_enable_set(int unit,
                                    uint32_t stat_counter_id,
                                    int enable)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_trigger_enable_get_set(unit,
                                                 FLEXCTR_OPERATION_SET,
                                                 stat_counter_id,
                                                 &enable));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter trigger enable get function.
 *
 * This function is used to get a trigger enable/disable status from
 * a flex counter action.
 *
 * \param [in] unit Unit number.
 * \param [in] stat_counter_id Flex counter ID.
 * \param [out] enable Enable/disable action trigger.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexctr_trigger_enable_get(int unit,
                                    uint32_t stat_counter_id,
                                    int *enable)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_trigger_enable_get_set(unit,
                                                 FLEXCTR_OPERATION_GET,
                                                 stat_counter_id,
                                                 enable));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter statistic get function.
 *
 * This function is used to get the specific statistic of a given counter action.
 *
 * \param [in] unit Unit number.
 * \param [in] stat_counter_id Flex counter ID.
 * \param [in] num_entries Total number of counter entries need to get.
 * \param [in] counter_indexes Each counter entry index of the given counter action.
 * \param [out] counter_values Statistic for the specific counter entry.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int bcm_ltsw_flexctr_stat_get(int unit,
                              uint32_t stat_counter_id,
                              uint32_t num_entries,
                              uint32_t *counter_indexes,
                              bcm_flexctr_counter_value_t *counter_values)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_stats_get_set(unit,
                                        FLEXCTR_OPERATION_GET,
                                        stat_counter_id,
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
 * \brief Flex counter statistic sync get function.
 *
 * This function is used to get the specific statistic of a given counter action.
 *
 * \param [in] unit Unit number.
 * \param [in] stat_counter_id Flex counter ID.
 * \param [in] num_entries Total number of counter entries need to get.
 * \param [in] counter_indexes Each counter entry index of the given counter action.
 * \param [out] counter_values Statistic for the specific counter entry.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int bcm_ltsw_flexctr_stat_sync_get(int unit,
                                   uint32_t stat_counter_id,
                                   uint32_t num_entries,
                                   uint32_t *counter_indexes,
                                   bcm_flexctr_counter_value_t *counter_values)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_stats_get_set(unit,
                                        FLEXCTR_OPERATION_GET,
                                        stat_counter_id,
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
 * \brief Flex counter statistic set function.
 *
 * This function is used to set the specific statistic of a given counter action.
 *
 * \param [in] unit Unit number.
 * \param [in] stat_counter_id Flex counter ID.
 * \param [in] num_entries Total number of counter entries need to get.
 * \param [in] counter_indexes Each counter entry index of the given counter action.
 * \param [in] counter_values Statistic for the specific counter entry.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int bcm_ltsw_flexctr_stat_set(int unit,
                              uint32_t stat_counter_id,
                              uint32_t num_entries,
                              uint32_t *counter_indexes,
                              bcm_flexctr_counter_value_t *counter_values)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_stats_get_set(unit,
                                        FLEXCTR_OPERATION_SET,
                                        stat_counter_id,
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
 * \brief Flex counter packet attribute profile creation function.
 *
 * This function is used to create a flex counter packet attribute profile
 * for given packet attributes.
 *
 * \param [in] unit Unit number.
 * \param [in] options BCM_FLEXCTR_PACKET_ATTRIBUTE_OPTIONS_XXX options.
 * \param [in] attr_profile Flex counter packet attribute profile data
 *                          structure.
 * \param [in/out] profile_id Created flex counter packet attribute profile ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexctr_packet_attribute_profile_create(
    int unit,
    int options,
    bcm_flexctr_packet_attribute_profile_t *attr_profile,
    int *profile_id)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        if (flexctr_pkt_attr_info[unit]->pkt_attr_initialized) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_packet_attribute_profile_create(unit,
                                                              options,
                                                              attr_profile,
                                                              profile_id));
        } else {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter packet attribute profile destroy function.
 *
 * This function is used to destroy a flex counter packet attribute profile
 * for given flex counter packet attribute profile ID.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Flex counter packet attribute profile ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexctr_packet_attribute_profile_destroy(
    int unit,
    int profile_id)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        if (flexctr_pkt_attr_info[unit]->pkt_attr_initialized) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_packet_attribute_profile_destroy(unit,
                                                               profile_id));
        } else {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter packet attribute profile get function.
 *
 * This function is used to get a flex counter packet attribute profile
 * information for given flex counter packet attributes profile ID.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Flex counter packet attribute profile ID.
 * \param [out] attr_profile Flex counter packet attribute profile data
 *                           structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexctr_packet_attribute_profile_get(
    int unit,
    int profile_id,
    bcm_flexctr_packet_attribute_profile_t *attr_profile)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        if (flexctr_pkt_attr_info[unit]->pkt_attr_initialized) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_packet_attribute_profile_get(unit,
                                                           profile_id,
                                                           attr_profile));
        } else {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter packet attribute profile traverse function.
 *
 * This function is used to traverse all flex counter packet attribute profiles
 * information.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn Packet attribute profile traverse callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexctr_packet_attribute_profile_traverse(
    int unit,
    bcm_flexctr_packet_attribute_profile_traverse_cb trav_fn,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        if (flexctr_pkt_attr_info[unit]->pkt_attr_initialized) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_packet_attribute_profile_traverse(unit,
                                                                trav_fn,
                                                                user_data));
        } else {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter packet attribute map entry add function.
 *
 * This function is used to add an offset/object entry to a
 * packet attribute profile for given packet attributes.
 * Note: the num_entries and related attribute map entry array
 * (in bcm_flexctr_packet_attribute_map_t) should be
 * the same as num_selectors and attr_selectors
 * (in bcm_flexctr_packet_attribute_profile_t) in the flex counter
 * packet attribute profile creation function
 * (bcm_flexctr_packet_attribute_profile_create()) for the same profile_id.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Flex counter packet attribute profile ID.
 * \param [in] attr_map Flex counter packet attribute map data structure.
 * \param [in] offset_value Offset value for this set of packet attribute
 *                          map entries.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexctr_packet_attribute_map_add(
    int unit,
    int profile_id,
    bcm_flexctr_packet_attribute_map_t *attr_map,
    uint32 offset_value)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        if (flexctr_pkt_attr_info[unit]->pkt_attr_initialized) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_packet_attribute_map_add(unit,
                                                       profile_id,
                                                       attr_map,
                                                       offset_value));
        } else {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter packet attribute map entry delete function.
 *
 * This function is used to delete an offset/object entry from a
 * packet attribute profile for given packet attributes.
 * Note: the num_entries and related attribute map entry array
 * (in bcm_flexctr_packet_attribute_map_t) should be
 * the same as num_selectors and attr_selectors
 * (in bcm_flexctr_packet_attribute_profile_t) in the flex counter
 * packet attribute profile creation function
 * (bcm_flexctr_packet_attribute_profile_create()) for the same profile_id.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Flex counter packet attribute profile ID.
 * \param [in] attr_map Flex counter packet attribute map data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexctr_packet_attribute_map_delete(
    int unit,
    int profile_id,
    bcm_flexctr_packet_attribute_map_t *attr_map)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        if (flexctr_pkt_attr_info[unit]->pkt_attr_initialized) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_packet_attribute_map_delete(unit,
                                                          profile_id,
                                                          attr_map));
        } else {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter packet attribute map entry delete all function.
 *
 * This function is used to delete all offset/object entries from a
 * packet attribute profile for given flex counter packet attribute profile ID.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Flex counter packet attribute profile ID.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexctr_packet_attribute_map_delete_all(
    int unit,
    int profile_id)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        if (flexctr_pkt_attr_info[unit]->pkt_attr_initialized) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_packet_attribute_map_delete_all(unit,
                                                              profile_id));
        } else {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter packet attribute map entry get function.
 *
 * This function is used to get an offset/object entry from a
 * packet attribute profile for given packet attributes.
 * Note: the num_entries and related attribute map entry array
 * (in bcm_flexctr_packet_attribute_map_t) should be
 * the same as num_selectors and attr_selectors
 * (in bcm_flexctr_packet_attribute_profile_t) in the flex counter
 * packet attribute profile creation function
 * (bcm_flexctr_packet_attribute_profile_create()) for the same profile_id.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Flex counter packet attribute profile ID.
 * \param [in] attr_map Flex counter packet attribute map data structure.
 * \param [out] offset_value Offset value for this set of packet attribute
 *                           map entries.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexctr_packet_attribute_map_get(
    int unit,
    int profile_id,
    bcm_flexctr_packet_attribute_map_t *attr_map,
    uint32 *offset_value)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        if (flexctr_pkt_attr_info[unit]->pkt_attr_initialized) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_packet_attribute_map_get(unit,
                                                       profile_id,
                                                       attr_map,
                                                       offset_value));
        } else {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Flex counter packet attribute map entry traverse function.
 *
 * This function is used to traverse all flex counter packet attribute
 * map entries information for given flex counter packet attribute profile ID.
 *
 * \param [in] unit Unit number.
 * \param [in] profile_id Flex counter packet attribute profile ID.
 * \param [in] trav_fn Packet attribute map entry traverse callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_flexctr_packet_attribute_map_traverse(
    int unit,
    int profile_id,
    bcm_flexctr_packet_attribute_map_traverse_cb trav_fn,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        if (flexctr_pkt_attr_info[unit]->pkt_attr_initialized) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_packet_attribute_map_traverse(unit,
                                                            profile_id,
                                                            trav_fn,
                                                            user_data));
        } else {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_flexctr_object_overlay_multi_get(int unit,
    bcm_flexctr_object_t object,
    int array_size,
    bcm_flexctr_object_t *overlay_object_array,
    int *count)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_flexctr_object_overlay_multi_get(unit,
                                                        object,
                                                        array_size,
                                                        overlay_object_array,
                                                        count));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_flexctr_packet_attribute_id_info_get(
    int unit,
    int profile_id,
    bcmi_ltsw_flexctr_packet_attribute_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        if (flexctr_pkt_attr_info[unit]->pkt_attr_initialized) {
            SHR_IF_ERR_VERBOSE_EXIT
                (ltsw_flexctr_packet_attribute_info_get(unit, profile_id,
                                                        info));
        } else {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_flexctr_counter_id_info_get(int unit,
                                      uint32_t stat_counter_id,
                                      bcmi_ltsw_flexctr_counter_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_counter_info_get(unit, stat_counter_id, info));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_flexctr_counter_id_get(int unit,
                                 bcmi_ltsw_flexctr_counter_info_t *info,
                                 uint32_t *stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_action_id_get(unit, info, stat_counter_id));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_flexctr_attach_counter_id_status_update(int unit,
                                                  uint32_t stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_attach_action_status_update(unit, stat_counter_id));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_flexctr_detach_counter_id_status_update(int unit,
                                                  uint32_t stat_counter_id)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_detach_action_status_update(unit, stat_counter_id));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_flexctr_hit_control_update(int unit,
                                     bcmi_ltsw_flexctr_hit_ctrl_t *ctrl)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        if (!ltsw_feature(unit, LTSW_FT_FLEXCTR_L2_HITBIT)) {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_hit_control_update(unit, ctrl));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_flexctr_action_update(int unit,
                                uint32_t stat_counter_id,
                                bcm_flexctr_action_t *act_cfg)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_action_update(unit, stat_counter_id, act_cfg));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_flexctr_field_group_update(int unit, bcm_flexctr_source_t source,
                                     int group_id, int old_pri, int new_pri)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_field_group_update(unit, source, group_id,
                                             old_pri, new_pri));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_flexctr_mode_get(int unit, bool egr, bcmi_ltsw_flexctr_mode_t *mode)
{
    int stage;

    stage = egr ? BCMI_LTSW_FLEXCTR_STAGE_EGR_CTR :
                  BCMI_LTSW_FLEXCTR_STAGE_ING_CTR;

    *mode = flexctr_pipe_unique[unit][stage] ?
             bcmiFlexctrModePipeUnique : bcmiFlexctrModeGlobal;
    return SHR_E_NONE;
}

int
bcmi_ltsw_flexctr_stat_info_get(int unit, bcm_flexctr_source_t source, int pipe,
                                bcmi_ltsw_flexctr_stat_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_stat_info_get(unit, source, pipe, info));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_flexctr_sw_dump(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_sw_dump(unit));

    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \breif Flex counter group action get function.
 *
 * This function is used to get a flex counter group action information.
 *
 * \param [in] unit Unit number.
 * \param [in] group_stat_counter_id Flex counter group ID.
 * \param [out] group_action Flex counter group action data structure.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int bcmi_ltsw_flexctr_group_action_get(
    int unit,
    uint32_t group_stat_counter_id,
    bcm_flexctr_group_action_t *group_action)
{
    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            SHR_ERR_EXIT(SHR_E_INIT);
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (ltsw_flexctr_group_action_get(unit,
                                           group_stat_counter_id,
                                           group_action));
    } else {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_flexctr_action_is_single_pipe(int unit, int stage,
                                        uint32_t action_index)
{
    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            return SHR_E_INIT;
        }

        return ltsw_flexctr_action_is_single_pipe(unit, stage, action_index);
    } else {
        return SHR_E_UNAVAIL;
    }
}

int
bcmi_ltsw_flexctr_field_opaque_object_value_get(int unit,
                                                uint32_t stat_counter_id,
                                                uint32_t counter_index,
                                                uint32_t size,
                                                uint32_t *obj_val)
{
    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            return SHR_E_INIT;
        }

        return mbcm_ltsw_flexctr_field_opaque_object_value_get(unit,
                                                               stat_counter_id,
                                                               counter_index,
                                                               size,
                                                               obj_val);
    } else {
        return SHR_E_UNAVAIL;
    }
}

int
bcmi_ltsw_flexctr_egr_presel_set(int unit, int val)
{
    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            return SHR_E_INIT;
        }

        return mbcm_ltsw_flexctr_egr_presel_set(unit, val);
    } else {
        return SHR_E_UNAVAIL;
    }
}

int
bcmi_ltsw_flexctr_egr_presel_get(int unit, int *val)
{
    if (ltsw_feature(unit, LTSW_FT_FLEXCTR_GEN2)) {

        if (flexctr_initialized[unit] == FALSE) {
            return SHR_E_INIT;
        }

        return mbcm_ltsw_flexctr_egr_presel_get(unit, val);
    } else {
        return SHR_E_UNAVAIL;
    }
}
