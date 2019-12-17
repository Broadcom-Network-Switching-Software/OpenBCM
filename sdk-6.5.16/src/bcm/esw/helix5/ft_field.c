/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        ft_field.c
 * Purpose:     Flowtracker module specific routines in flowtracker
 *              stage in FP.
 */

#include <soc/defs.h>

#if defined (BCM_FLOWTRACKER_SUPPORT) && defined (BCM_FIELD_SUPPORT)

#include <bcm/error.h>
#include <bcm/field.h>
#include <shared/bsl.h>
#include <bcm/flowtracker.h>

#include <bcm_int/esw/field.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/helix5.h>
#include <bcm_int/esw/keygen_api.h>
#include <bcm_int/esw/flowtracker/ft_group.h>
#include <bcm_int/esw/flowtracker/ft_alu.h>
#include <bcm_int/esw/flowtracker/ft_field.h>


/* Internal MACRO to goto cleanup. */
#define BCMI_IF_ERROR_CLEANUP(_rv_)                                                 \
        do {                                                                        \
            if (_rv_ != BCM_E_NONE) {                                               \
                goto cleanup;                                                       \
            }                                                                       \
        } while(0)

/* Macro to add group tracking elements map to database. */
#define _FP_FT_TRACKING_PARAM_QUAL_MAP_DECL                                         \
            _field_ft_tracking_param_qual_map_t _pq_map_

#define _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(_unit_, _map_db_,                        \
                    _param_, _qual_, _width_, _flags_)                              \
        do {                                                                        \
            _field_ft_tracking_param_qual_map_init((_unit_), &_pq_map_);            \
            _pq_map_.param = (_param_);                                             \
            _pq_map_.qual = (_qual_);                                               \
            _pq_map_.width = (_width_);                                             \
            _pq_map_.flags = (_flags_);                                             \
            rv = _field_ft_tracking_param_qual_map_insert((_unit_),                 \
                        (_param_), &(_pq_map_), (_map_db_));                        \
            BCMI_IF_ERROR_CLEANUP(rv);                                              \
        } while(0)

/* Macro to add alu data tracking information to map database. */
#define _FP_FT_TRACKING_ALU_DATA_MAP_ADD(_unit_, _map_db_,                          \
                    _param_, _qual_, _start_, _width_, _flags_)                     \
        do {                                                                        \
            _field_ft_tracking_param_qual_map_init((_unit_), &_pq_map_);            \
            _pq_map_.param = (_param_);                                             \
            _pq_map_.qual = (_qual_);                                               \
            _pq_map_.start_loc = (_start_);                                         \
            _pq_map_.width = (_width_);                                             \
            _pq_map_.flags = (_flags_);                                             \
            rv = _field_ft_tracking_param_qual_map_insert((_unit_),                 \
                        (_param_), &(_pq_map_), (_map_db_));                        \
            BCMI_IF_ERROR_CLEANUP(rv);                                              \
        } while(0)

/* Tracking param is not mapped to qualifier. */
#define _FP_FT_TRACKING_PARAM_F_NO_QUAL_MAPPED  (1 << 0)

/* Tracking param requires ALU 16-bit. */
#define _FP_FT_TRACKING_PARAM_F_ALU16_REQ       (1 << 1)

/* Tracking param required ALU 32-bit. */
#define _FP_FT_TRACKING_PARAM_F_ALU32_REQ       (1 << 2)

/* Tracking Param is mapped for ALU Data */
#define _FP_FT_TRACKING_PARAM_F_ALU_DATA        (1 << 3)

/* Tracking Param requires modulo 32 for Key */
#define _FP_FT_TRACKING_PARAM_F_KEY_MODULO_32   (1 << 4)

/* Tracking Param requires modulo 32 for Data */
#define _FP_FT_TRACKING_PARAM_F_DATA_MODULO_32  (1 << 5)

/* Tracking Param does not generate Session Key/Data */
#define _FP_FT_TRACKING_PARAM_F_NO_FILL         (1 << 6)

/* Tracking param required ALU. */
#define _FP_FT_TRACKING_PARAM_F_ALU_REQ                                             \
                (_FP_FT_TRACKING_PARAM_F_ALU16_REQ |                                \
                _FP_FT_TRACKING_PARAM_F_ALU32_REQ)

/* Tracking info type is param info. */
#define _FP_FT_TRACKING_INFO_TYPE_PARAM         (1 << 0)

/* Tracking info type is flowcheck info. */
#define _FP_FT_TRACKING_INFO_TYPE_FLOWCHECK     (1 << 1)

/* Tracking info type is Alu data info */
#define _FP_FT_TRACKING_INFO_TYPE_ALU_DATA      (1 << 2)

/* Start Index of ALU Data in param_qual_map db */
#define _FP_FT_TRACKING_ALU_DATA_PARAM_MAP_START                                    \
                (_FIELD_FT_TRACKING_ELEMENT_TYPE_MAX)
/*
 * Typedef:
 *      _field_ft_tracking_alu_data_info_t
 *
 * Purpose:
 *       ALU Data related tracking information.
 */
typedef struct _field_ft_tracking_alu_data_info_s {
    uint16 param;

    int offset;

    int width;
} _field_ft_tracking_alu_data_info_t;

/* FT Group Keygen Extraction process States */
#define _FIELD_FT_GROUP_KEYGEN_START                    0
#define _FIELD_FT_GROUP_KEYGEN_CFG_ALLOC                1
#define _FIELD_FT_GROUP_KEYGEN_CFG_ALU_QUAL_ADD         2
#define _FIELD_FT_GROUP_KEYGEN_CFG_FC_QUAL_ADD          3
#define _FIELD_FT_GROUP_KEYGEN_CFG_DATA_KEY_QUAL_ADD    4
#define _FIELD_FT_GROUP_KEYGEN_CFG_DATA_QUAL_ADJUST     5
#define _FIELD_FT_GROUP_KEYGEN_CFG_KEY_QUAL_ADJUST      6
#define _FIELD_FT_GROUP_KEYGEN_DATA_OPER_EXTRACT        7
#define _FIELD_FT_GROUP_KEYGEN_KEY_OPER_EXTRACT         8
#define _FIELD_FT_GROUP_KEYGEN_DATA_OPER_PROCESS        9
#define _FIELD_FT_GROUP_KEYGEN_KEY_OPER_PROCESS        10
#define _FIELD_FT_GROUP_KEYGEN_OPER_SAVE               11
#define _FIELD_FT_GROUP_KEYGEN_END                     12

/*
 * Typedef:
 *      _field_ft_group_keydata_keygen_md_t
 *
 * Purpose:
 *       Session key/data extraction related information.
 */
typedef struct _field_ft_group_keydata_keygen_md_s {
    /* List of allowed keygen mode for extraction. */
    bcmi_keygen_mode_t ext_modes[2];

    /* Number of valid indices in ext_modes. */
    int ext_modes_count;

    /* Keygen_cfg->qual_info_arr. */
    bcmi_keygen_qual_info_t *qual_info_arr;

    /* Total memory allocated to qual_info_arr. */
    uint16 max_qual_info_alloc;

    /* Actual number of qual info in qual_info_arr. */
    uint16 qual_info_count;

    /* Qset of qualifiers in qual_info_arr. */
    bcm_field_qset_t qset;

    /* Keygen_oper information after extraction. */
    bcmi_keygen_oper_t keygen_oper;

    /* Actual extraction Keygen mode. */
    bcmi_keygen_mode_t keygen_mode;

    /* Number of keygen parts. */
    uint16 num_parts;

} _field_ft_group_keydata_keygen_md_t;

/*
 * Typedef:
 *      _field_ft_group_keygen_oper_info_t
 *
 * Purpose:
 *       Session key/data extraction information
 *       in the form required by FT module.
 */
typedef struct _field_ft_group_keygen_oper_info_s {
    /* keygen mode of session key. */
    int mode;

    /* Number of Session key info. */
    int num_skey_info;

    /* Session key info. */
    bcmi_ft_group_alu_info_t *skey_info;

    /* Number of elements in skey_ext_info. */
    int skey_ext_count;

    /* Encoded session Key ext codes. */
    bcmi_ft_hw_extractor_info_t *skey_ext_info;

    /* Number of Session data info. */
    int num_sdata_info;

    /* Session data info. */
    bcmi_ft_group_alu_info_t *sdata_info;

    /* Number of elements in sdata_ext_info. */
    int sdata_ext_count;

    /* Encoded Session data ext codes.  */
    bcmi_ft_hw_extractor_info_t *sdata_ext_info;

} _field_ft_group_keygen_oper_info_t;

/*
 * Typedef:
 *      _field_ft_group_keygen_md_t
 *
 * Purpose:
 *       metadata for FT group keygen extraction for
 *       session key and session data.
 */
typedef struct _field_ft_group_keygen_md_s {
    /* Current processing state in _FIELD_FT_GROUP_KEYGEN_XXX. */
    uint16 state;

    /* Previous processing state in _FIELD_FT_GROUP_KEYGEN_XXX. */
    uint16 prev_state;

    /* Return code for current processing state. */
    int state_rv;

    /* FT information in field stage struct. */
    _field_ft_info_t *ft_info;

    /* FT Group Identifier. */
    bcm_flowtracker_group_t ft_group_id;

    /* Number of elements in tracking_params_info. */
    int num_tracking_params;

    /* List of tracking params info for FT Group. */
    bcm_flowtracker_tracking_param_info_t *tracking_params_info;

    /* Number of elements in flowtracker_checks. */
    int num_flowchecks;

    /* List of flowcheck Ids for FT Group/ */
    bcm_flowtracker_check_t *flowtracker_checks;

    /* Number of ALU data elements. */
    int num_alu_data;

    /* Pointer to ALU Data info. */
    _field_ft_tracking_alu_data_info_t *alu_data_info;

    /* Metadata for session key keygen. */
    _field_ft_group_keydata_keygen_md_t skey_keygen_md;

    /* Metadata for session data keygen. */
    _field_ft_group_keydata_keygen_md_t sdata_keygen_md;

    /* Operational info for key/data extraction. */
    _field_ft_group_keygen_oper_info_t keydata_oper_info;

    /* Set if output is required to saved to FT Group DB. */
    uint8 save_op;

} _field_ft_group_keygen_md_t;

/*
 * Typedef:
 *      _bcm_field_ft_entry_traverse_cb
 * Purpose:
 *      Callback function pointer per entry in flowtracke stage.
 */
typedef int (*_bcm_field_ft_entry_traverse_cb)(int unit,
    bcm_field_entry_t entry, bcm_flowtracker_group_t id,
    void *user_data1, void *user_data2);

/*
 * Function:
 *    _field_ft_tracking_param_qual_map_init
 *
 * Purpose:
 *    Initializes group tracking element map
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    element_map- (IN) group tracking element map
 *
 * Returns:
 */
STATIC void
_field_ft_tracking_param_qual_map_init(
        int unit,
        _field_ft_tracking_param_qual_map_t *pq_map)
{
    if (pq_map == NULL) {
        return;
    }

    sal_memset(pq_map, 0,
            sizeof(_field_ft_tracking_param_qual_map_t));

    return;
}

/*
 * Function:
 *    _field_ft_tracking_param_qual_map_insert
 *
 * Purpose:
 *    Insert tracking param-qual map to map database.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    param      - (IN) tracking param
 *    pq_map     - (IN) tracking param-qual map
 *    map_db     - (IN/OUT) param and qual Map database
 *
 * Returns:
 *    BCM_E_NONE
 */
STATIC int
_field_ft_tracking_param_qual_map_insert(
        int unit,
        uint16 param,
        _field_ft_tracking_param_qual_map_t *pq_map,
        _field_ft_tracking_param_qual_map_db_t *map_db)
{
    _field_ft_tracking_param_qual_map_t *temp_map = NULL;

    /* Validate input params. */
    if ((pq_map == NULL) || (map_db == NULL) ||
            (param >= _FIELD_FT_TRACKING_PARAM_MAX)) {
        return BCM_E_PARAM;
    }

    temp_map = map_db->param_qual_map[param];

    if (temp_map == NULL) {
        _FP_XGS3_ALLOC(temp_map,
                sizeof(_field_ft_tracking_param_qual_map_t),
                "tracking param-qual map");
    } else {
        return BCM_E_INTERNAL;
    }

    if (temp_map == NULL) {
        return BCM_E_MEMORY;
    }

    map_db->param_qual_map[param] = temp_map;
    sal_memcpy(temp_map, pq_map,
            sizeof(_field_ft_tracking_param_qual_map_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_ft_tracking_param_qual_map_db_cleanup
 *
 * Purpose:
 *   Cleanup tracking param and qualifier map database
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    ft_info    - (IN) Flowtracker specific info
 *
 * Returns:
 *    None.
 */
void
_field_ft_tracking_param_qual_map_db_cleanup(
        int unit,
        _field_ft_info_t *ft_info)
{
    uint16 param = 0;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;

    if ((ft_info == NULL) ||
        (ft_info->param_qual_map_db == NULL)) {
        return;
    }

    map_db = ft_info->param_qual_map_db;

    for (param = 0; param < _FIELD_FT_TRACKING_PARAM_MAX; param++) {
        if (map_db->param_qual_map[param]) {
            sal_free(map_db->param_qual_map[param]);
            map_db->param_qual_map[param] = NULL;
        }
    }

    sal_free(map_db);
    ft_info->param_qual_map_db = NULL;
}

/*
 * Function:
 *    _field_ft_tracking_param_qual_map_db_init
 *
 * Purpose:
 *   Initialize tracking params and qualifier map database
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    ft_info    - (IN/OUT) Flowtracker info structure
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_ft_tracking_param_qual_map_db_init(
        int unit,
        _field_ft_info_t *ft_info)
{
    int rv = BCM_E_NONE;
    uint16 shift = 0;
    uint32 flags = 0;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;
    _FP_FT_TRACKING_PARAM_QUAL_MAP_DECL;

    if (ft_info == NULL) {
        return BCM_E_PARAM;
    }

    _FP_XGS3_ALLOC(map_db,
            sizeof(_field_ft_tracking_param_qual_map_db_t),
            "tracking param qual map db");
    if (map_db == NULL) {
        return BCM_E_MEMORY;
    }

    ft_info->param_qual_map_db = map_db;

    /* Tracking param qualifier map. */
    flags = 0;
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeSrcIPv4,
            bcmFieldQualifySrcIp, 32, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeDstIPv4,
            bcmFieldQualifyDstIp, 32, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeSrcIPv6,
            bcmFieldQualifySrcIp6, 128, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeDstIPv6,
            bcmFieldQualifyDstIp6, 128, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeL4SrcPort,
            bcmFieldQualifyL4SrcPort, 16, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeL4DstPort,
            bcmFieldQualifyL4DstPort, 16, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeIPProtocol,
            bcmFieldQualifyIpProtocol, 8, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeVRF,
            bcmFieldQualifyVrf, 8, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeTTL,
            bcmFieldQualifyTtl, 8, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeIPLength,
            bcmFieldQualifyIp4Length, 16, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeTcpWindowSize,
            bcmFieldQualifyTcpWindowSize, 16, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeVxlanNetworkId,
            bcmFieldQualifyVxlanNetworkId, 24,
            flags | _FP_FT_TRACKING_PARAM_F_DATA_MODULO_32);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeNextHeader,
            bcmFieldQualifyIp6PktNextHeader, 8, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeHopLimit,
            bcmFieldQualifyIp6PktHopLimit, 8, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeInnerSrcIPv4,
            bcmFieldQualifyInnerSrcIp, 32, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeInnerDstIPv4,
            bcmFieldQualifyInnerDstIp, 32, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeInnerSrcIPv6,
            bcmFieldQualifyInnerSrcIp6, 128, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeInnerDstIPv6,
            bcmFieldQualifyInnerDstIp6, 128, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeInnerIPProtocol,
            bcmFieldQualifyInnerIpProtocol, 8, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeInnerTTL,
            bcmFieldQualifyInnerTtl, 8, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeInnerNextHeader,
            bcmFieldQualifyInnerIp6PktNextHeader, 8, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeInnerHopLimit,
            bcmFieldQualifyInnerIp6PktHopLimit, 8, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeInnerL4SrcPort,
            bcmFieldQualifyInnerL4SrcPort, 16, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeInnerL4DstPort,
            bcmFieldQualifyInnerL4DstPort, 16, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeTcpFlags,
            bcmFieldQualifyTcpControl, 8, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeOuterVlanTag,
            bcmFieldQualifyOuterVlan, 16, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeIP6Length,
            bcmFieldQualifyIp6Length, 16, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeInnerIPLength,
            bcmFieldQualifyTunnelPayloadIp4Length, 16, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeInnerIP6Length,
            bcmFieldQualifyTunnelPayloadIp6Length, 16, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeTunnelClass,
            bcmFieldQualifyTunnelId, 24,
            flags | _FP_FT_TRACKING_PARAM_F_DATA_MODULO_32);

    /* Tracking param with no qualifier, used for counting */
    flags = _FP_FT_TRACKING_PARAM_F_ALU32_REQ;

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeDosAttack,
            bcmFieldQualifyDosAttackEvents, 32,
            flags | _FP_FT_TRACKING_PARAM_F_DATA_MODULO_32);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeInnerDosAttack,
            bcmFieldQualifyInnerDosAttackEvents, 32,
            flags | _FP_FT_TRACKING_PARAM_F_DATA_MODULO_32);

    flags |= _FP_FT_TRACKING_PARAM_F_NO_QUAL_MAPPED;

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypePktCount,
            0, 32, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeByteCount,
            0, 32, flags);

    /* Timestamp tracking parameters are not sent to FT group. */
    flags = _FP_FT_TRACKING_PARAM_F_NO_FILL;
    flags |= _FP_FT_TRACKING_PARAM_F_NO_QUAL_MAPPED;

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeTimestampNewLearn,
            0, 0, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeTimestampFlowStart,
            0, 0, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeTimestampFlowEnd,
            0, 0, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeTimestampCheckEvent1,
            0, 0, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeTimestampCheckEvent2,
            0, 0, flags);

    /* Map Tracking Params to ignore processing. */
    flags = _FP_FT_TRACKING_PARAM_F_NO_FILL;
    flags |= _FP_FT_TRACKING_PARAM_F_NO_QUAL_MAPPED;
    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeNone,
            0, 0, flags);

    _FP_FT_TRACKING_PARAM_QUAL_MAP_ADD(unit,
            map_db, bcmFlowtrackerTrackingParamTypeFlowtrackerCheck,
            0, 0, flags);

    /* Tracking ALU Data */
    shift = _FP_FT_TRACKING_ALU_DATA_PARAM_MAP_START;
    flags = _FP_FT_TRACKING_PARAM_F_ALU_DATA;
    _FP_FT_TRACKING_ALU_DATA_MAP_ADD(unit,
            map_db, _bcmFieldFtAluDataTcpFlagsFIN + shift,
            bcmFieldQualifyTcpControl, 0, 1, flags);

    _FP_FT_TRACKING_ALU_DATA_MAP_ADD(unit,
            map_db, _bcmFieldFtAluDataTcpFlagsSYN + shift,
            bcmFieldQualifyTcpControl, 1, 1, flags);

    _FP_FT_TRACKING_ALU_DATA_MAP_ADD(unit,
            map_db, _bcmFieldFtAluDataTcpFlagsRST + shift,
            bcmFieldQualifyTcpControl, 2, 1, flags);

    _FP_FT_TRACKING_ALU_DATA_MAP_ADD(unit,
            map_db, _bcmFieldFtAluDataTcpFlagsPSH + shift,
            bcmFieldQualifyTcpControl, 3, 1, flags);

    _FP_FT_TRACKING_ALU_DATA_MAP_ADD(unit,
            map_db, _bcmFieldFtAluDataTcpFlagsACK + shift,
            bcmFieldQualifyTcpControl, 4, 1, flags);

    _FP_FT_TRACKING_ALU_DATA_MAP_ADD(unit,
            map_db, _bcmFieldFtAluDataTcpFlagsURG + shift,
            bcmFieldQualifyTcpControl, 5, 1, flags);

    _FP_FT_TRACKING_ALU_DATA_MAP_ADD(unit,
            map_db, _bcmFieldFtAluDataTcpFlagsECE + shift,
            bcmFieldQualifyTcpControl, 6, 1, flags);

    _FP_FT_TRACKING_ALU_DATA_MAP_ADD(unit,
            map_db, _bcmFieldFtAluDataTcpFlagsCWR + shift,
            bcmFieldQualifyTcpControl, 7, 1, flags);

cleanup:

    if (rv != BCM_E_NONE) {
        (void) _field_ft_tracking_param_qual_map_db_cleanup(unit, ft_info);
    }

    return rv;
}

/* Alu Data info */
static _field_ft_tracking_alu_data_info_t
_field_ft_tracking_alu_data_info[] = {
    {_bcmFieldFtAluDataTcpFlagsFIN, 2, 1},
    {_bcmFieldFtAluDataTcpFlagsSYN, 0, 1},
    {_bcmFieldFtAluDataTcpFlagsRST, 3, 1},
    {_bcmFieldFtAluDataTcpFlagsPSH, 4, 1},
    {_bcmFieldFtAluDataTcpFlagsACK, 1, 1},
    {_bcmFieldFtAluDataTcpFlagsURG, 5, 1},
    {_bcmFieldFtAluDataTcpFlagsECE, 6, 1},
    {_bcmFieldFtAluDataTcpFlagsCWR, 7, 1}
};

/*
 * Function:
 *    _field_hx5_ftg_alu_data_info_get
 *
 * Purpose:
 *    Get pointer to ALU Data info
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    id         - (IN) flowtracker group id.
 *    info       - (OUT) Pointer to ALU Data Info.
 *    num_info   - (OUT) number of ALU Data info.
 *
 * Returns:
 *    BCM_E_NONE
 */
STATIC int
_field_hx5_ftg_alu_data_info_get(
        int unit,
        bcm_flowtracker_group_t id,
        _field_ft_tracking_alu_data_info_t **info,
        int *num_info)
{
    /*
     * ALU Data info is fixed to extract TCP Flags.
     * If ALU data needs to be extracted from other source
     * flowtracker group need to convey it.
     * This function set the pointer to fixed ALU Data info
     * and no memory allocated is required for now.
     */
    if (num_info != NULL) {
        *num_info = COUNTOF(_field_ft_tracking_alu_data_info);
    }

    if (info != NULL) {
        *info = &_field_ft_tracking_alu_data_info[0];
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_hx5_ftg_keygen_cfg_qual_alloc
 *
 * Purpose:
 *   Allocate memory for qual_info_arr for session key and
 *   session data.
 *
 * Parameters:
 *    unit             - (IN) BCM device number.
 *    ftg_keygen_md    - (IN) Ft group Keygen metadata
 *
 * Returns:
 *    None.
 */

STATIC int
_field_hx5_ftg_keygen_cfg_qual_alloc(
        int unit,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int idx = 0, shift = 0;
    int num_key_params = 0;
    int num_data_params = 0;
    int num_tracking_params = 0;
    int num_flowchecks = 0;
    int total_data_quals = 0;
    int num_alu_data = 0;
    bcm_field_qualify_t qual;
    bcm_field_qset_t qset;
    bcm_flowtracker_tracking_param_info_t *tracking_param_info = NULL;
    _field_ft_tracking_alu_data_info_t *alu_data = NULL;
    _field_ft_tracking_param_qual_map_t *pq_map = NULL;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;
    bcmi_keygen_qual_info_t *qual_info_arr = NULL;

    if (ftg_keygen_md == NULL) {
        return (BCM_E_PARAM);
    }

    map_db = ftg_keygen_md->ft_info->param_qual_map_db;
    num_tracking_params = ftg_keygen_md->num_tracking_params;
    num_flowchecks = ftg_keygen_md->num_flowchecks;
    num_alu_data = ftg_keygen_md->num_alu_data;

    /* Calculate number of keys and data */
    for (idx = 0; idx < num_tracking_params; idx++) {
        tracking_param_info = &ftg_keygen_md->tracking_params_info[idx];
        if (tracking_param_info->flags &
                BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY) {
            num_key_params++;
        } else {
            num_data_params++;
        }
    }

    /* Validate number of key and data */
    if ((num_key_params == 0) || (num_data_params == 0)) {
        LOG_DEBUG(BSL_LS_BCM_FP,
                (BSL_META_U(unit, "FT Group=%d - Tracking Key and/or"
                            " Data not specified\n\r"),
                 ftg_keygen_md->ft_group_id));
        return BCM_E_CONFIG;
    }

    /* Calculate number of session data qualifiers */
    total_data_quals = 0;

    /* Qualifiers for ALU Data */
    BCM_FIELD_QSET_INIT(qset);
    shift = _FP_FT_TRACKING_ALU_DATA_PARAM_MAP_START;
    for (idx = 0; idx < num_alu_data; idx++) {
        alu_data = &ftg_keygen_md->alu_data_info[idx];
        pq_map = map_db->param_qual_map[alu_data->param + shift];
        qual = pq_map->qual;

        if (!(BCM_FIELD_QSET_TEST(qset, qual))) {
            total_data_quals++;
            BCM_FIELD_QSET_ADD(qset, qual);
        }
    }

    /*
     * Qualifiers in Flowtracker checks
     * Primary flowcheck can have action params also.
     * So, need to allocate memory for them too.
     */
    total_data_quals += (2 * num_flowchecks);

    /* Qualifiers in Tracking params fo session data */
    total_data_quals += num_data_params;

    /* Allocate qual_info_arr for session Key */
    _FP_XGS3_ALLOC(qual_info_arr,
            num_key_params * sizeof(bcmi_keygen_qual_info_t),
            "skey qual info arr");
    if (qual_info_arr == NULL) {
        rv = BCM_E_MEMORY;
    } else {
        ftg_keygen_md->skey_keygen_md.qual_info_arr = qual_info_arr;
        ftg_keygen_md->skey_keygen_md.max_qual_info_alloc = num_key_params;
    }

    /* Allocate qual_info_arr for session Data */
    if (BCM_SUCCESS(rv)) {
        qual_info_arr = NULL;
        _FP_XGS3_ALLOC(qual_info_arr,
                total_data_quals * sizeof(bcmi_keygen_qual_info_t),
                "sdata qual info arr");
        if (qual_info_arr == NULL) {
            rv = BCM_E_MEMORY;
        } else {
            ftg_keygen_md->sdata_keygen_md.qual_info_arr = qual_info_arr;
            ftg_keygen_md->sdata_keygen_md.max_qual_info_alloc = total_data_quals;
        }
    }

    return rv;
}

/*
 * Function:
 *    _field_hx5_ftg_qual_ext_hw_info_get
 *
 * Purpose:
 *    Convert keygen_oper to qual_ext_offset_info and ext_codes.
 *
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    key_or_data       - (IN) Conversion of session key or data.
 *    ftg_keygen_md     - (IN) FT Group Keygen metadata.
 *    qual_ext_offset_info - (OUT) Pointer to qual-extractor info.
 *    ext_codes         - (OUT) Pointer to extraction codes.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_qual_ext_hw_info_get(
        int unit,
        int key_or_data,
        _field_ft_group_keygen_md_t *ftg_keygen_md,
        _field_ft_qual_ext_offset_info_t **qual_ext_offset_info,
        _field_ft_keygen_ext_codes_t *ext_codes)
{
    int rv = BCM_E_NONE;
    int keygen_profile = 0;
    int num_keygen_parts = 0;
    uint16 ext_ctrl_sel_info_count = 0;
    bcmi_keygen_oper_t *keygen_oper = NULL;
    bcmi_keygen_ext_ctrl_sel_info_t *ext_ctrl_sel_info = NULL;

    if ((ftg_keygen_md == NULL) ||
        (ext_codes == NULL) ||
        (qual_ext_offset_info == NULL)) {
        return BCM_E_PARAM;
    }

    if (key_or_data == 1) {
        keygen_oper = &ftg_keygen_md->skey_keygen_md.keygen_oper;
        num_keygen_parts = ftg_keygen_md->skey_keygen_md.num_parts;
        keygen_profile = _BCM_FIELD_FT_KEYGEN_PROFILE_AAB;
    } else {
        keygen_oper = &ftg_keygen_md->sdata_keygen_md.keygen_oper;
        num_keygen_parts = ftg_keygen_md->sdata_keygen_md.num_parts;
        keygen_profile = _BCM_FIELD_FT_KEYGEN_PROFILE_AB;
    }

    /* Get qual_ext_offset_info */
    ext_ctrl_sel_info = keygen_oper->ext_ctrl_sel_info;
    ext_ctrl_sel_info_count = keygen_oper->ext_ctrl_sel_info_count;

    /* Dump Qualifiers and Extractors info from Keygen_oper */
    LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "FT Group=%d Keygen Info for %s\r\n"),
                ftg_keygen_md->ft_group_id,
                ((key_or_data == 1) ? "SESSION KEY" : "SESSION DATA")));
    (void) _bcm_field_keygen_qual_offset_info_dump(unit,
                num_keygen_parts, keygen_oper);

    (void) _bcm_field_ft_keygen_extractors_init(ext_codes);
    rv = _bcm_field_hx5_ft_keygen_extractors_set(unit, keygen_profile,
            ext_ctrl_sel_info_count, ext_ctrl_sel_info, ext_codes);
    BCMI_IF_ERROR_CLEANUP(rv);

    /* Process extracted qualifiers and extractors */
    if (ext_ctrl_sel_info != NULL) {
        rv = _field_hx5_ft_qual_ext_offset_info_get(unit,
                num_keygen_parts,
                keygen_oper, qual_ext_offset_info);
        BCMI_IF_ERROR_CLEANUP(rv);

        /* Update Mask of extractors */
        rv = _bcm_field_hx5_ft_keygen_extractors_mask_set(unit,
                *qual_ext_offset_info, ext_codes);
        BCMI_IF_ERROR_CLEANUP(rv);

        rv = _field_hx5_ft_qual_offset_ext_shift_apply(unit,
                0, ext_codes, *qual_ext_offset_info);
        BCMI_IF_ERROR_CLEANUP(rv);
    }

    /* Dump Hardware Extraction codes */
    (void) _bcm_field_hx5_ft_keygen_hw_extractors_dump(unit, ext_codes);

cleanup:

    if (BCM_FAILURE(rv)) {
        if (*qual_ext_offset_info != NULL) {
            sal_free(*qual_ext_offset_info);
            *qual_ext_offset_info = NULL;
        }
    }

    return rv;
}

/*
 * Function:
 *    _field_hx5_ftg_qual_offset_width_get
 *
 * Purpose:
 *   Get start and width from offset_arr for a qualifier.
 *
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    offset_arr        - (IN) Offset arr.
 *    start_bit         - (OUT) Start bit position.
 *    width             - (OUT) Number of bits.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_qual_offset_width_get(int unit,
            bcmi_keygen_qual_offset_t *offset_arr,
            int *start_bit,
            int *width)
{
    int offset = 0;
    int part_width = 0;
    int total_width = 0;
    int num_offsets = 0;

    for (offset = 0; offset < BCMI_KEYGEN_QUAL_OFFSETS_MAX; offset++) {
        part_width = offset_arr->width[offset];
        if (!part_width) {
            continue;
        }
        total_width += part_width;
        num_offsets += 1;
        if (num_offsets == offset_arr->num_offsets) {
            break;
        }
    }

    *start_bit = offset_arr->offset[0];
    *width = total_width;

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_hx5_ftg_keygen_oper_dump
 *
 * Purpose:
 *   Dump session key/data extraction information.
 *
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC void
_field_hx5_ftg_keygen_oper_dump(
        int unit,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    uint32 idx = 0;
    bcmi_ft_group_alu_info_t *keydata_info = NULL;
    _field_ft_group_keygen_oper_info_t *oper_info = NULL;

    oper_info = &ftg_keygen_md->keydata_oper_info;

    LOG_VERBOSE(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "===Extraction Operation Info===\r\n")));
    LOG_VERBOSE(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "---Session Keys---\r\n")));
    LOG_VERBOSE(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "  Session Key Mode = %d\r\n"),
             oper_info->mode));
    LOG_VERBOSE(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "  Number of Session Keys = %d\r\n"),
             oper_info->num_skey_info));
    LOG_VERBOSE(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "|%5s |%10s |%10s |%10s |\r\n"),
             "Idx", "Element", "Offset", "Length"));
    for (idx = 0; idx < oper_info->num_skey_info; idx++) {
        keydata_info = &oper_info->skey_info[idx];
        LOG_VERBOSE(BSL_LS_BCM_FP,
                (BSL_META_U(unit, "|%5d |%10d |%10d |%10d |\r\n"),
                 idx,
                 keydata_info->element_type1,
                 keydata_info->key1.location,
                 keydata_info->key1.length));
    }
    LOG_VERBOSE(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "---Session Data---\r\n")));
    LOG_VERBOSE(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "  Number of Session Data = %d\r\n"),
             oper_info->num_sdata_info));
    LOG_VERBOSE(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "|%5s |%10s |%10s |"),
             "Idx", "FlowCheck", "Type"));
    LOG_VERBOSE(BSL_LS_BCM_FP,
            (BSL_META_U(unit, "%10s |%10s |%10s |%10s |\r\n"),
             "Element", "Offset", "Length", "ALU Req"));
    for (idx = 0; idx < oper_info->num_sdata_info; idx++) {
        keydata_info = &oper_info->sdata_info[idx];
        LOG_VERBOSE(BSL_LS_BCM_FP,
                (BSL_META_U(unit, "|%5d |%10x |%10s |"),
                 idx,
                 keydata_info->flowchecker_id,
                 "Primary"));
        LOG_VERBOSE(BSL_LS_BCM_FP,
                (BSL_META_U(unit, "%10d |"
                            "%10d |%10d |%10d |\r\n"),
                 keydata_info->element_type1,
                 keydata_info->key1.location,
                 keydata_info->key1.length,
                 keydata_info->key1.is_alu));

        /* Print secondary and Action key */
        if (keydata_info->flowchecker_id != 0) {
            if (keydata_info->action_element_type !=
                    bcmFlowtrackerTrackingParamTypeNone) {
                LOG_VERBOSE(BSL_LS_BCM_FP,
                        (BSL_META_U(unit, "|%5d |%10x |%10s |"),
                         idx,
                         keydata_info->flowchecker_id,
                         "Action"));
                LOG_VERBOSE(BSL_LS_BCM_FP,
                        (BSL_META_U(unit, "%10d |"
                                    "%10d |%10d |%10d |\r\n"),
                         keydata_info->action_element_type,
                         keydata_info->action_key.location,
                         keydata_info->action_key.length,
                         keydata_info->key1.is_alu));
            }
            if (keydata_info->element_type2 !=
                    bcmFlowtrackerTrackingParamTypeNone) {
                LOG_VERBOSE(BSL_LS_BCM_FP,
                        (BSL_META_U(unit, "|%5d |%10x |%10s |"),
                         idx,
                         BCMI_FT_FLOWCHECKER_INDEX_SET(keydata_info->flowchecker_id,
                             BCM_FT_IDX_TYPE_2ND_CHECK),
                         "Second"));
                LOG_VERBOSE(BSL_LS_BCM_FP,
                        (BSL_META_U(unit, "%10d |"
                                    "%10d |%10d |%10d |\r\n"),
                         keydata_info->element_type2,
                         keydata_info->key2.location,
                         keydata_info->key2.length,
                         keydata_info->key2.is_alu));
            }
        }
    }
}

/*
 * Function:
 *    _field_hx5_ftg_keygen_initialize
 *
 * Purpose:
 *   Initialize ft group keygen metadata struct.
 *
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_initialize(
        int unit,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    _field_stage_t *stage_fc = NULL;

    if (ftg_keygen_md == NULL) {
        return (BCM_E_PARAM);
    }

    /* Validate Flowtracker group */
    rv = bcmi_ft_group_is_invalid(unit, ftg_keygen_md->ft_group_id);

    /* Get Flowtracker Stage info */
    if (BCM_SUCCESS(rv)) {
        rv = _field_stage_control_get(unit,
                _BCM_FIELD_STAGE_FLOWTRACKER, &stage_fc);
    }

    if (BCM_SUCCESS(rv)) {
        /* Ft info from stage */
        ftg_keygen_md->ft_info = stage_fc->ft_info;
    }

    /* Move to next state */
    ftg_keygen_md->state_rv = rv;
    ftg_keygen_md->prev_state = ftg_keygen_md->state;
    if (BCM_FAILURE(rv)) {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_END;
    } else {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_CFG_ALLOC;
    }

    return rv;
}

/*
 * Function:
 *    _field_hx5_ftg_keygen_cfg_alloc
 *
 * Purpose:
 *    Read FT group tracking param and flowchecks info and allocate
 *    memories to qual_info_arr for input to Keygen algo.
 *
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_cfg_alloc(
        int unit,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int num_tracking_params = 0;
    int num_flowchecks = 0;
    int num_alu_data = 0;
    bcm_flowtracker_group_t ft_group_id;

    ftg_keygen_md->prev_state = ftg_keygen_md->state;
    ft_group_id = ftg_keygen_md->ft_group_id;

    /* Get number of tracking params from group template */
    rv = bcm_esw_flowtracker_group_tracking_params_get(unit,
            ft_group_id, 0, NULL, &num_tracking_params);

    /* Get number of flowcheck info from flowtracker group */
    if (BCM_SUCCESS(rv)) {
        rv = bcmi_flowtracker_check_get_all(unit,
                ft_group_id, 0, 0, NULL, &num_flowchecks);
    }

    /* Get number of Alu Data */
    if (BCM_SUCCESS(rv)) {
        rv = _field_hx5_ftg_alu_data_info_get(unit,
                ft_group_id, NULL, &num_alu_data);
    }

    /* Allocate memory for tracking params info */
    if (BCM_SUCCESS(rv)) {
        ftg_keygen_md->num_tracking_params = num_tracking_params;
        _FP_XGS3_ALLOC(ftg_keygen_md->tracking_params_info,
                num_tracking_params * sizeof(bcm_flowtracker_tracking_param_info_t),
                "tracking params info");
        if (ftg_keygen_md->tracking_params_info == NULL) {
            rv = BCM_E_MEMORY;
        }
    }

    /* Allocate memory for flowchecks */
    if (BCM_SUCCESS(rv)) {
        ftg_keygen_md->num_flowchecks = num_flowchecks;
        _FP_XGS3_ALLOC(ftg_keygen_md->flowtracker_checks,
                num_flowchecks * sizeof(bcm_flowtracker_check_t),
                "flowtracker checks");
        if (ftg_keygen_md->flowtracker_checks == NULL) {
            rv = BCM_E_MEMORY;
        }
    }

    /* Read tracking params info */
    if (BCM_SUCCESS(rv)) {
        rv = bcm_esw_flowtracker_group_tracking_params_get(unit,
                ft_group_id, num_tracking_params,
                &ftg_keygen_md->tracking_params_info[0],
                &num_tracking_params);
    }

    /* Read flowtracker checks */
    if (BCM_SUCCESS(rv)) {
        rv = bcmi_flowtracker_check_get_all(unit,
                ft_group_id, 0, num_flowchecks,
                &ftg_keygen_md->flowtracker_checks[0],
                &num_flowchecks);
    }

    /* Read ALU Data */
    if (BCM_SUCCESS(rv)) {
        rv = _field_hx5_ftg_alu_data_info_get(unit, ft_group_id,
                &ftg_keygen_md->alu_data_info,
                &ftg_keygen_md->num_alu_data);
    }

    /* Allocate memory for qual_info arr of session key/Data */
    if (BCM_SUCCESS(rv)) {
        rv = _field_hx5_ftg_keygen_cfg_qual_alloc(unit, ftg_keygen_md);
    }

    /* Move to next state */
    ftg_keygen_md->state_rv = rv;
    if (BCM_FAILURE(rv)) {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_END;
    } else {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_CFG_ALU_QUAL_ADD;
    }

    return rv;
}

/*
 * Function:
 *    _field_hx5_ftg_keygen_cfg_alu_qual_add
 *
 * Purpose:
 *    Read FT group Alu data info and populate qual_info_arr
 *    for session data extraction.
 *
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_cfg_alu_qual_add(
        int unit,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int new_idx = 0;
    int idx = 0;
    int shift = 0;
    uint16 param;
    uint16 allocated_quals = 0;
    bcm_field_qualify_t qual;
    _field_ft_tracking_param_qual_map_t *pq_map = NULL;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;
    bcmi_keygen_qual_info_t *qual_info_arr = NULL;
    _field_ft_group_keydata_keygen_md_t *sdata_keygen_md = NULL;

    /* Check if ALU data is required */
    if (ftg_keygen_md->num_alu_data == 0) {
        goto cleanup;
    }

    ftg_keygen_md->prev_state = ftg_keygen_md->state;
    map_db = ftg_keygen_md->ft_info->param_qual_map_db;
    shift = _FP_FT_TRACKING_ALU_DATA_PARAM_MAP_START;

    /* Add to Session Data qual_info_arr */
    sdata_keygen_md = &ftg_keygen_md->sdata_keygen_md;
    allocated_quals = sdata_keygen_md->qual_info_count;
    qual_info_arr = sdata_keygen_md->qual_info_arr;

    for (idx = 0; idx < ftg_keygen_md->num_alu_data; idx++) {
        param = ftg_keygen_md->alu_data_info[idx].param;
        pq_map = map_db->param_qual_map[param + shift];
        qual = pq_map->qual;

        if (BCM_FIELD_QSET_TEST(sdata_keygen_md->qset, qual)) {
            continue;
        }

        new_idx = allocated_quals;
        qual_info_arr[new_idx].qual_id = qual;
        qual_info_arr[new_idx].flags =
            BCMI_KEYGEN_QUAL_F_STICKY_CONTAINERS |
            BCMI_KEYGEN_QUAL_F_USE_FINFO_NEW |
            BCMI_KEYGEN_QUAL_F_USE_L2_EXT_GRAN16;
        allocated_quals++;
        BCM_FIELD_QSET_ADD(sdata_keygen_md->qset, qual);
    }

    sdata_keygen_md->qual_info_count = allocated_quals;

cleanup:

    /* Move to next state */
    ftg_keygen_md->state_rv = rv;
    ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_CFG_FC_QUAL_ADD;

    return rv;
}

/*
 * Function:
 *    _field_hx5_ftg_keygen_cfg_flowcheck_qual_add
 *
 * Purpose:
 *    Read FT group Flowcheck info and populate qual_info_arr
 *    for session data extraction.
 *
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_cfg_flowcheck_qual_add(
        int unit,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int idx = 0;
    int p_idx = 0;
    int new_idx = 0;
    int num_params = 0;
    uint16 param;
    uint16 param_list[2];
    uint16 allocated_quals = 0;
    bcm_field_qualify_t qual;
    bcm_flowtracker_check_t check_id;
    bcm_flowtracker_check_info_t fc_info;
    bcmi_keygen_qual_info_t *qual_info_arr = NULL;
    _field_ft_tracking_param_qual_map_t *pq_map = NULL;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;
    _field_ft_group_keydata_keygen_md_t *sdata_keygen_md = NULL;
    bcm_flowtracker_check_action_info_t action_info;

    /* validate inputs */
    if (ftg_keygen_md == NULL) {
        return (BCM_E_PARAM);
    }

    /* Check if Flowcheck is required */
    if (ftg_keygen_md->num_flowchecks == 0) {
        goto cleanup;
    }

    ftg_keygen_md->prev_state = ftg_keygen_md->state;
    map_db = ftg_keygen_md->ft_info->param_qual_map_db;

    /* Add to Session Data qual_info_arr */
    sdata_keygen_md = &ftg_keygen_md->sdata_keygen_md;
    allocated_quals = sdata_keygen_md->qual_info_count;
    qual_info_arr = sdata_keygen_md->qual_info_arr;

    for (idx = 0; idx < ftg_keygen_md->num_flowchecks; idx++) {
        num_params = 0;
        check_id = ftg_keygen_md->flowtracker_checks[idx];

        sal_memset(&fc_info, 0, sizeof(bcm_flowtracker_check_info_t));
        rv = bcmi_ft_check_get(unit, check_id, &fc_info);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        param_list[0] = fc_info.param;
        num_params++;

        /* If primary flowcheck, get action info */
        if (BCMI_FT_IDX_IS_PRIMARY_CHECK(check_id)) {
            bcm_flowtracker_check_action_info_t_init(&action_info);
            rv = bcm_esw_flowtracker_check_action_info_get(unit,
                    check_id, &action_info);
            BCMI_IF_ERROR_CLEANUP(rv);

            if (action_info.param != bcmFlowtrackerTrackingParamTypeNone) {
                param_list[1] = action_info.param;
                num_params++;
            }
        }

        for (p_idx = 0; p_idx < num_params; p_idx++) {
            param = param_list[p_idx];
            pq_map = map_db->param_qual_map[param];

            if ((param > _FIELD_FT_TRACKING_PARAM_MAX) || (pq_map == NULL)) {
                rv = BCM_E_PARAM;
                BCMI_IF_ERROR_CLEANUP(rv);
            }
            if (pq_map->flags &
                    _FP_FT_TRACKING_PARAM_F_NO_QUAL_MAPPED) {
                continue;
            }

            qual = pq_map->qual;
            if (!BCM_FIELD_QSET_TEST(sdata_keygen_md->qset, qual)) {
                new_idx = allocated_quals;
                qual_info_arr[new_idx].qual_id = qual;
                qual_info_arr[new_idx].flags =
                    BCMI_KEYGEN_QUAL_F_STICKY_CONTAINERS |
                    BCMI_KEYGEN_QUAL_F_USE_FINFO_NEW |
                    BCMI_KEYGEN_QUAL_F_USE_L2_EXT_GRAN16;

                if (pq_map->flags &
                        _FP_FT_TRACKING_PARAM_F_DATA_MODULO_32) {
                    qual_info_arr[new_idx].flags |=
                        BCMI_KEYGEN_QUAL_F_EXTRACT_TO_MODULO_32_OFFSET;
                }
                allocated_quals++;
                BCM_FIELD_QSET_ADD(sdata_keygen_md->qset, qual);
            }
        }
    }

    sdata_keygen_md->qual_info_count = allocated_quals;

cleanup:

    /* Move to next state */
    ftg_keygen_md->state_rv = rv;
    if (BCM_FAILURE(rv)) {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_END;
    } else {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_CFG_DATA_KEY_QUAL_ADD;
    }

    return rv;
}

/*
 * Function:
 *    _field_hx5_ftg_keygen_cfg_data_key_qual_add
 *
 * Purpose:
 *    Read FT group Tracking param info and populate qual_info_arr
 *    for session key and session data extraction.
 *
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_cfg_data_key_qual_add(
        int unit,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int idx = 0;
    int new_idx = 0;
    int is_key = FALSE;
    int num_tracking_params = 0;
    uint16 param;
    uint16 *allocated_quals = NULL;
    bcm_field_qualify_t qual;
    bcmi_keygen_qual_info_t *qual_info_arr = NULL;
    bcm_flowtracker_tracking_param_info_t *tracking_param_info = NULL;
    _field_ft_tracking_param_qual_map_t *pq_map = NULL;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;
    _field_ft_group_keydata_keygen_md_t *skey_keygen_md = NULL;
    _field_ft_group_keydata_keygen_md_t *sdata_keygen_md = NULL;

    map_db = ftg_keygen_md->ft_info->param_qual_map_db;
    num_tracking_params = ftg_keygen_md->num_tracking_params;

    skey_keygen_md = &ftg_keygen_md->skey_keygen_md;
    sdata_keygen_md = &ftg_keygen_md->sdata_keygen_md;

    /* Loop through tracking params */
    for (idx = 0; idx < num_tracking_params; idx++) {
        tracking_param_info = &ftg_keygen_md->tracking_params_info[idx];

        if (tracking_param_info->flags &
                BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY) {
            is_key = TRUE;
            qual_info_arr = skey_keygen_md->qual_info_arr;
            allocated_quals = &skey_keygen_md->qual_info_count;
        } else {
            is_key = FALSE;
            qual_info_arr = sdata_keygen_md->qual_info_arr;
            allocated_quals = &sdata_keygen_md->qual_info_count;
        }

        /* Handling tracking param FlowtrackerCheck */
        if (tracking_param_info->param ==
                bcmFlowtrackerTrackingParamTypeFlowtrackerCheck) {
            rv = bcmi_ft_group_check_validate(unit,
                    BCMI_FT_GROUP_CHECK_MATCH_GROUP,
                    ftg_keygen_md->ft_group_id, tracking_param_info->check_id);
            if ((rv != BCM_E_NONE) || (is_key == TRUE)) {
                rv = BCM_E_PARAM;
                BCMI_IF_ERROR_CLEANUP(rv);
            }
        }

        param = tracking_param_info->param;
        pq_map = map_db->param_qual_map[param];

        if ((param > _FIELD_FT_TRACKING_PARAM_MAX) || (pq_map == NULL)) {
            rv = BCM_E_PARAM;
            BCMI_IF_ERROR_CLEANUP(rv);
        }
        if (pq_map->flags &
                _FP_FT_TRACKING_PARAM_F_NO_QUAL_MAPPED) {
            continue;
        }

        qual = pq_map->qual;
        if (((is_key == TRUE) && BCM_FIELD_QSET_TEST(skey_keygen_md->qset, qual)) ||
            ((is_key == FALSE) && BCM_FIELD_QSET_TEST(sdata_keygen_md->qset, qual))) {
            continue;
        }

        new_idx = *allocated_quals;
        qual_info_arr[new_idx].qual_id = qual;
        qual_info_arr[new_idx].flags =
            BCMI_KEYGEN_QUAL_F_STICKY_CONTAINERS |
            BCMI_KEYGEN_QUAL_F_USE_L2_EXT_GRAN16;

        if (is_key == TRUE) {
            qual_info_arr[new_idx].flags |=
                BCMI_KEYGEN_QUAL_F_USE_L2_EXT_GRAN8;
            if (pq_map->flags &
                    _FP_FT_TRACKING_PARAM_F_KEY_MODULO_32) {
                qual_info_arr[new_idx].flags |=
                    BCMI_KEYGEN_QUAL_F_EXTRACT_TO_MODULO_32_OFFSET;
            }
        } else {
            qual_info_arr[new_idx].flags |=
                BCMI_KEYGEN_QUAL_F_USE_FINFO_NEW;
            if (pq_map->flags &
                    _FP_FT_TRACKING_PARAM_F_DATA_MODULO_32) {
                qual_info_arr[new_idx].flags |=
                    BCMI_KEYGEN_QUAL_F_EXTRACT_TO_MODULO_32_OFFSET;
            }
        }

        (*allocated_quals)++;
        if (is_key == TRUE) {
            BCM_FIELD_QSET_ADD(skey_keygen_md->qset, qual);
        } else {
            BCM_FIELD_QSET_ADD(sdata_keygen_md->qset, qual);
        }
    }

cleanup:

    ftg_keygen_md->state_rv = rv;
    ftg_keygen_md->prev_state = ftg_keygen_md->state;

    if (BCM_FAILURE(rv)) {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_END;
    } else {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_CFG_DATA_QUAL_ADJUST;
    }

    return rv;
}

/*
 * Function:
 *    _field_hx5_ftg_keygen_qual_info_shift
 *
 * Purpose:
 *    Shift qual info in qual_info_arr downward.
 *
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    start_idx         - (IN) Start index (lower number)
 *    end_idx           - (IN) End Index (higher number)
 *    qual_info_arr     - (IN/OUT) Qual info array.
 *    qual_size_arr     - (IN/OUT) Qual size array.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_hx5_ftg_keygen_qual_info_shift(int unit,
        int start_idx,
        int end_idx,
        bcmi_keygen_qual_info_t *qual_info_arr,
        uint16 *qual_size_arr)
{
    int idx = 0;
    int qual_size = 0;
    bcmi_keygen_qual_info_t temp_qual_info;

    qual_size = qual_size_arr[end_idx];
    sal_memcpy(&temp_qual_info, &qual_info_arr[end_idx],
            sizeof(bcmi_keygen_qual_info_t));

    for (idx = end_idx; idx > start_idx; idx--) {
        qual_size_arr[idx] = qual_size_arr[idx - 1];
        sal_memcpy(&qual_info_arr[idx], &qual_info_arr[idx - 1],
            sizeof(bcmi_keygen_qual_info_t));
    }

    qual_size_arr[start_idx] = qual_size;
    sal_memcpy(&qual_info_arr[start_idx], &temp_qual_info,
            sizeof(bcmi_keygen_qual_info_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_hx5_ftg_keygen_cfg_data_qual_adjust
 *
 * Purpose:
 *    Re-arrange, if required, qual_info_arr for session data extraction.
 *
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_cfg_data_qual_adjust(
        int unit,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int idx = 0;
    int rank1_idx = 0;
    uint16 qual_size = 0;
    uint16 qual_info_count = 0;
    uint16 qual_size_arr[20];
    bcmi_keygen_cfg_t keygen_cfg;
    _field_ft_info_t *ft_info = NULL;
    bcmi_keygen_qual_info_t *qual_info_arr = NULL;

    if (ftg_keygen_md == NULL) {
        return (BCM_E_PARAM);
    }

    qual_info_arr = ftg_keygen_md->sdata_keygen_md.qual_info_arr;
    qual_info_count = ftg_keygen_md->sdata_keygen_md.qual_info_count;
    if ((qual_info_arr == NULL) || (qual_info_count <= 1)) {
        goto cleanup;
    }

    ft_info = ftg_keygen_md->ft_info;
    keygen_cfg.qual_cfg_info_db = ft_info->flow_key_qual_cfg;

    /* Create list of qualifier sizes */
    for (idx = 0; idx < qual_info_count; idx++) {
        rv = bcmi_keygen_qual_cfg_max_size_get(unit,
                &keygen_cfg, qual_info_arr[idx].qual_id, &qual_size);
        BCM_IF_ERROR_RETURN(rv);

        qual_size_arr[idx] = qual_size;
    }

    /* Sort qualifiers based on size and usage. Mask is not considered */
    /* Set rank1_idx after ALU_DATA qualifier. */
    rank1_idx = 1;
    for (idx = rank1_idx; idx < qual_info_count; idx++) {

        if ((qual_size_arr[idx] % 8) != 0) {
            /* If qual_size is not multiple of 8, Rank 1 */

            rv = _field_hx5_ftg_keygen_qual_info_shift(unit,
                    rank1_idx, idx, qual_info_arr, qual_size_arr);
            BCMI_IF_ERROR_CLEANUP(rv);
            rank1_idx++;
        }
    }

cleanup:

    ftg_keygen_md->state_rv = rv;
    ftg_keygen_md->prev_state = ftg_keygen_md->state;

    if (BCM_FAILURE(rv)) {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_END;
    } else {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_CFG_KEY_QUAL_ADJUST;
    }

    return rv;
}

/*
 * Function:
 *    _field_hx5_ftg_keygen_cfg_key_qual_adjust
 *
 * Purpose:
 *    Re-arrange, if required, qual_info_arr for session key extraction.
 *
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_cfg_key_qual_adjust(
        int unit,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int idx = 0;
    int rank1_idx = 0;
    int rank2_idx = 0;
    int rank3_idx = 0;
    uint16 qual_size = 0;
    uint16 qual_info_count = 0;
    uint16 qual_size_arr[20];
    bcmi_keygen_cfg_t keygen_cfg;
    _field_ft_info_t *ft_info = NULL;
    bcmi_keygen_qual_info_t *qual_info_arr = NULL;

    if (ftg_keygen_md == NULL) {
        return (BCM_E_PARAM);
    }

    qual_info_arr = ftg_keygen_md->skey_keygen_md.qual_info_arr;
    qual_info_count = ftg_keygen_md->skey_keygen_md.qual_info_count;
    if ((qual_info_arr == NULL) || (qual_info_count <= 1)) {
        goto cleanup;
    }

    ft_info = ftg_keygen_md->ft_info;
    keygen_cfg.qual_cfg_info_db = ft_info->flow_key_qual_cfg;

    /* Create list of qualifier sizes */
    for (idx = 0; idx < qual_info_count; idx++) {
        rv = bcmi_keygen_qual_cfg_max_size_get(unit,
                &keygen_cfg, qual_info_arr[idx].qual_id, &qual_size);
        BCM_IF_ERROR_RETURN(rv);

        qual_size_arr[idx] = qual_size;
    }

    /* Shuffle qualifiers based on size and usage. Mask is not considered */
    rank1_idx = rank2_idx = rank3_idx = 0;
    for (idx = 0; idx < qual_info_count; idx++) {

        if (qual_size_arr[idx] > 104) {
            /* If qual_size is greater than 104 bits, Rank 1 */

            rv = _field_hx5_ftg_keygen_qual_info_shift(unit,
                    rank1_idx, idx, qual_info_arr, qual_size_arr);
            BCMI_IF_ERROR_CLEANUP(rv);
            rank1_idx++; rank2_idx++; rank3_idx++;

        } else if ((qual_size_arr[idx] > 16) &&
                (qual_size_arr[idx] % 16) != 0) {
            /* If qual_size > 16 but not multiple of 16, rank 2. */

            rv = _field_hx5_ftg_keygen_qual_info_shift(unit,
                    rank2_idx, idx, qual_info_arr, qual_size_arr);
            BCMI_IF_ERROR_CLEANUP(rv);
            rank2_idx++; rank3_idx++;
        } else if ((qual_size_arr[idx] % 8) != 0) {
            /* If qual_size is not multiple of 8, rank 3. */

            rv = _field_hx5_ftg_keygen_qual_info_shift(unit,
                    rank3_idx, idx, qual_info_arr, qual_size_arr);
            BCMI_IF_ERROR_CLEANUP(rv);
            rank3_idx++;
        }
    }

cleanup:

    ftg_keygen_md->state_rv = rv;
    ftg_keygen_md->prev_state = ftg_keygen_md->state;

    if (BCM_FAILURE(rv)) {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_END;
    } else {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_DATA_OPER_EXTRACT;
    }

    return rv;
}

/*
 * Function:
 *    _field_hx5_ftg_keygen_data_oper_extract
 *
 * Purpose:
 *    Call keygen algo to extract session data.
 *
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_data_oper_extract(
        int unit,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int mode_idx = 0;
    int ext_modes_count = 0;
    _field_ft_info_t *ft_info = NULL;
    bcmi_keygen_mode_t ext_modes[2];
    bcmi_keygen_cfg_t keygen_cfg;
    bcmi_keygen_oper_t *keygen_oper = NULL;
    bcmi_keygen_mode_t keygen_mode = BCMI_KEYGEN_MODE_SINGLE;
    bcmi_keygen_mode_t keygen_ext_mode = BCMI_KEYGEN_MODE_SINGLE;
    _field_ft_group_keydata_keygen_md_t *sdata_keygen_md = NULL;

    ft_info = ftg_keygen_md->ft_info;
    sdata_keygen_md = &ftg_keygen_md->sdata_keygen_md;

    /* If valid ext modes list is not provided, error. */
    if (sdata_keygen_md->ext_modes_count == 0) {
        rv = BCM_E_INTERNAL;
    }

    /* Session data extraction modes */
    ext_modes_count = 1;
    ext_modes[0] = BCMI_KEYGEN_MODE_SINGLE;

    for (mode_idx = 0; mode_idx < ext_modes_count; mode_idx++) {

        switch(ext_modes[mode_idx]) {
            case BCMI_KEYGEN_MODE_SINGLE:

                keygen_mode = BCMI_KEYGEN_MODE_SINGLE;
                keygen_ext_mode = BCMI_KEYGEN_MODE_SINGLE;
                sal_memset(&keygen_cfg, 0, sizeof(bcmi_keygen_cfg_t));
                keygen_cfg.qual_info_arr = sdata_keygen_md->qual_info_arr;
                keygen_cfg.qual_info_count = sdata_keygen_md->qual_info_count;
                keygen_cfg.qual_cfg_info_db = ft_info->flow_key_qual_cfg;
                keygen_cfg.ext_cfg_db = ft_info->flow_key_ext_cfg_arr[keygen_ext_mode];
                keygen_cfg.mode = keygen_mode;
                keygen_cfg.num_parts = 2;
                keygen_cfg.flags |= BCMI_KEYGEN_CFG_FLAGS_SERIAL_MUX_ALLOC;

                keygen_oper = &sdata_keygen_md->keygen_oper;
                sal_memset(keygen_oper, 0, sizeof(bcmi_keygen_oper_t));
                rv = bcmi_keygen_cfg_process(unit, &keygen_cfg, keygen_oper);
                break;
            default:
                rv = BCM_E_INTERNAL;
        }
        if (BCM_SUCCESS(rv)) {
            break;
        }
    }

    /* Move to next state */
    ftg_keygen_md->state_rv = rv;
    ftg_keygen_md->prev_state = ftg_keygen_md->state;
    if (BCM_FAILURE(rv)) {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_END;
    } else {
        sdata_keygen_md->keygen_mode = keygen_mode;
        sdata_keygen_md->num_parts = keygen_cfg.num_parts;
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_KEY_OPER_EXTRACT;
    }

    return rv;
}

/*
 * Function:
 *    _field_hx5_ftg_keygen_qual_skip_check
 *
 * Purpose:
 *    Check if it is required to skip first qualifier.
 *    In case there are two qualifiers which ar 128 bits,
 *    rest of qualifiers cannot be masked as all Type A
 *    are exhausted. This could be a problem with IPv6
 *    5-tuple. To prevent it, if 2 such qualifiers are
 *    present if is better to skip one qualifier and
 *    extract it statically at the end of Key.
 *
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 *
 * Returns:
 *    BCM_E_XXX
 *
 * Notes:
 *  In function _field_hx5_ftg_keygen_cfg_key_qual_adjust(), we
 *  sort qualifiers are place 128-bit extractors in the front.
 *  It is pre-requisite to this function to work properly.
 */
STATIC int
_field_hx5_ftg_keygen_qual_skip_check(
        int unit,
        _field_ft_group_keydata_keygen_md_t *skey_keygen_md,
        int *skip_qual_extract)
{
    int idx = 0;
    bcm_field_qualify_t qual;
    int qual_skip_req = 0;

    for (idx = 0; idx < 2; idx++) {
        if (skey_keygen_md->qual_info_count > idx) {
            qual = skey_keygen_md->qual_info_arr[idx].qual_id;

            if ((qual == bcmFieldQualifySrcIp6) ||
                (qual == bcmFieldQualifyDstIp6) ||
                (qual == bcmFieldQualifyInnerSrcIp6) ||
                (qual == bcmFieldQualifyInnerDstIp6)) {
                qual_skip_req++;
            }
        }
    }
    if (qual_skip_req == 2) {
        *skip_qual_extract = TRUE;
    } else {
        *skip_qual_extract = FALSE;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_hx5_ftg_keygen_key_oper_extract
 *
 * Purpose:
 *    Call keygen algo to extract session key.
 *
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_key_oper_extract(
        int unit,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int mode_idx = 0;
    int ext_modes_count = 0;
    int skip_qual_extract = FALSE;
    _field_ft_info_t *ft_info = NULL;
    bcmi_keygen_mode_t ext_modes[2];
    bcmi_keygen_cfg_t keygen_cfg;
    bcmi_keygen_oper_t *keygen_oper = NULL;
    bcm_field_qualify_t skipped_qual = bcmFieldQualifySrcIp6;
    bcmi_keygen_mode_t keygen_mode = BCMI_KEYGEN_MODE_SINGLE;
    bcmi_keygen_mode_t keygen_ext_mode = BCMI_KEYGEN_MODE_SINGLE;
    _field_ft_group_keydata_keygen_md_t *skey_keygen_md = NULL;

    ft_info = ftg_keygen_md->ft_info;
    skey_keygen_md = &ftg_keygen_md->skey_keygen_md;
    sal_memset(&keygen_cfg, 0, sizeof(bcmi_keygen_cfg_t));

    /* Check if static qualifier extraction is required */
    rv = _field_hx5_ftg_keygen_qual_skip_check(unit,
            skey_keygen_md, &skip_qual_extract);
    BCMI_IF_ERROR_CLEANUP(rv);

    if (skip_qual_extract == TRUE) {
        skipped_qual = skey_keygen_md->qual_info_arr[0].qual_id;
        ext_modes[0] = BCMI_KEYGEN_MODE_DBLINTRA;
        ext_modes_count = 1;
    } else {
        ext_modes[0] = BCMI_KEYGEN_MODE_SINGLE;
        ext_modes[1] = BCMI_KEYGEN_MODE_DBLINTRA;
        ext_modes_count = 2;
    }

    for (mode_idx = 0; mode_idx < ext_modes_count; mode_idx++) {

        switch(ext_modes[mode_idx]) {
            case BCMI_KEYGEN_MODE_SINGLE:

                keygen_mode = BCMI_KEYGEN_MODE_SINGLE;
                keygen_ext_mode = BCMI_KEYGEN_MODE_SINGLE;

                sal_memset(&keygen_cfg, 0, sizeof(bcmi_keygen_cfg_t));
                keygen_cfg.qual_info_arr = skey_keygen_md->qual_info_arr;
                keygen_cfg.qual_info_count = skey_keygen_md->qual_info_count;
                keygen_cfg.qual_cfg_info_db = ft_info->flow_key_qual_cfg;
                keygen_cfg.ext_cfg_db = ft_info->flow_key_ext_cfg_arr[keygen_ext_mode];
                keygen_cfg.mode = keygen_mode;
                keygen_cfg.num_parts = 2;
                keygen_cfg.flags |= BCMI_KEYGEN_CFG_FLAGS_SERIAL_MUX_ALLOC;

                keygen_oper = &skey_keygen_md->keygen_oper;
                sal_memset(keygen_oper, 0, sizeof(bcmi_keygen_oper_t));
                rv = bcmi_keygen_cfg_process(unit, &keygen_cfg, keygen_oper);
                break;

            case BCMI_KEYGEN_MODE_DBLINTRA:

                keygen_mode = BCMI_KEYGEN_MODE_DBLINTRA;
                keygen_ext_mode = BCMI_KEYGEN_MODE_DBLINTRA;
                sal_memset(&keygen_cfg, 0, sizeof(bcmi_keygen_cfg_t));

                if (skip_qual_extract == TRUE) {
                    /* Remove static qual/extractor from qual_info_arr */
                    keygen_cfg.flags |= BCMI_KEYGEN_CFG_FLAGS_QSET_UPDATE;
                    rv = _field_hx5_ft_keygen_cfg_qual_ext_ctrl_skip(unit,
                            &keygen_cfg, skipped_qual, NULL);
                    BCMI_IF_ERROR_CLEANUP(rv);
                    keygen_cfg.qual_info_arr = &skey_keygen_md->qual_info_arr[1];
                    keygen_cfg.qual_info_count = skey_keygen_md->qual_info_count - 1;
                } else {
                    keygen_cfg.qual_info_arr = &skey_keygen_md->qual_info_arr[0];
                    keygen_cfg.qual_info_count = skey_keygen_md->qual_info_count;
                }
                keygen_cfg.qual_cfg_info_db = ft_info->flow_key_qual_cfg;
                keygen_cfg.ext_cfg_db = ft_info->flow_key_ext_cfg_arr[keygen_ext_mode];
                keygen_cfg.mode = keygen_mode;
                keygen_cfg.num_parts = 3;
                keygen_cfg.flags |= BCMI_KEYGEN_CFG_FLAGS_SERIAL_MUX_ALLOC;

                keygen_oper = &skey_keygen_md->keygen_oper;
                sal_memset(keygen_oper, 0, sizeof(bcmi_keygen_oper_t));
                rv = bcmi_keygen_cfg_process(unit, &keygen_cfg, keygen_oper);
                break;

            default:
                rv = BCM_E_INTERNAL;
                BCMI_IF_ERROR_CLEANUP(rv);
        }
        if (BCM_FAILURE(rv)) {
            /* Free Keygen_cfg */
            if (keygen_cfg.ext_ctrl_sel_info != NULL) {
                sal_free(keygen_cfg.ext_ctrl_sel_info);
                keygen_cfg.ext_ctrl_sel_info = NULL;
            }
        } else if (BCM_SUCCESS(rv) && (skip_qual_extract == TRUE)) {
            /* Update keygen_oper with static extraction info */
            rv = _field_hx5_ft_keygen_oper_qual_ext_ctrl_apply(unit,
                    &keygen_cfg, skipped_qual, keygen_oper);
        }
        /* Break if extraction is successful */
        if (BCM_SUCCESS(rv)) {
            break;
        }
    }

cleanup:

     /* Free Keygen_cfg */
     if (keygen_cfg.ext_ctrl_sel_info != NULL) {
         sal_free(keygen_cfg.ext_ctrl_sel_info);
         keygen_cfg.ext_ctrl_sel_info = NULL;
     }

    /* Move to next state */
    ftg_keygen_md->state_rv = rv;
    ftg_keygen_md->prev_state = ftg_keygen_md->state;
    if (BCM_FAILURE(rv)) {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_END;
    } else {
        skey_keygen_md->keygen_mode = keygen_mode;
        skey_keygen_md->num_parts = keygen_cfg.num_parts;
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_DATA_OPER_PROCESS;
    }

    return rv;
}

/*
 * Function:
 *    _field_hx5_ftg_flowcheck_oper_process
 *
 * Purpose:
 *    Process Session data keygen_oper for flowcheck info.
 *
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    qual_ext_offset_info - (IN) Qual-Extractor info
 *    ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_flowcheck_oper_process(
        int unit,
        _field_ft_qual_ext_offset_info_t *qual_ext_offset_info,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int idx = 0;
    int sd_idx = 0;
    int data_idx = 0;
    int q_idx = 0;
    int width = 0;
    int start_loc = 0;
    int temp_fc_idx = 0;
    int fc_idx = 0;
    bcmi_ft_alu_key_t *sdata_info_key = NULL;
    bcm_field_qualify_t qual;
    bcm_flowtracker_check_t check_id;
    bcmi_ft_group_alu_info_t *sdata_info = NULL;
    bcm_flowtracker_check_info_t fc_info;
    _field_ft_tracking_param_qual_map_t *pq_map = NULL;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;
    bcm_flowtracker_check_action_info_t action_info;

    map_db = ftg_keygen_md->ft_info->param_qual_map_db;
    sdata_info = ftg_keygen_md->keydata_oper_info.sdata_info;
    data_idx = ftg_keygen_md->keydata_oper_info.num_sdata_info;

    for (idx = 0; idx < ftg_keygen_md->num_flowchecks; idx++) {

        check_id = ftg_keygen_md->flowtracker_checks[idx];
        bcm_flowtracker_check_info_t_init(&fc_info);
        rv = bcmi_ft_check_get(unit, check_id, &fc_info);
        BCM_IF_ERROR_RETURN(rv);

        fc_idx = BCMI_FT_FLOWCHECKER_INDEX_GET(check_id);

        /* Search if flowcheck is already added in sdata_info */
        for (sd_idx = 0; sd_idx < data_idx; sd_idx++) {
            temp_fc_idx =
                BCMI_FT_FLOWCHECKER_INDEX_GET(sdata_info[sd_idx].flowchecker_id);

            if (temp_fc_idx == fc_idx) {
                break;
            }
        }
        /* If not found, use last and increment data_idx */
        if (sd_idx == data_idx) {
            data_idx++;
        }

        sdata_info[sd_idx].flowchecker_id =
            BCMI_FT_FLOWCHECKER_INDEX_SET(fc_idx, BCM_FT_IDX_TYPE_CHECK);

        if (BCMI_FT_IDX_IS_2ND_CHECK(check_id)) {
            sdata_info[sd_idx].element_type2 = fc_info.param;
            sdata_info_key = &sdata_info[sd_idx].key2;
        } else {
            sdata_info[sd_idx].element_type1 = fc_info.param;
            sdata_info_key = &sdata_info[sd_idx].key1;
        }

        pq_map = map_db->param_qual_map[fc_info.param];

        if (pq_map->flags & _FP_FT_TRACKING_PARAM_F_NO_FILL) {
            continue;
        }

        if (pq_map->flags & _FP_FT_TRACKING_PARAM_F_NO_QUAL_MAPPED) {
            sdata_info_key->location = 0;
            sdata_info_key->length = pq_map->width;
            sdata_info_key->is_alu =
                ((pq_map->flags & _FP_FT_TRACKING_PARAM_F_ALU_REQ) ? 1 : 0);
        } else {
            qual = pq_map->qual;
            for (q_idx = 0; q_idx < qual_ext_offset_info->size; q_idx++) {
                if (qual_ext_offset_info->qid_arr[q_idx] != qual) {
                    continue;
                }
                if (q_idx == qual_ext_offset_info->size) {
                    BCM_IF_ERROR_RETURN(BCM_E_INTERNAL);
                }
                start_loc = width = 0;
                rv = _field_hx5_ftg_qual_offset_width_get(unit,
                        &qual_ext_offset_info->offset_arr[q_idx],
                        &start_loc, &width);
                BCM_IF_ERROR_RETURN(rv);

                sdata_info_key->location = start_loc;
                sdata_info_key->length = width;
                sdata_info_key->is_alu = 1;
            }
        }

        /* Set action key from primary flowcheck */
        if (!BCMI_FT_IDX_IS_2ND_CHECK(check_id)) {
            bcm_flowtracker_check_action_info_t_init(&action_info);
            rv = bcm_esw_flowtracker_check_action_info_get(unit,
                    check_id, &action_info);
            BCM_IF_ERROR_RETURN(rv);

            if (action_info.param == bcmFlowtrackerTrackingParamTypeNone) {
                continue;
            }
            pq_map = map_db->param_qual_map[action_info.param];
            if (pq_map->flags & _FP_FT_TRACKING_PARAM_F_NO_QUAL_MAPPED) {
                continue;
            }

            qual = pq_map->qual;
            for (q_idx = 0; q_idx < qual_ext_offset_info->size; q_idx++) {
                if (qual_ext_offset_info->qid_arr[q_idx] != qual) {
                    continue;
                }
                start_loc = width = 0;
                rv = _field_hx5_ftg_qual_offset_width_get(unit,
                        &qual_ext_offset_info->offset_arr[q_idx],
                        &start_loc, &width);
                BCM_IF_ERROR_RETURN(rv);

                sdata_info[sd_idx].action_element_type = action_info.param;
                sdata_info[sd_idx].action_key.location = start_loc;
                sdata_info[sd_idx].action_key.length = width;
                sdata_info[sd_idx].action_key.is_alu = 1;
            }
        }
    }

    ftg_keygen_md->keydata_oper_info.num_sdata_info = data_idx;

    return rv;
}

/*
 * Function:
 *    _field_hx5_ftg_tracking_params_oper_process
 *
 * Purpose:
 *    Process Session data keygen_oper for tracking params..
 *
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_tracking_params_oper_process(
        int unit,
        _field_ft_qual_ext_offset_info_t *qual_ext_offset_info,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int width = 0;
    int idx = 0;
    int data_idx = 0;
    int q_idx = 0;
    int start_bit = 0;
    uint16 param = 0;
    bcm_field_qualify_t qual;
    _field_ft_tracking_param_qual_map_t *pq_map = NULL;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;
    _field_ft_group_keygen_oper_info_t *oper_info = NULL;
    bcm_flowtracker_tracking_param_info_t *tracking_param_info = NULL;

    oper_info = &ftg_keygen_md->keydata_oper_info;
    map_db = ftg_keygen_md->ft_info->param_qual_map_db;

    data_idx = oper_info->num_sdata_info;
    idx = 0;
    while(idx < ftg_keygen_md->num_tracking_params) {

        tracking_param_info = &ftg_keygen_md->tracking_params_info[idx];

        if (tracking_param_info->flags &
                BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY) {
            idx++;
            continue;
        }

        param = tracking_param_info->param;
        pq_map = map_db->param_qual_map[param];

        if (pq_map->flags & _FP_FT_TRACKING_PARAM_F_NO_FILL) {
            idx++;
            continue;
        }

        if (pq_map->flags & _FP_FT_TRACKING_PARAM_F_NO_QUAL_MAPPED) {
            oper_info->sdata_info[data_idx].key1.location = 0;
            oper_info->sdata_info[data_idx].key1.is_alu =
                ((pq_map->flags & _FP_FT_TRACKING_PARAM_F_ALU_REQ) ? 1 : 0);
            oper_info->sdata_info[data_idx].key1.length = pq_map->width;
            oper_info->sdata_info[data_idx].element_type1 = param;
            oper_info->sdata_info[data_idx].flowchecker_id = 0;
            data_idx++;
        } else {

            qual = pq_map->qual;
            for (q_idx = 0; q_idx < qual_ext_offset_info->size; q_idx++) {
                if (qual_ext_offset_info->qid_arr[q_idx] != qual) {
                    continue;
                }
                start_bit = width = 0;
                rv = _field_hx5_ftg_qual_offset_width_get(unit,
                        &qual_ext_offset_info->offset_arr[q_idx],
                        &start_bit, &width);
                BCMI_IF_ERROR_CLEANUP(rv);

                oper_info->sdata_info[data_idx].key1.location = start_bit;
                oper_info->sdata_info[data_idx].key1.is_alu = 0;
                oper_info->sdata_info[data_idx].key1.length = width;
                oper_info->sdata_info[data_idx].element_type1 = param;
                oper_info->sdata_info[data_idx].flowchecker_id = 0;
                data_idx++;
            }
        }
        idx++;
    }

    oper_info->num_sdata_info = data_idx;

cleanup:

    return rv;
}

/*
 * Function:
 *    _field_hx5_ftg_alu_data_oper_process
 *
 * Purpose:
 *    Output of session data keygen is processed for ALU data
 *    and results are saved in extractors codes.
 *
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    qual_ext_offset_info - (IN) Qual-Extractor info.
 *    ext_codes         - (OUT) extractor codes.
 *    ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_alu_data_oper_process(
        int unit,
        _field_ft_qual_ext_offset_info_t *qual_ext_offset_info,
        _field_ft_keygen_ext_codes_t *ext_codes,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int bit = 0;
    int q_idx = 0;
    int index = 0;
    int shift = 0;
    int width = 0;
    int offset = 0;
    int location = 0;
    int start_loc = 0;
    int qual_width = 0;
    uint16 param = 0;
    uint32 type_mask[1] = {0};
    bcm_field_qualify_t qual;
    _field_ft_tracking_param_qual_map_t *pq_map = NULL;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;
    _field_ft_tracking_alu_data_info_t *alu_data_info = NULL;
    _field_ft_keygen_type_c_ext_t *type_c_extractors = NULL;

    if ((qual_ext_offset_info == NULL) ||
            (ext_codes == NULL) || (ftg_keygen_md == NULL)) {
        return BCM_E_PARAM;
    }

    map_db = ftg_keygen_md->ft_info->param_qual_map_db;
    type_c_extractors = &ext_codes->type_c_extractors;
    shift = _FP_FT_TRACKING_ALU_DATA_PARAM_MAP_START;

    for (index = 0; index < ftg_keygen_md->num_alu_data; index++) {
        alu_data_info = &ftg_keygen_md->alu_data_info[index];
        param = alu_data_info->param;
        offset = alu_data_info->offset;
        width = alu_data_info->width;
        pq_map = map_db->param_qual_map[param + shift];
        qual = pq_map->qual;

        for (q_idx = 0; q_idx < qual_ext_offset_info->size; q_idx++) {
            if (qual_ext_offset_info->qid_arr[q_idx] != qual) {
                continue;
            }
            start_loc = qual_width = 0;
            rv = _field_hx5_ftg_qual_offset_width_get(unit,
                    &qual_ext_offset_info->offset_arr[q_idx],
                    &start_loc, &qual_width);
            BCM_IF_ERROR_RETURN(rv);

            if ((start_loc + width) > qual_width) {
                return BCM_E_INTERNAL;
            }
            location = start_loc + pq_map->start_loc;

            for (bit = 0; bit < width; bit++) {
                type_c_extractors->l1_e1_sel[offset + bit] = location + bit;
            }
            SHR_BITSET_RANGE(&type_mask[0], offset, width);
        }
    }

    ext_codes->type_c_mask.mask = type_mask[0];

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_hx5_ftg_keygen_data_oper_process
 *
 * Purpose:
 *    Process Session data keygen_oper.
 *
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_data_oper_process(
        int unit,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int num_sdata_info = 0;
    _field_ft_keygen_ext_codes_t ext_codes;
    _field_ft_qual_ext_offset_info_t *qual_ext_offset_info = NULL;
    _field_ft_group_keydata_keygen_md_t *sdata_keygen_md = NULL;
    _field_ft_group_keygen_oper_info_t *oper_info = NULL;

    /* Get qualifier/extractors hw info from keygen_oper */
    rv = _field_hx5_ftg_qual_ext_hw_info_get(unit,
                0, ftg_keygen_md, &qual_ext_offset_info, &ext_codes);

    if (BCM_FAILURE(rv) || (qual_ext_offset_info == NULL)) {
        goto cleanup;
    }

    oper_info = &ftg_keygen_md->keydata_oper_info;
    sdata_keygen_md = &ftg_keygen_md->sdata_keygen_md;

    /* Allocate memory for flowtracker group session data db */
    num_sdata_info = sdata_keygen_md->max_qual_info_alloc;
    _FP_XGS3_ALLOC(oper_info->sdata_info,
            num_sdata_info * sizeof(bcmi_ft_group_alu_info_t),
            "session data info");
    if (oper_info->sdata_info == NULL) {
        BCMI_IF_ERROR_CLEANUP(BCM_E_MEMORY);
    }

    /* Prepare Session data from flowchecks */
    if (BCM_SUCCESS(rv)) {
        rv = _field_hx5_ftg_flowcheck_oper_process(unit,
                qual_ext_offset_info, ftg_keygen_md);
    }

    /* Prepare Session data from tracking params */
    if (BCM_SUCCESS(rv)) {
        rv = _field_hx5_ftg_tracking_params_oper_process(unit,
                qual_ext_offset_info, ftg_keygen_md);
    }

    /* Process alu data oper info */
    if (BCM_SUCCESS(rv)) {
        rv = _field_hx5_ftg_alu_data_oper_process(unit,
                qual_ext_offset_info, &ext_codes, ftg_keygen_md);
    }

    /* Encode Session Data extractors. */
    if (BCM_SUCCESS(rv)) {
        rv = _bcm_field_hx5_ft_extractors_encode(unit, &ext_codes,
                &oper_info->sdata_ext_count, &oper_info->sdata_ext_info);
    }

cleanup:

    if (qual_ext_offset_info != NULL) {
        (void) _field_hx5_ft_qual_ext_offset_info_free(unit,
                &qual_ext_offset_info);
    }

    /* Move to next state */
    ftg_keygen_md->state_rv = rv;
    ftg_keygen_md->prev_state = ftg_keygen_md->state;
    if (BCM_FAILURE(rv)) {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_END;
    } else {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_KEY_OPER_PROCESS;
    }

    return rv;
}

/*
 * Function:
 *    _field_hx5_ftg_keygen_key_oper_process
 *
 * Purpose:
 *    Process Session data keygen_oper.
 *
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_key_oper_process(
        int unit,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    int idx = 0;
    int key_idx = 0;
    int qual_idx = 0;
    int start_bit = 0;
    int width = 0;
    int num_skey_info = 0;
    uint16 param  = 0;
    bcm_field_qualify_t qual;
    _field_ft_keygen_ext_codes_t ext_codes;
    _field_ft_tracking_param_qual_map_t *pq_map = NULL;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;
    _field_ft_group_keygen_oper_info_t *oper_info = NULL;
    _field_ft_group_keydata_keygen_md_t *skey_keygen_md = NULL;
    _field_ft_qual_ext_offset_info_t *qual_ext_offset_info = NULL;
    bcm_flowtracker_tracking_param_info_t *tracking_param_info = NULL;

    /* Get qualifier/extractors hw info from keygen_oper */
    rv = _field_hx5_ftg_qual_ext_hw_info_get(unit,
                1, ftg_keygen_md, &qual_ext_offset_info, &ext_codes);

    if (BCM_FAILURE(rv) || (qual_ext_offset_info == NULL)) {
        goto cleanup;
    }

    oper_info = &ftg_keygen_md->keydata_oper_info;
    map_db = ftg_keygen_md->ft_info->param_qual_map_db;
    skey_keygen_md = &ftg_keygen_md->skey_keygen_md;

    /* Allocate memory for flowtracker group session key db */
    num_skey_info = skey_keygen_md->max_qual_info_alloc;
    _FP_XGS3_ALLOC(oper_info->skey_info,
            num_skey_info * sizeof(bcmi_ft_group_alu_info_t),
            "session key info");
    if (oper_info->skey_info == NULL) {
        BCMI_IF_ERROR_CLEANUP(BCM_E_MEMORY);
    }

    key_idx = 0;
    while(idx < ftg_keygen_md->num_tracking_params) {

        tracking_param_info = &ftg_keygen_md->tracking_params_info[idx];

        /* Skip if tracking param is not Key. */
        if (!(tracking_param_info->flags &
                    BCM_FLOWTRACKER_TRACKING_PARAM_TYPE_KEY)) {
            idx++;
            continue;
        }

        param = tracking_param_info->param;
        pq_map = map_db->param_qual_map[param];
        qual = pq_map->qual;

        for (qual_idx = 0; qual_idx < qual_ext_offset_info->size; qual_idx++) {
            if (qual_ext_offset_info->qid_arr[qual_idx] != qual) {
                continue;
            }
            start_bit = width = 0;
            rv = _field_hx5_ftg_qual_offset_width_get(unit,
                    &qual_ext_offset_info->offset_arr[qual_idx],
                    &start_bit, &width);
            BCMI_IF_ERROR_CLEANUP(rv);

            oper_info->skey_info[key_idx].key1.location = start_bit;
            oper_info->skey_info[key_idx].key1.is_alu = 0;
            oper_info->skey_info[key_idx].key1.length = width;
            oper_info->skey_info[key_idx].element_type1 = param;
            oper_info->skey_info[key_idx].flowchecker_id = 0;
            key_idx++;
        }
        idx++;
    }
    oper_info->num_skey_info = key_idx;
    if (skey_keygen_md->keygen_mode == BCMI_KEYGEN_MODE_SINGLE) {
        oper_info->mode = bcmiFtGroupModeSingle;
    } else {
        oper_info->mode = bcmiFtGroupModeDouble;
    }

    /* Encode Session Key extractors. */
    rv = _bcm_field_hx5_ft_extractors_encode(unit, &ext_codes,
            &oper_info->skey_ext_count, &oper_info->skey_ext_info);

cleanup:

    if (qual_ext_offset_info != NULL) {
        (void) _field_hx5_ft_qual_ext_offset_info_free(unit,
                &qual_ext_offset_info);
    }

    /* Move to next state */
    ftg_keygen_md->state_rv = rv;
    ftg_keygen_md->prev_state = ftg_keygen_md->state;
    if (BCM_FAILURE(rv)) {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_END;
    } else {
        ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_OPER_SAVE;
    }

    return rv;
}

/*
 * Function:
 *    _field_hx5_ftg_keygen_oper_save
 *
 * Purpose:
 *    Save FT Group Session Key/Data to FT Group DB.
 *
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 *
 * Returns:
 *    BCM_E_PARAM       - Invalid inputs.
 *    BCM_E_MEMORY      - Failed to allocate memory.
 *    BCM_E_NONE        - Success.
 */
STATIC int
_field_hx5_ftg_keygen_oper_save(
        int unit,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_group_t ft_group_id;
    _field_ft_group_keygen_oper_info_t *oper_info = NULL;

    /* Dump Session Key and Session Data */
    _field_hx5_ftg_keygen_oper_dump(unit, ftg_keygen_md);

    if (ftg_keygen_md->save_op == FALSE) {
        rv = BCM_E_NONE;
        goto cleanup;
    }

    ft_group_id = ftg_keygen_md->ft_group_id;
    oper_info = &ftg_keygen_md->keydata_oper_info;

    rv = bcmi_esw_ft_group_extraction_mode_set(unit,
            1, ft_group_id, oper_info->mode);

    if (BCM_SUCCESS(rv)) {
        rv = bcmi_esw_ft_group_extraction_mode_set(unit,
                0, ft_group_id, oper_info->mode);
    }

    if (BCM_SUCCESS(rv) && (oper_info->skey_info != NULL)) {
        rv = bcmi_esw_ft_group_extraction_alu_info_set(unit, ft_group_id,
                1, oper_info->num_skey_info, oper_info->skey_info);
    }

    if (BCM_SUCCESS(rv) && (oper_info->skey_ext_info != NULL)) {
        rv = bcmi_esw_ft_group_extraction_hw_info_set(unit, ft_group_id,
                1, oper_info->skey_ext_count, oper_info->skey_ext_info);
    }

    if (BCM_SUCCESS(rv) && (oper_info->sdata_info != NULL)) {
        rv = bcmi_esw_ft_group_extraction_alu_info_set(unit, ft_group_id,
                0, oper_info->num_sdata_info, oper_info->sdata_info);
    }

    if (BCM_SUCCESS(rv) && (oper_info->sdata_ext_info != NULL)) {
        rv = bcmi_esw_ft_group_extraction_hw_info_set(unit, ft_group_id,
                0, oper_info->sdata_ext_count, oper_info->sdata_ext_info);
    }

cleanup:

    ftg_keygen_md->state_rv = rv;
    ftg_keygen_md->prev_state = ftg_keygen_md->state;
    ftg_keygen_md->state = _FIELD_FT_GROUP_KEYGEN_END;

    return rv;
}

/*
 * Function:
 *    _field_hx5_ftg_keygen_end
 *
 * Purpose:
 *    Deallocate memories in FT Group Keygen metadata.
 *
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    ftg_keygen_md     - (INOUT) FT Group Keygen metadata.
 *
 * Returns:
 *    BCM_E_PARAM       - Invalid inputs.
 *    BCM_E_MEMORY      - Failed to allocate memory.
 *    BCM_E_NONE        - Success.
 */
STATIC int
_field_hx5_ftg_keygen_end(
        int unit,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    if (ftg_keygen_md->tracking_params_info != NULL) {
        sal_free(ftg_keygen_md->tracking_params_info);
        ftg_keygen_md->tracking_params_info = NULL;
    }

    if (ftg_keygen_md->flowtracker_checks != NULL) {
        sal_free(ftg_keygen_md->flowtracker_checks);
        ftg_keygen_md->flowtracker_checks = NULL;
    }

    if (ftg_keygen_md->skey_keygen_md.qual_info_arr != NULL) {
        sal_free(ftg_keygen_md->skey_keygen_md.qual_info_arr);
        ftg_keygen_md->skey_keygen_md.qual_info_arr = NULL;
    }

    if (ftg_keygen_md->sdata_keygen_md.qual_info_arr != NULL) {
        sal_free(ftg_keygen_md->sdata_keygen_md.qual_info_arr);
        ftg_keygen_md->sdata_keygen_md.qual_info_arr = NULL;
    }

    (void) bcmi_keygen_oper_free(unit,
            ftg_keygen_md->skey_keygen_md.num_parts,
            &ftg_keygen_md->skey_keygen_md.keygen_oper);

    (void) bcmi_keygen_oper_free(unit,
            ftg_keygen_md->sdata_keygen_md.num_parts,
            &ftg_keygen_md->sdata_keygen_md.keygen_oper);

    if (ftg_keygen_md->keydata_oper_info.sdata_info != NULL) {
        sal_free(ftg_keygen_md->keydata_oper_info.sdata_info);
        ftg_keygen_md->keydata_oper_info.sdata_info = NULL;
    }

    if (ftg_keygen_md->keydata_oper_info.skey_info != NULL) {
        sal_free(ftg_keygen_md->keydata_oper_info.skey_info);
        ftg_keygen_md->keydata_oper_info.skey_info = NULL;
    }

    if (ftg_keygen_md->keydata_oper_info.skey_ext_info != NULL) {
        sal_free(ftg_keygen_md->keydata_oper_info.skey_ext_info);
        ftg_keygen_md->keydata_oper_info.skey_ext_info = NULL;
    }

    if (ftg_keygen_md->keydata_oper_info.sdata_ext_info != NULL) {
        sal_free(ftg_keygen_md->keydata_oper_info.sdata_ext_info);
        ftg_keygen_md->keydata_oper_info.sdata_ext_info = NULL;
    }

    return (ftg_keygen_md->state_rv);
}

/*
 * Function:
 *    _field_hx5_ftg_keygen_process
 *
 * Purpose:
 *    Gernerate session key, session data and alu data extractions
 *    corresponding to FT Group tracking params and flowchecks.
 *
 * Parameters:
 *    unit              - (IN) BCM device number.
 *    ftg_keygen_md     - (IN) FT Group Keygen metadata.
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_hx5_ftg_keygen_process(
        int unit,
        _field_ft_group_keygen_md_t *ftg_keygen_md)
{
    int rv = BCM_E_NONE;

    do {
        switch(ftg_keygen_md->state) {
            case _FIELD_FT_GROUP_KEYGEN_START:
                rv = _field_hx5_ftg_keygen_initialize(unit, ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                "Completed state->"
                                "_FIELD_FT_GROUP_KEYGEN_START ret=%d\r\n"),
                            ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_CFG_ALLOC:
                rv = _field_hx5_ftg_keygen_cfg_alloc(unit, ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                "Completed state->"
                                "_FIELD_FT_GROUP_KEYGEN_CFG_ALLOC ret=%d\r\n"),
                            ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_CFG_ALU_QUAL_ADD:
                rv = _field_hx5_ftg_keygen_cfg_alu_qual_add(unit, ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                "Completed state->"
                                "_FIELD_FT_GROUP_KEYGEN_CFG_ALU_QUAL_ADD ret=%d\r\n"),
                            ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_CFG_FC_QUAL_ADD:
                rv = _field_hx5_ftg_keygen_cfg_flowcheck_qual_add(unit, ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                "Completed state->"
                                "_FIELD_FT_GROUP_KEYGEN_CFG_FC_QUAL_ADD ret=%d\r\n"),
                            ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_CFG_DATA_KEY_QUAL_ADD:
                rv = _field_hx5_ftg_keygen_cfg_data_key_qual_add(unit, ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                "Completed state->"
                                "_FIELD_FT_GROUP_KEYGEN_CFG_DATA_KEY_QUAL_ADD ret=%d\r\n"),
                            ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_CFG_DATA_QUAL_ADJUST:
                rv = _field_hx5_ftg_keygen_cfg_data_qual_adjust(unit, ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                "Completed state->"
                                "_FIELD_FT_GROUP_KEYGEN_CFG_DATA_QUAL_ADJUST ret=%d\r\n"),
                            ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_CFG_KEY_QUAL_ADJUST:
                rv = _field_hx5_ftg_keygen_cfg_key_qual_adjust(unit, ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                "Completed state->"
                                "_FIELD_FT_GROUP_KEYGEN_CFG_KEY_QUAL_ADJUST ret=%d\r\n"),
                            ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_DATA_OPER_EXTRACT:
                rv = _field_hx5_ftg_keygen_data_oper_extract(unit, ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                "Completed state->"
                                "_FIELD_FT_GROUP_KEYGEN_DATA_OPER_EXTRACT ret=%d\r\n"),
                            ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_KEY_OPER_EXTRACT:
                rv = _field_hx5_ftg_keygen_key_oper_extract(unit, ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                "Completed state->"
                                "_FIELD_FT_GROUP_KEYGEN_KEY_OPER_EXTRACT ret=%d\r\n"),
                            ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_DATA_OPER_PROCESS:
                rv = _field_hx5_ftg_keygen_data_oper_process(unit, ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                "Completed state->"
                                "_FIELD_FT_GROUP_KEYGEN_DATA_OPER_PROCESS ret=%d\r\n"),
                            ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_KEY_OPER_PROCESS:
                rv = _field_hx5_ftg_keygen_key_oper_process(unit, ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                "Completed state->"
                                "_FIELD_FT_GROUP_KEYGEN_KEY_OPER_PROCESS ret=%d\r\n"),
                            ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_OPER_SAVE:
                rv = _field_hx5_ftg_keygen_oper_save(unit, ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                "Completed state->"
                                "_FIELD_FT_GROUP_KEYGEN_OPER_SAVE ret=%d\r\n"),
                            ftg_keygen_md->state_rv));
                break;
            case _FIELD_FT_GROUP_KEYGEN_END:
                rv = _field_hx5_ftg_keygen_end(unit, ftg_keygen_md);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                "Completed state->"
                                "_FIELD_FT_GROUP_KEYGEN_END ret=%d\r\n"),
                            ftg_keygen_md->state_rv));
                return rv;
            default:
                /* Not a valid case */
                return rv;
        }
    } while(1);

    return rv;
}


/*
 * Function:
 *    _bcm_field_ft_entry_traverse
 *
 * Purpose:
 *    Traverse fp entries in flowtracker stage and
 *    call callback to process the entry.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    id         - (IN) Flowtracekr group.
 *    cb_f       - (IN) Callback function pointer
 *    user_data1 - (IN) user data 1
 *    user_data2 - (IN) user data 2
 *
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_bcm_field_ftfp_entry_traverse(
        int unit,
        bcm_flowtracker_group_t id,
        _bcm_field_ft_entry_traverse_cb cb_f,
        void *user_data1,
        void *user_data2)
{
    int rv = BCM_E_NONE;
    int eidx = 0;
    int entries = 0;
    _field_group_t *fg = NULL;
    _field_control_t *fc = NULL;
    bcm_field_entry_t *entry_ids = NULL;

    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));

    /* Iterate FTFP Groups */
    for (fg = fc->groups; fg != NULL; fg = fg->next) {
        if (fg->stage_id != _BCM_FIELD_STAGE_FLOWTRACKER) {
            continue;
        }

        rv = bcm_esw_field_entry_multi_get(unit, fg->gid, 0, NULL, &entries);

        if (BCM_SUCCESS(rv)) {
            entry_ids = sal_alloc(entries * sizeof (bcm_field_entry_t),
                    "Entry ID array");

            if (entry_ids == NULL) {
                rv = BCM_E_MEMORY;
                break;
            } else {
                rv = bcm_esw_field_entry_multi_get(unit,
                        fg->gid, entries, entry_ids, &entries);

                if (BCM_SUCCESS(rv)) {
                    /* Iterate FTFP Entries */
                    for (eidx = 0; eidx < entries; eidx++) {
                        /* Call callback */
                        rv = cb_f(unit, entry_ids[eidx],
                                id, user_data1, user_data2);
                        if (BCM_FAILURE(rv)) {
                            break;
                        }
                    }
                }
                sal_free(entry_ids);
                entry_ids = NULL;
            }
        }
    }

    return (rv);
}

/*
 * Function:
 *    _bcm_field_ft_qual_config_get
 *
 * Purpose:
 *   Get Qualifier config info from Qual Cfg Info Db.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    qual_cfg_info_db - (IN) Qualifier cfg info db
 *    qual       - (IN) Qualifier
 *    offset     - (OUT) Offset of Qualifier in L0 Bus
 *    Width      - (OUT) Width of Qualifier in L0 Bus
 *
 * Returns:
 *    BCM_E_INTERNAL    - Incoreect Input
 *    BCM_E_NONE        - Success.
 * Notes:
 *    This function is only valid if all offets of qualifier
 *    are consecutive and only one instance.
 */
STATIC int
_bcm_field_ft_qual_config_get(int unit,
                              bcmi_keygen_qual_cfg_info_db_t *db,
                              bcm_field_qualify_t qual,
                              uint32 *offset,
                              uint32 *width)
{
    uint8 num_chunks;
    uint8 chunk = 0;
    uint16 temp_width = 0;
    uint16 temp_offset = 0;
    bcmi_keygen_qual_offset_cfg_t *e_params = NULL;
    bcmi_keygen_qual_cfg_t *qual_cfg_arr = NULL;
    bcmi_keygen_qual_cfg_info_t *qual_cfg_info = NULL;

    if (db == NULL) {
        return BCM_E_INTERNAL;
    }

    qual_cfg_info = db->qual_cfg_info[qual];
    qual_cfg_arr = qual_cfg_info->qual_cfg_arr;

    if (qual_cfg_arr == NULL) {
        return BCM_E_INTERNAL;
    }

    num_chunks = qual_cfg_arr->num_chunks;

    for (chunk = 0; chunk < num_chunks; chunk++) {
        e_params = &qual_cfg_arr->e_params[chunk];

        if (chunk == 0) {
            temp_offset = e_params->offset;
        }

        temp_width += e_params->width;
    }

    *offset = temp_offset;
    *width = temp_width;

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_hx5_ft_normalize_format_set
 *
 * Purpose:
 *   Set normalization format for given qualifier-pair.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    index      - (IN) Index in norm_qualifiers.
 *    norm_qualifier - (IN) Normalization qualifiers list.
 *    max_norm_commands - (IN) Maximum norm commands that can be
 *                      configured in hw memory.
 *    norm_cmds_buf - (IN/OUT) Buffer for norm commands format.
 *    valid_commands - (IN/OUT) valid commands in norm_cmds_bufs.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_hx5_ft_normalize_format_set(int unit,
        bcm_flowtracker_tracking_param_info_t **norm_tracking_params_info,
        int max_norm_commands,
        uint32 *norm_cmds_buf,
        int *valid_commands)
{
    uint32 comm_idx = 0;
    uint32 src_offset = 0;
    uint32 dst_offset = 0;
    uint32 src_width = 0;
    uint32 dst_width = 0;
    bcm_field_qualify_t src_qual;
    bcm_field_qualify_t dst_qual;
    _field_stage_t *stage_fc = NULL;
    bcm_flowtracker_tracking_param_type_t param;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;
    bcmi_keygen_qual_cfg_info_db_t *qual_cfg_info_db = NULL;

    if (*valid_commands >= max_norm_commands) {
      LOG_ERROR(BSL_LS_BCM_FP,
                (BSL_META_U(unit, "All Keys in tracking params"
                            " of flowtracker group cannot be"
                            " normalized\n\r")));
        return BCM_E_CONFIG;
    }

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit,
                _BCM_FIELD_STAGE_FLOWTRACKER, &stage_fc));

    map_db = stage_fc->ft_info->param_qual_map_db;
    qual_cfg_info_db = stage_fc->ft_info->flow_key_qual_cfg;

    /* Get Field Qualifiers */
    param = norm_tracking_params_info[0]->param;
    src_qual = map_db->param_qual_map[param]->qual;
    param = norm_tracking_params_info[1]->param;
    dst_qual = map_db->param_qual_map[param]->qual;

    BCM_IF_ERROR_RETURN(_bcm_field_ft_qual_config_get(unit,
                qual_cfg_info_db, src_qual, &src_offset, &src_width));
    BCM_IF_ERROR_RETURN(_bcm_field_ft_qual_config_get(unit,
                qual_cfg_info_db, dst_qual, &dst_offset, &dst_width));

    /* Validate alignemnt */
    if (((src_offset % 16) != 0) || ((dst_offset % 16) != 0) ||
            ((src_width % 16) != 0) || (dst_width != src_width)  ||
            (src_width == 0) || (dst_width == 0)) {
        return BCM_E_CONFIG;
    }
    src_offset /= 16;
    dst_offset /= 16;
    /* Formula for size is (n + 1)*2B */
    src_width = (src_width - 1) / 16;

    comm_idx = *valid_commands;

    soc_format_field32_set(unit, BSK_NORMALIZE_COMMAND_FORMATfmt,
            &norm_cmds_buf[comm_idx], NORMALIZE_ENABLEf, 1);

    soc_format_field32_set(unit, BSK_NORMALIZE_COMMAND_FORMATfmt,
            &norm_cmds_buf[comm_idx], NORMALIZE_SRC_START_OFFSETf,
            src_offset);

    soc_format_field32_set(unit, BSK_NORMALIZE_COMMAND_FORMATfmt,
            &norm_cmds_buf[comm_idx], NORMALIZE_DST_START_OFFSETf,
            dst_offset);

    soc_format_field32_set(unit, BSK_NORMALIZE_COMMAND_FORMATfmt,
            &norm_cmds_buf[comm_idx], NORMALIZE_SIZEf, src_width);

    *valid_commands = comm_idx + 1;

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_hx5_ft_normalize_controls_set
 *
 * Purpose:
 *    Set normalize controls of FTFP policy table.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    id         - (IN) Flowtracker Group Id
 *    policy_mem - (IN) Policy Memory
 *    enable     - (IN) Normalization Enable/Disable
 *    data_buf   - (IN/OUT) Policy memory buffer
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_field_hx5_ft_normalize_controls_set(
        int unit,
        bcm_flowtracker_group_t id,
        soc_mem_t policy_mem,
        uint32 enable,
        uint32 *data_buf)
{
    int idx = 0;
    int rv = BCM_E_NONE;
    int valid_commands = 0;
    int num_tracking_params = 0;
    uint32 norm_cmds_buf[4] = {0};
    bcm_flowtracker_tracking_param_info_t *tracking_params_info = NULL;
    bcm_flowtracker_tracking_param_info_t
        *norm_tracking_params_info[BCMI_FT_GROUP_NORM_TRACKING_PARAMS] = {NULL};

    /* Normalize Commands */
    soc_field_t normalize_cmds_f[] = {
        NORMALIZE_0_COMMANDf, NORMALIZE_1_COMMANDf,
        NORMALIZE_2_COMMANDf, NORMALIZE_3_COMMANDf
    };

    /* If enable=FALSE, reset Normalize commands */
    if (enable == 0) {
        for (idx = 0; idx < COUNTOF(normalize_cmds_f); idx++) {
            if (SOC_MEM_FIELD_VALID(unit, policy_mem, normalize_cmds_f[idx])) {
                soc_mem_field_set(unit, policy_mem, (void *) data_buf,
                        normalize_cmds_f[idx], &norm_cmds_buf[idx]);
            }
        }
        goto cleanup;
    }

    /* Get number of tracking params from group template */
    rv = bcm_esw_flowtracker_group_tracking_params_get(unit,
            id, 0, NULL, &num_tracking_params);
    BCMI_IF_ERROR_CLEANUP(rv);

    /* Allocate memory for tracking params info */
    _FP_XGS3_ALLOC(tracking_params_info, (num_tracking_params * \
            sizeof(bcm_flowtracker_tracking_param_info_t)),
            "tracking params info");
    if (tracking_params_info == NULL) {
        rv = BCM_E_MEMORY;
        BCMI_IF_ERROR_CLEANUP(rv);
    }

    /* Read tracking params info */
    rv = bcm_esw_flowtracker_group_tracking_params_get(unit,
            id, num_tracking_params, &tracking_params_info[0],
            &num_tracking_params);
    BCMI_IF_ERROR_CLEANUP(rv);

    /* Get tracking params for normalization */
    rv = bcmi_ft_group_norm_tracking_params_info_get(unit,
            id, &norm_tracking_params_info[0]);
    BCMI_IF_ERROR_CLEANUP(rv);

    /* Normalize if both param in the pair are present */
    for (idx = 0; idx < BCMI_FT_GROUP_NORM_TRACKING_PARAMS; idx += 2) {
        if ((norm_tracking_params_info[idx] != NULL) &&
                (norm_tracking_params_info[idx + 1] != NULL)) {
            rv = _field_hx5_ft_normalize_format_set(unit,
                    &norm_tracking_params_info[idx], COUNTOF(normalize_cmds_f),
                    norm_cmds_buf, &valid_commands);
            BCMI_IF_ERROR_CLEANUP(rv);
        }
    }

    /* Set to policy table */
    for (idx = 0; idx < valid_commands; idx++) {
        if (SOC_MEM_FIELD_VALID(unit, policy_mem, normalize_cmds_f[idx])) {
            soc_mem_field_set(unit, policy_mem, (void *) data_buf,
                    normalize_cmds_f[idx], &norm_cmds_buf[idx]);
        }
    }

cleanup:

    if (tracking_params_info != NULL) {
        sal_free(tracking_params_info);
    }

    return rv;
}

/*
 * Function:
 *    _field_ftfp_entry_control_set
 *
 * Purpose:
 *    Apply flowtracker group control to given entry if associated
 *    action is given flowtracker group.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    entry      - (IN) Field entry.
 *    id         - (IN) Flowtracker group Id
 *    user_data1 - (IN) user data 1
 *    user_data2 - (IN) user data 2
 *
 * Returns:
 *    BCM_E_PARAM       - If input parameters are not valid
 *    BCM_E_MEMORY      - If Failed to allocate required memory
 *    BCM_E_NONE        - Success.
 */
STATIC int
_field_ftfp_entry_control_set(
        int unit,
        bcm_field_entry_t entry,
        bcm_flowtracker_group_t id,
        void *user_data1,
        void *user_data2)
{
    int rv = BCM_E_NONE;
    int tcam_idx = 0;
    int param = 0;
    uint32 val = 0;
    uint32 ft_group_id_set = 0;
    uint32 action_ft_set = 0;
    uint32 ent_action_ft = 0;
    uint32 action_new_learn_set = 0;
    uint32 ent_action_new_learn = 0;
    uint32 cur_param = 0;
    _field_action_t *fa = NULL;
    _field_entry_t *f_ent = NULL;
    soc_field_t field = INVALIDf;
    soc_mem_t tcam_mem = INVALIDm;
    soc_mem_t policy_mem = INVALIDm;
    bcm_flowtracker_group_control_type_t control_type;
    bcm_flowtracker_group_t ent_flow_group_id = -1;
    uint32 data_buf[SOC_MAX_MEM_FIELD_WORDS] = {0};

    if ((user_data1 == NULL) || (user_data2 == NULL)) {
        return BCM_E_PARAM;
    }

    control_type = *((bcm_flowtracker_group_control_type_t *) user_data1);
    param = *((int *)user_data2);

    FP_LOCK(unit);

    rv = _field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent);
    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(unit);
        return (rv);
    }

    /* Loop through all actions */
    for (fa = f_ent->actions;
            (fa != NULL) && (fa->flags & _FP_ACTION_VALID);
            fa = fa->next) {
        switch(fa->action) {
            case bcmFieldActionFlowtrackerGroupId:
                if (!(fa->flags & _FP_ACTION_DIRTY)) {
                    ft_group_id_set = TRUE;
                    ent_flow_group_id = fa->param[0];
                }
                break;
            case bcmFieldActionFlowtrackerEnable:
                if (!(fa->flags & _FP_ACTION_DIRTY)) {
                    action_ft_set = TRUE;
                    ent_action_ft = fa->param[0];
                }
                break;
            case bcmFieldActionFlowtrackerNewLearnEnable:
                if (!(fa->flags & _FP_ACTION_DIRTY)) {
                    action_new_learn_set = 1;
                    ent_action_new_learn = fa->param[0];
                }
                break;
            default:
                break;
        }
    }

    if ((ft_group_id_set == FALSE) || (ent_flow_group_id != id)) {
        FP_UNLOCK(unit);
        return rv;
    }

    /* Get Policy Index */
    rv = _bcm_field_entry_tcam_idx_get(unit, f_ent, &tcam_idx);
    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(unit);
        return rv;
    }

    /* Get Policy Mem */
    rv = _bcm_field_th_tcam_policy_mem_get(unit,
            f_ent, &tcam_mem, &policy_mem);
    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(unit);
        return rv;
    }

    rv = soc_mem_read(unit, policy_mem, MEM_BLOCK_ANY,
            tcam_idx, data_buf);
    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(unit);
        return rv;
    }

    /* Supported ft_group Controls are DO_NOT_FT and LEARN_DISABLE */
    switch(control_type) {
        case bcmFlowtrackerGroupControlFlowtrackerEnable:
            field = DO_NOT_FTf;
            /* if Field Action is present, it takes priority. */
            if (action_ft_set == TRUE) {
                val = (ent_action_ft) ? 0 : 1;
            } else {
                val = (param) ? 0 : 1;
            }
            break;
        case bcmFlowtrackerGroupControlNewLearnEnable:
            field = LEARN_DISABLEf;
            /* if Field Action is present, it takes priority. */
            if (action_new_learn_set == TRUE) {
                val = (ent_action_new_learn) ? 0 : 1;
            } else {
                val = (param) ? 0 : 1;
            }
            break;
        case bcmFlowtrackerGroupControlFlowDirection:
            field = BIDIRECTIONAL_FLOWf;
            val= ((param == bcmFlowtrackerFlowDirectionBidirectional) ? 1 : 0);

            rv = _field_hx5_ft_normalize_controls_set(unit,
                    id, policy_mem, val, data_buf);
            break;
        default:
            rv = BCM_E_PARAM;
            break;
    }

    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(unit);
        return rv;
    }

    if (SOC_MEM_FIELD_VALID(unit, policy_mem, field)) {
        cur_param = soc_mem_field32_get(unit,
                policy_mem, data_buf, field);

        if (cur_param != val) {
            soc_mem_field32_set(unit,
                    policy_mem, data_buf, field, val);

            rv = soc_mem_write(unit, policy_mem,
                    MEM_BLOCK_ALL, tcam_idx, data_buf);
        }
    }

    FP_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *    _field_ftfp_entry_action_stale_set
 *
 * Purpose:
 *    Mark flowtracker group id action in ftfp entries as
 *    staled out.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    entry      - (IN) Field entry.
 *    id         - (IN) Flowtracker group Id
 *    user_data1 - (IN) user data 1
 *    user_data2 - (IN) user data 2
 *
 * Returns:
 *    BCM_E_PARAM       - If input parameters are not valid
 *    BCM_E_NONE        - Success.
 *    BCM_E_XXX
 */
STATIC int
_field_ftfp_entry_action_stale_set(
        int unit,
        bcm_field_entry_t entry,
        bcm_flowtracker_group_t id,
        void *user_data1,
        void *user_data2)
{
    int rv = BCM_E_NONE;
    _field_entry_t *f_ent = NULL;

    FP_LOCK(unit);

    rv = _field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent);
    if (BCM_FAILURE(rv)) {
        FP_UNLOCK(unit);
        return (rv);
    }

    rv = _bcm_field_hx5_ft_group_hw_free(unit, f_ent,
            _FP_ACTION_HW_FREE | _BCM_FIELD_ACTION_REF_STALE);

    FP_UNLOCK(unit);

    return rv;
}

/* Public Interfaces */

/*
 * Function:
 *    _bcm_field_ft_alu_data_config_get
 *
 * Purpose:
 *    Get ALU Data config info
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    id         - (IN) flowtracker group id.
 *    alu_data   - (IN) Alu Data param
 *    offset     - (OUT) Offset in ALU Data vector.
 *    width      - (OUT) Number of bits in ALU Data vector.
 *
 * Returns:
 *    BCM_E_NOT_FOUND - If given alu_data is not found.
 *    BCM_E_NONE
 */
int
_bcm_field_ft_alu_data_config_get(
        int unit,
        bcm_flowtracker_group_t id,
        _bcm_field_ft_alu_data_t alu_data,
        int *offset,
        int *width)
{
    int rv = BCM_E_NONE;
    int index = 0;
    int num_alu_data = 0;
    uint8 found = FALSE;
    uint16 param = 0;
    _field_ft_tracking_alu_data_info_t *alu_data_info = NULL;

    alu_data_info = &_field_ft_tracking_alu_data_info[0];
    num_alu_data = COUNTOF(_field_ft_tracking_alu_data_info);

    for (index = 0; index < num_alu_data; index++) {
        param = alu_data_info[index].param;
        if (param == alu_data) {
            found = TRUE;
            if (offset != NULL) {
                *offset = alu_data_info[index].offset;
            }

            if (width != NULL) {
                *width = alu_data_info[index].width;
            }
        }
    }

    if (found == FALSE) {
        rv = BCM_E_NOT_FOUND;
    }

    return rv;
}

/*
 * Function:
 *    _bcm_field_ft_group_tracking_info_process
 *
 * Purpose:
 *    Process FT group tracking information and update FT group
 *    database, if required
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    id         - (IN) flowtracker group id.
 *    save_output- (IN) TRUE if output is required to be saved to
 *                 FT Group DB.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_ft_group_tracking_info_process(
        int unit,
        bcm_flowtracker_group_t id,
        uint8 save_output)
{
    _field_ft_group_keygen_md_t ftg_keygen_md;

    sal_memset(&ftg_keygen_md, 0, sizeof(_field_ft_group_keygen_md_t));

    ftg_keygen_md.state_rv = BCM_E_NONE;
    ftg_keygen_md.ft_group_id = id;
    ftg_keygen_md.state = _FIELD_FT_GROUP_KEYGEN_START;
    ftg_keygen_md.prev_state = _FIELD_FT_GROUP_KEYGEN_START;
    ftg_keygen_md.save_op = save_output;

    return _field_hx5_ftg_keygen_process(unit, &ftg_keygen_md);
}

/*
 * Function:
 *    _bcm_field_ft_group_extraction_clear
 *
 * Purpose:
 *    Cleanup FT group tracking params extraction information
 *    from FT group database
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    id         - (IN) flowtracker group id.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_ft_group_extraction_clear(
        int unit,
        bcm_flowtracker_group_t id)
{
    int rv = BCM_E_NONE;

    /* Clear session key params extraction info */
    rv = bcmi_esw_ft_group_extraction_alu_info_set(unit,
                id, 1, 0, NULL);
    BCM_IF_ERROR_RETURN(rv);

    /* Clear session key hw extractors info */
    rv = bcmi_esw_ft_group_extraction_hw_info_set(unit,
                id, 1, 0, NULL);
    BCM_IF_ERROR_RETURN(rv);

    /* Clear session data params extraction info */
    rv = bcmi_esw_ft_group_extraction_alu_info_set(unit,
                id, 0, 0, NULL);
    BCM_IF_ERROR_RETURN(rv);

    /* Clear session data hw extractors info */
    rv = bcmi_esw_ft_group_extraction_hw_info_set(unit,
                id, 0, 0, NULL);

    return rv;
}

/*
 * Function:
 *    _bcm_field_ft_group_control_set
 *
 * Purpose:
 *    Apply flowtracker group controls to each entry in which
 *    action is given flowtracker group..
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    id         - (IN) Flowtracker group
 *    control_type- (IN) control to apply
 *    param      - (IN) Parameter associated with control
 *
 * Returns:
 *    BCM_E_PARAM       - Null field stage control structure.
 *    BCM_E_NONE        - Success.
 */
int
_bcm_field_ft_group_control_set(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_group_control_type_t control_type,
        int param)
{
    return _bcm_field_ftfp_entry_traverse(unit, id,
            _field_ftfp_entry_control_set,
            (void *) &control_type, (void *) &param);
}


/*
 * Function:
 *    _bcm_field_ft_group_invalidate
 *
 * Purpose:
 *    Mark flowtracker group Id action in ftfp entries
 *    stale.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_ft_group_invalidate(int unit)
{
    return _bcm_field_ftfp_entry_traverse(unit, -1,
            _field_ftfp_entry_action_stale_set,
            NULL, NULL);
}

/*
 * Function:
 *    _bcm_field_ft_tracking_param_range_get
 *
 * Purpose:
 *    Returns minimum and maximum values of tracking params.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    param      - (IN) Tracking param.
 *    min_value  - (OUT) Minimum value.
 *    max_value   - (OUT) Maximum value.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_ft_tracking_param_range_get(
        int unit,
        bcm_flowtracker_tracking_param_type_t param,
        uint32 *min_value,
        uint32 *max_value)
{
    int rv = BCM_E_NONE;
    uint16 max_bits;
    bcmi_keygen_cfg_t keygen_cfg;
    _field_stage_t *stage_fc = NULL;
    _field_ft_info_t *ft_info = NULL;
    _field_ft_tracking_param_qual_map_t *pq_map = NULL;
    _field_ft_tracking_param_qual_map_db_t *map_db = NULL;

    if (param >= _FIELD_FT_TRACKING_PARAM_MAX) {
        return BCM_E_PARAM;
    }

    rv = _field_stage_control_get(unit,
            _BCM_FIELD_STAGE_FLOWTRACKER, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    ft_info = stage_fc->ft_info;
    map_db = ft_info->param_qual_map_db;
    pq_map = map_db->param_qual_map[param];
    if (pq_map == NULL) {
        return BCM_E_PARAM;
    }
    if (pq_map->flags & _FP_FT_TRACKING_PARAM_F_NO_QUAL_MAPPED) {
        return BCM_E_PARAM;
    }

    /* Get number of bits required to represent qualifier. */
    keygen_cfg.qual_cfg_info_db = ft_info->flow_key_qual_cfg;
    rv = bcmi_keygen_qual_cfg_max_size_get(unit,
            &keygen_cfg, pq_map->qual, &max_bits);
    BCM_IF_ERROR_RETURN(rv);

    *min_value = 0;
    if (max_bits == 32) {
        *max_value = 0xFFFFFFFF;
    } else {
        *max_value = (1 << max_bits) - 1;
    }

    return rv;
}

/*
 * Function:
 *    _bcm_field_ft_session_profiles_install
 *
 * Purpose:
 *    Install session profiles for ftfp policy
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    id         - (IN) Flowtracker Group Id.
 *    profiles   - (OUT) Session Profiles.
 *
 * Returns:
 *    BCM_E_XXX
 */
int _bcm_field_ft_session_profiles_install(
        int unit,
        bcm_flowtracker_group_t id)
{
    int rv = BCM_E_NONE;
    bcmi_ft_session_profiles_t profiles;

    sal_memset(&profiles, 0, sizeof (bcmi_ft_session_profiles_t));
    rv = _field_hx5_ft_session_profiles_install(unit, id, &profiles);

    if (BCM_SUCCESS(rv)) {
        rv = bcmi_ft_group_ftfp_profiles_set(unit, id, &profiles);
    }

    if (BCM_SUCCESS(rv)) {
        rv = bcmi_ft_group_session_key_type_set(unit,
                id, profiles.session_key_profile_idx - 1);
    }

    return rv;
}

/*
 * Function:
 *    _bcm_field_ft_session_profiles_uninstall
 *
 * Purpose:
 *    Un-install session profiles for ftfp policy
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    id         - (IN) Flowtracker Group Id.
 *    profiles   - (IN) Session Profiles.
 *
 * Returns:
 *    BCM_E_XXX
 */
int _bcm_field_ft_session_profiles_uninstall(
        int unit,
        bcm_flowtracker_group_t id)
{
    int rv = BCM_E_NONE;
    bcmi_ft_session_profiles_t profiles;

    sal_memset(&profiles, 0, sizeof (bcmi_ft_session_profiles_t));

    rv = bcmi_ft_group_ftfp_profiles_get(unit, id, &profiles);
    BCM_IF_ERROR_RETURN(rv);

    return _field_hx5_ft_session_profiles_remove(unit, id, &profiles);
}

/*
 * Function:
 *    _bcm_field_ft_session_profiles_recover
 *
 * Purpose:
 *    Recover session profiles indices.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    id         - (IN) Flowtracker Group Id.
 *
 * Returns:
 *    BCM_E_XXX
 */
int
 _bcm_field_ft_session_profiles_recover(
        int unit,
        bcm_flowtracker_group_t id)
{
    bcmi_ft_session_profiles_t profiles;

    BCM_IF_ERROR_RETURN(bcmi_ft_group_ftfp_profiles_get(unit,
            id, &profiles));

    BCM_IF_ERROR_RETURN(_field_hx5_ft_session_profiles_reference(
            unit, id, &profiles));

    BCM_IF_ERROR_RETURN(bcmi_ft_group_session_key_type_set(unit,
                id, profiles.session_key_profile_idx - 1));

    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_field_ft_group_checkbmp_get
 *
 * Purpose:
 *    Get flowtracker check in ALU16 bitmap.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    id         - (IN) Flowtracker Group Id.
 *    num_checks - (IN) Number of flowchecks.
 *    check_list - (IN) List of Flowchecks
 *    check_bmp_data - (OUT) Data bitmap
 *    check_bmp_mask - (OUT) Mask bitmap.
 *
 * Returns:
 *    BCM_E_XXX
 */
int _bcm_field_ft_group_checkbmp_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 num_checks,
        bcm_field_flowtrackercheck_t *check_list,
        uint32 *check_bmp_data,
        uint32 *check_bmp_mask)
{
    int rv = BCM_E_NONE;
    int bit = 0;
    int iter = 0;
    int flags = 0;
    uint32 found = 0;
    uint32 value = 0;
    bcm_flowtracker_check_t check_id;
    bcm_flowtracker_check_t check_map[8] = {0};

    rv = bcmi_ft_group_is_invalid(unit, id);
    BCM_IF_ERROR_RETURN(rv);

    /* Get List of Flowcheck for flowtracker group */
    rv = bcmi_ft_alu_check16_mapping_get(unit, id, check_map);
    BCM_IF_ERROR_RETURN(rv);

    flags = BCMI_FT_GROUP_CHECK_MATCH_PRIMARY;

    for (iter = 0; iter < num_checks; iter++) {
        check_id = check_list[iter].flow_check_id;
        value = check_list[iter].value;

        rv = bcmi_ft_group_check_validate(unit,
                flags, id, check_id);
        BCM_IF_ERROR_RETURN(rv);

        for (bit = 0; bit < 8; bit++) {
            if (check_map[bit] == check_id) {
                *check_bmp_mask |= (1 << bit);
                if (value == TRUE) {
                    *check_bmp_data |= (1 << bit);
                }
                found++;
                break;
            }
        }
    }

    if (found != num_checks) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *    _bcm_field_ft_group_checklist_get
 *
 * Purpose:
 *    Get list of flowtracker check from ALU16 bitmap.
 *
 * Parameters:
 *    unit       - (IN) BCM device number.
 *    id         - (IN) Flowtracker Group Id.
 *    check_bmp_data - (IN) Data bitmap.
 *    check_bmp_mask - (IN) Mask bitmap.
 *    num_checks - (IN) Number of flowchecks.
 *    check_list - (OUT) List of Flowchecks.
 *    actual_num_checks - (OUT) actual number of flowchecks.
 *
 * Returns:
 *    BCM_E_XXX
 */
int _bcm_field_ft_group_checklist_get(
        int unit,
        bcm_flowtracker_group_t id,
        uint32 check_bmp_data,
        uint32 check_bmp_mask,
        uint32 num_checks,
        bcm_field_flowtrackercheck_t *check_list,
        uint32 *actual_num_checks)
{
    int rv = BCM_E_NONE;
    int i = 0, check_idx = 0;
    bcm_flowtracker_check_t check_map[8] = {0};

    if (actual_num_checks == NULL) {
        return BCM_E_PARAM;
    }

    rv = bcmi_ft_group_is_invalid(unit, id);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcmi_ft_alu_check16_mapping_get(unit, id, check_map);
    BCM_IF_ERROR_RETURN(rv);

    if (check_list == NULL) {
        *actual_num_checks = _shr_popcount(check_bmp_mask);
        return BCM_E_NONE;
    }

    for (i = 0; (i < 8) && (check_idx < num_checks); i++) {

        if ((check_bmp_data) & (1 << i)) {
            check_list[check_idx].value = 1;
        }
        if ((check_bmp_mask) & (1 << i)) {
            check_list[check_idx].flow_check_id = check_map[i];
            check_idx++;

            if (check_idx >= num_checks) {
                break;
            }
        }
    }

    *actual_num_checks = check_idx;

    return BCM_E_NONE;
}

#endif /* BCM_FLOWTRACKER_SUPPORT */
